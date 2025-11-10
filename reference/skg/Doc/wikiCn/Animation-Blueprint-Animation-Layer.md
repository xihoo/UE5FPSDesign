# 操作顺序

我所说的操作顺序是指动画图表的布局顺序。关于 blend mask 与 ik_hand_gun 骨骼以及上下半身分割存在一个问题。问题是当所有程序化动画都通过 ik_hand_x 骨骼执行时，一切工作正常，直到你开始分割上下半身。尽管 blend mask 设置为下半身忽略 ik_hand_X 骨骼，但它仍然会影响它们，导致不良结果。解决方案是在头部使用 virtual bones，并将程序化动画应用于这些骨骼，然后在上下半身分割后，将 ik_hand_gun 复制到 virtual bone。纠正这个问题的顺序是：首先将所有程序化动画应用于 virtual bones，然后执行上下半身分割，最后将 IK 应用于 ik_hand_X 骨骼。

# AnimBP 概述

在示例项目中，默认情况下 AnimBP 使用 linked Anim Layer。如果你不确定什么是 linked Anim Layer，我建议研究一下它们，因为它们非常有用，你可能想在自己的系统中使用它们。示例中的 linked Anim Layer 运行所有程序化逻辑。流程如下：你有基础动画（如待机动画），它被传递到 linked Anim Layer。linked Anim Layer 执行程序化步骤。linked Anim Layer 的输出然后被分割到一个按骨骼分层的混合中，该混合分离上下半身（用于移动）。最后应用 IK，因此 ik_hand_gun 复制 VB ik_hand_gun 骨骼，hand_r 和 hand_l 使用 two bone IK 移动到 ik_hand_r 和 ik_hand_l 骨骼，使手处于正确位置。如果你不想从头创建下面的 Anim Layer 和 AnimBP，你可以始终进行重定向。

# Anim Layer Interface

创建一个新的 anim layer interface 并向其中添加一个新的 Animation Layer。示例内容称其为 ALI_SKGSFExample，因此为便于使用示例项目作为参考，我们将使用相同的名称。在示例中，该层称为 SKGSFProcedurals，因此我们将使用该名称。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/b434a6dd-1616-4eec-b407-bddd50de09f4)

# Anim Layer

创建一个新的 Anim Layer。右键单击并悬停在 Animation 上，然后选择 Animation Blueprint。对于 Specific Skeleton，选择你要使用的骨骼。对于 Parent Class，选择 SKGShooterFrameworkAnimInstance。这是将驱动 Anim Layers 逻辑的 Anim Instance。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/cab65421-aea4-42c0-bd2b-2cf5e057595f)
打开你的 Anim Layer 并转到顶部的 Class Settings，然后在 Implemented Interfaces 下点击 Add 并选择 ALI SKGSFExample。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/e6a1f7c4-46d5-4ef4-acb5-fb77484cf2a2)
现在在左侧的 Animation Layers 下，我们有一个新的 Animation Layers 选项，称为 SKGSFProcedurals，这是我们在 Anim Layer Interface 中添加的 Anim Layer。双击打开。这里我们将添加来自示例的所有程序化动画图表逻辑，可以复制/粘贴进去。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/797e611b-332a-4d73-8e34-40b26645aee7)
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/2f1c6162-f3c6-42ae-8dbe-78a506be6227)
最后，对于这个 anim layer，我们要确保启用 Use Main Instance Montage Evaluation Data。这使得在主 anim instance 上播放的 montages 也能在 linked anim layer 上执行。点击 Class Defaults 并在底部启用 Use Main Instance Montage Evaluation Data。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/437eba46-cdf0-48cd-887a-8c089619dba4)

# Main AnimBP

Linked anim layer 大大简化和清理了主要的 AnimBP，使其不再那么令人生畏，更易于工作。我们要做的是在我们的 AnimBP 中通过单击 ANIMATION LAYERS 旁边的 + 创建一个 Animation Layer。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/0e0360d9-abf7-4bee-9168-2b97b3b8206a)
按照示例项目的命名，我们将此命名为 SKGSFProcedurals。
现在为了将我们的 Animation Layer 设置为我们想要的层，我们需要包含我们的 [Anim Layer Interface](#anim-layer-interface)。单击 Class Settings 并向下滚动到 Implemented Interfaces，单击 Add 并选择我们的 ALI SKGSFExample，如下所示。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/30382b8b-5777-4c5a-bcc2-5990808be512)
现在我们可以将其拖放到我们的 AnimGraph 中以便使用，如下所示。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/19ec4a8e-d273-473c-87c2-604a991163db)

## 添加最终 IK 节点

继续添加一个 Copy Bone 节点和两个 Two Bone IK 节点。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/4ccb60ce-0108-4bdd-a5d3-2f7543990d2c)
#### Copy Bone
此节点用于将我们的 ik_hand_gun 骨骼从程序化动画中定位到正确位置。我们希望 Source Bone 为 VB ik_hand_gun，Target Bone 为 ik_hand_gun，并且我们希望 Copy Translation 和 Copy Rotation 都为 true。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/507889b2-04f4-4830-ba44-be4f601f5952)
对于第一个 Two Bone IK 节点，我们希望 IKBone 为 hand_r。我们希望将 Effector Location Space 设置为 Bone Space，启用 Take Rotation From Effector Space，并且我们希望 Effector Target 为 ik_hand_r。向下移动，我们希望 Joint Target Location Space 为 Bone Space，Joint Target 为 upperarm_r。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/94b62d62-3969-427e-b3ab-e4a15d11e1e5)
第二个 Two Bone IK 节点与第一个相同，但是用于左侧。我们希望 IKBone 为 hand_l。我们希望将 Effector Location Space 设置为 Bone Space，启用 Take Rotation From Effector Space，并且我们希望 Effector Target 为 ik_hand_l。向下移动，我们希望 Joint Target Location Space 为 Bone Space，Joint Target 为 upperarm_l。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/7780ee00-644b-4151-924c-e40746aac7ab)

### 连接

现在我们将使用[上面的 IK 节点](#add-final-ik-nodes)尽可能简单地设置它。我们将放置我们的待机动画（这只是简单持握 M4 的待机动画），将其插入我们的 SKGSFProcedurals Animation Layer，然后插入 Copy Bone 节点，再插入 Two Bone IK 节点，再插入另一个 Two Bone IK 节点，最后插入输出姿势。所以总共有 5 个节点（不包括空间转换节点）。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/436ea59d-0872-461c-aee1-75907083af99)

### 链接 Anim Layer

必须执行链接，如果不这样做，SKGSFProcedurals Animation Layer 将不会运行。有两种方法可以做到这一点。如果你希望它从一开始就运行（基本上是在 pawn 生成时），我建议设置我的组件为你自动启用/链接。要做到这一点，转到你的 pawn 并选择 SKGShooterPawn 组件。在细节面板的 Initialize 类别下，你会看到 Linked Anim Layer Class。将其设置为你制作的 [Anim Layer BP](#anim-layer)。然后确保启用 Auto Setup Linked Anim Layer，如下所示。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/45133aeb-0461-440a-8034-5bc1788488ec)
另一种选择是稍后手动进行。我有你可以调用的函数来执行此操作。首先你要确保禁用 Auto Setup Linked Anim Layer，这样它就不会尝试自动应用，然后你要确保设置了 Linked Anim Layer Class。现在你可以从 SKGShooterPawnComponent 调用 LinkAnimLayerClass 或 LinkAnimLayerClassByInstance。LinkAnimLayerClass 会为你将 Linked Anim Layer 应用到适当的网格，而 LinkAnimLayerClassByInstance 用于如果你想对哪个 Anim Instance 应用 LinkedAnimLayer 有更多的手动控制。同样也有 Unlink 函数。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/dae310cf-9b58-4629-aec5-b6ff531eef01)

### 调整 Joint Target Location

现在我们有了基本设置并且 IK 已配置，我们将要调整我们的 Joint Target Location。我们将从右手开始，所以选择 hand_r 的第一个 Two Bone IK 节点并选择 joint target（它是可见的，可能位于你的肩膀附近）。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/2239102b-f80e-4f97-aaa7-58a9d9d6b793)
通常我们希望这个位置向下、向侧面和向后。这是我在示例项目中用于 hand_r 的值，你可以复制/粘贴：(X=-31.830163,Y=83.032387,Z=-15.502627)。对 hand_l 的第二个 Two Bone IK 节点重复相同的操作，并插入这些值作为良好的起点：(X=-2.987916,Y=-82.034790,Z=20.655229)
这将使你进入类似这样的位置。
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/5b5e43db-b194-4b9e-be8b-d852844a4b50)

### 上下半身分割

通常你会看到人们将上下半身彼此分开。例如，你可能希望 locomotion 驱动你的下半身，同时让持握火器或其他物体的动画驱动上半身。为此，我们将使用我们制作的 [blend mask](#blend-mask)。创建一个 Layered blend per bone 节点，将 Blend Mode 设置为 Blend Mask，然后选择我们创建的 ik_hand_gun blend mask。现在启用 Mesh Space Rotation Blend。在示例项目中，我有一个简单的由速度和方向驱动的 locomotion blend space。这些按照 youtube 上的教程很容易制作，但如果你想要，你可以从 Epic 随 Unreal Engine 提供的 Third Person Example Template 中获取一个简单的。我们要做的是将我们的 locomotion 插入 Base Pose，这是用于下半身的。然后我们将我们的 SKGSFProcedurals Anim Layer 插入 Blend Poses 0，这是上半身，如下所示：
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6de7a4b1-864e-4ec4-ba67-97839cee6816)
现在当你移动时，你的下半身将执行 locomotion。例如，这是我向前和向右行走
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c906a6e9-d965-4842-bc79-c97d1808aac5)