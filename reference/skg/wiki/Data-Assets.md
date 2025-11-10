The purpose of the implementation of data assets is to make adjustments easier, allow for more reusability, and reduce the need for blueprint objects. For example, lets say we had a firearm (M4) class that a designer wanted to tweak the aiming settings on so when you ADS, the firearm is moved closer. Instead of having to check out/modify the actual M4 firearm bp class, the designer can simply open the data asset for the aiming settings used on the M4, make the adjustment, and save. Likewise when constructing actors from data (not using blueprint classes for everything), it is much easier to set a data asset on a component such as the SKGProceduralAnimComponent as you can just give it the SKGPDAAimingSettings instead of going through each property and manually setting it one by one.

# Procedurals
## Initialize Data Assets
### SKGPDAOffhandIKInitialize
<img width="347" height="182" alt="image" src="https://github.com/user-attachments/assets/77a490ec-f3a2-4fcd-9a47-5af96694c85f" /><br>
- OffhandIKMeshName: This is the name of the mesh component that will contain the Left and Right HandIKSocketName, such as a firearm, handguard, forward grip, etc.
- LeftHandIKSocketName: This is the name of the left hand ik socket on the mesh defined in OffhandIKMeshName. If you are in the typical right hand dominate (right handed) this is the socket that will be used. If you call SetOffhandIKToLeftHand (this is the default) it will use the this socket.
- RightHandIKSocketName: This is the name of the right hand ik socket on the mesh defined in OffhandIKMeshName. If you are in the left hand dominate (left handed) mode then this is the socket that will be used. If you call SetOffhandIKToRightHand it will use this socket.

### SKGPDAProceduralAnimInitialize
<img width="581" height="407" alt="image" src="https://github.com/user-attachments/assets/70546c67-93cc-44e6-9a52-9b1b66338f58" /><br>
<img width="796" height="707" alt="image" src="https://github.com/user-attachments/assets/411f7d14-8f9d-40f9-82b1-263caee1768e" /><br>
- ProceduralMeshName: This is the name of the mesh component that will contain the ProceduralAimSocketNames such as a Firearm mesh or Optic mesh.
- ProceduralAimSocketNames: These are the aim sockets. For most games, a single aim socket is sufficient, but you can have several. For example, the Elcan optic has built in iron sights on the top of the housing, so it can have 2. Look at the reference to the above image, that optic has 3 so I added 3 aim sockets that the player can cycle between while playing.
- RightHandDominatePointAimAimSocketIndex: This is typically only going to be on firearms. If you want to point aim (typically represented in games by rolling the firearm and not aiming with the sights) while in right handed mode, this is the socket that gets used.
- LeftHandDominatePointAimAimSocketIndex: Same as the above but for left handed mode.
- CanUseLeftHandDominate: Whether or not you can use left handed mode (again this is intended to be used solely on whatever actor your holding) when trying to switch.
- ProceduralAnimGameplayTag: This is useful if you are trying to setup your anim graph to blend by gameplay tag for an idle animation. For example, if you had an M4 and a SKS, the M4's SKGDAProceduralAnimInitializeSettings will have the M4 tag here, the SKS would have the SKS tag. Then in your anim graph, you can simply fetch the tag from the procedural anim component of whatever your holding and blend with it. This is optional.
- GameplayTags: Tags you want this data asset to have. No use by default.

## Settings
### SKGPDAAimingSettings
<img width="384" height="336" alt="image" src="https://github.com/user-attachments/assets/b659e9d0-b80f-462c-94b7-d712ba735842" /><br>
- ForwardAxis: This is the forward axis of the mesh you are using. Epic states that X is forward in the engine, but they produce content in their examples with Y forward meshes. The meshes provided in this example project are Y forward, so the Forward Axis is set to Y.
- AimingDistance: This is the default aiming distance you want to use. For example, if this is on a firearm with a value of 8, the aim point will be 8 units in front of the root of the firearm (in the example m4, the root is at the pistol grip, so 8 units in front of that is how far away the firearm will be. This is hard to describe so I recommend playing around with it). If you add an optic with an AimingDistance of 5, this will be added to 13 units in front of the root.
- bUseFixedAimingDistance: Following the above scenario, if you enabled this on your optic, then regardless of where the optic is on your firearm and whatever AimingDistance settings were set on the firearm, your optics aim socket will be 5 units in front of your camera (assuming AimingDistance is set to 5).
- DefaultAimingSpeed: This is read from the SKGProceduralAnimComponent on whatever you are holding, such as a firearm. It does not get overridden by any attachments on the firearm by default. To have attachments affect your aiming speed, you will want to incorporate the provided stats system.
- bAllowCameraZoom: If this is true, when you aim your camera will zoom in by CameraZoomPercentage percent at the CameraZoomInterpSpeed.
- CameraZoomPercentage: How much the camera should zoom in while aiming.
- CameraZoomInterpSpeed: How fast the camera should zoom in/out when aiming.
- bCanAim: Whether or not this procedural anim component can be used to aim.

### SKGPDACycleAimingPointSettings
<img width="450" height="400" alt="image" src="https://github.com/user-attachments/assets/c9799145-d104-4424-8f2c-dc1d1a41d520" /><br>
These are the settings that apply when you cycle between aiming points. This can be going to/from point aiming, to/from different optics, or to/from different aiming sockets on the same optic.

### SKGPDADeadzoneSettings
<img width="443" height="325" alt="image" src="https://github.com/user-attachments/assets/ab4ee21a-d904-4e4b-97fa-a4e032f16cce" /><br>
- bUseDeadzone: Whether or not to use deadzone. You can also not add this data asset and achieve the same effect.
- PitchRate: This is the rate at which the deadzone pitch (up/down) will move in relation to your camera. Higher = faster.
- YawRate: Same as the above but left/right.
- PitchLimit: This is the limit of how high/low what your holding can move in relation to your camera. Higher = more.
- YawLimit: Same as the above but left/right.
- InterpolationSpeed: This is how fast what your holding will move to the new point. Higher = faster, lower = more smoothed out and slower.
- bDisableDeadzoneWhileAiming: If true, deadzone will be disabled when aiming.
- bAlwaysInterpolateBackToCenter: If true, deadzone will interpolate itself back to center when you are not moving your mouse.
- InterpolateBackToCenterSpeed: This is the speed at which deadzone will be interpolating back to center. Higher = move to center faster.
- DeadzoneAimingDisableInterpolationSpeed: This is the same as the above, but only applies when bDisableDeadzoneWhenAiming is true. When you aim, this is the speed at which the aim point interpolates back to the center of the screen.

### SKGPDAMovementLagSettings
<img width="708" height="500" alt="image" src="https://github.com/user-attachments/assets/244cec9d-e446-4b6b-ac15-cbf34bd46e99" /><br>
- bUseMovementLag: Whether or not we want to use movement lag. Can get the same effect by not setting this data asset.
- bDisableForwardMovementLagWhenAiming: If true, the pitch of the firearm (up/down) will not happen when moving forward/backward when aiming.
- bUseSpringInterpolation: If true, the movement lag will use spring interpolation, if false it will use normal interpolation (InterpSpeed).
- ForwardBackwardNormalizedMax: The smaller the value, the more the pitch of the firearm will change as you move forward/backward.
- LeftRightNormalizedMax: Same as the above but with left/right movement and it affects the roll of the firearm.
- UpDownNormalizedMax: Same as the above but with up/down movement such as crouching or jumping.
- ForwardBackwardMultiplier: This is the multiplier for forward/backward movement. Setting it to a negative inverts the direction.
- Multiplier: Same as the above for for the Pitch/Yaw/Roll.
- InterpSpeed: This is the interpolation speed for when bUseSpringInterpolation is set to false.

### SKGPDAMovementSwaySettings
<img width="520" height="462" alt="image" src="https://github.com/user-attachments/assets/fbca8093-3171-494e-b383-567af54df60f" /><br>
Location and Rotation settings are the same, one is for held actor location movement, one is for held actor rotation.
- bUseMovementSway: Whether or not we want to use movement sway. Can get the same effect by not setting this data asset.
- Multiplier: This is how much to multiply the Curve's values by so you can get different amounts of movement from the same curves.
- MultiplierInterpolationRate: You can control the multplier at runtime by calling SetMovementSwayMultiplier on the SKGShooterPawnComponent that your pawn has, likewise this can be reset to the default by calling ResetMovementSwayMultiplier. The MultiplierInterpolationRate is what lets the change in this multiplier be smooth.
- Curve: This is the curve used for the XYZ.
- InterpSpeed: This is for interpolating through the curve. Useful if you wish to switch curves at runtime to help blend it out.

### SKGPDAOffhandIKSettings
<img width="406" height="211" alt="image" src="https://github.com/user-attachments/assets/8344d6bb-53fa-4c33-ace9-40a0ee8e208b" /><br>
- LeftHandIKPose: This is the pose that will be applied to the left hand. For example, a handguard would have a handguard pose, a forward grip would have a forward grip pose. The active SKGOffhandIKComponent determines what grip pose is to be used.
- RightHandIKPose: Same as the above but used when we are in left hand dominate (left handed) mode.

### SKGPDAProceduralOffsets
<img width="687" height="550" alt="image" src="https://github.com/user-attachments/assets/6068277f-18f0-48c5-894b-143f079a8dd5" /><br>
- First/ThirdPersonBasePoseOffset: This affects the base pose so you can modify your idle position and everything without needing to adjust your animations. I do recommend not doing this and instead doing it by correcting your animations.
- ThirdPersonRightHandedAimingOffset: This offset is for when you are aiming. If you are watching someone else aim with no offset, the optic will align directly in front of the camera which is likely in the center of your characters head. This lets you see the person aiming with an offset instead so you can bring the firearm over to the shoulder.
- ThirdPersonLeftHandedAimingOffset: Same as the above but used when you are in left hand dominant mode.

### SKGPDAProceduralPoseSettings
<img width="621" height="1151" alt="image" src="https://github.com/user-attachments/assets/85461046-fd5d-4781-a3d0-e4fbd336b85c" /><br>
- See the [Creating a custom pose page](Creating-a-custom-pose).
- LocationCurve: This is the curve that manipulates the location of the held actor.
- RotationCurve: This is the curve that manipulates the rotation of the held actor.
- PlayRate: This is how fast it will move through the curve. Higher = faster.
- CurveEndTime: This is the last frame that contains a key in either the location or rotation. If the location ends at frame 6, but rotation ends at frame 10, set this to 10.
- CurveTag: This is the identifier for the pose. The above screenshot uses Poses.HighPort, so if you want to enter the HighPort pose, you pass in the tag Poses.HighPort to the PerformProceduralPose function off the SKGShooterPawnComponent.
- Interpolate: This is whether or not you want to interpolate throughout the pose. This is useful for switching between poses partway between or starting and canceling a pose so it blends out.
- InterpolateConstant: This just changes the interpolation type to a constant interp.
- InterpolationTime: This is how many frames it will interpolate through before it stops (in the screenshot it's set to 5, meaning it will only interpolate for the first 5 frames).
- InterpolationSpeed: This is how fast the interpolation rate is.
- InterpolationSpeedMultiplier: TODO

### SKGPDARecoilSettings
<img width="533" height="1099" alt="image" src="https://github.com/user-attachments/assets/0143a273-8ea5-49e5-a708-ef732d9bfcbe" /><br>
- Recoil is driven by a curve, so control rotation will have a curve, location and rotation of the held actor will also have one. The Random variables are to add variation. For example with the RecoilRotation PitchRandom, it is between 2 and 5. This means each time you shoot, the muzzle will flip up a different amount.

### SKGPDARotationLagSettings
<img width="677" height="602" alt="image" src="https://github.com/user-attachments/assets/07ff7b28-861d-4999-afcc-666d6e82ed8d" /><br>
- These settings are performed via your rotation IE your mouse movement as you turn left/right/up/down.
- bUseRotationLag: If false, rotation lag will not be used. Not setting the data asset also gives the same effect.
- bUseSpringInterpolation: Whether or not to use spring interpolation.
- MaxRotation: What is the max rotation in the Pitch/Yaw/Roll that the rotation lag can rotate to.
- MaxLocation: Same as the above but for location.
- RotationMultiplier: How much to increase the rotation amount by. Higher = more influence from your rotation.
- LocationMultiplier: Same as the above but for location.

# Muzzle
### SKGPDAMuzzleInitialize
<img width="375" height="152" alt="image" src="https://github.com/user-attachments/assets/8d8f032f-41ca-4c1f-8bd9-883510772916" /><br>
- MuzzleMeshComponentName: Name of the mesh component that has the MuzzleSocketName, such as a barrel, suppress, muzzle device, etc.
- MuzzleSocketName: Name of the socket used for the end of the muzzle where you want projectiles to launch from as well as for firearm collision.

### SKGPDAMuzzleSettings
<img width="552" height="331" alt="image" src="https://github.com/user-attachments/assets/85a344a4-9751-4932-bb28-e56383a39819" /><br>
- MuzzleTag: This is used to figure out which muzzle device should be used. For example, if you had a m4 with a barrel, your barrel would have the muzzle settings of MuzzleComponentType.Barrel. If you added a muzzle device to the barrel, then a suppressor to the muzzle device, the suppressor would have the tag of MuzzleComponentType.Suppressor. This lets the system know to use the suppressor, not the muzzle device, or the barrel.
- GameplayTags: Just gameplay tags for the data asset.
- bUseMuzzleTemperatureSystem: If true, when you call ShotPerformed on the SKGFirearmComponent or a specific SKGMuzzleComponent, it will accumulate temperature each shot as well as cool down.
- MaxMuzzleTemperatureFahrenheit: Max temperature you want the muzzle to reach in Fahrenheit.
- MuzzleStartGlowTemperature: This is the temperature you want the muzzle to reach for it to start to glow. This is intended to be hooked into the heat up material provided with the example project. See the suppressor for an example.
- IncreaseMuzzleTemperatureAmountFahrenheit: How much the muzzle device temperature should increase per shot.
- DecreaseMuzzleTemperatureAmountPerTick: How much the muzzle device should cool down each tick.

# Optic
### SKGPDAOpticInitialize
<img width="457" height="234" alt="image" src="https://github.com/user-attachments/assets/2b42de9d-56c1-4eac-a383-f90e3d228649" /><br>
- OpticMeshName: Name of the mesh that contains the reticle.
- OpticSceneCaptureComponentName: Name of the optional SKGOpticSceneCaptureComponent.
- bIsMagnifier: Whether or not this optic is to be considered a magnifier. This is just a boolean you can fetch to make magnifier implementation easier.
- GameplayTags: Just gameplay tags for the data asset, not used by default.

### SKGPDAOpticReticleSettings
<img width="556" height="502" alt="image" src="https://github.com/user-attachments/assets/3ad89930-64a3-4f42-8440-8a7473e89346" /><br>
- ReticleMaterialIndex: This is the index that the reticle material should be applied to (on the optic mesh).
- bSupportsNightVision: If true, it indicates that this optic can go into a night vision mode and switch to using the ReticleNightVisionBrightnessSettings for brightness.
- ReticleMaterials: This is an array of materials that you can use in the reticle and cycle through, such as a red/green reticle.
- UnAimedReticleMaterial: This is the material that will be used when you stop aiming with a magnified optic, such as swapping to one that is a reflective glass.
- UnAimedCaptureDelay: How long after aiming will it take before the optic switches to the UnAimedReticleMaterial.
- ReticleBrightnessSettings: Brightness settings for the reticle itself.
- ReticleNightVisionBrightnessSettings: Same as the above but for when the optic is in night vision mode.
- WeightedBlendables: These are post process materials, primarily for thermal use. If you look at the thermal optic in the example project, it has several of these that you can cycle through such as white hot, black hot, rainbow, etc.
- EyeboxPositionSocket: This is the socket to be used for centering the eyebox. I recommend leaving this as the default S_Aim or whatever you call your aim socket as it is typically centered on the optic.

### SKGPDAOpticMagnificationSettings
<img width="492" height="419" alt="image" src="https://github.com/user-attachments/assets/d554e5cb-1234-4049-9016-8ba9b4f70623" /><br>


### SKGPDAOpticZeroSettings
<img width="359" height="364" alt="image" src="https://github.com/user-attachments/assets/b4e69ae4-2e3c-4a16-8b6e-4db4f69a9be2" /><br>

