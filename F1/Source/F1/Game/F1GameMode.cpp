// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/F1GameMode.h"
#include "Game/F1PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

AActor* AF1GameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    AF1PlayerState* F1PS = Player->GetPlayerState<AF1PlayerState>();

    // 1. PlayerState가 유효한지 확인
    if (F1PS)
    {
        // 2. [핵심] 팀이 아직 배정되지 않았다면(NoTeam/255), 여기서 바로 배정해버립니다.
        // GetGenericTeamId()가 255(NoTeam)인지 확인
        if (F1PS->GetGenericTeamId().GetId() == FGenericTeamId::NoTeam.GetId())
        {
            PlayerCount++; // 접속자 수 증가

            uint8 NewTeamID = (PlayerCount % 2 != 0) ? 1 : 2;
            F1PS->SetGenericTeamId(FGenericTeamId(NewTeamID));

            UE_LOG(LogTemp, Warning, TEXT("[ChoosePlayerStart] Lazy Assign: Player %s assigned to Team %d"), *Player->GetName(), NewTeamID);
        }
    }

    // 3. 이제 팀 ID는 무조건 존재함. (방금 넣었거나 이미 있었거나)
    uint8 MyTeamID = (F1PS) ? F1PS->GetGenericTeamId().GetId() : 0;
    FName RequiredTag = (MyTeamID == 1) ? FName("Team1") : FName("Team2");

    // 4. 태그에 맞는 PlayerStart 찾기
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* StartSpot = *It;
        if (StartSpot->PlayerStartTag == RequiredTag)
        {
            return StartSpot; // 찾았다!
        }
    }

    UE_LOG(LogTemp, Error, TEXT("   -> NO MATCH! Falling back to default."));
    return Super::ChoosePlayerStart_Implementation(Player);
}