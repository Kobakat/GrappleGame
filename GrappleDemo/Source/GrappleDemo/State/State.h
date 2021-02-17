#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "State.generated.h"

class APlayerPawn;

UCLASS()
class GRAPPLEDEMO_API UState : public UObject
{
	GENERATED_BODY()
	
public:
	UState();
	~UState();

	virtual void OnStateEnter();
	virtual void StateTick();
	virtual void OnStateExit();

	virtual void Initialize(APlayerPawn* pawn);
	FString stateName; /*For inspector display; have each state set this in initialize*/

protected:
	APlayerPawn* player;

	
};
