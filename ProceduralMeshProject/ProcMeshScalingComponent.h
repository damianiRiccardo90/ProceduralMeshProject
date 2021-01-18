#pragma once

#include <Components/ActorComponent.h>

#include "ProcMeshScalingComponent.generated.h"


UENUM(BlueprintType)
enum class EGrabbedHandle : uint8
{
	TopLeft,
	BottomLeft,
	TopRight,
	BottomRight,
	NONE
};

class AProcMeshCube;

/**
 * Component that can be attached to any ProcMeshBase that control the scaling of the mesh
 */
UCLASS(Blueprintable)
class PROCEDURALMESHPROJECT_API UProcMeshScalingComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual void GenerateGrabHandles();

protected:

	UFUNCTION(BlueprintCallable, Category = "Internals")
	virtual void ClearHandles();

	UFUNCTION(BlueprintCallable, Category = "Internals")
	virtual void ResetHandlesLocation();

	void BindEvents();
	void UnbindEvents();

	UFUNCTION()
	void HandleBeginCursorOver(AActor* TouchedActor);
	UFUNCTION()
	void HandleEndCursorOver(AActor* TouchedActor);
	UFUNCTION()
	void HandleClicked(AActor* TouchedActor , FKey ButtonPressed);
	UFUNCTION()
	void HandleReleased(AActor* TouchedActor , FKey ButtonPressed);

	UPROPERTY(EditDefaultsOnly, Category = "Config|Grab Handles")
	FVector GrabHandleRadius;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Grab Handles")
	TSubclassOf<AProcMeshCube> GrabHandleClass;

private:

	UPROPERTY()
	AProcMeshCube* TopLeftGrabHandle = nullptr;
	UPROPERTY()
	AProcMeshCube* BottomLeftGrabHandle = nullptr;
	UPROPERTY()
	AProcMeshCube* TopRightGrabHandle = nullptr;
	UPROPERTY()
	AProcMeshCube* BottomRightGrabHandle = nullptr;

	FVector PlaneNormal;
	FPlane UpperSurfacePlane;
	EGrabbedHandle GrabbedHandle;

};
