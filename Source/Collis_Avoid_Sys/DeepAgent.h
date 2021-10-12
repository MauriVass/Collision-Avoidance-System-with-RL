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
	void SetThrottleAction(float throttle);
	void SetSteerAction(float steer);
	UFUNCTION(BlueprintCallable)
		void SetTickTime(float tickTime);
	UFUNCTION(BlueprintCallable)
		void SetManualControll(bool manualControll);

	UFUNCTION(BlueprintCallable)
		void ToggleIsTraining();
	UFUNCTION(BlueprintCallable)
		void ToggleIsGameStarting();

	UFUNCTION(BlueprintCallable)
		float GetEpsilon();
	UFUNCTION(BlueprintCallable)
		float GetReward();
	UFUNCTION(BlueprintCallable)
		float GetCumulativeReward();
	UFUNCTION(BlueprintCallable)
		int GetNumberSteps();
	UFUNCTION(BlueprintCallable)
		int GetEpisode();
	UFUNCTION(BlueprintCallable)
		bool GetIsTraining();
	UFUNCTION(BlueprintCallable)
		float GetTickTime();
	UFUNCTION(BlueprintCallable)
		bool GetManualControll();
	UFUNCTION(BlueprintCallable)
		float GetAverageSpeed();
	bool GetIsActionSpaceDescrete();


private:
	TArray<float> GetInput();

	UStaticMeshComponent* SensorPosition;

	static const int NumberSensor = 32;

	class AClient* Client;
	void PerformAction(int action);
	void Step();
	void RestartGame();

	FString SaveDirectory;
	FString FileName;
	void WriteToFile(int Episode, int numberSteps, float totalReward, float averageSpeed, bool gameEndedByCrush, bool allowOverwriting);

	UWheeledVehicleMovementComponent* MovementComponent;
	FTransform initialTransform;

	bool ManualControll;

	bool IsGameStarting;
	bool IsTraining;
	bool IsStateSpaceDescrete;
	int NumberActions;
	float Epsilon;
	float EpsilonDecay;
	float MinEpsilon;
	int Episode;
	int NumberSteps;
	int MaxNumberSteps;
	int NumberFitSteps;
	float AngleExtension;

	float Reward;
	float CumulativeReward;
	float NegativeReward;
	FVector TargetVector;
	void RewardFunction(TArray<float> currentState);
	float AverageSpeed;

	class Experience PreviousExperience;

	UPROPERTY(BlueprintSetter= SetIsGameEnded)
		bool IsGameEnded;
	UFUNCTION(BlueprintSetter)
	void SetIsGameEnded(bool value);

	int ModelSpecification;

	bool IsActionSpaceDescrete;
	int Action;
	float ThrottleAction;
	float SteerAction;

	int counter=0;

	float totalDistance;
	float TickTime;


};
