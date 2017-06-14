// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
class ARTSPlayerPawn;

#include <functional>
#include "CoreMinimal.h"
#include "RTSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

	ARTSPlayerController();
	
	/** Set up input bindings */
	void SetupInputComponent();

	/** Move forward axis binding. Simply forwards command to Pawn. */
	void MoveForward(float Value);

	/** Move right axis binding. Simply forwards command to Pawn. */
	void MoveRight(float Value);

	/** Zoom down axis binding. Simply forwards command to Pawn. */
	void ZoomDown(float Value);

	/** Retrieves pawn if one exists, and performs an action on it. */
	void DoOnPawn(std::function<void(ARTSPlayerPawn*)> Fn);
};
