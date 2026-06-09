# MTBRacing 开发计划

> 最后更新：2026-06-08（v2）
> 历史版本要点见底部"计划演进记录"

## 产品概述

一款基于 UE5 的第三人称多人山地车竞速 Demo，参考《极限国度》山地车玩法。玩家操控骑手在山地赛道竞速，远期支持 2-4 人局域网联机。

## 当前阶段策略：先跑通最短验证链

> **重要思路转变**：原计划直接上 Chaos Vehicle + 骨骼网格 + 自行车物理，但实际推进时卡在两轮载具骨骼/物理资产准备工作上（前向轴问题、骨骼绑定、PhysicsAsset 调试等）。
>
> **新策略**：分阶段验证，先用最简实现把"输入 → 移动 → 关卡 → 多人 → GAS"主干打通，再回头逐项替换为正式版本。这样每一步都有可运行结果，避免被某个子系统阻塞整个项目。

```
阶段策略时间轴：

        [当前]                                              [远期]
最简 APawn         GAS + 简单玩法         多人        替换为正式车辆物理 + 模型
+ Cube + Float ─→ 耐力/Boost/特技 ─→ Replication ─→ Chaos Vehicle + Skeletal Mesh
+ Enhanced Input
```

---

## 核心功能（最终目标，按优先级排序）

### P0（最小可玩 Demo）
1. **可控制的载具角色** —— 一个 Pawn，能用键盘/手柄前进、转向、刹车
2. **关卡 + 终点** —— 一段简单赛道，到终点显示胜利
3. **第三人称相机**

### P1（核心体验）
4. **载具手感** —— 加速度、最大速度、转向半径、刹车距离的调参
5. **GAS 能力系统** —— Boost 加速、耐力管理
6. **简易竞速 HUD** —— 速度表、计时器

### P2（完整 Demo）
7. **多人网络** —— 2-4 人局域网，载具同步
8. **特技系统** —— 空中翻转得分（GAS 能力）
9. **正式视觉** —— 骑手 + 自行车骨骼模型替换 Cube

### P3（打磨）
10. **正式物理** —— 用 Chaos Vehicle 替换 FloatingPawnMovement
11. **音效粒子** —— 引擎声、漂移粒子
12. **网络延迟补偿**

---

## 技术栈选型

| 类别 | 当前实际 | 远期目标 |
|---|---|---|
| 引擎 | UE 5.4 源码版 | 同 |
| 载具 | **APawn + UFloatingPawnMovement**（手写） | Chaos Vehicle System（P3 替换） |
| 能力 | 暂无 | Gameplay Ability System (GAS) |
| 动画 | 暂无（Cube 静态网格） | Animation Blueprint + Control Rig |
| 网络 | 单机 | UE5 Replication |
| 输入 | Enhanced Input ✅ | 同 |
| 语言 | C++ + Blueprint 混合 | 同 |
| 版本控制 | Git + GitHub | 同 |

---

## 开发阶段

### Phase 0: 环境准备 ✅
- Git / LFS / GitHub 仓库就位

### Phase 1: 载具基础（重新定义） ⚙️ 进行中
原计划：Chaos Vehicle 完整实现。
**新计划**：最简 APawn 跑通输入和移动。

- [x] 1.1 UE 5.4 C++ 项目壳，模块加载正常
- [x] 1.2 Core/MTBGameMode 骨架（继承 AGameModeBase，空构造）
- [x] 1.3 Input 资产创建（IMC_Bike + IA_Pedaling/Steering/Handbrake）
- [x] 1.4 BikePawn（APawn 派生）
  - [x] BoxCollision + Cube Mesh + SpringArm + Camera + FloatingPawnMovement
  - [x] Enhanced Input 绑定（BeginPlay 注册 IMC，SetupPlayerInputComponent 绑 IA）
  - [x] HandleThrottle / HandleSteering / HandleBrake 三个回调
- [x] 1.5 MTBGameMode 设 DefaultPawnClass = ABikePawn
- [x] 1.6 PIE 验证 ✅（2026-06-08）
  - [x] 编译通过
  - [x] 关卡能 Spawn Pawn
  - [x] WASD 控制方块移动转向
  - [x] Space 刹车
- [x] 1.7 手感调参 ✅（2026-06-09，山地版基线）
  - [x] MaxSpeed ≈ 2000
  - [x] Acceleration ≈ 1800
  - [x] Deceleration ≈ 1500
  - [x] TurnSpeed ≈ 140
  - [x] 数值仅保存在 BP_CubeBike 实例（C++ 默认值未改动）

> **Phase 1 完成** ✅。最简载具骨架（输入 → 移动 → 关卡）全部跑通，手感基线确定。

### Phase 2: GAS 能力系统
**前置**：Phase 1 完成、能稳定 PIE。

- [ ] 2.1 Build.cs 添加 `GameplayAbilities` / `GameplayTags` / `GameplayTasks`
- [ ] 2.2 `UMTBAttributeSet` —— Stamina / MaxStamina / CurrentSpeed
- [ ] 2.3 `UMTBAbilitySystemComponent` 挂到 BikePawn
- [ ] 2.4 `GA_Boost` —— 提升 MaxSpeed，消耗 Stamina（用 GameplayEffect 不直接改属性）
- [ ] 2.5 `GE_StaminaRegen` —— 每秒恢复耐力
- [ ] 2.6 简易 HUD：耐力条 + 速度

### Phase 3: 比赛流程（先于多人，方便单机调试）
- [ ] 3.1 PlayerStart + 起跑线
- [ ] 3.2 检查点 / 终点 Trigger（继承 AActor + UBoxComponent）
- [ ] 3.3 计时器 + UMG HUD（Timer + 排名占位）
- [ ] 3.4 终点结算 UI

### Phase 4: 多人网络
- [ ] 4.1 GameMode / GameState / PlayerState 拆分
- [ ] 4.2 BikePawn Replication（位置/速度/输入意图）
- [ ] 4.3 GAS 属性复制
- [ ] 4.4 局域网测试（Listen Server + 1 Client）

### Phase 5: 视觉替换
- [ ] 5.1 替换 Cube Mesh 为骑手 + 自行车 SkeletalMesh
- [ ] 5.2 程序化车体倾斜（蓝图 Anim Graph 用 Pawn 速度驱动）
- [ ] 5.3 骑手 IK（Control Rig，手放把手、脚踩踏板）

### Phase 6: 物理升级（可选 / 可推到最后）
- [ ] 6.1 评估是否要换 Chaos Vehicle（可能 FloatingPawnMovement 调好就够了）
- [ ] 6.2 如换：BikePawn 改继承 AWheeledVehiclePawn，配 Wheel Setups
- [ ] 6.3 重新调试整套手感

### Phase 7: 打磨
- [ ] 引擎音效、漂移粒子
- [ ] 网络延迟补偿
- [ ] 性能 profile

---

## 当前架构（C++）

### Pawn 系统（最简版）

```
ABikePawn : APawn
├── UBoxComponent           (Root, 碰撞)
├── UStaticMeshComponent    (Cube 可视化)
├── USpringArmComponent     (相机臂)
├── UCameraComponent        (第三人称)
├── UFloatingPawnMovement   (移动组件)
└── Input 资产引用
    ├── BikeIMC (UInputMappingContext*)
    ├── PedalingAction / SteeringAction / HandbrakeAction (UInputAction*)
```

### 输入流

```
键盘 W/S → IA_Pedaling      → HandleThrottle  → AddMovementInput(ForwardVector, Val)
键盘 A/D → IA_Steering      → HandleSteering  → AddActorWorldRotation(Yaw)
键盘 SPC → IA_Handbrake     → HandleBrake     → Movement->StopMovementImmediately()
```

### 调参点（已暴露 UPROPERTY EditAnywhere）

| 参数 | C++ 默认值 | BP_CubeBike 实例值（山地版） | 含义 |
|---|---|---|---|
| Movement->MaxSpeed | 2000 | **2000** | 最大速度（cm/s）= 20m/s |
| Movement->Acceleration | 1500 | **1800** | 加速度 |
| Movement->Deceleration | 1200 | **1500** | 自然减速度 |
| TurnSpeed | 120 | **140** | 转向速度（°/s） |

> 山地版手感基线（2026-06-09 调参确定）：平衡型，速度感适中、转向灵活、刹车干脆。
> 后续 GAS Boost 能力调参以此为对照基线。

---

## 目标源码结构（远期）

```
Source/MTBRacing/
├── Core/                  ✅ MTBGameMode
├── Pawn/                  ✅ BikePawn
├── GAS/                     Phase 2 加入
│   ├── MTBAttributeSet
│   ├── MTBAbilitySystemComponent
│   └── Abilities/
│       ├── GA_Boost
│       └── GA_AirTrick
├── Race/                    Phase 3 加入
│   ├── Checkpoint
│   └── RaceManager
└── Player/                  Phase 4 加入（可能）
    └── MTBPlayerController
```

---

## 关键实现注意事项

### C++/Blueprint 边界
- **Tick 逻辑必须在 C++**：Blueprint VM 开销在每帧执行时会造成性能问题
- **C++ 暴露给 Blueprint**：`UFUNCTION(BlueprintCallable)` 提供设计师友好的 API
- **Blueprint 用于**：高级游戏流程、UI 逻辑、原型设计、Pawn 子类配资产引用

### 内存管理
- 所有 `UObject*` 必须用 `UPROPERTY()` 声明
- 非拥有引用用 `TWeakObjectPtr<>`
- Actor 指针跨帧存储前检查 `IsValid()`

### GAS（Phase 2 起）
- 属性变化必须经过 GameplayEffect，不直接改 AttributeSet 字段
- 所有事件标识用 FGameplayTag，不用字符串
- 服务器权威，客户端预测通过 ASC 内置机制

### 网络同步（Phase 4 起）
- 载具 Transform：`COND_SimulatedOnly` 复制
- GAS 属性：使用内置复制机制
- 输入意图（Throttle/Steer 值）走 Server RPC

---

## 计划演进记录

### v1（2026-04-17）
- 直接上 Chaos Vehicle + Skeletal Mesh + GAS 全套架构
- 目标 Phase 1 直接做完整车辆物理

### v2（2026-06-08）—— 当前
- Phase 1 重新定义为"最简 APawn 跑通输入和移动"
- Chaos Vehicle 推到 Phase 6（可选）
- 引入分阶段验证思路：每阶段产出可运行结果

---

## 开发工作流

**写代码**：Visual Studio / Rider + CodeBuddy 插件
**规划讨论**：WorkBuddy（工作区设在项目目录）
**版本控制**：Git + GitHub

**换电脑继续**：
1. `git pull`
2. 用源码版引擎的 `GenerateProjectFiles.bat` 生成 .sln
3. VS 编译 Development Editor | Win64
4. 双击 `.uproject` 打开 Editor
5. 如需讨论，WorkBuddy 打开项目目录，说"继续 MTB 项目"
