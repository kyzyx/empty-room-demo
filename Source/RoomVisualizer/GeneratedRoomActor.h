// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RoomModel.h"
#include "GameFramework/Actor.h"
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
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = "RoomGeometry")
	void Update();

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
	void SaveRoomToFile(FString filename);

	static const double unitscale;
	static const double cubescale;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RoomGeometry")
private:
	RoomModel* roommodel;

};
