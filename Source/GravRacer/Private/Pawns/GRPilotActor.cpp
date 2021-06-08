// Copyright 2020 Dan Kestranek.


#include "Pawns/GRPilotActor.h"
#include "Engine/SkeletalMesh.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGRPilotActor::AGRPilotActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

// Called when the game starts or when spawned
void AGRPilotActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGRPilotActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGRPilotActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGRPilotActor, PilotMode);
}
