#include "ProcMeshScalingComponent.h"
#include <Kismet/GameplayStatics.h>
#include "LinearAlgebraFunctionLibrary.h"
#include "ProcMeshBase.h"
#include "ProcMeshCube.h"
#include <Engine/GameViewportClient.h>
#include <DrawDebugHelpers.h>


UProcMeshScalingComponent::UProcMeshScalingComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	GrabHandleRadius = FVector(15.f, 15.f, 15.f);
	GrabbedHandle = EGrabbedHandle::NONE;
}

void UProcMeshScalingComponent::BeginPlay()
{
	Super::BeginPlay();

	GenerateGrabHandles();

	BindEvents();
}

void UProcMeshScalingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UnbindEvents();

	ClearHandles();
}

void UProcMeshScalingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FVector CursorLocation;
		FVector CursorDirection;
		PlayerController->DeprojectMousePositionToWorld(CursorLocation, CursorDirection);
		FHitResult HitResult;
		PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), false, HitResult);
		const FVector RayDirection = HitResult.ImpactPoint - CursorLocation;

		if (AProcMeshBase* OwnerActor = Cast<AProcMeshBase>(GetOwner()))
		{
			if (TopLeftGrabHandle && BottomLeftGrabHandle && TopRightGrabHandle && BottomRightGrabHandle)
			{
				// Make sure plane normal * line != 0 (not orthogonal)
				if (FVector::DotProduct(PlaneNormal, RayDirection) != 0)
				{
					const FVector Intersection = FMath::LinePlaneIntersection(CursorLocation, HitResult.ImpactPoint, UpperSurfacePlane);
					if (GrabbedHandle == EGrabbedHandle::TopLeft)
					{
						const FVector TopLeftHandleLoc = TopLeftGrabHandle->GetActorLocation();
						const FVector StretchVector = Intersection - TopLeftHandleLoc;
						if (StretchVector.X > 0)
						{
							OwnerActor->Intrude(FVector(-FMath::Abs(StretchVector.X), 0.f, 0.f));
						}
						else if (StretchVector.X < 0)
						{
							OwnerActor->Extrude(FVector(-FMath::Abs(StretchVector.X), 0.f, 0.f));
						}
						if (StretchVector.Y > 0)
						{
							OwnerActor->Intrude(FVector(0.f, -FMath::Abs(StretchVector.Y), 0.f));
						}
						else if (StretchVector.Y < 0)
						{
							OwnerActor->Extrude(FVector(0.f, -FMath::Abs(StretchVector.Y), 0.f));
						}

					}
					else if (GrabbedHandle == EGrabbedHandle::BottomLeft)
					{
						const FVector BottomLeftHandleLoc = BottomLeftGrabHandle->GetActorLocation();
						const FVector StretchVector = Intersection - BottomLeftHandleLoc;
						if (StretchVector.X > 0)
						{
							OwnerActor->Intrude(FVector(-FMath::Abs(StretchVector.X), 0.f, 0.f));
						}
						else if (StretchVector.X < 0)
						{
							OwnerActor->Extrude(FVector(-FMath::Abs(StretchVector.X), 0.f, 0.f));
						}
						if (StretchVector.Y > 0)
						{
							OwnerActor->Extrude(FVector(0.f, FMath::Abs(StretchVector.Y), 0.f));
						}
						else if (StretchVector.Y < 0)
						{
							OwnerActor->Intrude(FVector(0.f, FMath::Abs(StretchVector.Y), 0.f));
						}
					}
					else if (GrabbedHandle == EGrabbedHandle::TopRight)
					{
						const FVector TopRightHandleLoc = TopRightGrabHandle->GetActorLocation();
						const FVector StretchVector = Intersection - TopRightHandleLoc;
						if (StretchVector.X > 0)
						{
							OwnerActor->Extrude(FVector(FMath::Abs(StretchVector.X), 0.f, 0.f));
						}
						else if (StretchVector.X < 0)
						{
							OwnerActor->Intrude(FVector(FMath::Abs(StretchVector.X), 0.f, 0.f));
						}
						if (StretchVector.Y > 0)
						{
							OwnerActor->Intrude(FVector(0.f, -FMath::Abs(StretchVector.Y), 0.f));
						}
						else if (StretchVector.Y < 0)
						{
							OwnerActor->Extrude(FVector(0.f, -FMath::Abs(StretchVector.Y), 0.f));
						}
					}
					else if (GrabbedHandle == EGrabbedHandle::BottomRight)
					{
						const FVector BottomRightHandleLoc = BottomRightGrabHandle->GetActorLocation();
						const FVector StretchVector = Intersection - BottomRightHandleLoc;
						if (StretchVector.X > 0)
						{
							OwnerActor->Extrude(FVector(FMath::Abs(StretchVector.X), 0.f, 0.f));
						}
						else if (StretchVector.X < 0)
						{
							OwnerActor->Intrude(FVector(FMath::Abs(StretchVector.X), 0.f, 0.f));
						}
						if (StretchVector.Y > 0)
						{
							OwnerActor->Extrude(FVector(0.f, FMath::Abs(StretchVector.Y), 0.f));
						}
						else if (StretchVector.Y < 0)
						{
							OwnerActor->Intrude(FVector(0.f, FMath::Abs(StretchVector.Y), 0.f));
						}
					}
				}
			}
		}
	}

	ResetHandlesLocation();
}

void UProcMeshScalingComponent::GenerateGrabHandles()
{
	ClearHandles();

	if (UWorld* World = GetWorld())
	{
		if (AProcMeshBase* OwnerActor = Cast<AProcMeshBase>(GetOwner()))
		{
			TopLeftGrabHandle = World->SpawnActor<AProcMeshCube>(
				GrabHandleClass,
				OwnerActor->GetActorLocation(),
				OwnerActor->GetActorRotation(),
				FActorSpawnParameters()
			);
			ensureMsgf(TopLeftGrabHandle, TEXT("Object of type %s has not been able to initialize its mesh actor."), *StaticClass()->GetFName().ToString());
			if (TopLeftGrabHandle)
			{
				TopLeftGrabHandle->AttachToActor(OwnerActor, FAttachmentTransformRules::KeepRelativeTransform);
				TopLeftGrabHandle->SetRadius(GrabHandleRadius);
			}

			BottomLeftGrabHandle = World->SpawnActor<AProcMeshCube>(
				GrabHandleClass,
				OwnerActor->GetActorLocation(),
				OwnerActor->GetActorRotation(),
				FActorSpawnParameters()
			);
			ensureMsgf(BottomLeftGrabHandle, TEXT("Object of type %s has not been able to initialize its mesh actor."), *StaticClass()->GetFName().ToString());
			if (BottomLeftGrabHandle)
			{
				BottomLeftGrabHandle->AttachToActor(OwnerActor, FAttachmentTransformRules::KeepRelativeTransform);
				BottomLeftGrabHandle->SetRadius(GrabHandleRadius);
			}

			TopRightGrabHandle = World->SpawnActor<AProcMeshCube>(
				GrabHandleClass,
				OwnerActor->GetActorLocation(),
				OwnerActor->GetActorRotation(),
				FActorSpawnParameters()
			);
			ensureMsgf(TopRightGrabHandle, TEXT("Object of type %s has not been able to initialize its mesh actor."), *StaticClass()->GetFName().ToString());
			if (TopRightGrabHandle)
			{
				TopRightGrabHandle->AttachToActor(OwnerActor, FAttachmentTransformRules::KeepRelativeTransform);
				TopRightGrabHandle->SetRadius(GrabHandleRadius);
			}

			BottomRightGrabHandle = World->SpawnActor<AProcMeshCube>(
				GrabHandleClass,
				OwnerActor->GetActorLocation(),
				OwnerActor->GetActorRotation(),
				FActorSpawnParameters()
			);
			ensureMsgf(BottomRightGrabHandle, TEXT("Object of type %s has not been able to initialize its mesh actor."), *StaticClass()->GetFName().ToString());
			if (BottomRightGrabHandle)
			{
				BottomRightGrabHandle->AttachToActor(OwnerActor, FAttachmentTransformRules::KeepRelativeTransform);
				BottomRightGrabHandle->SetRadius(GrabHandleRadius);
			}
		}
	}

	ResetHandlesLocation();
}

void UProcMeshScalingComponent::ClearHandles()
{
	if (TopLeftGrabHandle && BottomLeftGrabHandle && TopRightGrabHandle && BottomRightGrabHandle)
	{
		TopLeftGrabHandle->ClearMesh();
		BottomLeftGrabHandle->ClearMesh();
		TopRightGrabHandle->ClearMesh();
		BottomRightGrabHandle->ClearMesh();
	}
}

void UProcMeshScalingComponent::ResetHandlesLocation()
{
	if (AProcMeshBase* OwnerActor = Cast<AProcMeshBase>(GetOwner()))
	{
		const FVector TopLeftHandleRelativeLocation     = FVector(-OwnerActor->GetRadius().X, -OwnerActor->GetRadius().Y, OwnerActor->GetRadius().Z);
		const FVector BottomLeftHandleRelativeLocation  = FVector(-OwnerActor->GetRadius().X, OwnerActor->GetRadius().Y, OwnerActor->GetRadius().Z);
		const FVector TopRighHandleRelativeLocation     = FVector(OwnerActor->GetRadius().X, -OwnerActor->GetRadius().Y, OwnerActor->GetRadius().Z);
		const FVector BottomRightHandleRelativeLocation = FVector(OwnerActor->GetRadius().X, OwnerActor->GetRadius().Y, OwnerActor->GetRadius().Z);

		if (TopLeftGrabHandle && BottomLeftGrabHandle && TopRightGrabHandle && BottomRightGrabHandle)
		{
			TopLeftGrabHandle->SetActorRelativeLocation(TopLeftHandleRelativeLocation);
			BottomLeftGrabHandle->SetActorRelativeLocation(BottomLeftHandleRelativeLocation);
			TopRightGrabHandle->SetActorRelativeLocation(TopRighHandleRelativeLocation);
			BottomRightGrabHandle->SetActorRelativeLocation(BottomRightHandleRelativeLocation);
		}
	}
}

void UProcMeshScalingComponent::BindEvents()
{
	if (TopLeftGrabHandle && BottomLeftGrabHandle && TopRightGrabHandle && BottomRightGrabHandle)
	{
		TopLeftGrabHandle->OnBeginCursorOver.AddDynamic(this, &ThisClass::HandleBeginCursorOver);
		TopLeftGrabHandle->OnEndCursorOver.AddDynamic(this, &ThisClass::HandleEndCursorOver);
		TopLeftGrabHandle->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
		TopLeftGrabHandle->OnReleased.AddDynamic(this, &ThisClass::HandleReleased);

		BottomLeftGrabHandle->OnBeginCursorOver.AddDynamic(this, &ThisClass::HandleBeginCursorOver);
		BottomLeftGrabHandle->OnEndCursorOver.AddDynamic(this, &ThisClass::HandleEndCursorOver);
		BottomLeftGrabHandle->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
		BottomLeftGrabHandle->OnReleased.AddDynamic(this, &ThisClass::HandleReleased);

		TopRightGrabHandle->OnBeginCursorOver.AddDynamic(this, &ThisClass::HandleBeginCursorOver);
		TopRightGrabHandle->OnEndCursorOver.AddDynamic(this, &ThisClass::HandleEndCursorOver);
		TopRightGrabHandle->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
		TopRightGrabHandle->OnReleased.AddDynamic(this, &ThisClass::HandleReleased);

		BottomRightGrabHandle->OnBeginCursorOver.AddDynamic(this, &ThisClass::HandleBeginCursorOver);
		BottomRightGrabHandle->OnEndCursorOver.AddDynamic(this, &ThisClass::HandleEndCursorOver);
		BottomRightGrabHandle->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
		BottomRightGrabHandle->OnReleased.AddDynamic(this, &ThisClass::HandleReleased);
	}
}

void UProcMeshScalingComponent::UnbindEvents()
{
	if (TopLeftGrabHandle && BottomLeftGrabHandle && TopRightGrabHandle && BottomRightGrabHandle)
	{
		TopLeftGrabHandle->OnBeginCursorOver.RemoveDynamic(this, &ThisClass::HandleBeginCursorOver);
		TopLeftGrabHandle->OnEndCursorOver.RemoveDynamic(this, &ThisClass::HandleEndCursorOver);
		TopLeftGrabHandle->OnClicked.RemoveDynamic(this, &ThisClass::HandleClicked);
		TopLeftGrabHandle->OnReleased.RemoveDynamic(this, &ThisClass::HandleReleased);

		BottomLeftGrabHandle->OnBeginCursorOver.RemoveDynamic(this, &ThisClass::HandleBeginCursorOver);
		BottomLeftGrabHandle->OnEndCursorOver.RemoveDynamic(this, &ThisClass::HandleEndCursorOver);
		BottomLeftGrabHandle->OnClicked.RemoveDynamic(this, &ThisClass::HandleClicked);
		BottomLeftGrabHandle->OnReleased.RemoveDynamic(this, &ThisClass::HandleReleased);

		TopRightGrabHandle->OnBeginCursorOver.RemoveDynamic(this, &ThisClass::HandleBeginCursorOver);
		TopRightGrabHandle->OnEndCursorOver.RemoveDynamic(this, &ThisClass::HandleEndCursorOver);
		TopRightGrabHandle->OnClicked.RemoveDynamic(this, &ThisClass::HandleClicked);
		TopRightGrabHandle->OnReleased.RemoveDynamic(this, &ThisClass::HandleReleased);

		BottomRightGrabHandle->OnBeginCursorOver.RemoveDynamic(this, &ThisClass::HandleBeginCursorOver);
		BottomRightGrabHandle->OnEndCursorOver.RemoveDynamic(this, &ThisClass::HandleEndCursorOver);
		BottomRightGrabHandle->OnClicked.RemoveDynamic(this, &ThisClass::HandleClicked);
		BottomRightGrabHandle->OnReleased.RemoveDynamic(this, &ThisClass::HandleReleased);
	}
}

void UProcMeshScalingComponent::HandleBeginCursorOver(AActor* TouchedActor)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PlayerController->CurrentMouseCursor = EMouseCursor::GrabHand;
	}
}

void UProcMeshScalingComponent::HandleEndCursorOver(AActor* TouchedActor)
{
// 	HandleReleased(TouchedActor, FKey());
// 	
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PlayerController->CurrentMouseCursor = EMouseCursor::Default;
	}
}

void UProcMeshScalingComponent::HandleClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FVector CursorLocation;
		FVector CursorDirection;
		PlayerController->DeprojectMousePositionToWorld(CursorLocation, CursorDirection);

		const FVector TopLeftHandleLoc = TopLeftGrabHandle->GetActorLocation();
		const FVector TopRightHandleLoc = TopRightGrabHandle->GetActorLocation();
		const FVector BottomLeftHandleLoc = BottomLeftGrabHandle->GetActorLocation();
		PlaneNormal = FVector::CrossProduct(TopRightHandleLoc - TopLeftHandleLoc, BottomLeftHandleLoc - TopLeftHandleLoc);
		UpperSurfacePlane = FPlane(TopLeftHandleLoc, PlaneNormal);

		if (AProcMeshCube* CastedActor = Cast<AProcMeshCube>(TouchedActor))
		{
			if (CastedActor == TopLeftGrabHandle)
			{
				GrabbedHandle = EGrabbedHandle::TopLeft;
			}
			else if (CastedActor == BottomLeftGrabHandle)
			{
				GrabbedHandle = EGrabbedHandle::BottomLeft;
			}
			else if (CastedActor == TopRightGrabHandle)
			{
				GrabbedHandle = EGrabbedHandle::TopRight;
			}
			else if (CastedActor == BottomRightGrabHandle)
			{
				GrabbedHandle = EGrabbedHandle::BottomRight;
			}
			else
			{
				GrabbedHandle = EGrabbedHandle::NONE;
			}
		}

		PlayerController->CurrentMouseCursor = EMouseCursor::GrabHandClosed;

		PrimaryComponentTick.SetTickFunctionEnable(true);
	}
}

void UProcMeshScalingComponent::HandleReleased(AActor* TouchedActor, FKey ButtonPressed)
{
	//PlaneNormal = FPlane::ZeroVector;
	//UpperSurfacePlane = FPlane();

	//GrabbedHandle = EGrabbedHandle::NONE;

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PlayerController->CurrentMouseCursor = EMouseCursor::GrabHand;

		GrabbedHandle = EGrabbedHandle::NONE;
	}

	PrimaryComponentTick.SetTickFunctionEnable(false);
}
