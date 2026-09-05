# MTBRacing 项目 Memory

> 此文件由 WorkBuddy 维护，用于跨端开发时恢复 AI 上下文
> 最后更新：2026-08-30

## 记忆系统约定（2026-08-30 统一）

项目 AI 上下文只维护一套记忆系统，位于 `.workbuddy/memory/`：

| 用途 | 文件 |
|---|---|
| **项目规矩（强约束）** | `.workbuddy/rules/mtbracing-rules.md`（AI 自动加载的精简版） |
| **项目规矩（完整版）** | `docs/PROJECT_RULES.md`（R1~R8 全部条款 + 来源索引） |
| 每日工作日志 | `.workbuddy/memory/YYYY-MM-DD.md`（追加式） |
| 长期项目记忆 | `.workbuddy/memory/MEMORY.md`（本文件，跨端恢复用） |
| 开发计划 | `docs/DEV_PLAN.md`（阶段任务分解） |
| 代码 & 资产 | `Source/` + `Content/`（UE 标准） |

**历史遗留清理**：
- ❌ 已删除 `.codebuddy/`（内容已迁移到 08-30 日志）
- ❌ 不再使用根目录 MEMORY.md / FAQ.md / 其他形式

**新会话恢复上下文**：读 `.workbuddy/rules/mtbracing-rules.md`（规矩） + 本文件 + `docs/DEV_PLAN.md` + 最近 1~2 篇日志即可。

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

### ⚠ 本机 Git 缺陷：远程跟踪 ref 无法写入（2026-09-06 定位）

**环境**：git 2.55.0.windows.3。**这不是普通的 packed-refs 陈旧问题，是 git 的 ref 写入路径坏了。**

**症状**：`git fetch` / `git push` / `git update-ref` 全都报告成功（exit 0，
fetch 甚至打印 `旧sha..新sha main -> origin/main`），但 `git rev-parse origin/main`
永远返回旧值，`git status` 持续误报 `ahead N`。

**决定性证据**（`find .git -newer` 取证）：执行 `git update-ref` 后，
磁盘上**只有 `.git/logs/refs/remotes/origin/main`（reflog）被写入**，
ref 本身既没写 loose（`.git/refs/remotes/origin/main` 不存在）
也没写 packed（`packed-refs` mtime 不变）。reflog 里能看到一次次"成功"记录，
但 ref 从未落地。`GIT_TRACE_REFS=1` 显示事务 `finish: 0` 且旧值为全 0
（git 自己认为 ref 不存在，与 `for-each-ref` 读到的值矛盾）。

**已排除**：`.git` / packed-refs 不可写、READONLY 属性、reftable 后端、
OneDrive 同步回滚、自定义 hook（那 4 个是 Git LFS 标准安装）、
WorkBuddy 沙箱隔离（无沙箱下同样失败）、junction/reparse point。

**危害**：`git diff HEAD origin/main` 会拿远古提交做对比，输出「大量文件被删除」
的假 diff，看着像远程删库。**绝不能据此 `reset --hard` / `push --force`**。
矛盾判据：`git log HEAD..origin/main` 为空却显示大量删除 → 必属此坑。
取证用 `git ls-remote origin refs/heads/main` 拿远程真实值。

**修法（shell 直写，已验证可靠；git 自己的命令无效）**：
```bash
REAL=$(git ls-remote origin refs/heads/main | cut -f1)   # 远程真实值
LOCAL=$(cat .git/refs/heads/main)                        # 本地 HEAD
mkdir -p .git/refs/remotes/origin
printf '%s\n' "$REAL" > .git/refs/remotes/origin/main    # loose ref 优先级最高
# packed-refs 里的陈旧记录也要一起改，否则 loose ref 被清理后会复现
cp .git/packed-refs .git/packed-refs.bak
{ echo '# pack-refs with: peeled fully-peeled sorted '
  printf '%s refs/heads/main\n' "$LOCAL"
  printf '%s refs/remotes/origin/main\n' "$REAL"; } > .git/packed-refs
```
校验三值一致：`git rev-parse HEAD` / `git rev-parse origin/main` /
`git ls-remote origin refs/heads/main`，然后 `git fsck` 应零报错。

**⚠ 不要用 `git pack-refs --all`**：08-30 和 09-06 两次都试过，它能把当时的值
写进 packed-refs，但会顺手删掉 loose ref 和 `refs/remotes/origin/` 目录，
下次 push 后问题立刻复发。只能用上面的 shell 直写方案。

**建议根治方向**（下次有空时）：这大概率是 git 2.55.0.windows.3 的缺陷或本仓库
ref 存储的局部损坏。可尝试 ① 降级/升级 Git for Windows；
② `git clone` 一份新的本地副本，把工作区文件拷过去（最干净）。
在那之前，每次 push/fetch 后都要跑一遍上面的修法脚本。


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

## 当前源码结构（2026-09-05）

```
Source/MTBRacing/
├── MTBRacing.Build.cs       (Core/CoreUObject/Engine/InputCore/EnhancedInput
│                            + GameplayAbilities/GameplayTags/GameplayTasks)
├── MTBRacing.h / .cpp
├── Public/
│   ├── Core/MTBGameMode.h
│   ├── Pawn/BikePawn.h      (APawn 派生，最简实现)
│   └── Gas/MTBAttributeSet.h
└── Private/
    ├── Core/MTBGameMode.cpp (DefaultPawnClass = ABikePawn)
    ├── Pawn/BikePawn.cpp
    └── Gas/MTBAttributeSet.cpp
```

> 子目录命名统一 **PascalCase**：写 `Gas/` 不写 `GAS/`（2026-09-06 定调，
> 已同步到 `PROJECT_RULES.md` R4.1、`mtbracing-rules.md`、`DEV_PLAN.md`）。

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
  - [x] 2.1 Build.cs + .uproject 加 GAS 三模块，同时清除 ChaosVehicles 残留（08-31）
  - [x] 2.2 `UMTBAttributeSet` —— Stamina / MaxStamina / StaminaRegenRate / MaxSpeed（09-05）
  - [ ] **⚙ 下次开工第一件事：补 `PreAttributeBaseChange`**（见下方遗留问题）
  - [ ] 2.3 ASC 挂 BikePawn + MaxSpeed→Movement 搭桥
  - [ ] 2.4 `GA_Boost`
  - [ ] 2.5 `GE_StaminaRegen`
  - [ ] 2.6 简易 HUD（耐力条 + 速度）
- [ ] Phase 3: 比赛流程（PlayerStart/检查点/计时器/结算 UI）
- [ ] Phase 4: 多人网络
- [ ] Phase 5: 视觉替换（骑手 + 自行车骨骼模型）
- [ ] Phase 6: 物理升级（可选回归 Chaos Vehicle）
- [ ] Phase 7: 打磨优化

## ⚠ 当前遗留问题（下次开工先处理）

### 🔴 P0：`UMTBAttributeSet` 缺 `PreAttributeBaseChange` —— 2.5 会真爆

已从引擎源码确认两条**完全独立**的钳制路径：
- CurrentValue：`AttributeSet.cpp:99-102` — `PreAttributeChange` → `SetCurrentValue`
- BaseValue：`GameplayEffect.cpp:3720-3732` — `PreAttributeBaseChange` → `SetBaseValue`

当前只覆写了前者。`GE_StaminaRegen`（Infinite + Period=1.0）是 Periodic GE，
每次触发按 Instant 处理、改 BaseValue → **Stamina BaseValue 无上限增长**（100→110→…→700）。

欺骗性极强：`GetStamina()` 读 CurrentValue，被钳到 100，HUD 显示 100/100 一切正常。
但 Boost 扣耐力时要先消耗掰虚高的 BaseValue → 表现为"Boost 怎么用都不掉耐力"，
然后到某个点突然暴跌。从现象无法反推到根因。

**修法要点**：签名结尾必须有 `const`（`AttributeSet.h:226`），
漏了就不是覆写而是新增函数 —— 编译过但永不被调用，又一个静默失效。

### 🟡 P1：同文件其他 TODO（已写入代码注释）

1. `PreAttributeChange` 没调 `Super::` —— 5.4 基类是空实现故无功能 bug，
   但插入中间基类时会静默跳过它的钳制
2. `MaxStamina = 100.;` 少 `f` 后缀（double 字面量，功能无碍但不一致）
3. `MaxSpeed` 无钳制 —— 配错系数的 GE 能让它变负数，车会往后飘
4. `MaxStamina` 变小时 `Stamina` 不跟随收缩（Phase 2 不会触发）

### 🟡 P1：`HandleSteering` 旋转积分位置错误（Phase 4 会爆）

在输入回调里调 `GetWorld()->GetDeltaSeconds()` 做积分。输入回调每帧触发次数
不保证为 1（按住时 Triggered 可能多次、松开还有 Completed），转向速度会随触发次数漂移。
正确做法：回调只存 `SteeringInput`，在 `Tick` 里统一积分（也符合 "Tick 逻辑在 C++"）。
当前单机手感已调好、问题被掩盖，但网络预测阶段必爆。

### ✅ 已修（2026-09-06）

- `MTBGameMode.h` 构造函数补上 `public:`（原先靠 `GENERATED_BODY()` 展开末尾恰好是 `public:` 才能编译，属巧合）

## 手感调参基线（山地版，2026-06-09）

仅保存在 `BP_CubeBike` 实例值，C++ 默认值未改动：

| 参数 | C++ 默认 | BP 实例（采用） |
|---|---|---|
| MaxSpeed | 2000 | 2000 |
| Acceleration | 1500 | **1800** |
| Deceleration | 1200 | **1500** |
| TurnSpeed | 120 | **140** |

特征：平衡型，速度感适中、转向灵活、刹车干脆。后续 GAS Boost 调参以此为对照基线。

## UE 5.4 GAS 关键事实（从本机引擎源码实测，非教程转述）

源码位置：`C:\Users\Dindo\Documents\repo\UE_5.4\Engine\Plugins\Runtime\GameplayAbilities\Source\GameplayAbilities\Public\`

### 1. `ATTRIBUTE_ACCESSORS` 宏引擎【不提供】，必须项目自己 #define

`AttributeSet.h:418` 里的 `ATTRIBUTE_ACCESSORS` 只是**注释里的建议写法**，不是真宏。引擎只提供 4 个基础宏（`AttributeSet.h:427/434/440/450`）：
`GAMEPLAYATTRIBUTE_PROPERTY_GETTER` / `_VALUE_GETTER` / `_VALUE_SETTER` / `_VALUE_INITTER`

必须在自己的 AttributeSet 头文件里把这 4 个组合成 `ATTRIBUTE_ACCESSORS`。所有网上教程都直接用它却不说这点，是最常见的首个编译错误来源。

### 2. GameplayEffect 在 5.3 起改为 Component 架构（网上老教程全部过时）

`GameplayEffect.h` 里有大量 `UE_DEPRECATED(5.3, ...)`。旧的 GE 内联字段被拆成独立 `UGameplayEffectComponent`，目录：`Public/GameplayEffectComponents/`（10 个）。

对照表（旧 → 新）：
- `Modifiers` Tag 需求 → `UTargetTagRequirementsGameplayEffectComponent`
- 授予 Tag → `UTargetTagsGameplayEffectComponent`
- 资产 Tag → `UAssetTagsGameplayEffectComponent`
- 授予能力 → `UAbilitiesGameplayEffectComponent`
- 连带效果 → `UAdditionalEffectsGameplayEffectComponent`
- 概率生效 → `UChanceToApplyGameplayEffectComponent`
- 免疫 → `UImmunityGameplayEffectComponent`
- 移除其他 GE → `URemoveOtherGameplayEffectComponent`

**判据**：看教程时若 GE 的 Details 面板截图里有 "Granted Tags"、"Ongoing Tag Requirements" 等内联字段，就是 5.2 及更早的，5.4 里要改用对应 Component。

### 3. `FGameplayAttributeData` 双值结构（`AttributeSet.h:19-53`）

`BaseValue`（永久值，Instant GE 改）+ `CurrentValue`（含临时 buff，Duration/Infinite GE 改）。
Boost 用 Infinite GE 抬 CurrentValue，移除后自动回落 —— 不需要手写"恢复原值"逻辑。

### 4. ASC 三种复制模式（`AbilitySystemComponent.h:85-93`）

- `Minimal` — 只复制最小信息。**注释明确：不适用于 Owned ASC（玩家自己的），要用 Mixed**
- `Mixed` — 对 SimulatedProxy 最小、对 Owner/AutonomousProxy 完整。玩家角色标准选项
- `Full` — 全量复制给所有人。单机 / AI 用

### 5. 关键 API 签名（已实测）

- 属性变化监听：`ASC->GetGameplayAttributeValueChangeDelegate(Attr)`（`AbilitySystemComponent.h:534`）；旧的 `RegisterGameplayAttributeEvent` 已被建议替换
- 初始化：`InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)`（`:1547`）
- AttributeSet 三个钩子：`PreAttributeChange`(:215) / `PreAttributeBaseChange`(:226) / `PostGameplayEffectExecute`(:201)

### 参考文献（按可信度排序）

1. 本机引擎源码（最高权威，版本精确匹配）
2. tranek/GASDocumentation — https://github.com/tranek/GASDocumentation （社区事实标准，UE5.3 基准）
3. Epic 官方 GAS 文档 — https://dev.epicgames.com/documentation/unreal-engine/gameplay-ability-system-for-unreal-engine
4. Epic 官方 Lyra Abilities — https://docs.unrealengine.com/en-US/abilities-in-lyra-in-unreal-engine

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

### 2026-08-30（记忆系统整合 + 项目规矩成文）
- 项目搁置 2 个多月重启
- 决策：以 `.workbuddy/memory/` 为唯一记忆系统，删除 `.codebuddy/`
- 迁移 `.codebuddy/memory/2026-06-09.md` 里的 2 条 Q&A（AddMovementInput / ControlInputVector 数据流）到 08-30 日志
- 目录约定写入本文件顶部
- **规矩成文**：从 DEV_PLAN.md + MEMORY.md + 全部日志沉淀出 R1~R8 八类规矩
  - `docs/PROJECT_RULES.md` — 完整版（含来源索引、修订记录）
  - `.workbuddy/rules/mtbracing-rules.md` — AI 强约束精简版（自动加载）
  - 两份文件分工：精简版是 AI 每次必读的硬约束，完整版是人看的详细依据
- 后续 Phase 2 GAS 采用标准 AI 协作模式（讲架构 + 用户自己写代码）

### 2026-08-30 晚 ~ 08-31（Phase 2 启动，2.1 完成）
- Code review 发现 `.uproject` 残留 ChaosVehicles（6-08 转向时只清了 Build.cs），已随 2.1 一并清除
- **属性设计修正**：`CurrentSpeed` 不做 GameplayAttribute。判据 = "会不会被 GE 修改"；
  它是 `Movement->Velocity` 的派生只读值 → 改为 `StaminaRegenRate` + `MaxSpeed`
- **ASC 挂载决策：BikePawn**（非 PlayerState）。理由：竞速重置检查点用 `SetActorLocation`
  不销毁 Pawn，PlayerState 的"跨 Pawn 存活"优势用不上；Phase 4 若需要再迁移
- ReplicationMode：Phase 2 单机用 `Full`，Phase 4 转 `Mixed`（`Minimal` 不能用于玩家自己的 ASC）
- 建议实施顺序调整：2.5（GE，纯 Editor 配置）提到 2.4（GA_Boost）之前，先验证 GE 管道
- 2.1 三个验证点通过：Editor 打开 / Plugins 里 GAS 已启用 / Project Settings 能搜到 Gameplay Tags

### 2026-09-05（2.2 完成 + 深度 review）
- 用户自己写完 `UMTBAttributeSet`，编译通过。四个属性 / REPNOTIFY / 复制条件写法均正确
- **破例记录（R1 例外）**：用户明确要求"review 时把知识点写在注释里" →
  AI 直接改了 `Gas/MTBAttributeSet.h/.cpp`，**只加注释和 TODO 标记，不改任何代码逻辑**，
  bug 修复留给用户自己动手
- 发现 P0 真 bug：缺 `PreAttributeBaseChange`（详见上方"当前遗留问题"章节）

### 2026-09-06（目录命名定调 + 进度存档）
- **`Gas/` 定为约定**（非 `GAS/`）。理由：符合 UE 的 PascalCase 目录习惯，全大写反而是例外
- 已同步三份文档：`PROJECT_RULES.md` R4.1（附录 B 记 v1.1）、
  `mtbracing-rules.md` 第 3 节、`DEV_PLAN.md` 目标源码结构
- 补充通则：子目录名统一 PascalCase，缩写词照此处理

## 恢复上下文指令

新电脑/新会话继续开发时，告诉 AI：
> "继续 MTB 项目"

AI 会读取此文件 + `docs/DEV_PLAN.md` + 最近的日志恢复上下文。
