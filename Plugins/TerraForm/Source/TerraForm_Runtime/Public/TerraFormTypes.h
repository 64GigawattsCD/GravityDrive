// Copyright Craig Delancy 2019

#pragma once

#include "TerraForm_Runtime/Public/TerraForm_Runtime.h"
#include "TerraFormRuntimeDefinitions.h"
#include "CoreMinimal.h"

THIRD_PARTY_INCLUDES_START
#include "GDAL/Includes/gdal_priv.h"
#include "GDAL/Includes/ogr_feature.h"
#include "GDAL/Includes/ogrsf_frmts.h"
#include "GDAL/Includes/gdal_utils.h"
#include "GDAL/Includes/ogr_Geometry.h"
THIRD_PARTY_INCLUDES_END

#include "TerraFormTypes.generated.h"

UENUM(BlueprintType)
enum class ETerraFormLicense : uint8
{
	TL_Free 	UMETA(DisplayName = "Terra Form Free"),
	TL_Lite 	UMETA(DisplayName = "Terra Form Lite"),
	TL_Pro	 	UMETA(DisplayName = "Terra Form Pro"),
};

UENUM(BlueprintType)
enum class ETerraFormLinearUnit : uint8
{
	LU_MM 	UMETA(DisplayName = "Millimeters"),
	LU_CM 	UMETA(DisplayName = "Centimeters"),
	LU_Me 	UMETA(DisplayName = "Meters"),
	LU_KM 	UMETA(DisplayName = "Kilometers"),
	LU_In 	UMETA(DisplayName = "Inches"),
	LU_Fe 	UMETA(DisplayName = "Feet"),
	LU_Ya 	UMETA(DisplayName = "Yards"),
	LU_Mi 	UMETA(DisplayName = "Miles"),
	LU_None UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class ETerraFormAngularUnit : uint8
{
	AU_Degrees		UMETA(DisplayName = "Degrees"),
	AU_Radians		UMETA(DisplayName = "Radians"),
	AU_Rotations	UMETA(DisplayName = "Rotations"),
	AU_None			UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class ETerraFormTimeUnit : uint8
{
	TU_Sec 		UMETA(DisplayName = "Seconds"),
	TU_Minutes 	UMETA(DisplayName = "Minutes"),
	TU_Hours 	UMETA(DisplayName = "Hours"),
	TU_Days 	UMETA(DisplayName = "Days"),
	TU_None		UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class ETerraFormNorthOption : uint8
{
	NO_XP 	UMETA(DisplayName = "North is X+"),
	NO_XN 	UMETA(DisplayName = "North is X-"),
	NO_YP 	UMETA(DisplayName = "North is Y+"),
	NO_YN 	UMETA(DisplayName = "North is Y-"),
};

UENUM(BlueprintType)
enum class ESplineDirection : uint8
{
	SD_None 		UMETA(DisplayName = "None"),
	SD_Forward 		UMETA(DisplayName = "Forward"),
	SD_Backward 	UMETA(DisplayName = "Backward"),
	SD_Both 		UMETA(DisplayName = "Both"),
};

//More blueprint friendly than fvector2d
USTRUCT(BlueprintType)
struct TERRAFORM_RUNTIME_API FLatLongCoordinate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coordinate")
	int32 LaD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coordinate")
	int32 LaM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coordinate")
	float LaS; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coordinate")
	int32 LoD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coordinate")
	int32 LoM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coordinate")
	float LoS;
	
	FVector2D ToVector2D();

	FString ToString();
	
	FLatLongCoordinate(int32 LaD, int32 LaM, float LaS, int32 LoD, int32 LoM, float LoS)
		: LaD(LaD)
		, LaM(LaM)
		, LaS(LaS)
		, LoD(LoD)
		, LoM(LoM)
		, LoS(LoS)
	{}

	FLatLongCoordinate(FVector2D DecimalLatLong)
	{
		LaD = FMath::FloorToInt(DecimalLatLong.X);
		float rem = DecimalLatLong.X - LaD;
		rem *= 60;
		LaM = FMath::FloorToInt(rem);
		rem = rem - LaM;
		rem *= 60;
		LaS = rem;

		LoD = FMath::FloorToInt(DecimalLatLong.Y);
		rem = DecimalLatLong.Y - LoD;
		rem *= 60;
		LoM = FMath::FloorToInt(rem);
		rem = rem - LoM;
		rem *= 60;
		LoS = rem;
	}

	FLatLongCoordinate()
	{
		LaD = 0;
		LaM = 0;
		LaS = 0;
		LoD = 0;
		LoM = 0;
		LoS = 0;
	}
};

USTRUCT(BlueprintType)
struct TERRAFORM_RUNTIME_API FWorldFileData
{
	GENERATED_BODY()

	//A: x-component of the pixel width (x-scale)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WorlData")
	float A;

	//D: y-component of the pixel width (y-skew)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WorlData")
	float D;

	//B; x-component of the pixel height (x-skew)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WorlData")
	float B;

	//E: y-component of the pixel height (y-scale), typically negative
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WorlData")
	float E;

	//C: x-coordinate of the center of the original image's upper left pixel transformed to the map
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WorlData")
	float C;

	//F: y-coordinate of the center of the original image's upper left pixel transformed to the map
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WorlData")
	float F;

	//File contents as a single string
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WorlData")
	FString FileContents;

	FWorldFileData()
	{
		A = 0.f;
		B = 0.f;
		C = 0.f;
		D = 0.f;
		E = 0.f;
		F = 0.f;
	}

	FWorldFileData(float A, float D, float B, float E, float C, float F)
		: A(A)
		, D(B)
		, B(B)
		, E(E)
		, C(C)
		, F(F)
	{}
};

USTRUCT(BlueprintType)
struct TERRAFORM_RUNTIME_API FTFAttributeSet
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	TMap<FName, FString> StringAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	TMap<FName, float> FloatAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	TMap<FName, int32> IntegerAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	TMap<FName, bool> BoolAttributes;

	//Constructor
	FTFAttributeSet()
	{

	}

	void Append(FTFAttributeSet& AppendSet)
	{
		StringAttributes.Append(AppendSet.StringAttributes);
		FloatAttributes.Append(AppendSet.FloatAttributes);
		IntegerAttributes.Append(AppendSet.IntegerAttributes);
		BoolAttributes.Append(AppendSet.BoolAttributes);
	}

	
	FString GetString(FName Attribute) const;
	float GetFloat(FName Attribute) const;
	int32 GetInteger(FName Attribute) const;
	bool GetBool(FName Attribute) const;
};

struct FTFStringMappingEntry
{
	FName AtrtributeName;
	FName SelecctedOption;
	FString Constant;
	//If true, locks to option to constant value mode
	bool Lock;
	bool Disable;

	FTFStringMappingEntry() { SelecctedOption = "None"; }

	FTFStringMappingEntry(FName AtrtributeName, FName SelecctedOption)
		:AtrtributeName(AtrtributeName)
		, SelecctedOption(SelecctedOption)
	{
		Constant = "";
		Lock = false;
		Disable = false;
	}

	FTFStringMappingEntry(FName AtrtributeName, FString Constant, bool Lock = false)
		:AtrtributeName(AtrtributeName)
		, SelecctedOption(TFATTCONST)
		, Constant(Constant)
		, Lock(Lock)
	{
		Disable = false;
	}
};

struct FTFFloatMappingEntry
{
	FName AtrtributeName;
	FName SelecctedOption;
	float Constant;
	//If true, locks to option to constant value mode
	bool Lock;
	bool Disable;

	FTFFloatMappingEntry() { SelecctedOption = "None"; }

	FTFFloatMappingEntry(FName AtrtributeName, FName SelecctedOption)
		:AtrtributeName(AtrtributeName)
		, SelecctedOption(SelecctedOption)
	{
		Constant = 0.f;
		Lock = false;
		Disable = false;
	}

	FTFFloatMappingEntry(FName AtrtributeName, float Constant, bool Lock = false)
		:AtrtributeName(AtrtributeName)
		, SelecctedOption(TFATTCONST)
		, Constant(Constant)
		, Lock(Lock)
	{
		Disable = false;
	}
};

struct FTFIntegerMappingEntry
{
	FName AtrtributeName;
	FName SelecctedOption;
	int32 Constant;
	//If true, locks to option to constant value mode
	bool Lock;
	bool Disable;

	FTFIntegerMappingEntry() { SelecctedOption = "None"; }

	FTFIntegerMappingEntry(FName AtrtributeName, FName SelecctedOption)
		:AtrtributeName(AtrtributeName)
		, SelecctedOption(SelecctedOption)
	{
		Constant = 0;
		Lock = false;
		Disable = false;
	}

	FTFIntegerMappingEntry(FName AtrtributeName, int32 Constant, bool Lock = false)
		:AtrtributeName(AtrtributeName)
		, SelecctedOption(TFATTCONST)
		, Constant(Constant)
		, Lock(Lock)
	{
		Disable = false;
	}
};

struct FTFBoolMappingEntry
{
	FName AtrtributeName;
	FName SelecctedOption;
	bool Constant;
	//If true, locks to option to constant value mode
	bool Lock;
	bool Disable;

	FTFBoolMappingEntry() { SelecctedOption = "None"; }

	FTFBoolMappingEntry(FName AtrtributeName, FName SelecctedOption)
		:AtrtributeName(AtrtributeName)
		,SelecctedOption(SelecctedOption)
	{
		Constant = false;
		Lock = false;
		Disable = false;
	}

	FTFBoolMappingEntry(FName AtrtributeName, bool Constant, bool Lock = false)
		:AtrtributeName(AtrtributeName)
		, SelecctedOption(TFATTCONST)
		, Constant(Constant)
		,Lock(Lock)
	{
		Disable = false;
	}
};


//USTRUCT(BlueprintType)
struct FTFAttributeMapping
{
	//GENERATED_BODY()

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	TArray<TSharedPtr<FTFStringMappingEntry>> StringMapping;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	TArray<TSharedPtr<FTFFloatMappingEntry>> FloatMapping;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	TArray<TSharedPtr<FTFIntegerMappingEntry>> IntegerMapping;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	TArray<TSharedPtr<FTFBoolMappingEntry>> BoolMapping;

	TArray<FName> AttributeOptions;

	FTFAttributeMapping()
	{

	}

};

USTRUCT(BlueprintType)
struct TERRAFORM_RUNTIME_API FOGRShape
{
	GENERATED_BODY()

	//Name of data layer this shape came from
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShapeData")
	FString LayerName;

	//GIS name of the shape
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShapeData")
	FString GISName;

	//True if the shape data provided elevation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShapeData")
	bool HasElevation;

	//List of points that make up the shape
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShapeData")
	TArray<FVector> Points;

	//OGRwkbGeometryType as int, determines spline, point, area, etc.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShapeData")
	int32 GeomType;
	//OGRwkbGeometryType GeomType;

	//Per point attribute sets of this shape
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShapeData")
	TArray<FTFAttributeSet> AttributeSet;

	FOGRShape()
	{
		LayerName = "";
		GISName = "";
		HasElevation = false;
		GeomType = -1;
	}

	FOGRShape(OGRGeometry* InGeo, FString Name, FTFAttributeSet Attributes = FTFAttributeSet());

	//Used to append multiple shapes into the vert list, such as when linking multiple line segments into a spline
	void AppendGIS(OGRGeometry * InGeo, FTFAttributeSet Attributes = FTFAttributeSet());

	//Used to append multiple shapes into the vert list, such as when linking multiple line segments into a spline
	void AppendShape(FOGRShape InShape);

	bool IsSpline() const;
};


UENUM(BlueprintType)
enum class ELandscapeReprojectOption : uint8
{
	LR_None 	UMETA(DisplayName = "No Reprojection"),
	LR_Asset 	UMETA(DisplayName = "Projection Asset"),
	LR_ESPG 	UMETA(DisplayName = "EPSG Code"),
};

UENUM(BlueprintType)
enum class ETerraformCropMethod : uint8
{
	CM_Include 	UMETA(DisplayName = "Include all data"),
	CM_Exclude 	UMETA(DisplayName = "Exclude all no-data regions"),
	CM_Manual 	UMETA(DisplayName = "Manually specify corners"),
};

UENUM(BlueprintType)
enum class EGDALResampleAlgorithm : uint8
{
	GRA_NearestNeighbour = 0 UMETA(DisplayName = "Nearest Neighbor"),
	GRA_Bilinear = 1 UMETA(DisplayName = "Bilinear"),
	GRA_Cubic = 2 UMETA(DisplayName = "Cubic Convolution"),
	GRA_CubicSpline = 3 UMETA(DisplayName = "Cubic B-Spline"),
	GRA_Lanczos = 4 UMETA(DisplayName = "Lanczos"),
	GRA_Average = 5 UMETA(DisplayName = "Weighted Average"),
	GRA_Mode = 6 UMETA(DisplayName = "Mode"),
	GRA_Gauss = 7 UMETA(DisplayName = "Gauss"),
	GRA_Max = 8 UMETA(DisplayName = "Maximum"),
	GRA_Min = 9 UMETA(DisplayName = "Minimum"),
	GRA_Med = 10 UMETA(DisplayName = "Median"),
	GRA_Q1 = 11 UMETA(DisplayName = "First Quartile"),
	GRA_Q3 = 12 UMETA(DisplayName = "Third Quartile"),
	GRA_Sum = 13 UMETA(DisplayName = "Weighted Sum"),
};

USTRUCT(BlueprintType)
struct TERRAFORM_RUNTIME_API FMeshPoolEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshPool")
	class UStaticMesh* Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshPool")
	FVector ScaleMin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshPool")
	FVector ScaleMax;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshPool")
	bool UniformScale;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshPool")
	FRotator RotationMin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshPool")
	FRotator RotationMax;

	FMeshPoolEntry()
	{
		Mesh = nullptr;
		ScaleMin = FVector(1.f, 1.f, 1.f);
		ScaleMax = FVector(1.f, 1.f, 1.f);
		UniformScale = false;
		RotationMin = FRotator(0.f,0.f,0.f);
		RotationMax = FRotator(0.f,0.f,0.f);
	}

	FVector GetRandScale() const;
	FRotator GetRandRot() const;
};

USTRUCT(BlueprintType)
struct TERRAFORM_RUNTIME_API FMeshNamedPoolEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshPool")
	FString EntryName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshPool")
	FMeshPoolEntry MeshEntry;

	FMeshNamedPoolEntry()
	{
		EntryName = "None";
	}

	FMeshNamedPoolEntry(FString Name, FMeshPoolEntry Entry)
	{
		EntryName = Name;
		MeshEntry = Entry;
	}
};