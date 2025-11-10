By default, the Anim Layer provided in the example content is setup for the intent of Additive Animations for animations such as reloads. You are not forced to use additive animations and I will show how after we cover additives first. For this write up we will be using a reload animation as an example.

## Additive Animations
I would recommend looking up Additive Animations in unreal engine if you are unfamiliar so you can obtain a deeper understanding of them. Additive animations are animations that get played on top of your current animation. For example lets say you have a reload animation (no blending to specific parts of the body or anything) that is not additive that you want to play after your spine rotation is applied such as looking up/down. If you look down at the ground and reload, it will make you stand straight up and look forwards until the animation is complete. If the reload is additive, you will perform the reload from your current "pose" and continue to look down as expected.

### Setting up your animation to be additive
In this example we are going to use a reload animation. Feel free to use the example project's A_Manny_M4_Reload animation as a reference. Load up your animation (not montage) and scroll down until you see the Additive Settings category. From there you want to set the Additive Anim Type to Mesh Space, then for Base Pose Type you will want to decide which you want to use. What I recommend as a starting point is using the first frame of your idle animation, or your reload. The easiest setup is to use the Frame from this animation option and for Ref Frame Index using 0. This will make it use the first frame of the reload animation as it's starting point with everything else being played on top of that.\
![image](https://github.com/user-attachments/assets/4906d3ba-bd5d-4d63-b6c2-aca49b71e94e)\
The other route is to use Selected animation frame for the Base Pose Type, then set the animation as your idle pose at frame 0\
![image](https://github.com/user-attachments/assets/de15f228-cd2b-452a-a2c5-477cf811899b)\
Now your animation is additive and you can setup your montage.

### Setting up your montage
In your montage with the additive animation, you will notice the animation preview looks wonky and broken. You will want to scroll down to Additive Settings in the Asset Details. You will want to set this to your idle animation.\
![image](https://github.com/user-attachments/assets/3becc6ef-5f88-4502-b7b3-148497df6e53)\
This will let your animation preview look normal.
Before ![image](https://github.com/user-attachments/assets/d5ad0c9f-09a5-4fb6-a684-15af0787034a)\
After ![image](https://github.com/user-attachments/assets/ea16e013-0ddb-470f-b188-9dcd348cc552)

## Non additive animations
If you do not want to use additive animations, there are just some small changes to be made.

### Montage
In the montage I created a new slot for the sake of this example called DocExample and set the reload montage to utilize it.\
![image](https://github.com/user-attachments/assets/6aa81e04-3c3f-4efe-8703-d0026d1c7a92)\

### AnimBP
WORK IN PROGRESS
In the animbp we are going to want to make a new slot of DocExample (use your slot here) and position it before the procedural layer like so\
![image](https://github.com/user-attachments/assets/71f4562e-ede2-4fd6-a548-be82ed7792cf)\