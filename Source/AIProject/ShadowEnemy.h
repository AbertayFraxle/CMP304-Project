// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/PointLightComponent.h"
#include "Engine/PointLight.h"
#include "ShadowEnemy.generated.h"

#define ACTION_NUM 4
#define STATE_NUM 8

enum State {TARGET_AHEAD_LEFT, TARGET_AHEAD_RIGHT,TARGET_BEHIND_LEFT,TARGET_BEHIND_RIGHT,TARGET_AHEAD,TARGET_BEHIND,TARGET_LEFT,TARGET_RIGHT};
enum Action {MOVE_FORWARD,MOVE_BACKWARDS,MOVE_LEFT,MOVE_RIGHT};//{PURSUE_PLAYER,RETREAT};

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TArray<float>> Q;


	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	AActor* player;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void chooseState();
	void chooseAction();
	
	float getMax();
};
