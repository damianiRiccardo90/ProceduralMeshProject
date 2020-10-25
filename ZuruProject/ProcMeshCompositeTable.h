#pragma once

#include "ProcMeshBase.h"

#include "ProcMeshCompositeTable.generated.h"


class AProcMeshTable;
class AProcMeshChair;
class UMaterialInterface;

/**
 * A procedural table mesh, with chairs!!!
 */
UCLASS()
class ZURUPROJECT_API AProcMeshCompositeTable : public AProcMeshBase
{
	GENERATED_UCLASS_BODY()

public:

	virtual void SetRadius(const FVector& InRadius) override;
	virtual void SetCollisionProfileName(const FName& InProfileName) override;
	virtual void SetMaterial(UMaterialInterface* InMaterial) override;
	virtual void ClearMesh() override;

protected:

	virtual void GenerateMesh() override;
	virtual void GenerateInitialMeshes();
	virtual void SetupChairs();
	virtual void ComputeDistances();
	virtual void ValidateDimensions() override;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Table")
	TSubclassOf<AProcMeshTable> TableClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Chairs")
	TSubclassOf<AProcMeshChair> ChairClass;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Chairs")
	float MinChairDistance;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Chairs")
	int32 MinChairNumber;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Materials")
	UMaterialInterface* TableMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Materials")
	UMaterialInterface* ChairsMaterial = nullptr;

private:

	UPROPERTY()
	AProcMeshTable* TableMesh = nullptr;
	UPROPERTY()
	TArray<AProcMeshChair*> TopSideChairs;
	UPROPERTY()
	TArray<AProcMeshChair*> RightSideChairs;
	UPROPERTY()
	TArray<AProcMeshChair*> BottomSideChairs;
	UPROPERTY()
	TArray<AProcMeshChair*> LeftSideChairs;

	float TableDiameterX;
	float TableDiameterY;
	float TableLegDiameterX;
	float TableLegDiameterY;
	float ChairDiameterX;
	float Tran_ChairSpace;
	int32 Tran_MaxChairNum;
	float Tran_MaxChairDistance;
	float Long_ChairSpace;
	int32 Long_MaxChairNum;
	float Long_MaxChairDistance;

};
