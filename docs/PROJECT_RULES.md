# MTBRacing 项目规则（Rules）

> 本文件是从 `docs/DEV_PLAN.md` 和 `.workbuddy/memory/` 沉淀出来的项目约束，凡是与本文冲突的行为都算违规。
> 最后更新：2026-08-30

新会话/新协作者进项目，先读本文件、再读 `.workbuddy/memory/MEMORY.md` 和 `docs/DEV_PLAN.md`。

---

## 一、AI 协作模式（最高优先级）

### R1.1 学习导向，不代写
- 用户的目标是**自己学会 UE5**，绝大多数代码由用户亲手编写。
- AI 是**指导老师**，不是代码交付方。

### R1.2 AI 默认要做
- 讲清架构设计、API 选型、方案对比
- 指路官方文档、引擎源码位置、Editor 操作步骤
- 口述关键代码"应该长什么样"（关键结构、宏、签名）
- 做 code review、诊断编译错误 / 崩溃

### R1.3 AI 默认不做
- ❌ 直接写完整代码文件交付
- ❌ 替用户做任何他能自己做的 Editor 操作

### R1.4 代写的例外
- **只有**当用户明确说"帮我写"、"重写"、"改代码"时，AI 才能直接动源码文件。
- 例外行为**必须在当日日志里记录**，防止渐渐变成"代写默认"。
- 如果用户的表述模糊（例如"做一下"、"搞一个"），AI 应先问一句"是要代写还是讲解"再动手。

### R1.5 沟通语言与风格
- 全程中文
- 简洁直接、行动导向
- 提供具体路径、行号；先方案再实施
- 修改前先分析安全性，确认后再动

---

## 二、开发计划与阶段

### R2.1 分阶段验证策略
- 严格按 `docs/DEV_PLAN.md` 的 Phase 顺序推进：**Phase 1 → 2 → 3 → 4 → 5 → 6 → 7**
- 每阶段必须产出**可运行结果**（PIE 能跑、能验收）
- 不允许在某个子系统上死磕，卡住就走"最简验证链"绕过

### R2.2 P0/P1/P2/P3 优先级
- P0 最小可玩、P1 核心体验、P2 完整 Demo、P3 打磨
- 不完成 P0 不做 P1，以此类推

### R2.3 阶段完成的判定
- 对应 Phase 的所有子任务在 `DEV_PLAN.md` 里都打勾
- 关键节点（如 Phase 1 完成）必须 `git commit + push` 到 GitHub 固化

---

## 三、技术选型约束

### R3.1 引擎与语言
- 引擎：**UE 5.4 源码版**（不用 Launcher 二进制版做主开发）
- 语言：**C++ + Blueprint 混合**
  - Tick 逻辑必须写在 C++
  - `UFUNCTION(BlueprintCallable)` 暴露给蓝图
  - Blueprint 用于：高级游戏流程、UI 逻辑、原型、Pawn 子类挂资产

### R3.2 当前载具实现
- 当前使用 **APawn + UFloatingPawnMovement**（不是 AWheeledVehiclePawn）
- Chaos Vehicle 已推到 Phase 6，属可选项，未到该阶段不引入相关模块
- `MTBRacing.Build.cs` 不允许出现 `"ChaosVehicles"` 依赖，除非到 Phase 6 明确评估通过

### R3.3 插件按需加载
- 哪一步需要哪个插件才启用
- 不预装未使用的插件，避免污染项目和拖慢编译

### R3.4 GAS 使用规约（Phase 2 起）
- 属性变化必须走 GameplayEffect，**禁止直接改 AttributeSet 字段**
- 所有事件用 `FGameplayTag`，禁止裸字符串
- 服务器权威，客户端预测走 ASC 内置机制
- Build.cs 三件套必须齐：`GameplayAbilities` / `GameplayTags` / `GameplayTasks`

### R3.5 内存管理
- 所有 `UObject*` 必须 `UPROPERTY()` 声明
- 非拥有引用用 `TWeakObjectPtr<>`
- 跨帧存 Actor 指针前 `IsValid()` 检查

### R3.6 网络同步（Phase 4 起）
- 载具 Transform：`COND_SimulatedOnly` 复制
- GAS 属性：走内置复制机制
- 输入意图（Throttle / Steer 值）走 Server RPC

---

## 四、代码组织

### R4.1 目录结构
```
Source/MTBRacing/
├── Public/  (对外头文件)
└── Private/ (实现)
```
- 严格 Public/Private 分离
- 按子系统分子目录：Core/ Pawn/ Gas/ Race/ Player/
- 子目录名统一 **PascalCase**（首字母大写，其余小写）。缩写词也照此处理：写 `Gas/` 不写 `GAS/`，与 UE 引擎自身目录习惯一致

### R4.2 命名
- 类前缀：`A`(Actor) / `U`(UObject) / `F`(Struct) / `E`(Enum) / `I`(Interface)
- 项目类前缀：**`MTB`**（例：`AMTBGameMode`、`UMTBAttributeSet`）
- 蓝图前缀：`BP_` 蓝图类、`SK_` SkeletalMesh、`SM_` StaticMesh、`IMC_` InputMappingContext、`IA_` InputAction、`GA_` GameplayAbility、`GE_` GameplayEffect

### R4.3 参数调优点
- 需要在 Editor 里调的参数必须 `UPROPERTY(EditAnywhere, BlueprintReadWrite)`
- C++ 默认值给"合理保守值"，最终手感数值保存在 BP 实例
- **不因为 BP 里调好了就回改 C++ 默认值**（除非用户明确要求）

---

## 五、Content 资产规则

### R5.1 目录约定
```
Content/
├── Blueprints/          业务蓝图（BP_ 前缀）
├── Input/               Enhanced Input 资产
├── Levels/              关卡
├── Vehicles/            车辆资产（当前 SportsCar 备用）
└── UI/                  UI 资产（Phase 2/3 加入）
```

### R5.2 资产大小
- 单 `.uasset` 超过 **50MB** 前，先讨论是否需要 Git LFS
- 未使用的备用资产可以保留（当前 Vehicles/SportsCar 保留），但要在 MEMORY.md 里标"备用"

### R5.3 UE 生成物不入库
以下目录 `.gitignore` 掉，不允许入库：
- `Binaries/` `DerivedDataCache/` `Intermediate/` `Saved/` `.vs/`

---

## 六、文档与记忆系统

### R6.1 记忆系统唯一路径
- **只用 `.workbuddy/memory/`**，禁止在其他位置放 memory / FAQ
- 已删除 `.codebuddy/`，如果再出现视为违规

### R6.2 三类文件的分工
| 文件 | 用途 | 写入时机 |
|---|---|---|
| `.workbuddy/memory/YYYY-MM-DD.md` | 每日工作日志（追加式） | 完成一次实质工作就追加 |
| `.workbuddy/memory/MEMORY.md` | 长期项目记忆，跨端恢复用 | 项目结构/技术选型/规约变更时 |
| `docs/DEV_PLAN.md` | 阶段任务分解 + 架构 | 每阶段完成勾选、方向变化时更新 |
| `docs/PROJECT_RULES.md` | 本文件，项目规矩 | 规矩本身发生变化时 |

### R6.3 日志规范
- 文件名：`YYYY-MM-DD.md`
- 追加式，不覆盖当天已有内容
- 内容重心：**决策 + 遗留问题 + 下一步**，不记琐碎过程
- 决策变化必须留原因（"为什么这么改"比"改了什么"更重要）

### R6.4 恢复上下文指令
- 新会话说 **"继续 MTB 项目"** 就等于让 AI：
  1. 读 `docs/PROJECT_RULES.md`（先看规矩）
  2. 读 `.workbuddy/memory/MEMORY.md`
  3. 读 `docs/DEV_PLAN.md`
  4. 读最近 1~2 篇日志

---

## 七、Git 工作流

### R7.1 分支
- 单人开发阶段：**只用 `main` 分支**
- 远期多人协作时再引入 feature 分支

### R7.2 提交切分
- **代码 / 文档 / 资产分开 commit**（如 Phase 1 收尾时的两次提交）
- 每次 commit 只表达一个意图

### R7.3 Commit Message
- 用 Conventional Commits：`feat` / `fix` / `chore` / `refactor` / `docs`
- 中文说明允许（团队内部沟通方便）
- Body 里写"为什么这么改"，不只是"改了什么"

### R7.4 关键节点必推
- 每个 Phase 完成 → 必须 `git push origin main`
- 大重构（如去掉 Chaos Vehicle）→ 立即 commit + push

### R7.5 破坏性操作
- `git reset --hard`、`git push --force`、任何删文件的动作，必须先告知用户并确认
- 从不 `--no-verify` 跳过 hook（除非用户明确要求）

---

## 八、开发环境

### R8.1 多机同步
- 台式机 / 笔记本切换开发，**依赖 Git 拉取代码**，不同步 Binaries / Intermediate
- 换机后必须 `GenerateProjectFiles.bat` 重生 `.sln`（源码版引擎的强制要求）

### R8.2 工具分工
- **写代码**：VS / Rider + CodeBuddy 插件
- **规划讨论**：WorkBuddy（工作区设在项目根目录）
- **不允许**：在 WorkBuddy 里绕过 R1.1~R1.4 直接大段代写

---

## 附录 A：规矩来源索引

| 规矩 | 来源 |
|---|---|
| R1.* AI 协作 | `MEMORY.md` "AI 协作模式" 章节 |
| R2.* 分阶段策略 | `DEV_PLAN.md` "当前阶段策略" + Phase 划分 |
| R3.* 技术选型 | `DEV_PLAN.md` "技术栈选型" + "关键实现注意事项" |
| R4.* 代码组织 | `MEMORY.md` "当前源码结构" + 引擎通行约定 |
| R5.* 资产 | `MEMORY.md` "当前 Content 资产" + 06-09 提交建议 |
| R6.* 文档记忆 | `MEMORY.md` "记忆系统约定" + 08-30 日志 |
| R7.* Git | 06-09 提交切分实践 + 08-30 记忆整合 |
| R8.* 环境 | `MEMORY.md` "开发环境" + "开发工作流" |

## 附录 B：修订记录

- **v1（2026-08-30）**：首次整理，从 DEV_PLAN v2 + MEMORY.md（08-30 版）+ 全部日志中沉淀
- **v1.1（2026-09-06）**：R4.1 子目录命名从 `GAS/` 改为 `Gas/`，并补充 PascalCase 通则。
  起因：Phase 2.2 实际建的是 `Source/MTBRacing/{Public,Private}/Gas/`，与原文档的 `GAS/` 不一致。
  决策：改文档而非改目录 —— `Gas/` 符合 UE 的 PascalCase 习惯，全大写反而是例外。
  同步文件：`docs/PROJECT_RULES.md`、`.workbuddy/rules/mtbracing-rules.md`、`docs/DEV_PLAN.md`
