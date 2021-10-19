// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Http.h>
#include "Client.generated.h"

UCLASS()
class COLLIS_AVOID_SYS_API AClient : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FString UrlAddress;

	void GetPrediction(FHttpRequestPtr request, FHttpResponsePtr Response, bool bWasSuccessful);
	void Initialization(FHttpRequestPtr request, FHttpResponsePtr Response, bool bWasSuccessful);

	class ADeepAgent* Agent;

public:	
	// Sets default values for this actor's properties
	AClient();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SendMetadata(int numSensors, int numActions, float negativeReward, int modelSpecification, bool prioritize);
	void SendExperience(TArray<float> currentState, int action, TArray<float> nextState, float reward, bool endGame);
	void Predict(TArray<float> currentState);

	void SetDeepAgent(ADeepAgent* Agent);
};
