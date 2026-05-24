// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_EnemyCharacter.h"

ATDS_EnemyCharacter::ATDS_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	EffectComponent = CreateDefaultSubobject<UTDS_EffectComponent>(TEXT("EffectComponent"));
}

void ATDS_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	EffectComponent->SetAttachTarget(GetMesh());
}

void ATDS_EnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATDS_EnemyCharacter::RemoveEffect_Implementation(UTDS_StateEffect* RemoveEffect)
{
	EffectComponent->RemoveEffect(RemoveEffect);
}

void ATDS_EnemyCharacter::AddEffect_Implementation(UTDS_StateEffect* NewEffect)
{
	EffectComponent->ApplyEffect(NewEffect);
}

TArray<UTDS_StateEffect*> ATDS_EnemyCharacter::GetAllCurrentEffects_Implementation()
{
	return EffectComponent->GetActiveEffects();
}

bool ATDS_EnemyCharacter::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool Wrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	Wrote |= EffectComponent->ReplicateEffectSubobjects(Channel, Bunch, RepFlags);
	return Wrote;
}

void ATDS_EnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


