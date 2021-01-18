#include "LinearAlgebraFunctionLibrary.h"
#include <Kismet/KismetMathLibrary.h>


FVector ULinearAlgebraFunctionLibrary::TranslatePoint(const FVector& InVector, const FVector& InDelta)
{
	const FVector4 HomogeneousInVector = FVector4(InVector);
	
	const FMatrix TranslationMatrix = FMatrix(
		FPlane(FVector4(1.f,       0.f,       0.f,       0.f)),
		FPlane(FVector4(0.f,       1.f,       0.f,       0.f)),
		FPlane(FVector4(0.f,       0.f,       1.f,       0.f)),
		FVector4(       InDelta.X, InDelta.Y, InDelta.Z, 1.f)
	);
	
	return TranslationMatrix.TransformFVector4(HomogeneousInVector);
}

FVector ULinearAlgebraFunctionLibrary::ScalePoint(const FVector& InVector, const FVector& InDelta)
{
	const FVector4 HomogeneousInVector = FVector4(InVector);

	const FMatrix ScaleMatrix = FMatrix(
		FPlane(FVector4(InDelta.X, 0.f,       0.f,       0.f)),
		FPlane(FVector4(0.f,       InDelta.Y, 0.f,       0.f)),
		FPlane(FVector4(0.f,       0.f,       InDelta.Z, 0.f)),
		FVector4(       0.f,       0.f,       0.f,       1.f)
	);

	return ScaleMatrix.TransformFVector4(HomogeneousInVector);
}

FVector ULinearAlgebraFunctionLibrary::RotatePoint(const FVector& InVector, const FRotator& InRotation, const FVector& InRotationPoint)
{
	const FVector RadiansRotation = FVector(FMath::DegreesToRadians(InRotation.Roll), FMath::DegreesToRadians(InRotation.Pitch), FMath::DegreesToRadians(InRotation.Yaw));
	const FMatrix XRotationMatrix = FMatrix(
		FPlane(FVector4(1.f, 0.f,                           0.f,                            0.f)),
		FPlane(FVector4(0.f, FMath::Cos(RadiansRotation.X), -FMath::Sin(RadiansRotation.X), 0.f)),
		FPlane(FVector4(0.f, FMath::Sin(RadiansRotation.X), FMath::Cos(RadiansRotation.X),  0.f)),
		FVector4(       0.f, 0.f,                           0.f,                            1.f)
	);
	const FMatrix YRotationMatrix = FMatrix(
		FPlane(FVector4(FMath::Cos(RadiansRotation.Y), 0.f, -FMath::Sin(RadiansRotation.Y), 0.f)),
		FPlane(FVector4(0.f,                           1.f, 0.f,                            0.f)),
		FPlane(FVector4(FMath::Sin(RadiansRotation.Y), 0.f, FMath::Cos(RadiansRotation.Y),  0.f)),
		FVector4(       0.f,                           0.f, 0.f,                            1.f)
	);
	const FMatrix ZRotationMatrix = FMatrix(
		FPlane(FVector4(FMath::Cos(RadiansRotation.Z), -FMath::Sin(RadiansRotation.Z), 0.f, 0.f)),
		FPlane(FVector4(FMath::Sin(RadiansRotation.Z), FMath::Cos(RadiansRotation.Z),  0.f, 0.f)),
		FPlane(FVector4(0.f,                           0.f,                            1.f, 0.f)),
		FVector4(       0.f,                           0.f,                            0.f, 1.f)
	);
	const FMatrix RotationMatrix = XRotationMatrix * YRotationMatrix * ZRotationMatrix;

	const FVector TranslatedToOrigin = TranslatePoint(InVector, -InRotationPoint);
	const FVector Rotated = RotationMatrix.TransformFVector4(FVector4(TranslatedToOrigin));
	const FVector TranslatedBack = TranslatePoint(Rotated, InRotationPoint);

	return TranslatedBack;
}

float ULinearAlgebraFunctionLibrary::EuclideanNorm(const FVector2D& InVector)
{
	return UKismetMathLibrary::Sqrt(FMath::Square(InVector.X) + FMath::Square(InVector.Y));
}

FVector2D ULinearAlgebraFunctionLibrary::VectorProjection(const FVector2D& InArgVector, const FVector2D& InBaseVector)
{
	return (FVector2D::DotProduct(InArgVector, InBaseVector) / EuclideanNorm(InBaseVector)) * (InBaseVector / EuclideanNorm(InBaseVector));
}
