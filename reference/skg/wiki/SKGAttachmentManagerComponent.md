The purpose of this component is to allow for convenient events and fast easy access to your [SKGAttachmentComponents](SKGAttachmentComponent). It is mandatory if you are using the [SKGFirearmComponent](SKGFirearmComponent), but optional otherwise.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/5cefa93a-4456-47cb-bdc9-3484a1440436)

# Details Panel
### Manager Name
This is an optional field. In the example it is used for saving with the customizer as it will create a folder of this given name to put the saved presets in.
### Allow Client Side Modification
If this is true, a client can call functions that will modify attachments such as [HideAllAttachments](#hide-all-attachments). If it is false and a client tries to cheat and call the RPC, the client will automatically get disconnected.

# Functions
## Attachments Category
### GetAttachmentComponents
This function will return all the attachment components currently cached in the manager.
### GetAttachments
This function will return all the attachments (actors) cached in the manager.
### GetCompatibleAttachmentComponentsFromActor
This function will pass out all compatible attachment components that are currently cached in the manager that are compatible with the passed in Actor. It will return true if at least one result is found.
### GetCompatibleAttachmentComponentsFromClass
This function will pass out all compatible attachment components that are currently cached in the manager that are compatible with the passed in Actor Class. It will return true if at least one result is found.
### GetAttachmentComponentWithAttachment
This function will return an attachment component that contains the passed in attachment.

## AttachmentState Category
### HideAllAttachments
This function will hide all the currently existing attachments including newly added attachments.
### UnhideAllAttachments
This function will unhide all the currently existing attachments including newly added attachments.
### DisableCollisionAllAttachments
This function will disable collision for all the currently existing attachments including newly added attachments.
### EnableCollisionAllAttachments
This function will enable collision for all the currently existing attachments including newly added attachments.

## Getters
### GetManagerName
This function returns the [Manager Name](#manager-name) defined in the details panel.

## Events
### OnAttachmentComponentsChanged

### OnAttachmentComponentAdded
This event will fire off when an attachment component is added/registered from the manager.
### OnAttachmentComponentRemoved
This event will fire off when an attachment component is removed/unregistered from the manager.
### OnAttachmentComponentAttachmentAdded
This event will fire off when an attachment is added to an [SKGAttachmentComponent](SKGAttachmentComponent).
### OnAttachmentComponentAttachmentRemoved
This event will fire off when an attachment is removed from an [SKGAttachmentComponent](SKGAttachmentComponent).