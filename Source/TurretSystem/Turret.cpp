// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "DrawDebugHelpers.h"
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
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	DrawDebugSphere(GetWorld(), GetActorLocation(), SenseRange, 8, FColor::Blue, true, -1.0f, SDPG_World);

}

void ATurret::FindTarget()
{
	TArray<AActor*> OverlappingActors;

	const bool IsOverlapped = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(),
																		SenseRange, TraceObjectTypes, nullptr,
																		ActorsToIgnore, OverlappingActors);

	if(IsOverlapped)
	{	
		for (AActor*& HitResult : OverlappingActors)
		{

			if(!ClosestTarget)
			{
				ClosestTarget = HitResult;
			}

			if(GetDistanceTo(HitResult) < GetDistanceTo(ClosestTarget))
			{
				ClosestTarget = HitResult;
			}	
		}
	}
	else
	{
		ClosestTarget = nullptr;
	}

}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(ClosestTarget)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, ClosestTarget->GetName());
	}
}

