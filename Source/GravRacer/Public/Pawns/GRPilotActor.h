// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GRPilotActor.generated.h"

class USkeletalMeshComponent;

UENUM(BlueprintType)
enum class EPilotActorMode : uint8
{
	PM_Sitting		UMETA(DisplayName = "Sitting In Vehicle"),
	PM_Standing 	UMETA(DisplayName = "Standing"),
	PM_Render 		UMETA(DisplayName = "Icon Pose")
};

UCLASS()
class GRAVRACER_API AGRPilotActor : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	EPilotActorMode PilotMode;

	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
	void SetPilotMode(EPilotActorMode NewMode) { PilotMode = NewMode; };
	
public:	
	// Sets default values for this actor's properties
	AGRPilotActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
