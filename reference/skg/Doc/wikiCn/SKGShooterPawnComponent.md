# 概述
该组件被添加到您的Pawn中，允许您轻松与系统的其余部分进行接口。在这里您可以设置和获取与系统其余部分一起使用的持有Actor，并且有辅助函数可以访问当前持有Actor的SKGFirearmComponent、SKGProceduralAnimComponent、SKGOpticComponent等。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/190b3cc7-3f61-4499-b577-571f487bfae0)

# 详细信息
## 初始化:
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/8bd3aca1-c5d2-4fd5-84ff-153033d9d7fb)
### 链接动画层类
这是用于继承自我的动画实例的程序化类。如果您想使用链接动画层设置来获得更清晰的动画图，这很有用。
### 自动设置链接动画层
如果为true，[链接动画层类](#linked-anim-layer-class)将自动设置。
### 使用单一网格:
如果为true，将使用第三人称网格组件名称或第一人称网格组件名称(找到哪个就用哪个)，而不是两者都用，并将它们视为相同的。
### 第三人称网格组件名称:
这是您Pawn上的第三人称网格的名称。在ACharacter类中，默认为CharacterMesh0。
### 第一人称网格组件名称:
这是您Pawn上的第一人称网格的名称。如果您使用真正第一人称视角，则不需要第一人称网格。
### 摄像机组件名称:
Pawn上摄像机组件的名称。
### 摄像机附加套接字:
摄像机附加到的套接字名称。
### 复制远程偏航:
由于Unreal默认不复制您的偏航，我提供了一个系统，可以让您获得复制的控制旋转(俯仰和偏航被压缩成一个字节，在接收时解压缩)。
### 远程偏航复制速率:
偏航复制的速率。只有当鼠标通过SetMouseInput函数移动时才会复制(也用于死区)。
### 夜视材质参数集合:
用法在维基的夜视部分有介绍。

## 自由观察:
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/535bfdc8-df26-47ab-b684-242c3278b8a0)
### 自动设置使用控制器旋转偏航:
如果为true，这将自动设置您的Pawn是否启用了bUseControllerRotationYaw。
### 最大俯仰上/下/左/右:
自由观察时从中心位置可以向相应方向旋转的最大角度。
## 倾斜 - 进行中:
### 最大左/右倾斜角度:
可以向相应角度倾斜的最大角度。

## 倾斜
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/963f22ac-b551-4f7a-bdb1-3c49b8234fc4)
### 使用弹簧插值
如果为true，将在倾斜/取消倾斜时使用弹簧插值。
### 弹簧插值设置
请在谷歌上搜索这些设置，因为它们对所有弹簧插值都是标准的。
### 最大左/右倾斜角度
这是您可以向左/右倾斜的最大角度。由于压缩限制，最大值为45度，但您可以在动画图中使用多个倾斜节点来堆叠，以倾斜到任意角度。

## 调试
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c4ce8eda-2bf5-4bb1-b7a0-5da73c0ebd6d)
### 打印命中
这特定于火器碰撞，但它会将火器碰撞追踪的命中Actor打印到屏幕上。
### 绘制调试追踪
这特定于火器碰撞，但它会绘制火器碰撞追踪的调试追踪。

# 函数
## 持有Actor类别
### 设置持有Actor
此函数是让Actor(如火器)与系统交互的核心。当您拾取火器时，必须调用SetHeldActor。
### 清除持有Actor
此函数是清除Actor使用的核心。例如，当您丢弃火器时，必须调用ClearHeldActor。

## 动画层类别
### 链接动画层类
此函数将链接设置的[链接动画层类](#linked-anim-layer-class)到动画实例。
### 按实例链接动画层类
此函数将链接设置的[链接动画层类](#linked-anim-layer-class)到传递给函数的特定动画实例。
### 取消链接动画层类
此函数将从动画实例中取消链接设置的[链接动画层类](#linked-anim-layer-class)。
### 按实例取消链接动画层类
此函数将从传递给函数的特定动画实例中取消链接设置的[链接动画层类](#linked-anim-layer-class)。

## 自由观察类别
### 可以添加偏航 - 可以添加俯仰
这些函数检查在自由观察时是否可以应用更多的偏航/俯仰，或者是否已达到最大限制。
### 开始自由观察
调用时，此函数将启动自由观察系统。
### 停止自由观察
调用时，此函数将停止自由观察系统。
### 正在自由观察
如果当前正在自由观察，此函数将返回true，否则返回false。

## 鼠标输入类别
### 设置鼠标输入
此函数是与死区一起使用所必需的，因为它在接收到鼠标输入时运行。接收鼠标输入的X和Y值。

## 灵敏度类别
### 获取放大灵敏度倍数
此函数将返回给定的倍数，供您在鼠标输入(X和Y)中使用。它使您的鼠标灵敏度随放大/缩放量而缩放。

## 倾斜类别
### 向左/向右倾斜
这些函数接收一个目标角度，这个角度是您想要倾斜到的目标。这允许增量倾斜。
### 停止向左/向右倾斜
这些函数将停止给定方向的倾斜。
### 获取目标倾斜角度
此函数返回当前目标倾斜角度(您试图倾斜的角度)。

## 程序化动画类别
### 开始瞄准
此函数将开始使用给定的持有火器或瞄准镜进行瞄准。
### 停止瞄准
此函数将停止使用给定的持有火器或瞄准镜进行瞄准。
### 正在瞄准
如果当前正在瞄准，此函数将返回true；如果不瞄准，则返回false。
### 设置本地使用第一人称程序化
这是默认设置。基本上它会使设置的程序化为本地客户端使用第一人称设置。
### 设置本地使用第三人称程序化
此函数将使设置的程序化为本地客户端使用第三人称设置(对第三人称模式很有用)。
### 设置摇摆倍数
此函数将更改您的摇摆量。这对受伤和稳定瞄准很有用。
### 重置摇摆倍数
此函数将把摇摆倍数重置回默认值。
### 正在使用自定义摇摆倍数
如果您调用了SetSwayMultiplier而没有重置它(使用与默认值不同的值)，这将返回true。
### 执行程序化后坐力
此函数将使用火器上的[SKGProceduralAnimComponent](SKGProceduralAnimComponent)提供的设置执行程序化后坐力。
### 执行程序化姿态
此函数将触发由传入的游戏标签给出的姿态(如果找到)。如果bExitPose为true，它将执行该姿态的退出姿态。
### 执行自定义曲线
此函数将基于传入的CurveData触发曲线播放。
### 设置副手IK为左手
此函数将把您从使用右手程序化更改为使用左手程序化。
### 设置副手IK为右手
此函数将把您从使用左手程序化更改为使用右手程序化。
### 获取程序化姿态数据
此函数将从传入的游戏标签返回姿态数据(如果找到)。

## 红外类别
### 启用红外模式
此函数将从[SKGShooterWorldSubsystem](SKGShooterWorldSubsystem)触发红外模式，这将把注册的红外设备(如LightLaser)切换到红外模式(如果它们支持)。它还将自动设置[材质参数集合](#night-vision-material-parameter-collection)中的值。简而言之，当您启用夜视时只需调用此函数。
### 禁用红外模式
此函数将从[SKGShooterWorldSubsystem](SKGShooterWorldSubsystem)触发红外模式，这将把注册的红外设备(如LightLaser)从红外模式切换回正常模式(如果它们处于红外模式且支持)。它还将自动设置[材质参数集合](#night-vision-material-parameter-collection)中的值。简而言之，当您禁用夜视时只需调用此函数。

## 组件类别
### 获取当前火器组件
这将返回当前持有的[SKGFirearmComponent](SKGFirearmComponent)。
### 获取当前程序化动画组件
这将返回当前持有的(或来自火器的)[SKGProceduralAnimComponent](SKGProceduralAnimComponent)。
### 获取当前瞄准镜组件
这将返回当前持有的(或来自火器的)[SKGOpticComponent](SKGOpticComponent)。
### 获取当前激光组件
这将返回当前持有的(或来自火器的)[SKGLightLaserComponents](SKGLightLaserComponent)。

## 杂项类别
### 本地控制
如果此组件的所有者Pawn是本地控制器，这将返回true。
### 本地使用第一人称程序化
如果此组件设置为为其本地客户端使用第一人称程序化，这将返回true。
### 获取控制旋转
如果您的本地控制器，此函数将返回控制旋转；如果您的远程控制器，则返回模拟控制旋转的自定义复制旋转器。

## 获取器
### 获取程序化射手Pawn数据
这将返回特定于Pawn的当前程序化数据，供您在自己的动画图中使用，如当前持有Actor的ProceduralAnimGameplayTag、bOffHandIKIsLeftHand和用于自由观察的FreeLookLookAtRotation。
### 获取摄像机组件
此函数将返回存储的摄像机组件。
### 获取Pawn网格
如果本地控制，此函数将返回[第一人称网格](#first-person-mesh-component-name)；否则将返回[第三人称网格](#third-person-mesh-component-name)。
### 获取拥有Pawn
此函数将返回组件的拥有Pawn。
### 获取射手框架动画实例
此函数将返回设置的[SKGShooterFrameworkAnimInstance](SKGShooterFrameworkAnimInstance)。

# 事件
### 姿态完成时
当姿态完成时(从开始到结束或从结束到开始)触发此事件。
### 设置持有Actor时
当HeldActor更改时触发此事件。