// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GravRacerGameMode.generated.h"

/*WaitingToStart can be used for character selection
namespace MatchState
{
	extern const FName CharacterSelect;

}*/

class AGRCharacterSelectPawn;
class AGRPlayerStart;
class AGSPlayerController;
class AGSPlayerState;

UCLASS()
class GRAVRACER_API AGravRacerGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	FName GameplayPersistentLevel;
	FName CharacterSelectLevel;
	FName MainMenuLevel;

	TSubclassOf<AGRCharacterSelectPawn> CharacterSelectPawnClass;

	AGravRacerGameMode();

	virtual void OnMatchStateSet() override;
	virtual void HandleMatchIsWaitingToStart() override;

	void EndCharacterSelect();
	virtual void HandleMatchHasStarted() override;

	virtual bool ReadyToStartMatch_Implementation() override;
	virtual bool ReadyToEndMatch_Implementation() override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	void HeroDied(AController* Controller);
	virtual void RestartPlayer(AController* NewPlayer) override;

protected:
	float RespawnDelay;

	TSubclassOf<class AGravRacerPawn> PawnClass;
	virtual void BeginPlay() override;

	void SpawnRacingPawn(AController* Controller);
	void SpawnCharacterSelectPawn(AController* Controller);
	AGRPlayerStart* GetPlayerStartForController(AController* Controller);

public:

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void LoadLevelAndWait(FName LevelToLoad, FName LevelToUnload);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void LoadLevelsAndWait(TArray<FName> LevelsToLoad, TArray<FName> LevelsToUnload);

	UFUNCTION()
	void OnLevelLoaded();

	UFUNCTION()
	void ClientLoadComplete(APlayerController* PC, int32 Ack);

protected:
	FTimerHandle GamePhaseTimer;
	float CharacterSelectTime;

	TArray<FName> LevelsPendingLoad;
	TArray<FName> LevelsPendingUnload;
	int32 LatestLevelAck;
	TArray<APlayerController*> PlayersLoading;

	TArray<AGSPlayerController*> GetServerPlayerControllers();

	TArray<AGSPlayerState*> GetPlayerStates();

	void CheckPlayersLoaded();
	void LevelLoadingComplete();
};
