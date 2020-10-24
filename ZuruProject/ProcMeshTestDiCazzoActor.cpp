#include "ProcMeshTestDiCazzoActor.h"
#include <Components/SceneComponent.h>
#include "ProcMeshCompositeTable.h"


AProcMeshTestDiCazzoActor::AProcMeshTestDiCazzoActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;
}

void AProcMeshTestDiCazzoActor::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		Mesh = World->SpawnActor<AProcMeshCompositeTable>(
			AProcMeshCompositeTable::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			FActorSpawnParameters()
		);
		/*
		if (Mesh)
		{
			Mesh->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

			Mesh->Stretch(FVector(-30.f, 0.f, 0.f));

			Mesh->SetActorLocation(FVector(0.f, 0.f, 5000.f));
		}
		*/
	}

// 	Mesh->SetOrigin(FVector(0.f, -550.f, 0.f));
// 	Mesh->SetRotation(FRotator(0.f, 90.f, 0.f));
//  Mesh->GenerateMesh();
//  Mesh->Stretch(FVector(100.f, 0.f, 0.f));
}
