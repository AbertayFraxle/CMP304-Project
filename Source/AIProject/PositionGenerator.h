// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class AIPROJECT_API PositionGenerator
{
public:
	PositionGenerator();
	~PositionGenerator();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FVector> positions;

	int numPos;
	float limit;
	void Generate();
};
