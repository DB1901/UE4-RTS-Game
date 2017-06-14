// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RTSPlayerMovementComponent.h"
#include "RTSPlayerPawn.generated.h"

UCLASS()
class RTS_API ARTSPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARTSPlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	URTSPlayerMovementComponent* MovementComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Notify of desire to move forward by amount. */
	void MoveForward(float Value);

	/** Notify of desire to move right by amount. */
	void MoveRight(float Value);

	/** Notify of desire to zoom down. */
	void ZoomDown(float Value);

	/** Returns the height of the camera above ground */
	float FindHeightAboveGround() const;
};
