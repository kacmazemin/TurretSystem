// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretSystemFunctionLibrary.h"

#include "DrawDebugHelpers.h"

bool UTurretSystemFunctionLibrary::HasLineOfSight(const UObject* WorldContextObject, FHitResult& Hit, const FVector& From,
                                                  const FVector& To, const TArray<AActor*>& ActorsToIgnore)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);
	
	return !WorldContextObject->GetWorld()->LineTraceSingleByChannel(Hit, From, To, ECC_Visibility, QueryParams);
}