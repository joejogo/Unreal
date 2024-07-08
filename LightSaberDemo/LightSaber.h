// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PivotHelper.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "LightSaber.generated.h"

UCLASS()
class PORTFOLIO_API ALightSaber : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightSaber();
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* ArrowComp;
	UPROPERTY(EditDefaultsOnly)
	class UCapsuleComponent* BladeColl;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* DeflectZoneColl;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* HandleMesh;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* BladeMesh;
	UPROPERTY(EditDefaultsOnly)
	UAudioComponent* LaserDeflectSound;
	UPROPERTY(EditDefaultsOnly)
	UAudioComponent* SaberMoveSound;

	UFUNCTION(BlueprintCallable)
	void HandleLaserCollision(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintCallable)
	FRotator TiltSaber(FVector LaserImpactPoint, APivotHelper*& OutPivHelperTop, APivotHelper*& OutPivHelperB);
	UFUNCTION(BlueprintCallable)
	int GetSaberTiltModifier(FVector InboundLaserImpactPoint);
	UFUNCTION()
	TArray<AActor*> GetEnemiesInRange() const;

	UPROPERTY(BlueprintReadWrite)
	FVector SaberRestPosition;
	UPROPERTY(BlueprintReadWrite)
	FRotator SaberRestRotation;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> DebugObject;

	UPROPERTY()
	FTimerHandle JediTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BraceForLaserEvent(FVector LaserImpactPoint);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ReturnSaberToRestEvent();
	UFUNCTION(BlueprintCallable)
	void ReturnSaberToRest(float InterpAmt);
	
	UPROPERTY(BlueprintReadOnly)
	double LeftRightTiltAmt;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class APivotHelper> TiltAssistActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float EnemySearchRadius;

	
	

};
