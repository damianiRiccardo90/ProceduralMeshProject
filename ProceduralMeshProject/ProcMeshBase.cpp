#include "ProcMeshBase.h"
#include "LinearAlgebraFunctionLibrary.h"
#include <Engine/World.h>


void FProcMeshSectionData::ClearData()
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	Tangents.Empty();
	UVs.Empty();
	Colors.Empty();
}

AProcMeshBase::AProcMeshBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MinRadius = FVector(10.f, 10.f, 10.f);
	CollisionProfileName = FName("ProceduralMesh");

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;
}

void AProcMeshBase::BeginPlay()
{
	Super::BeginPlay();

	GenerateMesh();

	SetMaterial(MainMaterial);
}

void AProcMeshBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ClearMesh();
}

void AProcMeshBase::SetRadius(const FVector& InRadius)
{
	Radius = InRadius;

	GenerateMesh();

	OnSetRadius.Broadcast(InRadius);
}

FVector AProcMeshBase::GetRadius() const
{
	return Radius;
}

FVector AProcMeshBase::GetMinRadius() const
{
	return MinRadius;
}

void AProcMeshBase::Extrude(const FVector& InDelta)
{
	// The idea here is to be able to avoid generating the mesh all the times 
	// (using UpdateMeshSection_LinearColor instead of the less efficient CreateMeshSection_LinearColor)
	// and simultaneously avoiding the transform misalignment due to different actor and mesh component locations.
	// This approach let's you move the actor freely with the mesh following up correctly

	const FVector OldRadius = Radius;
	SetRadius(FVector(
		(Radius.X * 2 + FMath::Abs(InDelta.X)) / 2,
		(Radius.Y * 2 + FMath::Abs(InDelta.Y)) / 2,
		(Radius.Z * 2 + FMath::Abs(InDelta.Z)) / 2
	));

	AddActorWorldOffset(FVector(
		InDelta.X < 0 ? -(Radius.X - OldRadius.X) : Radius.X - OldRadius.X,
		InDelta.Y < 0 ? -(Radius.Y - OldRadius.Y) : Radius.Y - OldRadius.Y,
		InDelta.Z < 0 ? -(Radius.Z - OldRadius.Z) : Radius.Z - OldRadius.Z
	));
}

void AProcMeshBase::Intrude(const FVector& InDelta)
{
	const FVector OldRadius = Radius;
	SetRadius(FVector(
		(Radius.X * 2 - FMath::Abs(InDelta.X)) / 2,
		(Radius.Y * 2 - FMath::Abs(InDelta.Y)) / 2,
		(Radius.Z * 2 - FMath::Abs(InDelta.Z)) / 2
	));

	AddActorWorldOffset(FVector(
		InDelta.X < 0 ? -(Radius.X - OldRadius.X) : Radius.X - OldRadius.X,
		InDelta.Y < 0 ? -(Radius.Y - OldRadius.Y) : Radius.Y - OldRadius.Y,
		InDelta.Z < 0 ? -(Radius.Z - OldRadius.Z) : Radius.Z - OldRadius.Z
	));
}

void AProcMeshBase::SetCollisionProfileName(const FName& InProfileName)
{
	CollisionProfileName = InProfileName;
}

FName AProcMeshBase::GetCollisionProfileName() const
{
	return CollisionProfileName;
}

void AProcMeshBase::ClearMesh()
{
	// To be implemented in derived classes
}

void AProcMeshBase::SetMaterial(UMaterialInterface* InMaterial)
{
	// To be implemented in derived classes
}

void AProcMeshBase::GenerateMesh()
{
	// To be implemented in derived classes
}

void AProcMeshBase::ValidateDimensions()
{
	//Makes sure that radius is positive
	Radius = Radius.GetAbs();

	//Makes sure Radius >= MinRadius
	if (Radius.X < MinRadius.X)
	{
		Radius.X = MinRadius.X;
	}
	if (Radius.Y < MinRadius.Y)
	{
		Radius.Y = MinRadius.Y;
	}
	if (Radius.Z < MinRadius.Z)
	{
		Radius.Z = MinRadius.Z;
	}
}

void AProcMeshBase::AddQuadMesh(const FVector& InTopLeft, const FVector& InBottomLeft, const FVector& InTopRight, const FVector& InBottomRight, FProcMeshSectionData& OutSectionData)
{
	int32 TopLeftIndex = OutSectionData.Vertices.Num();
	OutSectionData.Vertices.Add(InTopLeft);
	int32 BottomLeftIndex = OutSectionData.Vertices.Num();
	OutSectionData.Vertices.Add(InBottomLeft);
	int32 TopRightIndex = OutSectionData.Vertices.Num();
	OutSectionData.Vertices.Add(InTopRight);
	int32 BottomRightIndex = OutSectionData.Vertices.Num();
	OutSectionData.Vertices.Add(InBottomRight);

	OutSectionData.Triangles.Add(TopLeftIndex);
	OutSectionData.Triangles.Add(BottomLeftIndex);
	OutSectionData.Triangles.Add(TopRightIndex);
	OutSectionData.Triangles.Add(BottomRightIndex);
	OutSectionData.Triangles.Add(TopRightIndex);
	OutSectionData.Triangles.Add(BottomLeftIndex);

	FVector UnitBitangent = FVector(InBottomLeft - InTopLeft).GetSafeNormal();
	FVector UnitTangent = FVector(InTopLeft - InTopRight).GetSafeNormal();
	FVector UnitNormal = FVector::CrossProduct(UnitBitangent, UnitTangent);

	for (int32 i = 0; i < 4; i++)
	{
		OutSectionData.Normals.Add(UnitNormal);
		OutSectionData.Tangents.Add(FProcMeshTangent(UnitTangent, false));
	}

	OutSectionData.UVs.Add(FVector2D(0.f, 1.f)); // Top Left
	OutSectionData.UVs.Add(FVector2D(0.f, 0.f)); // Bottom Left
	OutSectionData.UVs.Add(FVector2D(1.f, 1.f)); // Top Right
	OutSectionData.UVs.Add(FVector2D(1.f, 0.f)); // Bottom Right
}
