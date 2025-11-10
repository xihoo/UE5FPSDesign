The purpose of this component is to offer a simple to use flashlight and laser whether it be handheld or a firearm attachment. You do not need to setup both a flashlight and a laser, one or the other can be used such as only a flashlight or only a laser or both.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/e838c31f-4ace-4cbd-ab20-e0f157e9ce5f)

# Details Panel
## Initialize
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/87b24aec-98d1-4f64-aef1-ca4bce204e87)
### Light Component Name
This is the name of the light component. In the example it is a spotlight.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/835dc30d-6c4e-4258-835e-e46a12ebfac6)
### Laser Mesh Component Name
This is the name of the laser mesh to be used.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/2e449ff5-d0f7-4259-92b9-c2e8b87016cc)
### Laser Dot Component Name
This is the name of the laser dot to be used.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/62b9190b-fe9d-4783-b9be-7ac09b9b7fbf)
### Has Infrared Mode
If this is true, this light/laser can go in/out of infrared mode (for use with night vision).

## Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/31367b3a-ed5f-4edc-95cd-1b3e3556bc97)
### Laser Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/72d0b6e2-ed75-465d-be7b-66af3f1cf889)
#### Max Laser Distance
This is the furthest distance the laser can go. If it goes beyond this range it essentially becomes invisible.
#### Laser Scale Factor
This depends on the length of the mesh used as the laser. In the example the laser mesh is 20cm long, so a value of 20 is used for the scale factor. If it was 10cm long, a value of 10 would be used.
#### Infrared Laser Material
This section may be removed in the near future in favor of the night vision material function.
##### Laser
This is the material the laser will switch to when infrared mode is enabled.
##### Laser Dot
This is the material the laser dot will switch to when infrared mode is enabled.

### Light Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c82f1e83-90c5-4f2e-8883-4e57e8524e99)
#### Light Strobe Interval
This is how delay between strobes when the light is set to be in strobe mode.
#### Light Intensities
This is an array of light brightness's that can be cycles through.
#### Light Intensities Infrared
This is an array of light brightness's that can be cycles through when in infrared mode.

### Light Laser Cycle Modes
If you want a tarkov like system where you have a set series of modes to cycle through rather an individual control, you would use this as you can define the modes/features for the user to cycle through.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/dcf39b42-605a-4fa0-a8c9-6ba0331fb889)


# Functions
## LightLaser Category
### SetInfraredMode
This function sets the infrared mode on or off based on if you pass in True or False as a parameter.
### CycleLightLaserMode
This function will cycle your light/laser through the various states defined in [Light Laser Cycle Modes](#light-laser-cycle-modes).

## Laser Category
### SetLaserMode
This function lets you set the laser mode, such as turning it on or off.
### GetLaserMode
This function returns the lasers current mode such as on or off
### IsLaserOn
This function returns a simple true/false for easier use with branches. True if the laser is on, False if it is off.

## Light Category
### SetLightMode
This function will set the light mode based on what you pass in such as on, off, or strobe.
### GetLightMode
This function will return the current light mode such as on, off, or strobe.
### IsLightOn
This function returns a simple true/false for easier use with branches. True if the light is on or strobe, False if it is off.
### SetLightIntensityIndex
This function will set the flashlights brightness intensity based on the given index setup in [Light Intensities](#light-intensities). If you are in infrared mode it will use the [Light Intensities Infrared](#light-intensities-infrared).
### CycleLightIntensity
This function will iterate over each [Light Intensities](#light-intensities). If you are in infrared mode it will use the [Light Intensities Infrared](#light-intensities-infrared).

## Getters
### GetLightSoure
This function returns the light source found from [Light Component Name](#light-component-name).
### GetLaserMesh
This function returns the laser found from [Laser Mesh Component Name](#laser-mesh-component-name).
### GetLaserDot
This function returns the laser dot found from [Laser Dot Component Name](#laser-dot-component-name).
### IsInInfraredMode
This function returns true if the device is currently in infrared mode, false if not.

## Events
### OnLaserStateChanged
This event will fire each time the laser state is changed and pass in the new laser state.
### OnLightModeChanged
This event will fire each time the light state is changed and pass in the new light state.
### OnInfraredModeChanged
This event will fire each time infrared mode is changed and pass in the new infrared mode.
### OnLightStrobed
This event will fire each time the light is strobed on/off and will pass in the state of on/off.
### OnLaserImpact
This event will fire each time the laser makes am impact and will pass in the resulting hit information as well as whether or not a valid hit was made.