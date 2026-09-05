// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
// [知识点] 为什么必须 include AbilitySystemComponent.h：
// 下面 ATTRIBUTE_ACCESSORS 里的 GAMEPLAYATTRIBUTE_VALUE_SETTER 宏展开后会调用
// AbilityComp->SetNumericAttributeBase(...)（见引擎 AttributeSet.h:446）。
// 不 include 就会报"使用了未定义类型 UAbilitySystemComponent"，且错误位置指向宏展开处，很难定位。
#include "AbilitySystemComponent.h"
#include "MTBAttributeSet.generated.h"

// [知识点] ATTRIBUTE_ACCESSORS 引擎【不提供】，必须项目自己定义。
// 引擎 AttributeSet.h:407-425 里那段 ATTRIBUTE_ACCESSORS 只是注释里的"建议写法"，不是真宏。
// 引擎只提供下面这 4 个基础宏（AttributeSet.h:427/434/440/450），要自己组合。
// 网上所有 GAS 教程都直接用 ATTRIBUTE_ACCESSORS 却不说这点，是最常见的首个编译错误来源。
//
// 展开后每个属性白得 4 个函数（以 Stamina 为例）：
//   static FGameplayAttribute GetStaminaAttribute()  给 GE / delegate 用（标识属性"是谁"）
//   float GetStamina() const                        读 CurrentValue
//   void  SetStamina(float)                         走 ASC 改 BaseValue
//   void  InitStamina(float)                        初始化，Base 和 Current 一起设
//
// 注意：每行末尾 '\' 后面不能有任何空格，否则宏定义会断在那里，报一堆莫名其妙的语法错误。
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * MTB 属性集 —— 纯数据声明，不放任何游戏逻辑。
 *
 * [知识点] 判断"某个值该不该做 GameplayAttribute"的唯一标准：
 *   它会不会被 GameplayEffect 修改？
 *     会   → 是 Attribute（能自动复制、自动聚合多来源加成、带 Pre/Post 钩子）
 *     不会 → 别放进来（否则要每帧手动同步 + 白占复制带宽）
 *
 * 所以这里【没有】CurrentSpeed：它是 UFloatingPawnMovement::Velocity 的派生只读值，
 * 没有任何 GE 会改它。HUD 想显示速度直接读 Movement->Velocity.Size() 即可。
 */
UCLASS()
class MTBRACING_API UMTBAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UMTBAttributeSet();

	// [知识点] 属性类型必须是 FGameplayAttributeData，不能用裸 float。
	// 它是【双值结构】（引擎 AttributeSet.h:19-53）：
	//   BaseValue    永久值。Instant / Periodic GE 改这个
	//   CurrentValue 当前生效值。Duration / Infinite GE 的加成叠在这上面
	//
	// 这个设计解决了"多来源加成叠加"的噩梦。举例（MaxSpeed）：
	//   初始              Base=2000  Current=2000
	//   Boost 生效 x1.5   Base=2000  Current=3000
	//   顺风也生效 x1.2   Base=2000  Current=3600
	//   Boost 结束        Base=2000  Current=2400  ← 自动只剩顺风，不写一行"恢复原值"代码
	// 如果自己写 MaxSpeed = 3000 再 MaxSpeed = 2000，两个来源同时生效时必算错。
	//
	// [知识点] 为什么是 BlueprintReadOnly 而不是 ReadWrite：
	// 属性只能被 GameplayEffect 修改。给 BP 开直接写的口子 = 绕过 GAS 的复制和预测系统。
	//
	// [知识点] ReplicatedUsing = OnRep_Xxx：
	// Phase 2 是单机，这个暂时不生效，但现在补上比 Phase 4 回来补省事。
	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UMTBAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UMTBAttributeSet, MaxStamina)

	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_StaminaRegenRate)
	FGameplayAttributeData StaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UMTBAttributeSet, StaminaRegenRate)

	// [知识点] MaxSpeed 是 Attribute，但它【不会自动作用于移动组件】。
	// AttributeSet 只是数据袋子，UFloatingPawnMovement 根本不知道 GAS 存在。
	// 2.3 必须在 BikePawn 里搭桥：
	//   ASC->GetGameplayAttributeValueChangeDelegate(GetMaxSpeedAttribute())
	//      .AddUObject(this, &ABikePawn::OnMaxSpeedChanged);
	//   // 回调里：Movement->MaxSpeed = Data.NewValue;
	// 不搭桥的症状：Boost 激活了、属性面板 MaxSpeed 确实变 3000 了、但车速一点没变。
	// 这是 GAS 新手最经典的困惑。
	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MaxSpeed)
	FGameplayAttributeData MaxSpeed;
	ATTRIBUTE_ACCESSORS(UMTBAttributeSet, MaxSpeed)

	// [知识点] UFUNCTION() 是【强制】的，不是可选。
	// 少了它反射系统找不到这个函数，复制时静默失效 —— 不报编译错误，这是最坑的地方。
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	UFUNCTION()
	void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate);

	UFUNCTION()
	void OnRep_MaxSpeed(const FGameplayAttributeData& OldMaxSpeed);



	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// [知识点] AttributeSet 的四个钩子，分工严格（引擎源码注释原文可查）：
	//
	//  钩子                       时机                触发者                    用途
	//  ------------------------  ------------------  ------------------------  ----------------
	//  PreAttributeBaseChange    BaseValue 改变前     Instant / Periodic GE     钳制 BaseValue
	//  PreAttributeChange        CurrentValue 改变前  所有 GE + 移除 + 叠层等    钳制 CurrentValue
	//  PostGameplayEffectExecute Instant GE 执行后    仅 Instant / Periodic     业务反应逻辑
	//  PostAttributeChange       任何修改后           所有                      通知
	//
	// 源码注释明确：Pre* 两个钩子"只做钳制，不做业务逻辑"
	//   AttributeSet.h:209-215 — "meant to enforce things like Health = Clamp(Health, 0, MaxHealth)
	//                             and NOT things like trigger this extra thing if damage is applied"
	//   AttributeSet.h:222-226 — "should NOT invoke gameplay related events or callbacks"
	//
	// [重要推论] PostGameplayEffectExecute 的注释（AttributeSet.h:198-201）说：
	//   "It is not called during an application of a GameplayEffect,
	//    such as a 5 second +10 movement speed buff"
	// 即：Infinite GE 改 CurrentValue 时【不会】触发它。
	// 所以 GA_Boost 改 MaxSpeed 后在这里等不到通知 —— MaxSpeed→Movement 的桥
	// 必须用 GetGameplayAttributeValueChangeDelegate（那个对两种情况都触发）。
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// TODO(2.5 前必须补): 缺 PreAttributeBaseChange 覆写 —— 见 .cpp 里的详细说明。
	//   GE_StaminaRegen 是 Periodic GE，改的是 BaseValue，走的是 PreAttributeBaseChange，
	//   而当前只覆写了 PreAttributeChange（只管 CurrentValue）。
	//   后果：Stamina 的 BaseValue 会无上限增长（100→110→120→...）。
	// 签名（注意结尾的 const，漏了会变成新函数而不是覆写）：
	//   virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

};
