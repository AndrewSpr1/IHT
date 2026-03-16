// Fill out your copyright notice in the Description page of Project Settings.


#include "TestProj/Bot'sSystem/Public/GlobalCamera.h"


// Sets default values
AGlobalCamera::AGlobalCamera()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGlobalCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGlobalCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

