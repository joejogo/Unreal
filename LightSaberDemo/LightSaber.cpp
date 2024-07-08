// Fill out your copyright notice in the Description page of Project Settings.


#include "LightSaber.h"

#include "Laser.h"
#include "PivotHelper.h"
#include "TrainingTarget.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ALightSaber::ALightSaber()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	RootComponent = ArrowComp;
	BladeColl = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BladeColl"));
	BladeColl->SetupAttachment(ArrowComp);
	DeflectZoneColl = CreateDefaultSubobject<UBoxComponent>(TEXT("DeflectZoneColl"));
	DeflectZoneColl->SetupAttachment(BladeColl);
	HandleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandleMesh"));
	HandleMesh->SetupAttachment(BladeColl);
	BladeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BladeMesh"));
	BladeMesh->SetupAttachment(HandleMesh);
	LaserDeflectSound = CreateDefaultSubobject<UAudioComponent>(TEXT("LaserDeflectSound"));
	LaserDeflectSound->SetupAttachment(ArrowComp);
	SaberMoveSound = CreateDefaultSubobject<UAudioComponent>(TEXT("SaberMoveSound"));
	SaberMoveSound->SetupAttachment(ArrowComp);

	// Bind any event dynamics
	BladeColl->OnComponentBeginOverlap.AddDynamic(this, &ALightSaber::HandleLaserCollision);
	
}

// Called when the game starts or when spawned
void ALightSaber::BeginPlay()
{
	Super::BeginPlay();

	SaberRestPosition = FVector(-70,0,0);
	SaberRestRotation = FRotator::ZeroRotator;
}

// Called every frame
void ALightSaber::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<AActor*> ALightSaber::GetEnemiesInRange() const
{
	TArray<AActor*> EnemiesInRange;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	UClass* TargetClass = ATrainingTarget::StaticClass();
	TArray<AActor*> ActorsToIgnore;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), EnemySearchRadius,
		TraceObjectTypes, TargetClass, ActorsToIgnore, EnemiesInRange);
	return EnemiesInRange;
}

void ALightSaber::HandleLaserCollision(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || !(OtherActor->ActorHasTag("Laser"))) return;

	// Play laser deflect sound
	LaserDeflectSound->Play(.2);

	ALaser* const OtherActorLaser = Cast<ALaser>(OtherActor);
	FVector const ShooterLocation = OtherActorLaser->LaserOwner->GetActorLocation();	
	
	// Set the actor rotation to the deflect target
	TArray<AActor*> EnemiesInRange = GetEnemiesInRange();

	FVector TargetLocation = GetActorForwardVector() * 1000;
	if (EnemiesInRange.Num() > 0)
	{		
		int const RandEnemyIdx = UKismetMathLibrary::RandomIntegerInRange(0, EnemiesInRange.Num()-1);
		TargetLocation = EnemiesInRange[RandEnemyIdx]->GetActorLocation();
	}
	
	FRotator const LookAtSaberTarget = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
	FRotator const LookAtSaberShooter = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ShooterLocation);
	OtherActor->SetActorRotation(LookAtSaberTarget - LookAtSaberShooter);	

	// Deflect the laser by setting its ProjectileComponent velocity
	UProjectileMovementComponent* LaserProjMoveComp = OtherActorLaser->GetComponentByClass<UProjectileMovementComponent>();

	FVector InboundLaserVelocity = LaserProjMoveComp->Velocity;
	FVector DeflectedLaserVelocity = FVector(
	-InboundLaserVelocity.X,
	-InboundLaserVelocity.Y,
	InboundLaserVelocity.Z
		);
	LaserProjMoveComp->SetVelocityInLocalSpace(DeflectedLaserVelocity);
}

void ALightSaber::ReturnSaberToRest(float InterpAmt)
{	
	SetActorLocationAndRotation
	(
		// Position
		UKismetMathLibrary::VLerp(GetActorLocation(), SaberRestPosition, InterpAmt),
		// Rotation
		UKismetMathLibrary::RLerp(GetActorRotation(), SaberRestRotation, InterpAmt, true)
	);
}

int ALightSaber::GetSaberTiltModifier(FVector InboundLaserImpactPoint)
{
	if (InboundLaserImpactPoint.Y >= GetActorRightVector().Y)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

FRotator ALightSaber::TiltSaber(FVector LaserImpactPoint, APivotHelper*& OutPivHelperTop, APivotHelper*& OutPivHelperB)
{	
	// Randomize deflect point height along saber
	float DeflectPtHeight =  UKismetMathLibrary::RandomFloatInRange(150.f, 250.f);
	FVector SaberDeflectPoint = (GetActorUpVector() * FVector(0,0,DeflectPtHeight)) + GetActorLocation();
	
	// Tilt (Left to Right)
	double DotProd = UKismetMathLibrary::Dot_VectorVector(SaberDeflectPoint, LaserImpactPoint);
	double Magnitude_SaberDeflectPoint = UKismetMathLibrary::VSize(SaberDeflectPoint);
	double Magnitude_ImpactPoint = UKismetMathLibrary::VSize(LaserImpactPoint);
	double SaberRotateAngle = UKismetMathLibrary::DegAcos(DotProd /
		UKismetMathLibrary::Multiply_DoubleDouble(Magnitude_SaberDeflectPoint, Magnitude_ImpactPoint));
	double SaberTiltDirection = GetSaberTiltModifier(LaserImpactPoint);
	LeftRightTiltAmt = SaberRotateAngle * SaberTiltDirection;

	// Spawn tilt assists
	OutPivHelperB = GetWorld()->SpawnActor<APivotHelper>(TiltAssistActor, SaberDeflectPoint, FRotator::ZeroRotator);
	OutPivHelperTop = GetWorld()->SpawnActor<APivotHelper>(TiltAssistActor, LaserImpactPoint, FRotator::ZeroRotator);
	// Attach saber to OutPivHelperB
	AttachToActor(OutPivHelperB, FAttachmentTransformRules::KeepWorldTransform);
	// Attach OutPivHelperB to OutPivHelperTop
	OutPivHelperB->AttachToActor(OutPivHelperTop,
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			EAttachmentRule::KeepWorld,
			false)
		);
	
	//Rotate Saber (By rotating tilt assist)
	FRotator NewRotation = FRotator(
			UKismetMathLibrary::RandomFloatInRange(-30.f, 30.f),
			0,
			LeftRightTiltAmt);

	// Play saber move sound
	SaberMoveSound->Play(.15f);
	
	return NewRotation;
}
