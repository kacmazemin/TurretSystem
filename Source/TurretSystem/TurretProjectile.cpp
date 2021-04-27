// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretProjectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ATurretProjectile::ATurretProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetRelativeRotation(FRotator(0,0,90));

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(ProjectileMesh);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");

	ProjectileMovementComponent->Velocity = {0,0,5000};
	ProjectileMovementComponent->ProjectileGravityScale = .2f;
	
}

void ATurretProjectile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	BoxCollision->SetRelativeRotation(FRotator(0,0,90));
	BoxCollision->SetBoxExtent(ProjectileMesh->Bounds.BoxExtent);
	BoxCollision->SetRelativeLocation(FVector{0,0,ProjectileMesh->Bounds.BoxExtent.Y});
}

// Called when the game starts or when spawned
void ATurretProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATurretProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

