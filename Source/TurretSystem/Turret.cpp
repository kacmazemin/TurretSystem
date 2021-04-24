// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "DrawDebugHelpers.h"
#include "TurretSystemFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
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

	
	FOnTimelineFloat TimelineCallback;
	FOnTimelineEventStatic TimelineFinishedCallback;
	
	TimelineComponent = NewObject<UTimelineComponent>(this, FName("IdleAnimTimeline"));
	TimelineComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript; // Indicate it comes from a blueprint so it gets cleared when we rerun construction scripts
	this->BlueprintCreatedComponents.Add(TimelineComponent); // Add to array so it gets saved
	TimelineComponent->SetNetAddressable();

	TimelineCallback.BindUFunction(this, FName("IdleRotation"));
	TimelineFinishedCallback.BindUFunction(this, FName("IdleFinish"));
	
	TimelineComponent->AddInterpFloat(CurveFloat, TimelineCallback);
	TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);
	
	// TimelineComponent->SetLooping(true);
	TimelineComponent->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	TimelineComponent->RegisterComponent();

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
		if(TimelineComponent && TimelineComponent->IsPlaying())
		{
			TimelineComponent->Stop();			
		}
	}
	else
	{
		if(TimelineComponent && !TimelineComponent->IsPlaying())
		{
			TimelineComponent->Play();
			bIsReverse = false;
			GEngine->AddOnScreenDebugMessage(-1,1,FColor::Black,"WORKWORKWORK");
			playRotateSound();
		}
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

void ATurret::IdleRotation(const float Value)
{
	if(!BestTarget)
	{
		TurretSM->SetRelativeRotation({TurretSM->GetRelativeRotation().Pitch, Value, TurretSM->GetRelativeRotation().Roll});
	}
}

void ATurret::IdleFinish()
{
	if(TimelineComponent)
	{
		if(!TimelineComponent->IsPlaying())
		{
			if(bIsReverse)
			{
				bIsReverse = false;
				TimelineComponent->Play();
				playRotateSound();
			}
			else
			{
				bIsReverse = true;
				TimelineComponent->Reverse();
				playRotateSound();
			}
		}
	}
}

void ATurret::playRotateSound()
{
	if(RotationSoundCue)
	{
		AudioComponent->Stop();
		AudioComponent->Play();
	}
}