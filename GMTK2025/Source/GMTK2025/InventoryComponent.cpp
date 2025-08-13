// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
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
	//TODO: get reference to the vehicle to get the location

	if (VehicleItem != nullptr)
	{
		VehicleItem->RemoveItem();
	}

	UVehicleItems* NewVehicleItem = NewObject<UVehicleItems>(this, VehicleItemClass);
	if (NewVehicleItem)
	{
		NewVehicleItem->RegisterComponent();
		VehicleItem = NewVehicleItem;

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GetCollectableSound, GetActorLocation());
	}

}

UVehicleItems* UInventoryComponent::GetVehicleItems()
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
	//TODO: get reference to the vehicle to get the location
	//TODO: refactor boosting with coins logic

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), GetCollectableSound, GetActorLocation());
	Coins++;
	if (Coins == 10)
	{
		Boost(3);
		Coins = 0;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Boosted!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Coins: %d"), Coins));
	}
}