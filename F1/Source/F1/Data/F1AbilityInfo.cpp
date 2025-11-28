// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/F1AbilityInfo.h"

FF1AbilityInfo UF1AbilityData::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	// 배열을 순회하며 태그가 일치하는지 찾습니다.
	for (const FF1AbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag.MatchesTagExact(AbilityTag))
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find info for AbilityTag [%s] on AbilityInfo [%s]"),
			*AbilityTag.ToString(), *GetNameSafe(this));
	}

	return FF1AbilityInfo(); // 없으면 빈 깡통 반환
}