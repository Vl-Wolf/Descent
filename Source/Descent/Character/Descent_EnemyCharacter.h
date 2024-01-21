// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Descent//Interface/Descent_IGameActor.h"
#include "Descent_EnemyCharacter.generated.h"


UCLASS()
class DESCENT_API ADescent_EnemyCharacter : public ACharacter, public IDescent_IGameActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADescent_EnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	

};
