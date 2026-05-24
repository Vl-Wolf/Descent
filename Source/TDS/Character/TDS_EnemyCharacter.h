// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDS/Interface/TDS_IGameActor.h"
#include "TDS/Character/TDS_EffectComponent.h"
#include "TDS_EnemyCharacter.generated.h"


UCLASS()
class TDS_API ATDS_EnemyCharacter : public ACharacter, public ITDS_IGameActor
{
	GENERATED_BODY()

public:

	ATDS_EnemyCharacter();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
	UTDS_EffectComponent* EffectComponent;

protected:

	virtual void BeginPlay() override;

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:	

	virtual void Tick(float DeltaTime) override;
	
	virtual void RemoveEffect_Implementation(UTDS_StateEffect* RemoveEffect) override;

	virtual void AddEffect_Implementation(UTDS_StateEffect* NewEffect) override;
	
	virtual TArray<UTDS_StateEffect*> GetAllCurrentEffects_Implementation() override;
	
};
