该组件的目的是充当SKGOpticSceneCaptureComponent的接口，这样您就不需要直接与它交互，同时还提供几种光学相关的设置和功能。例如，它处理标线亮度、循环标线、归零、放大等。

![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/61dc5278-22b8-4af2-90cb-4cd592825762)
# 详细面板
## 初始化
### 光学网格名称
光学网格名称是您蓝图中静态/骨骼网格组件的名称。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/2ac84251-3cad-4154-a95b-f06bceda904b)
### 光学场景捕捉组件名称
光学场景捕捉组件名称是添加到您的放大光学器件上的SKGOpticSceneCaptureComponent的名称。这不是必填字段，仅用于具有SKGOpticSceneCaptureComponent的光学器件上。这可能会在未来进行修改，因为我希望同时支持使用SKGOpticSceneCaptureComponent的缩放和传统的无此组件的缩放方式。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/de620fdf-4469-4926-8978-530412e96676)
### 游戏标签
每个组件都已设置好游戏标签接口，以防您想将其中一个组件与GAS（游戏能力系统）一起使用。

## 设置
### 标线设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6f930e86-50b5-4bc6-a7b7-47ce86484bcd)

#### 标线材质索引
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/828892d6-f0cc-4623-8336-cef16c6f6256)
标线材质索引是将用于标线的材质索引。在下面的示例中，全息瞄准镜的材质索引为1，而热成像光学器件的材质索引为0。

#### 标线材质
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/a0d663ac-0dd6-41df-9520-6c0fc2c11540)
标线材质是一个您可以用来循环切换的材质数组。例如，如果我想在红色和绿色标线之间循环切换，我会设置两个条目，一个红色，一个绿色。

#### 未瞄准标线材质 - 未瞄准捕捉延迟
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/bd6510b7-85c1-41ce-ab4c-2699af4fe123)
未瞄准标线材质和未瞄准捕捉延迟适用于使用SKGOpticSceneCaptureComponent的放大光学器件。当您停止瞄准时，它会等待未瞄准捕捉延迟中指定的时间，然后将标线材质更改为标线材质中指定的材质。如果您想将其更改为简单的玻璃材质，这很有用。

#### 标线亮度参数名称 - 标线亮度设置 - 标线夜视亮度设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6566adef-858e-48d2-9802-ff19e321ab1f)
标线亮度参数名称适用于材质实例中的标线亮度参数。示例项目中提供了许多已预配置的材质和材质函数。默认参数名称"ReticleBrightness"适用于提供的材质参数名称。
标线亮度设置是您希望标线具有的不同亮度级别，您可以循环切换。标线夜视亮度设置是相同的，但仅在光学器件处于夜视模式时应用。



### 放大设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/13afab19-1260-4eec-9eea-3543ada90534)
放大设置仅适用于使用SKGOpticSceneCaptureComponent的放大光学器件（目前）。

#### 放大倍数
放大倍数是您希望光学器件使用的缩放级别/放大倍数的数组。例如，提供的LPVO使用1.0、2.5、5.0、8.0，因为它是一个1-8倍的光学器件。

#### 平滑缩放 - 平滑缩放速率
如果启用平滑缩放，当您改变放大倍数时，它将以平滑缩放速率中指定的速度在放大倍数之间平滑变化。

#### 随放大倍数缩小眼盒 - 缩小眼盒倍数
随放大倍数缩小眼盒选项是为了增加一些真实感。在现实生活中，当您用光学器件放大时，您的眼盒会缩小，使保持眼睛在眼盒中心变得更加困难。启用随放大倍数缩小眼盒后，它将根据您的缩小眼盒倍数设置在您放大/缩小时进行缩放。

#### 是否为第一焦平面
一些光学器件是第一焦平面，一些是第二焦平面。
第一焦平面：您的标线将随放大倍数缩放。
第二焦平面：您的标线在您改变放大倍数时不会缩放/改变。

### 光学归零设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/624b2a2c-b7b2-4f29-a1dc-7bf96b04cdd9)
光学归零设置仅适用于使用SKGOpticSceneCaptureComponent的放大光学器件（目前）。

#### 调整类型 - 调整量
调整量选项根据调整类型而变化。如果是MRAD，您将看到MRAD调整量；如果是MOA，您将看到MOA调整量。
MRAD：调整将以密位为单位进行
MOA：调整将以MOA为单位进行

#### 起始归零设置
#### 以默认归零开始
如果为true，您的光学器件将从默认仰角/风偏刻度中定义的刻度数开始。

##### 以随机归零开始
如果为true，您的光学器件将从随机最大仰角/风偏刻度中定义的刻度数开始。默认值为25，因此范围为-25到25。


# 函数
## SKGOptic类别
#### 开始瞄准
如果使用SKGOpticSceneCaptureComponent，它将开始捕捉。
#### 停止瞄准
如果使用SKGOpticSceneCaptureComponent，它将停止捕捉。
#### 是否为放大光学器件
如果您的光学器件是放大的，它将返回true，否则返回false。
#### 获取光学网格
此函数将返回您的光学器件的网格。

## 标线类别
#### 循环标线
如果您在[标线材质](#reticle-materials)下分配了多个标线，它将在它们之间循环并将其分配给您的标线。
#### 增加标线亮度 - 减少标线亮度
这些函数将根据[标线亮度设置](#reticle-brightness-parameter-name---reticle-brightness-settings---reticle-night-vision-brightness-settings)更改您的标线亮度。如果您处于夜视模式，它将使用[标线夜视亮度设置](#reticle-brightness-parameter-name---reticle-brightness-settings---reticle-night-vision-brightness-settings)中定义的值。
#### 切换标线夜视模式
调用此函数将在定义的[常规](#reticle-brightness-parameter-name---reticle-brightness-settings---reticle-night-vision-brightness-settings)和[夜视模式亮度设置](#reticle-brightness-parameter-name---reticle-brightness-settings---reticle-night-vision-brightness-settings)之间切换标线。

## 缩放类别
#### 放大 - 缩小
这些函数将放大和缩小光学器件（目前）基于您的[放大设置](#magnifications)实现SKGOpticSceneCaptureComponent。
#### 获取当前放大倍数
此函数将返回您的光学器件当前的放大倍数。

## 归零类别
#### 瞄准点上下默认 - 瞄准点左右默认
这些函数将将您的仰角/风偏重置为您的归零点。例如，如果您更改了仰角或风偏的刻度，它将将它们重置为零。
#### 瞄准点上/下/左/右
这些函数将根据您传入的刻度数调整您的仰角和风偏。例如，设置为使用0.1密位的MRAD光学器件将按每刻度1/10密位进行调整。
#### 应用注视旋转归零
此函数适用于具有SKGOpticSceneCaptureComponent的光学器件，旨在从SKGFirearmComponents的ZeroOpticsForZeroAtLocation函数调用。它旨在与SKGProjectileWorldSubsystem函数GetProjectileZeroAtLocation一起使用，该函数将为您获取旋转值。SKGFirearmComponent有一个函数ZeroOpticsForZeroAtLocation，它为火器上的所有光学器件自动执行此过程。