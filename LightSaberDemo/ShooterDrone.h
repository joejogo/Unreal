// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ShooterDrone.generated.h"

UCLASS()
class PORTFOLIO_API AShooterDrone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AShooterDrone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnCompOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/*
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> SparkFxActor;
	*/

	UFUNCTION(BlueprintCallable)
	void Shoot();

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* SphereColl;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* DroneMesh;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* LaserSpawnPoint;

	UPROPERTY(EditDefaultsOnly)
	class ALightSaber* JediInRange;

	UFUNCTION(BlueprintCallable)
	void Twitch() const;

	int MoveDir = 1;
	
	FTimerHandle DroneShootTimerHandle;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ALaser> LaserToFire;
	

};
