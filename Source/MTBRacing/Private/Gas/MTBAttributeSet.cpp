// Fill out your copyright notice in the Description page of Project Settings.


#include "Gas/MTBAttributeSet.h"
// [知识点] DOREPLIFETIME_CONDITION_NOTIFY 宏定义在这里，漏了会报"找不到标识符"
#include "Net/UnrealNetwork.h"

UMTBAttributeSet::UMTBAttributeSet()
{
	// [知识点] 这里的 `Stamina = 100.f` 能工作，是因为 FGameplayAttributeData 有一个
	// 非 explicit 的转换构造函数 FGameplayAttributeData(float)，它把 BaseValue 和
	// CurrentValue 都设成 100（引擎 AttributeSet.h:27-30）。
	// 所以效果等价于 InitStamina(100.f)。两种写法都对。
	//
	// 但更推荐 InitXxx() 系列，原因：
	//   1. 意图明确 —— 一眼看出"这是初始化，Base 和 Current 都设"
	//   2. 不依赖隐式转换 —— 如果哪天 FGameplayAttributeData 把构造函数标成 explicit，
	//      赋值写法会突然编译失败，InitXxx 不会
	//   3. 跟 SetXxx() 形成对比 —— SetXxx 走 ASC 只改 BaseValue，语义完全不同
	Stamina = 100.f;

	// TODO(小瑕疵): `100.` 是 double 字面量，少了 f 后缀。
	// 功能上没问题（会隐式窄化成 float，值仍是 100），但跟其他三行不一致，
	// 且在更严格的编译设置下可能触发 C4305 窄化警告。建议改成 100.f。
	MaxStamina = 100.;

	StaminaRegenRate = 10.f;

	// [知识点] 这个 2000 和 BikePawn 构造函数里的 Movement->MaxSpeed = 2000.f 是
	// 【同一个数字出现在两处】。Phase 2 暂时接受这个重复。
	// 将来可以让 BikePawn 从 Attribute 读初值，但那涉及初始化时序问题
	// （ASC / AttributeSet 什么时候才准备好），不在 Phase 2 折腾。
	//
	// 注意手感基线：BP_CubeBike 实例里 MaxSpeed 也是 2000（跟 C++ 默认一致），
	// 但 Acceleration/Deceleration/TurnSpeed 在 BP 里被改成了 1800/1500/140。
	// 按 R4 约定，C++ 默认值不因 BP 调参而回改。
	MaxSpeed = 2000.f;
}

// [知识点] 这四个 OnRep_ 函数体里【只能】写这一行宏，千万不要自己写赋值逻辑。
//
// 宏展开后（引擎 AttributeSet.h:401-405）实际调的是：
//   GetOwningAbilitySystemComponentChecked()->SetBaseAttributeValueFromReplication(
//       FGameplayAttribute(ThisProperty), PropertyName, OldValue);
//
// 关键在于它把 OldValue 一起传给了 ASC —— 这是【客户端预测回滚】的核心。
// 客户端本地预测改了属性，服务器把权威值复制回来时，ASC 需要知道旧值才能算出
// 预测偏差并做修正。
//
// 如果自己写 `Stamina = NewValue`：单机完全看不出问题，联机时属性会抖动/回跳，
// 而且这种 bug 极难排查（表现随延迟变化）。
void UMTBAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMTBAttributeSet, Stamina, OldStamina);
}

void UMTBAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMTBAttributeSet, MaxStamina, OldMaxStamina);
}

void UMTBAttributeSet::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMTBAttributeSet, StaminaRegenRate, OldStaminaRegenRate);
}

void UMTBAttributeSet::OnRep_MaxSpeed(const FGameplayAttributeData& OldMaxSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMTBAttributeSet, MaxSpeed, OldMaxSpeed);
}

void UMTBAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// [知识点] 为什么用 REPNOTIFY_Always 而不是默认的 REPNOTIFY_OnChanged：
	// GAS 的预测系统要求【每次复制都触发 RepNotify】，即使值看起来没变。
	// 因为客户端可能预测出了一个恰好相同的值，但 ASC 仍需要走一遍回滚校验流程。
	// 用 OnChanged 会导致预测状态机漏帧 —— 又是一个单机看不出、联机才炸的坑。
	//
	// [知识点] COND_None = 复制给所有人。Phase 4 上网络后，
	// 某些属性（比如别人看不见的内部数值）可以改成 COND_OwnerOnly 省带宽。
	DOREPLIFETIME_CONDITION_NOTIFY(UMTBAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMTBAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMTBAttributeSet, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMTBAttributeSet, MaxSpeed, COND_None, REPNOTIFY_Always);
}

void UMTBAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// TODO(建议补): 这里缺 Super::PreAttributeChange(Attribute, NewValue);
	// UE 5.4 的基类实现是空的 { }（AttributeSet.h:215），所以现在没有功能 bug。
	// 但这是【潜在坑】：
	//   - 引擎将来版本可能在基类加逻辑
	//   - 你以后若插入一个中间基类（比如 UMTBBaseAttributeSet），会静默跳过它的钳制
	// 覆写虚函数时先调 Super:: 是应该保持的习惯。

	// [知识点] NewValue 是【引用】，直接改它就完成钳制 —— 不需要 return 什么。
	// 源码流程（AttributeSet.cpp:99-102）：
	//   OldValue = DataPtr->GetCurrentValue();
	//   Dest->PreAttributeChange(*this, NewValue);   ← 你在这里改 NewValue
	//   DataPtr->SetCurrentValue(NewValue);          ← 引擎用改后的值
	//   Dest->PostAttributeChange(*this, OldValue, NewValue);
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}

	// TODO(建议补): MaxSpeed 没有钳制。
	// 至少防负值：if (Attribute == GetMaxSpeedAttribute()) NewValue = FMath::Max(NewValue, 0.f);
	// 否则将来某个配错的 GE（比如乘了负系数）能把 MaxSpeed 变成负数，
	// 车会往后飘，而且这种问题在属性面板上不容易一眼看出来。

	// TODO(建议补): MaxStamina 变小时 Stamina 没有跟着收缩。
	// 场景：MaxStamina 从 100 掉到 50，而 Stamina 当前是 100 → Stamina 仍是 100（超上限）。
	// GAS 的经典问题，标准解法是在 PostAttributeChange 里按比例调整 Stamina。
	// Phase 2 里 MaxStamina 不会变，所以不急，但要知道这个洞在。
}

// ============================================================================
// TODO(2.5 前必须补): PreAttributeBaseChange —— 这是个会真爆的问题，不是洁癖
// ============================================================================
//
// 【问题】上面的 PreAttributeChange 只钳制 CurrentValue，BaseValue 完全没人管。
//
// 已从引擎源码确认两条独立路径：
//
//   CurrentValue 路径（AttributeSet.cpp:99-102）
//     PreAttributeChange(NewValue)  →  DataPtr->SetCurrentValue(NewValue)
//                                      ↑ 只碰 CurrentValue
//
//   BaseValue 路径（GameplayEffect.cpp:3720-3732）
//     PreAttributeBaseChange(NewBaseValue)  →  DataPtr->SetBaseValue(NewBaseValue)
//     ↑ 当前【没有覆写】，等于不做任何钳制
//
// 【后果】GE_StaminaRegen（2.5 要做）是 Infinite + Period=1.0 的 GE。
// Periodic GE 每次触发都按 Instant 处理，改的是 BaseValue。于是：
//
//   t=0s   Base=100  Current=100
//   t=1s   Base=110  Current=100 ← GetStamina() 被 PreAttributeChange 钳到 100，看着正常
//   t=2s   Base=120  Current=100
//   ...
//   t=60s  Base=700  Current=100 ← BaseValue 已经飙到 700
//
// 表面上 HUD 显示 100/100 一切正常。但一旦 Boost 开始扣耐力，
// 你得先把这 600 点虚高的 BaseValue 消耗完才会看到耐力条动 ——
// 表现为"Boost 怎么用都不掉耐力"，然后到某个点突然开始暴跌。
// 这种 bug 从现象根本反推不到根因，是 GAS 最典型的隐性缺陷之一。
//
// 【修法】覆写 PreAttributeBaseChange，做同样的钳制。注意两点：
//   1. 签名结尾有 const（AttributeSet.h:226），漏了 const 就不是覆写而是新增函数，
//      编译能过但永远不会被调用 —— 又一个静默失效
//   2. 因为是 const 函数，里面只能调 const 方法（GetMaxStamina() 是 const，OK）
//
// tranek 文档 4.4.3 "Clamping Attributes" 专门讲这个，建议对照读一遍：
//   https://github.com/tranek/GASDocumentation#concepts-as-attributes
