This component serves several purposes. For firearms, it allows for various offsets for third person and slight adjustments to your idle animation, aiming, movement sway, movement and rotation lag, deadzone, recoil, procedural poses, point aiming, and a few more small things. When added to attachments such as optics or lasers, it is used for aiming. The settings have many settings for Spring Interpolation for Stiffness, Critical Dampening Factor, Mass, and Target Velocity Amount. If you are unsure what these are for, google them as the settings are exposed for all spring interpolation procedurals.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/04b98bd9-7407-407d-ae98-66c205c40cb3)

# Details Panel
## Initialize
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/ea4e7d46-44ff-4b51-a07f-14682dfb0fd0)
### Procedural Mesh Name
This is the name of the mesh that the component finds and stores for use in calculating information such as the aiming. This would be the static or skeletal mesh component of your firearm, optic, laser, etc.
### Procedural Aim Socket Names
These are the sockets to be used for aiming. For an optic where you want a single aim point, you would create a socket with the name X and position it in the center of the glass. Make sure the socket name matches that in the Procedural Aim Socket Names array. You can have as many of these as you wish. In the example below with this Elcan, you would have two, one for the glass and one for the iron sights on top of the body.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/bd3dbcb5-34ec-4cc6-b870-30bcc14dacc1)
### Right Hand Dominate Point Aim Aim Socket Index
This is the socket to be used for point aiming with firearms. This is useful for the component on a firearm. To use this, make a socket on the firearm mesh and name it X, offset this and rotate it around (such as canting it by 45 degrees) and put that socket name X in the Procedural Anim Socket Names array. Set the index to match whichever index that socket name X is in the array.
### Left Hand Dominate Point Aim Aim Socket Index
Same as the above, used when you switch hands (going right to left handed).
### Procedural Anim Gameplay Tag
This is used with my system in the anim graph, but it is optional. This is used as an identifier of "what am I holding" in the anim graph in the Blend Poses by Gameplay Tag node at the start. You do not need to use this if you have your own system in place.
### Gameplay Tags
Each component comes with the gameplay tag interface already setup in the case that you might want to utilize one of the components with GAS (Gameplay Ability System).

## Procedural
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c53ebfd4-d5d9-467a-b4c4-fe97a9ca3fda)
### Aiming Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/45030324-737a-4527-b6f2-375d9d0f3a9e)
#### Forward Axis
This is the axis your firearm/optic/etc is. The provided example firearms are Y forward similar to Lyra so Y is default, if your firearm is X forward then you would change this to X.
#### Aiming Distance
This is how close/far you want to be from the aiming point (such as being closer or further from the optic when you aim).
#### Use Fixed Aiming Distance
If this is true, when you aim your camera will be a specific distance (defined in Aiming Distance) from the actor. For example, lets say you have an optic at the rear of the firearm and your Aiming Distance is set to 10. Your camera will be 10cm away from the aiming point of the optic. Now lets say you move the optic to the front of the firearm, such as at the end of the handguard. Your camera will still be 10cm away from the aiming point of the optic. If this was false, you can move your firearm up/down the rail however much you want and your position wont change.
#### Can Aim
If true, you can aim with the actor, if false you cannot. This is toggleable

### Base Pose offset
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/abd1eb6f-c024-4731-ad88-e8964842fa6a)
This is used to modify your idle pose. If you do not have the ability to adjust your idle animation, you can use this option to alter it a bit
#### First Person offset
This is the offset that will only be applied to the local client.
#### Third Person Offset
This is the offset that will only be applied to remote clients or the local client if the UseThirdPerson is set in the SKGShooterPawnComponent.

#### Third Person Right Handed Aiming Offset
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/35bec8b5-fb78-4bad-889e-2fe412bc2169)
This works very much like the Base Pose Offset for the Third Person Offset. For remote clients or a local client using third person mode, it will adjust the position of the firearm when you aim so you can get it to line up how you want for third person view.
#### Third Person Left Handed Aiming Offset
Same as the above but for when you change to left handed mode.

### Cycle Aiming Point Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/0c7ad4e8-5b80-4d4c-af6d-bdd3185643b7)
#### Use Spring Interp For Cycling Aiming Point
If true, it will use spring interpolation for cycling the aiming point. By cycling aiming points I mean your aim changing between different actors (such as optics) or aiming points on the same actor. If false it will use standard linear interpolation similar to that found in games like Tarkov.
#### Spring Interp Settings
Google these as they are standard for all spring interpolation.

### Movement Sway Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c6696f27-d3fa-4eb7-ae18-ef78f6e7a803)
#### Use Movement Sway
If true, the system will apply/handle movement sway. If false it will be bypassed.
#### Movement Location/Rotation Settings
##### Multiplier
This is how much the curve is scaled. Higher = more sway.
##### Multiplier Interpolation Rate
How fast to interpolate between different Movement Sway Multipliers.
##### Curve
The curve used to define the sway itself.
##### Interp Speed
Interpolate speed in which it interpolates from its current position in the curve to the target (next) position in the curve.
#### Movement Rotation Settings
Same as the above, just for rotation instead of location.

### Movement Lag Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/aef1274a-3978-4c16-b613-f9897aed65a3)
#### Use Movement lag
If true, the system will apply/handle movement lag.
#### Use Spring Interpolation
If true, it will use spring interpolation rather than linear interpolation.
#### Spring Interp Settings
Google these as they are standard for all spring interpolation.
#### Left Right Max
The max the procedurals will move left/right.
#### Up Down Max
The max the procedurals will move up/down.
#### Multiplier
How much additionally to add to the movement lag calculation (higher values = more movement).
#### Interp Speed
How fast to interpolate to the target movement lag position (more = faster), not for Spring Interpolation.
### Rotation Lag Settings
The same as the above but for rotation.

### Deadzone Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/b5447843-16de-45f4-8cf5-260d00691a94)
#### Use Deadzone
If true, the system will apply/handle deadzone.
#### Pitch Rate
The speed at which the pitch (up/down) will be added to the deadzone.
#### Yaw Rate
The speed at which the yaw (left/right) will be added to the deadzone.
#### Pitch Limit
The limit of pitch (up/down) the deadzone maxes out at.
#### Yaw Limit
The limit of yaw (left/right) the deadzone maxes out at.
#### Interpolation Speed
Speed at which we interpolate from the current deadzone position to the next (smaller = slower which can make it lag behind your mouse a bit if you wish for that effect).
#### Disable Deadzone When Aiming
If true, deadzone will be disable when you aim, and active when you are not aiming.
#### Deadzone Aiming Disable Interpolation
This is used in conjunction with Disable Deadzone When Aiming. This is the interpolation rate for going from not aiming from the hip with deadzone to aiming to the center without deadzone.

### Recoil Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/b3afcea0-54a2-483b-8d2c-0e7e8a827e82)
#### Control/Location/Rotation Curve:
These are the curves used to drive the recoil as recoil will follow them. Control is for your controllers rotation, Location and Rotation are for the firearm.
#### Control/Location/Rotation Random:
These are values to randomize the recoil from a min/max (-5 and 5 on the yaw will mean -5 left and 5 right, somewhere in-between will be selected).
#### Location Interp To None Speed
The speed at which the location recoil is interpolated back to 0.
#### Rotation Interp To None Speed
Same as the above but for rotation.

### Pose Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/1364b2be-d46d-46b5-99dd-809efe5a0e98) \
This is an array of custom poses. More can be read about them on [this page](Creating-a-custom-pose) of the wiki.


# Functions
## Aim Category
### GetProceduralAimingSettings
This function returns the current aiming settings used for this component.
### CanAim
This function returns true based on whether or not the component [can aim](#can-aim), false if not aimable.
### SetCanAim
This function sets whether or not this procedural anim component is aimable.
### GetAimWorldTransform
This function returns the aim socket transform in world space.
### CycleAimSocket
This function will cycle which aim socket is used on the optic. For example, an Elcan would have two aim points, one through the optic and one ontop of the housing (the build in iron sights) so you would utilize the [Procedural Aim Socket Names](#procedural-aim-socket-names) and this function allows you to cycle between them.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/e352587c-40e4-4d7e-ae34-820ba919e09e)
### StartPointAiming
This function will start using the point aiming socket on the component.
### StopPointAiming
This function will stop using the point aiming socket on the component and switch back to the normal aim sockets.
### UpdateAimOffset
This function is called automatically by the [SKGShooterPawnComponent](SKGShooterPawnComponent) and [SKGFirearmComponent](SKGFirearmComponent) to set and update the aim socket for use with the procedurals.

## Pose Category
### GetPose
This function returns true if the pose is found and output the found pose based on the given tag.

## Getters
### GetAimOffset
This function returns the aim offset.
### GetProceduralAnimMesh
This function returns the mesh used with the system.