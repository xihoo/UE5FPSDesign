The purpose of this component is to offer a consistent point in which to read the muzzle transform, as well as offer a muzzle temperature system. The way it works is if you have just a barrel, it will utilize the barrel, if you add a muzzle device it will use that muzzle device, if you add a suppressor onto that muzzle device it will use the suppressor to read the muzzle information from.

![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/e0e0c6bd-95a1-4473-bc99-3f9f25626847)

# Details Panel
## Initialize
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/b5d90cca-955b-48c5-88b5-35c3e4151384)
### Muzzle Mesh Component Name
This is the name of the mesh to be used for the muzzle. If you have a barrel, it is the barrel mesh, if you have a complete firearm with a barrel, it is your firearm mesh, same thing with a muzzle device.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/fc2558d8-fda2-474a-9bc8-fb482d80ad63)
### Muzzle Socket Name
This is the name of the socket to be used for the muzzle. It is used for figuring out the projectile launch transform as well as the firearm collision end point.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/66926c90-8085-4415-b741-7a908ad97220)
### Muzzle Tag
This is used to distinguish which muzzle should be used. For example, if you have a complete firearm with a barrel, the firearm will have the tag Barrel. If you have a barrel as an attachment it will use the tag Barrel. If you have a muzzle brake, that will use the tag MuzzleDevice, if you have a suppressor, that will use the tag Suppressor.

## Muzzle Temperature
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/a42b42fd-7ec5-4bae-b60e-2981e11fae0c)
### Use Muzzle Temperature System
If true, this muzzle device will utilize the muzzle temperature system which simulates the muzzle being heated after each shot and slowly cooling when not firing.
### Muzzle Temperature Tick Interval
This is the rate at which the component will tick while cooling. You do not need a fast tick rate for this.
### Max Muzzle Temperature Fahrenheit
This is the highest temperature the muzzle can reach.
### Muzzle Temperature Normalize Rate
This is a value used to normalize the muzzle temperature between 0 and 1.
### Increase Muzzle Temperature Amount Fahrenheit
This is how much the muzzle temperature will increase per shot.
### Decrease Muzzle Temperature Amount Per Tick
This is how much the muzzle temperature will decrease each tick.


# Functions
## Tag Category
### GetMuzzleTag
This function simply returns the specified [tag](#muzzle-tag).

## MuzzleTemperature Category
### ShotPerformed
This function is to be called (typically from the [SKGFirearmComponent](SKGFirearmComponent)) when a shot is fired so that the temperature can increase.
### GetMuzzleTemperature
This function returns the current temperature of the muzzle.
### GetMuzzleTemperatureNormalized
This function returns the current temperature of the muzzle normalized based on the [Muzzle Temperature Normalize Rate](#muzzle-temperature-normalize-rate).
### IsUsingMuzzleTemperatureSystem
This function returns true if you are using the muzzle temperature system, false otherwise.

## Transform Category
### GetMuzzleTransform
This function returns the world transform of the muzzle socket.
### GetMuzzleProjectileTransformCompensated
This function returns a modified muzzle transform used to get a rough zero with your projectile. For example setting ZeroDistanceMeters to 100 and MOA to 1.0 will have your projectile intersect with your point of aim at 100 meters with an accuracy of 1 MOA.
### GetMuzzleProjectileTransform
This function does the same as the above, but without any rough zero, only accuracy.


# Events
### OnMuzzleTemperatureChanged
This event will fire each time the muzzle temperature changes and pass in the new temperature.