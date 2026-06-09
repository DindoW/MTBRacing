// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "BikePawn.generated.h"

class UInputAction;
class UInputMappingContext;
class UFloatingPawnMovement;
class UCameraComponent;
class USpringArmComponent;
class UBoxComponent;

/**
 * 最简自行车 Pawn — Cube 方块 + FloatingPawnMovement
 * 用于验证输入和移动系统，后续再替换为正式模型和物理。
 */
UCLASS()
class MTBRACING_API ABikePawn : public APawn
{
	GENERATED_BODY()

public:
	ABikePawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	// Input handlers
	void HandleThrottle(const FInputActionValue& Value);
	void HandleSteering(const FInputActionValue& Value);
	void HandleBrake(const FInputActionValue& Value);

public:
	// ===== Components =====

	/** 根碰撞体 (Box) — 充当车身 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CollisionBox;

	/** 可视化方块 Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BodyMesh;

	/** 弹簧臂 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;

	/** 相机 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	/** 移动组件 — UE 自带最简版 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFloatingPawnMovement* Movement;

	// ===== Input Assets (Blueprint 可配) =====

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* BikeIMC;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PedalingAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* SteeringAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* HandbrakeAction;

	// ===== Tuning =====

	/** 转向速度 (degrees/sec) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tuning")
	float TurnSpeed = 120.f;
};
