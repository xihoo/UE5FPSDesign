This section is for the Pose Settings of the [Procedural Anim Component](SKGProceduralAnimComponent#pose-settings). When I refer to Third Person I'm referring to either being in third person mode or looking at remote clients.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/04c4ad53-aa58-4f84-ac7a-e83c571d3c5f)

# Overview
The system is quite simple. It uses an array of poses that are based around gameplay tags where each tag represents a curve. This allows you to create an unlimited amount of poses to do whatever you want and perform them by calling a function with that tag. For example if you want to go to a pose you call PerformProceduralPose and pass in the tag you want as well as whether or not to play the exit pose. Setting the Exit Pose to true will perform the From Curve, setting it to true will perform the To Curve.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/ff968d38-7715-4bd4-8360-f81078933134)\
The pose settings have two sets of curves. The To Curve and the From Curve. The To Curve is going into the curve, such as going from Idle into High Port. The From Curve is the opposite, such as going from High Port to Idle. Additionally the To and From Curves have a First Person and Third Person Curve set. The purpose of this is so instead of being stuck playing a curve that looks good in first person but bad in third person, you can play a unique one for both to get good results in first person and third person. Additionally to aid in creating these poses, a pose customization widget was created that you can access by pressing Tab\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c806f49c-5ad0-4aa2-b448-635e3cd92349)

# Creating a New Pose
Here we will create a new pose from scratch. The idea here is I want to simply rotate the firearm to look at the chamber and when I'm done, have it go back to normal.

### Creating the Gameplay Tag
First lets create our Gameplay Tag that defines this pose. I recommend making use of datatables to help organize your tags. The Example Project has a datatable called GDT_ProceduralPoseTags and I will use that for this example. If you want to create your own, create a new data table and when prompted for which structure to use, select GameplayTagTableRow.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/0568e5ae-ae77-4e2c-b0ba-1c3018c330e2)
Once created, go to your Project Settings, Select GameplayTags, then under the Gameplay Tag Table List add a new element and select the datatable you just created. Now that it is all configured, lets create our tag. Open up your datatable and add a new tag. We will call this one ProceduralPose.LookAtChamber.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/78ad6231-8d27-462b-aa4f-7d9dd10338d3)

### Creating the To Curve
To Create the curve we want to right click in our Content Browser and go to Miscellaneous, Curve, then select CurveVector. Lets duplicate this 4 times so we have 4 total curves and name them C_LookAtChamberLocation, C_LookAtChamberRotation, C_LookAtChamberLocationReturn, C_LookAtChamberRotationReturn. Now lets figure out some values. Press Play and equip your firearm you want this pose to be for and press Tab to open the Pose Customization Widget. From here you can start tweaking your values by entering them in the text box at the top. I ended up with -5, 7, and 8 for the location, -55, 30, and -45 for the rotation. These will be my end point for the location/rotation curve. Note that this assume using a Y forward firearm. Firearms using X forward may need to have that rotation difference accounted for.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/a34f7f87-720c-4b37-96e7-63e2d1c03bde)\
Open up the C_LookAtChamberLocation curve and keyframe the XYZ at frame 0 with a value of 0, then move over to somewhere around frame 12 and add keyframes for XYZ with X: -5, Y: 7, Z: 8. Now do the same thing for C_LookAtChamberRotation except this is a touch different. X: -45, Y: 30, Z: -55. Here is an example of the Location curve.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/0fa3ffc1-d243-4964-acb9-9be6d55afe71)

### Configure The Pose Setting
Now we want to add this as a pose to our firearm. Lets go to our firearms ProceduralAnimComponent and locate the Pose Settings in the Details Panel. Locate the Curve Tag and set it to our LookAtChamber tag we just made.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/4c184fc9-bafb-45db-8be1-d8fcaf660a1d)
Now lets assign the curves for testing. In order for this to work, both the To AND From Curves will need to be set. Additionally, we need to set the Play Rate to something other than 0 so we will start with 4, then the Curve End Time needs to be set to the end point of the curve. This would be the last frame of either the Location or Rotation curve depending on whichever is longest. In my case it is 12. Now lets go ahead and enable interpolation, set the interpolation time to 4, interpolation speed to 10, and interpolation speed multiplier to 5.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/cfcb2a6c-d3f6-4bbd-8e81-338a4e806eb9)

### Testing our pose
If using the example project, you can go to the BP_SKGSFExamplePawn and go to the Firearm event graph. There you will see inputs for short stock,  high port, low port, etc. You can either swap the tag in one of these or duplicate one of these and swap the tag. For the sake of simpleness, I swapped the short stock tag for ProceduralPose.LookAtChamber.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/0408ec21-227a-4b0b-b80e-7cfe22941604)

### Tweaking the settings
Now that we have our pose setup and working, I noticed it is quite slow to go into the pose so lets increase the speed. This is what the Play Rate setting is for so lets go to our To Curve and increase the Play Rate to 22. This is the value I settled on after tinkering a bit and felt good. Now we have the problem of the curve just being linear lines. To start lets select our starting point and set it to Cubic Interpolation for both the Location and Rotation curves. From here you will want to start tweaking the middle, adding some additional keyframes and adjusting as you see fit. Think of it almost like working in the curve editor of blender if your familiar.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/73f4f933-6274-4f98-b81a-f21aa14b98cc)

### Creating the From Curve
The from curve is quite simple. Since we have the To Curve already, we already know our values to start from. Instead of going from 0 to X, we go from X to 0. So on frame 0, set your Location and Rotation Return Curves to equal that of your last frame of the To Curves. The easiest way to do this as I'm unsure if you can copy/paste keys to another curve would be to duplicate your graph and swap the start and end values like this.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/4a4c7017-fec8-432c-ac0f-67eb291843ce)\
You will want to do this for both the location and rotation. Likewise, we will set our Play Rate and Curve End Time to 12.