// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/F1MonsterCharacter.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/F1AbilitySystemLibrary.h"
#include "AI/F1AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "F1.h"
#include "GameplayTag/F1GameplayTags.h"
#include "UI/Widget/F1UserWidget.h"
#include "Components/WidgetComponent.h"

AF1MonsterCharacter::AF1MonsterCharacter()
{
    AbilitySystemComponent = CreateDefaultSubobject<UF1AbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AttributeSet = CreateDefaultSubobject<UF1AttributeSet>("AttributeSet");

    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
    HealthBar->SetupAttachment(GetRootComponent());

    AttackAbilityTag = FF1GameplayTags::Get().Ability_Attack_Melee; // BP에서 따로 설정
}

void AF1MonsterCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (!HasAuthority()) return;
    F1AIController = Cast<AF1AIController>(NewController);
    F1AIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
    F1AIController->GetBlackboardComponent()->SetValueAsVector(FName("HomeLocation"), GetActorLocation());
    F1AIController->GetBlackboardComponent()->SetValueAsFloat(FName("AttackRange"), DefaultAttackRange);
    F1AIController->RunBehaviorTree(BehaviorTree);
}

void AF1MonsterCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitAbilityActorInfo();

    InitUI();
}

void AF1MonsterCharacter::InitAbilityActorInfo()
{
    AbilitySystemComponent->InitAbilityActorInfo(this, this);
    Cast<UF1AbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

    // 1. 서버 전용: Attributes 및 Abilities 부여 (Library 사용)
    if (HasAuthority())
    {
        UF1AbilitySystemLibrary::InitializeDefaultAttributes(this);
        UF1AbilitySystemLibrary::AddCharacterAbilities(this);
    }
}

int32 AF1MonsterCharacter::GetRewardXP_Implementation()
{
    return RewardXP;
}

int32 AF1MonsterCharacter::GetRewardGold_Implementation()
{
    return RewardGold;
}

void AF1MonsterCharacter::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}

void AF1MonsterCharacter::SetCombatTarget(AActor* InTarget)
{
    CombatTarget = InTarget;

    if (F1AIController && F1AIController->GetBlackboardComponent())
    {
        F1AIController->GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), InTarget);
    }
}

void AF1MonsterCharacter::Attack()
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackAbilityTag));
    }
}

AActor* AF1MonsterCharacter::GetCombatTarget_Implementation() const
{
    return CombatTarget;
}

void AF1MonsterCharacter::MulticastHandleDeath_Implementation()
{
    // 1. 부모의 기능(래그돌, 애니 중단 등)을 먼저 실행!
    Super::MulticastHandleDeath_Implementation();

    // 2. [몬스터 전용] AI 끄기
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        AIC->StopMovement();
        if (AIC->GetBrainComponent())
        {
            AIC->GetBrainComponent()->StopLogic("Dead");
        }
    }

    // 3. [몬스터 전용] 5초 뒤 삭제
    SetLifeSpan(5.0f);
}

void AF1MonsterCharacter::InitUI()
{
    // 부모 클래스의 로직(있다면) 실행
    Super::InitUI();

    // 1. 방어 코드: 이미 초기화됐거나, 필수 요소가 없으면 중단
    if (bHealthBarInitialized) return;
    if (!AbilitySystemComponent || !AttributeSet) return;
    if (!HealthBar) return; // HealthBar는 WidgetComponent

    // 2. 위젯 객체 가져오기
    UUserWidget* Widget = HealthBar->GetUserWidgetObject();
    if (!Widget) return;

    // 3. 우리가 만든 F1UserWidget으로 캐스팅
    UF1UserWidget* F1UserWidget = Cast<UF1UserWidget>(Widget);
    if (!F1UserWidget) return;

    // ====================================================
    // [핵심] 몬스터는 "나 자신(this)"을 위젯 컨트롤러로 설정합니다.
    // 몬스터는 구조가 단순해서 별도의 Controller 클래스를 안 만듭니다.
    // ====================================================
    F1UserWidget->SetWidgetController(this);

    // 4. GAS 델리게이트 바인딩
    // AttributeSet에서 값이 변할 때 -> 내 델리게이트(OnHealthChanged)를 호출해라
    const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);

    // Health 변경 감지
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetHealthAttribute())
        .AddLambda([this](const FOnAttributeChangeData& Data)
            {
                OnHealthChanged.Broadcast(Data.NewValue);
            });

    // MaxHealth 변경 감지
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMaxHealthAttribute())
        .AddLambda([this](const FOnAttributeChangeData& Data)
            {
                OnMaxHealthChanged.Broadcast(Data.NewValue);
            });

    // 5. [중요] 초기값 강제 방송 (Broadcast Initial Values)
    // 게임 시작 시점의 체력(꽉 찬 상태)을 UI에 즉시 반영합니다.
    const float InitialHealth = F1AS->GetHealth();
    const float InitialMaxHealth = F1AS->GetMaxHealth();

    OnHealthChanged.Broadcast(InitialHealth);
    OnMaxHealthChanged.Broadcast(InitialMaxHealth);

    // 6. 위젯 보이게 설정 (숨겨져 있었다면)
    HealthBar->SetHiddenInGame(false);
    HealthBar->SetVisibility(true);

    // 초기화 완료 플래그 세팅
    bHealthBarInitialized = true;
}
