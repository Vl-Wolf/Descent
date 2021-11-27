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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	EPhysicalSurface GetSurfaceType() override;
	TArray<UTDS_StateEffect*> GetAllCurrentEffects() override;
	void RemoveEffect(UTDS_StateEffect* RemoveEffect) override;
	void AddEffect(UTDS_StateEffect* newEffect) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
		TArray<UTDS_StateEffect*> Effects;

};
