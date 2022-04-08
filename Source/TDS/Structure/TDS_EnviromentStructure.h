// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDS/StateEffects/TDS_StateEffect.h"
#include "TDS/Interface/TDS_IGameActor.h"
#include "TDS_EnviromentStructure.generated.h"

UCLASS()
class TDS_API ATDS_EnviromentStructure : public AActor, public ITDS_IGameActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDS_EnviromentStructure();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	EPhysicalSurface GetSurfaceType() override;
	TArray<UTDS_StateEffect*> GetAllCurrentEffects() override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void RemoveEffect(UTDS_StateEffect* RemoveEffect);
	void RemoveEffect_Implementation(UTDS_StateEffect* RemoveEffect) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void AddEffect(UTDS_StateEffect* newEffect);
	void AddEffect_Implementation(UTDS_StateEffect* newEffect) override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Setting")
		TArray<UTDS_StateEffect*> Effects;

	UPROPERTY(ReplicatedUsing = EffectAdd_OnRep)
		UTDS_StateEffect* EffectAdd = nullptr;
	UPROPERTY(ReplicatedUsing = EffectRemove_OnRep)
		UTDS_StateEffect* EffectRemove = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		TArray<UParticleSystemComponent*> ParticleSystemEffects;

	UFUNCTION()
		void EffectAdd_OnRep();
	UFUNCTION()
		void EffectRemove_OnRep();
	UFUNCTION()
		void SwitchEffect(UTDS_StateEffect* Effect, bool bIsAdd);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
		FVector OffsetEffect = FVector(0);


};
