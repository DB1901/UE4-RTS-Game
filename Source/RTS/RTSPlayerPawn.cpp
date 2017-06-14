// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSPlayerPawn.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"


// Sets default values
ARTSPlayerPawn::ARTSPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create root component (sphere with collision)
	auto* SphereComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	RootComponent = SphereComponent;

	// Create custom movement component
	MovementComponent = CreateDefaultSubobject<URTSPlayerMovementComponent>(TEXT("RTSMovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void ARTSPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARTSPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARTSPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARTSPlayerPawn::MoveForward(float Value)
{
	AddMovementInput(FVector(1, 0, 0), Value);
}

void ARTSPlayerPawn::MoveRight(float Value)
{
	AddMovementInput(FVector(0, 1, 0), Value);
}

void ARTSPlayerPawn::ZoomDown(float Value)
{
	if (Value == 0)
		return;
	AddMovementInput(FVector(0, 0, -1), Value);
}

float ARTSPlayerPawn::FindHeightAboveGround() const
{
	return MovementComponent->FindHeightAboveTerrain();
}