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

void AClient::SendData()
{
	TArray<int> currentState = { 1,2,3,4,5,6 };
	int action = 1;
	TArray<int> nexState = { 10,20,30,40,50,60 };
	float reward = 0.1;
	bool endGame = false;

	FString data = currentState.ToString() + action;
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
	HttpRequest->SetVerb("Post");
	HttpRequest->SetHeader("Content-Type", "text/plain");
	HttpRequest->SetURL(AClient::UrlAddress);
	HttpRequest->SetContentAsString(data);
	HttpRequest->ProcessRequest();
}

