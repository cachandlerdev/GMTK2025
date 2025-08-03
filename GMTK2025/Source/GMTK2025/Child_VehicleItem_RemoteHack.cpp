// Fill out your copyright notice in the Description page of Project Settings.


#include "Child_VehicleItem_RemoteHack.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerGhostActor.h"

void UChild_VehicleItem_RemoteHack::UseItem()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerGhostActor::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		APlayerGhostActor* GhostActor = Cast<APlayerGhostActor>(Actor);
		if (GhostActor)
		{
			GhostActor->Boost(BoostAmount);
			FVector ForceDirection = GhostActor->GetActorRightVector(); // Right direction
			ForceDirection *= 3500.0f; // Tune this for desired impulse

			// Push left or right randomly or based on player input
			int32 Direction = FMath::RandBool() ? 1 : -1;
			ForceDirection *= Direction;

			UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(GhostActor->GetRootComponent());
			if (RootComp)
			{
				RootComp->AddImpulse(ForceDirection, NAME_None, true);
			}
		}
	}
	RemoveItem();
}

void UChild_VehicleItem_RemoteHack::RemoveItem()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Remote Hack item removed."));
	}
	Super::RemoveItem();
}