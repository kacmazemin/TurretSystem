// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "DrawDebugHelpers.h"
#include "TurretSystemFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurretSM = CreateDefaultSubobject<UStaticMeshComponent>("TurretStaticMesh");
	TurretSM->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>("BoxCollision");
	SphereComponent->SetupAttachment(TurretSM);

	ActorsToIgnore.Reserve(2);
	ActorsToIgnore.Add(this);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

}

void ATurret::FindTarget()
{
	if(EnableSphere)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), SenseRange, 8, FColor::Blue, false, -1.0f, SDPG_World);		
	}

	TArray<AActor*> OverlappingActors;

	const bool IsOverlapped = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(),
																		SenseRange, TraceObjectTypes, nullptr,
																		ActorIgnoreSphereOverlap, OverlappingActors);
	float BestDistance = SenseRange;

	AActor* ClosestTarget = nullptr;

	if(IsOverlapped)
	{	
		for (AActor*& HitResult : OverlappingActors)
		{
			ActorsToIgnore[1] = HitResult;

			if(GetDistanceTo(HitResult) < BestDistance || !ClosestTarget)
			{
				if(UTurretSystemFunctionLibrary::HasLineOfSight(this, SightHitResult, GetActorLocation(), HitResult->GetActorLocation(), ActorsToIgnore))
				{
					ClosestTarget = HitResult;
					BestDistance = GetDistanceTo(ClosestTarget);
				}
			}
		}

		BestTarget = ClosestTarget;
	}

}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(BestTarget)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, BestTarget->GetName());
	}
}

