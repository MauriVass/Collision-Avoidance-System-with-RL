// Fill out your copyright notice in the Description page of Project Settings.


#include "Client.h"
#include <Http.h>

// Sets default values
AClient::AClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AClient::BeginPlay()
{
	Super::BeginPlay();
	
	AClient::UrlAddress = "http://127.0.0.1:8080";
}


// Called every frame
void AClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClient::SendExperience(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool endGame)
{
	/*TArray<int> currentState = { 1,2,3,4,5,6 };
	int action = 1;
	TArray<int> nextState = { 10,20,30,40,50,60 };
	float reward = 0.1;
	bool endGame = false;*/

	FString data = AClient::ConstructData(currentState,action,nextState,reward,endGame);
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
	HttpRequest->SetVerb("Post");
	HttpRequest->SetHeader("Content-Type", "text/plain");
	HttpRequest->SetURL(AClient::UrlAddress);
	HttpRequest->SetContentAsString(data);
	HttpRequest->ProcessRequest();
}


FString AClient::ConstructData(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool endGame)
{
	FString result;
	//CURRENT STATE
	for (int i = 0; i < currentState.Num(); i++)
	{
		if (i > 0)
			result.Append(".");
		result.Append(FString::FromInt(currentState[i]));
	}
	result.Append(";");

	//ACTION CHOSEN
	result.Append(FString::SanitizeFloat(action));
	result.Append(";");

	//NEXT STATE
	for (int i = 0; i < nextState.Num(); i++)
	{
		if (i > 0)
			result.Append(".");
		result.Append(FString::FromInt(nextState[i]));
	}
	result.Append(";");


	//REWARD GOT
	result.Append(FString::FromInt(reward));
	result.Append(";");

	//THE GAME ENDED OR NOT (0 false, 1 true)
	result.Append(FString::FromInt(endGame));
	return result;
}