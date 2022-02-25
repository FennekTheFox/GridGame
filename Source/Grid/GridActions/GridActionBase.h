#pragma once

#include "GridActionBase.generated.h"

#define LOCTEXT_NAMESPACE "ActionBase"

class UHexGridTile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGridActionDelegate, UGridActionBase*, Action);

UCLASS(Abstract, BlueprintType, Blueprintable, Category = "Grid Actions")
class UGridActionBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Action Base")
		virtual bool InitiateAction(UHexGridTile* InitialTile) { return false; };

	UFUNCTION(BlueprintCallable, Category = "Action Base")
		virtual bool ExecuteAction(UHexGridTile* TargetTile) { return false; };

	UFUNCTION(BlueprintCallable, Category = "Action Base")
		virtual void AbortAction() {};

	UFUNCTION(BlueprintPure, Category = "Action Base")
		virtual bool CanExecuteAction(UHexGridTile* TargetTile) { return false; };


	//Visualization
	UFUNCTION(BlueprintCallable, Category = "Action Base")
		virtual void UpdateVisualization(UHexGridTile* PotentialTarget) {};
	UFUNCTION(BlueprintCallable, Category = "Action Base")
		virtual void ShowHoveredVisualization(UHexGridTile* PotentialTarget) {};
	UFUNCTION(BlueprintCallable, Category = "Action Base")
		virtual void HideHoveredVisualization() {};

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action Properties")
		FText ActionName = LOCTEXT("GridAction_None", "None");
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action Properties")
		FText ActionCategory = LOCTEXT("GridActionCategory_General", "General");

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "ActionProperties|Callbacks")
		FGridActionDelegate OnActionStarted;
	UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "ActionProperties|Callbacks")
		FGridActionDelegate OnActionFinished;
};

#undef LOCTEXT_NAMESPACE