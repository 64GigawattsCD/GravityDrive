// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GravRacerViewportClient.generated.h"

class UCanvas;

UCLASS(Within=Engine, transient, config=Engine)
class UGravRacerViewportClient : public UGameViewportClient
{
	GENERATED_UCLASS_BODY()

public:


#if WITH_EDITOR
	virtual void DrawTransition(UCanvas* Canvas) override;
#endif //WITH_EDITOR	
};