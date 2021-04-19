// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class TURRETSYSTEM_API ATurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurret();

	UPROPERTY(EditInstanceOnly)
	UStaticMeshComponent* TurretSM = nullptr;

	UPROPERTY(EditInstanceOnly)
	USphereComponent* SphereComponent = nullptr;

	UFUNCTION(BlueprintCallable)
	void FindTarget();

	UPROPERTY(EditInstanceOnly, Category = "TurretConfig", meta = (ClampMin = "1.0", ClampMax = "2000.0", UIMin = "1.0", UIMax = "2000.0"))
	float SenseRange = 1000.f;

	UPROPERTY(EditInstanceOnly, Category = "TurretConfig")
	bool EnableSphere = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;
	
	UPROPERTY()
	TArray<AActor*> ActorIgnoreSphereOverlap;

	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	
	UPROPERTY()
	AActor* BestTarget = nullptr;


	FHitResult SightHitResult;
};
