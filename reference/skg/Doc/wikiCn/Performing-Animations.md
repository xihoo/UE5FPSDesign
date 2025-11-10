默认情况下，示例内容中提供的动画层(Anim Layer)是为叠加动画(Additive Animations)而设置的，例如重载动画。如果您不想使用叠加动画，我也会在介绍完叠加动画后展示如何设置。在本说明中，我们将以重载动画为例。

## 叠加动画(Additive Animations)
如果您不熟悉Unreal Engine中的叠加动画，建议您先查找相关资料以获得更深入的理解。叠加动画是在当前动画之上播放的动画。例如，假设您有一个重载动画（没有混合到身体特定部位），您希望在应用脊柱旋转（如向上/向下看）之后播放。如果您低头看地面并开始重载，非叠加动画会让您站直并向前看直到动画完成。但如果重载动画是叠加的，您将从当前"姿势"开始执行重载，并继续如预期般低头。

### 将动画设置为叠加动画
在此示例中，我们将使用重载动画。您可以参考示例项目中的A_Manny_M4_Reload动画。加载您的动画（非蒙太奇），向下滚动直到看到"Additive Settings"类别。在此处您需要将"Additive Anim Type"设置为"Mesh Space"，然后为"Base Pose Type"选择您想要的选项。我建议的起点是使用空闲动画的第一帧或重载动画的第一帧。最简单的设置是使用"Frame from this animation"选项，并将"Ref Frame Index"设置为0。这将使动画使用重载动画的第一帧作为起始点，其他所有内容都在其上播放。  
![image](https://github.com/user-attachments/assets/4906d3ba-bd5d-4d63-b6c2-aca49b71e94e)  
另一种方法是将"Base Pose Type"设置为"Selected animation frame"，然后将动画设置为空闲姿势的第0帧。  
![image](https://github.com/user-attachments/assets/de15f228-cd2b-452a-a2c5-477cf811899b)  
现在您的动画已经是叠加动画了，您可以设置蒙太奇。

### 设置蒙太奇
在包含叠加动画的蒙太奇中，您会注意到动画预览看起来很奇怪且不正常。您需要向下滚动到资源详情中的"Additive Settings"。您需要将其设置为空闲动画。  
![image](https://github.com/user-attachments/assets/3becc6ef-5f88-4502-b7b3-148497df6e53)  
这将使您的动画预览看起来正常。  
之前 ![image](https://github.com/user-attachments/assets/d5ad0c9f-09a5-4fb6-a684-15af0787034a)  
之后 ![image](https://github.com/user-attachments/assets/ea16e013-0ddb-470f-b188-9dcd348cc552)

## 非叠加动画
如果您不想使用叠加动画，只需进行一些小的更改。

### 蒙太奇
在蒙太奇中，为了本示例创建了一个名为DocExample的新插槽，并将重载蒙太奇设置为使用它。  
![image](https://github.com/user-attachments/assets/6aa81e04-3c3f-4efe-8703-d0026d1c7a92)  

### 动画蓝图(AnimBP)
正在进行中  
在动画蓝图中，我们需要创建一个名为DocExample的新插槽（在此处使用您的插槽），并将其定位在程序层之前，如下所示：  
![image](https://github.com/user-attachments/assets/71f4562e-ede2-4fd6-a548-be82ed7792cf)