// Copyright Craig Delancy 2019

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "GISProjectionAssetFactory.generated.h"

/**
 * 
 */
UCLASS(hidecategories=Object)
class TERRAFORM_EDITORASSETS_API UGISProjectionAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
	
public:

	//UGISProjectionAssetFactory(const FObjectInitializer& ObjectInitializaer);
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	
};
