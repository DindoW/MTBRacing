// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/BikeRearWheel.h"

UBikeRearWheel::UBikeRearWheel()
{
	WheelRadius = 35.0f;
	WheelWidth = 6.0f;
	MaxSteerAngle = 0.0f;      // 后轮不转向
	AxleType = EAxleType::Rear;
	MaxHandBrakeTorque = 3000.0f;  // 后刹
}
