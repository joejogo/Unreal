// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightSaber.h"
#include "Laser.generated.h"

UCLASS()
class PORTFOLIO_API ALaser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnLaserOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& HitRes);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> SparkFxActor;
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* BoxColl;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* LaserMesh;
	UPROPERTY(EditDefaultsOnly)
	float LaserSpeed = 2000.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* ProjMoveComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ALightSaber> PlayerSaber;

	UPROPERTY(EditInstanceOnly)
	AActor* LaserOwner;
	

	

};
