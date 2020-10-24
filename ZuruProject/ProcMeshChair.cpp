#include "ProcMeshChair.h"
#include "ProcMeshCube.h"


AProcMeshChair::AProcMeshChair(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Radius = FVector(25.f, 25.f, 50.f);
	LegRadius = FVector(2.5f, 2.5f, 22.5f);
	SeatRadius = FVector(25.f, 25.f, 2.5f);
	BackrestRadius = FVector(25.f, 2.5f, 25.f);
	MinRadius = FVector(LegRadius.X * 3, LegRadius.Y * 3, MinRadius.Z);
}

void AProcMeshChair::SetRadius(const FVector& InRadius)
{
	const FVector OldRadius = Radius;
	Radius = InRadius;
	ValidateDimensions();
	const FVector DeltaRadius = Radius - OldRadius;

	if (TopLeftLegMesh && TopRightLegMesh && BottomRightLegMesh && BottomLeftLegMesh && SeatMesh && BackrestMesh)
	{
		if (DeltaRadius.X != 0)
		{
			if (DeltaRadius.X > 0)
			{
				SeatMesh->Extrude(FVector(DeltaRadius.X, 0.f, 0.f));
				SeatMesh->Extrude(FVector(-DeltaRadius.X, 0.f, 0.f));
				BackrestMesh->Extrude(FVector(DeltaRadius.X, 0.f, 0.f));
				BackrestMesh->Extrude(FVector(-DeltaRadius.X, 0.f, 0.f));
			}
			else
			{
				SeatMesh->Intrude(FVector(DeltaRadius.X, 0.f, 0.f));
				SeatMesh->Intrude(FVector(-DeltaRadius.X, 0.f, 0.f));
				BackrestMesh->Intrude(FVector(DeltaRadius.X, 0.f, 0.f));
				BackrestMesh->Intrude(FVector(-DeltaRadius.X, 0.f, 0.f));
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
				SeatMesh->Extrude(FVector(0.f, DeltaRadius.Y, 0.f));
				SeatMesh->Extrude(FVector(0.f, -DeltaRadius.Y, 0.f));
			} 
			else
			{
				SeatMesh->Intrude(FVector(0.f, DeltaRadius.Y, 0.f));
				SeatMesh->Intrude(FVector(0.f, -DeltaRadius.Y, 0.f));
			}
			BackrestMesh->AddActorLocalOffset(FVector(0.f, DeltaRadius.Y, 0.f));
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

void AProcMeshChair::SetMaterial(UMaterialInterface* InMaterial)
{
	if (TopLeftLegMesh && TopRightLegMesh && BottomRightLegMesh && BottomLeftLegMesh && SeatMesh && BackrestMesh)
	{
		TopLeftLegMesh->SetMaterial(InMaterial);
		TopRightLegMesh->SetMaterial(InMaterial);
		BottomRightLegMesh->SetMaterial(InMaterial);
		BottomLeftLegMesh->SetMaterial(InMaterial);
		SeatMesh->SetMaterial(InMaterial);
		BackrestMesh->SetMaterial(InMaterial);
	}
}

void AProcMeshChair::ClearMesh()
{
	if (TopLeftLegMesh && TopRightLegMesh && BottomRightLegMesh && BottomLeftLegMesh && SeatMesh && BackrestMesh)
	{
		TopLeftLegMesh->ClearMesh();
		TopRightLegMesh->ClearMesh();
		BottomRightLegMesh->ClearMesh();
		BottomLeftLegMesh->ClearMesh();
		SeatMesh->ClearMesh();
		BackrestMesh->ClearMesh();
	}
}

void AProcMeshChair::GenerateMesh()
{
	ClearMesh();

	SpawnMeshActor<AProcMeshCube>(TopLeftLegMesh, AProcMeshCube::StaticClass());
	SpawnMeshActor<AProcMeshCube>(TopRightLegMesh, AProcMeshCube::StaticClass());
	SpawnMeshActor<AProcMeshCube>(BottomRightLegMesh, AProcMeshCube::StaticClass());
	SpawnMeshActor<AProcMeshCube>(BottomLeftLegMesh, AProcMeshCube::StaticClass());
	SpawnMeshActor<AProcMeshCube>(SeatMesh, AProcMeshCube::StaticClass());
	SpawnMeshActor<AProcMeshCube>(BackrestMesh, AProcMeshCube::StaticClass());

	ValidateDimensions();

	FVector TopLeftLegPosition     = FVector(-(SeatRadius.X - LegRadius.X), -(SeatRadius.Y - LegRadius.Y), -(SeatRadius.Z * 2 + LegRadius.Z));
	FVector TopRightLegPosition    = FVector(SeatRadius.X - LegRadius.X, -(SeatRadius.Y - LegRadius.Y), -(SeatRadius.Z * 2 + LegRadius.Z));
	FVector BottomRightLegPosition = FVector(SeatRadius.X - LegRadius.X, SeatRadius.Y - LegRadius.Y, -(SeatRadius.Z * 2 + LegRadius.Z));
	FVector BottomLeftLegPosition  = FVector(-(SeatRadius.X - LegRadius.X), SeatRadius.Y - LegRadius.Y, -(SeatRadius.Z * 2 + LegRadius.Z));
	FVector SeatPosition           = FVector(0.f, 0.f, -SeatRadius.Z);
	FVector BackrestPosition       = FVector(0.f, SeatRadius.Y - BackrestRadius.Y, BackrestRadius.Z);

	if (TopLeftLegMesh && TopRightLegMesh && BottomRightLegMesh && BottomLeftLegMesh && SeatMesh && BackrestMesh)
	{
		TopLeftLegMesh->SetRadius(LegRadius);
		TopLeftLegMesh->SetActorRelativeLocation(TopLeftLegPosition);

		TopRightLegMesh->SetRadius(LegRadius);
		TopRightLegMesh->SetActorRelativeLocation(TopRightLegPosition);

		BottomRightLegMesh->SetRadius(LegRadius);
		BottomRightLegMesh->SetActorRelativeLocation(BottomRightLegPosition);

		BottomLeftLegMesh->SetRadius(LegRadius);
		BottomLeftLegMesh->SetActorRelativeLocation(BottomLeftLegPosition);

		SeatMesh->SetRadius(SeatRadius);
		SeatMesh->SetActorRelativeLocation(SeatPosition);

		BackrestMesh->SetRadius(BackrestRadius);
		BackrestMesh->SetActorRelativeLocation(BackrestPosition);
	}
}

void AProcMeshChair::ValidateDimensions()
{
	// Makes sure that the components proportion and dimension are correct
	if (Radius.X != SeatRadius.X || Radius.X != BackrestRadius.X)
	{
		const float OldRadiusX = SeatRadius.X;
		const float RateX = Radius.X / OldRadiusX;
		SeatRadius.X *= RateX;
		BackrestRadius.X *= RateX;
	}
	if (Radius.Y != SeatRadius.Y)
	{
		const float OldRadiusY = SeatRadius.Y;
		const float RateY = Radius.Y / OldRadiusY;
		SeatRadius.Y *= RateY;
	}

	// Makes sure the minimum radius is correct
	if (TopLeftLegMesh && SeatMesh && BackrestMesh)
	{
		// Precondition: All chair leg dimensions are equal
		MinRadius.X = TopLeftLegMesh->GetMinRadius().X * 3;
		MinRadius.Y = TopLeftLegMesh->GetMinRadius().Y * 3;
		MinRadius.Z = TopLeftLegMesh->GetMinRadius().Z + SeatMesh->GetMinRadius().Z + BackrestMesh->GetMinRadius().Z;
	}

	Super::ValidateDimensions();
}
