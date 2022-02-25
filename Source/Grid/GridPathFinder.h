#pragma once

#include "GameplayTagContainer.h"
#include "GridPathFinder.generated.h"


struct FAStarHelper
{
public:
	FAStarHelper(UHexGridTile* From, UHexGridTile* To, AGridActor* InGrid)
		:Start(From)
		, Goal(To)
		, Grid(InGrid)
	{
		Comparer.FCost = &FCost;

		GCost.FindOrAdd(Start, 0.f);
		FCost.FindOrAdd(Start, DistanceHeuristic(Start, Goal));

		OpenSet.HeapPush(Start, Comparer);
	};

	bool Step();
	void CollectPath(TArray<UHexGridTile*>& Result) const;
public:
	bool Success = false;
private:
	int32 DistanceHeuristic(UHexGridTile* Probe, UHexGridTile* Target);

	struct FComparer
	{
		bool operator()(const UHexGridTile& L, const UHexGridTile& R) const
		{
			int32 LFCost = FCost->Contains(&L) ? FCost->FindChecked(&L) : TNumericLimits<int32>::Max();
			int32 RFCost = FCost->Contains(&R) ? FCost->FindChecked(&R) : TNumericLimits<int32>::Max();
			return LFCost <= RFCost;
		}
		TMap<UHexGridTile*, int32>* FCost;
	} Comparer;

	TArray<UHexGridTile*> OpenSet;
	TSet<UHexGridTile*> CloseSet;
	TMap<UHexGridTile*, int32> FCost;
	TMap<UHexGridTile*, int32> GCost;
	TMap<UHexGridTile*, UHexGridTile*> CameFrom;
	UHexGridTile* Start;
	UHexGridTile* Goal;
	AGridActor* Grid;
};


class AGridActor;
class UHexGridTile;

USTRUCT(BlueprintType)
struct FGridPathFinderRequest
{
	GENERATED_BODY()

public:
	FGridPathFinderRequest();

	UPROPERTY(BlueprintReadWrite, Category = "HexGridPathFindingRequest")
		AActor* Sender;

	UPROPERTY(BlueprintReadWrite, Category = "HexGridPathFindingRequest")
		UHexGridTile* Start;

	UPROPERTY(BlueprintReadWrite, Category = "HexGridPathFindingRequest")
		UHexGridTile* Goal;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "HexGridPathFindingRequest")
		int32 MaxCost;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "HexGridPathFindingRequest")
		int32 MaxSearchStep;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "HexGridPathFindingRequest")
		bool bRemoveDest;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "HexGridPathFindingRequest")
		FGameplayTagContainer ExtraTags;

	bool IsValid();
};

/**
	Default pathfinder using manhattan distance to calculate cost.
	Inherit from USquarePathFinder or UHexagonPathFinder to customize pathfinding
*/
UCLASS(Blueprintable)
class UGridPathFinderAgent : public UObject
{
	GENERATED_BODY()

public:
	UGridPathFinderAgent();
	virtual ~UGridPathFinderAgent();

	UFUNCTION(BlueprintPure, Category = "HexGridPathFinder")
		UHexGridTile* GetStart() const;

	UFUNCTION(BlueprintPure, Category = "HexGridPathFinder")
		UHexGridTile* GetDestination() const;

	UFUNCTION(BlueprintPure, Category = "HexGridPathFinder")
		AActor* GetSender() const;

	UFUNCTION(BlueprintPure, Category = "HexGridPathFinder")
		AGridActor* GetGrid() const;

	UFUNCTION(BlueprintPure, Category = "HexGridPathFinder")
		const FGameplayTagContainer& GetExtraTags() const;

	/** If return false, the pawn can't move from 'Start' to 'Dest', the path has been blocked, and the cost will be ignore */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGridPathFinder")
		bool IsReachable(const FGridPathFinderRequest InRequest);
	virtual bool IsReachable_Implementation(const FGridPathFinderRequest InRequest);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGridPathFinder")
		int32 GetCost(UHexGridTile* From, UHexGridTile* To);
	virtual int32 GetCost_Implementation(UHexGridTile* From, UHexGridTile* To);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGridPathFinder")
		int32 Heuristic(UHexGridTile* From, UHexGridTile* To);
	virtual int32 Heuristic_Implementation(UHexGridTile* From, UHexGridTile* To);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGridPathFinder")
		void Reset();
	virtual void Reset_Implementation() {};

	UFUNCTION(BlueprintCallable, Category = "HexGridPathFinder")
		virtual bool FindPath(const FGridPathFinderRequest InRequest, TArray<UHexGridTile*>& Result);
	UFUNCTION(BlueprintCallable, Category = "HexGridPathFinder")
		virtual void GetReachableTiles(const FGridPathFinderRequest InRequest, TArray<UHexGridTile*>& Result);

	FGridPathFinderRequest Request;

	UPROPERTY(BlueprintReadWrite)
		AGridActor* GridActor;
};


