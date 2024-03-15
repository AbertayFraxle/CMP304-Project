// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


#include "Misc/FileHelper.h"

// Sets default values
AShadowEnemy::AShadowEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShadowEnemy::BeginPlay()
{
	Super::BeginPlay();

	srand(time(NULL));

	TArray<float> qEntry;
	qEntry.Init(0, ACTION_NUM);
	Q.Init(qEntry, STATE_NUM);

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APointLight::StaticClass(), pointLights);
	
	mesh = FindComponentByClass<USkeletalMeshComponent>();

	TArray<FName> allSocks = mesh->GetAllSocketNames();

	for (FName name : allSocks) {

		if (name.ToString().Contains(FString("socket"))) {
			socketNames.AddUnique(name);
		}

	}

	//stored distance from the player
	targDist = FVector(GetActorLocation() - player->GetActorLocation()).Length();
}

// Called every frame
void AShadowEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	inLight = 0;

	inLights.Empty();

	for (int i = 0; i < (pointLights.Num()); i++) {


		UPointLightComponent* current = pointLights[i]->GetComponentByClass<UPointLightComponent>();

		for (int j = 0; j < (socketNames.Num()); j++) {


			FVector cLoc = mesh->GetSocketLocation(socketNames[j]);

			FVector dir = cLoc - pointLights[i]->GetActorLocation();
			float dist = dir.Length();
			dir.Normalize();


			if (dist <= current->AttenuationRadius) {

				FHitResult hit;
				GetWorld()->LineTraceSingleByChannel(hit, cLoc, pointLights[i]->GetActorLocation(), ECC_Visibility);
				//DrawDebugLine(GetWorld(), cLoc, pointLights[i]->GetActorLocation(), FColor::Red, false, 1.f);

				if (hit.bBlockingHit == 0) {
					inLights.AddUnique(pointLights[i]);
					inLight++;
				}

			}
		}


	}

	chooseState();
	chooseAction();

	switch (cAction) {
		/*case Action::PURSUE_PLAYER: {

			FVector direction = player->GetActorLocation() - GetActorLocation();
			direction.Z = 0;
			direction.Normalize();

			AddMovementInput(direction);

			break;
		}
		case Action::RETREAT: {

			float lowDist = 100000000;
			int lowIndex = 0;
			for (int i = 0; i < inLights.Num();i++) {
				float currDist = FVector(GetActorLocation() - inLights[i]->GetActorLocation()).Length();
				if (currDist < lowDist) {
					lowDist = currDist;
					lowIndex = i;
				}

			}

			if (inLights.IsValidIndex(lowIndex) ){
				FVector dir = GetActorLocation() - inLights[lowIndex]->GetActorLocation();
				dir.Z = 0;
				dir.Normalize();
				AddMovementInput(dir);
			}

			break;
		}
		*/
	case Action::MOVE_FORWARD: {
		FVector dir = FVector(1, 0, 0);
		AddMovementInput(dir);
		break;
	}
	case Action::MOVE_BACKWARDS: {
		FVector dir = FVector(-1, 0, 0);
		AddMovementInput(dir);
		break;
	}
	case Action::MOVE_LEFT: {
		FVector dir = FVector(0, -1, 0);
		AddMovementInput(dir);
		break;
	}
	case Action::MOVE_RIGHT: {
		FVector dir = FVector(0, 1, 0);
		AddMovementInput(dir);
		break;
	}
	}
	float calcReward = 0;

	if (FVector(GetActorLocation() - player->GetActorLocation()).Length() < targDist){
		calcReward = 40;
	}
	else {
		calcReward = -40;
	}

	calcReward -= (inLight * 10);

	reward += calcReward;

	Q[cState][cAction] = Q[cState][cAction] + 0.1 * (calcReward + (0.8 * getMax()) - Q[cState][cAction]);

	targDist = FVector(GetActorLocation() - player->GetActorLocation()).Length();
	float print = inLight;
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString::Printf(TEXT("inLight equals %f"), print));
}

// Called to bind functionality to input
void AShadowEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AShadowEnemy::chooseState()
{
	FVector targLoc = player->GetActorLocation();
	FVector thisLoc= GetActorLocation();
	
	if (targLoc.X > thisLoc.X) {
		if (targLoc.Y > thisLoc.Y) {
			cState = State::TARGET_AHEAD_RIGHT;
		}
		else if (targLoc.Y < thisLoc.Y) {
			cState = State::TARGET_AHEAD_LEFT;
		}
		else {
			cState = State::TARGET_AHEAD;
		}
	}
	else if (targLoc.X < thisLoc.X) {
		if (targLoc.Y > thisLoc.Y) {
			cState = State::TARGET_BEHIND_RIGHT;
		}
		else if (targLoc.Y < thisLoc.Y) {
			cState = State::TARGET_BEHIND_LEFT;
		}
		else {
			cState = State::TARGET_BEHIND;
		}
	}
	else {
		if (targLoc.Y > thisLoc.Y) {
			cState = State::TARGET_RIGHT;
		}
		else if (targLoc.Y < thisLoc.Y) {
			cState = State::TARGET_LEFT;
		}
	}
}

void AShadowEnemy::chooseAction()
{
	float bestValue = Q[cState][0];
	int bestAction = 0;

	int random = rand() % 100;

	if (random > 5) {
		
		for (int i = 0; i < ACTION_NUM; i++) {
			if (Q[cState][i] > bestValue) {
				bestValue = Q[cState][i];
				bestAction = i;
			}
		}
	}
	else {
		cAction = Action(rand()%ACTION_NUM);
		return;
	}
	cAction = Action(bestAction);

}

float AShadowEnemy::getMax() {

	float max = 0;

	for (int i = 0; i < ACTION_NUM; i++) {
		if (Q[cState][i] > max) {
			max = Q[cState][i];
		}
	}

	return max;
}

