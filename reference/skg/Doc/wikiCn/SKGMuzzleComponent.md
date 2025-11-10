该组件的目的是提供一个一致的点来读取枪口变换，以及提供一个枪口温度系统。其工作原理是，如果您只有一个枪管，它将使用枪管；如果您添加了一个枪口装置，它将使用该枪口装置；如果您在该枪口装置上添加了一个消音器，它将使用消音器来读取枪口信息。

![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/e0e0c6bd-95a1-4473-bc99-3f9f25626847)

# 详细面板
## 初始化
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/b5d90cca-955b-48c5-88b5-35c3e4151384)
### 枪口网格组件名称
这是用于枪口的网格名称。如果您有一个枪管，它是枪管网格；如果您有一个带枪管的完整火器，它是您的火器网格；枪口装置也是如此。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/fc2558d8-fda2-474a-9bc8-fb482d80ad63)
### 枪口套接字名称
这是用于枪口的套接字名称。它用于确定弹丸发射变换以及火器碰撞终点。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/66926c90-8085-4415-b741-7a908ad97220)
### 枪口标签
这用于区分应使用哪个枪口。例如，如果您有一个带枪管的完整火器，火器将具有标签"Barrel"。如果您有一个作为附件的枪管，它将使用标签"Barrel"。如果您有一个枪口制退器，将使用标签"MuzzleDevice"；如果您有一个消音器，将使用标签"Suppressor"。

## 枪口温度
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/a42b42fd-7ec5-4bae-b60e-2981e11fae0c)
### 使用枪口温度系统
如果为true，则此枪口装置将使用枪口温度系统，该系统模拟每次射击后枪口加热并在不射击时缓慢冷却。
### 枪口温度滴答间隔
这是冷却时组件滴答的速率。您不需要为此设置快速滴答速率。
### 最大枪口温度华氏度
这是枪口可以达到的最高温度。
### 枪口温度归一化速率
这是一个用于将枪口温度在0到1之间归一化的值。
### 每次射击增加的枪口温度华氏度
这是每次射击枪口温度将增加的量。
### 每次滴答减少的枪口温度
这是每次滴答枪口温度将减少的量。


# 函数
## 标签类别
### GetMuzzleTag
此函数简单地返回指定的[标签](#muzzle-tag)。

## 枪口温度类别
### ShotPerformed
此函数应在射击时调用（通常从[SKGFirearmComponent](SKGFirearmComponent)调用），以便温度可以增加。
### GetMuzzleTemperature
此函数返回枪口的当前温度。
### GetMuzzleTemperatureNormalized
此函数返回基于[枪口温度归一化速率](#muzzle-temperature-normalize-rate)归一化的枪口当前温度。
### IsUsingMuzzleTemperatureSystem
如果使用枪口温度系统，此函数返回true；否则返回false。

## 变换类别
### GetMuzzleTransform
此函数返回枪口套接字的世界变换。
### GetMuzzleProjectileTransformCompensated
此函数返回用于获得弹丸粗略归零的修改后的枪口变换。例如，将ZeroDistanceMeters设置为100，MOA设置为1.0，将使您的弹丸在100米处与您的瞄准点相交，精度为1 MOA。
### GetMuzzleProjectileTransform
此函数与上述函数相同，但没有任何粗略归零，只有精度。


# 事件
### OnMuzzleTemperatureChanged
每次枪口温度变化时都会触发此事件，并传入新温度。