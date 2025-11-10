SKGOpticSceneCaptureComponent是Unreal Engine自带的SceneCaptureComponent的简单替代品。它直接与SKGOpticComponent关联，但仍然可以独立使用。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/4d0c0936-6595-4c64-836d-eeb4edd8e613)

# 详细面板
## 初始化
### 游戏标签
每个组件都已设置好游戏标签接口，以防您想将其中一个组件与GAS（游戏能力系统）一起使用。

## 设置
### 光学场景捕捉组件设置
#### 渲染目标设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c7644e42-b6f4-435a-8a41-9275cc47d9ec)
##### 滴答间隔
此设置用于确定您在捕捉时组件更新/捕捉场景的频率。
##### 分辨率X/Y
这是场景捕捉创建的渲染目标的分辨率。
##### 纹理渲染目标格式
这是渲染目标的格式。

#### 材质设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/ef6f16c7-b24a-438a-8b5e-ee8c98bc3934)
##### 纹理目标参数名称
这是材质中使用的渲染目标的名称。设置为使用示例内容中提供的默认值。
##### 标线尺寸参数名称
这是材质中标线尺寸参数的名称。设置为使用示例内容中提供的默认值。
##### 眼盒灵敏度参数名称
这是用于随放大倍数缩放眼盒的眼盒灵敏度参数的名称。设置为使用示例内容中提供的默认值。


# 函数
注意 - 此组件旨在通过SKGOpticComponent使用
## 滴答类别
#### 开始捕捉 - 停止捕捉
这些函数启动和停止场景捕捉组件的捕捉（捕捉场景并绘制到渲染目标）。

## 缩放类别
#### 缩放
此函数将把场景捕捉缩放到目标放大倍数。如何到达目标放大倍数取决于SKGOpticComponent中的设置。C++用户可以通过调用Initialize函数并传入给定参数来手动设置自己的设置。

## 归零类别
#### 瞄准点上下默认 - 瞄准点左右默认
这些函数将将您的仰角/风偏重置为您的归零点。例如，如果您更改了仰角或风偏的刻度，它将将它们重置为零。
#### 瞄准点上/下/左/右
这些函数旨在从SKGOpticComponent调用。这些函数将根据旋转量调整您的仰角和风偏。
#### 应用注视旋转归零
此函数旨在从SKGFirearmComponents的ZeroOpticsForZeroAtLocation函数调用。此函数适用于具有SKGOpticSceneCaptureComponent的光学器件。它旨在与SKGProjectileWorldSubsystem函数GetProjectileZeroAtLocation一起使用，该函数将为您获取旋转值。SKGFirearmComponent有一个函数ZeroOpticsForZeroAtLocation，它为火器上的所有光学器件自动执行此过程。