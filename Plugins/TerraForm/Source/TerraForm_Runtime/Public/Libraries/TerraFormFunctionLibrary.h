// Copyright Craig Delancy 2019

#pragma once

#include "CoreMinimal.h"
#include "TerraForm_Runtime/Public/TerraFormTypes.h"
#include "TerraForm_Runtime/Public/TerraFormRuntimeDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TerraFormFunctionLibrary.generated.h"


class OGRSpatialReference;

UCLASS()
class TERRAFORM_RUNTIME_API UTerraFormFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static FVector2D DatasetPixelToProjected(GDALDataset * InRasterSet, int32 CollumnX, int32 LineY);
	static FVector2D DatasetPixelToProjected(GDALDataset * InRasterSet, double CollumnX, double LineY);
	static FVector2D ConvertProjectedToGeographic(FVector2D InProjected, OGRSpatialReference* InSpatRef);
	static FVector2D ConvertGeoGraphicToProjected(FVector2D InGeographic, OGRSpatialReference* InSpatRef);
	static FVector ReprojectCoordinate(FVector InCoordinate, OGRSpatialReference* InSpatRef, OGRSpatialReference* OutSpatRef);
	static OGRSpatialReference* EPSGToSpatialRef(FString InEPSG);
	
	static void HandleCPLErr(CPLErr Error);
	static void HandleOGRErr(OGRErr Error);

	static bool ResizeDataSet(GDALDataset* srcDataset, GDALDataset* &Resized, int32 NewSizeX, int32 NewSizeY, EGDALResampleAlgorithm ResampleAlgorithm);
	static bool ReprojectDataSet(GDALDataset* srcDataset, OGRSpatialReference* NewProjection, GDALDataset* &Resized, int32 SizeX, int32 SizeY, float MinX, float MaxX, float MinY, float MaxY, EGDALResampleAlgorithm ResampleAlgorithm = EGDALResampleAlgorithm::GRA_Bilinear);
	
	UFUNCTION(BlueprintCallable, Category = TerraForm_Units)
	static FName GetLinearUnitName(ETerraFormLinearUnit InUnit);
	UFUNCTION(BlueprintCallable, Category = TerraForm_Units)
	static ETerraFormLinearUnit LinearUnitFromString(FString InString);
	UFUNCTION(BlueprintCallable, Category = TerraForm_Units)
	static FString GetLinearUnitSuffix(ETerraFormLinearUnit InUnit);

	//Converts a float representing a distance between various units
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = TerraForm_Units)
	static float ConvertLinearUnits(ETerraFormLinearUnit InUnits, float InValue, ETerraFormLinearUnit OutUnits);

	//Converts a vectors representing a distance between various units
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = TerraForm_Units)
	static FVector ConvertVectorLinearUnits(ETerraFormLinearUnit InUnits, FVector InValue, ETerraFormLinearUnit OutUnits);

	UFUNCTION(BlueprintCallable, Category = TerraForm_Units)
	static FName GetTimeUnitName(ETerraFormTimeUnit InUnit);
	UFUNCTION(BlueprintCallable, Category = TerraForm_Units)
	static ETerraFormTimeUnit TimeUnitFromString(FString InString);
	UFUNCTION(BlueprintCallable, Category = TerraForm_Units)
	static FString GetTimeUnitSuffix(ETerraFormTimeUnit InUnit);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = TerraForm_Units)
	static float ConvertTimeUnits(ETerraFormTimeUnit InUnits, float InValue, ETerraFormTimeUnit OutUnits);

	UFUNCTION(BlueprintCallable, Category = TerraForm_Units)
	static FName GetAngularUnitName(ETerraFormAngularUnit InUnit);
	UFUNCTION(BlueprintCallable, Category = TerraForm_Units)
	static ETerraFormAngularUnit AngularUnitFromString(FString InString);
	UFUNCTION(BlueprintCallable, Category = TerraForm_Units)
	static FString GetAngularUnitSuffix(ETerraFormAngularUnit InUnit);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = TerraForm_Units)
	static float ConvertAngularUnits(ETerraFormAngularUnit InUnits, float InValue, ETerraFormAngularUnit OutUnits);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = TerraForm_Info)
	static FString GetTerraFormLicenseName();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = TerraForm_Info)
	static FString GetTerraFormVersionNumber();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = TerraForm_Info)
	static FString GetTerraFormVersionName();

	//if MSFP is true and all shapes are points, the points will be combined into a single spline
	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = TerraForm_Shape)
	static TArray<FOGRShape> ReadGDSIntoShapes(GDALDataset* GDS, bool MakeSplineFromPoints = false);

	static class ATerraFormLandscapeAnchor* GetActorLandscapeAnchor(AActor* InActor);
	static void ConvertAttributeSetToAllStrings(FTFAttributeSet& InSet);
	//If AllStrings is set true, it will skip converting the other attribute types to strings. Incoming attributes from dataset are kept as strings until they are assigned and converted
	static void RemapAttributes(FTFAttributeSet& InSet, FTFAttributeMapping Mapping, bool AllStrings = false);
	static void ReadProjectionAxisInfo(FString InProjCRS, ETerraFormLinearUnit &EastingUnit, float &EastingScale, int &EastingOrder, ETerraFormLinearUnit &NorthingUnit, float &NorthingScale, int &NorthingOrder);
	static void ReadAxisInfo(FString AxisString, ETerraFormLinearUnit &Unit, float &Scale, int &Order);
	static FString ReadStringParam(FString InString, FString Prefix, TCHAR OpenChar, TCHAR CloseChar, bool FromStart = true);
	static FVector2D GetDataSetSizeMeters(GDALDataset*GDS, bool PointMode = true);

	UFUNCTION(BlueprintCallable, Category = TerraForm_Info)
	static FString TerraFormGetAuthor();

	static TArray<FName> GetAttributeNameList(GDALDataset* GDS);

	UFUNCTION(BlueprintCallable, Category = TerraForm)
	static bool RenameActor(AActor* InActor, FString NewName);
};