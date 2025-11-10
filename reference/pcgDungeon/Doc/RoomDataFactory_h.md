# RoomDataFactory.h

## 概述

`RoomDataFactory.h` 定义了 `URoomDataFactory` 类。这是一个 Unreal Engine 的 `UFactory` 子类，用于在编辑器的“内容浏览器”(Content Browser) 中创建新的 `URoomData` 资产。与 `UDoorTypeFactory` 不同，`URoomDataFactory` 允许用户在创建时选择 `URoomData` 的具体子类。

## 类定义

### URoomDataFactory

此类继承自 `UFactory`，专门用于创建 `URoomData` 对象。

#### 构造函数

- `URoomDataFactory()`: 构造函数，设置工厂的基本属性。
  - `bCreateNew = true;`: 表明这是一个创建新对象的工厂。
  - `bEditAfterNew = true;`: 表明创建新对象后应立即在属性编辑器中打开进行编辑。
  - `SupportedClass = URoomData::StaticClass();`: 指定此工厂支持创建的基类是 `URoomData`。

#### 重写的基类方法

- `virtual uint32 GetMenuCategories() const override;`: 返回此工厂在“新建资产”菜单中应归属的分类。此实现从 `FProceduralDungeonEditorModule` 获取插件注册的分类。
- `virtual bool ConfigureProperties() override;`: 在创建对象之前调用，用于配置创建参数。对于 `URoomDataFactory`，这是关键方法，它会弹出一个类选择器对话框，让用户选择要创建的 `URoomData` 的具体子类。
- `virtual UObject* FactoryCreateNew(...) override;`: 核心的创建函数。根据 `ConfigureProperties` 中选择的类（存储在 `RoomDataClass` 中）来创建 `URoomData` 对象实例。

#### 受保护成员

- `TSubclassOf<class URoomData> RoomDataClass;`: 存储用户在 `ConfigureProperties` 中选择的 `URoomData` 子类。

URoomDataFactory 是一个 Unreal Engine Factory 类，它的主要作用是在 Unreal Editor 的内容浏览器 (Content Browser) 中，为你创建新的 URoomData 资产提供支持和定制化的流程。

  1. 定义与本质

   * 继承自 `UFactory`: 这表明它是一个专门用于在编辑器中“制造”（Create）新 UObject 资产的类。
   * 目标资产类型: 它的 SupportedClass 被设置为 URoomData::StaticClass()，意味着它专门用来创建 URoomData（或其子类）的实例。

  2. 核心功能与用途

  URoomDataFactory 最关键的功能是：在用户通过内容浏览器创建新的 `URoomData` 资产时，允许用户选择一个具体的 `URoomData` 子类。

   * 为什么需要这个功能？
       * URoomData 通常被设计为一个基类。开发者往往会创建许多继承自 URoomData 的子类，例如 UTreasureRoomData、UStartRoomData、UBossRoomData
         等，每个子类可能有自己独特的属性或蓝图逻辑。
       * 如果没有 URoomDataFactory，当你右键点击内容浏览器 -> Create Basic Asset -> URoomData 时，编辑器通常只会创建一个默认的、基类的 URoomData 对象。这就不够灵活。
       * URoomDataFactory 通过重写 ConfigureProperties() 方法，插入了一个类选择器对话框，让用户在创建资产时就能指定它应该是哪个具体的子类。

   * 详细流程:
       1. 用户操作: 用户在内容浏览器中右键，选择 Create Basic Asset -> URoomData。
       2. 调用 Factory: Unreal Editor 识别到 URoomData 关联的 Factory 是 URoomDataFactory，于是调用它。
       3. 配置属性 (`ConfigureProperties`): 这是 URoomDataFactory 的核心。
           * 它会弹出一个类选择器对话框 (SClassPickerDialog)。
           * 这个对话框会列出所有可用的 URoomData 子类（通过内部的 FRoomDataClassFilter 进行过滤，确保只显示 URoomData 及其子类，并排除抽象类等）。
           * 用户在这个对话框中选择他们想要创建的具体子类（例如 UTreasureRoomData）。
           * 用户点击 "OK" 后，这个选定的子类会被存储在 URoomDataFactory 的 RoomDataClass 成员变量中。
           * 如果用户点击 "Cancel"，则创建过程被取消。
       4. 创建资产 (`FactoryCreateNew`):
           * 如果 ConfigureProperties 成功（用户点了 OK），编辑器会接着调用 URoomDataFactory 的 FactoryCreateNew 方法。
           * 在这个方法里，它会检查 RoomDataClass 成员变量是否已经被设置（即，用户是否选择了子类）。
           * 如果设置了，它就会使用 NewObject<YourSelectedSubclass>(...) 来创建用户选定的那个具体子类的实例。
           * 如果 ConfigureProperties 没有被调用或者用户取消了，它可能会回退到创建默认的 URoomData 基类实例（尽管通常 ConfigureProperties 返回 false
             时，整个流程会停止）。

   * 与 `UProceduralDungeonEditorSettings` 的集成:
       * 为了让创建流程更智能，URoomDataFactory 还会读取插件的编辑器设置 (UProceduralDungeonEditorSettings)。
       * 它会检查 DefaultRoomDataClass 设置项，这可以指定一个默认的子类。
       * 它还会检查 bUseDefaultIfNoChild 设置项。如果这个选项为 true，并且 DefaultRoomDataClass 没有子类（即它是最终类），那么 URoomDataFactory
         会跳过类选择器对话框，直接使用 DefaultRoomDataClass 来创建资产，简化了用户的操作。

  3. 与其他 Factory 的对比

   * `UDoorTypeFactory`: 这个工厂用来创建 UDoorType 资产。它的流程相对简单，因为它没有 ConfigureProperties 的重写，通常直接创建 UDoorType
     基类实例。它不提供选择子类的功能，因为 UDoorType 可能设计上就不需要很多子类。

  总结:

  URoomDataFactory 的主要作用是增强 `URoomData` 
  资产的创建体验。它通过提供一个类选择器，让用户在创建资产时就能指定其具体类型（子类），从而避免了总是创建基类实例再手动更改类别的麻烦。这极大地提高了使用 URoomData
  及其各种子类进行地牢设计时的效率和便利性。它是连接编辑器 UI 和 URoomData 类层次结构的桥梁。