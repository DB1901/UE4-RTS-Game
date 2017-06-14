// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "RTSPlayerMovementComponent.generated.h"

/**
 *
 */
UCLASS()
class RTS_API URTSPlayerMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	URTSPlayerMovementComponent();

	/** Process movement vector and interpolate between frames */
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/** Defines the minimum camera height above ground */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinimumCameraHeight = 1000.0f;

	/** Defines the maximum camera height above ground. May be overriden if terrain is above this */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaximumCameraHeight = 10000.0f;

	/** The lower this value, the more responsive the camera. Should be in range (0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ZoomLag = 0.2f;

	/** The current Z value of the camera position */
	float GetCurrentAbsoluteCameraHeight() const;

	/** Defines the interpolation parameter between min and max camera height */
	float CameraHeightParameter = 0.5f;

	/** Updates the desired camera height, given the parameter t in range [0, 1] */
	float ComputeCameraDesiredHeight() const;

	/** If MinimumSpeed is not used, this multiplies the speed of the camera's movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaximumSpeedMultiplier = 2.0f;

	/** Minimum speed is clamped to this value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinimumSpeed = 100.0f;

	/** 1 = looking straight down, 0 = looking forward. Must be < 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaximumAllowablePitchParameter = 0.75f;

	/** 1 = looking straight down, 0 = looking forward. Must be < 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinimumAllowablePitchParameter = 0.4f;

	/** Sets the maximum movement sway when moving, in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaximumMovementSwayDegrees = 5.0f;

	/** Computes the desired camera angle given its height parameter */
	float ComputeCameraPitchDegrees() const;

	/** Override max speed. The higher the camera, the higher the speed */
	virtual float GetMaxSpeed() const override;

	/** Returns the object's height above ground */
	float FindHeightAboveTerrain() const;

	float FindTerrainHeight() const;

	/** Returns the current height parameter of the camera, as opposed to the target one */
	float GetCurrentCameraHeightParameter() const;

	float MovementSwayLag = 0.1f;

	FVector2D CurrentMovementSway = FVector2D::ZeroVector;
	FVector2D TargetMovementSway = CurrentMovementSway;

	FVector2D ComputeTargetMovementSway(const FVector2D& Delta);
};
