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

    // GAS �Ӽ� (Base Ŭ�������� ������ �͵�)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
    TSubclassOf<UGameplayEffect> DefaultAttributes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
    TSubclassOf<UGameplayEffect> GrowthAttributes;
};
