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
	randomChance = 50;

	printActions = false;

	learningRate = 0.5;
	discountRate = 0.9;
}

// Called when the game starts or when spawned
void AShadowEnemy::BeginPlay()
{
	Super::BeginPlay();

	srand(time(NULL));

	//Initialise the Q matrix and load it from txt file
	TArray<float> qEntry;
	qEntry.Init(0, ACTION_NUM);
	Q.Init(qEntry, STATE_NUM);
	LoadQFromFile();

	//get all point light actors for the enemy to avoid
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APointLight::StaticClass(), pointLights);
	
	//get a reference to self skeleton
	mesh = FindComponentByClass<USkeletalMeshComponent>();

	//get the points on the skeleton to raycast to lights from
	TArray<FName> allSocks = mesh->GetAllSocketNames();

	//discard bones
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

	//reset counter of sockets in light
	inLight = 0;
	
	//empty array of lights enemy is inside
	inLights.Empty();

	//loop for all point lights in the scene
	for (int i = 0; i < (pointLights.Num()); i++) {

		//get point light component
		UPointLightComponent* current = pointLights[i]->GetComponentByClass<UPointLightComponent>();

		//loop for all sockets
		for (int j = 0; j < (socketNames.Num()); j++) {

			//check if the socket is within the attenuation radius of the light
			FVector cLoc = mesh->GetSocketLocation(socketNames[j]);
			FVector dir = cLoc - pointLights[i]->GetActorLocation();
			float dist = dir.Length();
			dir.Normalize();

			
			if (dist <= current->AttenuationRadius) {

				//do a raycast from the socket to the light, and if unbroken, increase the inLight variable and save which light was hit
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

	
	//determine which state to be in
	chooseState();

	//determine which action to use
	chooseAction();

	//execute action
	switch (cAction) {
	case Action::MOVE_FORWARD: {
		//add forward input
		FVector dir = FVector(1, 0, 0);
		AddMovementInput(dir);
		break;
	}
	case Action::MOVE_BACKWARDS: {
		//add backward input
		FVector dir = FVector(-1, 0, 0);
		AddMovementInput(dir);
		break;
	}
	case Action::MOVE_LEFT: {
		//add left input
		FVector dir = FVector(0, -1, 0);
		AddMovementInput(dir);
		break;
	}
	case Action::MOVE_RIGHT: {
		//add right input
		FVector dir = FVector(0, 1, 0);
		AddMovementInput(dir);
		break;
	}
	case Action::MOVE_TOWARDS_PLAYER: {
		//determine vector to point towards player and move in that direction
		FVector dir =  player->GetActorLocation() - GetActorLocation();
		dir.Normalize();
		AddMovementInput(dir);
		
		break;
	}
	}

	if (printActions) {
		//output which action taken to the screen
		PrintAction();
	}

	//calculate the reward for this action
	float calcReward = 0;
	CalculateReward(calcReward);

	//do Q learning, with defined learning rate and discount rate
	Q[cState][cAction] = Q[cState][cAction] + learningRate * (calcReward + (discountRate * getMax()) - Q[cState][cAction]);

	//update the saved distance from the target
	targDist = FVector(GetActorLocation() - player->GetActorLocation()).Length();


	//print the inLight value
	//float print = inLight;
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString::Printf(TEXT("inLight equals %f"), print));

	//increase timer and if above 10, reset timer and save Q array
	timer += DeltaTime;
	if (timer > 10) {
		timer = 0;
		SaveQToFile();
	}
}

// Called to bind functionality to input
void AShadowEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AShadowEnemy::chooseState()
{
	//determine the state based on the relative position of the player to the enemy, and how much light the enemy is in
	FVector targLoc = player->GetActorLocation();
	FVector thisLoc= GetActorLocation();
	
	if (inLight <= 0) {
		if (targLoc.X > thisLoc.X) {
			if (targLoc.Y > thisLoc.Y) {
				cState = State::TARGET_AHEAD_RIGHT_NO_LIGHT;
			}
			else if (targLoc.Y < thisLoc.Y) {
				cState = State::TARGET_AHEAD_LEFT_NO_LIGHT;
			}
		}
		else if (targLoc.X < thisLoc.X) {
			if (targLoc.Y > thisLoc.Y) {
				cState = State::TARGET_BEHIND_RIGHT_NO_LIGHT;
			}
			else if (targLoc.Y < thisLoc.Y) {
				cState = State::TARGET_BEHIND_LEFT_NO_LIGHT;
			}
		}
	}
	else if (inLight <= 4) {
		if (targLoc.X > thisLoc.X) {
			if (targLoc.Y > thisLoc.Y) {
				cState = State::TARGET_AHEAD_RIGHT_LOW_LIGHT;
			}
			else if (targLoc.Y < thisLoc.Y) {
				cState = State::TARGET_AHEAD_LEFT_LOW_LIGHT;
			}
		}
		else if (targLoc.X < thisLoc.X) {
			if (targLoc.Y > thisLoc.Y) {
				cState = State::TARGET_BEHIND_RIGHT_LOW_LIGHT;
			}
			else if (targLoc.Y < thisLoc.Y) {
				cState = State::TARGET_BEHIND_LEFT_LOW_LIGHT;
			}
		}
	}
	else {
		if (targLoc.X > thisLoc.X) {
			if (targLoc.Y > thisLoc.Y) {
				cState = State::TARGET_AHEAD_RIGHT_HIGH_LIGHT;
			}
			else if (targLoc.Y < thisLoc.Y) {
				cState = State::TARGET_AHEAD_LEFT_HIGH_LIGHT;
			}
		}
		else if (targLoc.X < thisLoc.X) {
			if (targLoc.Y > thisLoc.Y) {
				cState = State::TARGET_BEHIND_RIGHT_HIGH_LIGHT;
			}
			else if (targLoc.Y < thisLoc.Y) {
				cState = State::TARGET_BEHIND_LEFT_HIGH_LIGHT;
			}
		}
	}
}

void AShadowEnemy::chooseAction()
{
	//determine which value in the Q matrix has the largest reward assosciated with it for the state, with random chance to be completely random 
	float bestValue = Q[cState][0];
	int bestAction = 0;

	int random = rand() % 100;

	if (random > randomChance) {
		
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

void AShadowEnemy::SaveQToFile()
{
	//output the current Q matrix to Qmatrix.txt in the content folder
	FString filePath = FPaths::ProjectContentDir() + TEXT("QMatrix.txt");
	FString forOutput;

	for (const TArray<float>& InnerArray : Q) {
		for (float value : InnerArray) {
			forOutput += FString::SanitizeFloat(value) + TEXT(" ");
		}
		forOutput += TEXT("\n");
	}

	FFileHelper::SaveStringToFile(forOutput, *filePath);
}

void AShadowEnemy::LoadQFromFile()
{
	//input the saved Q matrix from QMatrix.txt to the Q matrix

	FString filePath = FPaths::ProjectContentDir() + TEXT("QMatrix.txt");
	FString forInput;

	if (FPaths::FileExists(filePath)) {

		FFileHelper::LoadFileToString(forInput, *filePath);

		TArray<FString> lines;
		forInput.ParseIntoArrayLines(lines);

		Q.Empty();

		for (const FString& line : lines) {
			TArray<FString> tokens;
			line.ParseIntoArray(tokens, TEXT(" "), true);

			TArray<float> innerArray;
			for (const FString& token : tokens) {
				innerArray.Add(FCString::Atof(*token));
			}
			Q.Add(innerArray);
		}
	}
}

void AShadowEnemy::ResetQ()
{
	FString filePath = FPaths::ProjectContentDir() + TEXT("QMatrix.txt");
	FString forOutput;

	TArray<float> qEntry;
	qEntry.Init(0, ACTION_NUM);

	Q.Empty();
	Q.Init(qEntry, STATE_NUM);

	for (const TArray<float>& InnerArray : Q) {
		for (float value : InnerArray) {
			forOutput += FString::SanitizeFloat(value) + TEXT(" ");
		}
		forOutput += TEXT("\n");
	}

	FFileHelper::SaveStringToFile(forOutput, *filePath);
}

void AShadowEnemy::PrintAction()
{
	//print which action is currently selected
	switch (cAction) {
	case MOVE_FORWARD:
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, TEXT("Move Forward"));
		break;
	case MOVE_BACKWARDS:
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, TEXT("Move Backwards"));
		break;
	case MOVE_LEFT:
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, TEXT("Move Left"));
		break;
	case MOVE_RIGHT:
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, TEXT("Move Right"));
		break;
	case MOVE_TOWARDS_PLAYER:
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, TEXT("Move Towards Player"));
		break;
	}

}

void AShadowEnemy::CalculateReward(float & calcReward)
{
	
	//if the enemy gets closer to the target compared to last frame, reward positively based on distance
	if (FVector(GetActorLocation() - player->GetActorLocation()).Length() < targDist) {
		calcReward += 1000 / FVector(GetActorLocation() - player->GetActorLocation()).Length();


		if (FVector(GetActorLocation() - player->GetActorLocation()).Length() < 100) {
			calcReward = 10000;
		}
	}
	else if (FVector(GetActorLocation() - player->GetActorLocation()).Length() > targDist) {
		//if the enemy gets further from the target compared to last frame, reward negatively based on distance
		calcReward -= FVector(GetActorLocation() - player->GetActorLocation()).Length();

	}


	//raycast directly in from of enemy for a meter
	FHitResult hit;
	GetWorld()->LineTraceSingleByObjectType(hit, GetActorLocation(), GetActorLocation() + (GetActorForwardVector() * 100), ECC_WorldStatic);

	//if touching a wall, heavily penalise the potential reward
	if (hit.bBlockingHit != 0) {
		calcReward -= 2 * FVector(GetActorLocation() - player->GetActorLocation()).Length();
	}

	//if being hit by the point lights
	if (inLight > 0) {

		//search for the closest light
		float lowest = 100000;
		float lowAtt = 0;
		for (int i = 0; i < inLights.Num(); i++) {
			if (FVector(GetActorLocation() - inLights[i]->GetActorLocation()).Length() < lowest) {
				lowest = FVector(GetActorLocation() - inLights[i]->GetActorLocation()).Length();
				UPointLightComponent* light = inLights[i]->GetComponentByClass<UPointLightComponent>();
				lowAtt = light->AttenuationRadius;
			}
		}

		//reduce the reward based on how close the player is to the light multiplied by the amount its being hit by the light
		calcReward -= (inLight * (lowAtt / lowest) * 2000);
	}

	reward += calcReward;
}

float AShadowEnemy::getMax() {

	//return the maximum reward value in the Q array with current state
	float max = 0;

	for (int i = 0; i < ACTION_NUM; i++) {
		if (Q[cState][i] > max) {
			max = Q[cState][i];
		}
	}

	return max;
}

