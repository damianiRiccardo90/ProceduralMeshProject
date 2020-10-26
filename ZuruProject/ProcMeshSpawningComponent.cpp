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
	MeshIndicatorClass = AProcMeshCompositeTable::StaticClass();
	SpawnMeshCollisionProfileName = FName("IndicatorMesh");

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

			if (IndicatorMesh)
			{
				IndicatorMesh->SetActorLocation(HitResult.ImpactPoint + FVector(0.f, 0.f, IndicatorMesh->GetRadius().Z));
			}
		}
	}
}

void UProcMeshSpawningComponent::GenerateIndicatorMesh()
{
	ClearIndicatorMesh();

	if (UWorld* World = GetWorld())
	{
		IndicatorMesh = World->SpawnActor<AProcMeshBase>(
			MeshIndicatorClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			FActorSpawnParameters()
		);
		if (IndicatorMesh)
		{
			IndicatorMesh->SetCollisionProfileName(SpawnMeshCollisionProfileName);
			IndicatorMesh->SetMaterial(DeactivatedStateMaterial);
		}
	}

	BindEvents();
}

void UProcMeshSpawningComponent::ActivateComponent(const bool InbActivate)
{
	if (InbActivate)
	{
		GenerateIndicatorMesh();
	}
	else
	{
		ClearIndicatorMesh();
	}

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (InbActivate)
		{
			FHitResult HitResult;
			PlayerController->GetHitResultUnderCursorForObjects({ UEngineTypes::ConvertToObjectType(ECC_WorldStatic) }, false, HitResult);

			if (IndicatorMesh)
			{
				IndicatorMesh->SetActorLocation(HitResult.ImpactPoint + FVector(0.f, 0.f, IndicatorMesh->GetRadius().Z));
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

void UProcMeshSpawningComponent::ClearIndicatorMesh()
{
	UnbindEvents();

	if (IndicatorMesh)
	{
		IndicatorMesh->ClearMesh();
		IndicatorMesh->Destroy();
	}
}

void UProcMeshSpawningComponent::BindEvents()
{
	if (IndicatorMesh)
	{
		IndicatorMesh->OnActorBeginOverlap.AddDynamic(this, &ThisClass::HandleIndicatorMeshBeginOverlap);
		IndicatorMesh->OnActorEndOverlap.AddDynamic(this, &ThisClass::HandleIndicatorMeshEndOverlap);
	}
}

void UProcMeshSpawningComponent::UnbindEvents()
{
	if (IndicatorMesh)
	{
		IndicatorMesh->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::HandleIndicatorMeshBeginOverlap);
		IndicatorMesh->OnActorEndOverlap.RemoveDynamic(this, &ThisClass::HandleIndicatorMeshEndOverlap);
	}
}

void UProcMeshSpawningComponent::HandleIndicatorMeshBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (IndicatorMesh)
	{
		IndicatorMesh->SetMaterial(DeactivatedStateMaterial);
	}

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PlayerController->CurrentMouseCursor = EMouseCursor::SlashedCircle;
	}

	bCanSpawn = false;
}

void UProcMeshSpawningComponent::HandleIndicatorMeshEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (IndicatorMesh)
	{
		IndicatorMesh->SetMaterial(ActivatedStateMaterial);
	}

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PlayerController->CurrentMouseCursor = EMouseCursor::Hand;
	}

	bCanSpawn = true;
}
