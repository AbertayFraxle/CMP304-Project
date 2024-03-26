// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/PointLightComponent.h"
#include "Engine/PointLight.h"
#include "ShadowEnemy.generated.h"

#define ACTION_NUM 5
#define STATE_NUM 12


enum State {TARGET_AHEAD_LEFT_NO_LIGHT, TARGET_AHEAD_RIGHT_NO_LIGHT,TARGET_BEHIND_LEFT_NO_LIGHT,TARGET_BEHIND_RIGHT_NO_LIGHT, TARGET_AHEAD_LEFT_LOW_LIGHT, TARGET_AHEAD_RIGHT_LOW_LIGHT, TARGET_BEHIND_LEFT_LOW_LIGHT, TARGET_BEHIND_RIGHT_LOW_LIGHT, TARGET_AHEAD_LEFT_HIGH_LIGHT, TARGET_AHEAD_RIGHT_HIGH_LIGHT, TARGET_BEHIND_LEFT_HIGH_LIGHT, TARGET_BEHIND_RIGHT_HIGH_LIGHT};
enum Action {MOVE_FORWARD,MOVE_BACKWARDS,MOVE_LEFT,MOVE_RIGHT,MOVE_TOWARDS_PLAYER};

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

	//all the lights in scene
	TArray<AActor*> pointLights;

	//names of sockets to count if in the light
	TArray<FName> socketNames;

	//array to store lights that the AI is within
	TArray<AActor*> inLights;

	//reference to skeleton mesh
	USkeletalMeshComponent* mesh;

	//count of how many points of the AI are within light
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int inLight;

	//current state and current action
	State cState;
	Action cAction;

	//storing the distance from the target
	float targDist;

	//reward to motivate AI
	float reward;

	//timer for saving Q matrix
	float timer;

	//learning matrix, for AI to weigh options
	TArray<TArray<float>> Q;

	//player editable learning rate
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float learningRate;

	//player editable discount rate
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float discountRate;

	//player editable chance to perform random action
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float randomChance;

	//reference to the player to be pursued
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	AActor* player;

	//debug bool whether to print actions taken
	bool printActions;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//choose which state we're in
	void chooseState();

	//choose which action to perform
	void chooseAction();

	//saving and loading q matrix
	void SaveQToFile();
	void LoadQFromFile();

	//delete and reset q matrix
	UFUNCTION(BlueprintCallable)
	void ResetQ();

	//print action taken
	void PrintAction();
	
	//calculate the reward based on multiple checks
	void CalculateReward(float & calcReward);

	//get the maximum value of the Q matrix for current state
	float getMax();
};
