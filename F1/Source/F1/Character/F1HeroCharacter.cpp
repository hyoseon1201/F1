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

    // DataTable에서 캐릭터 정보 가져오기
    FCharacterClassInfo* ClassInfo = CharacterClassDataTable->FindRow<FCharacterClassInfo>(CharacterRowName, TEXT("SetCharacterClass"));

    if (!ClassInfo)
    {
        UE_LOG(LogTemp, Error, TEXT("Character class '%s' not found in DataTable!"), *CharacterRowName.ToString());
        return;
    }

    // 현재 캐릭터 정보 저장
    CurrentCharacterInfo = *ClassInfo;

    DefaultAttributes = CurrentCharacterInfo.DefaultAttributes;
    GrowthAttributes = CurrentCharacterInfo.GrowthAttributes;

    if (DefaultAttributes)
    {
        InitializeDefaultAttributes();
    }

    ApplyGrowthForCurrentLevel();

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
    FString RoleString = HasAuthority() ? TEXT("Server") : TEXT("Client");
    UE_LOG(LogTemp, Warning, TEXT("%s: OnRep_CurrentCharacterInfo triggered"), *RoleString);

    ApplyVisualsFromCurrentInfo();
}

void AF1HeroCharacter::ApplyVisualsFromCurrentInfo()
{
    if (CurrentCharacterInfo.CharacterMesh)
    {
        GetMesh()->SetSkeletalMesh(CurrentCharacterInfo.CharacterMesh);
        FString RoleString = HasAuthority() ? TEXT("Server") : TEXT("Client");
        UE_LOG(LogTemp, Warning, TEXT("%s: Applied Character Mesh: %s"), *RoleString, *CurrentCharacterInfo.CharacterMesh->GetName());
    }

    if (CurrentCharacterInfo.AnimBlueprint)
    {
        GetMesh()->SetAnimInstanceClass(CurrentCharacterInfo.AnimBlueprint);
        FString RoleString = HasAuthority() ? TEXT("Server") : TEXT("Client");
        UE_LOG(LogTemp, Warning, TEXT("%s: Applied AnimBlueprint: %s"), *RoleString, *CurrentCharacterInfo.AnimBlueprint->GetName());
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

    if (CurrentLevel <= 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("Level 1, no growth to apply"));
        return;
    }

    // 레벨 2부터 현재 레벨까지 성장 적용
    int32 GrowthLevels = CurrentLevel - 1;

    for (int32 i = 0; i < GrowthLevels; i++)
    {
        const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
        const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GrowthAttributes, 1.f, ContextHandle);
        GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }

    UE_LOG(LogTemp, Warning, TEXT("Applied %d growth levels for current level: %d"), GrowthLevels, CurrentLevel);
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
    check(F1PlayerState);

    FString RoleString = HasAuthority() ? TEXT("Server") : TEXT("Client");

    // InitAbilityActorInfo 호출 전 상태
    UAbilitySystemComponent* ASC = F1PlayerState->GetAbilitySystemComponent();

    // InitAbilityActorInfo 호출
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

    if (!HasAuthority())
    {
        SyncMovementSpeedWithAttributeSet();
    }
}
