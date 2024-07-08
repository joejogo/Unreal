// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterDrone.h"

#include "Laser.h"
#include "LightSaber.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "UniversalObjectLocators/UniversalObjectLocatorUtils.h"

// Sets default values
AShooterDrone::AShooterDrone()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereColl = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColl"));
	RootComponent = SphereColl;
	DroneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DroneMesh"));
	DroneMesh->SetupAttachment(SphereColl);
	LaserSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LaserSpawnPoint"));
	LaserSpawnPoint->SetupAttachment(DroneMesh);	
}

// Called when the game starts or when spawned
void AShooterDrone::BeginPlay()
{
	Super::BeginPlay();
	
	SphereColl->OnComponentBeginOverlap.AddDynamic(this, &AShooterDrone::OnCompOverlap);	
	GetWorldTimerManager().SetTimer(DroneShootTimerHandle, this, &AShooterDrone::Shoot, 1.f, true);	
}

// Called every frame
void AShooterDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	AddActorWorldOffset(FVector(0,MoveDir,0));;	
	
}

// Called to bind functionality to input
void AShooterDrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AShooterDrone::OnCompOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("DroneRebound")))
	{
		MoveDir = -MoveDir;	
	}	
}

void AShooterDrone::Shoot()
{
	// Look at the jedi first
	if (JediInRange)
	{
		FVector JediLoc = JediInRange->GetActorLocation();
		float JediApproxLocY = UKismetMathLibrary::RandomFloatInRange(JediLoc.Y-100, JediLoc.Y+100);
		FVector JediApproxLoc = FVector(JediLoc.X, JediApproxLocY, JediLoc.Z+250); // 200 for height of sword
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), JediApproxLoc));	
	}
	
	// Trace Start
	FVector TraceStartPt = LaserSpawnPoint->GetComponentLocation();
	// Trace end
	FVector TraceEndPt = (GetActorForwardVector() * 1000) + TraceStartPt;
	FHitResult TraceHit;
	bool bHitSuccess = GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStartPt, TraceEndPt, ECC_GameTraceChannel1);
	if (bHitSuccess && (TraceHit.GetActor() != nullptr))
	{
		ALightSaber* HitSaber = Cast<ALightSaber>(TraceHit.GetActor());
		if (!HitSaber) return;
		ALaser* SpawnedLaser = GetWorld()->SpawnActor<ALaser>(LaserToFire, TraceStartPt, GetActorForwardVector().Rotation());
		if (!SpawnedLaser) return;
		SpawnedLaser->LaserOwner = this;
		HitSaber->BraceForLaserEvent(TraceHit.ImpactPoint);
	}
}

void AShooterDrone::Twitch() const
{	
	DroneMesh->AddLocalRotation(FRotator(
		0.f,		
		UKismetMathLibrary::RandomFloatInRange(-30.f, 30.f),
		0.f
		));
}
