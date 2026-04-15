// MyPlayerState.h
#pragma once

#include"CoreMinimal.h"
#include"GameFramework/PlayerState.h"
#include"AbilitySystemInterface.h"
#include"MyPlayerState.generated.h"

class UAbilitySystemComponent;
class UCharacterAttributeSet;

/**
 * AMyPlayerState
 *
 * Player의 ASC를 소유하는 PlayerState입니다.
 *
 * 역할:
 * - ASC와 AttributeSet 소유
 * - IAbilitySystemInterface 구현
 * - 캐릭터가 죽어도 데이터 유지
 */
UCLASS()
class GAS_STUDY_API AMyPlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AMyPlayerState();

    //~ IAbilitySystemInterface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    // AttributeSet 접근
    UCharacterAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
    // ASC - PlayerState가 소유
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    // AttributeSet - PlayerState가 소유
    UPROPERTY()
    TObjectPtr<UCharacterAttributeSet> AttributeSet;
};