// Copyright Craig Delancy 2019

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "GISProjectionAssetFactoryNew.generated.h"

/**
 * 
 */
UCLASS(hidecategories=Object)
class TERRAFORM_EDITORASSETS_API UGISProjectionAssetFactoryNew : public UFactory
{
	GENERATED_BODY()
	
public:

	UGISProjectionAssetFactoryNew(const FObjectInitializer& ObjectInitializaer);
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual bool ShouldShowInNewMenu() const override;
	
	
};
