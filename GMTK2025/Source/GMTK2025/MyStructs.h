
#pragma once

#include "CoreMinimal.h"
#include "MyEnums.h"
#include "MyStructs.generated.h"

USTRUCT()
struct FInnerFloatArray
{
	GENERATED_BODY()

	UPROPERTY(/* Inner specifiers */)
	TArray<float> ArrayOfFloats;
};

USTRUCT()
struct FInnerBoolArray
{
	GENERATED_BODY()

	UPROPERTY(/* Inner specifiers */)
	TArray<bool> ArrayOfBools;
};

USTRUCT()
struct FInnerSteerDirectionArray
{
	GENERATED_BODY()

	UPROPERTY(/* Inner specifiers */)
	TArray<ESteerDirection> ArrayOfDirections;
};

USTRUCT()
struct FInnerTransformArray
{
	GENERATED_BODY()

	UPROPERTY(/* Inner specifiers */)
	TArray<FTransform> ArrayOfTransforms;
};
