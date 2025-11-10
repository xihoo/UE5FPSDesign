Both the Pawn and Character class are supported so you can utilize this plugin with the General Movement Component and Mover 2.0. For this writing I will refer to characters/pawns as just pawn. When I say pawn I'm referring to both.

# Components
To begin, you will want to add a [SKGShooterPawnComponent](SKGShooterPawnComponent) to your pawn. This component acts as the main interaction point between you and the rest of the systems. Follow the setup on the [SKGShooterPawnComponent](SKGShooterPawnComponent) page to configure it with your pawn.

# Skeleton Setup
The procedurals rely on a few virtual bones. For firearm collision there is a VB FirearmCollision bone, and for the rest there are the VB ik_hand_gun_parent, VB ik_hand_gun_ads, and VB ik_hand_gun. VB ik_hand_gun_ads and VB ik_hand_gun are children of VB ik_hand_gun_parent. Lastly there is a camera socket that goes on the head.

### VB ik_hand_gun_parent
Right click on the head bone and select Add Virtual Bone. For the Target Bone, select head. Name it ik_hand_gun_parent.
### VB ik_hand_gun_ads
Right click on the VB ik_hand_gun_parent bone and select Add Virtual Bone. For the Target Bone, select head. Name it ik_hand_gun_ads.
### VB ik_hand_gun
Right click on the VB ik_hand_gun_parent bone and select Add Virtual Bone. For the Target Bone, select ik_hand_gun. Name it ik_hand_gun.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/25a93681-dd2e-4cb6-a213-7542e925e132)

### VB FirearmCollision
This virtual bone can go anywhere on the upper body that the blend mask will set as a value of 1.0. I recommend putting this on the head as well. Right click on the head bone and select Add Virtual Bone. For the Target Bone, select ik_hand_root or root, it does not matter here. Name it FirearmCollision.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6cb16cd8-c181-4bbe-a06d-5083a85a0a64)

### Camera Socket
Right click on the head bone and select Add Socket. Name this socket anything you want as it can be set in the [SKGShooterPawnComponent Camera Attached Socket name](SKGShooterPawnComponent#camera-attached-socket). By default is it S_Camera.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/57caa19c-4c41-49a2-9d4a-b3819623ed2b)\
You will want to position this socket where you want. Some people want it in the middle of the eyes, some might want to favor the right eye. In the example I positioned it just in front of the right eye. Once positioned, this socket will need to be rotated. Go back to your pawn and attach the Camera Component to the Camera socket and you will see it it rotated incorrectly.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/76903e78-19f8-4c87-a98a-535dd70bbb7c)
Rotate the socket around until the camera is facing forwards and upright. If you are using the Unreal Engine mannequin or similar, you can just use these values for the sockets rotation: (Pitch=-0.000000,Yaw=89.999420,Roll=-90.000000) and the camera will be facing correctly.\
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/568af692-5705-4d61-9eb2-1dbbf0c480e2)

### Blend Mask
By default the example uses a blend mask to use with layered blend per bone rather than using the filters. To create a blend mask, go to your skeleton, click the COG wheel, then hover over Blend Profiles and select Add Blend Mask. We will name this blend mask ik_hand_gun.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/2f50fd46-3eab-4114-a547-f40800c6d2ae)
What we want is all parts that will be for the upper body (including ik_hand_X bones) to be at a value of 1, and the lower body at a value of 0. Take ik_hand_root and put its value to 1.0, then right click and click Recursively Set Blend Scales To 1. Do the same thing on spine_01 and you are done and can save.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/a910daeb-a1a8-4f6f-8554-df3871b6328c)
After making changes to your blend mask, make sure to hit Compile on your animbp as it does not seem to reflect changes on the fly.\\

Now that the skeleton is complete, you can move onto setting up your [animbp and anim layer here](Animation-Blueprint-%7C-Animation-Layer).