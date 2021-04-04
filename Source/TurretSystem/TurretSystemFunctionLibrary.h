// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/BlueprintFunctionLibraryFactory.h"
#include "TurretSystemFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TURRETSYSTEM_API UTurretSystemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "TurretSystem" )
	static bool HasLineOfSight(const UObject* WorldContextObject, FHitResult& Hit, const FVector& From,
                                                  const FVector& To, const TArray<AActor*>& ActorsToIgnore);

};
