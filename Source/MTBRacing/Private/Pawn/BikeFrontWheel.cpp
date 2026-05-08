// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/BikeFrontWheel.h"



UBikeFrontWheel::UBikeFrontWheel()
{
	WheelRadius = 35.0f;       // 山地车轮径约 35cm（26寸轮）
	WheelWidth = 6.0f;         // 轮胎宽度
	MaxSteerAngle = 40.0f;     // 最大转向角
	AxleType = EAxleType::Front;
}
