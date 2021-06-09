// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "DrawDebugHelpers.h"
#include "TurretProjectile.h"
#include "TurretSystemFunctionLibrary.h"
#include "Components/ArrowComponent.h"
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
	SetRootComponent(RootComponent);
	TurretSM->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereComponent = CreateDefaultSubobject<USphereComponent>("BoxCollision");
	SphereComponent->SetupAttachment(TurretSM);

	RotationAC = CreateDefaultSubobject<UAudioComponent>("RotationAudioComponent");
	RotationAC->SetupAttachment(TurretSM);
	RotationAC->bAlwaysPlay = true;

	FireAC = CreateDefaultSubobject<UAudioComponent>("FireAudioComponent");
	FireAC->SetupAttachment(TurretSM);
	FireAC->bAlwaysPlay = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->SetupAttachment(TurretSM);
	ArrowComponent->SetRelativeLocation({60.f,0.f,130.f});
	
	ActorsToIgnore.Reserve(3);
	ActorsToIgnore.Add(this);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	if(RotationSoundCue)
	{
		RotationAC->SetSound(RotationSoundCue);		
	}

	if(FireSoundEffect)
	{
		FireAC->SetSound(FireSoundEffect);
	}
	
	ActorsToIgnore.Add(ProjectileActor.GetDefaultObject());

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
			ActorsToIgnore[2] = HitResult;

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

		FireProjectile();
	}
	else
	{
		if(GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		}
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
		RotationAC->Stop();
		RotationAC->Play();
	}
}

void ATurret::PlayFireSound()
{
	if(FireAC)
	{
		FireAC->Stop();
		FireAC->Play();
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

void ATurret::FireProjectile()
{
	if(!GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle,[=]()
		{
			ATurretProjectile* TurretProjectile = GetWorld()->SpawnActor<ATurretProjectile>(ProjectileActor.Get(),
                ArrowComponent->GetComponentLocation(), {0,TurretSM->GetRelativeRotation().Yaw, 0});

			PlayFireSound();
			
		},1.f, false, FireRate);
	}
}