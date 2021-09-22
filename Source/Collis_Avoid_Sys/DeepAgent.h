// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Collis_Avoid_Sys/Experience.h>
#include "WheeledVehicle.h"
#include "DeepAgent.generated.h"

/**
 * 
 */
UCLASS()
class COLLIS_AVOID_SYS_API ADeepAgent : public AWheeledVehicle
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

public:
	virtual void Tick(float DeltaTime) override;

	void SetAction(int Action);
	void SetConfidence(float Confidence);
	UFUNCTION(BlueprintCallable)
		void ToggleIsTraining();

	UFUNCTION(BlueprintCallable)
		float GetEpsilon();
	UFUNCTION(BlueprintCallable)
		float GetReward();
	UFUNCTION(BlueprintCallable)
		float GetCumulativeReward();
	UFUNCTION(BlueprintCallable)
		int GetNumberSteps();
	UFUNCTION(BlueprintCallable)
		int GetEpoch();
	UFUNCTION(BlueprintCallable)
		bool GetIsTraining();

private:

	TArray<int> GetInput();

	UStaticMeshComponent* SensorPosition;

	static const int NumberSensor = 128;

	class AClient* Client;
	void SendExperience(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool endGame);
	void PerformAction(int action);
	void Step();
	void RestartGame();

	UWheeledVehicleMovementComponent* MovementComponent;
	FTransform initialTransform;

	bool IsTraining;
	int NumberActions;
	float Epsilon;
	float EpsilonDecay;
	float MinEpsilon;

	int Epoch;
	int NumberSteps;
	int MaxNumberSteps;
	int NumberFitSteps;

	float Reward;
	float CumulativeReward;

	class Experience PreviousExperience;

	UPROPERTY(BlueprintSetter= SetIsGameEnded)
		bool IsGameEnded;
	UFUNCTION(BlueprintSetter)
	void SetIsGameEnded(bool value);

	int Action;
	float Confidence;

	int counter=0;

	float totalDistance;
	float TickTime;

};
