// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Client.generated.h"

UCLASS()
class COLLIS_AVOID_SYS_API AClient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClient();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FString UrlAddress;

	FString ConstructData(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool EndGame );
	//FString ConstructData(State state);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SendExperience(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool endGame);

};
