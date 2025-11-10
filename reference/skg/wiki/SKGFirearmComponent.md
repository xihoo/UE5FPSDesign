This component is exclusive to firearm usage and primarily acts as an interface between the other components. It gets paired with the SKGAttachmentManagerComponent as it binds to delegates on that component that fire off when attachments are added, removed, changed, etc and handles their setup so adding a SKGAttachmentManagerComponent in your class alongside the SKGFirearmComponent is mandatory.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/62c2c352-e449-4395-8893-565df81afa84)

# Details Panel
## Initialize:
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/0ef41886-e1b5-4233-8c06-2bdc326e646a)
### Firearm Mesh Component Name:
The mesh component (static or skeletal) of your firearm.
### Attachment Manager Component Name:
The name of your attachment manager component.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/906a7881-fb18-46ee-95d7-4800050cf442)

## FirearmCollision:
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6ffc7151-0079-4d4a-92d5-4351fb505e6b)
### Use Firearm Collision:
If true, the firearm collision system will be used for this particular firearm.
### Firearm Collision Trace Diameter:
A sphere trace is used for the collision detection, this specifies how large you want the sphere to be. Default is 2cm.
### Firearm Collision Stop Aiming Distance:
If you are aiming and start to collide with a wall, this is how far the collision can be pushed back before it forces you to stop aiming.
### Firearm Collision Pose Location/Rotation:
These are the curves used for the firearm collision. The default goes into short stock and then pushes the muzzle into low port.
### Firearm Collision Pose Scale:
This controls how fast the firearm collision goes through the curves (lower = slower).
### Firearm Collision Pose Location/Rotation Interp Speed:
How fast it interpolates from the current position to the target position found in the curve.


# Functions
## ProceduralStats Category
### SetProceduralStats
This function allows for the procedurals stats to be set in a single place. Unlikely to be used but it exists just incase.
### GetProceduralStats
This function returns the procedural stats utilized by the procedural animation system.

## Zero Category
### ZeroOpticsForZeroAtLocation
This function currently only works for optics with scene captures (future plans). It will iterate over them and zero them for the given location returned by the [SKGProjectileWorldSubsystem](SKGProjectileWorldSubsystem). It was written this way to allow for a more universal approach and support for multiple projectile systems.

## Muzzle Category
### ShotPerformed
This function will call ShotPerformed on each part of the firearm that contains a SKGMuzzleComponent. See the [SKGMuzzleComponent](SKGMuzzleComponent) for what the function does down stream (used for the muzzle temperature system).
### GetMuzzleProjectileSocketTransform
This function will return the transform needed to launch the projectile from the muzzle with the given zero and accuracy set.
### GetMuzzleProjectileTransforms
This function does the same as GetMuzzleProjectileSocketTransform but will return an array of transforms used to launch a projectile. Useful for shotguns.
### GetMuzzleTransform
This function returns the raw transform of the current muzzle components muzzle.

## Pose Category
### GetPose
This function returns true if the pose is found and output the found pose.

## Aim Category
### CycleAimingDevice
This function will switch [aiming devices](SKGProceduralAnimComponent) for what the function does down stream (used for the muzzle temperature system). For example if you have multiple red dots on your firearm, it will cycle through them one at a time for aiming as the function is called.
### StartPointAiming
This function will start using the point aiming socket setup in this component.
### StopPointAiming
This function will stop the point aiming and return you back to aiming with your normal [aiming devices](SKGProceduralAnimComponent).
### IsPointAiming
This function returns true if you are currently point aiming, otherwise it returns false.
### GetAimSocketWorldTransform
This function returns the current [aiming device](SKGProceduralAnimComponent) aim socket transform in world space.

## FirearmStats Category
### SetFirearmStats
This function will set the firearm stats directly. Likely unused but there just incase.

## Getters
### GetProceduralAnimComponents
This function returns all the currently found [SKGProceduralAnimComponents](SKGProceduralAnimComponent).
### GetOffHandIKComponents
This function returns all the currently found [SKGOffHandIKComponents](SKGOffHandIKComponent).
### GetLightLaserComponents
This function returns all the currently found [SKGLightLaserComponents](SKGLightLaserComponent).
### GetMuzzleComponents
This function returns all the currently found [SKGMuzzleComponents](SKGMuzzleComponent).
### GetAttachmentStatComponents
This function returns all the currently found [SKGFirearmAttachmentStatComponents](SKGProceduralAnimComponent).
### GetFirearmStats
This function returns the current stats of the firearm
### GetAttachmentManagerComponent
This function returns the current [SKGAttachmentManagercomponent](SKGAttachmentManagercomponent).
### GetCurrentProceduralAnimComponent
This function returns the current [SKGProceduralAnimComponent](SKGProceduralAnimComponent).
### GetCurrentOffHandIKComponent
This function returns the current [SKGOffHandIKComponent](SKGOffHandIKComponent).
### GetCurrentMuzzleComponent
This function returns the current [SKGMuzzleComponent](SKGMuzzleComponent).
### GetCurrentOpticComponent
This function returns the current [SKGOpticComponent](SKGOpticComponent).
### GetCurrentStockComponent
This function returns the current [SKGStockComponent](SKGStockComponent).


# Events - Subject To Change
### OnAimingDeviceCycles
This event fires whenever your [aiming devices](SKGProceduralAnimComponent) changes.