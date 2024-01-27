// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Descent/StateEffects/Descent_StateEffect.h"
#include "Descent/Interface/Descent_IGameActor.h"
#include "Descent_EnvironmentStructure.generated.h"

UCLASS()
class DESCENT_API ADescent_EnvironmentStructure : public AActor, public IDescent_IGameActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADescent_EnvironmentStructure();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	EPhysicalSurface GetSurfaceType() override;
	TArray<UDescent_StateEffect*> GetAllCurrentEffects() override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void RemoveEffect(UDescent_StateEffect* RemoveEffect);
	void RemoveEffect_Implementation(UDescent_StateEffect* RemoveEffect) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void AddEffect(UDescent_StateEffect* newEffect);
	void AddEffect_Implementation(UDescent_StateEffect* newEffect) override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Setting")
		TArray<UDescent_StateEffect*> Effects;

	UPROPERTY(ReplicatedUsing = EffectAdd_OnRep)
		UDescent_StateEffect* EffectAdd = nullptr;
	UPROPERTY(ReplicatedUsing = EffectRemove_OnRep)
		UDescent_StateEffect* EffectRemove = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		TArray<UParticleSystemComponent*> ParticleSystemEffects;

	UFUNCTION()
		void EffectAdd_OnRep();
	UFUNCTION()
		void EffectRemove_OnRep();
	UFUNCTION()
		void SwitchEffect(UDescent_StateEffect* Effect, bool bIsAdd);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
		FVector OffsetEffect = FVector(0);


};
