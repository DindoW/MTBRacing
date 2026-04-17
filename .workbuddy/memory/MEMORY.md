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
| 引擎 | UE5.3+ |
| 载具 | Chaos Vehicle System |
| 能力 | Gameplay Ability System (GAS) |
| 动画 | Animation Blueprint + Control Rig |
| 网络 | UE5 Replication |
| 语言 | C++ + Blueprint 混合 |

## 开发工作流

- **写代码**：Visual Studio / Rider + CodeBuddy 插件
- **规划讨论**：WorkBuddy（工作区设在项目目录）
- **版本控制**：Git + GitHub (https://github.com/DindoW/MTBRacing.git)

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

## 恢复上下文指令

当在新电脑/新会话继续开发时，告诉 AI：
> "继续 MTB 项目"

AI 会读取此文件和 `docs/DEV_PLAN.md` 恢复上下文。
