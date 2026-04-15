// MyPlayerState.cpp
#include "Character/MyPlayerState.h"

#include"AbilitySystemComponent.h"
#include "GAS/Attributes/CharacterAttributeSet.h"

AMyPlayerState::AMyPlayerState()
{
    // ASC 생성 (PlayerState가 소유)
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(
        TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);

    // Full 모드: Player는 모든 GE를 복제 (중요!)
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

    // AttributeSet 생성 (PlayerState가 소유)
    AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));

    // PlayerState의 NetUpdateFrequency는 기본적으로 높음
		SetNetUpdateFrequency(100.0f);
}

UAbilitySystemComponent* AMyPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}