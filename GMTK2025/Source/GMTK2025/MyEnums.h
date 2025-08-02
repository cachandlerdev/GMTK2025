
#pragma once

#include "CoreMinimal.h"
#include "MyEnums.generated.h"

UENUM(BlueprintType)
enum class ESteerDirection : uint8
{
	STRAIGHT	UMETA(DisplayName = "STRAIGHT"),
	LEFT		UMETA(DisplayName = "LEFT"),
	RIGHT		UMETA(DisplayName = "RIGHT"),
};
