// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDS/Character/TDS_EffectComponent.h"
#include "TDS/StateEffects/TDS_StateEffect.h"
#include "TDS/Interface/TDS_IGameActor.h"
#include "TDS_EnvironmentStructure.generated.h"

UCLASS()
class TDS_API ATDS_EnvironmentStructure : public AActor, public ITDS_IGameActor
{
	GENERATED_BODY()
	
public:	

	ATDS_EnvironmentStructure();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
	UTDS_EffectComponent* EffectComponent;

protected:

	virtual void BeginPlay() override;
	
	bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:	

	virtual void Tick(float DeltaTime) override;

	EPhysicalSurface GetSurfaceType() override;

	virtual TArray<UTDS_StateEffect*> GetAllCurrentEffects_Implementation() override;

	virtual void RemoveEffect_Implementation(UTDS_StateEffect* RemoveEffect) override;

	virtual void AddEffect_Implementation(UTDS_StateEffect* NewEffect) override;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	FVector OffsetEffect = FVector::ZeroVector;
};
