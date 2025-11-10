# Overview
This component is what gets added to your pawn and allows you to easily interface with the rest of the system. It is where you Set and Get the held actor that is to be used with the rest of the system and has helper functions to get access to the current held actor's SKGFirearmComponent, SKGProceduralAnimComponent, SKGOpticComponent, etc.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/190b3cc7-3f61-4499-b577-571f487bfae0)

# Details
## Initialize:
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/8bd3aca1-c5d2-4fd5-84ff-153033d9d7fb)
### Linked Anim Layer Class
This is the class to use for the procedurals that inherits from my anim instance. This is useful if you want to utilize the linked anim layer setup for a cleaner anim graph.
### Auto Setup Linked Anim Layer
If true, the [Linked Anim Layer Class](#linked-anim-layer-class) will be automatically setup.
### Use Single Mesh:
If true, Third Person Mesh Component Name or First Person Mesh Component Name will be used (whichever one it finds) rather than both and treats them as the same.
### Third Person Mesh Component Name:
This is the name of your third person mesh on your pawn. In the ACharacter class this defaults to CharacterMesh0.
### First Person Mesh Component Name:
This is the name of your first person mesh on your pawn. You do not need a first person mesh if you are doing true first person.
### Camera Component Name:
The name of your camera component on the pawn.
### Camera Attached Socket:
The name of the socket that your camera gets attached to.
### Replicate Remote Yaw:
Since unreal by default does not replicate your yaw, I have provided a system that lets you get a replicated control rotation (pitch and yaw are compressed into a Byte and uncompressed when received).
### Remote Yaw Replication Rate:
The rate at which the Yaw will be replicated. It only gets replicated when the mouse moves via the SetMouseInput function (also used for deadzone).
### Night Vision Material Parameter Collection:
Usage covered in the Night Vision section of the Wiki.

## Free Look:
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/535bfdc8-df26-47ab-b684-242c3278b8a0)
### Auto Set Use Controller Rotation Yaw:
If true, this will auto set whether or not your pawn has bUseControllerRotationYaw enabled.
### Max Pitch UP/Down/Left/Right:
How much you can rotate in the corresponding directions from center when free looking.
## Leaning - In progress:
### Max Lean Left/Right Angle:
How much you can lean to the corresponding angle.

## Leaning
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/963f22ac-b551-4f7a-bdb1-3c49b8234fc4)
### Use Spring Interp
If true, it will utilize spring interpolation for the to/from when leaning/unleaning.
### Spring Interp Settings
Google these as they are standard for all spring interpolation.
### Max Lean Left/Right Angle
This is the max angle you can lean left/right. The max is 45 due to compression, however you can stack this with more than 1 lean node in the anim graph to lean as far as you want.

## Debug
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c4ce8eda-2bf5-4bb1-b7a0-5da73c0ebd6d)
### Print Hit
This is specific to firearm collision, but it will print out the hit actor to the screen from the firearm collision trace.
### Draw Debug Trace
This is specific to firearm collision, but it will draw the debug trace of the firearm collision trace.


# Functions
## HeldActor Category
### SetHeldActor
This function is core to letting actors (such as firearms) interact with the system. When you pickup a firearm, SetHeldActor must be called.
### ClearHeldActor
This function is core to clearing the actors from use. For example when you drop a firearm, ClearHeldActor must be called.

## AnimationLayer Category
### LinkAnimLayerClass
This function will link the set [Linked Anim Layer Class](#linked-anim-layer-class) to the anim instance.
### LinkAnimLayerClassByInstance
This function will link the set [Linked Anim Layer Class](#linked-anim-layer-class) to the specific anim instance passed into the function.
### UnlinkAnimLayerClass
This function will unlink the set [Linked Anim Layer Class](#linked-anim-layer-class) from the anim instance.
### UnlinkAnimLayerClassByInstance
This function will unlink the set [Linked Anim Layer Class](#linked-anim-layer-class) from the specific anim instance passed into the function.

## FreeLook Category
### CanAddYaw - CanAddPitch
These functions check to see if you can apply more yaw/pitch when free looking or if you have hit the max limit.
### StartFreeLook
When called, this function will start the system for free looking.
### StopFreeLook
When called, this function will stop the system for free looking.
### IsFreeLooking
This function will return true if you are currently free looking, will return false otherwise.

## MouseInput Category
### SetMouseInput
This function is required for use with deadzone as it runs on the received moused input. Takes in the X and Y from the mouse input.

## Sensitivity Category
### GetMagnificationSensitivityMultiplier
This function will return the given multiplier for you to use with your input (X and Y) for the mouse. It lets your mouse sensitivity scale with the magnification/zoom amount.

## Leaning Category
### LeanLeft/Right
These functions take in a TargetAngle, this angle is the goal in which you want to lean to. This allows for incremental leaning.
### StopLeaningLeft/Right
These functions will stop the lean from the given direction.
### GetTargetLeanAngle
This function returns the current target lean angle (the angle your trying to lean).

## ProceduralAnimation Category
### StartAiming
This function will start aiming with the given held firearm or optic.
### StopAiming
This function will stop aiming with the given held firearm or optic.
### IsAiming
This function will return true if you are currently aiming, false if you are not.
### SetUseFirstPersonProceduralsAsLocal
This is the default. Basically it will make the set procedurals use the first person settings for the local client.
### SetUseThirdPersonProceduralsAsLocal
This function will make the set procedurals use the third person settings for the local client (useful for third person mode).
### SetSwayMultiplier
This function will change the amount of sway you have. This is useful for injuries and steady aim.
### ResetSwayMultiplier
This function will reset the sway multiplier back to it's default.
### GetUsingCustomSwayMultiplier
This will return true if you called SetSwayMultiplier without resetting it (using a value different than the default).
### PerformProceduralRecoil
This function will perform procedural recoil using the given settings from the [SKGProceduralAnimComponent](SKGProceduralAnimComponent) on the firearm.
### PerformProceduralPose
This function will trigger the pose (if found) given by the passed in gameplay tag. If bExitPose is true, it will perform the exit pose of that pose.
### PerformCustomCurve
This function will trigger a curve to play based on the passed in CurveData.
### SetOffHandIKToLeftHand
This function will change you from using right handed procedurals to using left handed procedurals.
### SetOffHandIKToRightHand
This function will change you from using left handed procedurals to using right handed procedurals.
### GetProceduralPoseData
This function will return the pose data (if found) from the passed in gameplay tag.

## Infrared Category
### EnableInfraredMode
This function will trigger the infrared mode from the [SKGShooterWorldSubsystem](SKGShooterWorldSubsystem) which will switch registered infrared devices (like the LightLaser) to infrared mode if they support it. It will also auto set the values in the [Material Parameter Collection](#night-vision-material-parameter-collection). Long story short, when you enable night vision just call this function.
### DisableInfraredMode
This function will trigger the infrared mode from the [SKGShooterWorldSubsystem](SKGShooterWorldSubsystem) which will switch registered infrared devices (like the LightLaser) to normal mode from infrared mode (if they are in it and if they support it). It will also auto set the values in the [Material Parameter Collection](#night-vision-material-parameter-collection). Long story short, when you disable night vision just call this function.

## Components Category
### GetCurrentFirearmComponent
This will return the currently held [SKGFirearmComponent](SKGFirearmComponent).
### GetCurrentProceduralAnimComponent
This will return the currently held (or from a firearm) [SKGProceduralAnimComponent](SKGProceduralAnimComponent).
### GetCurrentOpticComponent
This will return the currently held (or from a firearm) [SKGOpticComponent](SKGOpticComponent).
### GetCurrentLightLaserComponents
This will return the currently held (or from a firearm) [SKGLightLaserComponents](SKGLightLaserComponent).

## Misc Category
### IsLocallyControlled
This will return true if the owning pawn of this component is the local controller.
### IsUsingFirstPersonProceduralsAsLocal
This will return true if this component is set to use first person procedurals for their local client.
### GetControlRotation
This function will return the control rotation if you are the local controller, if you are remote it returns a custom replicated rotator that mimicks the control rotation.

## Getters
### GetProceduralShooterPawnData
This will return the current procedural data specific to the pawn for use in your own anim graph such as the current held actors ProceduralAnimGameplayTag, bOffHandIKIsLeftHand, and FreeLookLookAtRotation for freelook.
### GetCameraComponent
This function will return the stored camera component.
### GetPawnMesh
This function will return the [First Person Mesh](#first-person-mesh-component-name) if locally controlled, otherwise it will return the [Third Person Mesh](#third-person-mesh-component-name).
### GetOwningPawn
This function will return the owning pawn of the component.
### GetShooterFrameworkAnimInstance
This function will return the set [SKGShooterFrameworkAnimInstance](SKGShooterFrameworkAnimInstance).


# Events
### OnPoseComplete
This event fires when a pose has been completed (start to end or end to start).
### OnHeldActorSet
This event fires when the HeldActor is changed.