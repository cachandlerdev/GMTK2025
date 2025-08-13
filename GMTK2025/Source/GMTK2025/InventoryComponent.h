// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleItems.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GMTK2025_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//TODO: Change this to an array of pickable items
	UFUNCTION(BlueprintCallable, Category = "Items")
	TArray<int> GetItems();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void AddVehicleItem(TSubclassOf<UVehicleItems> VehicleItemClass);

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	UVehicleItems* GetVehicleItems();

	UFUNCTION(BlueprintCallable, Category = "Item")
	void UseItem(const float axisValue);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UVehicleItems* VehicleItem;

	UFUNCTION(BlueprintCallable, Category = "Coins")
	void AddCoins();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* GetCollectableSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	int32 Coins = 0;

	UFUNCTION(BlueprintImplementableEvent, Category = "Items")
	void OnUseItemBP(UVehicleItems* Item);
};
