#pragma once

#include <Components/SceneComponent.h>

#include "ProcMeshSpawningComponent.generated.h"


class AProcMeshBase;
class UMaterialInterface;

/**
 * Component that controls the spawning process of any ProcMeshBase into the scene
 */
UCLASS(Blueprintable)
class ZURUPROJECT_API UProcMeshSpawningComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()

public:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual void GenerateIndicatorMesh();

	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual void ActivateComponent(const bool InbActivate);
	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual bool IsComponentActive() const;

	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual void ActivateSpawning(const bool InbActivate);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual AProcMeshBase* SpawnMesh();

protected:

	UFUNCTION(BlueprintCallable, Category = "Internals")
	virtual void ClearIndicatorMesh();

	void BindEvents();
	void UnbindEvents();

	UFUNCTION()
	void HandleIndicatorMeshBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void HandleIndicatorMeshEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Classes")
	TSubclassOf<AProcMeshBase> MeshClass;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Classes")
	TSubclassOf<AProcMeshBase> MeshIndicatorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Materials")
	UMaterialInterface* ActivatedStateMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Materials")
	UMaterialInterface* DeactivatedStateMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Collisions")
	FName SpawnMeshCollisionProfileName;

private:

	UPROPERTY()
	AProcMeshBase* IndicatorMesh = nullptr;

	bool bIsActive;
	bool bCanSpawn;

};
