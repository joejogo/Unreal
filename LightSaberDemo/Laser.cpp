// Fill out your copyright notice in the Description page of Project Settings.


#include "Laser.h"

#include "LightSaber.h"
#include "TrainingTarget.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALaser::ALaser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	BoxColl = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxColl"));
	RootComponent = BoxColl;
	LaserMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMesh"));
	LaserMesh->SetupAttachment(BoxColl);
	ProjMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjMoveComp"));

	// Init Projectile Movement
	ProjMoveComp->InitialSpeed = LaserSpeed;
	ProjMoveComp->MaxSpeed = LaserSpeed;
	ProjMoveComp->ProjectileGravityScale = 0.f;
	ProjMoveComp->Velocity = FVector(-1,0,0);

	InitialLifeSpan = 1.f;
}

// Called when the game starts or when spawned
void ALaser::BeginPlay()
{
	Super::BeginPlay();
	
	BoxColl->OnComponentBeginOverlap.AddDynamic(this, &ALaser::OnLaserOverlap);
}

// Called every frame
void ALaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALaser::OnLaserOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& HitRes)	
{
	if (HitRes.ImpactPoint != FVector::Zero() &&
		OtherActor->IsA(ATrainingTarget::StaticClass()) && OtherActor->ActorHasTag("TrainingTarget"))
	{
		SetHidden(false);
		BoxColl->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
		BoxColl->SetGenerateOverlapEvents(false);
		
		AActor* SpawnedSparkActor = GetWorld()->SpawnActor<AActor>(SparkFxActor, HitRes.ImpactPoint, FRotator::ZeroRotator);
		SpawnedSparkActor->GetComponentByClass<UNiagaraComponent>()->Activate();

		
	}
}
