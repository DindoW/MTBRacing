# MTBRacing 项目 Memory

> 此文件由 WorkBuddy 维护，用于跨端开发时恢复 AI 上下文

## 项目基本信息

- **项目名称**：MTBRacing
- **项目类型**：UE5 C++ 多人山地车竞速 Demo
- **参考游戏**：《极限国度》山地车玩法
- **目标平台**：Windows（开发阶段）
- **多人支持**：2-4人局域网对战

## 技术选型

| 技术 | 说明 |
|------|------|
| 引擎 | UE 5.4 源码版（`C:\Users\Dindo\Documents\repo\UE_5.4`） |
| 载具 | Chaos Vehicle System |
| 能力 | Gameplay Ability System (GAS) |
| 动画 | Animation Blueprint + Control Rig |
| 网络 | UE5 Replication |
| 语言 | C++ + Blueprint 混合 |

## 开发工作流

- **写代码**：Visual Studio / Rider + CodeBuddy 插件
- **规划讨论**：WorkBuddy（工作区设在项目目录）
- **版本控制**：Git + GitHub (https://github.com/DindoW/MTBRacing.git)

## ⚠️ AI 协作模式（重要原则）

**用户要自己学 UE5，所有代码和操作由用户亲自完成。AI 的角色是指导老师，不是代写。**

AI 要做的：
- 讲清架构设计、API 选型、方案对比
- 指路官方文档、引擎源码位置、Editor 操作步骤
- 口述代码**应该长什么样**（关键结构、关键宏、关键签名），不写完整文件
- 读用户代码做 code review
- 编译错误诊断、崩溃分析

AI 不做的：
- ❌ 直接写完整代码文件交付
- ❌ 执行改仓库的命令（除文档/memory 之外）
- ❌ 替用户做任何他能自己做的操作

## 当前进度

- [x] Phase 0: Git 环境配置
- [ ] Phase 1: 载具基础（待开始）
- [ ] Phase 2: GAS 能力系统
- [ ] Phase 3: 动画系统
- [ ] Phase 4: 多人网络
- [ ] Phase 5: 比赛系统
- [ ] Phase 6: 打磨优化

## 关键决策记录

### 2026-04-17
- 选择 Chaos Vehicle System（而非 Chaos Modular Vehicle），因为更简单直接，适合学习阶段
- 确定 C++/Blueprint 混合开发策略：C++ 实现核心逻辑，Blueprint 配置数据
- 开发顺序：单机载具 → GAS → 网络（每阶段保持可运行状态）

### 2026-04-20
- 确认使用 **UE 5.4 源码版**，引擎位于 `C:\Users\Dindo\Documents\repo\UE_5.4`
- 工作区路径保留空格（`Unreal Projects`），不改
- **插件按需加载原则**：不提前启用 ChaosVehicles、GameplayAbilities 等，哪一步需要再加哪个，避免无谓编译开销
- UE 项目壳由用户手动在 Launcher/Editor 中创建（Blank C++ 模板，不带 Starter Content）

## 恢复上下文指令

当在新电脑/新会话继续开发时，告诉 AI：
> "继续 MTB 项目"

AI 会读取此文件和 `docs/DEV_PLAN.md` 恢复上下文。
