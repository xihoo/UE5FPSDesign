The purpose of this component is to allow for an easy way to manage an attachment on an individual level. It ties directly with the attachment manager (which is optional) as they can work hand in hand allowing a easy to use, yet robust attachment system.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/3a31f72f-e248-4d6b-8c2c-6623a4dfd3f6)

# Details Panel
## Initialize
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/04eb221b-4492-4951-bfd7-0b44ff2f72e9)
### Attach To Mesh Name
This is the name of the mesh component for the attachment to be attached to. In the case of this example with the sight, it is the SkeletalMesh for the M4.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/f66d6a85-3174-46e2-97a2-fa696ae68767)
### Attach To Socket
This is the name of the socket on the mesh for the attachment to attach to. In the case of this example with the sight, it is the S_Attach_Optic on the M4's mesh.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/99029c8f-f7c9-4904-9b3a-99e47ab78283)
### Component Name
This is an optional section, it is useful for making customization menus.
### Component Image
This is an optional section, it is useful for making customization menus.

## Attachment
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/b8b68245-7437-4531-91e4-615ffa0adce6)
### Is Required To Function
This is used for customization menus. In the example I use it for essential parts such as the barrel and handguard so they cannot be removed in the customization menu.
### Auto Set Leader Pose Component
This is useful if you are using the attachment system for characters. When the attachment is attached (lets assume a shirt), it will automatically set the leader pose component for you with the Leader Pose Attachment Mesh Name.
### Leader Pose Attachment Mesh Name
This is the name of the mesh component to be used with the above Auto Set Leader Pose Component setting.
### Random Default Attachment
If this is true, a random attachment will be used from the [Compatible Attachments](#compatible-attachments) and setup upon spawn.
### Default Attachment
If set, this will spawn the specified attachment automatically upon spawn only if the attachment is compatible based on the setting below.
### Compatible Attachments
This is an array of data assets. In each data asset you define what attachments are allowed in each asset, then you can use them in combination with each other. Leave this empty if you want all attachments to be compatible.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/f6cd751c-a5e2-4a1c-a7b2-1c1ec19c55a4)

### Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/d58ba305-5866-4a14-8194-70379b479435)
#### Allow Client Side Modification
If this is true, clients can add/remove attachments. If it is false and a cheater attempts to do it (such as calling the RPC directly) it will disconnect the client.

#### Offset Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/dee00f70-3f72-4c3a-bec2-2e783675cbbe)
##### Replicate Offset Each Change
If true, each time an offset is applied to the attachment it will replicate. I recommend leaving this off. If it is off, [FinalizeAttachmentOffset](#finalizeattachmentoffset) will need to be called to replicate.
##### Offset Axis
This setting allows for you to define the direction you wish to move the attachment in the XYZ.
##### Offset Snap Distance
This setting is useful for rail sections. For example, if you move an optic up/down a rail you cannot move it freely because picatinny slots are spaced 1.046cm apart.
##### Minimum Offset Allowed
This is how far in the reverse direction the attachment is allowed to move. For example by default a sight might be all the way back, so this value would be 0 so it can only be moved forwards, but a collapsible stock like in the example can be moved back so it will have a value of -4.
##### Maximum Offset Allowed
This is how far in the forward direction an attachment is allowed to move. For example a sight moving down the rail.

#### Attachment Rules
These are the rules that are applied when AttachToComponent is called. Default values here are sufficient but still customizable incase you need something different.


# Functions
## Attachment Category
### TryLoadSetupAttachment
This function will load the passed in soft pointer to a class asynchronously. Once loaded it will try to spawn, setup, and attach the attachment.
### TrySetupAttachment
This function will try to spawn, setup, and attach the attachment from the passed in class.
### TrySetupExistingAttachment
This function will try to setup and attach the passed in attachment.
### TrySetupExistingAttachmentNoAttach
This function will try to setup the passed in attachment without attaching it. This is useful if you are using other systems that handle the attaching already.
### RemoveAttachment
This function will remove the attachment from the system, but it will not detach or destroy it. This is useful if you are going to be dropping this attachment to the ground or adding it to an inventory or just passing it along to another system.
### DestroyAttachment
This function will remove the attachment from the system and destroy it.
### HasAttachment
This function will return true if it holds a valid attachment registered with the system.

## Compatibility Category
### GetCompatibleAttachments
This function will return the created list of compatible attachments defined in [Compatible Attachments](#compatible-attachments).
### IsAttachmentClassCompatible
This function is overridable. By default it will return true if the passed in class is compatible with the attachments defined in [Compatible Attachments](#compatible-attachments).
### IsAttachmentCompatible
This function is overridable. By default it will return true if the passed in attachment is compatible with the attachments defined in [Compatible Attachments](#compatible-attachments).

## Preview Category
### SetPreviewAttachment
This function will create a locally spawned preview of the given attachment class you pass in. If there is an existing attachment it will simply hide it to only display the preview.
### DestroyPreviewAttachment
This function will destroy the spawned preview attachment. If there is an existing attachment it will unhide it so everything is back to normal.

## Offset Category
### FinalizeAttachmentOffset
This function is used when [Replicate Offset Each Change](#replicate-offset-each-change) is set to false. This must be called to replicate the offset.
### SetAttachmentOffset
This function will set the offset for the attachment to the passed in value. It is clamped between [Minimum Offset Allowed](#minimum-offset-allowed) and [Maximum Offset Allowed](#maximum-offset-allowed) and utilizes the [Offset Snap Distance](#offset-snap-distance) to jump to the nearest snap point if defined.
### IncrementOffset
This function will increment the offset by the [Offset Snap Distance](#offset-snap-distance). It is clamped between [Minimum Offset Allowed](#minimum-offset-allowed) and [Maximum Offset Allowed](#maximum-offset-allowed).
### DecrementOffset
This function will decrement the offset by the [Offset Snap Distance](#offset-snap-distance). It is clamped between [Minimum Offset Allowed](#minimum-offset-allowed) and [Maximum Offset Allowed](#maximum-offset-allowed).
### CanAttachmentBeOffset
This function returns true if an offset can be applied (if [Minimum Offset Allowed](#minimum-offset-allowed) and [Maximum Offset Allowed](#maximum-offset-allowed) are not equal).
### GetAttachOffsetTransform
This function returns the offset world transform of the attachment (where the [Attach To Socket](#attach-to-socket) with the offset applied).
### GetAttachTransform
This function returns the world transform of the attachment attach point (where the [Attach To Socket](#attach-to-socket) with the offset applied).
### GetAttachStartTransform
This function returns the start offset defined by [Minimum Offset Allowed](#minimum-offset-allowed) world transform of the attachment (where the [Attach To Socket](#attach-to-socket) with the [Minimum Offset Allowed](#minimum-offset-allowed) applied).
### GetAttachEndTransform
This function returns the start offset defined by [Maximum Offset Allowed](#maximum-offset-allowed) world transform of the attachment (where the [Attach To Socket](#attach-to-socket) with the [Maximum Offset Allowed](#maximum-offset-allowed) applied).
### GetAttachSnapPoints
This function returns an array of all the possible snap points defined by the range of [Minimum Offset Allowed](#minimum-offset-allowed) and [Maximum Offset Allowed](#maximum-offset-allowed) spaced by [Offset Snap Distance](#offset-snap-distance).

## Getters
### GetAttachToSocket
This function returns the [Attach To Socket](#attach-to-socket).
### GetComponentName
This function returns the [Component Name](#component-name).
### GetComponentImage
This function returns the [Component Image](#component-image).
### GetDefaultAttachment
This function returns the [Default Attachment](#default-attachment).
### GetAttachToMesh
This function returns the [Attach To Mesh Name](#attach-to-mesh-name).
### GetAttachment
This function returns the currently registered attachment.
### GetAttachmentOffset
This function returns the current attachment offset.
### GetIsRequiredToFunction
This function returns whether or not the attachment component is specified to be a required attachment to function.
### GetOffsetSettings
This function returns the [Offset Settings](#offset-settings).

## Events
### OnAttachmentAdded
This event will fire each time a attachment is registered
### OnAttachmentRemoved
This event will fire each time the registered attachment is removed
### OnAttachmentDestroyed
This event will fire each time the registered attachment is destroyed
### OnAttachmentChanged
This event will fire each time the registered attachment is changed
### OnOffsetChanged
This event will fire each time the Attachment Offset is changed.