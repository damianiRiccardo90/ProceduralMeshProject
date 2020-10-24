#pragma once

#include <Camera/CameraActor.h>

#include "FixedEditingCamera.generated.h"


UCLASS()
class ZURUPROJECT_API AFixedEditingCamera : public ACameraActor
{
	GENERATED_BODY()

public:

	AFixedEditingCamera(const class FObjectInitializer& ObjectInitializer);

};
