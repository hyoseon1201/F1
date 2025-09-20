// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/F1HeroCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Game/F1PlayerState.h"
#include "AbilitySystemComponent.h"
#include "Game/F1PlayerController.h"
#include "UI/HUD/F1HUD.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "Net/UnrealNetwork.h"

AF1HeroCharacter::AF1HeroCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// TEMP
	SetGenericTeamId(FGenericTeamId(1));
}

void AF1HeroCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(AF1HeroCharacter, CurrentCharacterInfo, COND_None, REPNOTIFY_Always);
}

void AF1HeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
    AddCharacterAbilities();
}

void AF1HeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void AF1HeroCharacter::SetCharacterClass(FName CharacterRowName)
{
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("SetCharacterClass called without authority!"));
        return;
    }

    if (!CharacterClassDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("CharacterClassDataTable is null!"));
        return;
    }

    FCharacterClassInfo* ClassInfo = CharacterClassDataTable->FindRow<FCharacterClassInfo>(CharacterRowName, TEXT("SetCharacterClass"));

    if (!ClassInfo)
    {
        UE_LOG(LogTemp, Error, TEXT("Character class '%s' not found in DataTable!"), *CharacterRowName.ToString());
        return;
    }

    CurrentCharacterInfo = *ClassInfo;
    DefaultAttributes = CurrentCharacterInfo.DefaultAttributes;
    GrowthAttributes = CurrentCharacterInfo.GrowthAttributes;

    if (DefaultAttributes)
    {
        InitializeDefaultAttributes();
    }

    ApplyGrowthForCurrentLevel();

    ApplyVisualsFromCurrentInfo();
    
    UE_LOG(LogTemp, Warning, TEXT("Character Class '%s' set successfully!"), *CurrentCharacterInfo.CharacterName);
}

int32 AF1HeroCharacter::GetCurrentLevel() const
{
    if (const UF1AttributeSet* AS = Cast<UF1AttributeSet>(AttributeSet))
    {
        return FMath::TruncToInt(AS->GetCharacterLevel());
    }
    return 1;
}

float AF1HeroCharacter::GetCurrentExperience() const
{
    if (const UF1AttributeSet* AS = Cast<UF1AttributeSet>(AttributeSet))
    {
        return AS->GetExperience();
    }
    return 0.0f;
}

void AF1HeroCharacter::OnRep_CurrentCharacterInfo()
{
    ApplyVisualsFromCurrentInfo();
}

void AF1HeroCharacter::ApplyVisualsFromCurrentInfo()
{
    if (CurrentCharacterInfo.CharacterMesh)
    {
        GetMesh()->SetSkeletalMesh(CurrentCharacterInfo.CharacterMesh);
    }

    if (CurrentCharacterInfo.AnimBlueprint)
    {
        GetMesh()->SetAnimInstanceClass(CurrentCharacterInfo.AnimBlueprint);
    }
}

void AF1HeroCharacter::ApplyLevelUpGrowth()
{
    if (!HasAuthority()) return;
    check(IsValid(GetAbilitySystemComponent()));

    if (!GrowthAttributes)
    {
        UE_LOG(LogTemp, Warning, TEXT("GrowthAttributes is null for hero!"));
        return;
    }

    const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GrowthAttributes, 1.f, ContextHandle);
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AF1HeroCharacter::ApplyGrowthForCurrentLevel()
{
    if (!HasAuthority() || !GrowthAttributes) return;

    int32 CurrentLevel = GetCurrentLevel();

    if (CurrentLevel <= 1) return;

    const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GrowthAttributes, CurrentLevel - 1, ContextHandle);
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AF1HeroCharacter::SyncMovementSpeedWithAttributeSet()
{
    if (const UF1AttributeSet* AS = Cast<UF1AttributeSet>(AttributeSet))
    {
        if (GetCharacterMovement())
        {
            float CurrentSpeed = AS->GetMovementSpeed();
            GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;;
        }
    }
}


void AF1HeroCharacter::InitAbilityActorInfo()
{
    AF1PlayerState* F1PlayerState = GetPlayerState<AF1PlayerState>();

    if (!F1PlayerState)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerState not valid in InitAbilityActorInfo"));
        return;
    }

    UAbilitySystemComponent* ASC = F1PlayerState->GetAbilitySystemComponent();

    F1PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(F1PlayerState, this);
    Cast<UF1AbilitySystemComponent>(F1PlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

    AbilitySystemComponent = F1PlayerState->GetAbilitySystemComponent();
    AttributeSet = F1PlayerState->GetAttributeSet();

    if (IsLocallyControlled())
    {
        if (AF1PlayerController* F1PlayerController = Cast<AF1PlayerController>(GetController()))
        {
            if (AF1HUD* F1HUD = Cast<AF1HUD>(F1PlayerController->GetHUD()))
            {
                F1HUD->InitOverlay(F1PlayerController, F1PlayerState, AbilitySystemComponent, AttributeSet);
            }
        }
    }

    if (HasAuthority())
    {
        SetCharacterClass(FName("Crunch"));
    }
    else
    {
        SyncMovementSpeedWithAttributeSet();
    }
}
