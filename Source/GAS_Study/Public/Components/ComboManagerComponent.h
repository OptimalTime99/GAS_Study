// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/Data/ComboData.h"
#include "Components/ActorComponent.h"
#include "ComboManagerComponent.generated.h"


class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAS_STUDY_API UComboManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UComboManagerComponent();

    // 공격 데이터 배열 (3개 설정)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
    TArray<FComboData> AttackList;

    // 콤보 리셋 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
    float ComboResetTime = 2.0f;

    // 콤보 윈도우 태그 (Combat.Window.Combo)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
    FGameplayTag ComboWindowTag;

    // 공격 중 태그 (State.Attacking) - 공격 상태 추적용
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
    FGameplayTag AttackingTag;

    // 공격 요청 (입력 시 호출)
    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool RequestAttack();

    // 콤보 리셋
    UFUNCTION(BlueprintCallable, Category = "Combo")
    void ResetCombo();

    UFUNCTION(BlueprintCallable, Category = "Combo")
    int32 GetCurrentComboIndex() const { return CurrentComboIndex; }

    UFUNCTION(BlueprintCallable, Category = "Combo")
    int32 GetMaxComboIndex() const;

    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool IsAttacking() const { return bIsAttacking; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    int32 CurrentComboIndex = 0;
    bool bComboWindowOpen = false;
    bool bIsAttacking = false; // ★ 공격 중 여부
    bool bPendingCombo = false; // ★ 버퍼링된 입력 있음

    FTimerHandle ComboResetTimerHandle;
    TObjectPtr<UAbilitySystemComponent> CachedASC;
    FDelegateHandle ComboWindowTagHandle;
    FDelegateHandle AttackingTagHandle;

    void HandleComboWindowTagChanged(const FGameplayTag Tag, int32 NewCount);
    void HandleAttackingTagChanged(const FGameplayTag Tag, int32 NewCount);
    FComboData* GetAttackByIndex(int32 Index);
    bool ActivateAbilityByTag(const FGameplayTag& Tag);
    bool ExecuteCombo(); // ★ 실제 콤보 실행
    void AdvanceCombo(); // ★ 다음 콤보로 진행
    void StartResetTimer();
};
