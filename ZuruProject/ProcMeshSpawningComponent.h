#pragma once

#include <Components/ActorComponent.h>

#include "ProcMeshSpawningComponent.generated.h"


class AProcMeshBase;

UCLASS(Blueprintable)
class ZURUPROJECT_API UProcMeshSpawningComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual void ActivateComponent();
	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual void DeactivateComponent();
	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual void ActivateSpawning();
	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual void DeactivateSpawning();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Classes")
	TSubclassOf<AProcMeshBase> MeshClass;

private:

	UPROPERTY()
	AProcMeshBase* SpawnableMesh = nullptr;

};
