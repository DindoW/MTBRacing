# MTBRacing 项目 Memory

> 此文件由 WorkBuddy 维护，用于跨端开发时恢复 AI 上下文
> 最后更新：2026-08-30

## 记忆系统约定（2026-08-30 统一）

项目 AI 上下文只维护一套记忆系统，位于 `.workbuddy/memory/`：

| 用途 | 文件 |
|---|---|
| 每日工作日志 | `.workbuddy/memory/YYYY-MM-DD.md`（追加式） |
| 长期项目记忆 | `.workbuddy/memory/MEMORY.md`（本文件，跨端恢复用） |
| 开发计划 | `docs/DEV_PLAN.md`（阶段任务分解） |
| 代码 & 资产 | `Source/` + `Content/`（UE 标准） |

**历史遗留清理**：
- ❌ 已删除 `.codebuddy/`（内容已迁移到 08-30 日志）
- ❌ 不再使用根目录 MEMORY.md / FAQ.md / 其他形式

**新会话恢复上下文**：读本文件 + `docs/DEV_PLAN.md` + 最近 1~2 篇日志即可。

## 项目基本信息

- **项目名称**：MTBRacing
- **项目类型**：UE5 C++ 多人山地车竞速 Demo
- **参考游戏**：《极限国度》山地车玩法
- **目标平台**：Windows（开发阶段）
- **多人支持**：2-4 人局域网对战（远期目标）

## 技术选型（已迭代）

| 类别 | 当前选择 | 备注 |
|------|----------|------|
| 引擎 | UE 5.4 源码版 | |
| 载具实现 | **APawn + FloatingPawnMovement**（手写最简版） | 原计划 Chaos Vehicle，因模型骨骼/物理调试卡壳，2026-06-08 转向最简实现先跑通流程 |
| 能力 | Gameplay Ability System (GAS) | 后期阶段加入 |
| 动画 | Animation Blueprint + Control Rig | 后期阶段加入 |
| 网络 | UE5 Replication | 后期阶段加入 |
| 语言 | C++ + Blueprint 混合 | C++ 实现核心，Blueprint 配置 |

> **重要**：原计划用 Chaos Vehicle System，但因为两轮载具骨骼/物理资产准备工作量过大（折腾 SM_Bike 轴向、Sketchfab 资源整理、Vehicle Template 迁移等），现已转向最简手写实现。Chaos Vehicle 不是不做，而是**等基础流程跑通、有时间精力时再回头加入**。`Content/Vehicles/SportsCar/` 资产已迁移保留，备用。

## 开发环境（多台机器）

| 机器 | 引擎路径 | 项目路径 |
|------|----------|----------|
| 台式机 | `C:\Users\Dindo\Documents\repo\UE_5.4` | `C:\Users\Dindo\Documents\repo\Unreal Projects\MTBRacing\` |
| 笔记本 | `D:\Projects\UnrealEngine`（5.4.4-release 源码版） | `D:\Projects\MTBRacing\` |

## 开发工作流

- **写代码**：Visual Studio / Rider + CodeBuddy 插件
- **规划讨论**：WorkBuddy（工作区设在项目目录）
- **版本控制**：Git + GitHub (https://github.com/DindoW/MTBRacing.git)
- **生成 VS 工程**：必须用源码版的 `GenerateProjectFiles.bat`，不能右键 `.uproject`

## ⚠️ AI 协作模式（重要原则）

**用户要自己学 UE5，绝大多数代码和操作由用户亲自完成。AI 是指导老师，不是代写。**

AI 默认要做：
- 讲清架构设计、API 选型、方案对比
- 指路官方文档、引擎源码位置、Editor 操作步骤
- 口述代码**应该长什么样**（关键结构、关键宏、关键签名）
- 读用户代码做 code review
- 编译错误诊断、崩溃分析

AI 默认不做：
- ❌ 直接写完整代码文件交付
- ❌ 替用户做任何他能自己做的操作

**例外**：当用户**明确要求**"帮我写"、"重写"、"改代码"时，AI 才直接动文件。这种情况要在日志里记录，避免渐渐变成"代写默认"。

## 当前源码结构（2026-06-08）

```
Source/MTBRacing/
├── MTBRacing.Build.cs       (依赖：Core/CoreUObject/Engine/InputCore/EnhancedInput)
├── MTBRacing.h / .cpp
├── Public/
│   ├── Core/MTBGameMode.h
│   └── Pawn/BikePawn.h      (APawn 派生，最简实现)
└── Private/
    ├── Core/MTBGameMode.cpp (DefaultPawnClass = ABikePawn)
    └── Pawn/BikePawn.cpp
```

## 当前 Content 资产

```
Content/
├── Blueprints/
│   ├── BP_CubeBike.uasset         (BikePawn 的 BP 子类，挂 Input 资产)
│   └── BP_MTBGameMode.uasset
├── Input/
│   ├── IMC_Bike.uasset
│   ├── IA_Pedaling.uasset         (W/S Axis1D)
│   ├── IA_Steering.uasset         (A/D Axis1D)
│   └── IA_Handbrake.uasset        (Space Digital)
├── Levels/L_TestTrack.umap
├── Vehicles/SportsCar/            (备用资产，当前未使用)
└── MotoInteractionAnims/          (备用动画资产，当前未使用)
```

## 当前进度

- [x] Phase 0: Git 环境配置
- [x] Phase 1: 载具基础（**已重定向并完成**）✅
  - [x] C++ 项目壳 + 模块依赖
  - [x] Core/MTBGameMode 骨架
  - [x] Input 资产（IMC + 3 个 IA）
  - [x] BikePawn（最简 APawn 实现，Cube + FloatingPawnMovement）
  - [x] PIE 验证 WASD 移动 / Space 刹车（2026-06-08）
  - [x] 1.7 手感调参（山地版基线，2026-06-09）
- [x] Git 提交 Phase 1 成果到 GitHub（2026-06-09，commit 857315b）
- [ ] **当前节点：Phase 2 GAS 能力系统（耐力、Boost）**
- [ ] Phase 3: 比赛流程（PlayerStart/检查点/计时器/结算 UI）
- [ ] Phase 4: 多人网络
- [ ] Phase 5: 视觉替换（骑手 + 自行车骨骼模型）
- [ ] Phase 6: 物理升级（可选回归 Chaos Vehicle）
- [ ] Phase 7: 打磨优化

## 手感调参基线（山地版，2026-06-09）

仅保存在 `BP_CubeBike` 实例值，C++ 默认值未改动：

| 参数 | C++ 默认 | BP 实例（采用） |
|---|---|---|
| MaxSpeed | 2000 | 2000 |
| Acceleration | 1500 | **1800** |
| Deceleration | 1200 | **1500** |
| TurnSpeed | 120 | **140** |

特征：平衡型，速度感适中、转向灵活、刹车干脆。后续 GAS Boost 调参以此为对照基线。

## 关键决策记录

### 2026-04-17
- 选择 Chaos Vehicle System（而非 Chaos Modular Vehicle），更简单适合学习
- 确定 C++/Blueprint 混合策略
- 开发顺序：单机载具 → GAS → 网络

### 2026-04-20
- UE 5.4 源码版
- **插件按需加载原则**：哪一步需要再启用哪个插件
- UE 项目壳由用户手动在 Launcher/Editor 创建（Blank C++，不带 Starter Content）

### 2026-05-08
- 换电脑开发，路径迁移到 D 盘
- Phase 1 第 2~4 步完成：Core/Pawn/BikeFrontWheel/BikeRearWheel 全部就位
- 采用 Public/Private 分离目录

### 2026-06-02
- 卡在两轮载具骨骼资产：SM_Bike 前向轴是 Y，方案讨论了多种
- 迁移 Vehicle Template 的 SK_SportsCar 到项目作为占位车

### 2026-06-08（重大转向）
- **放弃 AWheeledVehiclePawn + Chaos Vehicle**，改为最简 APawn + FloatingPawnMovement
- 删除 BikeFrontWheel / BikeRearWheel 类
- Build.cs 移除 ChaosVehicles 依赖
- 由 AI 直接重写 BikePawn.h/cpp（破例代写，因用户明确要求）
- **理由**：先把"输入 → 移动 → 关卡测试"这条最短验证链跑通，再回头补物理与模型
- 当晚 PIE 验证通过（WASD/Space 全部生效）

### 2026-06-09（Phase 1 收尾）
- 手感调参完成，采用山地版（MaxSpeed=2000 / Accel=1800 / Decel=1500 / TurnSpeed=140）
- 数值仅保存在 BP_CubeBike 实例，C++ 默认值保持不动（用户决策）
- 文档同步：DEV_PLAN.md 1.7 勾选 Phase 1 完成，调参表新增"BP 实例值"列
- Phase 1 成果 push 到 GitHub（commit 857315b）

### 2026-08-30（记忆系统整合）
- 项目搁置 2 个多月重启
- 决策：以 `.workbuddy/memory/` 为唯一记忆系统，删除 `.codebuddy/`
- 迁移 `.codebuddy/memory/2026-06-09.md` 里的 2 条 Q&A（AddMovementInput / ControlInputVector 数据流）到 08-30 日志
- 目录约定写入本文件顶部
- 后续 Phase 2 GAS 采用标准 AI 协作模式（讲架构 + 用户自己写代码）

## 恢复上下文指令

新电脑/新会话继续开发时，告诉 AI：
> "继续 MTB 项目"

AI 会读取此文件 + `docs/DEV_PLAN.md` + 最近的日志恢复上下文。
