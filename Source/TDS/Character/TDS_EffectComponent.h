// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDS/StateEffects/TDS_StateEffect.h"
#include "TDS_EffectComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TDS_API UTDS_EffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTDS_EffectComponent();

	UFUNCTION()
	void ApplyEffect(UTDS_StateEffect* NewEffect);
	
	UFUNCTION()
	void RemoveEffect(UTDS_StateEffect* NewEffect);
	
	UFUNCTION()
	TArray<UTDS_StateEffect*> GetActiveEffects() const;
	
	void SetAttachTarget(USceneComponent* InTarget, FVector InOffset = FVector::ZeroVector);
	
	bool ReplicateEffectSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);
	
protected:
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	TArray<UTDS_StateEffect*> ActiveEffects;
	
	UPROPERTY(ReplicatedUsing = EffectAdded_OnRep)
	UTDS_StateEffect* LastAddedEffect = nullptr;
	
	UPROPERTY(ReplicatedUsing = EffectRemoved_OnRep)
	UTDS_StateEffect* LastRemovedEffect = nullptr;
	
	UFUNCTION()
	void EffectAdded_OnRep();
	
	UFUNCTION()
	void EffectRemoved_OnRep();
	
	void ToggleEffectVisuals(UTDS_StateEffect* Effect, bool bIsAdd);
	
	UPROPERTY()
	TArray<UParticleSystemComponent*> ParticleSystemComponents;
	
	UPROPERTY()
	USceneComponent* AttachTarget = nullptr;
	
	FVector AttachOffset = FVector::ZeroVector;
};
