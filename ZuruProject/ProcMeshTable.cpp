#include "ProcMeshTable.h"
#include "ProcMeshCube.h"


AProcMeshTable::AProcMeshTable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Radius = FVector(100.f, 100.f, 42.5f);
	LegRadius = FVector(10.f, 10.f, 37.5f);
	PlankRadius = FVector(100.f, 100.f, 5.f);
}

FVector AProcMeshTable::GetLegRadius() const
{
	return LegRadius;
}

void AProcMeshTable::SetRadius(const FVector& InRadius)
{
	const FVector OldRadius = Radius;
	Radius = InRadius;
	ValidateDimensions();
	const FVector DeltaRadius = Radius - OldRadius;

	if (TopLeftLegMesh && TopRightLegMesh && BottomRightLegMesh && BottomLeftLegMesh && PlankMesh)
	{
		if (DeltaRadius.X != 0)
		{
			if (DeltaRadius.X > 0)
			{
				PlankMesh->Extrude(FVector(DeltaRadius.X, 0.f, 0.f));
				PlankMesh->Extrude(FVector(-DeltaRadius.X, 0.f, 0.f));
			} 
			else
			{
				PlankMesh->Intrude(FVector(DeltaRadius.X, 0.f, 0.f));
				PlankMesh->Intrude(FVector(-DeltaRadius.X, 0.f, 0.f));
			}
			TopLeftLegMesh->AddActorLocalOffset(FVector(-DeltaRadius.X, 0.f, 0.f));
			BottomLeftLegMesh->AddActorLocalOffset(FVector(-DeltaRadius.X, 0.f, 0.f));
			TopRightLegMesh->AddActorLocalOffset(FVector(DeltaRadius.X, 0.f, 0.f));
			BottomRightLegMesh->AddActorLocalOffset(FVector(DeltaRadius.X, 0.f, 0.f));
		} 
		else if (DeltaRadius.Y != 0)
		{
			if (DeltaRadius.Y > 0)
			{
				PlankMesh->Extrude(FVector(0.f, DeltaRadius.Y, 0.f));
				PlankMesh->Extrude(FVector(0.f, -DeltaRadius.Y, 0.f));
			} 
			else
			{
				PlankMesh->Intrude(FVector(0.f, DeltaRadius.Y, 0.f));
				PlankMesh->Intrude(FVector(0.f, -DeltaRadius.Y, 0.f));
			}
			BottomLeftLegMesh->AddActorLocalOffset(FVector(0.f, DeltaRadius.Y, 0.f));
			BottomRightLegMesh->AddActorLocalOffset(FVector(0.f, DeltaRadius.Y, 0.f));
			TopLeftLegMesh->AddActorLocalOffset(FVector(0.f, -DeltaRadius.Y, 0.f));
			TopRightLegMesh->AddActorLocalOffset(FVector(0.f, -DeltaRadius.Y, 0.f));
		}
		else if (DeltaRadius.Z != 0)
		{
			// TBD
		}
	}
}

void AProcMeshTable::SetMaterial(UMaterialInterface* InMaterial)
{
	if (TopLeftLegMesh && TopRightLegMesh && BottomRightLegMesh && BottomLeftLegMesh && PlankMesh)
	{
		TopLeftLegMesh->SetMaterial(InMaterial);
		TopRightLegMesh->SetMaterial(InMaterial);
		BottomRightLegMesh->SetMaterial(InMaterial);
		BottomLeftLegMesh->SetMaterial(InMaterial);
		PlankMesh->SetMaterial(InMaterial);
	}
}

void AProcMeshTable::ClearMesh()
{
	if (TopLeftLegMesh && TopRightLegMesh && BottomRightLegMesh && BottomLeftLegMesh && PlankMesh)
	{
		TopLeftLegMesh->ClearMesh();
		TopRightLegMesh->ClearMesh();
		BottomRightLegMesh->ClearMesh();
		BottomLeftLegMesh->ClearMesh();
		PlankMesh->ClearMesh();
	}
}

void AProcMeshTable::GenerateMesh()
{
	ClearMesh();

	SpawnMeshActor<AProcMeshCube>(TopLeftLegMesh, AProcMeshCube::StaticClass());
	SpawnMeshActor<AProcMeshCube>(TopRightLegMesh, AProcMeshCube::StaticClass());
	SpawnMeshActor<AProcMeshCube>(BottomRightLegMesh, AProcMeshCube::StaticClass());
	SpawnMeshActor<AProcMeshCube>(BottomLeftLegMesh, AProcMeshCube::StaticClass());
	SpawnMeshActor<AProcMeshCube>(PlankMesh, AProcMeshCube::StaticClass());

	ValidateDimensions();

	FVector TopLeftLegPosition     = FVector(-(Radius.X - LegRadius.X), -(Radius.Y - LegRadius.Y), -PlankRadius.Z);
	FVector TopRightLegPosition    = FVector(Radius.X - LegRadius.X, -(Radius.Y - LegRadius.Y), -PlankRadius.Z);
	FVector BottomRightLegPosition = FVector(Radius.X - LegRadius.X, Radius.Y - LegRadius.Y, -PlankRadius.Z);
	FVector BottomLeftLegPosition  = FVector(-(Radius.X - LegRadius.X), Radius.Y - LegRadius.Y, -PlankRadius.Z);
	FVector PlankPosition          = FVector(0.f, 0.f, LegRadius.Z);

	if (TopLeftLegMesh && TopRightLegMesh && BottomRightLegMesh && BottomLeftLegMesh && PlankMesh)
	{
		TopLeftLegMesh->SetRadius(LegRadius);
		TopLeftLegMesh->SetActorRelativeLocation(TopLeftLegPosition);

		TopRightLegMesh->SetRadius(LegRadius);
		TopRightLegMesh->SetActorRelativeLocation(TopRightLegPosition);

		BottomRightLegMesh->SetRadius(LegRadius);
		BottomRightLegMesh->SetActorRelativeLocation(BottomRightLegPosition);

		BottomLeftLegMesh->SetRadius(LegRadius);
		BottomLeftLegMesh->SetActorRelativeLocation(BottomLeftLegPosition);

		PlankMesh->SetRadius(PlankRadius);
		PlankMesh->SetActorRelativeLocation(PlankPosition);
	}
}

void AProcMeshTable::ValidateDimensions()
{
	// Makes sure that the components proportion and dimension are correct
	if (Radius.X != PlankRadius.X)
	{
		const float OldRadiusX = PlankRadius.X;
		const float RateX = Radius.X / OldRadiusX;
		PlankRadius.X *= RateX;
	}
	if (Radius.Y != PlankRadius.Y)
	{
		const float OldRadiusY = PlankRadius.Y;
		const float RateY = Radius.Y / OldRadiusY;
		PlankRadius.Y *= RateY;
	}

	// Makes sure the minimum radius is correct
	if (TopLeftLegMesh && PlankMesh)
	{
		// Precondition: All chair leg dimensions are equal
		MinRadius.X = TopLeftLegMesh->GetMinRadius().X * 3;
		MinRadius.Y = TopLeftLegMesh->GetMinRadius().Y * 3;
		MinRadius.Z = TopLeftLegMesh->GetMinRadius().Z + PlankMesh->GetMinRadius().Z;
	}

	Super::ValidateDimensions();
}
