#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Interaction/F1TeamOutlineInterface.h"
#include "Interaction/F1CombatInterface.h"
#include "AbilitySystemInterface.h"
#include "UI/WidgetController/F1OverlayWidgetController.h"
#include "F1CharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UWidgetComponent;

UCLASS()
class F1_API AF1CharacterBase : public ACharacter,
    public IGenericTeamAgentInterface,
    public IF1TeamOutlineInterface,
    public IAbilitySystemInterface,
    public IF1CombatInterface
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
    // Combat Interface
    // ===========================================
public:
    virtual int32 GetCurrentLevel() const override;
    virtual float GetCurrentExperience() const override;
    virtual void ApplyLevelBasedGrowth() override;

    virtual FVector GetCombatSocketLocation() override;
    virtual void Die() override;

    UFUNCTION(NetMulticast, Reliable)
    virtual void MulticastHandleDeath();

    // ===========================================
    // Combat System
    // ===========================================
protected:
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Sockets")
    FName WeaponTipSocketName = FName("WeaponTip");

    UPROPERTY(BlueprintReadOnly, Category = "Combat|Sockets")
    FName MuzzleSocketName = FName("Muzzle");

    UPROPERTY(BlueprintReadOnly, Category = "Combat|Sockets")
    FName HandSocketName = FName("RightHand");

    UPROPERTY(BlueprintReadOnly, Category = "Combat|Sockets")
    FName ChestSocketName = FName("Chest");

    virtual void UpdateCombatSocketsFromCharacterInfo();

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

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes")
    TSubclassOf<UGameplayEffect> DefaultAttributes;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes")
    TSubclassOf<UGameplayEffect> GrowthAttributes;

    UPROPERTY(EditAnywhere, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

    void InitializeDefaultAttributes();
    void AddCharacterAbilities();

    // ===========================================
    // Wolrd Widget
    // ===========================================

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UWidgetComponent> HealthBar;

    UPROPERTY(BlueprintAssignable)
    FOnAttributeChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FOnAttributeChangedSignature OnMaxHealthChanged;

    void InitializeHealthBarWidget();

private:
    bool bHealthBarInitialized = false;
};
