// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawDebugHelpers.h"
#include <Collis_Avoid_Sys/Experience.h>
#include "Client.h"
#include "DeepAgent.h"
#include "WheeledVehicleMovementComponent.h"
#include "Misc/FileHelper.h"

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

	ADeepAgent::ManualControll = false;

	ADeepAgent::IsTraining = true;
	ADeepAgent::Epoch = 0;
	ADeepAgent::IsActionSpaceDescrete = true;
	ADeepAgent::NumberActions = 5;
	ADeepAgent::Epsilon = 1.0;
	ADeepAgent::EpsilonDecay = 5 * FMath::Pow(10,-5);
	ADeepAgent::MinEpsilon = 0.05;
	ADeepAgent::MaxNumberSteps = 12000;
	ADeepAgent::NumberFitSteps = 1;

	ADeepAgent::TickTime = 0.025;

	ADeepAgent::Client->SendMetadata(ADeepAgent::NumberSensor, ADeepAgent::IsActionSpaceDescrete,ADeepAgent::NumberActions);
	ADeepAgent::RestartGame();
	UE_LOG(LogTemp, Error, TEXT("Begin"));

	//todo: remove full-path
	ADeepAgent::SaveDirectory = FString("C:/Users/mauri/Desktop/Mauri/software_per_programmazione/progetti/UnrealEngine/Collis_Avoid_Sys/CollisionAvoidanceSystem/Content/MyContent/Server");
	ADeepAgent::FileName = FString("run.rewards");
	ADeepAgent::WriteToFile(0,0,false,false);

	ADeepAgent::IsGameStarting = true;
	ADeepAgent::ToggleIsGameStarting();
}

float timer;
void ADeepAgent::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!ADeepAgent::ManualControll) {
		timer += DeltaTime;
		if (timer > ADeepAgent::TickTime) {
			ADeepAgent::Step();
			timer = 0;
		}
		ADeepAgent::PerformAction(ADeepAgent::Action);
	}

	UE_LOG(LogTemp, Error, TEXT("Speed: %f"), ADeepAgent::GetMesh()->GetPhysicsLinearVelocity().Size());
	//UE_LOG(LogTemp, Error, TEXT("Line trace has hit: %f"), DeltaTime);
}

void ADeepAgent::SetAction(int action)
{
	ADeepAgent::Action = action;
}
void ADeepAgent::SetThrottleAction(float throttle)
{
	ADeepAgent::ThrottleAction = throttle;
}
void ADeepAgent::SetSteerAction(float steer)
{
	ADeepAgent::SteerAction = steer;
}
void ADeepAgent::SetTickTime(float tickTime)
{
	ADeepAgent::TickTime = tickTime;
}
void ADeepAgent::SetManualControll(bool manualControll)
{
	ADeepAgent::ManualControll = manualControll;
}
void ADeepAgent::SetIsGameEnded(bool value)
{
	ADeepAgent::IsGameEnded = value;
}
void ADeepAgent::ToggleIsTraining()
{
	ADeepAgent::IsTraining = !ADeepAgent::IsTraining;
}
void ADeepAgent::ToggleIsGameStarting()
{
	ADeepAgent::IsGameStarting = !ADeepAgent::IsGameStarting;
	GetWorld()->GetFirstPlayerController()->SetPause(!ADeepAgent::IsGameStarting);
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
	ADeepAgent::TargetVector = FVector::ZeroVector;
	for (int i = 0; i < ADeepAgent::NumberSensor; i++)
	{
		FRotator angle = FRotator::ZeroRotator; 
		angle.Yaw = -AngleExtension/2 + AngleExtension / (ADeepAgent::NumberSensor-1) * i;
		FVector direction = (Rot + angle).Vector() * maxDistance;
		FVector end = Start + direction;
		GetWorld()->LineTraceSingleByChannel(Hit[i], Start, end, ECollisionChannel::ECC_WorldStatic, FParams);


		AActor* ActorHit = Hit[i].GetActor();

		float lifeTime = 0.06;
		if (ActorHit) {
			DrawDebugLine(GetWorld(), Start, end, FColor::Red, false, lifeTime, 0, 5);
			currentSate.Add(0);
			//ADeepAgent::totalDistance += Hit[i].Distance - maxDistance*3/4;
			//UE_LOG(LogTemp, Error, TEXT("Line trace has hit: %d %s"),i, *(ActorHit->GetName()));
		}
		else {
			ADeepAgent::totalDistance += maxDistance;
			DrawDebugLine(GetWorld(), Start, end, FColor::Green, false, lifeTime, 0, 5);
			currentSate.Add(1);

			ADeepAgent::TargetVector += direction;
		}
	}

	return currentSate;
}
bool ADeepAgent::GetIsTraining()
{
	return ADeepAgent::IsTraining;
}
float ADeepAgent::GetTickTime()
{
	return ADeepAgent::TickTime;
}

bool ADeepAgent::GetManualControll()
{
	return ADeepAgent::ManualControll;
}

bool ADeepAgent::GetIsActionSpaceDescrete()
{
	return ADeepAgent::IsActionSpaceDescrete;
}


void ADeepAgent::RewardFunction(TArray<int> currentState)
{
	//float ADeepAgent::Reward = 0.1;

	//##### Reward by magnitude velocity #####
	//ADeepAgent::Reward = ADeepAgent::GetMesh()->GetPhysicsLinearVelocity().Size()/2000.0;

	//##### Reward by number of green rays (not bad) #####
	//ADeepAgent::Reward = -1;
	//for (int i = 0; i < currentState.Num(); i++)
	//{
	//	if (currentState[i] == 1)
	//	{
	//		ADeepAgent::Reward++;
	//	}
	//}
	//ADeepAgent::Reward = ADeepAgent::Reward / 10;
	////multiplied by velocity (punish if the velocity is under a given threshold)
	//ADeepAgent::Reward *= (ADeepAgent::GetMesh()->GetPhysicsLinearVelocity().Size() - 90) / 1000;

	//##### No ADeepAgent::Reward (only negative ADeepAgent::Reward when hitting a wall) #####
	//float ADeepAgent::Reward = 0;
	
	//##### Distance from walls Reward #####
	//float ADeepAgent::Reward = (ADeepAgent::totalDistance) /10000.0;

	//##### Sum of green rays #####
	ADeepAgent::TargetVector /= ADeepAgent::TargetVector.Size();
	FVector start = ADeepAgent::SensorPosition->GetComponentLocation();
	FVector carDirection = ADeepAgent::SensorPosition->GetForwardVector();

	DrawDebugLine(GetWorld(), start, start + ADeepAgent::TargetVector * 2000, FColor::Orange, false, 0.1, 0, 5);
	DrawDebugLine(GetWorld(), start, start + carDirection * 2000, FColor::Blue, false, 0.1, 0, 5);

	float rewardDir = (FVector::DotProduct(ADeepAgent::TargetVector, carDirection) - 0.2) / 3;
	float rewardSpeed = (ADeepAgent::GetMesh()->GetPhysicsLinearVelocity().Size() - 200) / 1000;
	ADeepAgent::Reward = rewardDir + rewardSpeed;
	UE_LOG(LogTemp, Error, TEXT("%f %f"), rewardDir, rewardSpeed);

	//###### Angle between direction and target ######
	/*float angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(ADeepAgent::TargetVector, carDirection)));

	ADeepAgent::Reward = -0.1;
	if ( (angle>0 && ADeepAgent::Action>2) || 
		(angle < 0 && ADeepAgent::Action < 2) || 
		(FMath::Abs(angle) < 10 && ADeepAgent::Action == 2) )
	{
		ADeepAgent::Reward += 0.5;
	}
	ADeepAgent::Reward *= (ADeepAgent::GetMesh()->GetPhysicsLinearVelocity().Size() - 100) / 1000;*/

	//UE_LOG(LogTemp, Error, TEXT("%f %f"),ADeepAgent::Reward, ADeepAgent::GetMesh()->GetPhysicsLinearVelocity().Size());
	//UE_LOG(LogTemp, Error, TEXT("%f %f %f"),ADeepAgent::Reward, ADeepAgent::GetMesh()->GetComponentVelocity().Size(), );
	if (ADeepAgent::IsGameEnded) {
		ADeepAgent::Reward = -100;
	}
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

	//2 actions
	/*ADeepAgent::MovementComponent->SetThrottleInput(ADeepAgent::ThrottleAction+0.2);
	ADeepAgent::MovementComponent->SetSteeringInput(ADeepAgent::SteerAction);*/
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
		//UE_LOG(LogTemp, Error, TEXT("Best Action chosen: %f %f"), ADeepAgent::ThrottleAction, ADeepAgent::SteerAction);
	}
	else {
		//Choose random action
		if (ADeepAgent::IsActionSpaceDescrete) {
			ADeepAgent::Action = FMath::RandRange(0, ADeepAgent::NumberActions - 1);
		}
		else {//Choose random actions
			ADeepAgent::ThrottleAction = FMath::FRandRange(0, 1);
			ADeepAgent::SteerAction = FMath::FRandRange(-1, 1);
		}

		if (ADeepAgent::Epsilon > ADeepAgent::MinEpsilon) {
			ADeepAgent::Epsilon -= ADeepAgent::EpsilonDecay;
		}
		else {
			ADeepAgent::Epsilon = ADeepAgent::MinEpsilon;
			ADeepAgent::NumberFitSteps = 2;
		}
		//else ADeepAgent::Epsilon = 0;
		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT("Epsilon: %f"), ADeepAgent::Epsilon) );*/
		UE_LOG(LogTemp, Error, TEXT("Random Action chosen: %d"), ADeepAgent::Action);
		//UE_LOG(LogTemp, Error, TEXT("Random Action chosen: %f %f"), ADeepAgent::ThrottleAction, ADeepAgent::SteerAction);
	}

	//PERFORM ACTION
	ADeepAgent::PerformAction(ADeepAgent::Action);

	//NEXT STATE
	TArray<int> nextState = ADeepAgent::GetInput();

	//GAME STATUS
	//bool gameStatus = ADeepAgent::GameStatus;

	//ADeepAgent::Reward
	ADeepAgent::RewardFunction(currentState);
	ADeepAgent::CumulativeReward += ADeepAgent::Reward;

	//FIT NEURAL NETWORK WITH THIS EXPERIENCE
	if (ADeepAgent::IsTraining) {
		Experience currentExp = Experience(currentState, ADeepAgent::Action, nextState, ADeepAgent::Reward, IsGameEnded);
		if ((!ADeepAgent::PreviousExperience.GetInitilized() ||
			!ADeepAgent::PreviousExperience.CheckEqualExperiences(currentExp))) {
			//SEND EXPERIENCE
			counter++;
			try
			{
				if (counter % ADeepAgent::NumberFitSteps == 0)//&& (ADeepAgent::Epsilon > ADeepAgent::MinEpsilon)
					if(!ADeepAgent::IsActionSpaceDescrete)
						ADeepAgent::Client->SendExperience(currentState, ADeepAgent::ThrottleAction, ADeepAgent::SteerAction, nextState, ADeepAgent::Reward, ADeepAgent::IsGameEnded);
					else
						ADeepAgent::Client->SendExperience(currentState, ADeepAgent::Action, nextState, ADeepAgent::Reward, ADeepAgent::IsGameEnded);

			}
			catch (const std::exception&)
			{
				UE_LOG(LogTemp, Error, TEXT("%d %f"), ADeepAgent::NumberFitSteps, ADeepAgent::NumberFitSteps);
			}
			
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

	if(ADeepAgent::Epoch>0)
		ADeepAgent::WriteToFile(ADeepAgent::Epoch, ADeepAgent::CumulativeReward, ADeepAgent::IsGameEnded, true);
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

void ADeepAgent::WriteToFile(int epoch, float totalReward, bool gameEndedByCrush, bool allowOverwriting)
{
	FString TextToSave = FString("");
	TextToSave += FString::FromInt(epoch) + "," + FString::SanitizeFloat(totalReward) + "," + FString::FromInt(gameEndedByCrush);
	TextToSave += "\n";

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// CreateDirectoryTree returns true if the destination
	// directory existed prior to call or has been created
	// during the call.
	if (PlatformFile.CreateDirectoryTree(*ADeepAgent::SaveDirectory))
	{
		// Get absolute file path
		FString AbsoluteFilePath = ADeepAgent::SaveDirectory + "/" + ADeepAgent::FileName;

		//if (!PlatformFile.FileExists(*AbsoluteFilePath))
		{
			if(allowOverwriting)
				FFileHelper::SaveStringToFile(TextToSave, *AbsoluteFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
			else
				FFileHelper::SaveStringToFile(TextToSave, *AbsoluteFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_None);
		}
	}
}