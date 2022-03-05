#pragma once

#include "../GridActions/IGridInteractable.h"
#include "GridUnit.generated.h"

class UGridActionBase;
class UGridMovementComponent;
class USceneComponent;



UENUM()
enum EUnitBlocking
{
	Block = 0,
	LetPass = 1
};

UENUM()
enum EUnitAlignment
{
	Player,
	Enemy,
	Neutral,
	Friendly
};

UCLASS(Blueprintable, BlueprintType)
class AGridUnit : public APawn, public IGridInteractable
{
	GENERATED_BODY()

public:
	AGridUnit();
	~AGridUnit();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Unit Properties")
		TEnumAsByte<EUnitAlignment> Alignment;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Unit Properties")
		TMap<TEnumAsByte<EUnitAlignment>, TEnumAsByte<EUnitBlocking>> UnitBlocking =
	{
		{Player, LetPass},
		{Enemy, LetPass},
		{Neutral, LetPass},
		{Friendly, LetPass}
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components");
		UGridMovementComponent* GMC;


	TArray<UGridActionBase*> GetActions_Implementation() override;

private:
	USceneComponent* Root;

protected:
	void BeginPlay() override;

};