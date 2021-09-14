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

private:
	void GetInput();

	UStaticMeshComponent* SensorPosition;

	static const int NumberSensor = 16;

	class AClient* Client;
	void SendExperience();
};
