// Copyright 2020 Dan Kestranek.


#include "GravRacerGameMode.h"
#include "Engine/World.h"
#include "Pawns/GravRacerPawn.h"
#include "Player/GSPlayerController.h"
#include "Player/GSPlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Pawns/GRCharacterSelectPawn.h"
#include "Pawns/GRPlayerStart.h"
#include "Engine/LevelStreaming.h"
#include "GameFramework/GameState.h"

AGravRacerGameMode::AGravRacerGameMode()
{
	bDelayedStart = true;
	RespawnDelay = 5.0f;
	MaxInactivePlayers = 4;
	LatestLevelAck = 0;
	
	PawnClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/Vehicles/Base/BP_GravRacerPawn.BP_GravRacerPawn_C"));
	if (!PawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find PawnClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
	}

	CharacterSelectPawnClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/GravRacer/CharacterSelect/BP_CharSelectPawn.BP_CharSelectPawn_C"));
	if (!CharacterSelectPawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find CharacterSelectPawnClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
	}

	PlayerControllerClass = AGSPlayerController::StaticClass();
	PlayerStateClass = AGSPlayerState::StaticClass();
	//GameStateClass = AGraveRacer

	GameplayPersistentLevel = FName("GravRacer_P");
	CharacterSelectLevel = FName("CharacterSelect");
	MainMenuLevel = FName("MainMenu");

#if WITH_EDITOR
	CharacterSelectTime = 3.f;
#else
	CharacterSelectTime = 30.f;
#endif
}


void AGravRacerGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
}

void AGravRacerGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	LoadLevelAndWait(CharacterSelectLevel, FName());

	GetWorldTimerManager().SetTimer(GamePhaseTimer, this, &AGravRacerGameMode::EndCharacterSelect, CharacterSelectTime, false, 30.f);
}

void AGravRacerGameMode::EndCharacterSelect()
{
	SetMatchState(MatchState::EnteringMap);

	LoadLevelAndWait(FName("Test_01"), CharacterSelectLevel);
}


void AGravRacerGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	//Restart all players
	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		RestartPlayer(Iterator->Get());
	}
}

bool AGravRacerGameMode::ReadyToStartMatch_Implementation()
{
	//TODO: return true if all players have selected characters/vehicles

	return false;
}


bool AGravRacerGameMode::ReadyToEndMatch_Implementation()
{
	return false;
}


void AGravRacerGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	//Update initial placements while players are joining
	int32 I = -1;
	for (AGSPlayerState* PS : GetPlayerStates())
	{
		I++;
		PS->SetPlacement(I);
	}
}

void AGravRacerGameMode::HeroDied(AController* Controller)
{
	/*
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, Controller->GetPawn()->GetActorTransform(), SpawnParameters);

	Controller->UnPossess();
	Controller->Possess(SpectatorPawn);

	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;

	RespawnDelegate = FTimerDelegate::CreateUObject(this, &AGravRacerGameMode::RespawnHero, Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);

	AGSPlayerController* PC = Cast<AGSPlayerController>(Controller);
	if (PC)
	{
		PC->SetRespawnCountdown(RespawnDelay);
	}*/
}

void AGravRacerGameMode::BeginPlay()
{
	Super::BeginPlay();
	SetMatchState(MatchState::WaitingToStart);
}

void AGravRacerGameMode::RestartPlayer(AController* NewPlayer)
{
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		SpawnCharacterSelectPawn(NewPlayer);
	}
	else
	{
		SpawnRacingPawn(NewPlayer);
	}
}

void AGravRacerGameMode::SpawnRacingPawn(AController* Controller)
{
	if (!Controller || !Controller->IsPlayerController())
	{
		return;
	}

	AGRPlayerStart* SpawnPoint = GetPlayerStartForController(Controller);

	if (!SpawnPoint)
	{
		return;
	}

	// Respawn 
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AGravRacerPawn* Hero = GetWorld()->SpawnActor<AGravRacerPawn>(PawnClass, SpawnPoint->GetActorTransform(), SpawnParameters);

	APawn* OldPawn = Controller->GetPawn();
	Controller->UnPossess();
	if (OldPawn)
	{
		OldPawn->Destroy();
	}
	Controller->Possess(Hero);
	
}

void AGravRacerGameMode::SpawnCharacterSelectPawn(AController* Controller)
{
	if (!Controller || !Controller->IsPlayerController())
	{
		return;
	}

	AGRPlayerStart* SpawnPoint = GetPlayerStartForController(Controller);

	if (!SpawnPoint)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AGRCharacterSelectPawn* CS = GetWorld()->SpawnActor<AGRCharacterSelectPawn>(CharacterSelectPawnClass, SpawnPoint->GetActorTransform(), SpawnParameters);

	APawn* OldPawn = Controller->GetPawn();
	Controller->UnPossess();
	if (OldPawn)
	{
		OldPawn->Destroy();
	}

	Controller->Possess(CS);
}

AGRPlayerStart* AGravRacerGameMode::GetPlayerStartForController(AController* Controller)
{
	if (!Controller)
	{
		return nullptr;
	}

	AGSPlayerState* PlayerState = Controller->GetPlayerState<AGSPlayerState>();
	if (!PlayerState )
	{
		return nullptr;
	}

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(Controller->GetWorld(), AGRPlayerStart::StaticClass(), PlayerStarts);

	for (AActor* A : PlayerStarts)
	{
		AGRPlayerStart* Start = Cast<AGRPlayerStart>(A);
		if (Start && (PlayerState->GetPlacement()==0 || Start->Placement == PlayerState->GetPlacement()))
		{
			return Start;
		}
	}

	return nullptr;
}

void AGravRacerGameMode::LoadLevelAndWait(FName LevelToLoad, FName LevelToUnload)
{
	TArray<FName> LevelsToLoad;
	TArray<FName> LevelsToUnload;

	if (LevelToLoad != NAME_None)
	{
		LevelsToLoad.Add(LevelToLoad);
	}

	if (LevelToUnload != NAME_None)
	{
		LevelsToUnload.Add(LevelToUnload);
	}

	LoadLevelsAndWait(LevelsToLoad, LevelsToUnload);
}

void AGravRacerGameMode::LoadLevelsAndWait(TArray<FName> LevelsToLoad, TArray<FName> LevelsToUnload)
{
	LatestLevelAck++;

	//Turn off loading screen for listen server players
	for (AGSPlayerController* PC : GetServerPlayerControllers())
	{
		PC->ShowLoadingScreen();
	}

	for (FName LevelToUnload : LevelsToUnload)
	{
		if (!LevelsPendingUnload.Contains(LevelToUnload))
		{
			FLatentActionInfo LatentActionInfo;
			LatentActionInfo.CallbackTarget = this;
			LatentActionInfo.ExecutionFunction = "OnLevelLoaded";
			//Really not sure about this
			LatentActionInfo.UUID = FCString::Atoi(*FGuid::NewGuid().ToString());
			LatentActionInfo.Linkage = 0;

			UGameplayStatics::UnloadStreamLevel(this, LevelToUnload, LatentActionInfo, false);

			LevelsPendingUnload.Add(LevelToUnload);
		}
	}

	for (FName LevelToLoad : LevelsToLoad)
	{
		if (!LevelsPendingLoad.Contains(LevelToLoad))
		{
			FLatentActionInfo LatentActionInfo;
			LatentActionInfo.CallbackTarget = this;
			LatentActionInfo.ExecutionFunction = "OnLevelLoaded";
			//Really not sure about this
			LatentActionInfo.UUID = FCString::Atoi(*FGuid::NewGuid().ToString());
			LatentActionInfo.Linkage = 0;

			UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentActionInfo);

			LevelsPendingLoad.Add(LevelToLoad);
		}
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AGSPlayerController* PC = Cast<AGSPlayerController>(Iterator->Get());
		if (PC && !PC->IsLocalController())
		{
			PC->ClientLoadLevels(LevelsToLoad, LevelsToUnload, LatestLevelAck);
		}
	}
}

void AGravRacerGameMode::OnLevelLoaded()
{

	for (int32 I = LevelsPendingUnload.Num() - 1; I > -1; I--)
	{
		FName Level = LevelsPendingUnload[I];
		ULevelStreaming* LevelInst = UGameplayStatics::GetStreamingLevel(this, Level);
		if (!LevelInst || !LevelInst->IsLevelLoaded())
		{
			LevelsPendingUnload.Remove(Level);
		}
	}

	for (int32 I = LevelsPendingLoad.Num() - 1; I > -1; I--)
	{
		FName Level = LevelsPendingLoad[I];
		if (UGameplayStatics::GetStreamingLevel(this, Level))
		{
			LevelsPendingLoad.Remove(Level);
		}
	}

	if (LevelsPendingLoad.Num() == 0 && LevelsPendingUnload.Num() == 0)
	{
		//Turn off loading screen for listen server players
		for (AGSPlayerController* PC : GetServerPlayerControllers())
		{
			PC->HideLoadingScreen();
		}

		CheckPlayersLoaded();
	}
}


void AGravRacerGameMode::ClientLoadComplete(APlayerController* PC, int32 Ack)
{
	if (Ack >= LatestLevelAck)
	{
		PlayersLoading.Remove(PC);
	}

	CheckPlayersLoaded();
}


TArray<AGSPlayerController*> AGravRacerGameMode::GetServerPlayerControllers()
{
	TArray<AGSPlayerController*> PCs;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AGSPlayerController* PC = Cast<AGSPlayerController>(Iterator->Get());
		if (PC && PC->IsLocalController())
		{
			PCs.Add(PC);
		}
	}

	return PCs;
}


TArray<AGSPlayerState*> AGravRacerGameMode::GetPlayerStates()
{
	TArray<AGSPlayerState*> PSs;

	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		AController* C = Iterator->Get();
		AGSPlayerState* PS = C ? C->GetPlayerState<AGSPlayerState>() : nullptr;
		if (PS)
		{
			PSs.Add(PS);
		}
	}

	return PSs;
}

void AGravRacerGameMode::CheckPlayersLoaded()
{
	if (PlayersLoading.Num() == 0)
	{
		LevelLoadingComplete();
	}
}

void AGravRacerGameMode::LevelLoadingComplete()
{
	if (MatchState == MatchState::WaitingToStart)
	{
		//Character select is loaded
		//Turn off loaded screen
		for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
		{
			AController* C = Iterator->Get();
			if (C)
			{
				RestartPlayer(C);
			}
		}
	}
	else if (MatchState == MatchState::EnteringMap)
	{
		StartMatch();
		//SetMatchState(MatchState::InProgress);
	}
}
