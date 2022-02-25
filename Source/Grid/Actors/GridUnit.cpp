#include "GridUnit.h"
#include "Grid/GridComponents/GridMovementComponent.h"
#include "Components/SceneComponent.h"

AGridUnit::AGridUnit()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	GMC = CreateDefaultSubobject<UGridMovementComponent>(TEXT("Grid Movement"));
}

AGridUnit::~AGridUnit()
{

}

TArray<UGridActionBase*> AGridUnit::GetActions_Implementation()
{
	TArray<UGridActionBase*> Actions;

	TInlineComponentArray<UActorComponent*> Components(this);
	for (UActorComponent* Component : Components)
	{
		IGridActionComponent* AsActionComp = Cast<IGridActionComponent>(Component);

		if (AsActionComp)
		{
			AsActionComp->CollectActions(Actions);
		}
	}

	return Actions;
}

void AGridUnit::BeginPlay()
{
	Super::BeginPlay();

	GMC->AttachToGrid();
}

