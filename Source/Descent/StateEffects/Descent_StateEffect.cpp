// Fill out your copyright notice in the Description page of Project Settings.


#include "Descent_StateEffect.h"
#include "Descent/Character/Descent_HealthComponent.h"
#include "Descent/Interface/Descent_IGameActor.h"
#include "Kismet/GameplayStatics.h"
#include "Descent/FuncLibrary/Types.h"
#include "Net/UnrealNetwork.h"

bool UDescent_StateEffect::InitObject(AActor* Actor, FName BoneHit)
{
	myActor = Actor;
	NameBone = BoneHit;

	IDescent_IGameActor* myInterface = Cast<IDescent_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->Execute_AddEffect(myActor, this);
	}

	return true;
}

void UDescent_StateEffect::DestroyObject()
{
	IDescent_IGameActor* myInterface = Cast<IDescent_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->Execute_RemoveEffect(myActor, this);
	}

	myActor = nullptr;
	if (this && this->IsValidLowLevel())
	{
		this->ConditionalBeginDestroy();
	}
}

bool UDescent_StateEffect_ExecuteOnce::InitObject(AActor* Actor, FName BoneHit)
{
	Super::InitObject(Actor, BoneHit);
	ExecuteOnce();
	return true;
}

void UDescent_StateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}

void UDescent_StateEffect_ExecuteOnce::ExecuteOnce()
{
	if (myActor)
	{
		UDescent_HealthComponent* myHealthComponent = Cast<UDescent_HealthComponent>(myActor->GetComponentByClass(UDescent_HealthComponent::StaticClass()));
		if (myHealthComponent)
		{
			myHealthComponent->ChangeHealthValue_OnServer(Power);
		}
	}

	DestroyObject();
}

bool UDescent_StateEffect_ExecuteTimer::InitObject(AActor* Actor, FName BoneHit)
{
	Super::InitObject(Actor, BoneHit);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UDescent_StateEffect_ExecuteTimer::DestroyObject, Timer, false);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UDescent_StateEffect_ExecuteTimer::Execute, RateTimer, true);
	}
	
	/*if (ParticleEffect)
	{
		FName NameBoneToAttached = BoneHit;
		FVector Location = FVector(0);

		USceneComponent* mySkeletalMesh = Cast<USceneComponent>(myActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		if (mySkeletalMesh)
		{
			ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, mySkeletalMesh, NameBoneToAttached, Location, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
		}
		else
		{
			ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, myActor->GetRootComponent(), NameBoneToAttached, Location, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
		}
	}*/
	
	return true;
}

void UDescent_StateEffect_ExecuteTimer::DestroyObject()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}

	/*ParticleEmitter->DestroyComponent();
	ParticleEmitter = nullptr;*/
	Super::DestroyObject();
}

void UDescent_StateEffect_ExecuteTimer::Execute()
{
	if (myActor)
	{
		UDescent_HealthComponent* myHealthComponent = Cast<UDescent_HealthComponent>(myActor->GetComponentByClass(UDescent_HealthComponent::StaticClass()));
		if (myHealthComponent)
		{
			myHealthComponent->ChangeHealthValue_OnServer(Power);
		}
	}
}

bool UDescent_StateEffect_AreaOfEffect::InitObject(AActor* Actor, FName BoneHit)
{
	Super::InitObject(Actor, BoneHit);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UDescent_StateEffect_AreaOfEffect::DestroyObject, Timer, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UDescent_StateEffect_AreaOfEffect::FindActor, RateTimer, true);


	if (ParticleEffectAoE)
	{
		FName NameBoneToAttached = BoneHit;
		FVector Location = FVector(0);

		USceneComponent* mySkeletalMesh = Cast<USceneComponent>(myActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		if (mySkeletalMesh)
		{
			ParticleEmitterAoE = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, mySkeletalMesh, NameBoneToAttached, Location, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
		}
		else
		{
			ParticleEmitterAoE = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, myActor->GetRootComponent(), NameBoneToAttached, Location, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
		}
	}

	return true;
}

void UDescent_StateEffect_AreaOfEffect::DestroyObject()
{
	/*ParticleEmitterAoE->DestroyComponent();
	ParticleEmitterAoE = nullptr;*/
	Super::DestroyObject();
}

void UDescent_StateEffect_AreaOfEffect::FindActor()
{
	if (myActor)
	{
		FVector ActorLoc = myActor->GetActorLocation();
		FVector ActorForward = myActor->GetActorForwardVector();
		FVector StartEnd = ActorLoc + ActorForward;
		TArray<AActor*> ActorsToIngnore;
		TArray<FHitResult> HitResult;


		bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), StartEnd, StartEnd, RadiusArea,
			UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIngnore,
			EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

		if (Hit)
		{
			int32 i = 0;
			while (i < HitResult.Num())
			{
				EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(HitResult[i]);
				UTypes::AddEffectBySurfaceType(HitResult[i].GetActor(), HitResult[i].BoneName, Effect, mySurfaceType);
				i++;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("TDS_StateEffect_AreaOfEffect - Array empty"));
		}
	}
}

void UDescent_StateEffect::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDescent_StateEffect, NameBone);

}
