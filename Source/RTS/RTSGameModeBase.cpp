// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSGameModeBase.h"
#include "RTSPlayerPawn.h"
#include "RTSPlayerController.h"

ARTSGameModeBase::ARTSGameModeBase()
{
	// Set default player pawn
	DefaultPawnClass = ARTSPlayerPawn::StaticClass();

	// Set default player controller for controlling the pawn
	PlayerControllerClass = ARTSPlayerController::StaticClass();
}