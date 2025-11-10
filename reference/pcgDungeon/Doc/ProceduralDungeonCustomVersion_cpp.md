# ProceduralDungeonCustomVersion.cpp

该文件用于定义和注册 `ProceduralDungeon` 插件的自定义序列化版本。

## 主要功能

- **定义 GUID**：为插件的自定义版本定义一个唯一的 GUID。
- **注册自定义版本**：将自定义版本注册到引擎的核心版本系统中。

## 详细实现

### GUID 定义

- `const FGuid FProceduralDungeonCustomVersion::GUID(0x07E63959, 0x72E5DEE1, 0x7B00F72A, 0x1B442349);`
  - 定义了 `FProceduralDungeonCustomVersion` 结构体的 GUID。这个 GUID 是唯一的，用于标识这个特定的自定义版本。

### 自定义版本注册

- `FCustomVersionRegistration GRegisterDungeonCustomVersion(FProceduralDungeonCustomVersion::GUID, FProceduralDungeonCustomVersion::LatestVersion, TEXT("ProcDungeonVer"));`
  - 使用 `FCustomVersionRegistration` 类将自定义版本注册到引擎中。
  - 第一个参数是自定义版本的 GUID。
  - 第二个参数是最新版本号，从 `FProceduralDungeonCustomVersion::LatestVersion` 获取。
  - 第三个参数是版本的名称，这里命名为 "ProcDungeonVer"。

这个文件与 `ProceduralDungeonCustomVersion.h` 头文件配合使用，共同管理插件的序列化版本控制，确保在不同版本的插件之间能够正确地加载和保存数据。