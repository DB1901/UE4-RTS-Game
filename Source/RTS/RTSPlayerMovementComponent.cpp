// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSPlayerMovementComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Components/PrimitiveComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"

URTSPlayerMovementComponent::URTSPlayerMovementComponent()
{
}

void URTSPlayerMovementComponent::TickComponent(
	float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Reset rotation to default (maintaing yaw) so that movement calculations would not be influences
	// By things like camera roll
	FRotator rotation = FRotator::ZeroRotator;
	UpdatedComponent->SetWorldRotation(rotation);

	// Make sure that everything is still valid, and that we are allowed to move.
	if (!UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	// Fetch vectors
	auto InputVector = ConsumeInputVector();
	auto MovementVector = FVector2D(InputVector.X, InputVector.Y);
	auto ZoomAmount = InputVector.Z;
	auto FinalMovement = FVector::ZeroVector;

	// Move, if needed
	FVector delta;
	if (!MovementVector.IsNearlyZero())
	{
		// Compute direction
		delta = UpdatedComponent->GetForwardVector() * MovementVector.X + UpdatedComponent->GetRightVector() * MovementVector.Y;
		delta.Z = 0;

		if (!delta.IsNearlyZero())
		{
			delta.Normalize();
			// Compute speed
			delta = DeltaTime * GetMaxSpeed() * delta;
			// Assign
			FinalMovement.X = delta.X;
			FinalMovement.Y = delta.Y;
		}
	}

	// Change height parameter, if needed
	if (ZoomAmount != 0)
	{
		CameraHeightParameter += ((CameraHeightParameter / 2.0f) + 0.5f) * ZoomAmount * DeltaTime;
		CameraHeightParameter = FMath::Clamp(CameraHeightParameter, 0.0f, 1.0f);
	}

	// Recalculate height
	{
		// Desired height towards which we should interpolate, computed from height parameter.
		float DesiredHeight = ComputeCameraDesiredHeight();

		// Current height we should be at if possible
		float UnclampedCurrentTargetHeight = FMath::Lerp(GetCurrentAbsoluteCameraHeight(), DesiredHeight, DeltaTime / ZoomLag);

		// Height from terrain. We should glide up if too low!
		float TerrainHeight = FindTerrainHeight();

		// This is the real minimum height to avoid going through terrain
		float EffectiveMinimumHeight = TerrainHeight + MinimumCameraHeight;

		// We should still glide above terrain even if it's above height limit
		float EffectiveMaximumHeight = FMath::Max(EffectiveMinimumHeight, MaximumCameraHeight);

		// Set current height to current desired height clamped between min and max
		float NewHeight = FMath::Clamp(UnclampedCurrentTargetHeight, EffectiveMinimumHeight, EffectiveMaximumHeight);

		// Move down to z=0, and then up to height
		FinalMovement.Z = NewHeight - UpdatedComponent->GetComponentLocation().Z;
	}


	// Compute camera angles
	float DesiredPitch = ComputeCameraPitchDegrees();
	rotation.Pitch = DesiredPitch;
	if (!MovementVector.IsNearlyZero())
	{
		TargetMovementSway = ComputeTargetMovementSway(MovementVector);
	}
	else
	{
		TargetMovementSway = FVector2D::ZeroVector;
	}

	CurrentMovementSway.X = FMath::Lerp(CurrentMovementSway.X, TargetMovementSway.X, DeltaTime / MovementSwayLag);
	CurrentMovementSway.Y = FMath::Lerp(CurrentMovementSway.Y, TargetMovementSway.Y, DeltaTime / MovementSwayLag);

	// It's safe to create Y-sway by changing the pitch
	rotation.Pitch += CurrentMovementSway.Y;
	
	// However, to roll left/right we need the camera up vector.
	auto UpVector = rotation.RotateVector(FVector(0, 0, 1));
	auto XSwayQuaternion = FQuat(UpVector, FMath::DegreesToRadians(CurrentMovementSway.X));
	rotation = FRotator(XSwayQuaternion * FQuat(rotation));
	
	// Update position/pitch
	MoveUpdatedComponent(FinalMovement, rotation, true);

}

float URTSPlayerMovementComponent::GetCurrentAbsoluteCameraHeight() const
{
	return UpdatedComponent->GetComponentLocation().Z;
}

float URTSPlayerMovementComponent::ComputeCameraDesiredHeight() const
{
	return MinimumCameraHeight + (MaximumCameraHeight - MinimumCameraHeight) * CameraHeightParameter;
}

float URTSPlayerMovementComponent::ComputeCameraPitchDegrees() const
{
	// Height = 0 -> MinAllowablePitch
	// Height = 1 -> MaxallowablePitch
	float HeightParameter = GetCurrentCameraHeightParameter();
	// The higher this value, the less 'horizontal' the angle
	float NonLinearityParameter = 3.0f;
	float NonLinearHeightParameter = powf(HeightParameter, 1.0f/NonLinearityParameter);
	float Pitch = FMath::Lerp(MinimumAllowablePitchParameter, MaximumAllowablePitchParameter, NonLinearHeightParameter);
	return -Pitch * 90;
}

float URTSPlayerMovementComponent::GetMaxSpeed() const
{
	auto* CameraComponent = Cast<UCameraComponent>(UpdatedComponent);
	if (!CameraComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not calculate ideal camera speed: CameraComponent retrieval failed."));
		return MinimumSpeed;
	}

	// A point at 0,0 should move from left to right of screen in X milliseconds regardless of how high the camera is.
	float Theta = CameraComponent->FieldOfView / 2.0f;
	return FMath::Max(MaximumSpeedMultiplier * sinf(Theta) * GetCurrentAbsoluteCameraHeight(), MinimumSpeed);
}

float URTSPlayerMovementComponent::FindHeightAboveTerrain() const
{
	auto world = GetWorld();
	if (!world)
		return UpdatedComponent->GetComponentLocation().Z;;

	FHitResult Hit;
	FVector Origin = UpdatedComponent->GetComponentLocation();
	bool bHitFound = world->LineTraceSingleByChannel(Hit, Origin, FVector(Origin.X, Origin.Y, 0), ECC_WorldStatic);
	
	if (bHitFound)
	{
		return Hit.Distance;
	}

	// No collision...
	return UpdatedComponent->GetComponentLocation().Z;
}

float URTSPlayerMovementComponent::FindTerrainHeight() const
{
	float HeightAboveTerrain = FindHeightAboveTerrain();
	float CameraHeight = GetCurrentAbsoluteCameraHeight();
	return CameraHeight - HeightAboveTerrain;
}

float URTSPlayerMovementComponent::GetCurrentCameraHeightParameter() const
{
	return FMath::Clamp((GetCurrentAbsoluteCameraHeight() - MinimumCameraHeight)
					  / (MaximumCameraHeight - MinimumCameraHeight), 0.0f	, 1.0f);
}

FVector2D URTSPlayerMovementComponent::ComputeTargetMovementSway(const FVector2D& Delta)
{
	auto Normalised = Delta;
	Normalised.Normalize();
	Normalised *= MaximumMovementSwayDegrees;
	// Need to swap x/y, as x=forward in input, but we want it to represent horontal sway.
	return -FVector2D(Normalised.Y, Normalised.X);
}
