// Copyright Epic Games, Inc. All Rights Reserved.

//#include "GravRacer.h"
#include "AI/GravRacerAIController.h"
#include "Player/GSPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Pawns/GravRacerPawn.h"
#include "EngineUtils.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

AGravRacerAIController::AGravRacerAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));
 	
	BrainComponent = BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));	

	bWantsPlayerState = true;
}

void AGravRacerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AGravRacerPawn* Bot = Cast<AGravRacerPawn>(InPawn);

	// start behavior
	if (Bot && Bot->BotBehavior)
	{
		if (Bot->BotBehavior->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*Bot->BotBehavior->BlackboardAsset);
		}

		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		NeedAmmoKeyID = BlackboardComp->GetKeyID("NeedAmmo");

		BehaviorComp->StartTree(*(Bot->BotBehavior));
	}
}

void AGravRacerAIController::OnUnPossess()
{
	Super::OnUnPossess();

	BehaviorComp->StopTree();
}

void AGravRacerAIController::BeginInactiveState()
{
	Super::BeginInactiveState();

	AGameStateBase const* const GameState = GetWorld()->GetGameState();

	const float MinRespawnDelay = GameState ? GameState->GetPlayerRespawnDelay(this) : 1.0f;

	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &AGravRacerAIController::Respawn, MinRespawnDelay);
}

void AGravRacerAIController::Respawn()
{
	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}

void AGravRacerAIController::FindClosestEnemy()
{
	APawn* MyBot = GetPawn();
	if (MyBot == NULL)
	{
		return;
	}

	const FVector MyLoc = MyBot->GetActorLocation();
	float BestDistSq = MAX_FLT;
	AGravRacerPawn* BestPawn = NULL;

	for (TActorIterator<AGravRacerPawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AGravRacerPawn* TestPawn = *ActorItr;
		//TODO
		/*
		if (TestPawn->IsAlive() && TestPawn->IsEnemyFor(this))
		{
			const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				BestPawn = TestPawn;
			}
		}*/
	}

	if (BestPawn)
	{
		SetEnemy(BestPawn);
	}
}

bool AGravRacerAIController::FindClosestEnemyWithLOS(AGravRacerPawn* ExcludeEnemy)
{
	bool bGotEnemy = false;
	APawn* MyBot = GetPawn();
	if (MyBot != NULL)
	{
		/*
		const FVector MyLoc = MyBot->GetActorLocation();
		float BestDistSq = MAX_FLT;
		AGravRacerPawn* BestPawn = NULL;

		for (AGravRacerPawn* TestPawn : TActorRange<AGravRacerPawn>(GetWorld()))
		{
			if (TestPawn != ExcludeEnemy && TestPawn->IsAlive() && TestPawn->IsEnemyFor(this))
			{
				if (HasWeaponLOSToEnemy(TestPawn, true) == true)
				{
					const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
					if (DistSq < BestDistSq)
					{
						BestDistSq = DistSq;
						BestPawn = TestPawn;
					}
				}
			}
		}
		if (BestPawn)
		{
			SetEnemy(BestPawn);
			bGotEnemy = true;
		}*/
	}
	return bGotEnemy;
}

bool AGravRacerAIController::HasWeaponLOSToEnemy(AActor* InEnemyActor, const bool bAnyEnemy) const
{
	
	AGravRacerPawn* MyBot = Cast<AGravRacerPawn>(GetPawn());

	bool bHasLOS = false;
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(AIWeaponLosTrace), true, GetPawn());

	TraceParams.bReturnPhysicalMaterial = true;	
	FVector StartLocation = MyBot->GetActorLocation();	
	StartLocation.Z += GetPawn()->BaseEyeHeight; //look from eyes
	
	FHitResult Hit(ForceInit);
	const FVector EndLocation = InEnemyActor->GetActorLocation();
	GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, COLLISION_PROJECTILE, TraceParams);
	if (Hit.bBlockingHit == true)
	{
		// Theres a blocking hit - check if its our enemy actor
		AActor* HitActor = Hit.GetActor();
		if (Hit.GetActor() != NULL)
		{
			if (HitActor == InEnemyActor)
			{
				bHasLOS = true;
			}
			else if (bAnyEnemy == true)
			{
				// Its not our actor, maybe its still an enemy ?
				APawn* HitChar = Cast<APawn>(HitActor);
				if (HitChar != NULL)
				{
					AGSPlayerState* HitPlayerState = Cast<AGSPlayerState>(HitChar->GetPlayerState());
					AGSPlayerState* MyPlayerState = Cast<AGSPlayerState>(PlayerState);
					if ((HitPlayerState != NULL) && (MyPlayerState != NULL))
					{
						if (HitPlayerState->GetTeamNum() != MyPlayerState->GetTeamNum())
						{
							bHasLOS = true;
						}
					}
				}
			}
		}
	}

	

	return bHasLOS;
}

void AGravRacerAIController::ShootEnemy()
{
	/*
	
	*/
}

void AGravRacerAIController::CheckAmmo()
{
	if (BlackboardComp)
	{
		//TODO
		const int32 Ammo = 0;
		const int32 MaxAmmo = 1;
		const float Ratio = (float) Ammo / (float) MaxAmmo;

		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(NeedAmmoKeyID, (Ratio <= 0.1f));
	}
}

void AGravRacerAIController::SetEnemy(class APawn* InPawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, InPawn);
		SetFocus(InPawn);
	}
}

class AGravRacerPawn* AGravRacerAIController::GetEnemy() const
{
	if (BlackboardComp)
	{
		return Cast<AGravRacerPawn>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(EnemyKeyID));
	}

	return NULL;
}


void AGravRacerAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	// Look toward focus
	FVector FocalPoint = GetFocalPoint();
	if( !FocalPoint.IsZero() && GetPawn())
	{
		FVector Direction = FocalPoint - GetPawn()->GetActorLocation();
		FRotator NewControlRotation = Direction.Rotation();
		
		NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);

		SetControlRotation(NewControlRotation);

		APawn* const P = GetPawn();
		if (P && bUpdatePawn)
		{
			P->FaceRotation(NewControlRotation, DeltaTime);
		}
		
	}
}

void AGravRacerAIController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	// Stop the behaviour tree/logic
	BehaviorComp->StopTree();

	// Stop any movement we already have
	StopMovement();

	// Cancel the repsawn timer
	GetWorldTimerManager().ClearTimer(TimerHandle_Respawn);

	// Clear any enemy
	SetEnemy(NULL);

	// Finally stop firing
	AGravRacerPawn* MyBot = Cast<AGravRacerPawn>(GetPawn());
	/*
	
	MyBot->StopWeaponFire();*/	
}

