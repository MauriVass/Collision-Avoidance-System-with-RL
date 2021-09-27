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
	Experience(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool gameEnded);
	~Experience();

	void InitializeExperience(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool gameEnded);

	static FString ConstructData(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool gameEnded);
	static FString ConstructData(TArray<int> currentState, float throttleAction, float steerAction, TArray<int> nextState, float reward, bool gameEnded);
	static FString ConstructData(Experience exp);

	bool CheckEqualExperiences(Experience expOther);

	bool GetInitilized();

private:
	bool Initilized;

	TArray<int> CurrentState;
	int Action;
	TArray<int> NextState;
	float Reward;
	bool GameEnded;

};
