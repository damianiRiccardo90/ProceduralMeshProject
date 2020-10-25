#include "ProcMeshCompositeTable.h"
#include "ProcMeshTable.h"
#include "ProcMeshChair.h"
#include "LinearAlgebraFunctionLibrary.h"


AProcMeshCompositeTable::AProcMeshCompositeTable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Radius = FVector(100.f, 100.f, 42.5);
	MinChairDistance = 20.f;
	MinChairNumber = 1;

	TableClass = AProcMeshTable::StaticClass();
	ChairClass = AProcMeshChair::StaticClass();
}

void AProcMeshCompositeTable::SetRadius(const FVector& InRadius)
{
	const FVector OldRadius = Radius;
	Radius = InRadius;

	ComputeDistances();
	ValidateDimensions();
	
	SetupChairs();
}

void AProcMeshCompositeTable::SetCollisionProfileName(const FName& InProfileName)
{
	if (InProfileName != NAME_None)
	{
		if (TableMesh)
		{
			TableMesh->SetCollisionProfileName(InProfileName);
		}
		for (AProcMeshChair* TopSideChair : TopSideChairs)
		{
			if (TopSideChair)
			{
				TopSideChair->SetCollisionProfileName(InProfileName);
			}
		}
		for (AProcMeshChair* RightSideChair : RightSideChairs)
		{
			if (RightSideChair)
			{
				RightSideChair->SetCollisionProfileName(InProfileName);
			}
		}
		for (AProcMeshChair* BottomSideChair : BottomSideChairs)
		{
			if (BottomSideChair)
			{
				BottomSideChair->SetCollisionProfileName(InProfileName);
			}
		}
		for (AProcMeshChair* LeftSideChair : LeftSideChairs)
		{
			if (LeftSideChair)
			{
				LeftSideChair->SetCollisionProfileName(InProfileName);
			}
		}
	}
}

void AProcMeshCompositeTable::SetMaterial(UMaterialInterface* InMaterial)
{
	// If MainMaterial is not set via BP, try using internal materials for chairs and table
	if (TableMesh)
	{
		TableMesh->SetMaterial(InMaterial ? InMaterial : TableMaterial);
	}
	for (AProcMeshChair* TopSideChair : TopSideChairs)
	{
		if (TopSideChair)
		{
			TopSideChair->SetMaterial(InMaterial ? InMaterial : ChairsMaterial);
		}
	}
	for (AProcMeshChair* RightSideChair : RightSideChairs)
	{
		if (RightSideChair)
		{
			RightSideChair->SetMaterial(InMaterial ? InMaterial : ChairsMaterial);
		}
	}
	for (AProcMeshChair* BottomSideChair : BottomSideChairs)
	{
		if (BottomSideChair)
		{
			BottomSideChair->SetMaterial(InMaterial ? InMaterial : ChairsMaterial);
		}
	}
	for (AProcMeshChair* LeftSideChair : LeftSideChairs)
	{
		if (LeftSideChair)
		{
			LeftSideChair->SetMaterial(InMaterial ? InMaterial : ChairsMaterial);
		}
	}
}

void AProcMeshCompositeTable::ClearMesh()
{
	if (TableMesh)
	{
		TableMesh->ClearMesh();
		TableMesh->Destroy();
	}
	for (TArray<AProcMeshChair*> ChairArray : { TopSideChairs, RightSideChairs, BottomSideChairs, LeftSideChairs })
	{
		for (AProcMeshChair* ChairMesh : ChairArray)
		{
			if (ChairMesh)
			{
				TableMesh->ClearMesh();
				TableMesh->Destroy();
			}
		}
		ChairArray.Empty();
	}
}

void AProcMeshCompositeTable::GenerateMesh()
{
	ClearMesh();

	// We need to do this beforehand so we can fetch the default validated dimensions
	GenerateInitialMeshes();

	ComputeDistances();

	ValidateDimensions();

	SetupChairs();

	SetCollisionProfileName(CollisionProfileName);
	SetMaterial(MainMaterial);
}

void AProcMeshCompositeTable::GenerateInitialMeshes()
{
	// Generates table mesh and one chair per side
	// Creates new meshes only if necessary
	// Precondition: TopSideChairs.Num() == BottomSideChair.Num()
	if (TopSideChairs.Num() == 0)
	{
		AProcMeshChair* TopSideChairMesh = nullptr;
		AProcMeshChair* BottomSideChairMesh = nullptr;
		SpawnMeshActor<AProcMeshChair>(TopSideChairMesh, ChairClass);
		SpawnMeshActor<AProcMeshChair>(BottomSideChairMesh, ChairClass);
		TopSideChairs.Add(TopSideChairMesh);
		BottomSideChairs.Add(BottomSideChairMesh);
	}
	// Precondition: LeftSideChairs.Num() == RightSideChair.Num()
	if (LeftSideChairs.Num() == 0)
	{
		AProcMeshChair* LeftSideChairMesh = nullptr;
		AProcMeshChair* RightSideChairMesh = nullptr;
		SpawnMeshActor<AProcMeshChair>(LeftSideChairMesh, ChairClass);
		SpawnMeshActor<AProcMeshChair>(RightSideChairMesh, ChairClass);
		LeftSideChairs.Add(LeftSideChairMesh);
		RightSideChairs.Add(RightSideChairMesh);
	}
	if (TopSideChairs.IsValidIndex(0) && TopSideChairs[0] && !TableMesh)
	{
		SpawnMeshActor<AProcMeshTable>(TableMesh, TableClass);
		if (TableMesh)
		{
			TableMesh->SetRadius(Radius);
			TableMesh->SetActorRelativeLocation(FVector(0.f, 0.f, -((TopSideChairs[0]->GetRadius().Z - Radius.Z) * 2)));
		}
	}
}

void AProcMeshCompositeTable::SetupChairs()
{
	// Creating, positioning and rotating chairs on both the top and bottom side (longitudinal)
	// Precondition: TopSideChairs.Num() == BottomSideChair.Num()
	const int32 Tran_MaxIndex = FMath::Max(Tran_MaxChairNum, TopSideChairs.Num());
	for (int32 i = 0; i < Tran_MaxIndex; i++)
	{
		AProcMeshChair* TopSideChairMesh = nullptr;
		AProcMeshChair* BottomSideChairMesh = nullptr;

		// Creates new chairs (only if it needs to) and removes the extra ones
		if (i < Tran_MaxChairNum && i >= TopSideChairs.Num())
		{
			// Needs to spawn new chairs because we do not have enough of them
			SpawnMeshActor<AProcMeshChair>(TopSideChairMesh, ChairClass);
			SpawnMeshActor<AProcMeshChair>(BottomSideChairMesh, ChairClass);
			TopSideChairs.Add(TopSideChairMesh);
			BottomSideChairs.Add(BottomSideChairMesh);
		}
		else if (i < Tran_MaxChairNum)
		{
			// Already has a chair available
			if (TopSideChairs.IsValidIndex(i) && BottomSideChairs.IsValidIndex(i))
			{
				TopSideChairMesh = TopSideChairs[i];
				BottomSideChairMesh = BottomSideChairs[i];
			}
		}
		else if (i >= Tran_MaxChairNum)
		{
			// Removes extra chairs
			AProcMeshChair* RemovedTopSideChair = TopSideChairs.Pop();
			AProcMeshChair* RemovedBottomSideChair = BottomSideChairs.Pop();
			if (RemovedTopSideChair && RemovedBottomSideChair)
			{
				RemovedTopSideChair->Destroy();
				RemovedBottomSideChair->Destroy();
			}
		}

		if (TopSideChairMesh)
		{
			const FVector TopStartingPosition = FVector(
				-Radius.X + TableLegDiameterX + (TopSideChairMesh->GetRadius().X + Tran_MaxChairDistance),
				-Radius.Y,
				TopSideChairMesh->GetRadius().Z - Radius.Z
			);
			const FVector TopChairPosition = FVector(
				TopStartingPosition.X + (ChairDiameterX + Tran_MaxChairDistance) * i,
				TopStartingPosition.Y,
				TopStartingPosition.Z
			);
			TopSideChairMesh->SetActorRelativeTransform(FTransform(FRotator(0.f, 180.f, 0.f), TopChairPosition));
		}

		if (BottomSideChairMesh)
		{
			const FVector BottomStartingPosition = FVector(
				-Radius.X + TableLegDiameterX + (BottomSideChairMesh->GetRadius().X + Tran_MaxChairDistance),
				Radius.Y,
				BottomSideChairMesh->GetRadius().Z - Radius.Z
			);
			const FVector BottomChairPosition = FVector(
				BottomStartingPosition.X + (ChairDiameterX + Tran_MaxChairDistance) * i,
				BottomStartingPosition.Y,
				BottomStartingPosition.Z
			);
			BottomSideChairMesh->SetActorRelativeLocation(BottomChairPosition);
		}
	}

	// Creating, positioning and rotating chairs on both the left and right side (transversal)
	// Precondition: LeftSideChairs.Num() == RightSideChair.Num()
	const int32 Long_MaxIndex = FMath::Max(Long_MaxChairNum, LeftSideChairs.Num());
	for (int32 j = 0; j < Long_MaxIndex; j++)
	{
		AProcMeshChair* LeftSideChairMesh = nullptr;
		AProcMeshChair* RightSideChairMesh = nullptr;

		// Creates new chairs (only if it needs to) and removes the extra ones
		if (j < Long_MaxChairNum && j >= LeftSideChairs.Num())
		{
			// Needs to spawn new chairs because we do not have enough of them
			SpawnMeshActor<AProcMeshChair>(LeftSideChairMesh, ChairClass);
			SpawnMeshActor<AProcMeshChair>(RightSideChairMesh, ChairClass);
			LeftSideChairs.Add(LeftSideChairMesh);
			RightSideChairs.Add(RightSideChairMesh);
		}
		else if (j < Long_MaxChairNum)
		{
			// Already has a chair available
			if (LeftSideChairs.IsValidIndex(j) && RightSideChairs.IsValidIndex(j))
			{
				LeftSideChairMesh = LeftSideChairs[j];
				RightSideChairMesh = RightSideChairs[j];
			}
		}
		else if (j >= Long_MaxChairNum)
		{
			// Removes extra chairs
			AProcMeshChair* RemovedLeftSideChair = LeftSideChairs.Pop();
			AProcMeshChair* RemovedRightSideChair = RightSideChairs.Pop();
			if (RemovedLeftSideChair && RemovedRightSideChair)
			{
				RemovedLeftSideChair->ClearMesh();
				RemovedRightSideChair->ClearMesh();
			}
		}
		
		if (LeftSideChairMesh)
		{
			const FVector LeftStartingPosition = FVector(
				-Radius.X,
				-Radius.Y + TableLegDiameterY + (LeftSideChairMesh->GetRadius().X + Long_MaxChairDistance),
				LeftSideChairMesh->GetRadius().Z - Radius.Z
			);
			const FVector LeftChairPosition = FVector(
				LeftStartingPosition.X,
				LeftStartingPosition.Y + (ChairDiameterX + Long_MaxChairDistance) * j,
				LeftStartingPosition.Z
			);
			LeftSideChairMesh->SetActorRelativeTransform(FTransform(FRotator(0.f, 90.f, 0.f), LeftChairPosition));
		}

		if (RightSideChairMesh)
		{
			const FVector RightStartingPosition = FVector(
				Radius.X,
				-Radius.Y + TableLegDiameterY + (RightSideChairMesh->GetRadius().X + Long_MaxChairDistance),
				RightSideChairMesh->GetRadius().Z - Radius.Z
			);
			const FVector RightChairPosition = FVector(
				RightStartingPosition.X,
				RightStartingPosition.Y + (ChairDiameterX + Long_MaxChairDistance) * j,
				RightStartingPosition.Z
			);
			RightSideChairMesh->SetActorRelativeTransform(FTransform(FRotator(0.f, -90.f, 0.f), RightChairPosition));
		}
	}
}

void AProcMeshCompositeTable::ComputeDistances()
{
	TableDiameterX = Radius.X * 2;
	TableDiameterY = Radius.Y * 2;
	if (TableMesh)
	{
		TableLegDiameterX = TableMesh->GetLegRadius().X * 2;
		TableLegDiameterY = TableMesh->GetLegRadius().Y * 2;
	}
	if (TopSideChairs.IsValidIndex(0) && TopSideChairs[0])
	{
		ChairDiameterX = TopSideChairs[0]->GetRadius().X * 2;
	}
	Tran_ChairSpace = TableDiameterX - (TableLegDiameterX * 2);
	Long_ChairSpace = TableDiameterY - (TableLegDiameterY * 2);

	// We need to enforce this equation: ChairSpace == ChairDiameter * ChairNum + ChairDistance * (ChairNum + 1)
	// We compute the chair number first, and then we can find the max chair distance 

	// Transversal calculations =================>
	if ((ChairDiameterX + MinChairDistance * 2) < Tran_ChairSpace)
	{
		// Snapping to the lower number of chairs with the current space available
		Tran_MaxChairNum = FMath::FloorToInt((Tran_ChairSpace - MinChairDistance) / (ChairDiameterX + MinChairDistance));
		// Computing the chair distance given the number of chairs
		Tran_MaxChairDistance = (Tran_ChairSpace - Tran_MaxChairNum * ChairDiameterX) / (Tran_MaxChairNum + 1);
	}
	else
	{
		Tran_MaxChairNum = 1;
		Tran_MaxChairDistance = MinChairDistance;
	}
	// <================= Transversal calculations

	// Longitudinal calculations =================>
	if ((ChairDiameterX + MinChairDistance * 2) < Long_ChairSpace)
	{
		Long_MaxChairNum = FMath::FloorToInt((Long_ChairSpace - MinChairDistance) / (ChairDiameterX + MinChairDistance));
		Long_MaxChairDistance = (Long_ChairSpace - Long_MaxChairNum * ChairDiameterX) / (Long_MaxChairNum + 1);
	}
	else
	{
		Long_MaxChairNum = 1;
		Long_MaxChairDistance = MinChairDistance;
	}
	// <================= Longitudinal calculations
}

void AProcMeshCompositeTable::ValidateDimensions()
{
	// Makes sure min chair distance is non-negative
	if (MinChairDistance <= 0)
	{
		MinChairDistance = 10.f;
	}

	// Makes sure there is at least one chair
	if (MinChairNumber < 1)
	{
		MinChairNumber = 1;
	}

	// Computing MinRadius
	// Precondition: ChairDiameter.X == ChairDiameter.Y
	MinRadius.X = (TableLegDiameterX * 2 + ChairDiameterX * MinChairNumber + MinChairDistance * (MinChairNumber + 1)) / 2;
	MinRadius.Y = (TableLegDiameterY * 2 + ChairDiameterX * MinChairNumber + MinChairDistance * (MinChairNumber + 1)) / 2;

	Super::ValidateDimensions();

	// Makes sure that the components proportion and dimensions are correct
	if (TableMesh && Radius.X != TableMesh->GetRadius().X)
	{
		const float OldRadiusX = TableMesh->GetRadius().X;
		const float RateX = Radius.X / OldRadiusX;
		TableMesh->SetRadius(FVector(TableMesh->GetRadius().X * RateX, TableMesh->GetRadius().Y, TableMesh->GetRadius().Z));
	}
	if (TableMesh && Radius.Y != TableMesh->GetRadius().Y)
	{
		const float OldRadiusY = TableMesh->GetRadius().Y;
		const float RateY = Radius.Y / OldRadiusY;
		TableMesh->SetRadius(FVector(TableMesh->GetRadius().X, TableMesh->GetRadius().Y * RateY, TableMesh->GetRadius().Z));
	}
}
