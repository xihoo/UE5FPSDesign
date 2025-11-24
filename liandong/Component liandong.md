xihoo: 详细分析下这个游戏项目中，角色动画的工作机制和流程，尤其是数据是如何流转的

GitHub Copilot: 先给你一个整体结论，再按“谁管谁 / 数据怎么流”拆开讲，这样你可以对整个动画链路有清晰心里图。

**一、总体机制概览（从外到内）**

大致数据流向可以概括成：

1. **Pawn / 角色本身**
   - 接收输入、移动、开火、切镜等行为。
2. **`USKGShooterPawnComponent`（角色总管组件）**
   - 负责和武器、程序化动画、摄像机、自由视角、瞄准状态、偏移等“射击逻辑相关状态”。
   - 也是动画实例的“数据源”：每 Tick 提供一份 `FSKGProceduralAnimInstanceData` 给动画蓝图使用。
3. **`USKGFirearmComponent`（武器逻辑组件）**
   - 管理当前武器的所有“子组件”：程序化动画 `USKGProceduralAnimComponent`、Muzzle、光源/激光、瞄具、托腮、IK 等。
   - 把这些零散子组件的数据整合成一份 `FSKGProceduralAnimInstanceData` 存在自己身上。
4. **`USKGProceduralAnimComponent`（程序化动画组件）**
   - 挂在武器或某个能做程序化动画的 Actor 上。
   - 通过一堆 DataAsset（瞄准、位移偏移、摇摆、滞后、Deadzone、后坐力、Pose 等）形成“配置驱动的动画参数”。
   - 内部管理瞄准 Socket、瞄准偏移、Point Aim、各种 Procedural 设置。
5. **`USKGShooterFrameworkAnimInstance`（动画蓝图后端 C++ 实现）**
   - 挂在角色 SkeletalMesh 上的 `UAnimInstance` 子类。
   - 每帧从 `ShooterPawnComponent` 拉一份 `ProceduralAnimData`，再结合自身的蒙太奇、曲线、控制旋转等，计算最终用于 Pose 的各种 Offset（位移、旋转、瞄准、自由视角、后坐力、武器碰撞等）。
   - 这些变量暴露为 `UPROPERTY(BlueprintReadOnly)`，在 Anim Graph/Anim Layer 里被消费，驱动最终骨骼姿势。

所以：**输入 → Pawn 行为 → `ShooterPawnComponent` 状态 & 驱动武器 → `FirearmComponent` 汇总程序化数据 → 提供 `FSKGProceduralAnimInstanceData` → `USKGShooterFrameworkAnimInstance` 消费/插值/计算 → 动画图输出骨骼姿势**。

---

**二、关键参与者与职责**

### 1. `USKGShooterFrameworkAnimInstance`：动画蓝图的“消费者和计算者”

- 继承 `UAnimInstance`：
  - 重写 `NativeBeginPlay`
  - `NativeUpdateAnimation(float DeltaSeconds)`
  - `NativeThreadSafeUpdateAnimation(float DeltaSeconds)`

- 初始化阶段：
  - `NativeBeginPlay` 中调用 `SetupShooterPawnComponent()`：
    - 通过 `USKGShooterPawnComponent::GetShooterPawnComponent(GetOwningActor())` 找到当前 Pawn 上的 `ShooterPawnComponent`。
    - 绑定武器碰撞相关的追踪委托 `FirearmTraceDelegate` 到 `OnFirearmTraceComplete`。
    - 如果设置了 `bUsePelvisForVelocity`，额外缓存骨骼信息用于速度计算。

- 每帧主逻辑（游戏线程）：
  - `NativeUpdateAnimation`：
    1. 从 `ShooterPawnComponent` 取数据：
       - `ProceduralAnimData = ShooterPawnComponent->GetProceduralData();`
       - `FreeLookRecoilRotation = ProceduralAnimData.FreeLookStartRotation;`
       - `bOffhandIKIsLeftHand = ProceduralAnimData.bOffhandIKIsLeftHand;`
    2. 基础状态：
       - `SetupMandatoryData()`：设置 `bIsLocallyControlled`、`ControlRotation`、目标 Lean 角度、是否在瞄准。
       - `SetupInitialData()`：
         - 若需要持续更新摄像机偏移，则让 `ShooterPawnComponent` 重新计算摄像机偏移。
         - `SetCameraOffset(ShooterPawnComponent->GetCameraOffset());`
         - 从 `ProceduralAnimData` 中读取 OffhandIK 位移 / 旋转 / Pose。
    3. 若 `ProceduralAnimData` 有效：
       - 按内部逻辑（省略行数中的内容）设置 Recoil、MovementSway、Pose、FirearmCollision 等的基础数据。
    4. 瞄准相关：
       - `SetAimingData();` 根据 `ShooterPawnComponent->IsAiming()` 切换瞄准状态、目标 `AimAlpha`、处理瞄准曲线等。
    5. 其他自定义曲线：
       - `SetCustomCurveData(DeltaSeconds);`（比如被武器或外部系统触发的自定义曲线动画）
    6. 通知 Pawn 组件：
       - `ShooterPawnComponent->AnimInstanceTicked(DeltaSeconds);`，用于 pawn 级别的逻辑与动画 Tick 同步。

- 每帧线程安全更新（动画线程上运行）：
  - `NativeThreadSafeUpdateAnimation` 调用一系列 `HandleXXX(DeltaSeconds)`：
    - `HandlePawnVelocity()`：通过 Pawn / 骨骼计算速度和水平速度。
    - `HandleOffsets()`：把基础 Pose 偏移 / Camera 偏移 / 瞄准偏移合成。
    - `HandleProceduralSpine()`：基于 Lean、ControlRotation、SpringState 计算脊柱弯曲。
    - `HandleAiming()`：对 `AimAlpha`、`AimCurveAlpha`、Camera/Sight 偏移做插值和 Spring。
    - `HandleMovementSway()`：结合速度、输入曲线等计算武器/视角摇摆。
    - `HandleMovementLag()` / `HandleRotationLag()`：移动/旋转滞后。
    - `HandlePose()`：基于当前 Pose 曲线、时间位置，更新位移/旋转。
    - `HandleFirearmCollision()`：根据碰撞结果推开武器或切 Pose 等。
    - `HandleDeadzone()` / `InterpDeadzoneBackToCenter()`：鼠标 Deadzone、自由视角相关。
    - `HandleRecoil()` / `InterpRecoilToNone()`：后坐力的施加与恢复。

  这些结果都保存在 `UPROPERTY(BlueprintReadOnly)` 变量中，如：
  - `ComponentSpaceFinalLocation/Rotation`
  - `SpineLeanLeftRight`, `SpineRotationUpDown`
  - `CameraOffsetLocation/Rotation`, `SightOffsetLocation/Rotation`
  - `MovementSwayLocation/Rotation`
  - `AimAlpha`, `AimCurveAlpha`
  等，最终在 Anim Graph 或 Anim Layer 中通过节点（如 Modify Bone / Control Rig / Layered Blend）使用。

- 武器碰撞链路：
  - `SetFirearmCollisionData()` 安排 LineTrace 或 ShapeTrace。
  - 命中后在 `OnFirearmTraceComplete` 中：
    - 更新 `bFirearmCollisionHit`，并与上帧结果对比；
    - 计算碰撞位置、法线，用于推动武器或切换到“贴墙”Pose；
    - 通知 `ShooterPawnComponent::FirearmCollisionChanged`（间接或直接）触发蓝图事件。

---

### 2. `USKGShooterPawnComponent`：数据中转站 + 状态管理

- 作用非常多，但和动画最相关的是：
  - **持有并管理：**
    - `USKGFirearmComponent`（当前武器）
    - `USKGProceduralAnimComponent`（当前程序化动画组件）
    - 光源 / 激光、Optic、OffhandIK 等。
  - **作为 AnimInstance 的“上游数据源”：**
    - 提供 `FSKGProceduralAnimInstanceData GetProceduralData()`。
    - 提供 `GetCameraOffset()`、`NeedsToContinuallyUpdateCameraOffset()` 等 Camera 偏移。
    - 提供 FreeLook 数据、鼠标输入、Lean 角度、是否本地控制、是否 Aiming 等。
  - **和 AnimInstance 建立双向关联：**
    - `LinkAnimLayerClass` / `LinkAnimLayerClassByInstance`：在动画蓝图中链接 ShooterFramework 的 Anim Layer。
    - `Unlink...` 系列取消链接。
    - `AnimInstanceTicked(float DeltaSeconds)`：被 AnimInstance 在 `NativeUpdateAnimation` 结尾调用，用于：
      - 同步一些网络相关数据（如 Yaw 复制）。
      - 基于动画 Tick 做额外逻辑（比如计算 FreeLook 视线射线等）。

- 数据获取过程（核心）：`GetProceduralData()`
  - 在实现中（行号省略）一般会：
    1. 确定当前持有的武器 / ProceduralAnimComponent。
    2. 通过 `CurrentFirearmComponent->GetProceduralData(bIsAiming, bOffhandIKIsLeftHand)` 要一份 `FSKGProceduralAnimInstanceData&`。
    3. 再基于自己的状态（比如 FreeLook、FirearmCollision 推动、当前 Lean 等），可能对其中某些字段做调整或填充。
  - 最终返回 `FSKGProceduralAnimInstanceData` 给 AnimInstance 使用。

- 摄像机偏移：
  - `SetCameraOffset()` 根据当前武器、瞄具、第三人称/第一人称等，利用 `USKGProceduralAnimComponent` 和相关 DataAsset 计算 Camera 的偏移 `FTransform`。
  - `AnimInstance::SetupInitialData()` 每帧（在需要时）调用 `ShooterPawnComponent->SetCameraOffset()` 再读取 `GetCameraOffset()`，以保证 CameraOffset 始终和 Procedural 状态一致。

- 指令下发：
  - 射击、瞄准、自由视角等输入从 Pawn 传到 `USKGShooterPawnComponent`，该组件再：
    - 调 `USKGFirearmComponent::StartPointAiming` / `StopPointAiming` / `CycleAimingDevice`。
    - 调 `USKGProceduralAnimComponent::StartPointAiming` / `CycleAimSocket` / `SetCanAim`。
    - 调 `PerformProceduralRecoil`、`PerformProceduralPose`、`Multi_PlayCustomCurveUnreliable` 等，连带同步到 AnimInstance。

---

### 3. `USKGFirearmComponent`：汇总武器相关 Procedural 数据

- 生命周期：
  - `InitializeFirearmComponent()`：
    - `InitializeComponentFromData()` 从 Firearm DataAsset 初始化统计参数等。
    - `SetupComponents()` 找到并缓存所有相关子组件（Procedural、Muzzle、Optic、OffhandIK、LightLaser 等）。
    - 若有 `FirearmCollisionSettingsDataAsset`，填充 `FirearmCollisionSettings`。
    - `SetInitialProceduralData()` 填充 `ProceduralAnimData` 的默认值（从 `USKGProceduralAnimComponent`、CollisionSettings 等取数据）。
    - 标记 `bIsInitialized = true`。

- 动画数据主入口：`FSKGProceduralAnimInstanceData& GetProceduralData(bool bIsAiming, bool bOffhandIKIsLeftHand)`
  - 在实现里（行 674-736）大致会做的事情是：
    1. 基于 `CurrentProceduralAnimComponent` 和 DataAsset：
       - 取 BasePoseOffset、ThirdPersonAimingOffset；
       - 取 MovementSway / MovementLag / RotationLag / Deadzone / Recoil / Pose 等 Settings。
    2. 把这些塞进 `ProceduralAnimData` 对应字段：
       - `ProceduralAnimData.BasePoseOffset = ...`
       - `ProceduralAnimData.AimingSettings = ...`
       - `ProceduralAnimData.MovementSwaySettings = ...` 等。
    3. 再根据当前武器状态：
       - 是否瞄准（`bIsAiming`）
       - 当前使用的 OffhandIK 是左手还是右手（`bOffhandIKIsLeftHand`）
       - 当前 PointAiming 状态、StockOffset、当前光源/瞄具设置等
       对 `ProceduralAnimData` 里某些动态值做修改。
    4. 返回对 `ProceduralAnimData` 的引用，供 `ShooterPawnComponent` 使用。

- 和 `ProceduralAnimComponent` 的关系：
  - `SetBestProceduralAnimComponent()` 在武器上找到“最佳” `USKGProceduralAnimComponent` 作为当前使用的 Procedural 来源。
  - `OnRep_CurrentProceduralAnimComponent()` 通知客户端当前哪个 Procedural 组件在驱动。
  - `SetProceduralStats()` / `CalculateProceduralValues_Implementation()`：基于武器与配件的组合调整 Procedural 偏移/后坐力等数值。

---

### 4. `USKGProceduralAnimComponent`：配置驱动的程序化动画源

- 初始化：
  - `InitializeProceduralAnimComponent()`：
    - `InitializeComponentFromData()`：
      - 从 `USKGPDAProceduralAnimInitialize` 这类 DataAsset 中读取：
        - `ProceduralMeshName`：哪一个 Mesh 用来做程序化动画（找组件）。
        - `ProceduralAimSocketNames`：瞄准用的 Socket 列表。
        - Point Aim 时左右手惯用的 Socket Index。
        - 是否可以左手主导。
        - `ProceduralAnimGameplayTag`、`GameplayTags`。
      - 从各种 DataAsset 读取 Settings：
        - `AimingSettingsDataAsset` → `FSKGProceduralAimingSettings`
        - `ProceduralOffsetsDataAsset` → 各种基础偏移（BasePoseOffset、第一第三人称偏移等）
        - `MovementSwaySettingsDataAsset` → Sway 曲线与系数
        - `MovementLagSettingsDataAsset` → 移动滞后参数
        - `RotationLagSettingsDataAsset` → 旋转滞后参数
        - `DeadzoneSettingsDataAsset`
        - `RecoilSettingsDataAsset`
        - `PoseSettingsDataAssets` → 数组，转换为 `PoseSettings`（一组 `FSKGToFromCurveSettings`）
    - `SetupComponents()`：
      - 遍历 Owner 的组件，根据名字找 `ProceduralMeshName` 对应的 `UMeshComponent`，赋给 `ProceduralAnimMesh`。
      - 保证 `CurrentAimSocketTransform` 初始化。

- 提供的接口（给 Firearm / Pawn 使用）：
  - 瞄准相关：
    - `GetProceduralAimingSettings() const`
    - `CanAim()`/`SetCanAim()`
    - `GetAimWorldTransform()`：拿当前 Aim Socket 在世界空间的位置。
    - `CycleAimSocket()`：在多个瞄准 Socket 之间循环。
    - `StartPointAiming(bool bRightHandDominant)` / `StopPointAiming()`
    - `UpdateAimOffset(UMeshComponent* ComponentRelativeTo, FVector BaseDistance)` 和 `UpdateAimOffsetWithSocket`：
      - 通过当前 Aim Socket 与目标 Mesh/Socket 的相对位置，计算一个 `AimOffset`（`FTransform`）供 `GetAimOffset()` 返回。
  - Pose / Recoil / Sway 等：
    - `GetPose(FGameplayTag Tag, FSKGToFromCurveSettings& PoseData)`：根据 GameplayTag 获取对应的 Procedural Pose。
    - 一系列 `GetXXXSettings()` 接口：BasePoseOffset/ThirdPersonAimingOffset/MovementSwaySettings/MovementLagSettings/RotationLagSettings/DeadzoneSettings/RecoilSettings。

- 运行时可切换 DataAsset（`SKGProceduralAnimComponentRuntime.cpp`）：
  - `SetAimingSettingsSettings` / `RevertAimingSettings`
  - `SetProceduralOffsets` / `RevertProceduralOffsetsSettings`
  - `SetCycleAimingPointSettings` / `RevertCycleAimingPointSettings`
  - `SetMovementSwaySettings` / `RevertMovementSwaySettings`
  - `SetMovementLagSettings` / `RevertMovementLagSettings`
  - `SetRotationLagSettings` / `RevertRotationLagSettings`
  - `SetDeadzoneSettings` / `RevertDeadzoneSettings`
  - `SetRecoilSettings` / `RevertRecoilSettings`
  - `SetProceduralPoseSettings` / `RevertProceduralPoseSettings`
  - `RevertAllSettingsToDefault()`
  这些函数内部都会：
  - 更新对应 DataAsset 指针；
  - `MARK_PROPERTY_DIRTY_FROM_NAME(...)` 推送网络复制；
  - 调 `OnRep_SettingsDataAssetChanged()`，在本地重新解析这些 DataAsset 并更新内部 `PoseSettings` / `MovementSwaySettings` 等缓存；
  - 最终在下一帧通过 `USKGFirearmComponent::GetProceduralData` → `USKGShooterPawnComponent::GetProceduralData` → `USKGShooterFrameworkAnimInstance` 被消费。

---

**三、用“数据流转”串起来看一遍**

以“玩家按住右键进行瞄准，然后射击”为例，数据大致这样流：

1. 玩家输入 → Pawn 接收到瞄准键 / 开火键。
2. Pawn 调用 `USKGShooterPawnComponent::StartAiming()` / `StopAiming()`，以及开火时 `PerformProceduralRecoil`（或通过 Firearm 调用）。
   - `StartAiming()` 里可能：
     - 修改内部 `bIsAiming` 状态；
     - 通知 `USKGFirearmComponent` / `USKGProceduralAnimComponent` 进行瞄准相关设置（比如切到特定 Aim Socket，设定 Point Aim）。
3. 每帧 `USKGShooterPawnComponent::TickComponent` 运行（若启用 Tick）：
   - 更新自由视角、Yaw/Pitch 限制、鼠标输入缓存等。
   - 在需要时调用 `SetCameraOffset()` 重新基于当前武器瞄具/Procedural 偏移计算 CameraOffset。
4. 动画帧开始：
   - `USKGShooterFrameworkAnimInstance::NativeUpdateAnimation` 被调用。
   - 从 `ShooterPawnComponent->GetProceduralData()` 取出 `FSKGProceduralAnimInstanceData`：
     - 这背后是 `ShooterPawnComponent` 向 `CurrentFirearmComponent->GetProceduralData(bIsAiming, bOffhandIKIsLeftHand)` 要的。
     - `GetProceduralData` 内部从 `USKGProceduralAnimComponent` 及相关 DataAsset 里拉出所有 Procedural 参数，塞进 `ProceduralAnimData`。
   - AnimInstance 根据：
     - 当前是否本地控制（决定旋转用 ControlRotation 还是 BaseAimRotation）。
     - 当前是否瞄准（`bIsTryingToAim`, `bIsAiming`）。
     - 当前 Lean 角度。
     - `ProceduralAnimData` 里的 Aiming/Sway/Lag/Deadzone/Recoil/Pose 等设置。
     - 动态计算各种 Offset 和插值。
   - 结果：更新 `AimAlpha`、Camera/Sight offset、Sway offset、Spine rotation、Recoil offset 等变量。
5. Anim Graph 使用这些变量：
   - 在 AnimLayer 或 AnimGraph 中，通过节点把：
     - CameraOffset/SightOffset 转为肩膀/头部/武器骨骼的位移/旋转调整；
     - MovementSway → 武器骨骼摆动；
     - SpineRotationUpDown/Lean → 脊柱骨骼的旋转；
     - Recoil → 武器和 Camera 的抖动；
     - Deadzone → 区分枪/身体旋转与视线旋转；
   - 合成最终骨骼 Pose。
6. 若发生武器碰撞：
   - `SetFirearmCollisionData()` 设置 Trace 请求。
   - `OnFirearmTraceComplete` 得到 HitResult，更新 `bFirearmCollisionHit` 和相关推开数据，可能通知 `ShooterPawnComponent::FirearmCollisionChanged`。
   - 下一帧 `HandleFirearmCollision()` 与 `HandlePose()` 根据碰撞距离和曲线切入 Collision Pose 或改变枪的位移。

---

**四、小结：机制特点**

- **高度数据驱动**：武器、程序化动画几乎都通过 DataAsset 配置；运行时可动态切换 DataAsset（比如换配件/换姿势包），通过网络复制同步，AnimInstance 只是“读者 + 插值器”。
- **职责清晰**：
  - `ProceduralAnimComponent`：提供“某个可瞄准骨骼”的全套程序化动画参数。
  - `FirearmComponent`：聚合所有武器相关程序化参数成一份 `FSKGProceduralAnimInstanceData`。
  - `ShooterPawnComponent`：处理玩家输入、角色态（自由视角、瞄准、Lean、摄像机）、向 AnimInstance 提供统一 Procedural 数据。
  - `ShooterFrameworkAnimInstance`：把这些数据转成具体的动画变量和骨骼变换。
- **网络友好**：几乎所有重要状态（当前 Procedural DataAsset、当前 ProceduralAnimComponent、AimSocketIndex、Aiming 状态、Pose 状态等）都通过 `OnRep` + PushModel 复制，保证客户端动画和逻辑一致。

---

如果你愿意，我可以下一步结合某个具体 DataAsset（比如一个 Recoil 设置或 MovementSway 设置），画一条“这份配置值如何最终影响到某个骨骼”的更细的路径图，或者帮你对照 wiki/Doc 中的某个模块描述来梳理中文文档版的数据流。

xihoo: 继续下一步结合某个具体 DataAsset（比如一个 Recoil 设置或 MovementSway 设置），画一条“这份配置值如何最终影响到某个骨骼”的更细的路径图

GitHub Copilot: 先选一个比较清晰的链路来拆：**MovementSway（移动摇摆）**，再简要对比一下 Recoil（后坐力）的路径。下面用“从一个 DataAsset 的某个字段出发，一路走到骨骼”的方式画图。

---

**一、MovementSway 的完整数据路径**

以 `USKGPDAMovementSwaySettings`（移动摇摆设置 DataAsset）为例，它会被挂在某个 `USKGProceduralAnimComponent` 上，最终影响到武器或角色骨骼的偏移。

**1. 配置层：DataAsset → ProceduralAnimComponent**

- 你在编辑器里创建一个 `USKGPDAMovementSwaySettings` 资产（比如 `PDA_MovementSway_Rifle`），里面有类似：
  - 用到的曲线（移动距离 → 摇摆位置/旋转）
  - 摇摆强度系数、速度系数、加速度权重等
- 在某个 `USKGProceduralAnimComponent` 上配置这个资产：
  - `MovementSwaySettingsDataAsset = PDA_MovementSway_Rifle`

**代码路径：初始化时把 DataAsset 解析成纯数据结构**

在 `USKGProceduralAnimComponent::InitializeComponentFromData()` 中：

- 如果有 `MovementSwaySettingsDataAsset`：
  - 从这个 DataAsset 里读出所有 sway 相关参数
  - 填入内部的 `FSKGMovementSwaySettings MovementSwaySettings`（具体字段在数据类型头文件中）
- 如果 DataAsset 为空，则用默认值。

所以第一步：  
`PDA_MovementSway_Rifle (USKGPDAMovementSwaySettings)`  
→ `USKGProceduralAnimComponent::MovementSwaySettings`（一个 `FSKGMovementSwaySettings` 结构体）

---

**2. ProceduralAnimComponent → FirearmComponent：汇总为 ProceduralAnimData**

在 `USKGFirearmComponent::GetProceduralData(bool bIsAiming, bool bOffhandIKIsLeftHand)` 中（674–736 行一带）：

- 它会从当前 `USKGProceduralAnimComponent` 里取各类设置，例如：
  - `GetMovementSwaySettings()` 返回的就是 `FSKGMovementSwaySettings`
- 然后把这些值塞进自己维护的 `FSKGProceduralAnimInstanceData ProceduralAnimData` 里：

伪代码示意（结构）：

```cpp
FSKGProceduralAnimInstanceData& USKGFirearmComponent::GetProceduralData(bool bIsAiming, bool bOffhandIKIsLeftHand)
{
    // ...
    ProceduralAnimData.MovementSwaySettings = CurrentProceduralAnimComponent->GetMovementSwaySettings();
    // 还会设置其它：Offsets, Lag, Recoil, Deadzone, Poses...
    return ProceduralAnimData;
}
```

第二步：  
`USKGProceduralAnimComponent::MovementSwaySettings`  
→ `USKGFirearmComponent::ProceduralAnimData.MovementSwaySettings`

---

**3. FirearmComponent → ShooterPawnComponent：角色态再加工**

`USKGShooterPawnComponent::GetProceduralData()` 中：

- 拿到当前 `USKGFirearmComponent` 的引用（当前持有的武器）。
- 调用 `CurrentFirearmComponent->GetProceduralData(IsAiming(), bOffhandIKIsLeftHand)` 得到 `FSKGProceduralAnimInstanceData&`。
- 在这基础上：
  - 根据角色状态（自由视角、瞄准、Lean、当前姿势）进一步调整某些字段（例如推开量、当前瞄具等）。
- 最后返回一个 `FSKGProceduralAnimInstanceData` 值（或拷贝）给 AnimInstance。

第三步：  
`USKGFirearmComponent::ProceduralAnimData.MovementSwaySettings`  
→ `USKGShooterPawnComponent::GetProceduralData()` 返回的 `FSKGProceduralAnimInstanceData.MovementSwaySettings`

---

**4. ShooterPawnComponent → AnimInstance：NativeUpdateAnimation**

在 `USKGShooterFrameworkAnimInstance::NativeUpdateAnimation(float DeltaSeconds)`：

```cpp
ProceduralAnimData = ShooterPawnComponent->GetProceduralData();
```

- `ProceduralAnimData` 是 AnimInstance 内部的成员（同名 `FSKGProceduralAnimInstanceData`）。
- 这一步把前面所有层组合出的 MovementSway 设置拿进了 AnimInstance 内部。

紧接着会调用：

```cpp
SetMovementSwayData(DeltaSeconds);
```

方法体（你看到的是省略版）大致会做：

- if `ProceduralAnimData.MovementSwaySettings.bUseMovementSway`：
  - 根据 `PawnVelocity`、移动方向、`MovementSwaySettings` 里的曲线和系数，计算本帧的 sway 基础值：
    - `MovementSwayCurveLocation`
    - `MovementSwayCurveRotation`
  - 再乘上 `MovementSwayLocationMultiplier`、`MovementSwayRotationMultiplier` 等。
- else：
  - 把 `MovementSwayLocation` / `MovementSwayRotation` 清零或缓慢插值回零。

第四步：  
`FSKGProceduralAnimInstanceData.MovementSwaySettings`  
→ `AnimInstance::SetMovementSwayData()` 计算出  
`MovementSwayLocation` / `MovementSwayRotation`（AnimInstance 内变量，`UPROPERTY(BlueprintReadOnly`）

---

**5. AnimInstance → AnimGraph：变量驱动骨骼节点**

在动画蓝图/AnimLayer 里：

- `USKGShooterFrameworkAnimInstance` 暴露的属性：
  - `FVector MovementSwayLocation`
  - `FRotator MovementSwayRotation`
- 你在 AnimGraph 里通常会：
  - 用控制枪 Mesh 或手部骨骼的 “Modify Bone” / “FABRIK” / “Two Bone IK” 或者 Control Rig，把 `MovementSwayLocation/Rotation` 当作：
    - 某个局部空间 Offset（例如加到武器根骨）
    - 或者作为 Additive Pose 输入（通过一个专门的摇摆姿势 BlendSpace）

示意流程图：

```text
PDA_MovementSway_Rifle (USKGPDAMovementSwaySettings)
      │
      ▼
USKGProceduralAnimComponent::MovementSwaySettings (FSKGMovementSwaySettings)
      │
      ▼
USKGFirearmComponent::ProceduralAnimData.MovementSwaySettings
      │
      ▼
USKGShooterPawnComponent::GetProceduralData().MovementSwaySettings
      │
      ▼
USKGShooterFrameworkAnimInstance::ProceduralAnimData.MovementSwaySettings
      │
  SetMovementSwayData() 计算
      │
      ▼
MovementSwayLocation / MovementSwayRotation（AnimInstance 成员）
      │
      ▼
AnimGraph 节点（Modify Bone / Control Rig 等）
      │
      ▼
武器骨骼 / 手臂骨骼实际位置和旋转变化
```

---

**二、对比一下 Recoil（后坐力）的链路**

Recoil 和 MovementSway 非常类似，只是触发点不同：

1. **配置层**
   - `USKGPDARecoilSettings` → 配在 `USKGProceduralAnimComponent` 上。

2. **解析层**
   - 在 `USKGProceduralAnimComponent::InitializeComponentFromData()` 中：
     - 把 DataAsset 内容填入内部的 `FSKGRecoilSettings`。

3. **汇总到 ProceduralAnimData**
   - 在 `USKGFirearmComponent::GetProceduralData`：
     - 把 `GetRecoilSettings()` 结果塞到 `ProceduralAnimData.RecoilSettings`。

4. **传到 AnimInstance**
   - 和 MovementSway 一样，通过 `USKGShooterPawnComponent::GetProceduralData()` → `AnimInstance::ProceduralAnimData`。

5. **触发与计算**
   - 触发：
     - 开火时，`USKGShooterPawnComponent::PerformProceduralRecoil` 或 Firearm/武器蓝图调用 AnimInstance 的 `PerformRecoil`。
   - 在 AnimInstance：
     - `SetRecoilData()` 根据 `ProceduralAnimData.RecoilSettings` 设置当前的 recoil 目标值（比如最大偏移、多段曲线）。
     - `HandleRecoil(DeltaSeconds)` 每帧按 Spring 或者曲线让 recoil 叠加/衰减：
       - 更新 `RecoilLocation` / `RecoilRotation`（或类似命名）。

6. **AnimGraph 使用**
   - 这些 `RecoilLocation/Rotation` 和 `MovementSwayLocation/Rotation` 一样，被 AnimGraph 里的节点使用，最终表现为武器后坐和视角抖动。

Recoil 的流水线图可简化为：

```text
PDA_Recoil_Rifle (USKGPDARecoilSettings)
   → USKGProceduralAnimComponent::RecoilSettings
   → USKGFirearmComponent::ProceduralAnimData.RecoilSettings
   → USKGShooterPawnComponent::GetProceduralData()
   → USKGShooterFrameworkAnimInstance::ProceduralAnimData.RecoilSettings
   → SetRecoilData() / HandleRecoil()
   → RecoilLocation / RecoilRotation
   → AnimGraph 节点 → 骨骼/摄像机抖动
```

---

**三、如果你想具体看到“某个骨骼”的使用方式，可以怎么查**

在工程里，你可以这样顺藤摸瓜：

1. 在 AnimBlueprint/AnimLayer 里搜索 `MovementSwayLocation` 或 `RecoilLocation` 这两个变量的引用，看它们接到哪些节点上。
2. 通常会连到：
   - `Modify Bone`（比如对 `weapon_root` / `hand_r` / `hand_l`）
   - 或 Control Rig 的输入变量，再在 Rig 图中找到具体改的骨骼。
3. 结合节点设置：
   - 如果是 Local Space：说明是对该骨骼自身的局部位移/旋转。
   - 如果是 Component/World Space：会看到整个 Mesh 的抖动，可能是“镜头摇”那种。

---

如果你愿意，我可以帮你针对某个具体的 `AnimBlueprint` 文件，把 `MovementSwayLocation` 或 `RecoilLocation` 在图里的连线翻译成“具体哪个骨骼怎么被改了”的中文说明（比如：`weapon_root` 在局部空间上增加 X 方向 2cm 的往复位移，形成跑动晃动感）。