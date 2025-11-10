The SKGOpticSceneCaptureComponent is a simple replacement for the built in SceneCaptureComponent that comes with Unreal Engine. It ties in directly with the SKGOpticComponent, but can still be used on its own.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/4d0c0936-6595-4c64-836d-eeb4edd8e613)

# Details Panel
## Initialize
### Gameplay Tags
Each component comes with the gameplay tag interface already setup in the case that you might want to utilize one of the components with GAS (Gameplay Ability System).

## Settings
### Optic Scene Capture Component Settings
#### Render Target Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c7644e42-b6f4-435a-8a41-9275cc47d9ec)
##### Tick Interval
This setting is for how often the component will update/capture the scene when you are capturing.
##### Resolution X/Y
This is for the resolution of the render target the scene capture creates.
##### Texture Render Target Format
This is for the format of the render target.

#### Material Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/ef6f16c7-b24a-438a-8b5e-ee8c98bc3934)
##### Texture Target Parameter Name
This is the name of the render target used in the material. Set to use the default provided in the example content.
##### Reticle Size Parameter Name
This is the name of the reticle size parameter in the material. Set to use the default provided in the example content.
##### Eyebox Sensitivity Parameter Name
This is the name of the eyebox sensitivity parameter that is used for scaling the eyebox with magnification. Set to use the default provided in the example content.


# Functions
Note - This component is intended to be used through the SKGOpticComponent
## Tick Category
#### StartCapture - StopCapture
These functions start and stop the capture (of capturing the scene and drawing to the render target) of the scene capture component.

## Zoom Category
#### Zoom
This function will zoom the scene capture to the target magnification. How it will get to the target magnification depends on the settings from the SKGOpticComponent. C++ users can setup their own settings manually by calling the Initialize function and passing in the given parameters.

## Zero Category
#### PointOfImpactUpDownDefault - PointOfImpactLeftRightDefault
These functions will reset your elevation/windage back to your zero. For example if you changed your clicks on elevation or windage, it will reset them back to zero.
#### PointOfImpactUp/Down/Left/Right
These functions are intended to be called from the SKGOpticComponent. These functions will adjust your elevation and windage based on the rotation amount.
#### ApplyLookAtRotationZero
This function is intended to be called from the SKGFirearmComponents ZeroOpticsForZeroAtLocation function. This function is for optics with the SKGOpticSceneCaptureComponent. It is meant to be utilized alongside the SKGProjectileWorldSubsystem function GetProjectileZeroAtLocation which will get the rotation value for you. The SKGFirearmComponent has a function ZeroOpticsForZeroAtLocation which automates this process for all the optics on the firearm.