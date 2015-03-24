// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomVisualizer.h"
#include "GeneratedRoomActor.h"

#include "rectanglerenderer.h"
#include "geometrygenerator.h"
#include "Engine.h"

#include <map>

FString AGeneratedRoomActor::defaultloadfile = "";
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

void generateRoomModel(RoomModel* roommodel) {
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
	int dim = 512;
	float* texture = new float[4 * dim * dim];
	for (int i = 0; i < dim*dim; ++i) {
		bool black = false;
		if (i / dim < dim / 2) black = !black;
		if (i % dim < dim / 2) black = !black;
		texture[i * 4 + 0] = black ? 0 : 0.7;
		texture[i * 4 + 1] = black ? 0 : 0.7;
		texture[i * 4 + 2] = black ? 0 : 0.7;
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
	rwo.wall = &(roommodel->walls[3]);
	roommodel->walls[3].windows.push_back(rwo);
	rwo.wall = &(roommodel->walls[2]);
	roommodel->walls[2].windows.push_back(rwo);
	rwo.wall = &(roommodel->walls[1]);
	roommodel->walls[1].windows.push_back(rwo);
	rwo.wall = &(roommodel->walls[0]);
	roommodel->walls[0].windows.push_back(rwo);

	RoomWindow* rw = new RoomWindow();
	rw->rwo = &(roommodel->walls[0].windows.back());
	rw->intensity = Color(500,500,500);
	roommodel->lights.push_back(rw);
	Light* l = new Light(FVector(-300,300,250),Color(1e4,1e4,1e4));
	l->cutoff = 65;
	l->direction = FVector(0, 0, -1);
	roommodel->lights.push_back(l);
}

// Sets default values
AGeneratedRoomActor::AGeneratedRoomActor(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	numlightcomponents = 0;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubemeshfinder(TEXT("/Game/FirstPerson/Meshes/UnitCube"));
	static ConstructorHelpers::FObjectFinder<UMaterial> basediffusematerialfinder(TEXT("/Game/FirstPerson/Meshes/DiffuseMaterialBase"));
	//static ConstructorHelpers::FObjectFinder<UMaterial> basetexturedmaterial(TEXT("/Game/FirstPerson/Meshes/TexturedMaterialMirrorBase"));
	static ConstructorHelpers::FObjectFinder<UMaterial> basetexturedmaterialfinder(TEXT("/Game/FirstPerson/Meshes/TexturedMaterialWrapBase"));
	static ConstructorHelpers::FObjectFinder<UMaterial> basewindowmaterialfinder(TEXT("/Game/FirstPerson/Meshes/WindowMaterial"));
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, "rootroomcomponent");
	if (cubemeshfinder.Succeeded()) cubemesh = cubemeshfinder.Object;
	if (basediffusematerialfinder.Succeeded()) basediffusematerial = basediffusematerialfinder.Object;
	if (basetexturedmaterialfinder.Succeeded()) basetexturedmaterial = basetexturedmaterialfinder.Object;
	if (basewindowmaterialfinder.Succeeded()) basewindowmaterial = basewindowmaterialfinder.Object;

	if (!cubemesh) return;
	if (!this->GetWorld()) return;

	roommodel = new RoomModel();
	if (defaultloadfile == "") {
		generateRoomModel(roommodel);
	}
	else {
		load(*roommodel, TCHAR_TO_UTF8(*defaultloadfile));
	}
	GeometryGenerator gg(roommodel);
	gg.generate();
	std::vector<Rect> rectangles;
	gg.getRectangles(rectangles);

	for (int i = 0; i < roommodel->lights.size(); ++i) {
		Light* light = roommodel->lights[i];
		if (light->getType() == "point" || light->getType() == "line") {
			USpotLightComponent* component = ObjectInitializer.CreateDefaultSubobject<USpotLightComponent>(this, FName(*(FString::Printf(TEXT("spotlight%d"), i))));
			++numlightcomponents;
			component->bAutoRegister = true;
			component->AttachTo(GetRootComponent());
			component->RegisterComponent();
			FVector a, b, c;

			a = light->direction;
			FTransform t;

			if (light->getType() == "line") {
				LineLight* l = (LineLight*)light;
				FVector v;
				float d;
				(l->position - l->endpoint).ToDirectionAndLength(v, d);
				t.SetLocation((l->position + l->endpoint) / 2);
				a -= FVector::DotProduct(a, v)*v;
				component->SourceLength = d;
			}
			else {
				t.SetLocation(light->position);
			}
			if (!a.IsNearlyZero()) {
				a.Normalize();
				b = FVector(1, 0, 0);
				double r = 1 + FVector::DotProduct(a, b);
				if (r < 1e-8) {
					r = 0;
					c = abs(c.X) > abs(c.Z) ? FVector(-c.Y, c.X, 0) : FVector(0, -c.Z, c.Y);
				}
				else {
					c = FVector::CrossProduct(a, b);
				}
				FQuat q(r, c.X, c.Y, c.Z);
				q.Normalize();
				t.SetRotation(q);
			}

			component->SetWorldTransform(t);
			double m = std::max(light->intensity.r, std::max(light->intensity.g, light->intensity.b));
			component->SetLightColor(FLinearColor(light->intensity.r / m, light->intensity.g / m, light->intensity.b / m));
			component->SetIntensity(m);
			component->SetLightFalloffExponent(light->dropoff);
			component->SetOuterConeAngle(light->cutoff);
			component->SetVisibility(true);
		}
		else if (light->getType() == "window") {
			RoomWindow* l = (RoomWindow*)light;
			UStaticMeshComponent* component = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FName(*(FString::Printf(TEXT("window%d"), i))));
			++numlightcomponents;
			component->bAutoRegister = true;
			component->SetStaticMesh(cubemesh);
			component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			component->AttachTo(GetRootComponent());
			component->RegisterComponent();
			Rect r = gg.getRectangleForWindow(l->rwo);
			FTransform t;
			FVector p;
			FVector d(0, 0, 0);
			for (int j = 0; j < 3; ++j) {
				p[j] = r.p[j] * unitscale;
			}
			d[r.axisIndices().first] = (r.w)*unitscale;
			d[r.axisIndices().second] = (r.h)*unitscale;
			p = p + 0.5 * d;
			d[r.axis] = r.depth*unitscale;
			p[r.axis] -= r.normal*0.5*d[r.axis];
			d /= cubescale;
			t.SetScale3D(d);
			t.SetLocation(p);
			component->SetWorldTransform(t);
			component->SetVisibility(true);
		}
	}


	for (int i = 0; i < rectangles.size(); ++i) {
		// Generate cube component
		UStaticMeshComponent* component = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FName(*(FString::Printf(TEXT("roomrectangle%d"), i))));
		component->bAutoRegister = true;
		component->SetStaticMesh(cubemesh);
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
	}
}

// Called when the game starts or when spawned
void AGeneratedRoomActor::BeginPlay()
{
	Super::BeginPlay();

}


void AGeneratedRoomActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GeometryGenerator gg(roommodel);
	gg.generate();
	std::vector<Rect> rectangles;
	gg.getRectangles(rectangles);

	// Count materials
	std::map<Material*, UMaterialInstanceDynamic*> materials;
	UMaterialInstanceDynamic* tmp = (UMaterialInstanceDynamic*)1;

	materials[&(roommodel->baseboardMaterial)] = NULL;
	materials[&(roommodel->wallMaterial)] = NULL;
	materials[&(roommodel->ceilingMaterial)] = NULL;
	materials[&(roommodel->floorMaterial)] = NULL;
	for (int i = 0; i < roommodel->walls.size(); ++i) {
		for (int j = 0; j < roommodel->walls[i].windows.size(); ++j) {
			materials[&(roommodel->walls[i].windows[j].frameMaterial)] = NULL;
			materials[&(roommodel->walls[i].windows[j].material)] = tmp;
		}
	}
	for (auto it = materials.begin(); it != materials.end(); ++it) {
		UMaterialInstanceDynamic* newmat;
		Material* m = it->first;
		if (it->second != NULL) {
			newmat = UMaterialInstanceDynamic::Create(basewindowmaterial, this);
			// Set sun direction appropriately
			// newmat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(m->diffuse.r, m->diffuse.g, m->diffuse.b));
		}
		else {
			if (m->texture != NULL) {
				newmat = UMaterialInstanceDynamic::Create(basetexturedmaterial, this);
			}
			else {
				newmat = UMaterialInstanceDynamic::Create(basediffusematerial, this);
				newmat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(m->diffuse.r, m->diffuse.g, m->diffuse.b));
			}
		}
		if (m->texture != NULL) {
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
		it->second = newmat;
	}
	for (int i = 0; i < roommodel->lights.size(); ++i) {
		Light* light = roommodel->lights[i];
		if (light->getType() == "point" || light->getType() == "line") {
		} else if (light->getType() == "window") {
			RoomWindow* l = (RoomWindow*)light;
			// TODO: Set sun direction, intensity
			materials[&(l->rwo->material)]->SetVectorParameterValue(TEXT("DiffuseComponent"), FLinearColor(l->intensity.r, l->intensity.g, l->intensity.b));
			UStaticMeshComponent* component = (UStaticMeshComponent*)RootComponent->GetChildComponent(i);
			component->SetMaterial(0, materials[&(l->rwo->material)]);
		}
	}
	for (int i = 0; i < rectangles.size(); ++i) {
		UStaticMeshComponent* component = (UStaticMeshComponent*) RootComponent->GetChildComponent(i+numlightcomponents);
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
		USceneComponent* component = RootComponent->GetChildComponent(i+numlightcomponents);
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

void AGeneratedRoomActor::setLoadFile(FString filename) {
	defaultloadfile = filename;
}

void AGeneratedRoomActor::SaveRoomToFile(FString filename) {
	save(*roommodel, TCHAR_TO_UTF8(*filename));
}
