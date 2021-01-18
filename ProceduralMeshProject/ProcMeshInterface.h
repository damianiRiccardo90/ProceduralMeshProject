#pragma once

#include <UObject/Interface.h>

#include "ProcMeshInterface.generated.h"


class UMaterialInterface;

UINTERFACE(MinimalAPI)
class UProcMeshInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IProcMeshInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	virtual void SetRadius(const FVector& InRadius) =0;
	virtual FVector GetRadius() const =0;
	virtual FVector GetMinRadius() const =0;
	virtual void Extrude(const FVector& InDelta) =0;
	virtual void Intrude(const FVector& InDelta) =0;
	virtual void SetCollisionProfileName(const FName& InProfileName) =0;
	virtual FName GetCollisionProfileName() const =0;
	virtual void SetMaterial(UMaterialInterface* InMaterial) =0;
	virtual void ClearMesh() =0;

protected:

	virtual void GenerateMesh() =0;
	virtual void ValidateDimensions() =0;

};
