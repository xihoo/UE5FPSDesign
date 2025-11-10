该组件的目的是提供一种简单的方式来管理单个附件。它与附件管理器（可选）直接关联，它们可以协同工作，形成一个易于使用但功能强大的附件系统。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/3a31f72f-e248-4d6b-8c2c-6623a4dfd3f6)

# 详细面板
## 初始化
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/04eb221b-4492-4951-bfd7-0b44ff2f72e9)
### 附加到网格名称
这是附件要附加到的网格组件的名称。在本示例中，对于瞄准镜来说，它是M4的骨骼网格。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/f66d6a85-3174-46e2-97a2-fa696ae68767)
### 附加到插槽
这是网格上附件要附加到的插槽名称。在本示例中，对于瞄准镜来说，它是M4网格上的S_Attach_Optic插槽。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/99029c8f-f7c9-4904-9b3a-99e47ab78283)
### 组件名称
这是一个可选部分，对于制作自定义菜单很有用。
### 组件图像
这是一个可选部分，对于制作自定义菜单很有用。

## 附件
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/b8b68245-7437-4531-91e4-615ffa0adce6)
### 功能必需
这用于自定义菜单。在示例中，我将其用于必需部件，如枪管和护木，这样它们就不能在自定义菜单中被移除。
### 自动设置主导姿态组件
如果您将附件系统用于角色，这很有用。当附件被附加时（假设是一件衬衫），它会自动为您设置主导姿态组件和主导姿态附件网格名称。
### 主导姿态附件网格名称
这是与上述自动设置主导姿态组件设置一起使用的网格组件名称。
### 随机默认附件
如果为true，将从[兼容附件](#compatible-attachments)中随机选择一个附件并在生成时设置。
### 默认附件
如果设置，这将在生成时自动生成指定的附件，但仅当该附件根据以下设置兼容时才会生成。
### 兼容附件
这是一个数据资产数组。在每个数据资产中，您定义允许在每个资产中使用的附件，然后您可以将它们组合使用。如果希望所有附件都兼容，请将此留空。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/f6cd751c-a5e2-4a1c-a7b2-1c1ec19c55a4)

### 设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/d58ba305-5866-4a14-8194-70379b479435)
#### 允许客户端修改
如果为true，客户端可以添加/移除附件。如果为false且作弊者试图这样做（例如直接调用RPC），将断开客户端连接。

#### 偏移设置
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/dee00f70-3f72-4c3a-bec2-2e783675cbbe)
##### 每次更改时复制偏移
如果为true，每次对附件应用偏移时都会进行复制。建议将其关闭。如果关闭，则需要调用[FinalizeAttachmentOffset](#finalizeattachmentoffset)来复制。
##### 偏移轴
此设置允许您定义希望在XYZ方向上移动附件的方向。
##### 偏移捕捉距离
此设置对导轨部分很有用。例如，如果您沿导轨上下移动瞄准镜，由于皮卡汀尼槽口间隔为1.046cm，因此不能自由移动。
##### 允许的最小偏移
这是附件允许向反方向移动的距离。例如，默认情况下瞄准镜可能完全向后，因此该值为0，只能向前移动，但示例中的可折叠枪托可以向后移动，因此其值为-4。
##### 允许的最大偏移
这是附件允许向前移动的距离。例如瞄准镜沿导轨移动。

#### 附件规则
这些是在调用AttachToComponent时应用的规则。这里的默认值已足够，但仍可自定义，以防您需要不同的设置。


# 函数
## 附件类别
### TryLoadSetupAttachment
此函数将异步加载传入的类软指针。加载后，它将尝试生成、设置和附加附件。
### TrySetupAttachment
此函数将尝试从传入的类生成、设置和附加附件。
### TrySetupExistingAttachment
此函数将尝试设置和附加传入的附件。
### TrySetupExistingAttachmentNoAttach
此函数将尝试设置传入的附件但不附加它。如果您使用其他已处理附加的系统，这很有用。
### RemoveAttachment
此函数将从系统中移除附件，但不会分离或销毁它。如果您要将此附件丢到地上或添加到库存中，或者只是传递给另一个系统，这很有用。
### DestroyAttachment
此函数将从系统中移除附件并销毁它。
### HasAttachment
如果系统中注册了有效的附件，此函数将返回true。

## 兼容性类别
### GetCompatibleAttachments
此函数将返回在[兼容附件](#compatible-attachments)中定义的已创建兼容附件列表。
### IsAttachmentClassCompatible
此函数可重写。默认情况下，如果传入的类与在[兼容附件](#compatible-attachments)中定义的附件兼容，它将返回true。
### IsAttachmentCompatible
此函数可重写。默认情况下，如果传入的附件与在[兼容附件](#compatible-attachments)中定义的附件兼容，它将返回true。

## 预览类别
### SetPreviewAttachment
此函数将创建您传入的附件类的本地生成预览。如果存在现有附件，它将简单地隐藏它以仅显示预览。
### DestroyPreviewAttachment
此函数将销毁生成的预览附件。如果存在现有附件，它将取消隐藏它，使一切恢复正常。

## 偏移类别
### FinalizeAttachmentOffset
当[每次更改时复制偏移](#replicate-offset-each-change)设置为false时使用此函数。必须调用此函数来复制偏移。
### SetAttachmentOffset
此函数将把附件的偏移设置为传入的值。它在[允许的最小偏移](#minimum-offset-allowed)和[允许的最大偏移](#maximum-offset-allowed)之间进行限制，并利用[偏移捕捉距离](#offset-snap-distance)跳转到最近的捕捉点（如果已定义）。
### IncrementOffset
此函数将按[偏移捕捉距离](#offset-snap-distance)增加偏移。它在[允许的最小偏移](#minimum-offset-allowed)和[允许的最大偏移](#maximum-offset-allowed)之间进行限制。
### DecrementOffset
此函数将按[偏移捕捉距离](#offset-snap-distance)减少偏移。它在[允许的最小偏移](#minimum-offset-allowed)和[允许的最大偏移](#maximum-offset-allowed)之间进行限制。
### CanAttachmentBeOffset
如果可以应用偏移（如果[允许的最小偏移](#minimum-offset-allowed)和[允许的最大偏移](#maximum-offset-allowed)不相等），此函数返回true。
### GetAttachOffsetTransform
此函数返回附件的偏移世界变换（应用偏移后的[附加到插槽](#attach-to-socket)位置）。
### GetAttachTransform
此函数返回附件附加点的世界变换（应用偏移后的[附加到插槽](#attach-to-socket)位置）。
### GetAttachStartTransform
此函数返回由[允许的最小偏移](#minimum-offset-allowed)定义的附件起始偏移世界变换（应用[允许的最小偏移](#minimum-offset-allowed)后的[附加到插槽](#attach-to-socket)位置）。
### GetAttachEndTransform
此函数返回由[允许的最大偏移](#maximum-offset-allowed)定义的附件起始偏移世界变换（应用[允许的最大偏移](#maximum-offset-allowed)后的[附加到插槽](#attach-to-socket)位置）。
### GetAttachSnapPoints
此函数返回由[允许的最小偏移](#minimum-offset-allowed)和[允许的最大偏移](#maximum-offset-allowed)范围以及[偏移捕捉距离](#offset-snap-distance)间隔定义的所有可能捕捉点的数组。

## 获取器
### GetAttachToSocket
此函数返回[附加到插槽](#attach-to-socket)。
### GetComponentName
此函数返回[组件名称](#component-name)。
### GetComponentImage
此函数返回[组件图像](#component-image)。
### GetDefaultAttachment
此函数返回[默认附件](#default-attachment)。
### GetAttachToMesh
此函数返回[附加到网格名称](#attach-to-mesh-name)。
### GetAttachment
此函数返回当前注册的附件。
### GetAttachmentOffset
此函数返回当前附件偏移。
### GetIsRequiredToFunction
此函数返回附件组件是否被指定为功能必需的附件。
### GetOffsetSettings
此函数返回[偏移设置](#offset-settings)。

## 事件
### OnAttachmentAdded
每次注册附件时都会触发此事件
### OnAttachmentRemoved
每次移除注册的附件时都会触发此事件
### OnAttachmentDestroyed
每次销毁注册的附件时都会触发此事件
### OnAttachmentChanged
每次更改注册的附件时都会触发此事件
### OnOffsetChanged
每次附件偏移更改时都会触发此事件。