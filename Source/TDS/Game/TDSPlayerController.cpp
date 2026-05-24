// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSPlayerController.h"

#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ATDSPlayerController::ATDSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ATDSPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (!PlayerCharacter || !PlayerCharacter->GetIsAlive())
		return;
	
	if (CursorDecal && IsLocalPlayerController())
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		CursorDecal->SetWorldLocation(Hit.Location);
		CursorDecal->SetWorldRotation(Hit.ImpactNormal.Rotation());
	}
	
	PlayerCharacter->AddMovementInput(FVector::ForwardVector, AxisX);
	PlayerCharacter->AddMovementInput(FVector::RightVector, AxisY);
	
	EMovementState CurrentMovementState = PlayerCharacter->GetMovementState();
	if (CurrentMovementState == EMovementState::Sprint_State)
	{
		FVector Direction = FVector(AxisX, AxisY, 0.0f);
		if (!Direction.IsNearlyZero())
		{
			float Yaw = Direction.ToOrientationRotator().Yaw;
			
			PlayerCharacter->SetActorRotation(FQuat(FRotator(0.0f, Yaw, 0.0f)));
			PlayerCharacter->SetActorRotationByYaw_OnServer(Yaw);
		}
	}
	else
	{
		FHitResult HitResult;
		GetHitResultUnderCursor(ECC_GameTraceChannel1, true, HitResult);
		float Yaw = UKismetMathLibrary::FindLookAtRotation(PlayerCharacter->GetActorLocation(), HitResult.Location).Yaw;

		PlayerCharacter->SetActorRotation(FQuat(FRotator(0.0f, Yaw, 0.0f)));
		PlayerCharacter->SetActorRotationByYaw_OnServer(Yaw);
		
		PlayerCharacter->SetAimLocation(HitResult.Location);
	}
}

void ATDSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAxis(TEXT("MoveForward"), this, &ATDSPlayerController::InputAxisX);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &ATDSPlayerController::InputAxisY);

	InputComponent->BindAction(TEXT("ChangeToSprint"), EInputEvent::IE_Pressed, this, &ATDSPlayerController::InputSprintPressed);
	InputComponent->BindAction(TEXT("ChangeToSprint"), EInputEvent::IE_Released, this, &ATDSPlayerController::InputSprintReleased);
	
	InputComponent->BindAction(TEXT("ChangeToWalk"), EInputEvent::IE_Pressed, this, &ATDSPlayerController::InputWalkPressed);
	InputComponent->BindAction(TEXT("ChangeToWalk"), EInputEvent::IE_Released, this, &ATDSPlayerController::InputWalkReleased);
	
	InputComponent->BindAction(TEXT("AimEvent"), EInputEvent::IE_Pressed, this, &ATDSPlayerController::InputAimPressed);
	InputComponent->BindAction(TEXT("AimEvent"), EInputEvent::IE_Released, this, &ATDSPlayerController::InputAimReleased);


	InputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &ATDSPlayerController::InputAttackPressed);
	InputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Released, this, &ATDSPlayerController::InputAttackReleased);
	
	InputComponent->BindAction(TEXT("ReloadEvent"), EInputEvent::IE_Released, this, &ATDSPlayerController::InputReload);

	InputComponent->BindAction(TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &ATDSPlayerController::InputSwitchNextWeapon);
	InputComponent->BindAction(TEXT("SwitchPreviousWeapon"), EInputEvent::IE_Pressed, this, &ATDSPlayerController::InputSwitchPreviousWeapon);

	InputComponent->BindAction(TEXT("AbilityAction"), EInputEvent::IE_Pressed, this, &ATDSPlayerController::InputAbility);

	InputComponent->BindAction(TEXT("DropCurrentWeapon"), EInputEvent::IE_Pressed, this, &ATDSPlayerController::InputDropWeapon);
	
	
	InputComponent->BindKey(EKeys::One, IE_Pressed, this, &ATDSPlayerController::InputSwitchToSlot<0>);
	InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ATDSPlayerController::InputSwitchToSlot<1>);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ATDSPlayerController::InputSwitchToSlot<2>);
	InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &ATDSPlayerController::InputSwitchToSlot<3>);

}

void ATDSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (GetWorld() && GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		if (CursorMaterial && IsLocalPlayerController())
		{			
			CursorDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
		}
	}
}

void ATDSPlayerController::OnUnPossess()
{
	if (CursorDecal)
		CursorDecal->SetVisibility(false);
	
	Super::OnUnPossess();
}

void ATDSPlayerController::InputAxisX(float Value)
{
	AxisX = Value;
}

void ATDSPlayerController::InputAxisY(float Value)
{
	AxisY = Value;
}

void ATDSPlayerController::InputAttackPressed()
{
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->SetFiring(true);
}

void ATDSPlayerController::InputAttackReleased()
{
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->SetFiring(false);
}

void ATDSPlayerController::InputWalkPressed()
{
	bIsWalkEnabled = true;
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->SetMovementState_OnServer(DetermineMovementState());
}

void ATDSPlayerController::InputWalkReleased()
{
	bIsWalkEnabled = false;
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->SetMovementState_OnServer(DetermineMovementState());
}

void ATDSPlayerController::InputSprintPressed()
{
	bIsSprintEnabled = true;
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->SetMovementState_OnServer(DetermineMovementState());
}

void ATDSPlayerController::InputSprintReleased()
{
	bIsSprintEnabled = false;
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->SetMovementState_OnServer(DetermineMovementState());
}

void ATDSPlayerController::InputAimPressed()
{
	bIsAimEnabled = true;
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->SetMovementState_OnServer(DetermineMovementState());
}

void ATDSPlayerController::InputAimReleased()
{
	bIsAimEnabled = false;
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->SetMovementState_OnServer(DetermineMovementState());
}

void ATDSPlayerController::InputReload()
{
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->RequestReload();
}

void ATDSPlayerController::InputAbility()
{
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->RequestAbility();
}

void ATDSPlayerController::InputDropWeapon()
{
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->RequestDropWeapon();
}

void ATDSPlayerController::InputSwitchNextWeapon()
{
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->RequestSwitchNextWeapon();
}

void ATDSPlayerController::InputSwitchPreviousWeapon()
{
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetPawn());
	if (PlayerCharacter)
		PlayerCharacter->RequestSwitchPreviousWeapon();
}

EMovementState ATDSPlayerController::DetermineMovementState() const
{
	if (bIsSprintEnabled)
		return EMovementState::Sprint_State;
	
	if (bIsWalkEnabled && bIsAimEnabled)
		return EMovementState::AimWalk_State;
	
	if (bIsWalkEnabled)
		return EMovementState::Walk_State;
	
	if (bIsAimEnabled)
		return EMovementState::Aim_State;
	
	return EMovementState::Run_State;
}
