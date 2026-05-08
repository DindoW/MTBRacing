// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/BikePawn.h"
#include "Pawn/BikeFrontWheel.h"
#include "Pawn/BikeRearWheel.h"
#include "ChaosWheeledVehicleMovementComponent.h"


ABikePawn::ABikePawn()
{
	UChaosWheeledVehicleMovementComponent* VehicleMovement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
	if (VehicleMovement)
	{
		// 配置车轮数组 — 自行车 2 轮
		VehicleMovement->WheelSetups.SetNum(2);

		// 前轮
		VehicleMovement->WheelSetups[0].WheelClass = UBikeFrontWheel::StaticClass();
		VehicleMovement->WheelSetups[0].BoneName = FName("PhysWheel_F");

		// 后轮
		VehicleMovement->WheelSetups[1].WheelClass = UBikeRearWheel::StaticClass();
		VehicleMovement->WheelSetups[1].BoneName = FName("PhysWheel_R");
	}
}

void ABikePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABikePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
