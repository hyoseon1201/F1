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
#include "Components/CapsuleComponent.h"
#include <AbilitySystem/F1AbilitySystemLibrary.h>

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

    if (HasAuthority())
    {
        // 1. 데이터 설정 (GE/GA 클래스 정보 로드 및 복제 시작)
        SetCharacterClass(FName("Dekker"));

        // 2. GAS 연결 (ASC/AS 포인터 설정)
        InitAbilityActorInfo();

        // 3. Attributes 및 Abilities 부여 (Library 사용)
        UF1AbilitySystemLibrary::InitializeDefaultAttributes(this);
        UF1AbilitySystemLibrary::AddCharacterAbilities(this);

        // 4. 레벨 기반 성장 적용 (있다면)
        ApplyLevelBasedGrowth();
    }
}

void AF1HeroCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // 1. GAS 연결 (클라이언트에서 PlayerState 데이터와 Character 연결)
    InitAbilityActorInfo();

    if (IsLocallyControlled()) // 또는 이 캐릭터가 보는 화면이라면
    {
        // 델리게이트 등록 및 최초 바인딩
        BindMovementSpeedDelegate();
        SyncMovementSpeedWithAttributeSet();
    }

    // 2. UI 초기화 (ASC 연결 및 데이터 복제가 완료되었을 가능성이 높은 시점)
    InitializeHealthBarWidget();
}

void AF1HeroCharacter::SetCharacterClass(FName CharacterRowName)
{
    // ... 유효성 검사 (유지) ...
    if (!HasAuthority() || !CharacterClassDataTable) return;

    FCharacterClassInfo* ClassInfo = CharacterClassDataTable->FindRow<FCharacterClassInfo>(CharacterRowName, TEXT("SetCharacterClass"));
    if (!ClassInfo) return;

    CurrentCharacterInfo = *ClassInfo;
    DefaultAttributes = CurrentCharacterInfo.DefaultAttributes;
    GrowthAttributes = CurrentCharacterInfo.GrowthAttributes;
    StartupAbilities = CurrentCharacterInfo.StartupAbilities;

    UpdateCombatSocketsFromCharacterInfo();

    ApplyVisualsFromCurrentInfo();
}

int32 AF1HeroCharacter::GetCurrentLevel() const
{
    int32 BaseLevel = Super::GetCurrentLevel();

    return BaseLevel;
}

float AF1HeroCharacter::GetCurrentExperience() const
{
    return Super::GetCurrentExperience();
}

void AF1HeroCharacter::ApplyLevelBasedGrowth()
{
    if (!HasAuthority() || !GrowthAttributes)
    {
        return;
    }

    int32 CurrentLevel = GetCurrentLevel();
    if (CurrentLevel <= 1)
    {
        return;
    }

    const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(
        GrowthAttributes,
        CurrentLevel - 1,
        ContextHandle
    );
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

FVector AF1HeroCharacter::GetCombatSocketLocation()
{
    return Super::GetCombatSocketLocation();
}

void AF1HeroCharacter::Die()
{
    SetLifeSpan(LifeSpan);
    Super::Die();
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

    GetCapsuleComponent()->SetCapsuleRadius(CurrentCharacterInfo.CapsuleRadius);
    GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentCharacterInfo.CapsuleHalfHeight);

    GetMesh()->SetRelativeLocation(CurrentCharacterInfo.MeshRelativeLocation);
    GetMesh()->SetRelativeRotation(CurrentCharacterInfo.MeshRelativeRotation);
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

void AF1HeroCharacter::UpdateCombatSocketsFromCharacterInfo()
{
    WeaponTipSocketName = CurrentCharacterInfo.WeaponTipSocketName;
    MuzzleSocketName = CurrentCharacterInfo.MuzzleSocketName;
    HandSocketName = CurrentCharacterInfo.HandSocketName;
    ChestSocketName = CurrentCharacterInfo.ChestSocketName;
}

void AF1HeroCharacter::InitAbilityActorInfo()
{
    AF1PlayerState* F1PlayerState = GetPlayerState<AF1PlayerState>();
    if (!F1PlayerState) return;

    // 1. GAS 연결 (Owner: PlayerState, Avatar: Character)
    F1PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(F1PlayerState, this);
    Cast<UF1AbilitySystemComponent>(F1PlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

    // 2. 포인터 설정
    AbilitySystemComponent = F1PlayerState->GetAbilitySystemComponent();
    AttributeSet = F1PlayerState->GetAttributeSet();

    // 3. HUD 초기화 (로컬 플레이어 전용)
    if (IsLocallyControlled())
    {
        if (AF1PlayerController* F1PlayerController = Cast<AF1PlayerController>(GetController()))
        {
            if (AF1HUD* F1HUD = Cast<AF1HUD>(F1PlayerController->GetHUD()))
            {
                // HUD 초기화는 ASC 연결 직후, 로컬 플레이어 클라이언트에서 수행합니다.
                F1HUD->InitOverlay(F1PlayerController, F1PlayerState, AbilitySystemComponent, AttributeSet);
            }
        }
    }
}