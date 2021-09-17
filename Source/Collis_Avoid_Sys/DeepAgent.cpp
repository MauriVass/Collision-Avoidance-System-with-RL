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

	ADeepAgent::NumberActions = 3;
	ADeepAgent::Epsilon = 1.0;
	ADeepAgent::EpsilonDecay = 1 * FMath::Pow(10,-4);
	ADeepAgent::MinEpsilon = 0.1;
	ADeepAgent::MaxNumberSteps = 10000;

	ADeepAgent::RestartGame();
	UE_LOG(LogTemp, Error, TEXT("Begin"));
}

float timer;
void ADeepAgent::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	timer += DeltaTime;
	if (timer > 0.05 || true) {
		ADeepAgent::Step();
		timer = 0;
	}

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

TArray<int> ADeepAgent::GetInput() {
	TArray<int> currentSate;

	FVector Start;
	FRotator Rot;

	Start = ADeepAgent::SensorPosition->GetComponentLocation();
	Rot = ADeepAgent::SensorPosition->GetComponentRotation();

	TArray<FHitResult, TFixedAllocator<ADeepAgent::NumberSensor>> Hit;
	for (int i = 0; i < ADeepAgent::NumberSensor; i++)
	{
		FHitResult hit;
		Hit.Init(hit, ADeepAgent::NumberSensor);
	}
	float AngleExtension = 160;
	FCollisionQueryParams FParams;
	
	for (int i = 0; i < ADeepAgent::NumberSensor; i++)
	{
		FRotator angle = FRotator::ZeroRotator; 
		angle.Yaw = -AngleExtension/2 + AngleExtension / (ADeepAgent::NumberSensor-1) * i;
		FVector End = Start + (Rot+angle).Vector() * 1200;
		GetWorld()->LineTraceSingleByChannel(Hit[i], Start, End, ECollisionChannel::ECC_WorldStatic, FParams);


		AActor* ActorHit = Hit[i].GetActor();

		float lifeTime = 0.1;
		if (ActorHit) {
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, lifeTime, 0, 5);
			currentSate.Add(1);
			//UE_LOG(LogTemp, Error, TEXT("Line trace has hit: %d %s"),i, *(ActorHit->GetName()));
		}
		else {
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, lifeTime, 0, 5);
			currentSate.Add(0);
		}
	}

	return currentSate;
}

void ADeepAgent::SendExperience(TArray<int> currentState, int action, TArray<int> nextState, float reward, bool endGame)
{
	ADeepAgent::Client->SendExperience(currentState, action, nextState, reward, endGame);
}

void ADeepAgent::Step()
{
	//CURRENT STATE FROM INPUT SENSORS
	TArray<int> currentState = ADeepAgent::GetInput();

	//EXPLORATION - EXPLOITATION TRADEOFF
	float exploitation = FMath::SRand();
	if (exploitation > ADeepAgent::Epsilon) {
		//chose best action
		if(counter%2==0)
			ADeepAgent::Client->Predict(currentState);
		UE_LOG(LogTemp, Error, TEXT("Best Action chosen: %d"), ADeepAgent::Action);
	}
	else {
		//Choose random action
		ADeepAgent::Action = FMath::RandRange(0,ADeepAgent::NumberActions-1);
		if(ADeepAgent::Epsilon>ADeepAgent::MinEpsilon)
			ADeepAgent::Epsilon -= ADeepAgent::EpsilonDecay;
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT("Epsilon: %f"), ADeepAgent::Epsilon) );
		UE_LOG(LogTemp, Error, TEXT("Random Action chosen: %d"), ADeepAgent::Action);
	}

	//PERFORM ACTION
	switch (ADeepAgent::Action)
	{
	case 0:
		ADeepAgent::MovementComponent->SetThrottleInput(1);
		break;
	case 1:
		ADeepAgent::MovementComponent->SetSteeringInput(1);
		break;
	case 2:
		ADeepAgent::MovementComponent->SetSteeringInput(-1);
		break;
	default:
		break;
	}

	//NEXT STATE
	TArray<int> nextState = ADeepAgent::GetInput();

	//GAME STATUS
	//bool gameStatus = ADeepAgent::GameStatus;

	//REWARD
	//float reward = 0.1;
	//float reward = ADeepAgent::GetMesh()->GetPhysicsLinearVelocity().Size()/10.0;
	float reward = -1;
	for (int i = 0; i < currentState.Num(); i++)
	{
		if (currentState[i]==0)
		{
			reward++;
		}
	}
	reward /= 10;
	//UE_LOG(LogTemp, Error, TEXT("%f %f %f"),reward, ADeepAgent::GetMesh()->GetComponentVelocity().Size(), );
	if (ADeepAgent::IsGameEnded) {
		reward = -10;
	}

	Experience currentExp = Experience(currentState, ADeepAgent::Action, nextState, reward, IsGameEnded);
	if (!ADeepAgent::PreviousExperience.GetInitilized() || !ADeepAgent::PreviousExperience.CheckEqualExperiences(currentExp)) {
		//SEND EXPERIENCE
		counter++;
		if(counter%6==0)
			ADeepAgent::SendExperience(currentState, ADeepAgent::Action, nextState, reward, ADeepAgent::IsGameEnded);
	}

	if (!ADeepAgent::PreviousExperience.GetInitilized()) {
		ADeepAgent::PreviousExperience.InitializeExperience(currentState, ADeepAgent::Action, nextState, reward, IsGameEnded);
	}
	else{
		ADeepAgent::PreviousExperience = currentExp;
	}

	ADeepAgent::NumberSteps++;
	if (ADeepAgent::IsGameEnded || ADeepAgent::NumberSteps>=ADeepAgent::MaxNumberSteps) {
		ADeepAgent::RestartGame();
	}
}

void ADeepAgent::RestartGame()
{
	ADeepAgent::MovementComponent->StopMovementImmediately();
	this->GetMesh()->SetAllPhysicsPosition(ADeepAgent::initialTransform.GetLocation());
	this->GetMesh()->SetAllPhysicsRotation(ADeepAgent::initialTransform.GetRotation());
	ADeepAgent::MovementComponent->SetThrottleInput(1);

	ADeepAgent::NumberSteps = 0;
	ADeepAgent::IsGameEnded = false;
	UE_LOG(LogTemp, Error, TEXT("Restart"));
}

void ADeepAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Input1", IE_Pressed, this, &ADeepAgent::Restart);
}