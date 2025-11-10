Pawn和Character类都受支持，因此您可以将此插件与通用移动组件和Mover 2.0一起使用。在本说明中，我将把角色/Pawn统称为Pawn。当我说Pawn时，我指的是两者。

# 组件
首先，您需要在您的Pawn上添加一个[SKGShooterPawnComponent](SKGShooterPawnComponent)。该组件充当您与其余系统之间的主要交互点。请按照[SKGShooterPawnComponent](SKGShooterPawnComponent)页面上的设置说明将您的Pawn配置好。

# 骨架设置
程序化依赖于几个虚拟骨骼。对于火器碰撞，有一个VB FirearmCollision骨骼，对于其余部分，有VB ik_hand_gun_parent、VB ik_hand_gun_ads和VB ik_hand_gun。VB ik_hand_gun_ads和VB ik_hand_gun是VB ik_hand_gun_parent的子级。最后，头部上有一个摄像机套接字。

### VB ik_hand_gun_parent
右键单击头部骨骼并选择添加虚拟骨骼。对于目标骨骼，选择头部。将其命名为ik_hand_gun_parent。
### VB ik_hand_gun_ads
右键单击VB ik_hand_gun_parent骨骼并选择添加虚拟骨骼。对于目标骨骼，选择头部。将其命名为ik_hand_gun_ads。
### VB ik_hand_gun
右键单击VB ik_hand_gun_parent骨骼并选择添加虚拟骨骼。对于目标骨骼，选择ik_hand_gun。将其命名为ik_hand_gun。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/25a93681-dd2e-4cb6-a213-7542e925e132)

### VB FirearmCollision
这个虚拟骨骼可以放在上半身的任何位置，混合遮罩会将其设置为1.0的值。我建议也放在头部。右键单击头部骨骼并选择添加虚拟骨骼。对于目标骨骼，选择ik_hand_root或root，这里无关紧要。将其命名为FirearmCollision。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6cb16cd8-c181-4bbe-a06d-5083a85a0a64)

### 摄像机套接字
右键单击头部骨骼并选择添加套接字。您可以将此套接字命名为任何您想要的名称，因为它可以在[SKGShooterPawnComponent摄像机附加套接字名称](SKGShooterPawnComponent#camera-attached-socket)中设置。默认情况下，它为S_Camera。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/57caa19c-4c41-49a2-9d4a-b3819623ed2b)
您需要将此套接字定位到您想要的位置。有些人希望将其放在眼睛中间，有些人可能更喜欢右眼。在示例中，我将其定位在右眼前方。定位后，需要旋转此套接字。回到您的Pawn，将摄像机组件附加到摄像机套接字上，您会看到它旋转不正确。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/76903e78-19f8-4c87-a98a-535dd70bbb7c)
旋转套接字直到摄像机朝前且直立。如果您使用的是Unreal Engine人体模型或类似模型，您可以直接使用这些值作为套接字的旋转：(Pitch=-0.000000,Yaw=89.999420,Roll=-90.000000)，摄像机将正确朝向。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/568af692-5705-4d61-9eb2-1dbbf0c480e2)

### 混合遮罩
默认情况下，示例使用混合遮罩与每骨骼分层混合，而不是使用过滤器。要创建混合遮罩，请转到您的骨架，单击齿轮图标，然后将鼠标悬停在混合配置文件上并选择添加混合遮罩。我们将把这个混合遮罩命名为ik_hand_gun。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/2f50fd46-3eab-4114-a547-f40800c6d2ae)
我们希望所有上半身部分（包括ik_hand_X骨骼）的值为1，下半身为0。将ik_hand_root的值设为1.0，然后右键单击并单击递归设置混合比例为1。对spine_01执行相同操作，完成后即可保存。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/a910daeb-a1a8-4f6f-8554-df3871b6328c)
在更改混合遮罩后，请确保在animbp上点击编译，因为它似乎不会实时反映更改。

现在骨架设置完成，您可以继续设置您的[animbp和动画层](Animation-Blueprint-%7C-Animation-Layer)。