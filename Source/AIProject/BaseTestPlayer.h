// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseTestPlayer.generated.h"

UCLASS()
class AIPROJECT_API ABaseTestPlayer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseTestPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* measure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> times;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> positions;
	
	//number of times to test
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int numPos;

	//size of arena
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float limit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float totalTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float nearTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float lightTime;

	UFUNCTION(BlueprintCallable)
	void Generate();
	UFUNCTION(BlueprintCallable)
	void LoadPos();

	UFUNCTION(BlueprintCallable)
	void SaveTimes();


};
