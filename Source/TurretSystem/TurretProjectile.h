// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretProjectile.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class TURRETSYSTEM_API ATurretProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurretProjectile();
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditInstanceOnly)
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(EditInstanceOnly)
	UStaticMeshComponent* ProjectileMesh = nullptr;

	UPROPERTY(EditInstanceOnly)
	UBoxComponent* BoxCollision = nullptr;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
