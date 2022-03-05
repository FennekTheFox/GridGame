#pragma once

#include "IGridActionComponent.h"
#include "Components/ActorComponent.h"
#include "GridAttackComponent.generated.h"

class UGridMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGridAttackDelegate);

UCLASS(ClassGroup = (Grid), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class UGridAttackComponent : public UActorComponent, public IGridActionComponent
{
	GENERATED_BODY()

public:
	UGridAttackComponent();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Grid Attack Properties")
		int32 MinimumRange = 1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Grid Attack Properties")
		int32 MaximumRange = 1;


	UPROPERTY(BlueprintAssignable)
		FGridAttackDelegate OnComplete;


	//BEGIN IGridActionComponent
	void CollectActions(TArray<UGridActionBase*>& OutActions) override;
	//END IGridActionComponent

	UFUNCTION(BlueprintCallable)
		void SetShowAttackableArea(bool bShow);
	UFUNCTION(BlueprintCallable)
		void AttackTile(UHexGridTile* TargetTile);
	UFUNCTION(BlueprintPure)
		bool CanAttackTile(UPARAM(ref)UHexGridTile* TargetTile);

	UFUNCTION()
		void OnPreAttackMovementComplete();

	virtual void GetAllAttackableTiles(TArray<UHexGridTile*>& Result);
	virtual bool GetPathToClosestTileToAttackFrom(UHexGridTile* Target, TArray<UHexGridTile*>& Result);
	virtual UHexGridTile* GetClosestTileToAttackFrom(UHexGridTile* Target);
	virtual void GetTileToAttackFromSortedByDistance(UHexGridTile* Target, TArray<UHexGridTile*>& OutArray);
	virtual void GetAllTilesToAttackFrom(UHexGridTile* Target, TArray<UHexGridTile*>& Result);


	void BeginPlay() override;
	



private:
	TArray<UHexGridTile*> ShownAttackableTiles;
	UGridMovementComponent* MovementComponent;
};