// Fill out your copyright notice in the Description page of Project Settings.


#include "Experience.h"

Experience::Experience()
{
}

Experience::Experience(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool gameEnded)
{
	Experience::InitializeExperience(currentState, action, nextState, reward, gameEnded);
}

Experience::~Experience()
{
	Experience::Initilized = false;
}

void Experience::InitializeExperience(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool gameEnded)
{
	Experience::CurrentState = currentState;
	Experience::Action = action;
	Experience::NextState = nextState;
	Experience::Reward = reward;
	Experience::GameEnded = gameEnded;

	Experience::Initilized = true;
}

FString Experience::ConstructData(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool gameEnded)
{
	FString result;
	//CURRENT STATE
	for (int i = 0; i < currentState.Num(); i++)
	{
		if (i > 0)
			result.Append(":");
		result.Append(FString::FromInt(currentState[i]));
	}
	result.Append(";");

	//ACTION CHOSEN
	result.Append(FString::FromInt(action));
	result.Append(";");

	//NEXT STATE
	for (int i = 0; i < nextState.Num(); i++)
	{
		if (i > 0)
			result.Append(":");
		result.Append(FString::FromInt(nextState[i]));
	}
	result.Append(";");

	//REWARD GOT
	result.Append(FString::SanitizeFloat(reward));
	result.Append(";");

	//THE GAME ENDED OR NOT (0 false, 1 true)
	result.Append(FString::FromInt(gameEnded));
	return result;
}
FString Experience::ConstructData(TArray<int> currentState, float throttleAction, float steerAction, TArray<int> nextState, float reward, bool gameEnded)

{
	FString result;
	//CURRENT STATE
	for (int i = 0; i < currentState.Num(); i++)
	{
		if (i > 0)
			result.Append(":");
		result.Append(FString::FromInt(currentState[i]));
	}
	result.Append(";");

	//ACTION CHOSEN
	result.Append(FString::SanitizeFloat(throttleAction));
	result.Append(":");
	result.Append(FString::SanitizeFloat(steerAction));
	result.Append(";");

	//NEXT STATE
	for (int i = 0; i < nextState.Num(); i++)
	{
		if (i > 0)
			result.Append(":");
		result.Append(FString::FromInt(nextState[i]));
	}
	result.Append(";");

	//REWARD GOT
	result.Append(FString::SanitizeFloat(reward));
	result.Append(";");

	//THE GAME ENDED OR NOT (0 false, 1 true)
	result.Append(FString::FromInt(gameEnded));
	return result;
}
FString Experience::ConstructData(Experience exp)
{
	return Experience::ConstructData(exp.CurrentState,exp.Action,exp.NextState,exp.Reward,exp.GameEnded);
}

bool Experience::CheckEqualExperiences(Experience expOther)
{
	FString exp_string = Experience::ConstructData(*this);
	FString expOther_string = Experience::ConstructData(expOther);

	if (exp_string.Equals(expOther_string))
		return true;
	else
		return false;
}

bool Experience::GetInitilized()
{
	return Experience::Initilized;
}
