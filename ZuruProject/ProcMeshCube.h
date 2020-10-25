#pragma once

#include "ProcMeshBase.h"
#include <ProceduralMeshComponent.h>

#include "ProcMeshCube.generated.h"


class UProceduralMeshComponent;
class UMaterialInterface;

/**
 * A procedural mesh cube
 */
UCLASS()
class ZURUPROJECT_API AProcMeshCube : public AProcMeshBase
{
	GENERATED_UCLASS_BODY()

public:

	virtual void SetRadius(const FVector& InRadius) override;
	virtual void SetCollisionProfileName(const FName& InProfileName) override;
	virtual void SetMaterial(UMaterialInterface* InMaterial) override;
	virtual void ClearMesh() override;

protected:

	virtual void GenerateMesh() override;
	virtual void ExtrudeMesh(const FVector& InDelta);
	virtual void IntrudeMesh(const FVector& InDelta);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Config|Vertex Color")
	FLinearColor FrontTopLeftColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Vertex Color")
	FLinearColor FrontBottomLeftColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Vertex Color")
	FLinearColor FrontTopRightColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Vertex Color")
	FLinearColor FrontBottomRightColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Vertex Color")
	FLinearColor BackTopRightColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Vertex Color")
	FLinearColor BackBottomRightColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Vertex Color")
	FLinearColor BackTopLeftColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Vertex Color")
	FLinearColor BackBottomLeftColor = FLinearColor::Red;

private:

	UPROPERTY()
	UProceduralMeshComponent* CubeMesh = nullptr;

	FProcMeshSectionData MeshData;

};
