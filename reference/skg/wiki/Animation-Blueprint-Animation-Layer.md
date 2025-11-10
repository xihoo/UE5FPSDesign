# Order Of Operations
What I mean by order of operations is the order in which the anim graph is laid out. There is an issue regarding the blend masks with the ik_hand_gun bone and splitting the upper/lower body. The issue is when all the procedurals were being performed via ik_hand_x bones everything worked just fine until you would split the upper and lower body. Even though the blend mask was set up so the lower body ignored the ik_hand_X bones it would still influence them giving a bad result. The solution was to use virtual bones on the head and apply the procedurals to those and then after the upper/lower body split, copy ik_hand_gun to the virtual bone. The order in which this is corrected is to apply all the procedurals to the virtual bones, perform the upper/lower body split, then apply the IK to the ik_hand_X bones.

# Animbp Overview
By default in the example project, the AnimBP uses a linked Anim Layer. If you are unsure what a linked Anim Layer was I recommend researching them as they are quite useful and you might want to use them for parts of your own system. The linked Anim Layer for the example runs all of the procedural logic. So the flow goes like this: You have your base animation such as an idle animation, this gets passed into the linked Anim Layer. The linked Anim Layer performs the procedural steps. The output of the linked Anim Layer then gets split into a layered blend per bone that separates the upper and lower body (for locomotion). Finally IK is applied, so ik_hand_gun copies the VB ik_hand_gun bone and hand_r and hand_l use two bone IK to move to the ik_hand_r and ik_hand_l bones so the hands are in the correct place. If you do not want to create the below Anim Layer and AnimBP from scratch, you can always retarget.

# Anim Layer Interface
Create a new anim layer interface and add a new Animation Layer to it. The example content calls it ALI_SKGSFExample so we will use the same name for simplicity of using the example project as a reference. In the example this layer is called SKGSFProcedurals so we will use that name.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/b434a6dd-1616-4eec-b407-bddd50de09f4)

# Anim Layer
Create a new Anim Layer. Right click and hover over Animation, and select Animation Blueprint. For the Specific Skeleton, select the skeleton you want to use. For the Parent Class, select SKGShooterFrameworkAnimInstance. This is the Anim Instance that will drive the Anim Layers logic.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/cab65421-aea4-42c0-bd2b-2cf5e057595f)
Open your Anim Layer and go to Class Settings at the top, then under Implemented Interfaces hit Add and select the ALI SKGSFExample.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/e6a1f7c4-46d5-4ef4-acb5-fb77484cf2a2)
Now under our Animation Layers on the left hand side, we have a new Animation Layers option called SKGSFProcedurals which is the Anim Layer we added in the Anim Layer Interface. Double click to open. This is where we will add all our procedural anim graph logic from the example which can be copied/pasted in.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/797e611b-332a-4d73-8e34-40b26645aee7)
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/2f1c6162-f3c6-42ae-8dbe-78a506be6227)
Lastly for this anim layer, we want to make sure Use Main Instance Montage Evaluation Data is enabled. This montages played on the main anim instance to be performed on the linked anim layer as well. Click Class Defaults and enable Use Main Instance Montage Evaluation Data at the bottom.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/437eba46-cdf0-48cd-887a-8c089619dba4)

# Main AnimBP
The linked anim layer greatly simplifies and declutters the main AnimBP making it much less intimidating and easier to work in. What we want to do is create a Animation Layer in our AnimBP by clicking the + next to ANIMATION LAYERS.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/0e0360d9-abf7-4bee-9168-2b97b3b8206a)\
Following the naming of the example project, we will name this SKGSFProcedurals.\
Now in order to set our Animation Layer to the layer we want, we need to include our [Anim Layer Interface](#anim-layer-interface). Click on Class Settings and scroll down to Implemented Interfaces, click Add and select our ALI SKGSFExample like so.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/30382b8b-5777-4c5a-bcc2-5990808be512)\
Now we can drag/drop it into our AnimGraph like so for use.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/19ec4a8e-d273-473c-87c2-604a991163db)\

## Add Final IK Nodes
Go ahead and add a Copy Bone node, and two Two Bone IK nodes.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/4ccb60ce-0108-4bdd-a5d3-2f7543990d2c)
#### Copy Bone
This node is used to position our ik_hand_gun bone in place from the procedurals. We want the Source Bone to be the VB ik_hand_gun and the Target Bone to be ik_hand_gun, and we want Copy Translation and Copy Rotation to be true\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/507889b2-04f4-4830-ba44-be4f601f5952)\
For the first Two Bone IK node we want the IKBone to be hand_r. We want to set Effector Location Space to Bone Space, enable Take Rotation From Effector Space, and we want our Effector Target to be ik_hand_r. Moving down we want our Joint Target Location Space to be in Bone Space, and the Join Target to be upperarm_r.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/94b62d62-3969-427e-b3ab-e4a15d11e1e5)\
The second Two Bone IK node is the same as the first but for the left side. We want the IKBone to be hand_l. We want to set Effector Location Space to Bone Space, enable Take Rotation From Effector Space, and we want our Effector Target to be ik_hand_l. Moving down we want our Joint Target Location Space to be in Bone Space, and the Join Target to be upperarm_l.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/7780ee00-644b-4151-924c-e40746aac7ab)

### Hooking Up
For now we will set it up as simple as possible using the [above IK nodes](#add-final-ik-nodes). We will place down our idle animation (this is the idle animation of simply holding an M4), that will be plugged into our SKGSFProcedurals Animation Layer, that will be plugged into a Copy Bone node, that will be plugged into a Two Bone IK node, that will be plugged into another Two Bone IK node, and that will be plugged into the output pose. So 5 nodes in total (minus the space conversion nodes).\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/436ea59d-0872-461c-aee1-75907083af99)

### Linking the Anim Layer
Linking must be performed, if it is not done then the SKGSFProcedurals Animation Layer will not run. There are two ways to do this. If you want this running from the start (basically when the pawn is spawned) I recommend setting my component to auto enable/link for you. To do this go to your pawn and select the SKGShooterPawn component. Under the Initialize cateogry in the details panel you will see Linked Anim Layer Class. Set this to the [Anim Layer BP](#anim-layer) you made. Then ensure Auto Setup Linked Anim Layer is enabled like so.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/45133aeb-0461-440a-8034-5bc1788488ec)\
The other option is to do it later manually. I have functions you can call to perform this. First you will want to make sure Auto Setup Linked Anim Layer is disabled so it does not try to auto apply it, then you will want to ensure that the Linked Anim Layer Class is set. Now from the SKGShooterPawnComponent you can call LinkAnimLayerClass or LinkAnimLayerClassByInstance. LinkAnimLayerClass will apply the Linked Anim Layer to the appropriate meshes for you while LinkAnimLayerClassByInstance is for if you want more manual control over which Anim Instance gets the LinkedAnimLayer applied to. Likewise there are Unlink functions as well.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/dae310cf-9b58-4629-aec5-b6ff531eef01)

### Adjust Joint Target Location
Now that we have a basic setup and our IK is configured, we will want to adjust our Joint Target Location. We will start with the right hand so select the first Two Bone IK node for hand_r and select the join target (it is visible and likely around your shoulder)\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/2239102b-f80e-4f97-aaa7-58a9d9d6b793)
Typically we will want this location to be down, to the side, and back. Here is the value I'm using in the example project for hand_r that you can copy/paste: (X=-31.830163,Y=83.032387,Z=-15.502627). Repeat the same thing for the second Two Bone IK node for hand_l and plug in these values as a good starting point: (X=-2.987916,Y=-82.034790,Z=20.655229)\
This will get you into a position similar to this.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/5b5e43db-b194-4b9e-be8b-d852844a4b50)\

### Upper/Lower body split
Commonly you will see people split the upper and lower body from each other. For example, you likely want locomotion to drive your lower body while having the animation of holding a firearm or some other object driving the upper body. To do this we are going to use the [blend mask we made](#blend-mask). Create a Layered blend per bone node, set the Blend Mode to Blend Mask, then select the ik_hand_gun blend mask we created. Now enable Mesh Space Rotation Blend. In the example project I have a simple blend space for the locomotion that is driven by speed and direction. These are quite easy to make following tutorials on youtube, but if you want you can grab a simple one from the Third Person Example Template provided by Epic with Unreal Engine. What we are going to do is plug our locomotion into Base Pose, this is for the lower body. Then we will plug in our SKGSFProcedurals Anim Layer to Blend Poses 0 which is the upper body like this:\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6de7a4b1-864e-4ec4-ba67-97839cee6816)\
Now as you move your lower body will perform the locomotion. For example this is me walking forwards and to the right
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c906a6e9-d965-4842-bc79-c97d1806aac5)
