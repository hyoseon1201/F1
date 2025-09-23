// F1CharacterTypes.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayEffectTypes.h"
#include "F1CharacterTypes.generated.h"

class UGameplayEffect;
class USkeletalMesh;
class UAnimInstance;
class UTexture2D;

UENUM(BlueprintType)
enum class EAttackType : uint8
{
    Melee       UMETA(DisplayName = "Melee"),
    Ranged      UMETA(DisplayName = "Ranged")
};

/**
 * ĳ���� Ŭ���� ���� ����ü
 * DataTable���� �� è�Ǿ��� ��� ������ ����
 */
USTRUCT(BlueprintType)
struct F1_API FCharacterClassInfo : public FTableRowBase
{
    GENERATED_BODY()

    FCharacterClassInfo()
    {
        CharacterName = TEXT("Unknown");
    }

    // ĳ���� �⺻ ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Info")
    FString CharacterName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Info")
    UTexture2D* CharacterIcon = nullptr;

    // ���־� ���� (�޽�, �ִϸ��̼�)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Assets")
    USkeletalMesh* CharacterMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Assets")
    TSubclassOf<UAnimInstance> AnimBlueprint;

    // ĳ���� ���� ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Physics", meta = (ClampMin = "10.0", ClampMax = "200.0"))
    float CapsuleRadius = 42.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Physics", meta = (ClampMin = "30.0", ClampMax = "300.0"))
    float CapsuleHalfHeight = 96.0f;

    // �޽� ��ġ/ȸ�� ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Transform")
    FVector MeshRelativeLocation = FVector(0.0f, 0.0f, -96.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Transform")
    FRotator MeshRelativeRotation = FRotator(0.0f, -90.0f, 0.0f);

    // GAS �Ӽ� (Base Ŭ�������� ������ �͵�)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
    TSubclassOf<UGameplayEffect> DefaultAttributes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
    TSubclassOf<UGameplayEffect> GrowthAttributes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Combat")
    EAttackType AttackType = EAttackType::Melee;

    // ���� ���� �߰�
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Sockets")
    FName WeaponTipSocketName = FName("WeaponTip");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Sockets")
    FName MuzzleSocketName = FName("Muzzle");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Sockets")
    FName HandSocketName = FName("RightHand");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Sockets")
    FName ChestSocketName = FName("Chest");
};
