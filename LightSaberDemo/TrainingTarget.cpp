// Fill out your copyright notice in the Description page of Project Settings.


#include "TrainingTarget.h"

// Sets default values
ATrainingTarget::ATrainingTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATrainingTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrainingTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

