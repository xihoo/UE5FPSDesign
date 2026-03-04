# 游戏启动Menu流程设计

本文档详细说明如何确保游戏开始时正确加载并显示Menu系统，包括初始化时机、层级创建、错误处理等关键问题。

---

## 目录

1. [整体流程](#整体流程)
2. [初始化阶段](#初始化阶段)
3. [World准备阶段](#world准备阶段)
4. [Menu显示阶段](#menu显示阶段)
5. [进入游戏阶段](#进入游戏阶段)
6. [错误处理与恢复](#错误处理与恢复)
7. [调试与日志](#调试与日志)

---

## 整体流程

```
游戏启动Menu完整流程

┌─────────────────────────────────────────────────────────────────────────────┐
│                              游戏启动                                        │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          阶段1: GameInstance创建                             │
│  - 创建GameInstance                                                           │
│  - 调用Init()                                                                 │
│  - 初始化MenuManager (Subsystem)                                              │
│  - 加载SaveGame设置                                                           │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          阶段2: World加载                                    │
│  - UGameInstance::StartGameInstance()                                         │
│  - 创建World                                                                  │
│  - World初始化完成 (OnPostWorldInitialization)                               │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          阶段3: MenuLayer创建                                │
│  - 创建MenuLayer Widget                                                       │
│  - 创建PopupLayer Widget                                                      │
│  - 添加到Viewport                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          阶段4: 显示主菜单                                    │
│  - 推入MainMenu Widget                                                        │
│  - 设置输入模式为UIOnly                                                       │
│  - 设置焦点                                                                   │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          阶段5: 等待用户操作                                  │
│  - 点击"新游戏"/"继续游戏"                                                     │
│  - 或点击"设置"等其他按钮                                                      │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          阶段6: 进入游戏                                      │
│  - 关闭主菜单                                                                 │
│  - 切换到Gameplay模式                                                         │
│  - 生成Player                                                                 │
│  - 开始游戏                                                                   │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 初始化阶段

### 1. GameInstance初始化

```cpp
// FPSGameInstance.h
UCLASS()
class FPSGAME_API UFPSTGameInstance : public UGameInstance
{
    GENERATED_BODY()
    
public:
    virtual void Init() override;
    virtual void StartGameInstance() override;
    virtual void Shutdown() override;
    
private:
    // 标记是否已初始化Menu
    bool bMenuInitialized = false;
    
    // 延迟初始化Menu的句柄
    FTimerHandle MenuInitTimerHandle;
    
    // 初始化Menu系统
    void InitializeMenuSystem();
    
    // 检查World是否准备好
    bool IsWorldReadyForMenu() const;
};
```

```cpp
// FPSGameInstance.cpp
#include "FPSGameInstance.h"
#include "UI/Menu/FPSMenuManager.h"

void UFPSTGameInstance::Init()
{
    Super::Init();
    
    UE_LOG(LogTemp, Log, TEXT("GameInstance::Init - 开始初始化Menu系统"));
    
    // 1. 初始化MenuManager (Subsystem会自动创建)
    if (UFPSMenuManager* MenuManager = GetSubsystem<UFPSMenuManager>())
    {
        // MenuManager会在其Initialize中加载设置
        UE_LOG(LogTemp, Log, TEXT("MenuManager已创建，等待World准备"));
    }
    
    // 2. 绑定World初始化事件
    FWorldDelegates::OnPostWorldInitialization.AddUObject(
        this, 
        &UFPSTGameInstance::OnPostWorldInitialization
    );
    
    // 3. 监听World清理事件 (返回主菜单时)
    FWorldDelegates::OnWorldCleanup.AddUObject(
        this, 
        &UFPSTGameInstance::OnWorldCleanup
    );
}

void UFPSTGameInstance::StartGameInstance()
{
    Super::StartGameInstance();
    
    UE_LOG(LogTemp, Log, TEXT("GameInstance::StartGameInstance - 游戏实例启动"));
    
    // 此时World已经开始加载，但可能还未完全准备好
    // 实际的Menu初始化将在OnPostWorldInitialization中进行
}

void UFPSTGameInstance::OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
    UE_LOG(LogTemp, Log, TEXT("World初始化完成: %s"), *World->GetName());
    
    // 检查是否是游戏World (不是编辑器World)
    if (World->IsGameWorld() && !World->IsPreviewWorld())
    {
        // 延迟一帧初始化Menu，确保所有Subsystem都准备好
        GetTimerManager().SetTimerForNextTick([this, World]()
        {
            InitializeMenuSystem();
        });
    }
}
```

### 2. MenuManager初始化

```cpp
// FPSMenuManager.cpp
void UFPSMenuManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogTemp, Log, TEXT("MenuManager::Initialize"));
    
    // 1. 加载设置
    GameSettings = UFPSGameSettings::LoadOrCreateSettings();
    if (GameSettings)
    {
        GameSettings->ApplyAllSettings();
        UE_LOG(LogTemp, Log, TEXT("游戏设置已加载，语言: %s"), *GameSettings->Language);
    }
    
    // 2. 此时不创建MenuLayer，因为World可能还没准备好
    // MenuLayer将在SetupMenuLayers中创建
}

void UFPSMenuManager::SetupMenuLayers()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("SetupMenuLayers失败: World为空"));
        return;
    }
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("SetupMenuLayers失败: PlayerController为空"));
        return;
    }
    
    // 检查是否已创建
    if (MenuLayer && PopupLayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("MenuLayer已存在，跳过创建"));
        return;
    }
    
    // 创建MenuLayer容器 (使用Stack模式)
    if (MenuLayerClass)
    {
        MenuLayer = CreateWidget<UCommonActivatableWidgetStack>(PC, MenuLayerClass);
        if (MenuLayer)
        {
            MenuLayer->AddToViewport(100); // ZOrder = 100
            UE_LOG(LogTemp, Log, TEXT("MenuLayer已创建并添加到Viewport"));
        }
    }
    
    // 创建PopupLayer容器
    if (PopupLayerClass)
    {
        PopupLayer = CreateWidget<UCommonActivatableWidgetStack>(PC, PopupLayerClass);
        if (PopupLayer)
        {
            PopupLayer->AddToViewport(200); // ZOrder = 200，在MenuLayer之上
            UE_LOG(LogTemp, Log, TEXT("PopupLayer已创建并添加到Viewport"));
        }
    }
    
    // 标记已初始化
    bLayersInitialized = true;
    
    // 广播事件
    OnMenuLayersReady.Broadcast();
}
```

---

## World准备阶段

### 处理World未准备好的情况

```cpp
void UFPSTGameInstance::InitializeMenuSystem()
{
    if (bMenuInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("Menu系统已初始化，跳过"));
        return;
    }
    
    // 检查World是否真正准备好
    if (!IsWorldReadyForMenu())
    {
        UE_LOG(LogTemp, Warning, TEXT("World未准备好，延迟初始化"));
        
        // 延迟100ms再次尝试
        GetTimerManager().SetTimer(
            MenuInitTimerHandle,
            this,
            &UFPSTGameInstance::InitializeMenuSystem,
            0.1f,
            false
        );
        return;
    }
    
    // 获取MenuManager并初始化MenuLayer
    if (UFPSMenuManager* MenuManager = GetSubsystem<UFPSMenuManager>())
    {
        MenuManager->SetupMenuLayers();
        
        // 延迟一帧显示主菜单，确保Widget已完全初始化
        GetTimerManager().SetTimerForNextTick([MenuManager]()
        {
            MenuManager->OpenMainMenu();
        });
        
        bMenuInitialized = true;
        UE_LOG(LogTemp, Log, TEXT("Menu系统初始化完成"));
    }
}

bool UFPSTGameInstance::IsWorldReadyForMenu() const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }
    
    // 检查是否有有效的PlayerController
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC)
    {
        return false;
    }
    
    // 检查PlayerController是否有LocalPlayer
    if (!PC->GetLocalPlayer())
    {
        return false;
    }
    
    // 检查ViewportClient
    if (!World->GetGameViewport())
    {
        return false;
    }
    
    return true;
}
```

---

## Menu显示阶段

### 1. 打开主菜单

```cpp
void UFPSMenuManager::OpenMainMenu()
{
    // 确保MenuLayer已创建
    if (!MenuLayer)
    {
        UE_LOG(LogTemp, Error, TEXT("OpenMainMenu失败: MenuLayer未创建"));
        SetupMenuLayers();
        
        if (!MenuLayer)
        {
            UE_LOG(LogTemp, Fatal, TEXT("无法创建MenuLayer，无法显示主菜单"));
            return;
        }
    }
    
    // 确保使用正确的输入模式
    SwitchToUIMode();
    
    // 推入主菜单
    if (MainMenuClass)
    {
        PushMenu(MainMenuClass);
        UE_LOG(LogTemp, Log, TEXT("主菜单已打开"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("MainMenuClass未设置"));
    }
}
```

### 2. 输入模式切换

```cpp
void UFPSMenuManager::SwitchToUIMode()
{
    UWorld* World = GetWorld();
    if (!World) return;
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;
    
    // 缓存游戏输入模式
    if (!bCachedInputMode)
    {
        CachedInputMode = PC->PlayerInput->GetInputMode();
        bCachedMouseVisibility = PC->bShowMouseCursor;
        bCachedInputMode = true;
    }
    
    // 添加UI输入映射上下文 (高优先级)
    if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = 
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (UIMappingContext)
            {
                InputSubsystem->AddMappingContext(UIMappingContext, 10);
            }
        }
    }
    
    // 设置输入模式为UIOnly
    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(InputMode);
    PC->SetShowMouseCursor(true);
    
    // 暂停游戏 (主菜单通常不需要，但暂停菜单需要)
    // SetGamePaused(true);
}
```

---

## 进入游戏阶段

### 1. 关闭主菜单开始游戏

```cpp
void UFPSMenuManager::CloseMainMenuAndStartGame()
{
    UE_LOG(LogTemp, Log, TEXT("关闭主菜单，开始游戏"));
    
    // 播放关闭动画 (可选)
    if (UFPSMenuBase* CurrentMenu = GetCurrentMenu())
    {
        CurrentMenu->PlayCloseAnimation();
    }
    
    // 延迟关闭，等待动画完成
    FTimerHandle CloseTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        CloseTimerHandle,
        [this]()
        {
            // 清空Menu栈
            PopToRoot();
            
            // 切换到游戏模式
            SwitchToGameplayMode();
            
            // 广播游戏开始事件
            OnGameStarted.Broadcast();
        },
        0.3f, // 等待动画时间
        false
    );
}

void UFPSMenuManager::SwitchToGameplayMode()
{
    UWorld* World = GetWorld();
    if (!World) return;
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;
    
    // 移除UI输入映射上下文
    if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = 
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (UIMappingContext)
            {
                InputSubsystem->RemoveMappingContext(UIMappingContext);
            }
        }
    }
    
    // 恢复游戏输入模式
    if (bCachedInputMode)
    {
        PC->SetInputMode(CachedInputMode);
        PC->SetShowMouseCursor(bCachedMouseVisibility);
    }
    else
    {
        // 默认游戏模式
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->SetShowMouseCursor(false);
    }
    
    // 恢复游戏
    SetGamePaused(false);
}
```

---

## 错误处理与恢复

### 1. 多层级检查机制

```cpp
// FPSMenuManager.h
UCLASS()
class FPSGAME_API UFPSMenuManager : public UGameInstanceSubsystem
{
    // ...
    
public:
    /** 检查Menu系统是否完全初始化 */
    UFUNCTION(BlueprintPure, Category = "Menu|Debug")
    bool IsMenuSystemReady() const;
    
    /** 强制重新初始化Menu系统 (用于恢复) */
    UFUNCTION(BlueprintCallable, Category = "Menu|Debug")
    void ForceReinitializeMenuSystem();
    
    /** 获取初始化状态详情 */
    UFUNCTION(BlueprintPure, Category = "Menu|Debug")
    FString GetInitStatusString() const;
    
private:
    // 初始化状态
    UPROPERTY()
    bool bLayersInitialized = false;
    
    UPROPERTY()
    bool bCachedInputMode = false;
    
    FInputModeDataBase CachedInputMode;
    bool bCachedMouseVisibility = false;
    
    // 最大重试次数
    static constexpr int32 MAX_INIT_RETRY = 10;
    int32 InitRetryCount = 0;
};
```

```cpp
// FPSMenuManager.cpp
bool UFPSMenuManager::IsMenuSystemReady() const
{
    if (!MenuLayer)
    {
        return false;
    }
    
    if (!PopupLayer)
    {
        return false;
    }
    
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC)
    {
        return false;
    }
    
    return true;
}

FString UFPSMenuManager::GetInitStatusString() const
{
    FString Status;
    Status += FString::Printf(TEXT("MenuLayer: %s\n"), MenuLayer ? TEXT("OK") : TEXT("NULL"));
    Status += FString::Printf(TEXT("PopupLayer: %s\n"), PopupLayer ? TEXT("OK") : TEXT("NULL"));
    Status += FString::Printf(TEXT("GameSettings: %s\n"), GameSettings ? TEXT("OK") : TEXT("NULL"));
    Status += FString::Printf(TEXT("World: %s\n"), GetWorld() ? TEXT("OK") : TEXT("NULL"));
    
    if (GetWorld())
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        Status += FString::Printf(TEXT("PlayerController: %s"), PC ? TEXT("OK") : TEXT("NULL"));
    }
    
    return Status;
}

void UFPSMenuManager::ForceReinitializeMenuSystem()
{
    UE_LOG(LogTemp, Warning, TEXT("强制重新初始化Menu系统"));
    
    // 清理现有层
    if (MenuLayer)
    {
        MenuLayer->RemoveFromParent();
        MenuLayer = nullptr;
    }
    
    if (PopupLayer)
    {
        PopupLayer->RemoveFromParent();
        PopupLayer = nullptr;
    }
    
    bLayersInitialized = false;
    
    // 重新初始化
    SetupMenuLayers();
}
```

### 2. 蓝图可访问的调试功能

```cpp
// 在GameInstance中添加调试图表命令
void UFPSTGameInstance::CreateDebugCommands()
{
#if WITH_CHEAT_MANAGER
    // 控制台命令: Menu.ForceInit
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("Menu.ForceInit"),
        TEXT("强制重新初始化Menu系统"),
        FConsoleCommandDelegate::CreateLambda([this]()
        {
            if (UFPSMenuManager* MenuManager = GetSubsystem<UFPSMenuManager>())
            {
                MenuManager->ForceReinitializeMenuSystem();
            }
        }),
        ECVF_Cheat
    );
    
    // 控制台命令: Menu.Status
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("Menu.Status"),
        TEXT("显示Menu系统状态"),
        FConsoleCommandDelegate::CreateLambda([this]()
        {
            if (UFPSMenuManager* MenuManager = GetSubsystem<UFPSMenuManager>())
            {
                UE_LOG(LogTemp, Log, TEXT("=== Menu系统状态 ==="));
                UE_LOG(LogTemp, Log, TEXT("%s"), *MenuManager->GetInitStatusString());
            }
        }),
        ECVF_Cheat
    );
#endif
}
```

---

## 调试与日志

### 1. 关键日志输出

```cpp
// 在关键位置添加日志

// GameInstance.cpp
void UFPSTGameInstance::Init()
{
    UE_LOG(LogMenuSystem, Log, TEXT("========================================"));
    UE_LOG(LogMenuSystem, Log, TEXT("GameInstance::Init - 游戏实例初始化开始"));
    UE_LOG(LogMenuSystem, Log, TEXT("========================================"));
}

void UFPSTGameInstance::OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
    UE_LOG(LogMenuSystem, Log, TEXT("World初始化完成: %s (GameWorld: %s)"), 
        *World->GetName(),
        World->IsGameWorld() ? TEXT("Yes") : TEXT("No"));
}

// MenuManager.cpp
void UFPSMenuManager::SetupMenuLayers()
{
    UE_LOG(LogMenuSystem, Log, TEXT("SetupMenuLayers - 开始创建Menu层级"));
    
    // ...
    
    if (MenuLayer)
    {
        UE_LOG(LogMenuSystem, Log, TEXT("MenuLayer创建成功: %s"), *MenuLayer->GetName());
    }
    else
    {
        UE_LOG(LogMenuSystem, Error, TEXT("MenuLayer创建失败!"));
    }
}

void UFPSMenuManager::OpenMainMenu()
{
    UE_LOG(LogMenuSystem, Log, TEXT("OpenMainMenu - 打开主菜单"));
    
    if (!IsMenuSystemReady())
    {
        UE_LOG(LogMenuSystem, Error, TEXT("Menu系统未准备好，无法打开主菜单!"));
        UE_LOG(LogMenuSystem, Error, TEXT("状态: %s"), *GetInitStatusString());
        return;
    }
    
    // ...
}
```

### 2. 定义日志类别

```cpp
// FPSLogCategories.h
#pragma once

#include "CoreMinimal.h"

// 声明Menu系统日志类别
DECLARE_LOG_CATEGORY_EXTERN(LogMenuSystem, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogLocalization, Log, All);
```

```cpp
// FPSLogCategories.cpp
#include "FPSLogCategories.h"

DEFINE_LOG_CATEGORY(LogMenuSystem);
DEFINE_LOG_CATEGORY(LogLocalization);
```

### 3. 蓝图调试工具

```cpp
UCLASS()
class FPSGAME_API UFPSMenuDebugWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "Menu|Debug")
    void ShowMenuSystemStatus();
    
    UFUNCTION(BlueprintCallable, Category = "Menu|Debug")
    void ForceReinitMenuSystem();
    
    UFUNCTION(BlueprintCallable, Category = "Menu|Debug")
    void OpenMenuByClass(TSubclassOf<UFPSMenuBase> MenuClass);
    
    UFUNCTION(BlueprintCallable, Category = "Menu|Debug")
    void TestPopup(FString Title, FString Message);
};
```

---

## 总结

确保游戏开始时正确进入Menu的关键点：

1. **正确的初始化顺序**
   - GameInstance::Init() → 创建MenuManager
   - OnPostWorldInitialization → 创建MenuLayer
   - 延迟一帧 → 显示主菜单

2. **World准备检查**
   - PlayerController必须存在
   - LocalPlayer必须有效
   - GameViewport必须准备好

3. **错误恢复机制**
   - 重试机制 (延迟初始化)
   - 强制重新初始化
   - 详细的日志输出

4. **调试支持**
   - 控制台命令
   - 状态查询
   - 蓝图调试工具
