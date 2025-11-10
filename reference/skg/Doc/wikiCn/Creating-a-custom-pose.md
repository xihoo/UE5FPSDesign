本节介绍[程序化动画组件](SKGProceduralAnimComponent#pose-settings)的姿态设置。当我提到第三人称时，我指的是处于第三人称模式或查看远程客户端。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/04c4ad53-aa58-4f84-ac7a-e83c571d3c5f)

# 概述
系统非常简单。它使用一组基于游戏标签的姿态数组，每个标签代表一条曲线。这允许你创建无限数量的姿态来做任何你想做的事情，并通过调用带有该标签的函数来执行它们。例如，如果你想进入一个姿态，你可以调用PerformProceduralPose并传入你想要的标签以及是否播放退出姿态。将退出姿态设置为true将执行From曲线，设置为true将执行To曲线。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/ff968d38-7715-4bd4-8360-f81078933134)
姿态设置有两组曲线。To曲线和From曲线。To曲线是进入曲线，例如从待机状态进入高港口状态。From曲线则相反，例如从高港口状态回到待机状态。此外，To和From曲线都有第一人称和第三人称曲线集。这样做的目的是，与其被迫播放在第一人称中看起来不错但在第三人称中看起来糟糕的曲线，不如为两者播放独特的曲线，以在第一人称和第三人称中都获得良好的效果。此外，为了帮助创建这些姿态，创建了一个姿态自定义小部件，你可以通过按Tab键访问
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c806f49c-5ad0-4aa2-b448-635e3cd92349)

# 创建新姿态
在这里我们将从头开始创建一个新姿态。这里的想法是我想简单地旋转火器以查看枪膛，当我完成后，让它恢复正常。

### 创建游戏标签
首先让我们创建定义此姿态的游戏标签。我建议使用数据表来帮助组织你的标签。示例项目有一个名为GDT_ProceduralPoseTags的数据表，我将在此示例中使用它。如果你想创建自己的数据表，请创建一个新数据表，并在提示选择使用哪个结构时，选择GameplayTagTableRow。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/0568e5ae-ae77-4e2c-b0ba-1c3018c330e2)
创建完成后，转到你的项目设置，选择游戏标签，然后在游戏标签表列表下添加一个新元素并选择你刚刚创建的数据表。现在一切都已配置好，让我们创建我们的标签。打开你的数据表并添加一个新标签。我们将其称为ProceduralPose.LookAtChamber。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/78ad6231-8d27-462b-aa4f-7d9dd10338d3)

### 创建To曲线
要创建曲线，我们想在内容浏览器中右键单击，转到杂项，曲线，然后选择CurveVector。让我们复制4次，这样我们总共有4条曲线，分别命名为C_LookAtChamberLocation、C_LookAtChamberRotation、C_LookAtChamberLocationReturn、C_LookAtChamberRotationReturn。现在让我们确定一些值。按下播放键，装备你想要此姿态的火器，然后按Tab键打开姿态自定义小部件。从这里你可以通过在顶部的文本框中输入值来开始调整你的值。我最终得到了位置的-5、7和8，旋转的-55、30和-45。这些将是我位置/旋转曲线的终点。请注意，这假设使用Y向前的火器。使用X向前的火器可能需要考虑该旋转差异。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/a34f7f87-720c-4b37-96e7-63e2d1c03bde)
打开C_LookAtChamberLocation曲线，在第0帧为XYZ设置关键帧，值为0，然后移动到大约第12帧左右，为XYZ添加关键帧，X: -5, Y: 7, Z: 8。现在对C_LookAtChamberRotation做同样的事情，但这个稍微不同。X: -45, Y: 30, Z: -55。这里是位置曲线的示例。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/0fa3ffc1-d243-4964-acb9-9be6d55afe71)

### 配置姿态设置
现在我们想将此添加为火器的姿态。让我们转到火器的ProceduralAnimComponent并在详细信息面板中找到姿态设置。找到曲线标签并将其设置为我们刚刚制作的LookAtChamber标签。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/4c184fc9-bafb-45db-8be1-d8fcaf660a1d)
现在让我们分配曲线进行测试。为了使其工作，To曲线和From曲线都需要设置。此外，我们需要将播放速率设置为非0值，所以我们从4开始，然后曲线结束时间需要设置为曲线的终点。这将是位置曲线或旋转曲线中较长者的最后一帧。在我的情况下是12。现在让我们继续启用插值，将插值时间设置为4，插值速度设置为10，插值速度倍数设置为5。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/cfcb2a6c-d3f6-4bbd-8e81-338a4e806eb9)

### 测试我们的姿态
如果使用示例项目，你可以转到BP_SKGSFExamplePawn并转到火器事件图表。在那里你会看到短枪托、高港口、低港口等的输入。你可以交换其中一个标签或复制其中一个并交换标签。为了简单起见，我将短枪托标签交换为ProceduralPose.LookAtChamber。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/0408ec21-227a-4b0b-b80e-7cfe22941604)

### 调整设置
现在我们已经设置并运行了姿态，我注意到进入姿态相当慢，所以让我们增加速度。这就是播放速率设置的作用，所以让我们转到To曲线并将播放速率增加到22。这是我经过一些调整后觉得合适的速度值。现在我们遇到的问题是曲线只是直线。首先让我们选择起点并将位置和旋转曲线都设置为三次插值。从这里你将想要开始调整中间部分，添加一些额外的关键帧并根据需要进行调整。如果你熟悉的话，这几乎就像在blender的曲线编辑器中工作一样。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/73f4f933-6274-4f98-b81a-f21aa14b98cc)

### 创建From曲线
From曲线非常简单。由于我们已经有了To曲线，我们已经知道要从哪里开始。我们不是从0到X，而是从X到0。所以在第0帧，将你的位置和旋转返回曲线设置为等于To曲线的最后一帧。最简单的方法是我不能确定你是否可以将关键帧复制/粘贴到另一条曲线，就是复制你的图表并像这样交换开始和结束值。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/4a4c7017-fec8-432c-ac0f-67eb291843ce)
你将想要对位置和旋转都这样做。同样，我们将播放速率和曲线结束时间设置为12。