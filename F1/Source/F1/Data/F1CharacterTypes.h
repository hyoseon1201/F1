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
 * 캐릭터 클래스 정보 구조체
 * DataTable에서 각 챔피언의 모든 정보를 정의
 */
USTRUCT(BlueprintType)
struct F1_API FCharacterClassInfo : public FTableRowBase
{
    GENERATED_BODY()

    FCharacterClassInfo()
    {
        CharacterName = TEXT("Unknown");
    }

    // 캐릭터 기본 정보
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Info")
    FString CharacterName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Info")
    UTexture2D* CharacterIcon = nullptr;

    // 비주얼 에셋 (메시, 애니메이션)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Assets")
    USkeletalMesh* CharacterMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Assets")
    TSubclassOf<UAnimInstance> AnimBlueprint;

    // 캐릭터 물리 설정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Physics", meta = (ClampMin = "10.0", ClampMax = "200.0"))
    float CapsuleRadius = 42.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Physics", meta = (ClampMin = "30.0", ClampMax = "300.0"))
    float CapsuleHalfHeight = 96.0f;

    // 메시 위치/회전 조정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Transform")
    FVector MeshRelativeLocation = FVector(0.0f, 0.0f, -96.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Transform")
    FRotator MeshRelativeRotation = FRotator(0.0f, -90.0f, 0.0f);

    // GAS 속성 (Base 클래스에서 가져올 것들)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
    TSubclassOf<UGameplayEffect> DefaultAttributes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
    TSubclassOf<UGameplayEffect> GrowthAttributes;
};
