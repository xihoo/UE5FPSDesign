该组件的目的是提供一个简单易用的手电筒和激光器，无论是手持式还是火器附件。您不需要同时设置手电筒和激光器，可以只使用其中一个，例如只使用手电筒或只使用激光器，或者两者都使用。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/e838c31f-4ace-4cbd-ab20-e0f157e9ce5f)

# 详细面板
## 初始化
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/87b24aec-98d1-4f64-aef1-ca4bce204e87)
### 灯光组件名称
这是灯光组件的名称。在示例中，它是一个聚光灯。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/835dc30d-6c4e-4258-835e-e46a12ebfac6)
### 激光网格组件名称
这是要使用的激光网格的名称。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/2e449ff5-d0f7-4259-92b9-c2e8b87016cc)
### 激光点组件名称
这是要使用的激光点的名称。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/62b9190b-fe9d-4783-b9be-7ac09b9b7fbf)
### 具有红外模式
如果为true，则此灯光/激光器可以进入/退出红外模式（用于夜视）。

## 设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/31367b3a-ed5f-4edc-95cd-1b3e3556bc97)
### 激光设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/72d0b6e2-ed75-465d-be7b-66af3f1cf889)
#### 最大激光距离
这是激光可以到达的最远距离。如果超出此范围，它基本上会变得不可见。
#### 激光缩放因子
这取决于用作激光的网格的长度。在示例中，激光网格长20cm，因此缩放因子使用值20。如果长10cm，则使用值10。
#### 红外激光材质
此部分可能会在不久的将来被移除，以支持夜视材质函数。
##### 激光
这是红外模式启用时激光将切换到的材质。
##### 激光点
这是红外模式启用时激光点将切换到的材质。

### 灯光设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c82f1e83-90c5-4f2e-8883-4e57e8524e99)
#### 灯光频闪间隔
这是灯光设置为频闪模式时频闪之间的延迟。
#### 灯光强度
这是一个可以循环使用的灯光亮度数组。
#### 红外模式下的灯光强度
这是一个在红外模式下可以循环使用的灯光亮度数组。

### 灯光激光循环模式
如果您想要一个类似tarkov的系统，其中您有一系列要循环的模式而不是单独控制，您可以使用此功能，因为您可以定义用户要循环的模式/功能。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/dcf39b42-605a-4fa0-a8c9-6ba0331fb889)


# 函数
## 灯光激光类别
### 设置红外模式
此函数根据您传入True或False作为参数来开启或关闭红外模式。
### 循环灯光激光模式
此函数将通过在[灯光激光循环模式](#light-laser-cycle-modes)中定义的各种状态循环您的灯光/激光器。

## 激光类别
### 设置激光模式
此函数允许您设置激光模式，例如开启或关闭。
### 获取激光模式
此函数返回激光的当前模式，例如开启或关闭
### 激光是否开启
此函数为分支使用返回简单的true/false。如果激光开启则返回True，关闭则返回False。

## 灯光类别
### 设置灯光模式
此函数将根据您传入的内容设置灯光模式，例如开启、关闭或频闪。
### 获取灯光模式
此函数将返回当前灯光模式，例如开启、关闭或频闪。
### 灯光是否开启
此函数为分支使用返回简单的true/false。如果灯光开启或频闪则返回True，关闭则返回False。
### 设置灯光强度索引
此函数将根据在[灯光强度](#light-intensities)中设置的给定索引设置手电筒的亮度强度。如果您处于红外模式，它将使用[红外模式下的灯光强度](#light-intensities-infrared)。
### 循环灯光强度
此函数将遍历每个[灯光强度](#light-intensities)。如果您处于红外模式，它将使用[红外模式下的灯光强度](#light-intensities-infrared)。

## 获取器
### 获取光源
此函数返回从[灯光组件名称](#light-component-name)找到的光源。
### 获取激光网格
此函数返回从[激光网格组件名称](#laser-mesh-component-name)找到的激光。
### 获取激光点
此函数返回从[激光点组件名称](#laser-dot-component-name)找到的激光点。
### 是否处于红外模式
此函数返回设备当前是否处于红外模式，是则返回true，否则返回false。

## 事件
### 激光状态改变时
每次激光状态改变时都会触发此事件，并传入新的激光状态。
### 灯光模式改变时
每次灯光状态改变时都会触发此事件，并传入新的灯光状态。
### 红外模式改变时
每次红外模式改变时都会触发此事件，并传入新的红外模式。
### 灯光频闪时
每次灯光频闪开启/关闭时都会触发此事件，并传入开启/关闭状态。
### 激光撞击时
每次激光产生撞击时都会触发此事件，并传入结果撞击信息以及是否产生有效撞击。