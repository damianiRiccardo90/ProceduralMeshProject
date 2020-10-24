#include "ProcMeshCube.h"
#include <ProceduralMeshComponent.h>
#include "LinearAlgebraFunctionLibrary.h"
#include <Materials/MaterialInterface.h>


AProcMeshCube::AProcMeshCube(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Radius = FVector(100.f, 100.f, 100.f);
	MinRadius = FVector(1.f, 1.f, 1.f);

	CubeMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	CubeMesh->SetupAttachment(SceneComponent);
}

void AProcMeshCube::SetRadius(const FVector& InRadius)
{
	const FVector OldRadius = Radius;
	Radius = InRadius;
	ValidateDimensions();
	const FVector DeltaRadius = Radius - OldRadius;

	// Alter the mesh on both sides equally, this way you do not alter the alignment
	if (DeltaRadius.X > 0)
	{
		ExtrudeMesh(FVector(DeltaRadius.X, 0.f, 0.f));
		ExtrudeMesh(FVector(-DeltaRadius.X, 0.f, 0.f));
	}
	else if (DeltaRadius.X < 0)
	{
		IntrudeMesh(FVector(DeltaRadius.X, 0.f, 0.f));
		IntrudeMesh(FVector(-DeltaRadius.X, 0.f, 0.f));
	}
	if (DeltaRadius.Y > 0)
	{
		ExtrudeMesh(FVector(0.f, DeltaRadius.Y, 0.f));
		ExtrudeMesh(FVector(0.f, -DeltaRadius.Y, 0.f));
	}
	else if (DeltaRadius.Y < 0)
	{
		IntrudeMesh(FVector(0.f, DeltaRadius.Y, 0.f));
		IntrudeMesh(FVector(0.f, -DeltaRadius.Y, 0.f));
	}
	if (DeltaRadius.Z > 0)
	{
		ExtrudeMesh(FVector(0.f, 0.f, DeltaRadius.Z));
		ExtrudeMesh(FVector(0.f, 0.f, -DeltaRadius.Z));
	}
	else if (DeltaRadius.Z < 0)
	{
		IntrudeMesh(FVector(0.f, 0.f, DeltaRadius.Z));
		IntrudeMesh(FVector(0.f, 0.f, -DeltaRadius.Z));
	}
}

void AProcMeshCube::SetMaterial(UMaterialInterface* InMaterial)
{
	if (CubeMesh)
	{
		CubeMesh->SetMaterial(0, InMaterial);
	}
}

void AProcMeshCube::ClearMesh()
{
	if (CubeMesh)
	{
		CubeMesh->ClearAllMeshSections();
	}
	MeshData.ClearData();
}

void AProcMeshCube::GenerateMesh()
{
	// Workaround to avoid messed up transform when actor and mesh do not have the same world transform
	const FTransform ActualTransform = GetTransform();
	SetActorTransform(FTransform(FRotator::ZeroRotator, FVector::ZeroVector));

	ValidateDimensions();
	ClearMesh();

//	  6+------+4
//	  /|     /|
//	0+-+----+2|
//	 | |    | |
//	 |7+----+-+5
//	 |/     |/
//	1+------+3

	FVector FrontTopLeft     = FVector(-Radius.X, Radius.Y, Radius.Z);   // 0
	FVector FrontBottomLeft  = FVector(-Radius.X, Radius.Y, -Radius.Z);  // 1
	FVector FrontTopRight    = FVector(Radius.X, Radius.Y, Radius.Z);    // 2
	FVector FrontBottomRight = FVector(Radius.X, Radius.Y, -Radius.Z);   // 3
	FVector BackTopRight     = FVector(Radius.X, -Radius.Y, Radius.Z);   // 4
	FVector BackBottomRight  = FVector(Radius.X, -Radius.Y, -Radius.Z);  // 5
	FVector BackTopLeft      = FVector(-Radius.X, -Radius.Y, Radius.Z);  // 6
	FVector BackBottomLeft   = FVector(-Radius.X, -Radius.Y, -Radius.Z); // 7

	FrontTopLeft     = ULinearAlgebraFunctionLibrary::TranslatePoint(FVector::ZeroVector, FrontTopLeft);
	FrontBottomLeft  = ULinearAlgebraFunctionLibrary::TranslatePoint(FVector::ZeroVector, FrontBottomLeft);
	FrontTopRight    = ULinearAlgebraFunctionLibrary::TranslatePoint(FVector::ZeroVector, FrontTopRight);
	FrontBottomRight = ULinearAlgebraFunctionLibrary::TranslatePoint(FVector::ZeroVector, FrontBottomRight);
	BackTopRight     = ULinearAlgebraFunctionLibrary::TranslatePoint(FVector::ZeroVector, BackTopRight);
	BackBottomRight  = ULinearAlgebraFunctionLibrary::TranslatePoint(FVector::ZeroVector, BackBottomRight);
	BackTopLeft      = ULinearAlgebraFunctionLibrary::TranslatePoint(FVector::ZeroVector, BackTopLeft);
	BackBottomLeft   = ULinearAlgebraFunctionLibrary::TranslatePoint(FVector::ZeroVector, BackBottomLeft);

	FrontTopLeft     = ULinearAlgebraFunctionLibrary::RotatePoint(FrontTopLeft,     FRotator::ZeroRotator, FVector::ZeroVector);
	FrontBottomLeft  = ULinearAlgebraFunctionLibrary::RotatePoint(FrontBottomLeft,  FRotator::ZeroRotator, FVector::ZeroVector);
	FrontTopRight    = ULinearAlgebraFunctionLibrary::RotatePoint(FrontTopRight,    FRotator::ZeroRotator, FVector::ZeroVector);
	FrontBottomRight = ULinearAlgebraFunctionLibrary::RotatePoint(FrontBottomRight, FRotator::ZeroRotator, FVector::ZeroVector);
	BackTopRight     = ULinearAlgebraFunctionLibrary::RotatePoint(BackTopRight,     FRotator::ZeroRotator, FVector::ZeroVector);
	BackBottomRight  = ULinearAlgebraFunctionLibrary::RotatePoint(BackBottomRight,  FRotator::ZeroRotator, FVector::ZeroVector);
	BackTopLeft      = ULinearAlgebraFunctionLibrary::RotatePoint(BackTopLeft,      FRotator::ZeroRotator, FVector::ZeroVector);
	BackBottomLeft   = ULinearAlgebraFunctionLibrary::RotatePoint(BackBottomLeft,   FRotator::ZeroRotator, FVector::ZeroVector);

	// Front Quad - Vertices[0, 3]
	AddQuadMesh(FrontTopLeft, FrontBottomLeft, FrontTopRight, FrontBottomRight, MeshData);
	// Right Quad - Vertices[4, 7]
	AddQuadMesh(FrontTopRight, FrontBottomRight, BackTopRight, BackBottomRight, MeshData);
	// Back Quad - Vertices[8, 11]
	AddQuadMesh(BackTopRight, BackBottomRight, BackTopLeft, BackBottomLeft, MeshData);
	// Left Quad - Vertices[12, 15]
	AddQuadMesh(BackTopLeft, BackBottomLeft, FrontTopLeft, FrontBottomLeft, MeshData);
	// Bottom Quad - Vertices[16, 19]
	AddQuadMesh(FrontBottomLeft, BackBottomLeft, FrontBottomRight, BackBottomRight, MeshData);
	// Top Quad - Vertices[20, 23]
	AddQuadMesh(BackTopLeft, FrontTopLeft, BackTopRight, FrontTopRight, MeshData);

	MeshData.Colors.Add(FrontTopLeftColor);
	MeshData.Colors.Add(FrontBottomLeftColor);
	MeshData.Colors.Add(FrontTopRightColor);
	MeshData.Colors.Add(FrontBottomRightColor);
	MeshData.Colors.Add(BackTopRightColor);
	MeshData.Colors.Add(BackBottomRightColor);
	MeshData.Colors.Add(BackTopLeftColor);
	MeshData.Colors.Add(BackBottomLeftColor);

	if (CubeMesh)
	{
		CubeMesh->CreateMeshSection_LinearColor(
			0,
			MeshData.Vertices,
			MeshData.Triangles,
			MeshData.Normals,
			MeshData.UVs,
			MeshData.Colors,
			MeshData.Tangents,
			true
		);
	}

	// Workaround to avoid messed up transform when actor and mesh do not have the same world transform
	SetActorTransform(ActualTransform);
}

void AProcMeshCube::ExtrudeMesh(const FVector& InDelta)
{
	if (MeshData.Vertices.Num() == 24)
	{
		if (InDelta.X > 0)
		{
			// Extrude Right Quad - Vertices[4, 7]
			MeshData.Vertices[4].X += InDelta.X;
			MeshData.Vertices[5].X += InDelta.X;
			MeshData.Vertices[6].X += InDelta.X;
			MeshData.Vertices[7].X += InDelta.X;
			// Following up with the remaining vertices
			MeshData.Vertices[2].X += InDelta.X; // Front Quad
			MeshData.Vertices[3].X += InDelta.X;
			MeshData.Vertices[18].X += InDelta.X; // Bottom Quad
			MeshData.Vertices[19].X += InDelta.X;
			MeshData.Vertices[8].X += InDelta.X; // Back Quad
			MeshData.Vertices[9].X += InDelta.X;
			MeshData.Vertices[22].X += InDelta.X; // Top Quad
			MeshData.Vertices[23].X += InDelta.X;
		}
		else if (InDelta.X < 0)
		{
			// Extrude Left Quad - Vertices[12, 15]
			MeshData.Vertices[12].X += InDelta.X;
			MeshData.Vertices[13].X += InDelta.X;
			MeshData.Vertices[14].X += InDelta.X;
			MeshData.Vertices[15].X += InDelta.X;
			// Following up with the remaining vertices
			MeshData.Vertices[0].X += InDelta.X; // Front Quad
			MeshData.Vertices[1].X += InDelta.X;
			MeshData.Vertices[16].X += InDelta.X; // Bottom Quad
			MeshData.Vertices[17].X += InDelta.X;
			MeshData.Vertices[10].X += InDelta.X; // Back Quad
			MeshData.Vertices[11].X += InDelta.X;
			MeshData.Vertices[20].X += InDelta.X; // Top Quad
			MeshData.Vertices[21].X += InDelta.X;
		}
		if (InDelta.Y > 0)
		{
			// Extrude Front Quad - Vertices[0, 3]
			MeshData.Vertices[0].Y += InDelta.Y;
			MeshData.Vertices[1].Y += InDelta.Y;
			MeshData.Vertices[2].Y += InDelta.Y;
			MeshData.Vertices[3].Y += InDelta.Y;
			// Following up with the remaining vertices
			MeshData.Vertices[4].Y += InDelta.Y; // Right Quad
			MeshData.Vertices[5].Y += InDelta.Y;
			MeshData.Vertices[21].Y += InDelta.Y; // Top Quad
			MeshData.Vertices[23].Y += InDelta.Y;
			MeshData.Vertices[14].Y += InDelta.Y; // Left Quad
			MeshData.Vertices[15].Y += InDelta.Y;
			MeshData.Vertices[16].Y += InDelta.Y; // Bottom Quad
			MeshData.Vertices[18].Y += InDelta.Y;
		}
		else if (InDelta.Y < 0)
		{
			// Extrude Back Quad - Vertices[8, 11]
			MeshData.Vertices[8].Y += InDelta.Y;
			MeshData.Vertices[9].Y += InDelta.Y;
			MeshData.Vertices[10].Y += InDelta.Y;
			MeshData.Vertices[11].Y += InDelta.Y;
			// Following up with the remaining vertices
			MeshData.Vertices[6].Y += InDelta.Y; // Right Quad
			MeshData.Vertices[7].Y += InDelta.Y;
			MeshData.Vertices[20].Y += InDelta.Y; // Top Quad
			MeshData.Vertices[22].Y += InDelta.Y;
			MeshData.Vertices[12].Y += InDelta.Y; // Left Quad
			MeshData.Vertices[13].Y += InDelta.Y;
			MeshData.Vertices[17].Y += InDelta.Y; // Bottom Quad
			MeshData.Vertices[19].Y += InDelta.Y;
		}
		if (InDelta.Z > 0)
		{
			// Extrude Top Quad - Vertices[20, 23]
			MeshData.Vertices[20].Z += InDelta.Z;
			MeshData.Vertices[21].Z += InDelta.Z;
			MeshData.Vertices[22].Z += InDelta.Z;
			MeshData.Vertices[23].Z += InDelta.Z;
			// Following up with the remaining vertices
			MeshData.Vertices[0].Z += InDelta.Z; // Front Quad
			MeshData.Vertices[2].Z += InDelta.Z;
			MeshData.Vertices[4].Z += InDelta.Z; // Right Quad
			MeshData.Vertices[6].Z += InDelta.Z;
			MeshData.Vertices[8].Z += InDelta.Z; // Back Quad
			MeshData.Vertices[10].Z += InDelta.Z;
			MeshData.Vertices[12].Z += InDelta.Z; // Left Quad
			MeshData.Vertices[14].Z += InDelta.Z;
		}
		else if (InDelta.Z < 0)
		{
			// Extrude Bottom Quad - Vertices[16, 19]
			MeshData.Vertices[16].Z += InDelta.Z;
			MeshData.Vertices[17].Z += InDelta.Z;
			MeshData.Vertices[18].Z += InDelta.Z;
			MeshData.Vertices[19].Z += InDelta.Z;
			// Following up with the remaining vertices
			MeshData.Vertices[1].Z += InDelta.Z; // Front Quad
			MeshData.Vertices[3].Z += InDelta.Z;
			MeshData.Vertices[5].Z += InDelta.Z; // Right Quad
			MeshData.Vertices[7].Z += InDelta.Z;
			MeshData.Vertices[9].Z += InDelta.Z; // Back Quad
			MeshData.Vertices[11].Z += InDelta.Z;
			MeshData.Vertices[13].Z += InDelta.Z; // Left Quad
			MeshData.Vertices[15].Z += InDelta.Z;
		}
	}
	if (CubeMesh)
	{
		CubeMesh->UpdateMeshSection_LinearColor(
			0,
			MeshData.Vertices,
			MeshData.Normals,
			MeshData.UVs,
			MeshData.Colors,
			MeshData.Tangents
		);
	}
}

void AProcMeshCube::IntrudeMesh(const FVector& InDelta)
{
	if (MeshData.Vertices.Num() == 24)
	{
		if (InDelta.X > 0)
		{
			// Intrude Right Quad - Vertices[4, 7]
			MeshData.Vertices[4].X -= InDelta.X;
			MeshData.Vertices[5].X -= InDelta.X;
			MeshData.Vertices[6].X -= InDelta.X;
			MeshData.Vertices[7].X -= InDelta.X;
			// Following up with the remaining vertices
			MeshData.Vertices[2].X -= InDelta.X; // Front Quad
			MeshData.Vertices[3].X -= InDelta.X;
			MeshData.Vertices[18].X -= InDelta.X; // Bottom Quad
			MeshData.Vertices[19].X -= InDelta.X;
			MeshData.Vertices[8].X -= InDelta.X; // Back Quad
			MeshData.Vertices[9].X -= InDelta.X;
			MeshData.Vertices[22].X -= InDelta.X; // Top Quad
			MeshData.Vertices[23].X -= InDelta.X;
		}
		else if (InDelta.X < 0)
		{
			// Intrude Left Quad - Vertices[12, 15]
			MeshData.Vertices[12].X -= InDelta.X;
			MeshData.Vertices[13].X -= InDelta.X;
			MeshData.Vertices[14].X -= InDelta.X;
			MeshData.Vertices[15].X -= InDelta.X;
			// Following up with the remaining vertices
			MeshData.Vertices[0].X -= InDelta.X; // Front Quad
			MeshData.Vertices[1].X -= InDelta.X;
			MeshData.Vertices[16].X -= InDelta.X; // Bottom Quad
			MeshData.Vertices[17].X -= InDelta.X;
			MeshData.Vertices[10].X -= InDelta.X; // Back Quad
			MeshData.Vertices[11].X -= InDelta.X;
			MeshData.Vertices[20].X -= InDelta.X; // Top Quad
			MeshData.Vertices[21].X -= InDelta.X;
		}
		if (InDelta.Y > 0)
		{
			// Intrude Front Quad - Vertices[0, 3]
			MeshData.Vertices[0].Y -= InDelta.Y;
			MeshData.Vertices[1].Y -= InDelta.Y;
			MeshData.Vertices[2].Y -= InDelta.Y;
			MeshData.Vertices[3].Y -= InDelta.Y;
			// Following up with the remaining vertices
			MeshData.Vertices[4].Y -= InDelta.Y; // Right Quad
			MeshData.Vertices[5].Y -= InDelta.Y;
			MeshData.Vertices[21].Y -= InDelta.Y; // Top Quad
			MeshData.Vertices[23].Y -= InDelta.Y;
			MeshData.Vertices[14].Y -= InDelta.Y; // Left Quad
			MeshData.Vertices[15].Y -= InDelta.Y;
			MeshData.Vertices[16].Y -= InDelta.Y; // Bottom Quad
			MeshData.Vertices[18].Y -= InDelta.Y;
		}
		else if (InDelta.Y < 0)
		{
			// Intrude Back Quad - Vertices[8, 11]
			MeshData.Vertices[8].Y -= InDelta.Y;
			MeshData.Vertices[9].Y -= InDelta.Y;
			MeshData.Vertices[10].Y -= InDelta.Y;
			MeshData.Vertices[11].Y -= InDelta.Y;
			// Following up with the remaining vertices
			MeshData.Vertices[6].Y -= InDelta.Y; // Right Quad
			MeshData.Vertices[7].Y -= InDelta.Y;
			MeshData.Vertices[20].Y -= InDelta.Y; // Top Quad
			MeshData.Vertices[22].Y -= InDelta.Y;
			MeshData.Vertices[12].Y -= InDelta.Y; // Left Quad
			MeshData.Vertices[13].Y -= InDelta.Y;
			MeshData.Vertices[17].Y -= InDelta.Y; // Bottom Quad
			MeshData.Vertices[19].Y -= InDelta.Y;
		}
		if (InDelta.Z > 0)
		{
			// Intrude Top Quad - Vertices[20, 23]
			MeshData.Vertices[20].Z -= InDelta.Z;
			MeshData.Vertices[21].Z -= InDelta.Z;
			MeshData.Vertices[22].Z -= InDelta.Z;
			MeshData.Vertices[23].Z -= InDelta.Z;
			// Following up with the remaining vertices
			MeshData.Vertices[0].Z -= InDelta.Z; // Front Quad
			MeshData.Vertices[2].Z -= InDelta.Z;
			MeshData.Vertices[4].Z -= InDelta.Z; // Right Quad
			MeshData.Vertices[6].Z -= InDelta.Z;
			MeshData.Vertices[8].Z -= InDelta.Z; // Back Quad
			MeshData.Vertices[10].Z -= InDelta.Z;
			MeshData.Vertices[12].Z -= InDelta.Z; // Left Quad
			MeshData.Vertices[14].Z -= InDelta.Z;
		}
		else if (InDelta.Z < 0)
		{
			// Intrude Bottom Quad - Vertices[16, 19]
			MeshData.Vertices[16].Z -= InDelta.Z;
			MeshData.Vertices[17].Z -= InDelta.Z;
			MeshData.Vertices[18].Z -= InDelta.Z;
			MeshData.Vertices[19].Z -= InDelta.Z;
			// Following up with the remaining vertices
			MeshData.Vertices[1].Z -= InDelta.Z; // Front Quad
			MeshData.Vertices[3].Z -= InDelta.Z;
			MeshData.Vertices[5].Z -= InDelta.Z; // Right Quad
			MeshData.Vertices[7].Z -= InDelta.Z;
			MeshData.Vertices[9].Z -= InDelta.Z; // Back Quad
			MeshData.Vertices[11].Z -= InDelta.Z;
			MeshData.Vertices[13].Z -= InDelta.Z; // Left Quad
			MeshData.Vertices[15].Z -= InDelta.Z;
		}
	}
	if (CubeMesh)
	{
		CubeMesh->UpdateMeshSection_LinearColor(
			0,
			MeshData.Vertices,
			MeshData.Normals,
			MeshData.UVs,
			MeshData.Colors,
			MeshData.Tangents
		);
	}
}
