#include "ProceduralMeshProjectGameModeBase.h"
#include "DefaultPlayerController.h"
#include "ProcMeshBase.h"
#include <ProceduralMeshComponent.h>


AProceduralMeshProjectGameModeBase::AProceduralMeshProjectGameModeBase(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = ADefaultPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
	SpectatorClass = nullptr;
}

void AProceduralMeshProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		AProcMeshBase* StartingMesh = World->SpawnActor<AProcMeshBase>(
			StartingMeshClass,
			SpawnOrigin + StartingMeshSpawnLocation,
			StartingMeshSpawnRotation,
			FActorSpawnParameters()
		);
		if (StartingMesh)
		{
			StartingMesh->SetActorLocation(SpawnOrigin + FVector(0.f, 0.f, StartingMesh->GetRadius().Z));

			//TestMeshActor->SetRadius(TestMeshActor->GetRadius() + FVector(-12.5f, 0.f, 0.f));
			//const FVector OldRadius = TestMeshActor->GetRadius();
			//TestMeshActor->Extrude(FVector(0.f, OldRadius.Y * 4, 0.f));
			//TestMeshActor->Intrude(FVector(0.f, OldRadius.Y * 2, 0.f));

			/*
			if (GEngine)
			{
				FString FormattedString = FText::Format(FText::FromString("Gigabro ORIGIN IS --> Xo: {0} Yo: {1} Zo: {2}!"), 
					FText::AsNumber(TestMeshActor->GetActorLocation().X), FText::AsNumber(TestMeshActor->GetActorLocation().Y), FText::AsNumber(TestMeshActor->GetActorLocation().Z)).ToString();
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FormattedString);
			}
			*/
		}
	}
}
