#include "ProcMeshSpawningComponent.h"
#include "ProcMeshBase.h"
#include "ProcMeshCompositeTable.h"


UProcMeshSpawningComponent::UProcMeshSpawningComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	MeshClass = AProcMeshCompositeTable::StaticClass();
}

void UProcMeshSpawningComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FHitResult HitResult;
		PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), false, HitResult);


	}
}

void UProcMeshSpawningComponent::ActivateComponent()
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FHitResult HitResult;
		PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), false, HitResult);

		if (!SpawnableMesh)
		{
			SpawnableMesh = World->SpawnActor<AProcMeshBase>(
				MeshClass,
				HitResult.ImpactPoint,
				FRotator::ZeroRotator,
				FActorSpawnParameters()
			);
		}
		if (SpawnableMesh)
		{
			SpawnableMesh->AddActorWorldOffset(FVector(0.f, 0.f, SpawnableMesh->GetRadius().Z));
		}

		PrimaryComponentTick.SetTickFunctionEnable(true);
	}
}
