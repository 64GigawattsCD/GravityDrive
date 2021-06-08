// Copyright 2020 Dan Kestranek.


#include "Pawns/GRCharacterSelectPawn.h"
#include "Player/GSPlayerController.h"

// Sets default values
AGRCharacterSelectPawn::AGRCharacterSelectPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGRCharacterSelectPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGRCharacterSelectPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGRCharacterSelectPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AGRCharacterSelectPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AGSPlayerController* PC = Cast<AGSPlayerController>(GetController());
	if (PC)
	{
		PC->CreateCharacterSelect();
	}
}