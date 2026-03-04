# UE5 Menu 系统本地化多语言设计方案

本文档定义基于 UE5 官方 Localization 框架的完整多语言本地化方案，与现有 CommonUI Menu 系统无缝集成。

---

## 目录

1. [概述](#概述)
2. [架构设计](#架构设计)
3. [文本本地化](#文本本地化)
4. [运行时语言切换](#运行时语言切换)
5. [字体和UI适配](#字体和ui适配)
6. [音频本地化](#音频本地化)
7. [蓝图最佳实践](#蓝图最佳实践)
8. [C++实现](#c实现)
9. [工作流和工具](#工作流和工具)
10. [目录结构](#目录结构)

---

## 概述

### 支持语言

| 语言代码 | 语言名称 | 优先级 |
|----------|----------|--------|
| `zh-Hans` | 简体中文 | P0 |
| `zh-Hant` | 繁体中文 | P1 |
| `en` | 英语 | P0 |
| `ja` | 日语 | P1 |
| `ko` | 韩语 | P2 |
| `de` | 德语 | P2 |
| `fr` | 法语 | P2 |
| `es` | 西班牙语 | P2 |

### 设计原则

1. **文本与代码分离** - 所有用户可见文本必须走本地化系统
2. **运行时切换** - 无需重启游戏即可切换语言
3. **字体安全** - 确保所有语言的字符都能正确显示
4. **文化适配** - 日期、时间、数字格式本地化
5. **音频分离** - 语音和文本独立管理

---

## 架构设计

### 整体架构图

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         本地化系统架构                                       │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                        FPSLocalizationManager                                │
│  (GameInstanceSubsystem)                                                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │
│  │ Culture      │  │ Text         │  │ Font         │  │ Audio           │ │
│  │ Management   │  │ Localization │  │ Management   │  │ Localization    │ │
│  └──────────────┘  └──────────────┘  └──────────────┘  └─────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
           ┌──────────────────────────┼──────────────────────────┐
           │                          │                          │
           ▼                          ▼                          ▼
┌─────────────────────┐  ┌─────────────────────┐  ┌─────────────────────┐
│   FText (UI文本)     │  │  UCommonTextBlock   │  │  UFontManager       │
│   - NSLOCTEXT()     │  │  - SetText()        │  │  - Dynamic Font     │
│   - LOCTEXT()       │  │  - Auto Localization│  │  - Font Fallback    │
└─────────────────────┘  └─────────────────────┘  └─────────────────────┘
           │                          │                          │
           └──────────────────────────┼──────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                     UE5 Localization Dashboard                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │
│  │ Text Gather  │  │ Translation  │  │ Import/Export│  │ Live Preview    │ │
│  │ (收集文本)    │  │ (翻译)       │  │ (导入导出)   │  │ (实时预览)      │ │
│  └──────────────┘  └──────────────┘  └──────────────┘  └─────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 与Menu系统集成

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     Menu系统与本地化集成                                     │
└─────────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────┐
  │  UFPSGameSettings│◄─────────────────┐
  │  - Language     │                  │
  └────────┬────────┘                  │
           │ 保存到SaveGame            │
           ▼                          │
  ┌─────────────────┐                  │
  │ LocalizationManager                │
  │  - SetCurrentCulture()             │
  │  - ApplyLocalization()             │
  └────────┬────────┘                  │
           │ 切换语言                   │
           ▼                          │
  ┌─────────────────┐                 │
  │  UCommonTextBlock│                │
  │  - RefreshText() │                │
  └────────┬────────┘                 │
           │ 刷新所有文本              │
           ▼                          │
  ┌─────────────────┐                 │
  │  所有菜单Widget  │─────────────────┘
  │  - MainMenu     │ 文本变更回调
  │  - SettingsMenu │
  │  - PauseMenu    │
  └─────────────────┘
```

---

## 文本本地化

### 1. 命名空间(Namespace)规划

| 命名空间 | 用途 | 示例 |
|----------|------|------|
| `Menu` | 主菜单/暂停菜单 | 主菜单按钮、标题 |
| `Settings` | 设置菜单 | 设置项标签、描述 |
| `Graphics` | 图形设置 | 画质选项、分辨率 |
| `Audio` | 音频设置 | 音量控制、音效 |
| `Controls` | 控制设置 | 键位绑定、灵敏度 |
| `Gameplay` | 游戏设置 | 难度、游戏选项 |
| `Popup` | 弹窗文本 | 确认、警告、错误 |
| `Common` | 通用文本 | 是/否、确定/取消 |

### 2. C++代码中的文本定义

```cpp
// FPSLocalizationNamespace.h
#pragma once

// 定义所有本地化命名空间，避免硬编码字符串
namespace FPSLocNS
{
    // 主菜单命名空间
    inline const FString Menu = TEXT("Menu");
    
    // 设置命名空间
    inline const FString Settings = TEXT("Settings");
    inline const FString Graphics = TEXT("Graphics");
    inline const FString Audio = TEXT("Audio");
    inline const FString Controls = TEXT("Controls");
    inline const FString Gameplay = TEXT("Gameplay");
    
    // 弹窗命名空间
    inline const FString Popup = TEXT("Popup");
    
    // 通用命名空间
    inline const FString Common = TEXT("Common");
}

// 定义常用本地化键
namespace FPSLocKey
{
    // Menu
    inline const FName ContinueGame = TEXT("ContinueGame");
    inline const FName NewGame = TEXT("NewGame");
    inline const FName Settings = TEXT("Settings");
    inline const FName Achievements = TEXT("Achievements");
    inline const FName Quit = TEXT("Quit");
    inline const FName Resume = TEXT("Resume");
    inline const FName MainMenu = TEXT("MainMenu");
    inline const FName QuitGame = TEXT("QuitGame");
    
    // Settings Categories
    inline const FName GraphicsSettings = TEXT("GraphicsSettings");
    inline const FName AudioSettings = TEXT("AudioSettings");
    inline const FName ControlSettings = TEXT("ControlSettings");
    inline const FName GameplaySettings = TEXT("GameplaySettings");
    
    // Common
    inline const FName Confirm = TEXT("Confirm");
    inline const FName Cancel = TEXT("Cancel");
    inline const FName Yes = TEXT("Yes");
    inline const FName No = TEXT("No");
    inline const FName Apply = TEXT("Apply");
    inline const FName Reset = TEXT("Reset");
    inline const FName Back = TEXT("Back");
}
```

### 3. C++代码中的文本使用

```cpp
// FPSMenuBase.cpp - 使用NSLOCTEXT宏
void UFPSMenuBase::SetLocalizedTitle(const FName& Key)
{
    if (MenuTitle)
    {
        // 方式1: 使用NSLOCTEXT (编译时确定命名空间)
        FText TitleText = NSLOCTEXT("Menu", "MainMenuTitle", "MAIN MENU");
        MenuTitle->SetText(TitleText);
    }
}

// FPSCommonButton.cpp - 动态设置文本
void UFPSCommonButton::SetLocalizedText(const FString& Namespace, const FName& Key)
{
    if (ButtonText)
    {
        // 方式2: 使用FText::FromStringTable (运行时从String Table加载)
        FText LocalizedText = FText::FromStringTable(
            FName(*Namespace), 
            Key.ToString()
        );
        
        // 方式3: 使用LOCTEXT (当前文件命名空间)
        // 需要在cpp文件顶部定义: #define LOCTEXT_NAMESPACE "Menu"
        // FText Text = LOCTEXT("Key", "Default Value");
        
        SetButtonText(LocalizedText);
    }
}

// 推荐方式: 使用本地化表查找
FText UFPSLocalizationManager::GetLocalizedText(const FString& Namespace, const FName& Key)
{
    // 从String Table获取
    if (const UStringTable* Table = GetStringTable(FName(*Namespace)))
    {
        return FText::FromStringTable(Table->GetStringTableId(), Key.ToString());
    }
    
    // 返回默认文本 (Key本身作为fallback)
    return FText::FromName(Key);
}
```

### 4. 支持的语言代码枚举

```cpp
// FPSLocalizationTypes.h
#pragma once

#include "CoreMinimal.h"
#include "FPSLocalizationTypes.generated.h"

UENUM(BlueprintType)
enum class EGameLanguage : uint8
{
    ChineseSimplified   UMETA(DisplayName = "简体中文"),
    ChineseTraditional  UMETA(DisplayName = "繁體中文"),
    English             UMETA(DisplayName = "English"),
    Japanese            UMETA(DisplayName = "日本語"),
    Korean              UMETA(DisplayName = "한국어"),
    German              UMETA(DisplayName = "Deutsch"),
    French              UMETA(DisplayName = "Français"),
    Spanish             UMETA(DisplayName = "Español"),
    
    Default = English
};

// 语言代码转换
namespace FPSLocalizationHelper
{
    inline FString GetLanguageCode(EGameLanguage Language)
    {
        switch (Language)
        {
        case EGameLanguage::ChineseSimplified:  return TEXT("zh-Hans");
        case EGameLanguage::ChineseTraditional: return TEXT("zh-Hant");
        case EGameLanguage::English:            return TEXT("en");
        case EGameLanguage::Japanese:           return TEXT("ja");
        case EGameLanguage::Korean:             return TEXT("ko");
        case EGameLanguage::German:             return TEXT("de");
        case EGameLanguage::French:             return TEXT("fr");
        case EGameLanguage::Spanish:            return TEXT("es");
        default:                                return TEXT("en");
        }
    }
    
    inline EGameLanguage GetLanguageFromCode(const FString& Code)
    {
        if (Code == TEXT("zh-Hans")) return EGameLanguage::ChineseSimplified;
        if (Code == TEXT("zh-Hant")) return EGameLanguage::ChineseTraditional;
        if (Code == TEXT("en"))      return EGameLanguage::English;
        if (Code == TEXT("ja"))      return EGameLanguage::Japanese;
        if (Code == TEXT("ko"))      return EGameLanguage::Korean;
        if (Code == TEXT("de"))      return EGameLanguage::German;
        if (Code == TEXT("fr"))      return EGameLanguage::French;
        if (Code == TEXT("es"))      return EGameLanguage::Spanish;
        return EGameLanguage::Default;
    }
    
    // 获取语言的本地化显示名称
    inline FText GetLanguageDisplayName(EGameLanguage Language)
    {
        const UEnum* EnumPtr = StaticEnum<EGameLanguage>();
        if (EnumPtr)
        {
            return EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(Language));
        }
        return FText::GetEmpty();
    }
}
```

---

## 运行时语言切换

### 1. 本地化管理器

```cpp
// FPSLocalizationManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FPSLocalizationTypes.h"
#include "FPSLocalizationManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLanguageChanged, EGameLanguage, NewLanguage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLocalizationChanged);

UCLASS()
class FPSGAME_API UFPSLocalizationManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    // ========== 语言设置 ==========
    
    /** 获取当前语言 */
    UFUNCTION(BlueprintPure, Category = "Localization")
    EGameLanguage GetCurrentLanguage() const { return CurrentLanguage; }
    
    /** 获取当前语言代码 */
    UFUNCTION(BlueprintPure, Category = "Localization")
    FString GetCurrentLanguageCode() const;
    
    /** 设置语言 (异步加载，带回调) */
    UFUNCTION(BlueprintCallable, Category = "Localization")
    void SetLanguage(EGameLanguage NewLanguage);
    
    /** 从语言代码设置 */
    UFUNCTION(BlueprintCallable, Category = "Localization")
    void SetLanguageFromCode(const FString& LanguageCode);
    
    /** 获取所有支持的语言 */
    UFUNCTION(BlueprintPure, Category = "Localization")
    TArray<EGameLanguage> GetSupportedLanguages() const;
    
    /** 获取语言的显示名称 */
    UFUNCTION(BlueprintPure, Category = "Localization")
    static FText GetLanguageDisplayName(EGameLanguage Language);
    
    // ========== 文本获取 ==========
    
    /** 从命名空间获取本地化文本 */
    UFUNCTION(BlueprintPure, Category = "Localization", meta = (AutoCreateRefTerm = "Namespace, Key"))
    FText GetLocalizedText(const FString& Namespace, const FString& Key) const;
    
    /** 格式化文本 (支持参数) */
    UFUNCTION(BlueprintPure, Category = "Localization", CustomThunk, meta = (VarArgs = "Args"))
    FText FormatLocalizedText(const FString& Namespace, const FString& Key, const TArray<FText>& Args);
    
    // ========== 字体管理 ==========
    
    /** 获取当前语言的字体 */
    UFUNCTION(BlueprintPure, Category = "Localization|Font")
    UFont* GetCurrentLanguageFont() const;
    
    /** 获取指定语言的字体 */
    UFUNCTION(BlueprintPure, Category = "Localization|Font")
    UFont* GetLanguageFont(EGameLanguage Language) const;
    
    // ========== 事件委托 ==========
    
    /** 语言变更事件 */
    UPROPERTY(BlueprintAssignable, Category = "Localization|Events")
    FOnLanguageChanged OnLanguageChanged;
    
    /** 本地化数据更新事件 (所有文本需要刷新) */
    UPROPERTY(BlueprintAssignable, Category = "Localization|Events")
    FOnLocalizationChanged OnLocalizationChanged;
    
private:
    // 当前语言
    UPROPERTY()
    EGameLanguage CurrentLanguage;
    
    // 字体映射表
    UPROPERTY(EditDefaultsOnly, Category = "Localization", meta = (AllowPrivateAccess = true))
    TMap<EGameLanguage, TSoftObjectPtr<UFont>> LanguageFonts;
    
    // 默认字体
    UPROPERTY(EditDefaultsOnly, Category = "Localization", meta = (AllowPrivateAccess = true))
    TSoftObjectPtr<UFont> DefaultFont;
    
    // 异步加载句柄
    TArray<FStreamableHandle*> AsyncLoadHandles;
    
    // 应用语言变更
    void ApplyLanguageChange(EGameLanguage NewLanguage);
    
    // 加载本地化资源
    void LoadLocalizationAssets(EGameLanguage Language);
    
    // 刷新所有UI文本
    void RefreshAllUIText();
    
    // 从SaveGame加载语言设置
    void LoadLanguageFromSettings();
};
```

### 2. 本地化管理器实现

```cpp
// FPSLocalizationManager.cpp
#include "Localization/FPSLocalizationManager.h"
#include "Kismet/GameplayStatics.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "UI/Menu/FPSGameSettings.h"

void UFPSLocalizationManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    CurrentLanguage = EGameLanguage::Default;
    
    // 从设置加载语言
    LoadLanguageFromSettings();
}

void UFPSLocalizationManager::Deinitialize()
{
    // 取消所有异步加载
    for (auto* Handle : AsyncLoadHandles)
    {
        if (Handle)
        {
            Handle->CancelHandle();
        }
    }
    AsyncLoadHandles.Empty();
    
    Super::Deinitialize();
}

FString UFPSLocalizationManager::GetCurrentLanguageCode() const
{
    return FPSLocalizationHelper::GetLanguageCode(CurrentLanguage);
}

void UFPSLocalizationManager::SetLanguage(EGameLanguage NewLanguage)
{
    if (CurrentLanguage == NewLanguage)
    {
        return;
    }
    
    // 异步加载新语言的资源
    LoadLocalizationAssets(NewLanguage);
    
    // 应用语言变更
    ApplyLanguageChange(NewLanguage);
    
    // 保存到设置
    if (UFPSGameSettings* Settings = UFPSGameSettings::LoadOrCreateSettings())
    {
        Settings->Language = FPSLocalizationHelper::GetLanguageCode(NewLanguage);
        Settings->SaveSettings();
    }
}

void UFPSLocalizationManager::SetLanguageFromCode(const FString& LanguageCode)
{
    EGameLanguage Language = FPSLocalizationHelper::GetLanguageFromCode(LanguageCode);
    SetLanguage(Language);
}

void UFPSLocalizationManager::ApplyLanguageChange(EGameLanguage NewLanguage)
{
    EGameLanguage OldLanguage = CurrentLanguage;
    CurrentLanguage = NewLanguage;
    
    // 获取语言代码
    FString CultureCode = GetCurrentLanguageCode();
    
    // 设置UE5的当前Culture
    FInternationalization::Get().SetCurrentCulture(CultureCode);
    
    // 广播事件
    OnLanguageChanged.Broadcast(NewLanguage);
    OnLocalizationChanged.Broadcast();
    
    UE_LOG(LogTemp, Log, TEXT("Language changed from %s to %s"),
        *FPSLocalizationHelper::GetLanguageCode(OldLanguage),
        *CultureCode);
}

void UFPSLocalizationManager::LoadLocalizationAssets(EGameLanguage Language)
{
    // 异步加载字体资源
    if (TSoftObjectPtr<UFont>* FontPtr = LanguageFonts.Find(Language))
    {
        FStreamableDelegate OnLoaded;
        OnLoaded.BindLambda([this, Language]()
        {
            UE_LOG(LogTemp, Log, TEXT("Font for language %s loaded"), 
                *FPSLocalizationHelper::GetLanguageCode(Language));
        });
        
        FStreamableHandle* Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
            FontPtr->ToSoftObjectPath(),
            OnLoaded
        );
        
        if (Handle)
        {
            AsyncLoadHandles.Add(Handle);
        }
    }
    
    // TODO: 加载其他本地化资源 (音频、贴图等)
}

FText UFPSLocalizationManager::GetLocalizedText(const FString& Namespace, const FString& Key) const
{
    // 优先从String Table获取
    FName TableId(*Namespace);
    FName StringKey(*Key);
    
    if (UStringTable::Exists(TableId))
    {
        return FText::FromStringTable(TableId, StringKey.ToString());
    }
    
    // Fallback: 返回Key作为文本
    return FText::FromString(Key);
}

UFont* UFPSLocalizationManager::GetCurrentLanguageFont() const
{
    return GetLanguageFont(CurrentLanguage);
}

UFont* UFPSLocalizationManager::GetLanguageFont(EGameLanguage Language) const
{
    if (const TSoftObjectPtr<UFont>* FontPtr = LanguageFonts.Find(Language))
    {
        return FontPtr->LoadSynchronous();
    }
    
    // Fallback to default font
    return DefaultFont.LoadSynchronous();
}

TArray<EGameLanguage> UFPSLocalizationManager::GetSupportedLanguages() const
{
    TArray<EGameLanguage> Languages;
    const UEnum* EnumPtr = StaticEnum<EGameLanguage>();
    
    if (EnumPtr)
    {
        for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i)  // -1 to skip Default/MAX
        {
            int64 Value = EnumPtr->GetValueByIndex(i);
            if (Value != static_cast<int64>(EGameLanguage::Default))
            {
                Languages.Add(static_cast<EGameLanguage>(Value));
            }
        }
    }
    
    return Languages;
}

void UFPSLocalizationManager::LoadLanguageFromSettings()
{
    if (UFPSGameSettings* Settings = UFPSGameSettings::LoadOrCreateSettings())
    {
        FString SavedLanguage = Settings->Language;
        if (!SavedLanguage.IsEmpty())
        {
            SetLanguageFromCode(SavedLanguage);
        }
    }
}

FText UFPSLocalizationManager::GetLanguageDisplayName(EGameLanguage Language)
{
    return FPSLocalizationHelper::GetLanguageDisplayName(Language);
}
```

### 3. 游戏设置集成

```cpp
// FPSGameSettings.h - 添加语言设置
UCLASS()
class FPSGAME_API UFPSGameSettings : public USaveGame
{
    // ... 现有代码 ...
    
public:
    // ========== 游戏设置 ==========
    
    // 语言代码 (zh-Hans, en, ja 等)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    FString Language;
    
    // 语言枚举 (便于蓝图使用)
    UPROPERTY(Transient, BlueprintReadOnly, Category = "Gameplay")
    EGameLanguage LanguageEnum;
    
    // 应用语言设置
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplyLanguageSettings();
    
    // ... 现有代码 ...
};

// FPSGameSettings.cpp
void UFPSGameSettings::ApplyDefaultSettings()
{
    // ... 其他默认值 ...
    
    // 语言默认跟随系统，或使用英语
    FString SystemCulture = FInternationalization::Get().GetCurrentCulture()->GetName();
    Language = SystemCulture;
    LanguageEnum = FPSLocalizationHelper::GetLanguageFromCode(SystemCulture);
    
    // 如果不支持系统语言，回退到英语
    if (LanguageEnum == EGameLanguage::Default)
    {
        Language = TEXT("en");
        LanguageEnum = EGameLanguage::English;
    }
}

void UFPSGameSettings::ApplyLanguageSettings()
{
    if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
    {
        if (UFPSLocalizationManager* LocManager = GameInstance->GetSubsystem<UFPSLocalizationManager>())
        {
            LocManager->SetLanguageFromCode(Language);
        }
    }
}
```

---

## 字体和UI适配

### 1. 多语言字体配置

```cpp
// FPSFontManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FPSFontManager.generated.h"

UCLASS()
class FPSGAME_API UFPSFontManager : public UWorldSubsystem
{
    GENERATED_BODY()
    
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    
    /** 注册需要动态更新字体的文本块 */
    UFUNCTION(BlueprintCallable, Category = "Font")
    void RegisterTextBlock(class UCommonTextBlock* TextBlock);
    
    /** 注销文本块 */
    UFUNCTION(BlueprintCallable, Category = "Font")
    void UnregisterTextBlock(class UCommonTextBlock* TextBlock);
    
    /** 更新所有已注册文本块的字体 */
    UFUNCTION(BlueprintCallable, Category = "Font")
    void UpdateAllTextFonts();
    
private:
    UPROPERTY()
    TArray<TWeakObjectPtr<UCommonTextBlock>> RegisteredTextBlocks;
    
    UFUNCTION()
    void OnLanguageChanged(EGameLanguage NewLanguage);
};
```

### 2. 动态字体更新组件

```cpp
// FPSLocalizedTextBlock.h
#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "FPSLocalizedTextBlock.generated.h"

/**
 * 自动响应语言切换的本地化文本块
 */
UCLASS(meta = (DisableNativeTick))
class FPSGAME_API UFPSLocalizedTextBlock : public UCommonTextBlock
{
    GENERATED_BODY()
    
public:
    UFPSLocalizedTextBlock(const FObjectInitializer& ObjectInitializer);
    
    virtual void NativeOnInitialized() override;
    virtual void NativeDestruct() override;
    
    /** 设置本地化键 */
    UFUNCTION(BlueprintCallable, Category = "Localized Text")
    void SetLocalizationKey(const FString& Namespace, const FName& Key);
    
    /** 刷新文本 (语言切换时调用) */
    UFUNCTION(BlueprintCallable, Category = "Localized Text")
    void RefreshText();
    
    /** 是否自动更新字体 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localized Text")
    bool bAutoUpdateFont = true;
    
protected:
    UPROPERTY(EditAnywhere, Category = "Localized Text")
    FString LocalizationNamespace;
    
    UPROPERTY(EditAnywhere, Category = "Localized Text")
    FName LocalizationKey;
    
    UPROPERTY(EditAnywhere, Category = "Localized Text", meta = (EditCondition = "bAutoUpdateFont"))
    bool bUseBoldFont = false;
    
    UFUNCTION()
    void HandleLanguageChanged(EGameLanguage NewLanguage);
    
    void UpdateFont();
};
```

### 3. UI布局适配

```cpp
// FPSLocalizedWidget.h
#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "FPSLocalizedWidget.generated.h"

/**
 * 支持RTL(从右到左)语言的Widget基类
 */
UCLASS(Abstract)
class FPSGAME_API UFPSLocalizedWidget : public UCommonActivatableWidget
{
    GENERATED_BODY()
    
public:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    
protected:
    /** 是否是RTL语言 */
    UFUNCTION(BlueprintPure, Category = "Localization")
    bool IsRTLLanguage() const;
    
    /** 应用RTL布局调整 */
    UFUNCTION(BlueprintCallable, Category = "Localization")
    void ApplyRTLLayout();
    
    /** 刷新所有本地化文本 */
    UFUNCTION(BlueprintCallable, Category = "Localization")
    void RefreshAllLocalizedText();
    
    /** 收集所有子LocalizedTextBlock */
    UFUNCTION(BlueprintCallable, Category = "Localization")
    TArray<class UFPSLocalizedTextBlock*> GetAllLocalizedTextBlocks() const;
    
    UFUNCTION()
    void OnLanguageChangedHandler(EGameLanguage NewLanguage);
    
    // RTL语言列表
    UPROPERTY(EditDefaultsOnly, Category = "Localization")
    TArray<EGameLanguage> RTLLanguages;
};
```

---

## 音频本地化

### 1. 语音管理

```cpp
// FPSAudioLocalization.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FPSAudioLocalization.generated.h"

USTRUCT(BlueprintType)
struct FLocalizedVoiceLine
{
    GENERATED_BODY()
    
    // 语音ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName VoiceID;
    
    // 各语言对应的语音资产
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<EGameLanguage, TSoftObjectPtr<USoundBase>> LocalizedVoices;
    
    // 字幕文本 (使用本地化Key)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SubtitleNamespace;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SubtitleKey;
};

UCLASS()
class FPSGAME_API UFPSAudioLocalization : public UObject
{
    GENERATED_BODY()
    
public:
    /** 获取当前语言的语音 */
    UFUNCTION(BlueprintPure, Category = "Audio Localization")
    USoundBase* GetLocalizedVoice(const FName& VoiceID) const;
    
    /** 播放本地化语音 */
    UFUNCTION(BlueprintCallable, Category = "Audio Localization")
    void PlayLocalizedVoice(const FName& VoiceID, FVector Location = FVector::ZeroVector);
    
    /** 注册语音资产 */
    UFUNCTION(BlueprintCallable, Category = "Audio Localization")
    void RegisterVoiceLine(const FLocalizedVoiceLine& VoiceLine);
    
private:
    UPROPERTY()
    TMap<FName, FLocalizedVoiceLine> VoiceLineRegistry;
    
    UPROPERTY()
    class UFPSLocalizationManager* LocManager;
};
```

---

## 蓝图最佳实践

### 1. 使用String Tables

```
Content/Localization/StringTables/
├── ST_Menu.uasset          (菜单文本)
├── ST_Settings.uasset      (设置文本)
├── ST_Popup.uasset         (弹窗文本)
├── ST_Common.uasset        (通用文本)
└── ST_HUD.uasset           (HUD文本)
```

### 2. 蓝图节点使用示例

```
[事件图表: WBP_MainMenu]

事件 BeginPlay
    │
    └──► 获取 Localization Manager
         │
         └──► 绑定事件 OnLanguageChanged
              │
              └──► [自定义事件] RefreshAllText

[自定义事件] RefreshAllText
    │
    ├──► ContinueButton.SetText (ST_Menu, "ContinueGame")
    ├──► NewGameButton.SetText (ST_Menu, "NewGame")
    ├──► SettingsButton.SetText (ST_Menu, "Settings")
    └──► QuitButton.SetText (ST_Menu, "Quit")

[设置语言下拉框变更事件]
    │
    └──► LocalizationManager.SetLanguage (SelectedLanguage)
```

### 3. 语言设置页面蓝图

```
WBP_LanguageSettings (继承 UFPSMenuBase)
│
├── ContentContainer (VerticalBox)
│   ├── LanguageLabel (LocalizedTextBlock)
│   │   └── 默认文本: "Language" (ST_Settings, "Language")
│   │
│   ├── LanguageComboBox (FPSComboBox)
│   │   ├── 选项1: "简体中文" (Value: zh-Hans)
│   │   ├── 选项2: "English" (Value: en)
│   │   ├── 选项3: "日本語" (Value: ja)
│   │   └── OnSelectionChanged
│   │       │
│   │       └──► 获取LocalizationManager
│   │            │
│   │            └──► SetLanguageFromCode (SelectedValue)
│   │
│   └── ApplyButton (FPSCommonButton)
│       └── OnClicked
│           │
│           └──► SaveAllSettings
```

---

## C++实现

### 1. 游戏设置页面扩展

```cpp
// FPSGameplaySettingsWidget.h - 添加语言设置
UCLASS()
class FPSGAME_API UFPSGameplaySettingsWidget : public UFPSMenuBase
{
    GENERATED_BODY()
    
protected:
    virtual void NativeOnInitialized() override;
    
    // 语言设置
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* LanguageCombo;
    
    // 难度设置 (原有)
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* DifficultyCombo;
    
    // 初始化语言选项
    void InitializeLanguageOptions();
    
    // 语言变更处理
    UFUNCTION()
    void OnLanguageChanged(int32 SelectedIndex);
    
    // 语言切换回调
    UFUNCTION()
    void HandleLanguageChanged(EGameLanguage NewLanguage);
};

// FPSGameplaySettingsWidget.cpp
void UFPSGameplaySettingsWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    InitializeLanguageOptions();
    
    // 绑定语言变更事件
    if (UFPSLocalizationManager* LocManager = GetLocalizationManager())
    {
        LocManager->OnLanguageChanged.AddDynamic(this, &UFPSGameplaySettingsWidget::HandleLanguageChanged);
    }
}

void UFPSGameplaySettingsWidget::InitializeLanguageOptions()
{
    if (!LanguageCombo) return;
    
    TArray<FText> Options;
    TArray<EGameLanguage> Languages;
    
    if (UFPSLocalizationManager* LocManager = GetLocalizationManager())
    {
        Languages = LocManager->GetSupportedLanguages();
    }
    
    for (EGameLanguage Lang : Languages)
    {
        Options.Add(UFPSLocalizationManager::GetLanguageDisplayName(Lang));
    }
    
    // 设置当前选中项
    int32 CurrentIndex = 0;
    if (UFPSLocalizationManager* LocManager = GetLocalizationManager())
    {
        EGameLanguage CurrentLang = LocManager->GetCurrentLanguage();
        CurrentIndex = Languages.IndexOfByKey(CurrentLang);
        if (CurrentIndex < 0) CurrentIndex = 0;
    }
    
    LanguageCombo->SetupOptions(Options, CurrentIndex);
    LanguageCombo->OnSelectionChanged.AddDynamic(this, &UFPSGameplaySettingsWidget::OnLanguageChanged);
}

void UFPSGameplaySettingsWidget::OnLanguageChanged(int32 SelectedIndex)
{
    TArray<EGameLanguage> Languages;
    if (UFPSLocalizationManager* LocManager = GetLocalizationManager())
    {
        Languages = LocManager->GetSupportedLanguages();
        if (Languages.IsValidIndex(SelectedIndex))
        {
            LocManager->SetLanguage(Languages[SelectedIndex]);
        }
    }
}

void UFPSGameplaySettingsWidget::HandleLanguageChanged(EGameLanguage NewLanguage)
{
    // 更新UI显示
    // 可以在这里显示"语言已切换"的提示
}
```

### 2. 菜单管理器扩展

```cpp
// FPSMenuManager.h - 添加本地化支持
UCLASS()
class FPSGAME_API UFPSMenuManager : public UGameInstanceSubsystem
{
    // ... 现有代码 ...
    
public:
    /** 获取本地化管理器 */
    UFUNCTION(BlueprintPure, Category = "Menu|Localization")
    class UFPSLocalizationManager* GetLocalizationManager() const;
    
    // ... 现有代码 ...
};
```

---

## 工作流和工具

### 1. 本地化工作流

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         本地化工作流程                                       │
└─────────────────────────────────────────────────────────────────────────────┘

阶段1: 文本收集 (开发者)
    │
    ├──► 在代码/蓝图中使用FText/LocalizedText
    │
    └──► 运行 Localization Dashboard → Gather Text
         │
         └──► 生成 .manifest 和 .archive 文件

阶段2: 翻译 (翻译人员)
    │
    ├──► 导出 .po 文件 (Portable Object)
    │
    ├──► 使用 PoEdit / Crowdin / Excel 翻译
    │
    └──► 导入翻译后的 .po 文件

阶段3: 编译 (构建时)
    │
    ├──► Localization Dashboard → Compile Text
    │
    └──► 生成 .locres 文件 (运行时资源)

阶段4: 运行时
    │
    └──► 游戏加载对应语言的 .locres
         │
         └──► 根据Culture Code显示对应文本
```

### 2. 配置文件

```ini
# Config/Localization/Game.ini
[/Script/Localization.LocalizationSettings]
+NativeCulturePriorities=(CultureName="en")
+NativeCulturePriorities=(CultureName="zh-Hans")

[/Script/Localization.LocalizationTarget]
+Settings=Game

[Game]
Name=Game
+TargetCultures=en
+TargetCultures=zh-Hans
+TargetCultures=zh-Hant
+TargetCultures=ja
+TargetCultures=ko
```

### 3. 本地化仪表板配置

```
编辑器 → 窗口 → 本地化仪表板 (Localization Dashboard)

目标 (Targets):
├── Game
│   ├── 收集文本来源:
│   │   ├── Source/FPSGame/**/*.cpp
│   │   ├── Source/FPSGame/**/*.h
│   │   └── Content/UI/**/*.uasset
│   │
│   └── 目标语言:
│       ├── en (英语 - 源语言)
│       ├── zh-Hans (简体中文)
│       ├── zh-Hant (繁体中文)
│       ├── ja (日语)
│       └── ko (韩语)
│
└── Metadata
    └── 元数据本地化
```

---

## 目录结构

```
Content/
├── Localization/
│   ├── Game/
│   │   ├── en/
│   │   │   └── Game.archive
│   │   ├── zh-Hans/
│   │   │   └── Game.archive
│   │   └── ... (其他语言)
│   │
│   └── StringTables/
│       ├── ST_Menu.uasset
│       ├── ST_Settings.uasset
│       ├── ST_Popup.uasset
│       ├── ST_Common.uasset
│       └── ST_HUD.uasset
│
├── UI/
│   └── Menu/
│       └── Settings/
│           └── WBP_GameplaySettings.uasset (包含语言设置)
│
└── Fonts/
    ├── NotoSansSC/
    │   └── NotoSansSC-Regular.uasset (简体中文)
    ├── NotoSansTC/
    │   └── NotoSansTC-Regular.uasset (繁体中文)
    ├── NotoSansJP/
    │   └── NotoSansJP-Regular.uasset (日语)
    └── Roboto/
        └── Roboto-Regular.uasset (拉丁语系)

Source/FPSGame/
├── Public/
│   └── Localization/
│       ├── FPSLocalizationManager.h
│       ├── FPSLocalizationTypes.h
│       ├── FPSFontManager.h
│       ├── FPSLocalizedTextBlock.h
│       └── FPSAudioLocalization.h
│
└── Private/
    └── Localization/
        ├── FPSLocalizationManager.cpp
        ├── FPSFontManager.cpp
        ├── FPSLocalizedTextBlock.cpp
        └── FPSAudioLocalization.cpp

Config/
└── Localization/
    └── Game.ini
```

---

## 与现有Menu系统集成

### 需要修改的文件

1. **FPSGameSettings.h/cpp**
   - 添加 `Language` 和 `LanguageEnum` 属性
   - 添加 `ApplyLanguageSettings()` 方法

2. **FPSGameplaySettingsWidget.h/cpp**
   - 添加语言设置行 (`LanguageCombo`)
   - 实现语言切换UI

3. **新增文件**
   - `FPSLocalizationManager` - 本地化核心管理
   - `FPSLocalizationTypes` - 语言枚举和辅助函数
   - `FPSLocalizedTextBlock` - 自动本地化文本组件
   - `FPSFontManager` - 动态字体管理

### 修改后的菜单层级

```
UFPSMenuManager (管理器，放在GameInstance)
├── UFPSMainMenu (主菜单)
├── UFPSPauseMenu (暂停菜单)
├── UFPSSettingsMenu (设置菜单)
│   ├── UFPSGraphicsSettingsWidget
│   ├── UFPSAudioSettingsWidget
│   ├── UFPSControlSettingsWidget
│   └── UFPSGameplaySettingsWidget
│       ├── 难度设置
│       ├── HUD选项
│       └── 语言设置 (新增) ⭐
│
└── UFPSLocalizationManager (新增) ⭐
    ├── 语言切换
    ├── 字体管理
    └── 文本刷新
```

---

## 总结

本本地化设计方案为UE5 FPS Menu系统提供了:

1. **完整的本地化架构** - 与现有Menu系统无缝集成
2. **运行时语言切换** - 无需重启游戏，即时生效
3. **多语言字体支持** - 动态加载对应语言的字体资源
4. **文本管理最佳实践** - String Tables + Namespace组织
5. **音频本地化** - 语音和字幕分离管理
6. **RTL语言支持** - 阿拉伯语、希伯来语等从右到左布局
7. **自动化工作流** - 支持Gather/Compile/Export/Import完整流程

---

## 参考文档

- [UE5 Localization Documentation](https://docs.unrealengine.com/5.0/en-US/localization-in-unreal-engine/)
- [Text Localization](https://docs.unrealengine.com/5.0/en-US/text-localization-in-unreal-engine/)
- [String Tables](https://docs.unrealengine.com/5.0/en-US/string-tables-in-unreal-engine/)
