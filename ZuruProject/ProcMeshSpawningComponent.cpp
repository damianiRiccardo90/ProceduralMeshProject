#include "ProcMeshSpawningComponent.h"
#include "ProcMeshBase.h"
#include "ProcMeshCompositeTable.h"
#include <Kismet/GameplayStatics.h>


UProcMeshSpawningComponent::UProcMeshSpawningComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	MeshClass = AProcMeshCompositeTable::StaticClass();
	SpawnMeshCollisionProfileName = FName("SpawnableMesh");

	bIsActive = false;
	bCanSpawn = false;
}

void UProcMeshSpawningComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsActive)
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FHitResult HitResult;
			PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), false, HitResult);

			if (SpawnableMesh)
			{
				SpawnableMesh->SetActorLocation(HitResult.ImpactPoint + FVector(0.f, 0.f, SpawnableMesh->GetRadius().Z));
			}
		}
	}
}

void UProcMeshSpawningComponent::GenerateSpawnableMesh()
{
	ClearSpawnableMesh();

	if (UWorld* World = GetWorld())
	{
		SpawnableMesh = World->SpawnActor<AProcMeshBase>(
			MeshClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			FActorSpawnParameters()
		);
		if (SpawnableMesh)
		{
			SpawnableMesh->SetCollisionProfileName(SpawnMeshCollisionProfileName);
			SpawnableMesh->SetMaterial(DeactivatedStateMaterial);
		}
	}

	BindEvents();
}

void UProcMeshSpawningComponent::ActivateComponent(const bool InbActivate)
{
	if (InbActivate)
	{
		GenerateSpawnableMesh();
	}
	else
	{
		ClearSpawnableMesh();
	}

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (InbActivate)
		{
			FHitResult HitResult;
			PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), false, HitResult);

			if (SpawnableMesh)
			{
				SpawnableMesh->SetActorLocation(HitResult.ImpactPoint + FVector(0.f, 0.f, SpawnableMesh->GetRadius().Z));
			}

			PlayerController->CurrentMouseCursor = EMouseCursor::Hand;
		}
		else
		{
			PlayerController->CurrentMouseCursor = EMouseCursor::Default;
		}
	}

	ActivateSpawning(InbActivate);

	bIsActive = InbActivate;

	PrimaryComponentTick.SetTickFunctionEnable(InbActivate);
}

bool UProcMeshSpawningComponent::IsComponentActive() const
{
	return bIsActive;
}

void UProcMeshSpawningComponent::ActivateSpawning(const bool InbActivate)
{
	bCanSpawn = InbActivate;
}

AProcMeshBase* UProcMeshSpawningComponent::SpawnMesh()
{
	AProcMeshBase* SpawnedMesh = nullptr;

	if (bIsActive && bCanSpawn)
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FHitResult HitResult;
			PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), false, HitResult);

			if (UWorld* World = GetWorld())
			{
				SpawnedMesh = World->SpawnActor<AProcMeshBase>(
					MeshClass,
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					FActorSpawnParameters()
					);
				if (SpawnedMesh)
				{
					SpawnedMesh->SetActorLocation(HitResult.ImpactPoint + FVector(0.f, 0.f, SpawnedMesh->GetRadius().Z));
				}
			}
		}
	}

	return SpawnedMesh;
}

void UProcMeshSpawningComponent::ClearSpawnableMesh()
{
	UnbindEvents();

	if (SpawnableMesh)
	{
		SpawnableMesh->ClearMesh();
		SpawnableMesh->Destroy();
	}
}

void UProcMeshSpawningComponent::BindEvents()
{
	if (SpawnableMesh)
	{
		SpawnableMesh->OnActorBeginOverlap.AddDynamic(this, &ThisClass::HandleSpawnableMeshBeginOverlap);
		SpawnableMesh->OnActorEndOverlap.AddDynamic(this, &ThisClass::HandleSpawnableMeshEndOverlap);
	}
}

void UProcMeshSpawningComponent::UnbindEvents()
{
	if (SpawnableMesh)
	{
		SpawnableMesh->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::HandleSpawnableMeshBeginOverlap);
		SpawnableMesh->OnActorEndOverlap.RemoveDynamic(this, &ThisClass::HandleSpawnableMeshEndOverlap);
	}
}

void UProcMeshSpawningComponent::HandleSpawnableMeshBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (SpawnableMesh)
	{
		SpawnableMesh->SetMaterial(DeactivatedStateMaterial);
	}

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PlayerController->CurrentMouseCursor = EMouseCursor::SlashedCircle;
	}

	bCanSpawn = false;
}

void UProcMeshSpawningComponent::HandleSpawnableMeshEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (SpawnableMesh)
	{
		SpawnableMesh->SetMaterial(ActivatedStateMaterial);
	}

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PlayerController->CurrentMouseCursor = EMouseCursor::Hand;
	}

	bCanSpawn = true;
}
