// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "DrawDebugHelpers.h"
#include "TurretSystemFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundCue.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurretSM = CreateDefaultSubobject<UStaticMeshComponent>("TurretStaticMesh");
	TurretSM->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>("BoxCollision");
	SphereComponent->SetupAttachment(TurretSM);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	AudioComponent->SetupAttachment(TurretSM);
	AudioComponent->bAlwaysPlay = true;
	
	ActorsToIgnore.Reserve(2);
	ActorsToIgnore.Add(this);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	if(RotationSoundCue)
	{
		AudioComponent->SetSound(RotationSoundCue);		
	}
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
		RotateToTarget();
	}
	else
	{
		IdleRotate(DeltaTime);
	}
}

void ATurret::RotateToTarget()
{
	if(BestTarget && TurretSM)
	{
		const FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(TurretSM->GetRelativeLocation(),BestTarget->GetActorLocation());
		TurretSM->SetRelativeRotation({TurretSM->GetRelativeRotation().Pitch, DesiredRotation.Yaw, TurretSM->GetRelativeRotation().Roll});
	}
}

void ATurret::PlayRotateSound()
{
	if(RotationSoundCue)
	{
		AudioComponent->Stop();
		AudioComponent->Play();
	}
}

void ATurret::IdleRotate(const float DeltaSecond)
{
	if(!bIsRotating)
	{
		RandValue = FMath::FRandRange(-180.f,180.f);
		
		PlayRotateSound();
		bIsRotating = true;	
	}

	if(bIsRotating && !bIsInDelayTime)
	{
		RotateValue = FMath::FInterpTo(TurretSM->GetRelativeRotation().Yaw, RandValue, DeltaSecond, InterpolationSpeed);
        TurretSM->SetRelativeRotation({TurretSM->GetRelativeRotation().Pitch, RotateValue, TurretSM->GetRelativeRotation().Roll});
	}

	if(FMath::IsNearlyEqual(RandValue, TurretSM->GetRelativeRotation().Yaw, 1.f) && !bIsInDelayTime)
	{
		bIsInDelayTime = true;
		
		if(bIsInDelayTime)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle,[&]()
			{
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			    bIsInDelayTime = false;
				bIsRotating = false;
			},1.f,false,FMath::RandRange(1.1f, 1.6f));
		}
	}

}