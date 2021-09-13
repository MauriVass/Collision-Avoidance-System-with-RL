// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawDebugHelpers.h"
#include "DeepAgent.h"


void ADeepAgent::BeginPlay() {
	Super::BeginPlay();

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TArray<USceneComponent*> Childrens;
	this->GetMesh()->GetChildrenComponents(true, Childrens);
	/*UE_LOG(LogTemp, Error, TEXT("Number Children %d"), acc.Num());
	for (int i = 0; i < acc.Num(); i++)
	{
		UE_LOG(LogTemp, Error, TEXT("Children name %d %s"),i, *acc[i]->GetName());
	}*/
	if (Childrens.Num() > 0)
		ADeepAgent::SensorPosition = Childrens[1];
}

void ADeepAgent::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	ADeepAgent::GetInput();
}

void ADeepAgent::GetInput() {
	FVector Start;
	FRotator Rot;

	//GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(Start, Rot);
	Start = ADeepAgent::SensorPosition->GetComponentLocation();
	Rot = ADeepAgent::SensorPosition->GetComponentRotation();

	float AngleExtension = 90;
	TArray<FHitResult,TFixedAllocator<ADeepAgent::NumberSensor>> Hit;
	for (int i = 0; i < ADeepAgent::NumberSensor; i++)
	{
		FHitResult hit;
		Hit.Init(hit, ADeepAgent::NumberSensor);
	}
	FCollisionQueryParams FParams;

	for (int i = 0; i < ADeepAgent::NumberSensor; i++)
	{
		FRotator angle = FRotator::ZeroRotator; 
		angle.Yaw = -AngleExtension + AngleExtension * 2 / (ADeepAgent::NumberSensor-1) * i;
		FVector End = Start + (Rot.Vector()+angle.Vector()) * 2000;
		GetWorld()->LineTraceSingleByChannel(Hit[i], Start, End, ECollisionChannel::ECC_Visibility, FParams);


		AActor* ActorHit = Hit[i].GetActor();

		float lifeTime = 0.3;
		if (ActorHit) {
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, lifeTime, 0, 10);
			//UE_LOG(LogTemp, Error, TEXT("Line trace has hit: %d %s"),i, *(ActorHit->GetName()));
		}
		else {
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, lifeTime, 0, 10);
		}
	}
}

void ADeepAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Input", IE_Pressed, this, &ADeepAgent::GetInput);
}