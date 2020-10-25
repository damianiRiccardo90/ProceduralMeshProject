#pragma once

#include "ProcMeshBase.h"

#include "ProcMeshTable.generated.h"


class AProcMeshCube;
class UMaterialInterface;

/**
 * A procedural table mesh
 */
UCLASS()
class ZURUPROJECT_API AProcMeshTable : public AProcMeshBase
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Mesh Utility")
	virtual FVector GetLegRadius() const;

	virtual void SetRadius(const FVector& InRadius) override;
	virtual void SetCollisionProfileName(const FName& InProfileName) override;
	virtual void SetMaterial(UMaterialInterface* InMaterial) override;
	virtual void ClearMesh() override;

protected:

	virtual void GenerateMesh() override;
	virtual void ValidateDimensions() override;

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
	AProcMeshCube* PlankMesh = nullptr;

	FVector LegRadius;
	FVector PlankRadius;

};
