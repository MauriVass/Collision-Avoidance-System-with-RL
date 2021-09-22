// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawDebugHelpers.h"
#include <Collis_Avoid_Sys/Experience.h>
#include "Client.h"
#include "DeepAgent.h"
#include "WheeledVehicleMovementComponent.h"

void ADeepAgent::BeginPlay() {
	Super::BeginPlay();

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	TArray<USceneComponent*> Childrens;
	this->GetMesh()->GetChildrenComponents(true, Childrens);
	/*UE_LOG(LogTemp, Error, TEXT("Number Children %d"), Childrens.Num());
	for (int i = 0; i < Childrens.Num(); i++)
	{
		UE_LOG(LogTemp, Error, TEXT("Children name %d %s"),i, *Childrens[i]->GetName());
	}*/
	if (Childrens.Num() > 0)
		ADeepAgent::SensorPosition = Cast<UStaticMeshComponent>(Childrens[0]);

	ADeepAgent::Client = GetWorld()->SpawnActor<AClient>();
	ADeepAgent::Client->SetActorLabel("Client");
	ADeepAgent::Client->SetDeepAgent(this);

	ADeepAgent::MovementComponent = this->GetVehicleMovementComponent();
	ADeepAgent::initialTransform = this->GetTransform();

	ADeepAgent::IsTraining = true;
	ADeepAgent::Epoch = 0;
	ADeepAgent::NumberActions = 5;
	ADeepAgent::Epsilon = 1.0;
	ADeepAgent::EpsilonDecay = 4 * FMath::Pow(10,-5);
	ADeepAgent::MinEpsilon = 0.05;
	ADeepAgent::MaxNumberSteps = 12000;
	ADeepAgent::NumberFitSteps = 3;

	ADeepAgent::TickTime = 0.01;

	ADeepAgent::Client->SendMetadata(ADeepAgent::NumberSensor, ADeepAgent::NumberActions);
	ADeepAgent::RestartGame();
	UE_LOG(LogTemp, Error, TEXT("Begin"));
}

float timer;
void ADeepAgent::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	timer += DeltaTime;
	if (timer > ADeepAgent::TickTime) {//20 fps
		//ADeepAgent::Step();
		timer = 0;
	}
	//ADeepAgent::PerformAction(ADeepAgent::Action);

	//UE_LOG(LogTemp, Error, TEXT("Line trace has hit: %f"), DeltaTime);
}

void ADeepAgent::SetAction(int action)
{
	ADeepAgent::Action = action;
}
void ADeepAgent::SetConfidence(float confidence)
{
	ADeepAgent::Confidence = confidence;
}
void ADeepAgent::SetIsGameEnded(bool value)
{
	ADeepAgent::IsGameEnded = value;
}
void ADeepAgent::ToggleIsTraining()
{
	ADeepAgent::IsTraining = !ADeepAgent::IsTraining;
	if (ADeepAgent::IsTraining) {
		ADeepAgent::TickTime = 0.005;
	}
	else
		ADeepAgent::TickTime = 0.01;
}

float ADeepAgent::GetEpsilon()
{
	return ADeepAgent::Epsilon;
}
float ADeepAgent::GetReward()
{
	return ADeepAgent::Reward;
}
float ADeepAgent::GetCumulativeReward()
{
	return ADeepAgent::CumulativeReward;
}
int ADeepAgent::GetNumberSteps() {
	return ADeepAgent::NumberSteps;
}
int ADeepAgent::GetEpoch() {
	return ADeepAgent::Epoch;
}
TArray<int> ADeepAgent::GetInput() {
	TArray<int> currentSate;

	FVector Start;
	FRotator Rot;

	Start = ADeepAgent::SensorPosition->GetComponentLocation();
	Rot = ADeepAgent::SensorPosition->GetComponentRotation(); 
	ADeepAgent::totalDistance = 0;

	TArray<FHitResult, TFixedAllocator<ADeepAgent::NumberSensor>> Hit;
	for (int i = 0; i < ADeepAgent::NumberSensor; i++)
	{
		FHitResult hit;
		Hit.Init(hit, ADeepAgent::NumberSensor);
	}
	float AngleExtension = 220;
	FCollisionQueryParams FParams;
	
	float maxDistance = 1000;
	for (int i = 0; i < ADeepAgent::NumberSensor; i++)
	{
		FRotator angle = FRotator::ZeroRotator; 
		angle.Yaw = -AngleExtension/2 + AngleExtension / (ADeepAgent::NumberSensor-1) * i;
		FVector End = Start + (Rot+angle).Vector() * maxDistance;
		GetWorld()->LineTraceSingleByChannel(Hit[i], Start, End, ECollisionChannel::ECC_WorldStatic, FParams);


		AActor* ActorHit = Hit[i].GetActor();

		float lifeTime = 0.06;
		if (ActorHit) {
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, lifeTime, 0, 5);
			currentSate.Add(0);
			//ADeepAgent::totalDistance += Hit[i].Distance - maxDistance*3/4;
			//UE_LOG(LogTemp, Error, TEXT("Line trace has hit: %d %s"),i, *(ActorHit->GetName()));
		}
		else {
			ADeepAgent::totalDistance += maxDistance;
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, lifeTime, 0, 5);
			currentSate.Add(1);
		}
	}

	return currentSate;
}
bool ADeepAgent::GetIsTraining()
{
	return ADeepAgent::IsTraining;
}

void ADeepAgent::SendExperience(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool endGame)
{
	ADeepAgent::Client->SendExperience(currentState, action, nextState, ADeepAgent::Reward, endGame);
}

void ADeepAgent::PerformAction(int action)
{
	//1) 3 actions
	/*switch (ADeepAgent::Action)
	{
	case 0:
		break;
	case 1:
		ADeepAgent::MovementComponent->SetSteeringInput(1);
		ADeepAgent::MovementComponent->SetThrottleInput(0.2);
		break;
	case 2:
		ADeepAgent::MovementComponent->SetSteeringInput(-1);
		ADeepAgent::MovementComponent->SetThrottleInput(0.2);
		break;
	default:
		break;
	}*/

	//2) 2 actions
	/*ADeepAgent::MovementComponent->SetThrottleInput(0.7);
	switch (ADeepAgent::Action)
	{
	case 0:
		ADeepAgent::MovementComponent->SetSteeringInput(1);
		break;
	case 1:
		ADeepAgent::MovementComponent->SetSteeringInput(-1);
		break;
	default:
		break;
	}*/

	//3) 5 Actions
	switch (ADeepAgent::Action)
	{
	case 0:
		ADeepAgent::MovementComponent->SetThrottleInput(0.2);
		ADeepAgent::MovementComponent->SetSteeringInput(-1);
		break;
	case 1:
		ADeepAgent::MovementComponent->SetThrottleInput(0.6);
		ADeepAgent::MovementComponent->SetSteeringInput(-0.5);
		break;
	case 2:
		ADeepAgent::MovementComponent->SetThrottleInput(1);
		break;
	case 3:
		ADeepAgent::MovementComponent->SetThrottleInput(0.6);
		ADeepAgent::MovementComponent->SetSteeringInput(0.5);
		break;
	case 4:
		ADeepAgent::MovementComponent->SetThrottleInput(0.2);
		ADeepAgent::MovementComponent->SetSteeringInput(1);
		break;
	default:
		break;
	}
}

void ADeepAgent::Step()
{
	//CURRENT STATE FROM INPUT SENSORS
	TArray<int> currentState = ADeepAgent::GetInput();

	//EXPLORATION - EXPLOITATION TRADEOFF
	float exploitation = FMath::SRand();
	if (exploitation > ADeepAgent::Epsilon || !ADeepAgent::IsTraining) {
		//Chose best action
		ADeepAgent::Client->Predict(currentState);
		UE_LOG(LogTemp, Error, TEXT("Best Action chosen: %d"), ADeepAgent::Action);
	}
	else {
		//Choose random action
		ADeepAgent::Action = FMath::RandRange(0,ADeepAgent::NumberActions-1);
		if (ADeepAgent::Epsilon > ADeepAgent::MinEpsilon) {
			ADeepAgent::Epsilon -= ADeepAgent::EpsilonDecay;
		}
		else {
			ADeepAgent::TickTime /= 2.0;
			ADeepAgent::NumberFitSteps *= 2;
		}
		//else ADeepAgent::Epsilon = 0;
		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT("Epsilon: %f"), ADeepAgent::Epsilon) );*/
		UE_LOG(LogTemp, Error, TEXT("Random Action chosen: %d"), ADeepAgent::Action);
	}

	//PERFORM ACTION
	ADeepAgent::PerformAction(ADeepAgent::Action);

	//NEXT STATE
	TArray<int> nextState = ADeepAgent::GetInput();

	//GAME STATUS
	//bool gameStatus = ADeepAgent::GameStatus;

	//ADeepAgent::Reward
	//float ADeepAgent::Reward = 0.1;
	
	//ADeepAgent::Reward = ADeepAgent::GetMesh()->GetPhysicsLinearVelocity().Size()/2000.0;

	//ADeepAgent::Reward by number of green rays
	ADeepAgent::Reward = -1;
	for (int i = 0; i < currentState.Num(); i++)
	{
		if (currentState[i]==1)
		{
			ADeepAgent::Reward++;
		}
	}
	ADeepAgent::Reward = ADeepAgent::Reward / 10;
	//multiplied by velocity (punish if the velocity is under a given threshold)
	ADeepAgent::Reward *= (ADeepAgent::GetMesh()->GetPhysicsLinearVelocity().Size() - 90) / 1000;

	//No ADeepAgent::Reward (only negative ADeepAgent::Reward when hitting a wall)
	//float ADeepAgent::Reward = 0;

	//Distance from walls ADeepAgent::Reward
	//float ADeepAgent::Reward = (ADeepAgent::totalDistance) /10000.0;
	UE_LOG(LogTemp, Error, TEXT("%f %f"),ADeepAgent::Reward, ADeepAgent::GetMesh()->GetPhysicsLinearVelocity().Size());

	//UE_LOG(LogTemp, Error, TEXT("%f %f %f"),ADeepAgent::Reward, ADeepAgent::GetMesh()->GetComponentVelocity().Size(), );
	if (ADeepAgent::IsGameEnded) {
		ADeepAgent::Reward = -10000;
	}
	ADeepAgent::CumulativeReward += ADeepAgent::Reward;

	if (ADeepAgent::IsTraining) {
		Experience currentExp = Experience(currentState, ADeepAgent::Action, nextState, ADeepAgent::Reward, IsGameEnded);
		if ((!ADeepAgent::PreviousExperience.GetInitilized() ||
			!ADeepAgent::PreviousExperience.CheckEqualExperiences(currentExp))) {
			//SEND EXPERIENCE
			counter++;
			if (counter % ADeepAgent::NumberFitSteps == 0)//&& (ADeepAgent::Epsilon > ADeepAgent::MinEpsilon)
				ADeepAgent::SendExperience(currentState, ADeepAgent::Action, nextState, ADeepAgent::Reward, ADeepAgent::IsGameEnded);
		}

		if (!ADeepAgent::PreviousExperience.GetInitilized()) {
			ADeepAgent::PreviousExperience.InitializeExperience(currentState, ADeepAgent::Action, nextState, ADeepAgent::Reward, IsGameEnded);
		}
		else {
			ADeepAgent::PreviousExperience = currentExp;
		}
	}

	ADeepAgent::NumberSteps++;
	if (ADeepAgent::IsGameEnded || ADeepAgent::NumberSteps>=ADeepAgent::MaxNumberSteps) {
		ADeepAgent::RestartGame();
	}
}

void ADeepAgent::RestartGame()
{
	ADeepAgent::MovementComponent->StopMovementImmediately();
	FVector pos = ADeepAgent::initialTransform.GetLocation();
	float offset = 200;
	pos.X += FMath::FRandRange(-offset*2, offset);
	pos.Y += FMath::FRandRange(-offset, offset);
	this->GetMesh()->SetAllPhysicsPosition(pos);

	//bool changeDirection = FMath::RandBool();
	FQuat rot = ADeepAgent::initialTransform.GetRotation();
	/*if (changeDirection) {
		rot += FQuat::Rotator(FRotator(0,180,0));
	}*/
	this->GetMesh()->SetAllPhysicsRotation( rot );

	this->GetMesh()->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	this->GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	ADeepAgent::MovementComponent->SetThrottleInput(1);

	ADeepAgent::Epoch++;
	ADeepAgent::NumberSteps = 0;
	ADeepAgent::CumulativeReward = 0;
	ADeepAgent::IsGameEnded = false;
	UE_LOG(LogTemp, Error, TEXT("Restart"));
}

void ADeepAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Input1", IE_Pressed, this, &ADeepAgent::Restart);
}