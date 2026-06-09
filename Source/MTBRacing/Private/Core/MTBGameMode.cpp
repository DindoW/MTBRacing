// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/MTBGameMode.h"
#include "Pawn/BikePawn.h"

AMTBGameMode::AMTBGameMode()
{
	DefaultPawnClass = ABikePawn::StaticClass();
}
