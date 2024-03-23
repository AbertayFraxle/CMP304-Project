// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "PlayerCharacter.generated.h"

UCLASS()
class AIPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* playerCam;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* cameraBoom;


	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* moveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* lookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputMappingContext* inputMappingContext;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
