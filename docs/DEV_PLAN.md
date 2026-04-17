# MTBRacing 开发计划

> 最后更新：2026-04-17

## 产品概述

一款基于UE5的第三人称多人山地车竞速Demo，参考《极限国度》的山地车玩法。玩家操控骑手驾驶山地车在山地赛道上进行竞速，支持2-4人局域网联机对战。

## 核心功能

1. **山地车载具系统**
   - 基于Chaos Vehicle物理系统的山地车模拟
   - 悬挂系统、轮胎摩擦、地形适应
   - 空中姿态控制、落地缓冲

2. **骑手动画系统**
   - 骑手与车辆的IK绑定（手握把手、脚踩踏板）
   - 身体随车辆倾斜的程序化动画
   - 特技动作蒙太奇（空翻、甩尾）

3. **GAS能力系统**
   - Boost加速能力（消耗耐力条）
   - 耐力属性系统（自动恢复）
   - 特技分数系统（空中动作得分）
   - 速度/漂移Buff效果

4. **多人竞速框架**
   - 2-4人局域网多人对战
   - 载具位置/速度网络同步
   - 客户端预测与服务器校正
   - 比赛流程（倒计时、排名、终点判定）

5. **赛道与场景**
   - 山地赛道关卡（起点、检查点、终点）
   - 基础地形（坡道、弯道、跳台）
   - 简易竞速UI（速度表、排名、计时器）

---

## 技术栈选型

| 类别 | 技术选择 | 说明 |
| --- | --- | --- |
| 引擎 | UE5.3+ | Chaos Vehicle + 异步物理支持 |
| 载具物理 | Chaos Vehicle System | 轻量级载具物理模拟，支持任意车轮数 |
| 能力系统 | Gameplay Ability System (GAS) | 技能、Buff、属性管理 |
| 动画 | Animation Blueprint + Control Rig | 骑手IK、程序化倾斜动画 |
| 网络 | UE5 Replication + 网络物理 | 属性复制、RPC、物理同步 |
| 开发语言 | C++ + Blueprint混合 | C++实现核心逻辑，蓝图配置数据 |
| 版本控制 | Git + GitHub | 支持双机切换开发，UE专用.gitignore |

---

## 开发阶段

### Phase 0: 环境准备 ✅
- [x] Git 仓库配置
- [x] UE5 专用 .gitignore
- [x] Git LFS 配置
- [x] GitHub 远程仓库

### Phase 1: 载具基础（进行中）
- [ ] 创建 UE5 C++ 项目
- [ ] 启用 ChaosVehicles 和 GameplayAbilities 插件
- [ ] 实现 MTBVehiclePawn 基础类
- [ ] 配置 Chaos Vehicle 物理参数
- [ ] Enhanced Input 输入系统

### Phase 2: GAS 能力系统
- [ ] MTBAttributeSet 属性集
- [ ] GA_Boost 加速能力
- [ ] 空中检测和特技能力

### Phase 3: 动画系统
- [ ] 骑手动画蓝图
- [ ] Control Rig 骑手 IK
- [ ] 程序化身体倾斜

### Phase 4: 多人网络
- [ ] GameMode/GameState/PlayerState
- [ ] 载具网络同步
- [ ] GAS 属性复制

### Phase 5: 比赛系统
- [ ] 比赛流程逻辑
- [ ] 山地赛道关卡
- [ ] 竞速 HUD

### Phase 6: 打磨优化
- [ ] 载具手感调优
- [ ] 网络延迟补偿
- [ ] 音效和粒子效果

---

## 核心架构

### 载具系统

```
AMTBPawn (AWheeledVehiclePawn)
├── UChaosWheeledVehicleMovementComponent  // 载具移动组件
├── USkeletalMeshComponent (车架+骑手)      // 骨骼网格体
├── UAbilitySystemComponent                 // GAS组件
├── UMTBAttributeSet                        // 属性集
└── UMTBInputComponent                      // 增强输入
```

### GAS 设计

**属性集**：
- `Stamina` / `MaxStamina` - 耐力值
- `CurrentSpeed` - 当前速度
- `TrickScore` - 特技得分

**能力**：
- `GA_Boost` - 加速，消耗耐力
- `GA_AirTrick` - 空中特技
- `GA_Drift` - 漂移

**效果**：
- `GE_BoostSpeed` - 速度增益
- `GE_StaminaCost` - 耐力消耗
- `GE_StaminaRegen` - 耐力恢复

---

## 目录结构

```
Source/MTBRacing/
├── MTBRacing.Build.cs
├── Core/
│   ├── MTBGameMode.h/cpp
│   ├── MTBGameState.h/cpp
│   └── MTBPlayerState.h/cpp
├── Vehicle/
│   ├── MTBVehiclePawn.h/cpp
│   ├── MTBVehicleMovementComponent.h/cpp
│   └── MTBWheelSetup.h/cpp
├── GAS/
│   ├── MTBAbilitySystemComponent.h/cpp
│   ├── MTBAttributeSet.h/cpp
│   └── Abilities/
│       ├── MTBGameplayAbility.h/cpp
│       ├── GA_Boost.h/cpp
│       └── GA_AirTrick.h/cpp
├── Player/
│   ├── MTBPlayerController.h/cpp
│   └── MTBInputComponent.h/cpp
└── Network/
    └── MTBReplicationHelpers.h/cpp

Content/MTBRacing/
├── Blueprints/
├── Animation/
├── Data/
├── Input/
├── Levels/
└── UI/
```

---

## 关键实现注意事项

### C++/Blueprint 边界
- **Tick 逻辑必须在 C++**：Blueprint VM 开销在每帧执行时会造成性能问题
- **C++ 暴露给 Blueprint**：`UFUNCTION(BlueprintCallable)` 提供设计师友好的 API
- **Blueprint 用于**：高级游戏流程、UI 逻辑、原型设计

### 内存管理
- 所有 `UObject*` 必须用 `UPROPERTY()` 声明
- 非拥有引用用 `TWeakObjectPtr<>`
- Actor 指针跨帧存储前检查 `IsValid()`

### 网络同步
- 载具 Transform：`COND_SimulatedOnly` 复制
- GAS 属性：使用内置复制机制
- RPC：使用 `FScopedServerAbilityRPCBatcher` 批量发送

---

## 开发工作流

**写代码**：Visual Studio / Rider + CodeBuddy 插件
**规划讨论**：WorkBuddy（工作区设在项目目录）
**版本控制**：Git + GitHub

**换电脑继续开发**：
1. `git pull`
2. 打开 IDE + CodeBuddy
3. 如需讨论，WorkBuddy 打开项目目录，说"继续 MTB 项目"
