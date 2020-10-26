#pragma once

#include "ProcMeshBase.h"

#include "ProcMeshChair.generated.h"


class AProcMeshCube;
class UMaterialInterface;

/**
 * A procedural chair mesh
 */
UCLASS()
class ZURUPROJECT_API AProcMeshChair : public AProcMeshBase
{
	GENERATED_UCLASS_BODY()

public:

	virtual void SetRadius(const FVector& InRadius) override;
	virtual void SetCollisionProfileName(const FName& InProfileName) override;
	virtual void SetMaterial(UMaterialInterface* InMaterial) override;
	virtual void ClearMesh() override;

protected:

	virtual void GenerateMesh() override;
	virtual void ValidateDimensions() override;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Classes")
	TSubclassOf<AProcMeshCube> CubeMeshClass;

private:

	UPROPERTY()
	AProcMeshCube* TopLeftLegMesh = nullptr;
	UPROPERTY()
	AProcMeshCube* TopRightLegMesh = nullptr;
	UPROPERTY()
	AProcMeshCube* BottomRightLegMesh = nullptr;
	UPROPERTY()
	AProcMeshCube* BottomLeftLegMesh = nullptr;
	UPROPERTY()
	AProcMeshCube* SeatMesh = nullptr;
	UPROPERTY()
	AProcMeshCube* BackrestMesh = nullptr;

	FVector LegRadius;
	FVector SeatRadius;
	FVector BackrestRadius;

};
