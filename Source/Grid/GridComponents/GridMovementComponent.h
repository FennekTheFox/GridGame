#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Grid/GridActor.h"
#include "IGridActionComponent.h"

#include "GridMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGridMovementDelegate);


UENUM()
enum EGridMovementState
{
	Idle, 
	Walking,
	Jumping, 
	Landing,
};


UCLASS(ClassGroup=(Grid), Blueprintable, meta=(BlueprintSpawnableComponent))
class UGridMovementComponent : public UActorComponent, public IGridActionComponent
{
	GENERATED_BODY()

public:
	UGridMovementComponent();

	UFUNCTION(BlueprintCallable)
		void AttachToGrid(AGridActor* NewGrid = nullptr);
	UFUNCTION(BlueprintCallable)
		bool MoveToTile(UPARAM(ref)UHexGridTile* TargetTile);
	UFUNCTION(BlueprintCallable)
		void AbortMovement();
	UFUNCTION(BlueprintCallable)
		void PauseMovement();
	UFUNCTION(BlueprintCallable)
		void ResumeMovement();
	UFUNCTION(BlueprintPure)
		bool CanMoveToTile(UPARAM(ref)UHexGridTile* TargetTile, TArray<UHexGridTile*>& PotentialPath);
	UFUNCTION(BlueprintPure)
		TArray<UHexGridTile*> GetAllReachableTiles(TArray<UHexGridTile*>& ReachableTiles);
	UFUNCTION(BlueprintCallable)
		void SetShowMovableArea(bool bShow);


	//BEGIN IGridActionComponent
	void CollectActions(TArray<UGridActionBase*>& OutActions) override;
	//END IGridActionComponent


	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsComponentTickEnabled() const override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Settings")
		float MovementSpeed = 300;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Settings")
		int32 MovementRadius = 10;

	UPROPERTY(BlueprintAssignable)
		FGridMovementDelegate OnComplete;

	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte<EGridMovementState> MovementState = EGridMovementState::Idle;
	UPROPERTY(BlueprintReadOnly)
		UHexGridTile* CurrentTile;
	UPROPERTY(BlueprintReadOnly)
		AGridActor* Grid;
	UPROPERTY(BlueprintReadOnly)
		UGridPathFinderAgent* PathFinder;


private:
	bool bInMotion = false;
	bool bPaused = false;
	TArray<UHexGridTile*> PathToTravel;
	UHexGridTile* NextTile;
	FVector NextTileLoc;
	FVector CurrentTileLoc;
	float DistanceToTravel;
	float TimePassed;

protected:
	void OnRegister() override;
	void OnUnregister() override;

};