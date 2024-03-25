// Fill out your copyright notice in the Description page of Project Settings.


#include "PositionGenerator.h"


#include "Math/UnrealMathUtility.h"
PositionGenerator::PositionGenerator()
{
}

PositionGenerator::~PositionGenerator()
{
}

void PositionGenerator::Generate()
{

	for (int i = 0; i < numPos; i++) {
		positions.Add(FVector(FMath::FRandRange(0, limit), FMath::FRandRange(0, limit), 0));
	}

	FString filePath = FPaths::ProjectContentDir() + TEXT("TestPositions.txt");
	FString forOutput;


	for (const FVector& position : positions) {

		forOutput += TEXT("\n");
	}

	FFileHelper::SaveStringToFile(forOutput, *filePath);
}
