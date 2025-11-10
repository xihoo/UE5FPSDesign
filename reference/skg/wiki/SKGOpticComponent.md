The purpose of this component is to act like an interface to the SKGOpticSceneCaptureComponent so you don't interact with it directly, as well as offer several optic relation settings and functionality. For example it handles reticle brightness, cycling reticles, zeroing, magnification, etc.

![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/61dc5278-22b8-4af2-90cb-4cd592825762)
# Details Panel
## Initalize
### Optic Mesh Name
The Optic Mesh Name is the name of the Static/Skeletal Mesh Component on your blueprint.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/2ac84251-3cad-4154-a95b-f06bceda904b)
### Optic Scene Capture Component Name
The Optic Scene Capture Component Name is the name of SKGOpticSceneCaptureComponent added to your magnified optics. This is not a mandatory field and is only used on optics that have the SKGOpticSceneCaptureComponent on them. This is subject to modification in the future as I want to support both zooming with a SKGOpticSceneCaptureComponent as well as traditional style without.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/de620fdf-4469-4926-8978-530412e96676)
### Gameplay Tags
Each component comes with the gameplay tag interface already setup in the case that you might want to utilize one of the components with GAS (Gameplay Ability System).

## Settings
### Reticle Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6f930e86-50b5-4bc6-a7b7-47ce86484bcd)

#### Reticle Material Index
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/828892d6-f0cc-4623-8336-cef16c6f6256)
The Reticle Material Index is the material index that will be used for the reticle. In the examples below the Holosight has the material index of 1 while the Thermal optic has a material index of 0.

#### Reticle Materials
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/a0d663ac-0dd6-41df-9520-6c0fc2c11540)
The Reticle Materials is an array of materials you can use to cycle between. For example if I want to be able to cycle between a red and green reticle, I would have two entries with one being red and one being green.

#### Un Aimed Reticle Material - Un Aimed Capture Delay
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/bd6510b7-85c1-41ce-ab4c-2699af4fe123)
The Un Aimed Reticle Material and Un Aimed Capture Delay applies to magnified optics using a SKGOpticSceneCaptureComponent . When you stop aiming, it will wait the amount of time specified in the Un Aimed Capture Delay before changing the reticle material to the one specified in the Reticle Material. This is useful if you want to change it to something such as a simple glass material.

#### Reticle Brightness Parameter Name - Reticle Brightness Settings - Reticle Night Vision Brightness Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6566adef-858e-48d2-9802-ff19e321ab1f)
The Reticle Brightness Parameter Name applies to the reticle brightness parameter in the material instance. Provided in the example project are many materials and material functions already preconfigured for you. The default parameter name "ReticleBrightness" applies to the provided material parameter names.\
The Reticle Brightness settings are different brightness levels you want the reticles to have that you can cycle through. The Reticle Night Vision Brightness Settings are the same but only apply when the optic is in Night Vision Mode.



### Magnification Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/13afab19-1260-4eec-9eea-3543ada90534)
The Magnification Settings apply only to magnified optics using the SKGOpticSceneCaptureComponent (Currently).

#### Magnifications
Magnifications is an array of zoom levels/magnification you want the optic to use. For example, the provided LPVO uses 1.0, 2.5, 5.0, 8.0 as it is a 1-8 power optic.

#### Smooth Zoom - Smooth Zoom Rate
If Smooth Zoom is enables, as you change magnifications it will change smoothly between the magnifications ate a speed specified in Smooth Zoom Rate.

#### Shrink Eyebox With Magnification - Shrink Eyebox Multiplier
The Shrink Eyebox With Magnification option is for some added realism. As you zoom in with your optic in real life, your eyebox will shrink and make it more difficult to keep your eye centered in the eyebox. With Shrink Eyebox With Magnification enabled, it will scale as you zoom in/out based on your Shrink Eyebox Multiplier setting.

#### Is First Focal Plane
Some optics are First Focal Plane, some are Second Focal Plane.\
First Focal Plane: Your reticle will scale up/down with your magnification.\
Second Focal Plane: Your reticle will not scale/change at all as you change magnifications.


### Optic Zero Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/624b2a2c-b7b2-4f29-a1dc-7bf96b04cdd9)
The Optic Zero Settings apply only to magnified optics using the SKGOpticSceneCaptureComponent (Currently).

#### Adjustment Type - Adjustment Amount
The Adjustment Amount option changes based on the Adjustment Type. If MRAD you will see MRADAdjustment Amount, if MOA you will see MOAAdjustment Amount.\
MRAD: Adjustments will be made in Mills\
MOA: Adjustments will be made in MOA

#### Starting Zero Settings
#### Start with Default Zero
If true, your optic will start with the amount of clicks defined in Default Elevation/Windage Clicks.

##### Start With Random Zero
If true, your optic will start with a random amount of clicks defined in Random Max Elevation/Windage Clicks. Default is 25 so it will range from -25 to 25.


# Functions
## SKGOptic Category
#### StartedAiming
If using a SKGOpticSceneCaptureComponent it will start capture.
#### StoppedAiming
If using a SKGOpticSceneCaptureComponent it will stop capture.
#### IsMagnifiedOptic
If your optic is magnified it will return true, false otherwise.
#### GetOpticMesh
This function will return the mesh of your optic.

## Reticle Category
#### CycleReticle
If you have multiple reticles assigned under your [Reticle Materials](#reticle-materials) it will cycle through them and assign them to your reticle.
#### IncreaseReticleBrightness - DecreaseReticleBrightness
These functions will change your reticles brightness defined in [Reticle Brightness Settings](#reticle-brightness-parameter-name---reticle-brightness-settings---reticle-night-vision-brightness-settings). If you are in Night Vision Mode, it will use the values defined in [Reticle Night Vision Brightness Settings](#reticle-brightness-parameter-name---reticle-brightness-settings---reticle-night-vision-brightness-settings).
#### ToggleReticleNightVisionMode
Calling this will switch the reticle between the defined [regular](#reticle-brightness-parameter-name---reticle-brightness-settings---reticle-night-vision-brightness-settings) and [night vision mode brightness settings](#reticle-brightness-parameter-name---reticle-brightness-settings---reticle-night-vision-brightness-settings).

## Zoom Category
#### ZoomIn - ZoomOut
These functions will zoom in and out optics that (currently) implement the SKGOpticSceneCaptureComponent based on your [Magnification Settings](#magnifications).
#### GetCurrentMagnification
This function will return the magnification your optic is currently on.

## Zero Category
#### PointOfImpactUpDownDefault - PointOfImpactLeftRightDefault
These functions will reset your elevation/windage back to your zero. For example if you changed your clicks on elevation or windage, it will reset them back to zero.
#### PointOfImpactUp/Down/Left/Right
These functions will adjust your elevation and windage based on the amount of clicks you pass in. For example, an optic set to use MRAD at 0.1 mils will adjust at 1/10th of a mil per click.
#### ApplyLookAtRotationZero
This function is for optics with the SKGOpticSceneCaptureComponent and meant to be called from the SKGFirearmComponents ZeroOpticsForZeroAtLocation fuction. It is meant to be utilized alongside the SKGProjectileWorldSubsystem function GetProjectileZeroAtLocation which will get the rotation value for you. The SKGFirearmComponent has a function ZeroOpticsForZeroAtLocation which automates this process for all the optics on the firearm.