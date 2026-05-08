// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "BikePawn.generated.h"

/**
 * 
 */
UCLASS()
class MTBRACING_API ABikePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()
	
public:
	ABikePawn();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;
	
};
