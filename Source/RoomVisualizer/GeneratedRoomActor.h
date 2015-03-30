// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RoomModel.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include "GeneratedRoomActor.generated.h"

UCLASS(Blueprintable)
class ROOMVISUALIZER_API AGeneratedRoomActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGeneratedRoomActor(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = "RoomGeometry")
	void Update(bool updatelighting=false);

	//-----------------------------
	// Room model editing functions
	//-----------------------------
	UFUNCTION(BlueprintCallable, Category = "RoomGeometry")
	void SetBaseboardHeight(float newBBHeight);
	UFUNCTION(BlueprintCallable, Category = "RoomGeometry")
	void SetBaseboardDepth(float newBBDepth);
	UFUNCTION(BlueprintCallable, Category = "RoomGeometry")
	void SetRoomHeight(float newHeight);
	UFUNCTION(BlueprintCallable, Category = "RoomGeometry")
		void SetWindowDepth(float newDepth);
	UFUNCTION(BlueprintCallable, Category = "RoomGeometry")
		void SetWindowBrightness(float newBrightness);

	UFUNCTION(BlueprintCallable, Category = "RoomGeometry")
	void SaveRoomToFile(FString filename);

	UFUNCTION(BlueprintCallable, Category = "RoomGeometry")
	static void setLoadFile(FString filename);

	static FString defaultloadfile;
	static const double unitscale;
	static const double cubescale;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RoomGeometry")
private:
	int numlightcomponents;
	RoomModel* roommodel;

	UStaticMesh* cubemesh;
	UStaticMesh* cylindermesh;
	UMaterial* basediffusematerial;
	UMaterial* basetexturedmaterial;
	UMaterial* basewindowmaterial;
	UMaterial* baselinelightmaterial;

};
