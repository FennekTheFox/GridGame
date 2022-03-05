#pragma once

#include "GridActionBase.h"
#include "GridAction_Attack.generated.h"

class UHexGridTile;

UCLASS(BlueprintType, Blueprintable, Category = "Grid Actions")
class UGridAction_Attack final : public UGridActionBase
{
	GENERATED_BODY()

	UGridAction_Attack();
	~UGridAction_Attack();

	//BEGIN UGridActionBase
	bool InitiateAction(UHexGridTile* InitTile) override;
	bool ExecuteAction(UHexGridTile* TargetTile) override;
	void AbortAction() override;
	bool CanExecuteAction(UHexGridTile* CheckTile) override;
	void UpdateVisualization(UHexGridTile* PotentialTarget) override;

	void ShowHoveredVisualization(UHexGridTile* PotentialTarget) override;
	void HideHoveredVisualization() override;
	//END GridActionBase

private:
	UFUNCTION()
		void ActionFinishedCallback();

private:
	UPROPERTY()
		class UGridAttackComponent* AttackComponent;
	UPROPERTY()
		TArray<class UHexGridTile*> VisualizedPath;
	UPROPERTY()
		UHexGridTile* VisualizedTarget = nullptr;
};
