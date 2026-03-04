# C++ 到 UMG 绑定流程图

## 完整绑定流程概览

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        C++ 基类定义                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  UCLASS()                                                                   │
│  class UFPSMenuBase : public UCommonActivatableWidget                       │
│  {                                                                          │
│      UPROPERTY(meta = (BindWidget))                                         │
│      UCommonTextBlock* MenuTitle;  ◄──────┐   【1. 用BindWidget标记】        │
│                                             │                               │
│      UPROPERTY(meta = (BindWidgetOptional)) │                               │
│      UImage* BackgroundImage;  ◄────────────┘   【可选绑定】                 │
│                                                                             │
│      UFUNCTION(BlueprintCallable)           │   【2. 暴露函数给蓝图】        │
│      void OnBackPressed();  ◄───────────────┘                               │
│                                                                             │
│      UFUNCTION(BlueprintImplementableEvent)                                 │
│      void PlayOpenAnimation();  ◄─────────────── 【3. 蓝图实现事件】         │
│  }                                                                          │
│                                                                             │
└───────────────────────────────┬─────────────────────────────────────────────┘
                                │
                                │ 编译生成
                                ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        创建蓝图 Widget                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  1. 右键 → User Interface → Widget Blueprint                               │
│                                                                             │
│  2. 打开蓝图 → Class Settings → Parent Class                                │
│                       │                                                     │
│                       ▼                                                     │
│           选择 FPSMenuBase (C++类)                                          │
│                                                                             │
└───────────────────────────────┬─────────────────────────────────────────────┘
                                │
                                │ 设计界面
                                ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        Designer 面板布局                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  CanvasPanel (Root)                                                         │
│  └── BackgroundImage (Image)  ◄──┐                                          │
│      └── 【4. Variable Name = BackgroundImage】                               │
│      └── 【5. 勾选 Is Variable】  │                                          │
│                                   │   【名称必须与C++完全一致】              │
│  └── MenuTitle (CommonTextBlock) ◄┘                                         │
│      └── Variable Name = MenuTitle                                          │
│      └── 勾选 Is Variable                                                   │
│                                                                             │
│  ⚠️ 如果名称不匹配，C++中此指针将为 nullptr                                  │
│                                                                             │
└───────────────────────────────┬─────────────────────────────────────────────┘
                                │
                                │ 绑定事件
                                ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        Graph 面板逻辑                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  【6. 调用C++函数】                                                          │
│                                                                             │
│  右键 → Call Function → 选择UFPSMenuBase的函数                              │
│                                                                             │
│  BackButton (OnClicked) ───────┐                                            │
│                                ▼                                            │
│                        OnBackPressed (执行C++代码)                          │
│                                                                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  【7. 实现蓝图事件】                                                         │
│                                                                             │
│  MyBlueprint 面板 → Override → PlayOpenAnimation                             │
│                                │                                            │
│                                ▼                                            │
│  添加动画轨道，设计开启动画                                                    │
│  (C++中调用PlayOpenAnimation()时会触发此蓝图事件)                            │
│                                                                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  【8. 绑定委托】                                                             │
│                                                                             │
│  Event Construct                                                            │
│       │                                                                     │
│       ├──► SettingRow (引用)                                                │
│       │    └── Bind Event to OnValueChanged  ◄──┐                           │
│       │         ├── Target: SettingRow          │   【创建绑定】             │
│       │         └── Event: Custom Event ◄───────┘                           │
│       │                                              【实现响应逻辑】         │
│       └──► Custom Event "OnSettingChanged"                                  │
│            └── [在这里写值变化后的处理]                                       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
                                │
                                │ 运行游戏
                                ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        运行时行为                                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  1. NativeOnInitialized() 被调用                                            │
│       │                                                                     │
│       └──► MenuTitle 已自动绑定 (如果Blueprint中命名正确)                     │
│                                                                             │
│  2. NativeConstruct() 被调用                                                │
│       │                                                                     │
│       └──► Blueprint的Event Construct执行                                    │
│                                                                             │
│  3. 用户点击 BackButton                                                     │
│       │                                                                     │
│       └──► Blueprint的OnClicked事件触发                                      │
│            └──► 调用C++的OnBackPressed()                                     │
│                 └──► 执行C++代码逻辑                                         │
│                      └──► 例如：GetMenuManager()->PopMenu()                  │
│                                                                             │
│  4. 菜单打开                                                                 │
│       │                                                                     │
│       └──► C++调用PlayOpenAnimation()                                        │
│            └──► 触发Blueprint的动画事件                                       │
│                 └──► 播放开启动画                                             │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 关键绑定检查清单

### C++ 端

- [ ] 使用 `UPROPERTY(meta = (BindWidget))` 标记需要绑定的Widget
- [ ] 使用 `UPROPERTY(meta = (BindWidgetOptional))` 标记可选绑定的Widget
- [ ] 使用 `UFUNCTION(BlueprintCallable)` 暴露给蓝图调用的函数
- [ ] 使用 `UFUNCTION(BlueprintImplementableEvent)` 声明蓝图实现的事件
- [ ] 使用 `UFUNCTION(BlueprintNativeEvent)` 声明蓝图可覆盖的事件
- [ ] 使用 `DECLARE_DYNAMIC_MULTICAST_DELEGATE` 声明蓝图可绑定的委托

### UMG 端

- [ ] Variable Name **必须与C++变量名完全一致**（区分大小写）
- [ ] 勾选 `Is Variable` 选项
- [ ] Parent Class 必须正确选择（C++类或继承自C++的Blueprint）
- [ ] 绑定事件时选择正确的Target
- [ ] 调用C++函数前确保对象有效（IsValid检查）

---

## 常见错误排查

### 1. "Accessed None" 错误

```
Error: Accessed None trying to read property MenuTitle
```

**原因**：BindWidget名称不匹配
**解决**：检查C++和Blueprint中的Variable Name是否完全一致

### 2. "Blueprint Runtime Error"

```
Blueprint Runtime Error: "Attempted to access missing member"
```

**原因**：尝试调用不存在的C++函数
**解决**：确保C++函数标记了 `BlueprintCallable` 并重新编译

### 3. Widget未显示

**原因**：可能忘记添加到视口
**解决**：
```cpp
// C++
UUserWidget* Widget = CreateWidget<UUserWidget>(PlayerController, WidgetClass);
Widget->AddToViewport();
```

### 4. 委托不触发

**原因**：委托绑定失败或对象被销毁
**解决**：检查绑定逻辑，确保在Construct中绑定

---

## 快速参考：常用宏

| 宏 | 用途 | 蓝图可见性 |
|----|------|-----------|
| `UPROPERTY(meta = (BindWidget))` | 必须绑定的Widget | 只读 |
| `UPROPERTY(meta = (BindWidgetOptional))` | 可选绑定的Widget | 只读 |
| `UFUNCTION(BlueprintCallable)` | 蓝图可调用函数 | 可调用 |
| `UFUNCTION(BlueprintPure)` | 纯函数（无副作用） | 可调用 |
| `UFUNCTION(BlueprintImplementableEvent)` | 蓝图实现事件 | 可实现 |
| `UFUNCTION(BlueprintNativeEvent)` | C++实现+蓝图可覆盖 | 可覆盖 |
| `UFUNCTION(BlueprintAssignable)` | 蓝图可绑定委托 | 可绑定 |

---

## 完整代码示例

### C++ 基类

```cpp
// FPSMyWidget.h
UCLASS()
class FPSGAME_API UFPSMyWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    // BindWidget - 必须绑定
    UPROPERTY(meta = (BindWidget))
    UButton* ConfirmButton;
    
    // BindWidgetOptional - 可选绑定
    UPROPERTY(meta = (BindWidgetOptional))
    UImage* IconImage;
    
    // 蓝图可调用
    UFUNCTION(BlueprintCallable, Category = "MyWidget")
    void Confirm();
    
    // 蓝图可实现
    UFUNCTION(BlueprintImplementableEvent, Category = "MyWidget")
    void OnConfirmed();
    
    // 委托
    UPROPERTY(BlueprintAssignable, Category = "MyWidget")
    FOnButtonClicked OnConfirmedDelegate;
};
```

### 使用蓝图

```cpp
// FPSMyWidget.cpp
void UFPSMyWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    // 检查绑定
    if (!ConfirmButton)
    {
        UE_LOG(LogTemp, Error, TEXT("ConfirmButton not bound!"));
        return;
    }
    
    // C++端绑定点击事件
    ConfirmButton->OnClicked.AddDynamic(this, &UFPSMyWidget::Confirm);
}

void UFPSMyWidget::Confirm()
{
    // C++逻辑
    UE_LOG(LogTemp, Log, TEXT("Confirmed!"));
    
    // 调用蓝图实现的事件
    OnConfirmed();
    
    // 广播委托
    OnConfirmedDelegate.Broadcast();
}
```

### UMG Blueprint

**Designer**: 
- 创建Button，Variable Name = "ConfirmButton"
- 创建Image，Variable Name = "IconImage"（可选）

**Graph**:
```
[Event Construct]
    └──► Bind Event to OnConfirmedDelegate
        └── Target: self
        └── Event: Custom Event "HandleConfirmed"

[Custom Event: HandleConfirmed]
    └──► Print String "确认成功！"

// 也可以Override C++事件
[Override: OnConfirmed]
    └──► Play Animation "ConfirmAnim"
```
