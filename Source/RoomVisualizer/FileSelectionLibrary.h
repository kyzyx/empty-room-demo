// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "FileSelectionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ROOMVISUALIZER_API UFileSelectionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "FileSelectionLibrary")
	static FString GetSaveFilename();

	UFUNCTION(BlueprintCallable, Category = "FileSelectionLibrary")
	static FString GetOpenFilename();
	
	
};
