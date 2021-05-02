// Copyright Epic Games, Inc. All Rights Reserved.

#include "GravRacerViewportClient.h"

UGravRacerViewportClient::UGravRacerViewportClient(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetSuppressTransitionMessage(true);
}

#if WITH_EDITOR
void UGravRacerViewportClient::DrawTransition(UCanvas* Canvas)
{
	if (GetOuterUEngine() != nullptr)
	{
		ETransitionType Type = GetOuterUEngine()->TransitionType;
		switch (Type)
		{
		case ETransitionType::Connecting:
			DrawTransitionMessage(Canvas, NSLOCTEXT("GameViewportClient", "ConnectingMessage", "CONNECTING").ToString());
			break;
		case ETransitionType::WaitingToConnect:
			DrawTransitionMessage(Canvas, NSLOCTEXT("GameViewportClient", "Waitingtoconnect", "Waiting to connect...").ToString());
			break;
		}
	}
}
#endif //WITH_EDITOR