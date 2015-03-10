// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomVisualizer.h"
#include "GeneratedRoomActor.h"

#include "rectanglerenderer.h"
#include "geometrygenerator.h"
#include "Engine.h"

#include <map>

const double AGeneratedRoomActor::unitscale = 100;
const double AGeneratedRoomActor::cubescale = 10;

// From https://wiki.unrealengine.com/Procedural_Materials
void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
	if (Texture && Texture->Resource)
	{
		struct FUpdateTextureRegionsData
		{
			FTexture2DResource* Texture2DResource;
			int32 MipIndex;
			uint32 NumRegions;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint32 SrcBpp;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;
		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
		RegionData->MipIndex = MipIndex;
		RegionData->NumRegions = NumRegions;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcBpp = SrcBpp;
		RegionData->SrcData = SrcData;

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			UpdateTextureRegionsData,
			FUpdateTextureRegionsData*, RegionData, RegionData,
			bool, bFreeData, bFreeData,
			{
			for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
			{
				int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
				if (RegionData->MipIndex >= CurrentFirstMip)
				{
					RHIUpdateTexture2D(
						RegionData->Texture2DResource->GetTexture2DRHI(),
						RegionData->MipIndex - CurrentFirstMip,
						RegionData->Regions[RegionIndex],
						RegionData->SrcPitch,
						RegionData->SrcData
						+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
						+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
						);
				}
			}
			if (bFreeData)
			{
				FMemory::Free(RegionData->Regions);
				FMemory::Free(RegionData->SrcData);
			}
			delete RegionData;
		});
	}
}

// Sets default values
AGeneratedRoomActor::AGeneratedRoomActor(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubemesh(TEXT("/Game/FirstPerson/Meshes/UnitCube"));
	static ConstructorHelpers::FObjectFinder<UMaterial> basediffusematerial(TEXT("/Game/FirstPerson/Meshes/DiffuseMaterialBase"));
	//static ConstructorHelpers::FObjectFinder<UMaterial> basetexturedmaterial(TEXT("/Game/FirstPerson/Meshes/TexturedMaterialMirrorBase"));
	static ConstructorHelpers::FObjectFinder<UMaterial> basetexturedmaterial(TEXT("/Game/FirstPerson/Meshes/TexturedMaterialWrapBase"));
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, "rootroomcomponent");
	if (cubemesh.Succeeded()) {
		roommodel = new RoomModel();
		roommodel->baseboardDepth = 0.05;
		roommodel->baseboardHeight = 0.2;
		RectangleWallObject rwo;
		rwo.height = 0.8;
		rwo.width = 0.8;
		rwo.horizontalposition = 1;
		rwo.verticalposition = 1;
		rwo.trimDepth = 0.05;
		rwo.trimWidth = 0.1;

		Wall wall;
		int dim = 500;
		float* texture = new float[4 * dim * dim];
		for (int i = 0; i < dim*dim; ++i) {
			bool black = false;
			if (i / dim < dim/2) black = !black;
			if (i % dim < dim/2) black = !black;
			texture[i * 4 + 0] = black ? 0 : 256;
			texture[i * 4 + 1] = black ? 0 : 256;
			texture[i * 4 + 2] = black ? 0 : 256;
			texture[i * 4 + 3] = 1;

		}
		Texture* tmp = new Texture();
		tmp->height = dim;
		tmp->width = dim;
		tmp->scale = 0.001;
		tmp->texture = texture;
		/*
		wall.length = 6;
		wall.normal = -1;
		roommodel->walls.push_back(wall);
		wall.length = 6;
		wall.normal = -1;
		roommodel->walls.push_back(wall);
		wall.length = 3;
		wall.normal = 1;
		roommodel->walls.push_back(wall);
		wall.length = 2;
		wall.normal = 1;
		roommodel->walls.push_back(wall);
		wall.length = 3;
		wall.normal = 1;
		roommodel->walls.push_back(wall);
		wall.length = 4;
		wall.normal = 1;
		roommodel->walls.push_back(wall);
		*/
		wall.length = 6;
		wall.normal = -1;
		roommodel->walls.push_back(wall);
		wall.length = 6;
		wall.normal = -1;
		roommodel->walls.push_back(wall);
		wall.length = 6;
		wall.normal = 1;
		roommodel->walls.push_back(wall);
		wall.length = 2;
		wall.normal = 1;
		roommodel->walls.push_back(wall);
		wall.length = 2;
		wall.normal = -1;
		roommodel->walls.push_back(wall);
		wall.length = 2;
		wall.normal = 1;
		roommodel->walls.push_back(wall);
		wall.length = 2;
		wall.normal = 1;
		roommodel->walls.push_back(wall);
		wall.length = 2;
		wall.normal = 1;
		roommodel->walls.push_back(wall);
		roommodel->height = 2.5;

		roommodel->wallMaterial = Material(0.7, 0.7, 0.5);
		roommodel->ceilingMaterial = roommodel->wallMaterial;
		roommodel->floorMaterial = Material(0.2, 0.2, 0.5);
		roommodel->floorMaterial.texture = tmp;
		roommodel->baseboardMaterial = Material(0.5, 0.25, 0.1);

		//rwo.recessed = 0;
		rwo.recessed = 0.2;
		rwo.frameMaterial = roommodel->baseboardMaterial;
		rwo.material = roommodel->wallMaterial;
		roommodel->walls[3].windows.push_back(rwo);
		roommodel->walls[2].windows.push_back(rwo);
		roommodel->walls[1].windows.push_back(rwo);
		roommodel->walls[0].windows.push_back(rwo);
		GeometryGenerator gg(roommodel);
		gg.generate();
		std::vector<Rect> rectangles;
		gg.getRectangles(rectangles);

		// Count materials
		std::map<Material*, UMaterialInstanceDynamic*> materials;
		
		materials[&(roommodel->baseboardMaterial)] = NULL;
		materials[&(roommodel->wallMaterial)] = NULL;
		materials[&(roommodel->ceilingMaterial)] = NULL;
		materials[&(roommodel->floorMaterial)] = NULL;
		for (int i = 0; i < roommodel->walls.size(); ++i) {
			for (int j = 0; j < roommodel->walls[i].windows.size(); ++j) {
				materials[&(roommodel->walls[i].windows[j].frameMaterial)] = NULL;
				materials[&(roommodel->walls[i].windows[j].material)] = NULL;
			}
		}

		if (GEngine) {
			for (auto it = materials.begin(); it != materials.end(); ++it) {
				UMaterialInstanceDynamic* newmat;
				Material* m = it->first;
				if (m->texture != NULL) {
					newmat = UMaterialInstanceDynamic::Create(basetexturedmaterial.Object, this);
					int w = m->texture->width;
					int h = m->texture->height;
					UTexture2D* newtex = UTexture2D::CreateTransient(w, h, PF_A32B32G32R32F);
					newtex->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
					newtex->SRGB = 0;
					newtex->AddToRoot();
					newtex->UpdateResource();
					FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(0, 0, 0, 0, w, h);
					UpdateTextureRegions(newtex, 0, 1, region, w * 4 * sizeof(float), 4 * sizeof(float), (uint8*)m->texture->texture, false);
					newmat->SetTextureParameterValue("TextureImage", newtex);
				}
				else {
					newmat = UMaterialInstanceDynamic::Create(basediffusematerial.Object, this);
					newmat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(m->diffuse.r, m->diffuse.g, m->diffuse.b));
				}
				it->second = newmat;
			}


			for (int i = 0; i < rectangles.size(); ++i) {
				// Generate cube component
				UStaticMeshComponent* component = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FName(*(FString::Printf(TEXT("roomrectangle%d"), i))));
				component->bAutoRegister = true;
				component->SetStaticMesh(cubemesh.Object);
				component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				component->AttachTo(GetRootComponent());
				component->RegisterComponent();
				FTransform t;
				FVector p;
				FVector d(0, 0, 0);
				for (int j = 0; j < 3; ++j) {
					p[j] = rectangles[i].p[j] * unitscale;
				}
				d[rectangles[i].axisIndices().first] = (rectangles[i].w)*unitscale;
				d[rectangles[i].axisIndices().second] = (rectangles[i].h)*unitscale;
				p = p + 0.5 * d;
				d[rectangles[i].axis] = rectangles[i].depth*unitscale;
				p[rectangles[i].axis] -= rectangles[i].normal*0.5*d[rectangles[i].axis];
				d /= cubescale;
				t.SetScale3D(d);
				t.SetLocation(p);
				component->SetWorldTransform(t);
				component->SetVisibility(true);
				UE_LOG(LogTemp, Warning, TEXT("%f %f %f %f %f %f"), rectangles[i].w, rectangles[i].h, rectangles[i].axis, rectangles[i].p[0], rectangles[i].p[1], rectangles[i].p[2]);
				if (materials.find(rectangles[i].material) != materials.end() && materials[rectangles[i].material] != NULL) {
					if (rectangles[i].material->texture != NULL) {
						double numtilings = rectangles[i].w / (rectangles[i].material->texture->scale*rectangles[i].material->texture->width);
						materials[rectangles[i].material]->SetScalarParameterValue("UScale", numtilings);
						numtilings = rectangles[i].h / (rectangles[i].material->texture->scale*rectangles[i].material->texture->height);
						materials[rectangles[i].material]->SetScalarParameterValue("VScale", numtilings);
					}
					component->SetMaterial(0, materials[rectangles[i].material]);
				}
				else component->SetMaterial(0, materials[&(roommodel->wallMaterial)]);
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("OH CRAP"));
	}
}

// Called when the game starts or when spawned
void AGeneratedRoomActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGeneratedRoomActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AGeneratedRoomActor::SetBaseboardHeight(float newBBHeight) {
	roommodel->baseboardHeight = newBBHeight;
	Update();
}
void AGeneratedRoomActor::SetBaseboardDepth(float newBBDepth) {
	roommodel->baseboardDepth = newBBDepth;
	Update();
}
void AGeneratedRoomActor::SetRoomHeight(float newHeight) {
	roommodel->height = newHeight;
	Update();
}

void AGeneratedRoomActor::Update() {
	GeometryGenerator gg(roommodel);
	gg.generate();
	std::vector<Rect> rectangles;
	gg.getRectangles(rectangles);
	for (int i = 0; i < rectangles.size(); ++i) {
		USceneComponent* component = RootComponent->GetChildComponent(i);
		FTransform t;
		FVector p;
		FVector d(0, 0, 0);
		for (int j = 0; j < 3; ++j) {
			p[j] = rectangles[i].p[j] * unitscale;
		}
		d[rectangles[i].axisIndices().first] = (rectangles[i].w)*unitscale;
		d[rectangles[i].axisIndices().second] = (rectangles[i].h)*unitscale;
		p = p + 0.5 * d;
		d[rectangles[i].axis] = rectangles[i].depth*unitscale;
		p[rectangles[i].axis] -= rectangles[i].normal*0.5*d[rectangles[i].axis];
		d /= cubescale;
		t.SetScale3D(d);
		t.SetLocation(p);
		component->SetWorldTransform(t);
	}
}

void AGeneratedRoomActor::SaveRoomToFile(FString filename) {
	save(*roommodel, TCHAR_TO_UTF8(*filename));
}