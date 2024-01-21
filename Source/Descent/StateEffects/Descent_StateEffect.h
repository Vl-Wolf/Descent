// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "Descent_StateEffect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class DESCENT_API UDescent_StateEffect : public UObject
{
	GENERATED_BODY()
	
public:

	virtual bool IsSupportedForNetworking()const override { return true; };

	virtual bool InitObject(AActor* Actor, FName BoneHit);
	virtual void DestroyObject();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
		TArray<TEnumAsByte<EPhysicalSurface>> PossibleInteractSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
		bool bIsStakable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Timer")
		UParticleSystem* ParticleEffect = nullptr;
	UParticleSystemComponent* ParticleEmitter = nullptr;

	AActor* myActor = nullptr;

	UPROPERTY(Replicated)
	FName NameBone;

};

UCLASS()
class DESCENT_API UDescent_StateEffect_ExecuteOnce : public UDescent_StateEffect
{
	GENERATED_BODY()

public:

	bool InitObject(AActor* Actor, FName BoneHit) override;
	void DestroyObject() override;

	virtual void ExecuteOnce();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Once")
		float Power = 20.0f;

};

UCLASS()
class DESCENT_API UDescent_StateEffect_ExecuteTimer : public UDescent_StateEffect
{
	GENERATED_BODY()

public:

	bool InitObject(AActor* Actor, FName BoneHit) override;
	void DestroyObject() override;

	virtual void Execute();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Timer")
		float Power = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Timer")
		float Timer = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Timer")
		float RateTimer = 1.0f;

	FTimerHandle TimerHandle_ExecuteTimer;
	FTimerHandle TimerHandle_EffectTimer;

};

UCLASS()
class DESCENT_API UDescent_StateEffect_AreaOfEffect : public UDescent_StateEffect_ExecuteTimer
{
	GENERATED_BODY()

public:

	bool InitObject(AActor* Actor, FName BoneHit) override;
	void DestroyObject() override;

	virtual void FindActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Area of Effect")
		float RadiusArea = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Damage")
		TSubclassOf<UDescent_StateEffect> Effect = nullptr;

	//�� �������� ���������!!!
	UParticleSystem* ParticleEffectAoE = ParticleEffect;
	UParticleSystemComponent* ParticleEmitterAoE = ParticleEmitter;
};
