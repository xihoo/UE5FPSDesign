
  
## 1. 设计目标与约束  
  
### 1.1 总体目标  
  
为 UE5 高品质 FPS 项目设计一套 Inventory（库存/背包）系统，用于统一管理 `ADFPSPlayerCharacter`（以及潜在的 AI 角色）所拥有的各类物品：  
  
- 武器（主武器、副武器、特殊武器、近战武器等）  
- 装备/护甲/配件（例如头盔、防弹衣、战术配件）  
- 消耗品（血包、护盾包、临时增益道具等）  
- 任务物品/钥匙（仅用于任务逻辑和解锁条件）  
  
并与“词条组件系统”深度集成，实现：  
  
- 物品自身可以通过 `UFPSEntryComponent`（或等价结构）携带词条  
- 仅当物品被**装备/激活**时，才将这些词条映射为 GAS 的实际效果应用到角色或武器  
- 当物品被**卸下/切换/丢弃**时，能够可靠地撤销对应词条效果  
  
### 1.2 约束与设计原则  
  
- 面向高品质 FPS 体验：  
  - 操作必须快速、低认知负担，不允许复杂 MMO 式背包拖拽打断战斗节奏  
  - 装备切换与词条生效/失效需无感或延迟极低，避免切枪卡顿  
- 与现有射击/GAS 架构对齐：  
  - 角色：`ADFPSPlayerCharacter`  
  - 武器与射击层：`ADFPSWeapon`、`UDFPSFirearmComponent`、`UDFPSShooterPawnComponent`、`UDFPSAttachmentManagerComponent`  
  - GAS：现有 `UAttributeSet`、`UGameplayAbility`、`UGameplayEffect`、`FGameplayTag`  
- 与词条系统的协同：  
  - 词条的应用与撤销**由 Inventory/装备状态驱动**，而不是简单在 `BeginPlay` 永久生效  
  - 同一物品离开 Inventory 后不得残留 GAS 效果  
- 联机与性能：  
  - 服务端权威维护 Inventory；客户端只复制必要数据用于展示与本地表现  
  - 使用紧凑的网络数据结构（ID + 数量 + 状态）表示物品  
  
---  
  
## 2. 系统整体架构  
  
### 2.1 核心模块与类  
  
建议在 `Source/dungeonFPS/` 下增加或扩展如下模块（命名可按项目实际调整）：  
  
- `DFPSInventorySystem/`  
  - 组件：  
    - `UDFPSInventoryComponent`：挂在 `ADFPSPlayerCharacter`（以及 AI）上的库存组件  
  - 数据：  
    - `UDFPSItemDefinition`（基础物品配置，`UPrimaryDataAsset`）  
    - 若干子类/Tag 表示不同物品类型，如武器、护甲、消耗品等  
  - 实例：  
    - `FDFPSItemInstance`：运行时物品实例数据（引用 `UDFPSItemDefinition` + 数量 + 状态）  
    - `FDFPSSlot`：某个槽位上的物品引用与状态  
  - 逻辑：  
    - `UDFPSInventoryBlueprintLibrary`：常用操作辅助（拾取、丢弃、切换等）  
  
与已有系统集成点：  
  
- 角色蓝图/类中：  
  - 新增并初始化 `UDFPSInventoryComponent`  
- 武器系统中：  
  - 将当前使用的 `ADFPSWeapon` 视为 Inventory 中某一武器槽位的已装备物品  
- 词条系统中：  
  - 物品（尤其武器、装备）上挂载 `UDFPSEntryComponent`  
  - `UDFPSInventoryComponent` 在装备状态改变时驱动 `UDFPSEntryComponent::ApplyEntriesToOwner/RemoveEntriesFromOwner`  
  
### 2.2 物品数据模型  
  
#### 2.2.1 物品定义（静态配置）  
  
- `UDFPSItemDefinition : UPrimaryDataAsset`  
  - 关键字段示意：  
    - `FName ItemId`（物品唯一 ID）  
    - `FText DisplayName`  
    - `FText Description`  
    - `UTexture2D* Icon`  
    - `EDFPSItemType ItemType`（Weapon / Armor / Attachment / Consumable / Quest / Misc）  
    - `FGameplayTagContainer ItemTags`（如 Weapon.Main, Armor.Body, Consumable.Heal 等）  
    - 容量/重量信息：`float Weight`, `int32 MaxStackCount`  
    - 对于武器类：引用武器类/数据：`TSubclassOf<ADFPSWeapon> WeaponClass`  
    - 对于护甲/装备：对应装备插槽信息：`EDFPSEquipSlotType EquipSlot`  
    - 对于消耗品：使用效果描述（可走单独系统或直接引用 `UGameplayEffect`/`UGameplayAbility`）  
    - 词条配置：  
      - `TSoftObjectPtr<UDFPSEntryPoolDefinition> DefaultEntryPool`  
      - `int32 EntryRollCount`（初始 Roll 数量）  
  
#### 2.2.2 物品实例（运行时）  
  
- `FDFPSItemInstance`（纯数据结构，可复制/可存档）：  
  - `FName ItemId`  
  - `int32 Count`  
  - `FGuid InstanceId`（区分相同物品不同个体，用于唯一绑定词条）  
  - `TArray<FDFPSEntryInstance> AttachedEntries`（可选：若词条结果不希望完全通过 Seed 重建）  
  - 运行时状态：  
    - `bool bIsEquipped`  
    - `EDFPSEquipSlotType EquippedSlot`  
  
> 说明：若使用“物品 Actor + 组件”模式，也可以让 Actor 上挂 `UDFPSEntryComponent`，Inventory 只存轻量引用（如 Actor 指针/ID）。推荐在 FPS 中，**武器用 Actor**，而**护甲和大部分道具用纯数据 + 词条实例**。  
  
#### 2.2.3 槽位（Slot）模型  
  
- `EDFPSWeaponSlotType`：`Primary`, `Secondary`, `Sidearm`, `Melee`, `Special`  
- `EDFPSEquipSlotType`：`Head`, `Body`, `Hands`, `Feet`, `Universal1`, `Universal2` ...（可根据项目需要简化）  
- `FDFPSSlot` 示例：  
  - `EDFPSWeaponSlotType WeaponSlotType`  
  - `FGuid ItemInstanceId`  
  - `EDFPSSlotState SlotState`（解锁状态、是否可用等）  
  
---  
  
## 3. 物品类型划分  
  
### 3.1 武器类物品  
  
- 由 `UDFPSItemDefinition` + `ADFPSWeapon` 双层结构组成：  
  - Inventory 视角：管理是否拥有、在哪个武器栏、当前选中哪一栏  
  - 世界视角：`ADFPSWeapon` Actor 实例挂接在角色上或场景中  
- 武器上的词条：  
  - `ADFPSWeapon` 上挂有 `UDFPSEntryComponent`  
  - 初次生成武器时，根据对应 `UDFPSItemDefinition.DefaultEntryPool` Roll 出词条  
  
### 3.2 护甲/装备类  
  
- 用于修改角色属性、防御、弹药携带上限等  
- 一般不需要单独 Actor，可用纯数据（背包中一行）+ 词条实例  
- 装备后：  
  - 通过 `UFPSEntryComponent` 或类似数据结构将词条映射到角色 `ASC`  
  
### 3.3 消耗品  
  
- 一次性或可叠加使用  
- 可直接引用一个 `UGameplayEffect` 或 `UGameplayAbility` 作为使用时效果  
- 通常**不长时间挂词条**，但可以：  
  - 使用后临时添加一个带持续时间的词条（例如短期增强），通过词条系统施加到角色  
  
### 3.4 任务物品/钥匙  
  
- 主要服务于任务和关卡逻辑  
- 通常不参与词条系统，仅做存在性/数量判断  
  
---  
  
## 4. 物品与词条系统的关系  
  
### 4.1 物品如何持有词条  
  
推荐两种方式：  
  
1. **武器/装备 Actor 上挂 `UDFPSEntryComponent`**：  
   - 武器：`ADFPSWeapon` 直接持有 `UDFPSEntryComponent`  
   - 护甲/配件：若有独立 Actor，也可同样挂接  
2. **纯数据物品通过“虚拟词条容器”与 Entry 系统集成**：  
   - `FDFPSItemInstance` 中保存 `AttachedEntries` 与必要的 Seed/RepData  
   - 需要生效时，Inventory 将这些 Entry 实例应用到角色的 `ASC`  
  
当前阶段建议：  
  
- 武器：使用方案 1  
- 护甲/装备：可用方案 2，以减少运行时 Actor 量  
  
### 4.2 词条的应用/移除时机（由 Inventory 驱动）  
  
整体原则：  
  
- **拥有 ≠ 生效**：  
  - 物品只要在 Inventory 中就表示“拥有”，但词条效果只有在“装备/激活”状态下才对角色或武器产生影响  
- **装备生效、卸下失效**：  
  - 当一个物品被装备到某槽位时：  
    - Inventory 负责通知该物品对应的 `UFPSEntryComponent` 或词条容器，调用：  
      - `ApplyEntriesToOwner()`（伪函数名）  
  - 当物品被卸下或替换时：  
    - Inventory 负责调用：  
      - `RemoveEntriesFromOwner()`  
  
具体流程示例：  
  
- 装备武器：  
  - 将 `FDFPSItemInstance` 绑定到某 `EDFPSWeaponSlotType` 槽位  
  - Spawning 或附着 `ADFPSWeapon` Actor 到角色（若尚未生成）  
  - 调用武器上的 `UDFPSEntryComponent::ApplyEntriesForEquipped()`，将词条效果应用到：  
    - 角色 ASC（角色被动）  
    - 武器自身（武器属性）  
- 切换当前武器（从 Primary 切 Secondary）：  
  - Inventory 检测当前 ActiveSlot 改变：  
    - 对旧武器槽位：  
      - 视设计决定是否彻底移除词条，或仅移除“需持握时生效”的部分  
        - 建议：**仅当武器被“装备在手”时，才生效其“手感相关”词条**  
    - 对新武器槽位：  
      - 激活该武器的 Entry，应立即 Apply  
  
- 装备/卸下护甲：  
  - 护甲词条通常应在“穿在身上”期间始终生效，因此：  
    - 装上护甲槽位：Apply  
    - 脱下或更换：Remove + 新护甲 Apply  
  
---  
  
## 5. 装备栏与槽位设计  
  
### 5.1 武器栏设计  
  
标准 FPS 配置建议：  
  
- `EWeaponSlotType`：  
  - `Primary`（主武器，如突击步枪）  
  - `Secondary`（第二主武器或特殊武器）  
  - `Sidearm`（副武器/手枪）  
  - `Melee`（近战）  
  - （可选）`Gadget`（投掷物/特殊装置）  
  
`UDFPSInventoryComponent` 内部：  
  
- `FDFPSSlot WeaponSlots[MaxWeaponSlots]`  
- `EDFPSWeaponSlotType CurrentActiveSlot`  
  
切换逻辑：  
  
- 当 `CurrentActiveSlot` 改变：  
  - 对旧武器槽位：  
    - 调用对应武器的 `UDFPSEntryComponent::OnUnequippedFromActiveSlot()`，移除“持握时生效”的词条  
  - 对新武器槽位：  
    - 调用 `UDFPSEntryComponent::OnEquippedAsActiveSlot()`，Apply  
  - 驱动动画/武器切换逻辑（与现有射击框架对接）  
  
### 5.2 装备槽设计  
  
视项目深度可选：  
  
- 简化方案：  
  - 仅提供若干汎用装备槽（如 `Armor1`, `Armor2`），每个槽可挂一个装备物品  
- 完整方案：  
  - `EEquipSlotType`：`Head`, `Body`, `Gloves`, `Boots`, `Backpack`, `Gadget1`, `Gadget2`...  
  
`UDFPSInventoryComponent` 中维护：  
  
- `TMap<EDFPSEquipSlotType, FDFPSSlot> EquipSlots`  
  
规则：  
  
- 每个 `UDFPSItemDefinition` 指明其可占用的槽位类型  
- 装备逻辑：  
  - 若目标槽位已有物品：需要先卸下（移到背包）或丢弃  
  - 切换时保证旧物品词条先 Remove，新物品词条再 Apply  
  
---  
  
## 6. 操作流程设计  
  
### 6.1 捡起物品  
  
场景中的物品一般以 `AFPSWorldItem` 或 `AFPSWeaponPickup` 等 Actor 存在：  
  
1. 角色进入拾取范围，触发交互（按键或自动拾取）  
2. 服务端检查：  
   - Inventory 是否有合适的空槽/堆叠空间  
3. 若可拾取：  
   - 从场景中销毁 Pickup Actor（或隐藏标记已被拾取）  
   - 在 `UDFPSInventoryComponent` 中：  
     - 对于武器：  
       - 若武器栏有空位，可直接放入对应武器槽  
       - 否则放入背包区（非装备状态）  
     - 对于其它物品：  
       - 根据堆叠逻辑增加数量或新建 `FDFPSItemInstance`  
   - 如果拾取的是“现成有词条的武器 Actor”：  
     - 需要将其 `UFPSEntryComponent` 的状态复制/绑定到新 `FDFPSItemInstance` 或直接保留 Actor + 组件  
  
### 6.2 装备/切换武器与装备  
  
- 装备武器：  
  - 从背包区选择一件武器物品并指定槽位（或自动选择主武器位）  
  - 如该槽位已有武器，则执行“替换”流程：  
    - 卸载旧武器的词条（Remove）  
    - 移到背包/丢弃  
  - 对新武器：  
    - 生成/附着 `ADFPSWeapon` Actor  
    - 调用其 `UFPSEntryComponent::ApplyEntriesForEquipped()`  
  
- 切换当前武器槽：  
  - 更新 `CurrentActiveSlot`  
  - 通知旧武器和新武器处理“持握时生效”的词条应用/移除  
  
- 装备/卸下护甲：  
  - 类似武器流程，但无需世界 Actor：  
    - 更新 `EquipSlots` 内对应 `FFPSSlot`  
    - 对词条容器调用 Apply/Remove 到角色 ASC  
  
### 6.3 丢弃/销毁物品  
  
- 丢弃：  
  - 若物品当前为**装备或激活状态**：  
    - 必须先执行词条 Remove  
    - 卸载武器 Actor（若有）  
  - 然后从 Inventory 移除对应 `FFPSItemInstance`  
  - 在场景中生成对应 Pickup Actor（如武器掉在地上）  
- 销毁（例如用完的任务物品、消耗品使用耗尽）：  
  - Remove 相关词条  
  - 不生成 Pickup Actor  
  
---  
  
## 7. 联机与复制  
  
### 7.1 服务端权威  
  
- Inventory 状态完全由服务端维护：  
  - 物品添加/移除/移动/装备变更都通过服务器逻辑处理  
- 客户端通过复制的结构仅用于：  
  - UI 展示当前背包与装备  
  - 本地表现（如在 UI 上显示武器和词条）  
  
### 7.2 Inventory 数据复制策略  
  
- 复制结构示例：  
  - `FReplicatedItemEntry`：  
    - `FName ItemId`  
    - `int32 Count`  
    - `FGuid InstanceId`  
    - `uint8 SlotType`（武器槽/装备槽/背包）  
    - `uint8 SlotIndex`  
    - 精简词条信息 `TArray<FFPSEntryRepData>`（可选，若需要客户端展示详细词条）  
  
- `UDFPSInventoryComponent` 中：  
  - 使用 `TArray<FReplicatedItemEntry>` 或分组结构 + `ReplicatedUsing` 回调  
  
### 7.3 与武器/物品 Actor 的生成与销毁  
  
- 武器：  
  - 服务端根据 Inventory 决定何时生成/附着 `ADFPSWeapon` Actor：  
    - 当某武器槽被填充并处于“装备在手”状态时  
  - 物品移出 Inventory 或被丢弃时：  
    - 销毁或分离对应 `ADFPSWeapon` Actor  
- 其他物品：  
  - 通常只在世界中以 Pickup Actor 形式存在  
  - 被拾取后，世界中的 Actor 销毁；Inventory 中只保留数据  
  
---  
  
## 8. UI & UX 设计要点  
  
### 8.1 Inventory 界面风格  
  
- 整体风格：战术感、写实偏科幻，与已有 HUD/UI 统一  
- 界面构成：  
  - 左侧：角色当前装备概览（武器栏 + 护甲槽展示）  
  - 右侧：背包物品列表（简明的网格或列表）  
  - 中央/侧边：选中物品详情面板（基础属性 + 词条列表）  
  
### 8.2 与词条 UI 的结合  
  
- 在物品详情面板中：  
  - 展示：  
    - 基础信息（名称、类型、图标）  
    - 词条列表：  
      - 来自物品上的 `UFPSEntryComponent` 或 `AttachedEntries`  
  - 使用与词条系统文档一致的描述生成规则（`DescriptionFormat` + 数值插值）  
- 在快速切换界面（如武器轮盘）中：  
  - 使用简化版词条标记/图标，避免信息过载  
  
---  
  
## 9. 可扩展性与未来规划  
  
### 9.1 箱子系统与战利品  
  
- 在关卡中添加 `ADFPSLootContainer`：  
  - 内部持有若干 `FDFPSItemInstance`  
  - 打开箱子时，将箱子内物品展示在专用 UI 中，与玩家 Inventory 界面并列  
  - 拾取逻辑与普通落地物统一  
  
### 9.2 交易与任务奖励  
  
- 交易：  
  - 通过修改 Inventory 中物品数量 + UI 表现实现  
- 任务奖励：  
  - 直接在任务完成时向 Inventory 添加对应物品或随机生成的武器/装备（可利用词条池）  
  
### 9.3 Roguelite 与周目成长  
  
- 每次通关或死亡后：  
  - 可根据回合表现奖励持久化的“Meta Inventory”（基地仓库）  
  - 在每个回合开始时，从 Meta Inventory 中抽取若干装备/词条注入本局角色 Inventory  
  
---  
  
## 10. 与现有工程目录的集成建议  
  
- 源码：  
  - `Source/dungeonFPS/DFPSInventorySystem/`  
    - `Components/DFPSInventoryComponent.*`  
    - `Data/DFPSItemDefinition.*`  
    - `Runtime/DFPSItemInstance.*`  
    - `Runtime/DFPSInventoryBlueprintLibrary.*`  
- 内容资源：  
  - `Content/DungeonFPS/Items/Definitions/`  
  - `Content/DungeonFPS/Items/Icons/`  
  - `Content/DungeonFPS/Items/Weapons/`（与现有武器资源保持一致）  
  
---  
  
## 11. 与词条系统文档的对齐要点（摘要）  
  
- 词条系统不再默认在 `BeginPlay` 永久应用，而是：  
  - 提供 `ApplyEntriesToOwner` / `RemoveEntriesFromOwner` / `OnEquipped` / `OnUnequipped` 等接口  
  - 由 `UDFPSInventoryComponent` 在装备状态变化时明确调用