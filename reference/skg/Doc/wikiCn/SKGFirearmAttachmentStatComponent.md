该组件的目的是提供一种通用的、无限制的处理统计数据的方式。该组件旨在由蓝图和C++开发者进行扩展。

# 详细面板
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6cb4e371-8ae9-4c00-9bd5-03f55ca33e38)
## 初始化
### 附件统计数据资产
这是用于定义统计数据的数据资产。

# 数据资产
默认情况下，该数据资产包含重量、人体工程学、精度、垂直后坐力、水平后坐力、枪口初速百分比和射速变化百分比的值。  
使用数据资产的原因是它可以在蓝图中扩展，因为C++结构体无法在蓝图中扩展。

# 函数
### GetAttachmentStats
此函数简单地返回组件设置的[数据资产](#attachment-stats-data-asset)。

# 使用方法
## 可重写函数
### AddFirearmAttachmentStats
每次添加SKGFirearmAttachmentStatComponent时都会调用此函数。默认实现将提供的统计数据相加，因此如果您仍希望保留该功能但还想扩展它，请确保调用父函数/Super。
### RemoveFirearmAttachmentStats
每次移除SKGFirearmAttachmentStatComponent时都会调用此函数。默认实现将提供的统计数据相减，因此如果您仍希望保留该功能但还想扩展它，请确保调用父函数/Super。
### CalculateProceduralValues
这就是自由发挥的地方。每次火器统计数据复制时都会调用此函数，因此计算是在本地完成的，如果您愿意，可以对本地和远程客户端产生不同的影响。在这里，您定义每个统计数据如何影响每个程序化系统。默认情况下，它只是使用在[SKGProceduralAnimComponent](SKGProceduralAnimComponent)详细面板中设置的默认值。