// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create camera for player to use
	playerCam = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));

	//create the spring arm component to attach to the camera for third person camera
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	cameraBoom->TargetArmLength = 500;

	//attach camera arm to root component, set variables to be usable with CharacterMovementController
	cameraBoom->SetupAttachment(RootComponent);
	cameraBoom->bInheritPitch = true;
	cameraBoom->bInheritYaw = true;
	cameraBoom->bUsePawnControlRotation = true;

	playerCam->SetupAttachment(cameraBoom);

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//get the player's controller and set up Enhanced Input mapping context using it
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(inputMappingContext, 0);
		}
	}

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//bind all functions to the input actions
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		EnhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

	}
}

void APlayerCharacter::Move(const FInputActionValue& value)
{
	FVector2D MovementVector = value.Get<FVector2D>();
	MovementVector.Normalize();
	//if not dashing, add the input vector to the movement component to move player

	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
	
}

void APlayerCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookVector = value.Get<FVector2D>();

	//use vector from mouse input to the camera
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);

}
