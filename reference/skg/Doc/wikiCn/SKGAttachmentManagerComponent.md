该组件的目的是提供便捷的事件和快速访问您的[SKGAttachmentComponents](SKGAttachmentComponent)。如果您使用[SKGFirearmComponent](SKGFirearmComponent)，这是必需的，否则是可选的。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/5cefa93a-4456-47cb-bdc9-3484a1440436)

# 详细面板
### 管理器名称
这是一个可选字段。在示例中，它用于与自定义器一起保存，因为它将创建一个具有此给定名称的文件夹来存放保存的预设。
### 允许客户端修改
如果为true，客户端可以调用修改附件的函数，如[HideAllAttachments](#hide-all-attachments)。如果为false且客户端试图作弊并调用RPC，客户端将自动断开连接。

# 函数
## 附件类别
### GetAttachmentComponents
此函数将返回管理器中当前缓存的所有附件组件。
### GetAttachments
此函数将返回管理器中缓存的所有附件（Actor）。
### GetCompatibleAttachmentComponentsFromActor
此函数将输出管理器中当前缓存的与传入Actor兼容的所有兼容附件组件。如果至少找到一个结果，将返回true。
### GetCompatibleAttachmentComponentsFromClass
此函数将输出管理器中当前缓存的与传入Actor类兼容的所有兼容附件组件。如果至少找到一个结果，将返回true。
### GetAttachmentComponentWithAttachment
此函数将返回包含传入附件的附件组件。

## 附件状态类别
### HideAllAttachments
此函数将隐藏所有当前存在的附件，包括新添加的附件。
### UnhideAllAttachments
此函数将取消隐藏所有当前存在的附件，包括新添加的附件。
### DisableCollisionAllAttachments
此函数将禁用所有当前存在的附件的碰撞，包括新添加的附件。
### EnableCollisionAllAttachments
此函数将启用所有当前存在的附件的碰撞，包括新添加的附件。

## 获取器
### GetManagerName
此函数返回在详细面板中定义的[管理器名称](#manager-name)。

## 事件
### OnAttachmentComponentsChanged

### OnAttachmentComponentAdded
当从管理器添加/注册附件组件时，此事件将触发。
### OnAttachmentComponentRemoved
当从管理器移除/注销附件组件时，此事件将触发。
### OnAttachmentComponentAttachmentAdded
当向[SKGAttachmentComponent](SKGAttachmentComponent)添加附件时，此事件将触发。
### OnAttachmentComponentAttachmentRemoved
当从[SKGAttachmentComponent](SKGAttachmentComponent)移除附件时，此事件将触发。