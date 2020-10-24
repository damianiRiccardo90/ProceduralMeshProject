#include "ZuruProjectGameModeBase.h"
#include <Kismet/GameplayStatics.h>
#include "DefaultPlayerController.h"
#include "FixedEditingCamera.h"
#include "FreeSpectatorPawn.h"
#include "ProcMeshCompositeTable.h"
#include <ProceduralMeshComponent.h>


AZuruProjectGameModeBase::AZuruProjectGameModeBase(const class FObjectInitializer& ObjectInitializer)
{
	PlayerControllerClass = ADefaultPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
	SpectatorClass = nullptr;
}

void AZuruProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		if (ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		{
			PlayerController->EditingCamera = World->SpawnActor<AFixedEditingCamera>(
				AFixedEditingCamera::StaticClass(),
				SpawnOrigin + FixedCameraSpawnLocation,
				FixedCameraSpawnRotation,
				FActorSpawnParameters()
			);

			PlayerController->FreeSpectator = World->SpawnActor<AFreeSpectatorPawn>(
				AFreeSpectatorPawn::StaticClass(),
				SpawnOrigin + FreeSpectatorSpawnLocation,
				FreeSpectatorSpawnRotation,
				FActorSpawnParameters()
			);
		}

		AProcMeshBase* TestMeshActor = World->SpawnActor<AProcMeshBase>(
			TableClass,
			SpawnOrigin + TestMeshSpawnLocation,
			TestMeshSpawnRotation,
			FActorSpawnParameters()
		);
		if (TestMeshActor)
		{
			TestMeshActor->SetActorLocation(SpawnOrigin + FVector(0.f, 0.f, TestMeshActor->GetRadius().Z));

			//TestMeshActor->SetRadius(TestMeshActor->GetRadius() + FVector(-12.5f, 0.f, 0.f));
			//const FVector OldRadius = TestMeshActor->GetRadius();
			//TestMeshActor->Extrude(FVector(0.f, OldRadius.Y * 4, 0.f));
			//TestMeshActor->Intrude(FVector(0.f, OldRadius.Y * 2, 0.f));

			/*
			if (GEngine)
			{
				FString FormattedString = FText::Format(FText::FromString("The O'Bow Lya ORIGIN IS --> Xo: {0} Yo: {1} Zo: {2}!"), 
					FText::AsNumber(TestMeshActor->GetActorLocation().X), FText::AsNumber(TestMeshActor->GetActorLocation().Y), FText::AsNumber(TestMeshActor->GetActorLocation().Z)).ToString();
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FormattedString);
			}
			*/
		}
	}
}
