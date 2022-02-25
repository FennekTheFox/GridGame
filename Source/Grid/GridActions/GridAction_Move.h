#pragma once

#include "GridActionBase.h"
#include "GridAction_Move.generated.h"

#define LOCTEXT_NAMESPACE "ActionBase"


UCLASS(BlueprintType, Blueprintable, Category = "Grid Actions")
class UGridAction_Move final : public UGridActionBase
{
	GENERATED_BODY()

	UGridAction_Move()
	{
		ActionName = LOCTEXT("GridAction_Move", "Move");
	}


	//BEGIN UGridActionBase
	bool InitiateAction(UHexGridTile* InitTile) override;
	bool ExecuteAction(UHexGridTile* TargetTile) override;
	void AbortAction() override;
	bool CanExecuteAction(UHexGridTile* CheckTile) override;
	void UpdateVisualization(UHexGridTile* PotentialTarget) override;
	//END GridActionBase

private:
	UFUNCTION()
	void ActionFinishedCallback();

	void ShowHoveredVisualization(UHexGridTile* PotentialTarget) override;

	void HideHoveredVisualization() override;


private:
	UPROPERTY()
		class UGridMovementComponent* MovementComponent;
	UPROPERTY()
		TArray<class UHexGridTile*> CachedPath;
};


#undef LOCTEXT_NAMESPACE