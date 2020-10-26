#pragma once

#include <ProceduralMeshComponent.h>
#include "ProcMeshInterface.h"
#include <GameFramework/Actor.h>

#include "ProcMeshBase.generated.h"


USTRUCT(BlueprintType)
struct FProcMeshSectionData
{
	GENERATED_USTRUCT_BODY()

public:

	void ClearData();

	UPROPERTY()
	TArray<FVector> Vertices;
	UPROPERTY()
	TArray<int32> Triangles;
	UPROPERTY()
	TArray<FVector> Normals;
	UPROPERTY()
	TArray<FProcMeshTangent> Tangents;
	UPROPERTY()
	TArray<FVector2D> UVs;
	UPROPERTY()
	TArray<FLinearColor> Colors;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProcMeshSetRadiusSignature, const FVector&, NewRadius);

class USceneComponent;
class UMaterialInterface;

UCLASS(Abstract)
class AProcMeshBase: public AActor, public IProcMeshInterface
{
	GENERATED_UCLASS_BODY()

public:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Mesh Utility")
	virtual void SetRadius(const FVector& InRadius) override;
	UFUNCTION(BlueprintCallable, Category = "Mesh Utility")
	virtual FVector GetRadius() const override;
	UFUNCTION(BlueprintCallable, Category = "Mesh Utility")
	virtual FVector GetMinRadius() const override;

	UFUNCTION(BlueprintCallable, Category = "Mesh Utility")
	virtual void Extrude(const FVector& InDelta);
	UFUNCTION(BlueprintCallable, Category = "Mesh Utility")
	virtual void Intrude(const FVector& InDelta);

	UFUNCTION(BlueprintCallable, Category = "Mesh Utility")
	virtual void SetCollisionProfileName(const FName& InProfileName) override;
	UFUNCTION(BlueprintCallable, Category = "Mesh Utility")
	virtual FName GetCollisionProfileName() const override;

	UFUNCTION(BlueprintCallable, Category = "Mesh Utility")
	virtual void SetMaterial(UMaterialInterface* InMaterial) override;

	UFUNCTION(BlueprintCallable, Category = "Mesh Utility")
	virtual void ClearMesh() override;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FProcMeshSetRadiusSignature OnSetRadius;

protected:

	UFUNCTION(BlueprintCallable, Category = "Mesh Internals")
	virtual void GenerateMesh() override;

	UFUNCTION(BlueprintCallable, Category = "Mesh Internals")
	virtual void ValidateDimensions() override;

	UFUNCTION(BlueprintCallable, Category = "Mesh Internals")
	void AddQuadMesh(const FVector& InTopLeft, const FVector& InBottomLeft, const FVector& InTopRight, const FVector& InBottomRight, FProcMeshSectionData& OutSectionData);

	template<class T>
	void SpawnMeshActor(T*& OutMeshActor, TSubclassOf<AProcMeshBase> InMeshClass);

	UPROPERTY(EditDefaultsOnly, Category = "Config|Radius")
	FVector Radius;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Radius")
	FVector MinRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Collisions")
	FName CollisionProfileName;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Materials")
	UMaterialInterface* MainMaterial = nullptr;

	UPROPERTY()
	USceneComponent* SceneComponent = nullptr;

};

template<class T>
void AProcMeshBase::SpawnMeshActor(T*& OutMeshActor, TSubclassOf<AProcMeshBase> InMeshClass)
{
	if (OutMeshActor)
	{
		OutMeshActor->ClearMesh();
		OutMeshActor->Destroy();
	}

	if (UWorld* World = GetWorld())
	{
		OutMeshActor = World->SpawnActor<T>(InMeshClass, GetActorLocation(), GetActorRotation(), FActorSpawnParameters());
	}

	ensureMsgf(OutMeshActor, TEXT("Object of type %s has not been able to initialize its mesh actor."), *StaticClass()->GetFName().ToString());

	if (OutMeshActor)
	{
		OutMeshActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}
}
