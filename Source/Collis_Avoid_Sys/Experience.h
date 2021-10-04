// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class COLLIS_AVOID_SYS_API Experience
{
public:
	Experience();
	Experience(TArray<float> currentState, int action, TArray<float> nextState, float reward, bool gameEnded);
	~Experience();

	void InitializeExperience(TArray<float> currentState, int action, TArray<float> nextState, float reward, bool gameEnded);

	static FString ConstructData(TArray<float> currentState, int action, TArray<float> nextState, float reward, bool gameEnded);
	static FString ConstructData(TArray<float> currentState, float throttleAction, float steerAction, TArray<float> nextState, float reward, bool gameEnded);
	static FString ConstructData(Experience exp);

	bool CheckEqualExperiences(Experience expOther);

	bool GetInitilized();

private:
	bool Initilized;

	TArray<float> CurrentState;
	int Action;
	TArray<float> NextState;
	float Reward;
	bool GameEnded;

};
