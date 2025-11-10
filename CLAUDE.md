# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**UE5FPSDesign** is a comprehensive design documentation repository for an Unreal Engine 5 multiplayer PvE FPS game project. The game follows a roguelike FPS structure similar to "Potato Brothers" (土豆兄弟) with procedural dungeon generation, combat, upgrades, and item systems.

**IMPORTANT**: This is currently a documentation-only repository. No actual UE5 project files exist yet - this needs to be created first before development can begin.

## Language and Communication

- **All documentation and code comments must be written in Chinese** (中文)
- Follow the established pattern from existing documentation
- Technical terms may use English when appropriate (e.g., "GAS", "UE5", "Component")

## Core Development Principles

1. **GAS Framework Integration** (GAS框架集成) - All gameplay mechanics should use Unreal's Gameplay Ability System
2. **C++ Implementation** (C++实现) - Prefer C++ for core systems, use Blueprints for design-heavy features
3. **Data-Driven Architecture** (数据驱动架构) - Use UE5 Data Assets for configuration and avoid hard-coding values
4. **Multiplayer-First Design** (多人游戏优先) - Always consider networking and replication from the start
5. **SKG Framework Reference** (SKG框架参考) - Use SKG Shooter Framework as inspiration but adapt for GAS integration (don't use the plugin directly)

## Architecture Overview

### Core Class Hierarchy (基类层次结构)
```
GameInstance
├── GameMode
├── GameState
├── PlayerController
├── PlayerState
└── Character
    └── PlayerCharacter
        ├── InventoryComponent (库存组件)
        ├── SkillComponent (技能组件)
        ├── FirearmComponent (射击组件)
        ├── InteractionComponent (交互组件)
        └── AbilitySystemComponent (GAS组件)
```

### Key Systems (核心系统)

1. **Enhanced Input System** (增强输入系统) - `/Input/`
2. **Inventory System** (库存系统) - `/库存/`
   - Item Definition (物品定义)
   - Item Instance (物品实例)
3. **Skill System** (技能系统) - `/技能/`
   - Skill Definition (技能定义)
   - Skill Instance (技能实例)
4. **Shooting Framework** (射击框架) - `/射击框架/`
   - Based on SKG framework patterns
   - Integrated with GAS for abilities
5. **Procedural Dungeon Generation** (随机地牢生成) - `/随机地图/`
   - Uses Procedural Dungeon Plugin as reference
6. **GAS Integration** (GAS集成) - `/GAS集成/`
   - Damage system (伤害系统)
   - Elemental reactions (元素反应)
   - Character attributes (角色属性)

## Development Workflow

### Project Setup (First Time)
Since no UE5 project exists yet, the first task should be:

1. Create new UE5 project with C++ support
2. Set up basic folder structure following UE5 conventions:
   ```
   Source/[ProjectName]/
   ├── Public/
   ├── Private/
   └── [ProjectName].Build.cs
   Content/
   ├── Blueprints/
   ├── Data/
   ├── Materials/
   └── UI/
   ```
3. Configure project for multiplayer development
4. Set up GAS plugin integration

### Common Development Commands

**Note**: These commands will be available once the UE5 project is created:

```bash
# Generate project files (after creating .uproject)
# On Windows:
"%UE_ROOT%\Engine\Binaries\DotNET\UnrealBuildTool.exe" -projectfiles -project="[ProjectName].uproject" -game -rocket -progress

# On Mac:
/Users/Shared/Epic\ Games/UE_5.x/Engine/Binaries/DotNET/UnrealBuildTool /path/to/project.uproject -projectfiles -game -rocket -progress

# Build the project (Development Editor)
# On Windows:
"%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" [ProjectName]Editor Win64 Development "[ProjectPath]\[ProjectName].uproject" -waitmutex

# On Mac:
/Users/Shared/Epic\ Games/UE_5.x/Engine/Build/BatchFiles/Mac/Build.sh [ProjectName]Editor Mac Development "[ProjectPath]/[ProjectName].uproject" -waitmutex
```

### Code Organization

- **Base Classes** (基类): Place in `Source/[ProjectName]/Core/`
- **Components** (组件): Place in `Source/[ProjectName]/Components/`
- **GAS Classes** (GAS类): Place in `Source/[ProjectName]/AbilitySystem/`
- **Data Assets** (数据资产): Place in `Content/Data/`
- **UI Classes** (UI类): Place in `Source/[ProjectName]/UI/`

### Reference Frameworks

1. **SKG Shooter Framework** - Located in `/reference/skg/`
   - Study the modular weapon system design
   - Adapt the procedural animation concepts for GAS integration
   - Reference the component-based firearm architecture

2. **Procedural Dungeon Plugin** - Located in `/reference/pcgDungeon/`
   - Understand room-based generation system
   - Implement similar patterns for dungeon layout

## Key Implementation Notes

### GAS Integration Requirements
- All gameplay mechanics (shooting, skills, damage) must use GameplayAbilities
- Character attributes should be implemented as AttributeSets
- Use GameplayEffects for damage, buffs, and debuffs
- Implement GameplayTags for categorizing abilities and effects

### Multiplayer Considerations
- Mark all gameplay-critical variables with UPROPERTY(Replicated)
- Use Server RPCs for authoritative actions
- Implement client prediction for responsive gameplay
- Consider bandwidth optimization for frequent updates

### Data Asset Patterns
- Create base DataAsset classes for Items, Skills, Weapons
- Use inheritance hierarchy for specialized item types
- Store all balance values in Data Assets, not code
- Implement validation functions for data integrity

## Documentation Structure

Each major system has dedicated Chinese documentation in its respective folder:
- `/基类/` - Base class specifications and inheritance patterns
- `/射击框架/` - Weapon system architecture and components
- `/GAS集成/` - Ability system integration patterns
- `/库存/` - Inventory system design and item definitions
- `/技能/` - Skill system architecture and progression
- `/随机地图/` - Procedural generation algorithms and room design

## Current Development Status

- ✅ Comprehensive design documentation completed
- ✅ Reference framework analysis completed
- ❌ UE5 project setup (not started)
- ❌ Base class implementation (not started)
- ❌ GAS integration setup (not started)

## Next Development Steps

1. Create UE5 project with appropriate settings
2. Implement base Character and Controller classes
3. Set up GAS foundation (AbilitySystemComponent, base AttributeSet)
4. Create modular component system based on documentation
5. Implement data asset framework for configuration