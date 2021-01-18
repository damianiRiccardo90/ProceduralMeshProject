#pragma once

#include <Blueprint/UserWidget.h>
#include <Internationalization/Text.h>
#include "PrintToScreenWidget.generated.h"


UCLASS()
class UPrintToScreenWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void PrintToScreen(const TArray<FText>& InMessageLines);

};
