该组件的目的是允许测距仪的简易功能，无论是手持式还是武器安装式。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/908369fb-e616-4793-ae7b-bcf31daab341)

# 详细面板
## 初始化
### 测距仪网格名称
这是用于执行测距仪追踪的网格名称。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/df9da103-07de-4fa9-b70e-57604ca04ae4)
### 测距仪激光套接字名称
这是测距仪网格名称上用于追踪的套接字名称。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/cea64815-6f75-48bb-b19f-396b0f6073c3)
### 文本渲染组件名称
这是用于测距仪显示距离的文本渲染组件名称。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6253b086-a222-4ba8-8182-f16986641350)

## 设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c91f8ad7-0669-4059-a1d2-410aa2952044)
### 最小追踪距离
这是测距仪将读取的最小距离。如果击中点距离小于此距离，则返回0。
### 最大追踪距离
这是测距仪将追踪的最大距离。超过此距离的击中点将返回0。
### 测量符号
#### 米/码/英寸/厘米符号
这些是显示中使用的相应符号，例如使用米时为150M/150m。
### 使用码米
如果为true，将使用码/米进行测量；如果为false，将使用英寸/厘米。
### 使用公制
如果为true，将根据上述设置使用米/厘米进行测量；如果为false，将根据上述设置使用码/英寸。
### 自动测距
如果为true，测距仪将根据自动测距间隔的给定间隔自动获取其观察目标的距离。
### 自动测距间隔
这是测距仪获取距离的速率。1.0 = 每1秒获取一次。

# 函数
## 距离类别
### 获取距离
此函数将测量测距仪观察的目标并应用并返回结果。
### 以字符串形式获取距离
此函数将测量测距仪观察的目标并应用并以字符串形式返回结果。

## 获取器
### 获取测距仪网格
此函数将返回用于测距仪系统的[网格](#range-finder-mesh-name)。
### 获取文本渲染组件
此函数将返回用于测距仪系统的[文本渲染组件](#text-render-component-name)。

# 事件
### 测距时
每次测距仪运行时都会触发此事件。