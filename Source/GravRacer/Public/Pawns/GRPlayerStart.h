// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "GRPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class GRAVRACER_API AGRPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStart")
	int32 Placement;
};
