#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Interaction/F1TeamOutlineInterface.h"
#include "AbilitySystemInterface.h"
#include "F1CharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class F1_API AF1CharacterBase : public ACharacter,
    public IGenericTeamAgentInterface,
    public IF1TeamOutlineInterface,
    public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AF1CharacterBase();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Team System
public:
    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) { TeamID = NewTeamID; }
    virtual FGenericTeamId GetGenericTeamId() const { return TeamID; }

protected:
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Team")
    FGenericTeamId TeamID = FGenericTeamId::NoTeam;
#pragma endregion

#pragma region Outline Interface
public:
    virtual void HighlightActor() override;
    virtual void UnHighlightActor() override;

    UPROPERTY(BlueprintReadOnly, Category = "Highlight")
    bool bHighlighted = false;

private:
    bool IsEnemyToPlayer() const;
#pragma endregion

#pragma region GAS
public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UAttributeSet> AttributeSet;
#pragma endregion
};
