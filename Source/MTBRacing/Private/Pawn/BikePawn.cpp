// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawn/BikePawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ABikePawn::ABikePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// --- 1. Root 碰撞 Box ---
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetBoxExtent(FVector(100.f, 50.f, 50.f)); // 长2m 宽1m 高1m
	CollisionBox->SetSimulatePhysics(false);
	CollisionBox->SetCollisionProfileName(TEXT("Pawn"));
	SetRootComponent(CollisionBox);

	// --- 2. 可视 Mesh (引擎自带 Cube) ---
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(CollisionBox);
	// 加载引擎内置 Cube
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeFinder.Object);
		// Cube 默认 100x100x100，缩放到 200x100x100 模拟车身比例
		BodyMesh->SetRelativeScale3D(FVector(2.f, 1.f, 1.f));
	}
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 碰撞由 Box 处理

	// --- 3. SpringArm + Camera ---
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(CollisionBox);
	SpringArm->TargetArmLength = 600.f;
	SpringArm->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bDoCollisionTest = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// --- 4. FloatingPawnMovement ---
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	Movement->MaxSpeed = 2000.f;       // 20 m/s
	Movement->Acceleration = 1500.f;   // 加速度
	Movement->Deceleration = 1200.f;   // 减速度（松开油门的自然减速）
}

void ABikePawn::BeginPlay()
{
	Super::BeginPlay();

	// 绑定 Input Mapping Context
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (BikeIMC)
			{
				Subsystem->AddMappingContext(BikeIMC, 0);
			}
		}
	}
}

void ABikePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABikePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (PedalingAction)
		{
			EIC->BindAction(PedalingAction, ETriggerEvent::Triggered, this, &ABikePawn::HandleThrottle);
			EIC->BindAction(PedalingAction, ETriggerEvent::Completed, this, &ABikePawn::HandleThrottle);
		}

		if (SteeringAction)
		{
			EIC->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ABikePawn::HandleSteering);
			EIC->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ABikePawn::HandleSteering);
		}

		if (HandbrakeAction)
		{
			EIC->BindAction(HandbrakeAction, ETriggerEvent::Triggered, this, &ABikePawn::HandleBrake);
			EIC->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ABikePawn::HandleBrake);
		}
	}
}

void ABikePawn::HandleThrottle(const FInputActionValue& Value)
{
	// W/S 或手柄扳机 → 前进/后退
	float Val = Value.Get<float>();

	// AddMovementInput 沿 Pawn 前向（+X）累积
	AddMovementInput(GetActorForwardVector(), Val);
}

void ABikePawn::HandleSteering(const FInputActionValue& Value)
{
	// A/D → 绕 Z 轴旋转 Pawn
	float Val = Value.Get<float>();
	float DeltaYaw = Val * TurnSpeed * GetWorld()->GetDeltaSeconds();
	AddActorWorldRotation(FRotator(0.f, DeltaYaw, 0.f));
}

void ABikePawn::HandleBrake(const FInputActionValue& Value)
{
	// Space → 急停（直接把速度清零）
	bool bPressed = Value.Get<bool>();
	if (bPressed && Movement)
	{
		Movement->StopMovementImmediately();
	}
}
