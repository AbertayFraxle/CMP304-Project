// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/PointLightComponent.h"
#include "Engine/PointLight.h"
#include "ShadowEnemy.generated.h"

#define ACTION_NUM 5
#define STATE_NUM 12

#define LEARNING_RATE 0.5
#define DISCOUNT_RATE 0.9

enum State {TARGET_AHEAD_LEFT_NO_LIGHT, TARGET_AHEAD_RIGHT_NO_LIGHT,TARGET_BEHIND_LEFT_NO_LIGHT,TARGET_BEHIND_RIGHT_NO_LIGHT, TARGET_AHEAD_LEFT_LOW_LIGHT, TARGET_AHEAD_RIGHT_LOW_LIGHT, TARGET_BEHIND_LEFT_LOW_LIGHT, TARGET_BEHIND_RIGHT_LOW_LIGHT, TARGET_AHEAD_LEFT_HIGH_LIGHT, TARGET_AHEAD_RIGHT_HIGH_LIGHT, TARGET_BEHIND_LEFT_HIGH_LIGHT, TARGET_BEHIND_RIGHT_HIGH_LIGHT};
enum Action {MOVE_FORWARD,MOVE_BACKWARDS,MOVE_LEFT,MOVE_RIGHT,MOVE_TOWARDS_PLAYER};//{PURSUE_PLAYER,RETREAT};

UCLASS()
class AIPROJECT_API AShadowEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShadowEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<AActor*> pointLights;
	TArray<FName> socketNames;
	TArray<AActor*> inLights;
	USkeletalMeshComponent* mesh;
	UINT8 inLight;
	State cState;
	Action cAction;

	bool playerInLight;

	float targDist;

	float reward;
	float timer;

	FVector storedPos;

	TArray<TArray<float>> Q;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float randomChance;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	AActor* player;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void chooseState();
	void chooseAction();

	void SaveQToFile();
	void LoadQFromFile();

	void PrintAction();
	
	void CalculateReward(float & calcReward);

	float getMax();
};
