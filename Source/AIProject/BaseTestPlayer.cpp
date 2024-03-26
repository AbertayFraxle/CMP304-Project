// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTestPlayer.h"
#include "Misc/DateTime.h"
// Sets default values
ABaseTestPlayer::ABaseTestPlayer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	numPos = 10;
	limit = 4000;

	
}

// Called when the game starts or when spawned
void ABaseTestPlayer::BeginPlay()
{
	Super::BeginPlay();
	times.Init(0.f, numPos);
}

// Called every frame
void ABaseTestPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseTestPlayer::Generate() {

	//generate positions to use in test and write to file
	for (int i = 0; i < numPos; i++) {
		positions.Add(FVector(FMath::FRandRange(0, limit), FMath::FRandRange(0, limit), 0));
	}

	FString filePath = FPaths::ProjectContentDir() + TEXT("TestPositions.txt");
	FString forOutput;

	for (const FVector& position : positions) {
		forOutput +=position.ToString() + TEXT("\n");
	}

	FFileHelper::SaveStringToFile(forOutput, *filePath);
	
}

void ABaseTestPlayer::LoadPos() {

	//load predetermined positions from file, or generate them if not there
	FString filePath = FPaths::ProjectContentDir() + TEXT("TestPositions.txt");
	FString forInput;

	if (FPaths::FileExists(filePath)) {
		FFileHelper::LoadFileToString(forInput, *filePath);

		TArray<FString> lines;
		forInput.ParseIntoArrayLines(lines);

		positions.Empty();

		for (const FString& line : lines) {
		
			FVector vector;
			vector.InitFromString(line);
			positions.Add(vector);
		}
	}
	else {
		Generate();
	}
}

void ABaseTestPlayer::SaveTimes()
{
	//output the times the testing took to the content dir
	FString filePath = FPaths::ProjectContentDir() + TEXT("Timings") + FDateTime::Now().ToString() + TEXT(".txt");
	FString forOutput;
	int count = 1;


	for (float cTime : times) {
		forOutput += TEXT("Time ")  + FString::FromInt(count) + TEXT(" ")+ FString::SanitizeFloat(cTime) + TEXT("\n");
		count++;
	}

	forOutput += TEXT("Total time ") + FString::SanitizeFloat(totalTime) + TEXT("\n");
	forOutput += TEXT("Near time ") + FString::SanitizeFloat(nearTime) + TEXT("\n");
	forOutput += TEXT("InLight time ") + FString::SanitizeFloat(lightTime) + TEXT("\n");

	FFileHelper::SaveStringToFile(forOutput, *filePath);
}

