// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Characters/GSCharacterBase.h"
#include "GSPlayerController.generated.h"

class UPaperSprite;
class AVehicleTrackPoint;
/**
 * 
 */
UCLASS()
class GRAVRACER_API AGSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGSPlayerController();

	void CreateLoadingScreen();
	void ShowLoadingScreen();
	void HideLoadingScreen();

	void CreateCharacterSelect();
	void RemoveCharacterSelect();

	void CreateHUD();
	class UGSHUDWidget* GetGSHUD();

	virtual void BeginPlay() override;
	/**
	* Weapon HUD info
	*/

	UFUNCTION(BlueprintCallable, Category = "GASShooter|UI")
	void SetEquippedWeaponPrimaryIconFromSprite(UPaperSprite* InSprite);

	UFUNCTION(BlueprintCallable, Category = "GASShooter|UI")
	void SetEquippedWeaponStatusText(const FText& StatusText);

	UFUNCTION(BlueprintCallable, Category = "GASShooter|UI")
	void SetPrimaryClipAmmo(int32 ClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASShooter|UI")
	void SetPrimaryReserveAmmo(int32 ReserveAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASShooter|UI")
	void SetSecondaryClipAmmo(int32 SecondaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASShooter|UI")
	void SetSecondaryReserveAmmo(int32 SecondaryReserveAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASShooter|UI")
	void SetHUDReticle(TSubclassOf<class UGSHUDReticle> ReticleClass);


	UFUNCTION(Client, Reliable, WithValidation)
	void ShowDamageNumber(float DamageAmount, AGSCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);
	void ShowDamageNumber_Implementation(float DamageAmount, AGSCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);
	bool ShowDamageNumber_Validate(float DamageAmount, AGSCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);

	// Simple way to RPC to the client the countdown until they respawn from the GameMode. Will be latency amount of out sync with the Server.
	UFUNCTION(Client, Reliable, WithValidation)
	void SetRespawnCountdown(float RespawnTimeRemaining);
	void SetRespawnCountdown_Implementation(float RespawnTimeRemaining);
	bool SetRespawnCountdown_Validate(float RespawnTimeRemaining);

	UFUNCTION(Client, Reliable, WithValidation)
	void ClientSetControlRotation(FRotator NewRotation);
	void ClientSetControlRotation_Implementation(FRotator NewRotation);
	bool ClientSetControlRotation_Validate(FRotator NewRotation);

	UFUNCTION()
	void OnTrackPointReached(AVehicleTrackPoint* NewCheckpoint);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASShooter|UI")
	TSubclassOf<class UGSHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASShooter|UI")
	TSubclassOf<class UUserWidget> UICharacterSelectWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASShooter|UI")
	TSubclassOf<class UGRLoadingScreenWidget> LoadingScreenWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "GASShooter|UI")
	class UGSHUDWidget* UIHUDWidget;

	UPROPERTY(BlueprintReadWrite, Category = "GASShooter|UI")
	class UUserWidget* CharacterSelectWidget;

	UPROPERTY(BlueprintReadWrite, Category = "GASShooter|UI")
	class UGRLoadingScreenWidget* LoadingScreenWidget;

	// Server only
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnRep_PlayerState() override;

	UFUNCTION(Exec)
	void Kill();

	UFUNCTION(Server, Reliable)
	void ServerKill();
	void ServerKill_Implementation();
	bool ServerKill_Validate();

public:
	UFUNCTION(Client, Reliable, WithValidation)
	void ClientLoadLevels(const TArray<FName> &LevelsToLoad, const TArray<FName> &LevelsToUnload, int32 Ack);

protected:

	int32 LevelLoadAck;
	TArray<FName> LevelsPendingLoad;
	TArray<FName> LevelsPendingUnload;

	UFUNCTION()
	void OnLevelLoaded();
	void LevelLoadingComplete();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerConfirmLevelLoad(int32 Ack);
};
