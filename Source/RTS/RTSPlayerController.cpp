// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSPlayerController.h"
#include "RTSPlayerPawn.h"

ARTSPlayerController::ARTSPlayerController()
{

}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &ARTSPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARTSPlayerController::MoveRight);
	InputComponent->BindAxis("ZoomDown", this, &ARTSPlayerController::ZoomDown);
}

void ARTSPlayerController::MoveForward(float Value)
{
	DoOnPawn([&Value](ARTSPlayerPawn* pawn) { pawn->MoveForward(Value); });
}

void ARTSPlayerController::MoveRight(float Value)
{
	DoOnPawn([&Value](ARTSPlayerPawn* pawn) { pawn->MoveRight(Value); });
}

void ARTSPlayerController::ZoomDown(float Value)
{
	DoOnPawn([&Value](ARTSPlayerPawn* pawn) { pawn->ZoomDown(Value); });
}

void ARTSPlayerController::DoOnPawn(std::function<void(ARTSPlayerPawn*)> Fn)
{
	auto pawn = Cast<ARTSPlayerPawn>(GetPawn());
	if (pawn)
	{
		Fn(pawn);
	}
}