#pragma once

#include "../GridActions/IGridInteractable.h"
#include "GridUnit.generated.h"

class UGridActionBase;
class UGridMovementComponent;
class USceneComponent;

UCLASS(Blueprintable, BlueprintType)
class AGridUnit : public APawn, public IGridInteractable
{
	GENERATED_BODY()

public:
	AGridUnit();
	~AGridUnit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
		UGridMovementComponent* GMC;


	TArray<UGridActionBase*> GetActions_Implementation() override;

private:
	USceneComponent* Root;

protected:
	void BeginPlay() override;

};