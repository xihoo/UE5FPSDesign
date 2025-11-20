
  
## 1. 设计目标与约束  
  
### 1.1 总体目标  
  
为 UE5 高品质 FPS 项目设计一套可挂载在任意 `AActor` 上的“词条组件”系统，用于实现类似 affix / modifier / perk 的词条玩法，包括但不限于：  
  
- 武器随机词条（例如 +15% 暴击率、射速提升、后坐力降低）  
- 角色被动能力（例如 移动中精度惩罚降低、受伤回血、爆头触发特殊效果）  
- 装备/配件/道具类词条（例如 “快速装填弹匣”“战术手电提升可见性但暴露位置”）  
  
其核心特性：  
  
- 深度集成 GAS：  
  - 通过 `GameplayEffect` 精确修改 `Attribute`（射速、后坐力、弹匣容量、生命/护盾等）  
  - 通过授予/移除 `UGameplayAbility` 提供被动/主动能力  
  - 使用 `GameplayTag` 作为词条类别、条件、冲突与触发规则的统一描述语言  
- 网络联机友好：  
  - 服务端权威随机生成和管理词条  
  - 客户端通过复制的“精简词条信息”（ID/Seed/Stack 等）进行展示与本地缓存  
- 高拓展性：  
  - 允许设计师主要通过 `PrimaryDataAsset`/`DataTable`/`GameplayTag` 配置新词条  
  - 词条池和组合规则可按武器类型、角色职业、关卡、Roguelite 层数等扩展  
  
### 1.2 约束与设计原则  
  
- 面向现有角色/武器架构    
- 玩家角色：`AFPSPlayerCharacter`（或文档中称 `ADFPSPlayerCharacter`，下文统一称 `AFPSPlayerCharacter`）    
- 射击层：`UFPSShooterPawnComponent`、`UFPSFirearmComponent`、`AFPSWeapon` 及其配件组件    
- GAS 层：已有 AttributeSet、Ability、Effect、Tag 体系（参考工程中已有 FPS/GAS 设计文档）  
- 高品质 FPS 体验优先：  
  - 词条不得破坏基础手感（如极端后坐力变为 0 造成失衡）  
  - 网络延迟下行为确定且可预测（客户端预测中，词条影响需可本地缓存和快速查询）  
- 数据驱动：  
  - 核心逻辑：C++ + GAS；配置层：`UPrimaryDataAsset` + `UDataTable` + `FGameplayTag`  
- 易调试：  
  - 提供调试 UI/控制台命令查看当前 Actor 全部词条及其实际 GAS 效果  
  - 支持“强制重掷”/“指定词条注入”等调试能力（仅开发环境）  
  
---  
  
## 2. 系统整体架构  
  
### 2.1 模块划分  
  
建议在项目中新增/扩展如下模块（命名可按现有工程风格调整）：  
  
- `DFPSAffixSystem`（逻辑模块）  
  - 词条组件：`UDFPSEntryComponent`（暂定名，可叫 `UDFPSAffixComponent`）  
  - 词条实例模型：`FDFPSEntryInstance`  
  - 词条配置数据：`UDFPSEntryDefinition`, `UDFPSEntryPoolDefinition`  
  - 生成器与规则引擎：`UDFPSEntryGenerator`, `UDFPSEntryRuleLibrary`  
  - 网络/存档辅助：`FDFPSEntryRepData`, `FDFPSEntrySaveData`  
- 集成点：  
  - 角色：`ADFPSPlayerCharacter` / `UDFPSShooterPawnComponent`  
  - 武器：`ADFPSWeapon` / `UDFPSAttachmentManagerComponent`  
  - 装备：将来可加入 `ADFPSItem`, `ADFPSArmor` 等  
  
### 2.2 主要类与组件关系（示意）  
  
- `UDFPSEntryComponent : UActorComponent`  
  - 挂在任意 `AActor` 上，负责：  
    - 持有当前生效的 `FDFPSEntryInstance` 列表  
    - 根据配置（池/规则/Seed）生成词条  
    - 将词条实例映射为 GAS 的 `UGameplayEffect`/`UGameplayAbility`/Tag 并应用/撤销  
    - 处理网络复制与 UI 查询  
- `UDFPSEntryDefinition : UPrimaryDataAsset`  
  - 描述一个“词条原型”（静态配置）  
- `UDFPSEntryPoolDefinition : UPrimaryDataAsset`  
  - 描述一个“词条池”，指定可被 Roll 的词条集合与权重规则  
- `FDFPSEntryInstance`  
  - 单个已实例化词条（运行时对象）：指向 `UDFPSEntryDefinition` + Roll 出来的参数 + 当前状态（层数等）  
- `UDFPSEntryGenerator`  
  - 静态/BlueprintCallable 的工具类，根据池与规则创建 `FDFPSEntryInstance` 数组  
- `UDFPSEntryRuleLibrary`  
  - 约束检查、冲突解决、过滤逻辑（只纯函数、不持数据，方便复用与测试）  
  
### 2.3 典型交互流程（以武器生成为例）  
  
1. 关卡中生成一把 `AFPSWeapon`  
2. 服务端在 `BeginPlay` 或生成逻辑中：  
   - 查找与该武器类型匹配的 `UDFPSEntryPoolDefinition`（如：AR 主武器池）  
   - 调用 `UDFPSEntryGenerator::GenerateEntries(WeaponPool, Seed, Context)`    
   - 生成若干 `FDFPSEntryInstance`，存入 `Weapon->UDFPSEntryComponent`  
3. `UDFPSEntryComponent`：  
   - 将每个词条翻译成对应的 `GameplayEffect` 应用到武器/角色（根据定义）  
   - 若词条授予 Ability，则通过已有 GAS 接口授予玩家或武器能力系统组件  
4. 网络：  
   - 服务端将生成结果压缩为 `FFPSEntryRepData`，通过 `ReplicatedUsing` 复制到客户端  
   - 客户端根据 ID 查本地 DataAsset 缓存，构建用于 UI 展示的词条描述  
5. UI：  
   - 装备详情面板/武器预览/词条提示 UI 从 `UDFPSEntryComponent` 查询当前实例列表并展示  
  
---  
  
## 3. 词条模型设计  
  
### 3.1 词条类型分类  
  
通过 `FGameplayTag` 标记类型，并在 DataAsset 中用枚举+Tag 双重表达：  
  
1. 数值类（Attribute Modifier）  
   - 作用：调整现有或新增的 `Attribute`，例如：  
     - 武器射速：`Weapon.FireRate`  
     - 准星扩散：`Weapon.Spread`  
     - 后坐力倍率：`Weapon.RecoilMultiplier`  
     - 角色移动速度：`Character.MoveSpeed`  
   - 表达形式：绑定到 `GameplayEffect` 模板，或直接定义 Attribute 修改参数  
2. 能力类（Ability Grant）  
   - 作用：授予或移除 `UGameplayAbility`  
   - 示例：战术冲刺、快速换弹、爆头治疗、弹药回收  
   - 表达形式：存储 `TSubclassOf<UGameplayAbility>` 或 DataAsset ID  
3. 触发类（Trigger / Proc）  
   - 作用：在特定事件/条件下触发一次性或持续效果  
   - 触发条件（典型用 GameplayTag + Chance 表达）：  
     - 命中敌人（Tag: `Event.Hit.Confirmed`）  
     - 爆头（Tag: `Event.Hit.Headshot`）  
     - 击杀（Tag: `Event.Kill`）  
     - 受击/受伤（Tag: `Event.DamageTaken`）  
   - 效果可以是：  
     - 触发 `GameplayCue`  
     - 应用一个 `GameplayEffect`（如短时间增伤）  
4. 标签类（Tag-Only / Rule Tag）  
   - 不直接修改数值，而是提供语义标签，用于：  
     - 其它系统条件判断（例如 “仅近战武器生效”）  
     - 冲突/排斥约束（“不能与 Tag X 共存”）  
     - UI 分组、过滤表现（如传说词条、职业限定）  
  
### 3.2 与 GAS 的映射关系  
  
在 `UDFPSEntryDefinition` 中抽象以下映射结构：  
  
- AttributeMod（属性修改）  
  - 若干 `FDFPSEntryAttributeMod`：  
    - `FGameplayAttribute Attribute`  
    - `EGameplayModOp::Type Operation`（Add、Multiply、Override）  
    - `float MagnitudeBase` （基础数值）  
    - `float MagnitudePerLevel`（随词条等级成长）  
    - 可选 Curve/Scaling Tag（支持随玩家等级/关卡深度曲线调整）  
  - 通常映射为一个 `UGameplayEffect` 的 Modifiers 条目：  
    - 方案 A：每个词条定义自己的 `UGameplayEffect` 类/Asset，并在词条中仅引用该 GE  
    - 方案 B：词条定义 AttributeMod，运行时构建/聚合到一个统一的“词条 GE”  
      - 性能与复杂度权衡：建议先采用 方案 A（简单、现有工具兼容）  
- GameplayEffect 应用：  
  - `FDFPSEntryGameplayEffectInfo`：  
    - `TSubclassOf<UGameplayEffect> EffectClass`  
    - `EDFSPSEffectTargetType`（Self = 宿主 Actor，Owner = 拥有者，如角色，Instigator = 触发者等）  
    - `bool bPersistent`（是否在词条存在期间持续挂载）  
    - `int32 StackCount` / `StackPolicy`  
- Ability Grant：  
  - `FDFPSEntryGrantedAbility`：  
    - `TSubclassOf<UGameplayAbility> AbilityClass`  
    - `int32 Level`  
    - `FGameplayTagContainer AbilityTags`（筛选/查找用）  
- Trigger：  
  - `FDFPSEntryTriggerDefinition`：  
    - `FGameplayTagContainer TriggerEventTags`（监听的事件标签）  
    - `float ProcChance`  
    - `TSubclassOf<UGameplayEffect> EffectOnProc` 或 `TSubclassOf<UGameplayAbility> AbilityOnProc`  
    - `FGameplayTag CooldownTag` / `float CooldownDuration`（限制触发频率）  
  
### 3.3 词条数据结构（DataAsset / DataTable / GameplayTag）  
  
- `UDFPSEntryDefinition : UPrimaryDataAsset`  
  - 核心字段示意（伪结构，仅描述）：  
    - `FName EntryId`（全局唯一 ID）  
    - `FText DisplayName`  
    - `FText DescriptionFormat`（带占位符，用于数值插值）  
    - `FGameplayTagContainer EntryTags`（类别、武器类型、职业等）  
    - `EDFPSEntryType`（Numeric / Ability / Trigger / TagOnly / Mixed）  
    - `EDFPSEntryRarity`（Common/Rare/Epic/Legendary 等）  
    - `int32 MinRollLevel / MaxRollLevel`（用于池过滤）  
    - `TArray<FDFPSEntryAttributeMod> AttributeMods`  
    - `TArray<FDFPSEntryGameplayEffectInfo> PersistentEffects`  
    - `TArray<FDFPSEntryGrantedAbility> GrantedAbilities`  
    - `TArray<FDFPSEntryTriggerDefinition> Triggers`  
    - `FDFPSEntryConflictRules ConflictRules`  
- `DataTable` 可作为补充：  
  - 若有大量同质化数值词条，可使用 `UDataTable` 管理数值行，DataAsset 中只引用 RowName  
- `GameplayTag` 使用：  
  - `Tag.Entry.Type.Numeric / Ability / Trigger / TagOnly`  
  - `Tag.Entry.Rarity.Common / Rare / ...`  
  - `Tag.Entry.WeaponType.AR / SMG / Sniper`  
  - `Tag.Entry.Class.Assault / Support / ...`  
  - `Tag.Entry.Conflict.*` 用于排斥定义  
  
### 3.4 品质、稀有度、分组、冲突与排斥  
  
- 品质/稀有度（Rarity）  
  - 提供 `EEntryRarity` 枚举 + 对应 `FGameplayTag`  
  - 与生成池/权重强相关（越高稀有度，基础权重越低）  
- 分组（Group / Category）  
  - 使用 Tag 表达：  
    - `Entry.WeaponHandling`, `Entry.Damage`, `Entry.Utility`, `Entry.Defense`  
  - 支持在池配置中指定“一把武器最多 X 个Handling 组词条”等规则  
- 冲突与排斥（Conflict）  
  - 在 `FEntryConflictRules` 中表达：  
    - `FGameplayTagContainer MutuallyExclusiveWithTags`（如：重装弹与分裂弹互斥）  
    - `FGameplayTagContainer RequiresTags`（前置条件）  
    - `FGameplayTagContainer ForbiddenOnOwnerTags`（某职业/武器禁用）  
  - 生成时做静态检查，运行期若通过调试接口强制加入冲突词条，则：  
    - 方案 A：新词条拒绝添加  
    - 方案 B：自动移除已存在的冲突词条（可用于“覆盖型”词条）  
  
---  
  
## 4. 生成与组合规则  
  
### 4.1 词条池配置方式  
  
使用 `UDFPSEntryPoolDefinition : UPrimaryDataAsset` 来描述一个池：  
  
- 关键字段示意：  
  - `FName PoolId`  
  - `FText DisplayName`  
  - `FGameplayTagContainer PoolTags`（如 `Pool.Weapon.AR.Starter`）  
  - `int32 MinEntries` / `MaxEntries`（每次 Roll 的词条数量范围）  
  - `TArray<FDFPSEntryPoolCandidate>`  
    - `TSoftObjectPtr<UDFPSEntryDefinition> EntryDef`  
    - `float BaseWeight`  
    - `EDFPSEntryRarity Rarity`  
    - `FGameplayTagQuery RequiredOwnerTags`（如 Owner 是 AR、职业是 Assault）  
    - `FGameplayTagQuery ForbiddenOwnerTags`  
- 池划分建议：  
  - 按武器类型：`AR_Main`, `SMG_Main`, `Sniper_Main`, `Pistol_Sidearm`…  
  - 按角色职业：`Assault_Passives`, `Support_Passives`  
  - 按关卡/难度：`EarlyGame`, `MidGame`, `LateGame`, `Roguelite_DeepFloor`  
  
### 4.2 随机权重与 Roll 流程（伪代码级）  
  
目标：给定宿主 Actor、词条池和一个可选 Seed，产出一组不冲突且满足数量/品质分布约束的词条实例。  
  
伪代码（描述性）：  
  
- 输入：  
  - `OwnerActor`  
  - `UDFPSEntryPoolDefinition* Pool`  
  - `int32 Seed`（可选，用于可重现）  
  - `FFPSEntryGenerationContext Context`  
- 流程（高层）：  
  1. 基于 Seed 初始化 `FRandomStream`  
  2. `Candidates = FilterCandidatesByOwner(Pool.Candidates, OwnerActor, Context)`  
  3. `ResultEntries = []`  
  4. For `RollIndex` from 1 to `RandomInRange(Pool.MinEntries, Pool.MaxEntries)`:  
     - 从 `Candidates` 中按权重随机选一个 `Candidate`  
       - 权重可随当前已抽到的品质分布做动态调整（例如已抽到很多高稀有度则降低其权重）  
     - 生成 `TempEntryInstance`（附带随机等级或数值偏移）  
     - 使用 `UDFPSEntryRuleLibrary::CanAddEntry(ResultEntries, TempEntryInstance, Context)` 检查：  
       - 冲突标签、排斥、数量上限等  
     - 若可添加：  
       - `ResultEntries.Add(TempEntryInstance)`  
       - 调用 `UDFPSEntryRuleLibrary::OnEntryAddedForWeightAdjustment(...)` 更新权重状态  
     - 若不可添加：  
       - 重试若干次（避免死循环），否则提前结束 Roll  
  5. 输出 `ResultEntries`  
  
权重细节建议：  
  
- 基础权重：来自池配置 `BaseWeight`  
- 稀有度修正：例如 Common = 1.0, Rare = 0.5, Epic = 0.2, Legendary = 0.1  
- 上限控制：可对特定 Tag（如 “Legendary”）设置全局上限（比如一件物品最多 1 个）  
  
### 4.3 防止无效/冲突组合的约束检查  
  
通过 `UDFPSEntryRuleLibrary` 提供静态函数：  
  
- `CanAddEntry(const TArray<FDFPSEntryInstance>& ExistingEntries, const FDFPSEntryInstance& NewEntry, const FDFPSEntryGenerationContext& Context)`  
- `IsCombinationValid(const TArray<FDFPSEntryInstance>& Entries, const FDFPSEntryGenerationContext& Context)`  
  
---  
  
## 5. 与 Actor / 组件集成方案  
  
> 本节在原有基础上补充了与 Inventory / 装备状态联动的设计：词条的实际应用/撤销由 Inventory 和装备状态驱动，而不是简单在 `BeginPlay` 永久生效。  
  
### 5.1 挂载位置与生命周期  
  
推荐挂点：  
  
- 角色：  
  - `ADFPSPlayerCharacter` 上添加 `UDFPSEntryComponent`（角色全局被动 + 特权，用于角色自身的“天赋/职业词条”）  
- 武器：  
  - `ADFPSWeapon` 上添加 `UDFPSEntryComponent`（武器专属词条，仅在该武器被装备/激活时生效）  
- 装备/配件：  
  - 将来在 `UDFPSAttachmentManagerComponent` 挂载配件 Actor 时，也可在配件 Actor 上挂载 `UDFPSEntryComponent`  
  - 对于纯数据装备（无独立 Actor），可使用“虚拟词条容器”（仅保存 `FDFPSEntryInstance` 数据），由角色/Inventory 组件负责 Apply/Remove  
  
生命周期与事件：  
  
- 生成时机（仅生成词条，不立即应用）：  
  - 掉落物/箱子内武器：在服务端生成掉落物时，根据关卡/难度进行 Roll，并写入对应武器或物品实例的 `UDFPSEntryComponent` / AttachedEntries  
  - 初始武器：角色生成/入关时（`BeginPlay` 或能力初始化阶段）根据配置 Roll 出词条，但**不立即对角色 ASC 生效**，等待被装备时才 Apply  
  - Roguelite 强化：在“结算界面”或“升级选项界面”确认后，向目标 Actor 或物品实例附加新的 `FFPSEntryInstance`  
- 应用/撤销由 Inventory / 装备驱动：  
  - 当某物品被装备到武器槽或装备槽时：  
    - Inventory 调用该物品对应的 `UDFPSEntryComponent::ApplyEntriesForEquipped()`（或等价接口），将词条映射为 GAS 效果  
  - 当物品被卸下/替换/丢弃时：  
    - Inventory 调用 `UDFPSEntryComponent::RemoveEntriesForUnequipped()` 撤销对应效果  
- 重掷（Reroll）：  
  - 提供接口：  
    - 清除当前词条（撤销所有 GAS 效果，如有正在应用）  
    - 调用生成器重新 Roll 并更新实例数据  
  - 支持带 Seed，方便存档/重现  
- 存档/读档：  
  - 存储 `FFPSEntrySaveData`：  
    - 包含 EntryId / Level / RollSeed / 关键参数  
  - 读档时：  
    - 由 Seed + EntryDef 重建完整效果，避免存储大块浮点数据  
    - 读档后调用 `IsCombinationValid` 校验合法性，并根据当前装备状态决定是否 Apply  
  
### 5.2 将词条转化为实际 GAS 效果并绑定宿主  
  
`UDFPSEntryComponent` 的核心职责之一：  
  
- 提供显式接口供 Inventory / 装备系统调用：  
  - `ApplyEntriesToOwner(const FDFPSEntryApplyContext& Context)`：在“装备/激活”时调用，将当前 `FDFPSEntryInstance` 列表映射为实际 GAS 效果  
  - `RemoveEntriesFromOwner(const FDFPSEntryRemoveContext& Context)`：在“卸下/失效”时调用，撤销所有通过本组件应用的效果  
  - 可选区分：  
    - `ApplyPersistentPassives()` / `RemovePersistentPassives()`（如护甲/被动天赋）  
    - `ApplyActiveWeaponEffects()` / `RemoveActiveWeaponEffects()`（仅当前持握武器时生效的词条）  
  
- 在 `ApplyEntriesToOwner` 内部（或对应 Blueprint 封装）：  
  - 遍历 `FFPSEntryInstance` 列表  
  - 对于每个实例：  
    - 按 `FEntryGameplayEffectInfo` 通过 `UAbilitySystemComponent` 向对应目标（Self/Owner/Instigator）应用 `GameplayEffect`：  
      - 对角色属性类词条：通常应用到角色的 `ASC`  
      - 对武器表现类词条（例如枪口扩散）：  
        - 优先转化为 Attribute，由 `UFPSFirearmComponent`/`UFPSShooterPawnComponent` 读取  
        - 必要时通过自定义数据接口传递给射击组件  
    - 对于 Ability 类词条：  
      - 调用现有授能流程，将能力授予：  
        - 角色 ASC（常见情况）  
        - 或武器专属 ASC（若已有）  
    - 对于 Trigger 类词条：  
      - 向相关 GAS/射击事件系统注册监听（基于 GameplayEvent + Tag）  
  
- 在 `RemoveEntriesFromOwner` 中：  
  - 撤销所有持久 `GameplayEffect`（记录应用句柄，按需移除）  
  - 移除被词条授予的 Ability（可使用 Tag 或记录授予句柄）  
  - 取消所有事件监听  
  
> 要求：`UDFPSEntryComponent` 不应在 `BeginPlay` 时自动 Apply，而是等待 Inventory / 装备系统显式调用，以避免未装备物品错误地对角色产生加成。  
  
### 5.3 与角色死亡/武器切换/掉落拾取的关系  
  
- 角色死亡：  
  - 通常保留词条配置，但暂停部分效果：  
    - 持久型属性词条按常规死亡/复活流程处理（由 ASC 控制）  
    - 触发类可能需要在复活时重置冷却  
- 武器切换与 Inventory 联动：  
  - 当前持握武器槽（如 `Primary`、`Secondary`）由 `UDFPSInventoryComponent` 维护  
  - 当 Active 武器槽改变时：  
    - 对旧槽位武器：  
      - 调用其 `UDFPSEntryComponent::RemoveEntriesFromOwner`（至少移除“持握时生效”的部分）  
    - 对新槽位武器：  
      - 调用其 `UDFPSEntryComponent::ApplyEntriesToOwner`，让与该武器相关的词条立即生效  
- 装备切换：  
  - 当角色在某 `EEquipSlotType` 上更换护甲/装备时：  
    - Inventory 先对旧装备的词条执行 Remove，再对新装备执行 Apply  
- 掉落拾取：  
  - 掉落的 `AFPSWeapon` 自身已持有 `UDFPSEntryComponent` 与已生成好的 `FFPSEnryInstance`  
  - 角色拾取：  
    - 若作为“未装备”武器放入背包：  
      - 保留其词条数据，但**不调用 Apply**，直到玩家真正将其装备到某个武器槽  
    - 若直接替换当前武器栏：  
      - 先对原武器执行 Remove  
      - 再将新武器绑定到该槽位并调用 Apply  
  
---  
  
## 6. 联机与复制  
  
### 6.1 服务端权威生成  
  
- 所有词条生成、应用与变更都在服务端进行  
- 客户端仅作为展示与本地预测（如需）：  
  - 不允许客户端主动更改词条内容  
  - 若有客户端操作引起的词条变更（例如 UI 中花资源重掷），应走 RPC 到服务端再回推  
  
### 6.2 需要复制的数据及优化策略  
  
为减少带宽，建议复制“轻量级词条信息”，而非完整效果数据：  
  
- `FFPSEntryRepData`（示意）：  
  - `FName EntryId`  
  - `int32 Level`  
  - `int32 SeedOrRollIndex`（用于在客户端重算部分展示用随机值，如范围显示）  
  - `uint8 StackCount`  
  - 可选：`uint8 Rarity` （若 UI 需快速使用）  
- `UDFPSEntryComponent` 维护：  
  - 在服务端持有完整 `FFPSEntryInstance`（包含对 `UDFPSEntryDefinition` 的强指针）  
  - 通过 `ReplicatedUsing` 的 `TArray<FFPSEntryRepData>` 同步给客户端  
  - 客户端收到后，通过已加载的 DataAsset 根据 `EntryId` 查找配置，组合出完整的 UI 展示结构  
  
复制策略优化：  
  
- 对于频繁改变的词条（例如临时 Buff 类词条）：  
  - 可以依赖现有 GAS `GameplayEffect` 复制来展示状态，词条系统仅处理“静态/持久型”词条  
- 对于武器/物品：  
  - 词条组合一般在生成时确定，不频繁变化  
  - 可只在创建/重掷/拾取时同步一次全量，平时不再变动  
- 对于存档：  
  - 以同样的简化结构 `FFPSEntrySaveData`（与 RepData 相似）进行存储  
  
---  
  
## 7. UI & UX 设计要点  
  
### 7.1 展示风格定位  
  
- 写实偏科幻、简洁但有层级感的词条 UI：  
  - 颜色区分稀有度（白/绿/蓝/紫/橙）  
  - 使用小图标 + 简短标题 + 数值描述  
  - 支持多行词条，每行的结构统一：`[图标] 标题 + 数值 + 说明`  
- 典型展示位置：  
  - 武器信息面板（查看当前武器详情）  
  - 背包/库存界面  
  - 掉落物/箱子选择界面中的比较视图（当前装备 vs 候选）  
  
### 7.2 Tooltip 与数值解释  
  
- 描述生成策略：  
  - 每个 `UDFPSEntryDefinition` 包含：  
    - `DisplayName`  
    - `DescriptionFormat`（如 “{0}% 增加射速，{1}% 降低后坐力”）  
  - 运行时填入具体数值：  
    - 由 `FFPSEntryInstance` 中预先计算的最终/展示数值（例如 `DisplayMagnitude`）  
- 文字说明与实际效果的一致性：  
  - 数值应基于应用到 Attribute / GE 前的同一来源（防止 UI 与实际不一致）  
  - 对于百分比型数值统一采用同一舍入规则（例如一位小数）  
- 多语言支持：  
  - `DisplayName` 和 `DescriptionFormat` 使用 `FText` 并走项目统一本地化流程  
  - 特殊能力/触发类词条：  
    - 描述中引用标准化的事件文案，如 “爆头时有 {0}% 几率回复 {1} 点护盾”  
  
---  
  
## 8. 可扩展性与二期规划  
  
### 8.1 新词条类型扩展  
  
预留扩展方向：  
  
- 环境/地图词条：  
  - 将 `UDFPSEntryComponent` 挂在 `ALevelScriptActor` 或场景环境 Actor 上  
  - 例如本局“低重力”“高辐射区域”对所有玩家生效  
- Roguelite 进程词条：  
  - 通过关卡/层数膨胀难度与奖励：  
    - 使用 Context 中的 “楼层数/关卡深度” 调整权重与数值  
- 玩法型词条：  
  - 如 “子弹会在墙体反弹一次”“爆头会制造范围爆炸”：  
    - 通常通过授予专用 `UGameplayAbility`/扩展 `UFPSFirearmComponent` 行为实现  
    - 词条系统只负责条件与数值参数注入  
  
### 8.2 扩展机制设计建议  
  
- 对所有核心类型使用枚举 + Tag 双表示，避免后续类型爆炸难以扩展  
- `FFPSEntryInstance` 设计为可扩展结构（保留 `TMap<FName, float>` 的“额外数值字段”空间）  
- 触发系统尽量依赖 GAS GameplayEvent 与 Tag，为未来新玩法提供统一挂载点  
  
---  
  
## 9. 与现有工程目录的集成建议  
  
### 9.1 目录结构建议  
  
在 `Source/dungeonFPS/` 下，参考现有模块划分，增加或对齐类似结构：  
  
- `Source/dungeonFPS/AffixSystem/`  
  - `Components/`  
    - `DFPSEntryComponent.h/.cpp`  
  - `Data/`  
    - `DFPSEntryDefinition.h/.cpp`  
    - `DFPSEntryPoolDefinition.h/.cpp`  
  - `Runtime/`  
    - `DFPSEntryInstance.h/.cpp`  
    - `DFPSEntryGenerator.h/.cpp`  
    - `DFPSEntryRuleLibrary.h/.cpp`  
- Content 目录：  
  - `Content/DungeonFPS/Entries/`  
    - `Definitions/`（各类 DataAsset）  
    - `Pools/`（各武器/职业/关卡池）  
  
### 9.2 命名约定  
  
- 类命名：  
  - `UDFPSEntryComponent`, `UDFPSEntryDefinition`, `UDFPSEntryPoolDefinition`  
  - 若希望更贴近日常术语，可将 Entry 替换为 `Affix` 或 `Modifier`  
  
---  
  
## 10. 函数/类签名草案示例（无具体实现）  
  
仅用于对齐设计，不写任何真正实现代码：  
  
- 组件接口（示意）：  
  - `UDFPSEntryComponent::GenerateFromPool(UDFPSEntryPoolDefinition* Pool, int32 Seed, const FDFPSEntryGenerationContext& Context)`  
  - `UDFPSEntryComponent::ApplyAllEntries()`  
  - `UDFPSEntryComponent::ClearAllEntries()`  
  - `UDFPSEntryComponent::GetActiveEntries(TArray<const FDFPSEntryInstance*>& OutEntries) const`  
- 规则库（示意）：  
  - `UDFPSEntryRuleLibrary::CanAddEntry(const TArray<FDFPSEntryInstance>& Existing, const FDFPSEntryInstance& Candidate, const FDFPSEntryGenerationContext& Context)`  
  - `UDFPSEntryRuleLibrary::IsCombinationValid(const TArray<FDFPSEntryInstance>& Entries, const FDFPSEntryGenerationContext& Context)`  
- UI 查询接口（示意）：  
  - `UFUNCTION(BlueprintCallable) GetEntryDisplayData(FDFPSEntryDisplayData& OutData) const`