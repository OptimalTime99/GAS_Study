// EnemyCharacterBase.h
#pragma once

#include"CoreMinimal.h"
#include"GameFramework/Character.h"
#include"AbilitySystemInterface.h"
#include"EnemyCharacterBase.generated.h"

class UAbilitySystemComponent;
class UEnemyAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
struct FOnAttributeChangeData;

/**
 * AEnemyCharacterBase
 *
 * 적 캐릭터의 베이스 클래스입니다.
 * ASC를 Character가 직접 소유합니다 (Player와 다름).
 *
 * Player와의 차이:
 * - ASC 위치: Character (Player는 PlayerState)
 * - 초기화: BeginPlay (Player는 PossessedBy)
 * - 사망 시: Ragdoll → Timer → Destroy
 */

UCLASS()
class GAS_STUDY_API AEnemyCharacterBase : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AEnemyCharacterBase();

    //~ IAbilitySystemInterface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    /** AttributeSet Getter */
    UEnemyAttributeSet* GetAttributeSet() const { return AttributeSet; }

    /** Health 접근자 */
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    float GetMaxHealth() const;

protected:
    virtual void BeginPlay() override;

    // ===== GAS Components =====

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UEnemyAttributeSet> AttributeSet;

    // ===== 초기 설정 =====

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

    // ===== 사망 처리 =====

    /** 사망 후 제거까지 대기 시간 */
    UPROPERTY(EditDefaultsOnly, Category = "Death")
    float DeathLifeSpan = 5.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Death")
    bool bIsDead = false;

    // ===== 내부 함수 =====

    virtual void InitializeAbilitySystem();
    void GiveDefaultAbilities();
    void ApplyDefaultEffects();

    /** Health 변경 콜백 (ASC 델리게이트) */
    void OnHealthChanged(const FOnAttributeChangeData& Data);

    /** 사망 처리: Ragdoll → Timer → Destroy */
    void HandleDeath();
};
