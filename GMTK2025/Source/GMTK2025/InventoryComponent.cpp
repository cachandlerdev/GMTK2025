// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "VehiclePawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<int> UInventoryComponent::GetItems()
{
	TArray<int> Items;
	Items.Init(1, 2);

	return Items;

}

void UInventoryComponent::AddVehicleItem(TSubclassOf<UVehicleItems> VehicleItemClass)
{
	if (VehicleItem != nullptr)
	{
		VehicleItem->RemoveItem();
	}

	UVehicleItems* NewVehicleItem = NewObject<UVehicleItems>(this, VehicleItemClass);
	if (NewVehicleItem)
	{
		//NewVehicleItem->RegisterComponent();
		VehicleItem = NewVehicleItem;

		//if (GetCollectableSound)
		//{
		//	UGameplayStatics::PlaySoundAtLocation(GetWorld(), GetCollectableSound, GetOwner()->GetActorLocation(), GetCollectableSound->GetVolumeMultiplier());
		//}
	}
}

UVehicleItems* UInventoryComponent::GetVehicleItem()
{
	return VehicleItem;
}

void UInventoryComponent::UseItem(const float axisValue)
{
	if (axisValue != 0)
	{
		if (VehicleItem != nullptr)
		{
			OnUseItemBP(VehicleItem);
			VehicleItem->UseItem();
		}
	}
}

void UInventoryComponent::AddCoins()
{
	//TODO: refactor boosting with coins logic

	if (GetCollectableSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GetCollectableSound, GetOwner()->GetActorLocation(), GetCollectableSound->GetVolumeMultiplier());
	}
	Coins++;
	if (Coins == 10)
	{
		//Disabled boost until the boosting refactor is done
		
		AVehiclePawn* Owner = Cast<AVehiclePawn>(GetOwner());

		if (BoostSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BoostSound, Owner->GetActorLocation(), Owner->GetActorRotation(), BoostSound->GetVolumeMultiplier());
		}
		Owner->MovementComponent->Boost(CoinBoostStrength);
		Coins = 0;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Boosted!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Coins: %d"), Coins));
	}
}