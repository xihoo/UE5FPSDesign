This component is basically a LeftHandIKComponent, but since it supports left and right hand it is called OffHandIKComponent. It is meant to handle the offhand positioning and pose. This component in the example is added to the firearm, handguard, and forward grips.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/cce86834-9263-47ec-b73e-9eff69849ad4)

# Details Panel
## Initialize
### Off Hand IKMesh name:
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6eff0306-fca3-45b1-93f5-f4e1967d17d7)
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/b8d6b2d9-717f-48dd-8783-d173d4e253e3)
This is the name of the mesh that it reads the socket from, such as the firearm mesh, handguard's mesh, and forward grip's mesh component names as shown in the above images.
### Left/Right Hand IKSocket Name
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/d067558a-920b-456f-a198-fbd3f86b3b53)
This is the name of the socket to use for the offhand to be positioned to.
### Left/Right Hand IKPose
This is the pose to play for the offhand. Example of use is forward grips. You likely want a different pose on the offhand for vertical grips, no grip, and angled grips.
### Off Hand IKTag
This is used for order. For example if you have a Firearm, Handguard (as attachment), and Forward grip (as attachment), it will set it up so you only use the forward grip for the offhand ik, if you remove the forward grip it will use the handguard, if you remove the handguard it will use the firearm.
### Gameplay Tags
Each component comes with the gameplay tag interface already setup in the case that you might want to utilize one of the components with GAS (Gameplay Ability System).


# Functions
### UpdateOffHandIK
This is called automatically from the SKGFirearmComponent and is used for calculating the relative offset from the ComponentRelativeTo to the offset socket.
### GetOffHandIKOffset
This will return the calculated offset from [UpdateOffHandIK](#updateoffhandik)
### GetOffHandIKPose
This will return the pose set for the component. If in left hand mode it will return the left hand pose, if in right hand mode it will use the right hand pose.
### GetOffHandIKMesh
This will return the mesh used for the system, such as a handguard, forward grip, firearm, etc.