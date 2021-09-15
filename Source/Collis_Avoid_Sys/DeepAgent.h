// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

private:
	TArray<int> GetInput();

	UStaticMeshComponent* SensorPosition;

	static const int NumberSensor = 16;

	class AClient* Client;
	void SendExperience();
	void Predict();

	void Step();

	UWheeledVehicleMovementComponent* MovementComponent;

	int NumberActions;
	float Epsilos;
	float EpsilonDecay;
	/*bool GameStatus;*/
	UPROPERTY(BlueprintGetter = GetGameStatus, BlueprintSetter=SetGameStatus)
		bool GameStatus;
	UFUNCTION(BlueprintSetter)
	void SetGameStatus(bool value);
	UFUNCTION(BlueprintGetter)
		bool GetGameStatus();

	int Action;
	float Confidence;

};
