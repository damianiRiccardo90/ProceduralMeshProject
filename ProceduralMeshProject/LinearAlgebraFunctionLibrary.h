#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "LinearAlgebraFunctionLibrary.generated.h"


UCLASS()
class PROCEDURALMESHPROJECT_API ULinearAlgebraFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static FVector TranslatePoint(const FVector& InVector, const FVector& InDelta);
	UFUNCTION(BlueprintCallable)
	static FVector ScalePoint(const FVector& InVector, const FVector& InDelta);
	UFUNCTION(BlueprintCallable)
	static FVector RotatePoint(const FVector& InVector, const FRotator& InRotation, const FVector& InRotationPoint);
	UFUNCTION(BlueprintCallable)
	static float EuclideanNorm(const FVector2D& InVector);
	UFUNCTION(BlueprintCallable)
	static FVector2D VectorProjection(const FVector2D& InArgVector, const FVector2D& InBaseVector);

};
