#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Interaction/F1TeamOutlineInterface.h"
#include "AbilitySystemInterface.h"
#include "F1CharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;

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
    virtual void InitAbilityActorInfo();
    // ===========================================
    // Team system
    // ===========================================
public:
    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) { TeamID = NewTeamID; }
    virtual FGenericTeamId GetGenericTeamId() const { return TeamID; }

protected:
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Team")
    FGenericTeamId TeamID = FGenericTeamId::NoTeam;

    // ===========================================
    // Outline Interface
    // ===========================================
public:
    virtual void HighlightActor() override;
    virtual void UnHighlightActor() override;

    UPROPERTY(BlueprintReadOnly, Category = "Highlight")
    bool bHighlighted = false;

private:
    bool IsEnemyToPlayer() const;


    // ===========================================
    // GAS
    // ===========================================
public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UAttributeSet> AttributeSet;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
    TSubclassOf<UGameplayEffect> DefaultAttributes;

    void InitializeDefaultAttributes();
    void AddCharacterAbilities();

private:
    UPROPERTY(EditAnywhere, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};
