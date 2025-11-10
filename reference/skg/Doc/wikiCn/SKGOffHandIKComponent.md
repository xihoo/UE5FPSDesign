该组件基本上是一个左手IK组件，但由于它支持左手和右手，所以称为OffHandIKComponent。它旨在处理副手的定位和姿态。在示例中，该组件被添加到火器、护木和前握把上。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/cce86834-9263-47ec-b73e-9eff69849ad4)

# 详细面板
## 初始化
### 副手IK网格名称：
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6eff0306-fca3-45b1-93f5-f4e1967d17d7)
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/b8d6b2d9-717f-48dd-8783-d173d4e253e3)
这是读取套接字的网格名称，例如火器网格、护木网格和前握把网格组件名称，如上图所示。
### 左/右手IK套接字名称
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/d067558a-920b-456f-a198-fbd3f86b3b53)
这是用于定位副手的套接字名称。
### 左/右手IK姿态
这是为副手播放的姿态。使用示例是前握把。对于垂直握把、无握把和斜角握把，您可能希望副手有不同的姿态。
### 副手IK标签
这用于排序。例如，如果您有一个火器、护木（作为附件）和前握把（作为附件），它将设置为仅使用前握把进行副手IK，如果您移除前握把，它将使用护木，如果您移除护木，它将使用火器。
### 游戏标签
每个组件都已设置好游戏标签接口，以防您想将其中一个组件与GAS（游戏能力系统）一起使用。


# 函数
### UpdateOffHandIK
这是由SKGFirearmComponent自动调用的，用于计算从ComponentRelativeTo到偏移套接字的相对偏移。
### GetOffHandIKOffset
这将返回从[UpdateOffHandIK](#updateoffhandik)计算出的偏移
### GetOffHandIKPose
这将返回为组件设置的姿态。如果处于左手模式，它将返回左手姿态；如果处于右手模式，它将使用右手姿态。
### GetOffHandIKMesh
这将返回用于系统的网格，例如护木、前握把、火器等。