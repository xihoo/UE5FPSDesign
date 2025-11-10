该组件专用于火器使用，主要充当其他组件之间的接口。它与SKGAttachmentManagerComponent配对，因为它绑定到该组件上的委托，当附件被添加、移除、更改等时触发，并处理它们的设置，因此在您的类中 alongside SKGFirearmComponent 添加 SKGAttachmentManagerComponent 是必需的。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/62c2c352-e449-4395-8893-565df81afa84)

# 详细面板
## 初始化:
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/0ef41886-e1b5-4233-8c06-2bdc326e646a)
### 火器网格组件名称:
您的火器的网格组件（静态或骨骼）。
### 附件管理器组件名称:
您的附件管理器组件的名称。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/906a7881-fb18-46ee-95d7-4800050cf442)

## 火器碰撞:
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6ffc7151-0079-4d4a-92d5-4351fb505e6b)
### 使用火器碰撞:
如果为true，则火器碰撞系统将用于此特定火器。
### 火器碰撞追踪直径:
球体追踪用于碰撞检测，这指定了您希望球体有多大。默认为2cm。
### 火器碰撞停止瞄准距离:
如果您正在瞄准并开始与墙壁碰撞，这是在强制您停止瞄准之前碰撞可以被推回的距离。
### 火器碰撞姿态位置/旋转:
这些是用于火器碰撞的曲线。默认进入短托并然后将枪口推入低姿态。
### 火器碰撞姿态缩放:
这控制火器碰撞通过曲线的速度（较低=较慢）。
### 火器碰撞姿态位置/旋转插值速度:
从当前位置插值到曲线中找到的目标位置的速度。


# 函数
## 程序化统计类别
### SetProceduralStats
此函数允许在单个位置设置程序化统计数据。不太可能使用，但存在以防万一。
### GetProceduralStats
此函数返回程序化动画系统使用的程序化统计数据。

## 归零距离类别
### ZeroOpticsForZeroAtLocation
此函数目前仅适用于带有场景捕捉的光学设备（未来计划）。它将遍历它们并为[SKGProjectileWorldSubsystem](SKGProjectileWorldSubsystem)返回的给定位置归零。这样编写是为了允许更通用的方法并支持多个弹丸系统。

## 枪口类别
### ShotPerformed
此函数将对包含SKGMuzzleComponent的火器的每个部分调用ShotPerformed。请参见[SKGMuzzleComponent](SKGMuzzleComponent)了解该函数的下游作用（用于枪口温度系统）。
### GetMuzzleProjectileSocketTransform
此函数将返回从枪口发射弹丸所需的变换，其中包含设置的归零和精度。
### GetMuzzleProjectileTransforms
此函数与GetMuzzleProjectileSocketTransform相同，但将返回用于发射弹丸的变换数组。对霰弹枪很有用。
### GetMuzzleTransform
此函数返回当前枪口组件枪口的原始变换。

## 姿态类别
### GetPose
如果找到姿态，此函数返回true并输出找到的姿态。

## 瞄准类别
### CycleAimingDevice
此函数将切换[瞄准设备](SKGProceduralAnimComponent)以执行下游功能（用于枪口温度系统）。例如，如果您的火器上有多个红点，它将在每次调用函数时循环切换它们进行瞄准。
### StartPointAiming
此函数将开始使用在此组件中设置的点瞄准套接字。
### StopPointAiming
此函数将停止点瞄准并让您回到使用正常的[瞄准设备](SKGProceduralAnimComponent)进行瞄准。
### IsPointAiming
如果您当前正在点瞄准，此函数返回true，否则返回false。
### GetAimSocketWorldTransform
此函数返回当前[瞄准设备](SKGProceduralAnimComponent)在世界空间中的瞄准套接字变换。

## 火器统计类别
### SetFirearmStats
此函数将直接设置火器统计数据。可能未使用，但存在以防万一。

## 获取器
### GetProceduralAnimComponents
此函数返回当前找到的所有[SKGProceduralAnimComponents](SKGProceduralAnimComponent)。
### GetOffHandIKComponents
此函数返回当前找到的所有[SKGOffHandIKComponents](SKGOffHandIKComponent)。
### GetLightLaserComponents
此函数返回当前找到的所有[SKGLightLaserComponents](SKGLightLaserComponent)。
### GetMuzzleComponents
此函数返回当前找到的所有[SKGMuzzleComponents](SKGMuzzleComponent)。
### GetAttachmentStatComponents
此函数返回当前找到的所有[SKGFirearmAttachmentStatComponents](SKGProceduralAnimComponent)。
### GetFirearmStats
此函数返回火器的当前统计数据
### GetAttachmentManagerComponent
此函数返回当前的[SKGAttachmentManagercomponent](SKGAttachmentManagercomponent)。
### GetCurrentProceduralAnimComponent
此函数返回当前的[SKGProceduralAnimComponent](SKGProceduralAnimComponent)。
### GetCurrentOffHandIKComponent
此函数返回当前的[SKGOffHandIKComponent](SKGOffHandIKComponent)。
### GetCurrentMuzzleComponent
此函数返回当前的[SKGMuzzleComponent](SKGMuzzleComponent)。
### GetCurrentOpticComponent
此函数返回当前的[SKGOpticComponent](SKGOpticComponent)。
### GetCurrentStockComponent
此函数返回当前的[SKGStockComponent](SKGStockComponent)。


# 事件 - 可能会更改
### OnAimingDeviceCycles
每当您的[瞄准设备](SKGProceduralAnimComponent)更改时，此事件触发。