实现数据资产的目的是使调整更容易，允许更多的可重用性，并减少对蓝图对象的需求。例如，假设我们有一个火器（M4）类，设计师想要调整瞄准设置，以便在你ADS时，火器会更靠近。设计师可以简单地打开用于M4的瞄准设置的数据资产，进行调整并保存，而不是必须检出/修改实际的M4火器bp类。同样，在从数据构建角色时（不为所有内容使用蓝图类），在组件（如SKGProceduralAnimComponent）上设置数据资产要容易得多，因为你可以直接给它SKGPDAAimingSettings，而不是逐个属性手动设置。

# 程序化
## 初始化数据资产
### SKGPDAOffhandIKInitialize
<img width="347" height="182" alt="image" src="https://github.com/user-attachments/assets/77a490ec-f3a2-4fcd-9a47-5af96694c85f" /><br>
- OffhandIKMeshName: 这是将包含左手和右手IK插槽名称的网格组件名称，例如火器、护木、前握把等。
- LeftHandIKSocketName: 这是在OffhandIKMeshName中定义的网格上的左手IK插槽名称。如果你处于典型的右手主导（右撇子）模式，将使用此插槽。如果你调用SetOffhandIKToLeftHand（这是默认设置），它将使用此插槽。
- RightHandIKSocketName: 这是在OffhandIKMeshName中定义的网格上的右手IK插槽名称。如果你处于左手主导（左撇子）模式，则将使用此插槽。如果你调用SetOffhandIKToRightHand，它将使用此插槽。

### SKGPDAProceduralAnimInitialize
<img width="581" height="407" alt="image" src="https://github.com/user-attachments/assets/70546c67-93cc-44e6-9a52-9b1b66338f58" /><br>
<img width="796" height="707" alt="image" src="https://github.com/user-attachments/assets/411f7d14-8f9d-40f9-82b1-263caee1768e" /><br>
- ProceduralMeshName: 这是将包含ProceduralAimSocketNames的网格组件名称，例如火器网格或光学网格。
- ProceduralAimSocketNames: 这些是瞄准插槽。对于大多数游戏来说，单个瞄准插槽就足够了，但你可以有多个。例如，Elcan光学设备在机匣顶部有内置的机械瞄具，所以它可以有2个。查看上面图片的参考，那个光学设备有3个，所以我添加了3个瞄准插槽，玩家可以在游戏中循环切换。
- RightHandDominatePointAimAimSocketIndex: 这通常只在火器上使用。如果你想要在右手模式下进行点瞄准（通常在游戏中通过翻转火器而不是用瞄准具瞄准来表示），将使用此插槽。
- LeftHandDominatePointAimAimSocketIndex: 与上述相同，但用于左手模式。
- CanUseLeftHandDominate: 在尝试切换时是否可以使用左手模式（再次说明，这仅用于你持有的任何角色）。
- ProceduralAnimGameplayTag: 如果你试图设置动画图表以通过游戏标签混合待机动画，这很有用。例如，如果你有一个M4和一个SKS，M4的SKGDAProceduralAnimInitializeSettings将在这里有M4标签，SKS将有SKS标签。然后在你的动画图表中，你可以简单地从你持有的任何东西的程序化动画组件中获取标签并与其混合。这是可选的。
- GameplayTags: 你希望此数据资产拥有的标签。默认情况下无用途。

## 设置
### SKGPDAAimingSettings
<img width="384" height="336" alt="image" src="https://github.com/user-attachments/assets/b659e9d0-b80f-462c-94b7-d712ba735842" /><br>
- ForwardAxis: 这是你使用的网格的前向轴。Epic声明X在引擎中是前向的，但他们在示例中生成的内容使用Y前向网格。此示例项目中提供的网格是Y前向的，所以前向轴设置为Y。
- AimingDistance: 这是你想要使用的默认瞄准距离。例如，如果这在火器上设置为8，瞄准点将在火器根部前方8个单位（在示例M4中，根部在手枪握把处，所以前方8个单位就是火器的距离。这很难描述，所以我建议你尝试一下）。如果你添加一个瞄准距离为5的光学设备，这将添加到根部前方13个单位。
- bUseFixedAimingDistance: 按照上述场景，如果你在光学设备上启用此功能，那么无论光学设备在火器上的位置以及火器上设置的任何瞄准距离设置如何，你的光学设备瞄准插槽将在相机前方5个单位（假设瞄准距离设置为5）。
- DefaultAimingSpeed: 这是从你持有的任何东西（如火器）上的SKGProceduralAnimComponent读取的。默认情况下，它不会被火器上的任何附件覆盖。要让附件影响你的瞄准速度，你需要结合提供的统计数据系统。
- bAllowCameraZoom: 如果为真，当你瞄准时，相机会以CameraZoomPercentage百分比在CameraZoomInterpSpeed下放大。
- CameraZoomPercentage: 瞄准时相机应该放大的程度。
- CameraZoomInterpSpeed: 瞄准时相机放大/缩小的速度。
- bCanAim: 此程序化动画组件是否可以用于瞄准。

### SKGPDACycleAimingPointSettings
<img width="450" height="400" alt="image" src="https://github.com/user-attachments/assets/c9799145-d104-4424-8f2c-dc1d1a41d520" /><br>
这些是当你在瞄准点之间循环时应用的设置。这可以是从点瞄准到/从点瞄准，到/从不同光学设备，或到/从同一光学设备上的不同瞄准插槽。

### SKGPDADeadzoneSettings
<img width="443" height="325" alt="image" src="https://github.com/user-attachments/assets/ab4ee21a-d904-4e4b-97fa-a4e032f16cce" /><br>
- bUseDeadzone: 是否使用死区。你也可以不添加此数据资产并达到相同效果。
- PitchRate: 这是死区俯仰（上/下）相对于相机移动的速率。值越高=越快。
- YawRate: 与上述相同，但为左/右。
- PitchLimit: 这是你持有的东西相对于相机可以移动的高低限制。值越高=越多。
- YawLimit: 与上述相同，但为左/右。
- InterpolationSpeed: 这是你持有的东西移动到新点的速度。值越高=越快，值越低=更平滑但更慢。
- bDisableDeadzoneWhileAiming: 如果为真，瞄准时将禁用死区。
- bAlwaysInterpolateBackToCenter: 如果为真，当你不移动鼠标时，死区将插值回到中心。
- InterpolateBackToCenterSpeed: 这是死区插值回到中心的速度。值越高=回到中心越快。
- DeadzoneAimingDisableInterpolationSpeed: 与上述相同，但仅在bDisableDeadzoneWhenAiming为真时应用。当你瞄准时，这是瞄准点插值回到屏幕中心的速度。

### SKGPDAMovementLagSettings
<img width="708" height="500" alt="image" src="https://github.com/user-attachments/assets/244cec9d-e446-4b6b-ac15-cbf34bd46e99" /><br>
- bUseMovementLag: 是否要使用移动滞后。不设置此数据资产也可以达到相同效果。
- bDisableForwardMovementLagWhenAiming: 如果为真，在瞄准时向前/向后移动时，火器的俯仰（上/下）不会发生。
- bUseSpringInterpolation: 如果为真，移动滞后将使用弹簧插值，如果为假，它将使用正常插值（InterpSpeed）。
- ForwardBackwardNormalizedMax: 值越小，当你向前/向后移动时火器的俯仰变化越大。
- LeftRightNormalizedMax: 与上述相同，但为左/右移动，它影响火器的横滚。
- UpDownNormalizedMax: 与上述相同，但为上/下移动，如蹲下或跳跃。
- ForwardBackwardMultiplier: 这是前/后移动的乘数。将其设置为负值会反转方向。
- Multiplier: 与上述相同，用于俯仰/偏航/横滚。
- InterpSpeed: 当bUseSpringInterpolation设置为假时的插值速度。

### SKGPDAMovementSwaySettings
<img width="520" height="462" alt="image" src="https://github.com/user-attachments/assets/fbca8093-3171-494e-b383-567af54df60f" /><br>
位置和旋转设置相同，一个用于持有角色的位置移动，一个用于持有角色的旋转。
- bUseMovementSway: 是否要使用移动摇摆。不设置此数据资产也可以达到相同效果。
- Multiplier: 这是将曲线值乘以多少，以便你可以从相同曲线获得不同数量的移动。
- MultiplierInterpolationRate: 你可以通过在你的角色上的SKGShooterPawnComponent上调用SetMovementSwayMultiplier在运行时控制乘数，同样可以通过调用ResetMovementSwayMultiplier将其重置为默认值。MultiplierInterpolationRate使此乘数的变化变得平滑。
- Curve: 这是用于XYZ的曲线。
- InterpSpeed: 这是通过曲线插值的。如果你希望在运行时切换曲线以帮助混合出来，这很有用。

### SKGPDAOffhandIKSettings
<img width="406" height="211" alt="image" src="https://github.com/user-attachments/assets/8344d6bb-53fa-4c33-ace9-40a0ee8e208b" /><br>
- LeftHandIKPose: 这是将应用于左手的姿态。例如，护木将有护木姿态，前握把将有前握把姿态。活动的SKGOffhandIKComponent确定要使用哪种握把姿态。
- RightHandIKPose: 与上述相同，但用于左手主导（左撇子）模式。

### SKGPDAProceduralOffsets
<img width="687" height="550" alt="image" src="https://github.com/user-attachments/assets/6068277f-18f0-48c5-894b-143f079a8dd5" /><br>
- First/ThirdPersonBasePoseOffset: 这影响基础姿态，因此你可以在不需要调整动画的情况下修改待机位置等。我建议不要这样做，而是通过纠正动画来实现。
- ThirdPersonRightHandedAimingOffset: 这是用于瞄准时的偏移。如果你观看别人在没有偏移的情况下瞄准，光学设备将直接对准相机前方，这可能在你角色头部的中心。这让你可以看到带偏移的瞄准者，这样你可以将火器移到肩膀上。
- ThirdPersonLeftHandedAimingOffset: 与上述相同，但用于左手主导模式。

### SKGPDAProceduralPoseSettings
<img width="621" height="1151" alt="image" src="https://github.com/user-attachments/assets/85461046-fd5d-4781-a3d0-e4fbd336b85c" /><br>
- 参见[创建自定义姿态页面](Creating-a-custom-pose)。
- LocationCurve: 这是操纵持有角色位置的曲线。
- RotationCurve: 这是操纵持有角色旋转的曲线。
- PlayRate: 这是通过曲线移动的速度。值越高=越快。
- CurveEndTime: 这是位置或旋转中包含关键帧的最后一帧。如果位置在第6帧结束，但旋转在第10帧结束，则将其设置为10。
- CurveTag: 这是姿态的标识符。上面的截图使用Poses.HighPort，所以如果你想进入HighPort姿态，你将标签Poses.HighPort传递给SKGShooterPawnComponent上的PerformProceduralPose函数。
- Interpolate: 这是你是否希望在整个姿态中进行插值。这对于在姿态之间中途切换或开始和取消姿态以使其混合出来很有用。
- InterpolateConstant: 这只是将插值类型更改为常量插值。
- InterpolationTime: 这是在停止之前将插值多少帧（在截图中设置为5，意味着它只会为前5帧进行插值）。
- InterpolationSpeed: 这是插值速率的速度。
- InterpolationSpeedMultiplier: 待办

### SKGPDARecoilSettings
<img width="533" height="1099" alt="image" src="https://github.com/user-attachments/assets/0143a273-8ea5-49e5-a708-ef732d9bfcbe" /><br>
- 后坐力由曲线驱动，因此控制旋转将有曲线，持有角色的位置和旋转也将有曲线。随机变量用于添加变化。例如，对于RecoilRotation PitchRandom，它在2到5之间。这意味着每次射击时，枪口都会向上翻转不同的量。

### SKGPDARotationLagSettings
<img width="677" height="602" alt="image" src="https://github.com/user-attachments/assets/07ff7b28-861d-4999-afcc-666d6e82ed8d" /><br>
- 这些设置通过你的旋转执行，即你左右/上下转动时的鼠标移动。
- bUseRotationLag: 如果为假，将不使用旋转滞后。不设置数据资产也会产生相同效果。
- bUseSpringInterpolation: 是否使用弹簧插值。
- MaxRotation: 旋转滞后可以旋转到的俯仰/偏航/横滚的最大旋转。
- MaxLocation: 与上述相同，但用于位置。
- RotationMultiplier: 增加旋转量的程度。值越高=你的旋转影响越大。
- LocationMultiplier: 与上述相同，但用于位置。

# 枪口
### SKGPDAMuzzleInitialize
<img width="375" height="152" alt="image" src="https://github.com/user-attachments/assets/8d8f032f-41ca-4c1f-8bd9-883510772916" /><br>
- MuzzleMeshComponentName: 具有MuzzleSocketName的网格组件名称，例如枪管、消音器、枪口装置等。
- MuzzleSocketName: 用于枪口末端的插槽名称，你希望从这里发射弹丸以及用于火器碰撞。

### SKGPDAMuzzleSettings
<img width="552" height="331" alt="image" src="https://github.com/user-attachments/assets/85a344a4-9751-4932-bb28-e56383a39819" /><br>
- MuzzleTag: 这用于确定应使用哪个枪口装置。例如，如果你有一个带枪管的M4，你的枪管将具有MuzzleComponentType.Barrel的枪口设置。如果你在枪管上添加了一个枪口装置，然后在枪口装置上添加了一个消音器，消音器将具有MuzzleComponentType.Suppressor的标签。这让系统知道使用消音器，而不是枪口装置或枪管。
- GameplayTags: 仅用于数据资产的游戏标签。
- bUseMuzzleTemperatureSystem: 如果为真，当你在SKGFirearmComponent或特定的SKGMuzzleComponent上调用ShotPerformed时，它将在每次射击时积累温度并冷却。
- MaxMuzzleTemperatureFahrenheit: 你希望枪口达到的最高温度（华氏度）。
- MuzzleStartGlowTemperature: 这是你希望枪口达到开始发光的温度。这旨在连接到示例项目提供的加热材料。参见消音器示例。
- IncreaseMuzzleTemperatureAmountFahrenheit: 每次射击时枪口装置温度应增加多少。
- DecreaseMuzzleTemperatureAmountPerTick: 每个刻度枪口装置应冷却多少。

# 光学
### SKGPDAOpticInitialize
<img width="457" height="234" alt="image" src="https://github.com/user-attachments/assets/2b42de9d-56c1-4eac-a383-f90e3d228649" /><br>
- OpticMeshName: 包含分划板的网格名称。
- OpticSceneCaptureComponentName: 可选的SKGOpticSceneCaptureComponent的名称。
- bIsMagnifier: 此光学设备是否被视为放大镜。这只是一个布尔值，你可以获取它以使放大镜实现更容易。
- GameplayTags: 仅用于数据资产的游戏标签，默认情况下不使用。

### SKGPDAOpticReticleSettings
<img width="556" height="502" alt="image" src="https://github.com/user-attachments/assets/3ad89930-64a3-4f42-8440-8a7473e89346" /><br>
- ReticleMaterialIndex: 分划板材料应应用到的索引（在光学网格上）。
- bSupportsNightVision: 如果为真，表示此光学设备可以进入夜视模式并切换到使用ReticleNightVisionBrightnessSettings进行亮度调节。
- ReticleMaterials: 这是你可以在分划板中使用并循环的材料数组，例如红/绿分划板。
- UnAimedReticleMaterial: 当你停止使用放大光学设备瞄准时将使用的材料，例如切换到反射玻璃。
- UnAimedCaptureDelay: 瞄准后多长时间光学设备会切换到UnAimedReticleMaterial。
- ReticleBrightnessSettings: 分划板本身的亮度设置。
- ReticleNightVisionBrightnessSettings: 与上述相同，但用于光学设备处于夜视模式时。
- WeightedBlendables: 这些是后期处理材料，主要用于热成像。如果你查看示例项目中的热成像光学设备，它有几个你可以循环的材料，如白热、黑热、彩虹等。
- EyeboxPositionSocket: 用于居中眼框的插槽。我建议将其保留为默认的S_Aim或你称呼瞄准插槽的任何名称，因为它通常居中在光学设备上。

### SKGPDAOpticMagnificationSettings
<img width="492" height="419" alt="image" src="https://github.com/user-attachments/assets/d554e5cb-1234-4049-9016-8ba9b4f70623" /><br>


### SKGPDAOpticZeroSettings
<img width="359" height="364" alt="image" src="https://github.com/user-attachments/assets/b4e69ae4-2e3c-4a16-8b6e-4db4f69a9be2" /><br>