// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CombatManagerSubsystem.generated.h"

/**
 * 전투 상태 변경 Delegate
 * @param bInCombat 전투 중(true) / 평화(false)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnCombatStateChanged,
    bool, bInCombat
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnCombatTimerUpdated,
    float, RemainingTime
);

/**
 * 
 */
UCLASS()
class GAS_STUDY_API UCombatManagerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    // Subsystem 초기화
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Subsystem 정리
    virtual void Deinitialize() override;

private:
    // 현재 전투 중인지 여부
    bool bIsInCombat = false;

    // 전투 자동 종료 타이머
    FTimerHandle CombatTimeoutHandle;

    // 전투 타임아웃 시간 (기본 5초)
    UPROPERTY(EditDefaultsOnly, Category= "Combat")
    float CombatTimeout = 5.0f;

public:
    /**
     * 현재 전투 상태 확인
     * @return true면 전투 중, false면 평화 상태
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
    bool IsInCombat() const { return bIsInCombat; }

public:
    /** 전투 상태 변경 이벤트 */
    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnCombatStateChanged OnCombatStateChanged;

    /** 전투 타이머 갱신 이벤트 (UI용) */
    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnCombatTimerUpdated OnCombatTimerUpdated;

public:
    /**
     * 전투 시작 / 타이머 갱신
     * * 동작:
     * - 평화 상태: 전투 시작 + 타이머 5초 시작
     * - 전투 중: 타이머만 5초로 갱신 (전투 연장)
     * * 호출 시점:
     * - GA_ComboAttackBase::ActivateAbility()에서 호출
     */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartCombat();

    /**
     * 전투 종료
     * 
     * 호출 시점:
     * - 5초 타이머 만료 시 자동 호출
     * - 모든 적 처치 시 수동 호출 가능
     */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void EndCombat();
};
