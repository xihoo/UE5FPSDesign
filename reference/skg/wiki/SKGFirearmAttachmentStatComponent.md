The purpose of this component is to allow for a generic way of handling stats that is non restrictive. This component is intended to be extended by blueprint and C++ devs.

# Details Panel
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6cb4e371-8ae9-4c00-9bd5-03f55ca33e38)
## Initialize
### Attachment Stats Data Asset
This is the data asset to be used that defines the stats.

# Data Asset
By default, this data asset contains values for Weight, Ergonomics, Accuracy, Vertical Recoil, Horizontal Recoil, Muzzle Velocity Percentage, and Fire Rate Change Percentage.\
The reason a data asset was used is so it can be extended in blueprint because C++ structs cannot be extended in blueprint.

# Functions
### GetAttachmentStats
This function simply returns the set [Data Asset](#attachment-stats-data-asset) of the component.

# How To Use
## Overrideable Functions
### AddFirearmAttachmentStats
This function gets called each time an SKGFirearmAttachmentStatComponent is added. The implementation by default adds the provided stats together so if you still want that functionality but also want to extend it, make sure to call the parent function/Super.
### RemoveFirearmAttachmentStats
This function gets called each time an SKGFirearmAttachmentStatComponent is removed. The implementation by default subtracts the provided stats so if you still want that functionality but also want to extend it, make sure to call the parent function/Super.
### CalculateProceduralValues
This function is where the freedom is. It gets called each time the firearm stats replicate so the calculation is done locally so you can affect it differently for local and remote clients if you wish. In here you define how each stat affects each procedural system. By default it simply uses the default values set in the details panel of the [SKGProceduralAnimComponent](SKGProceduralAnimComponent).