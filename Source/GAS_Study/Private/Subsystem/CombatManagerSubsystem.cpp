// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/CombatManagerSubsystem.h"

void UCombatManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Warning, TEXT("UCombatManagerSubsystem Initialize"));
}

void UCombatManagerSubsystem::Deinitialize()
{
    Super::Deinitialize();
    UE_LOG(LogTemp, Warning, TEXT("UCombatManagerSubsystem Deinitialize"));
}

void UCombatManagerSubsystem::StartCombat()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 이미 전투 중이면 타이머만 갱신
	if (bIsInCombat)
	{
		// 타이머 리셋 (5초 연장)
		World->GetTimerManager().SetTimer(
			CombatTimeoutHandle,
			this,
			&UCombatManagerSubsystem::EndCombat,
			CombatTimeout,
			false
		);

		// UI에 타이머 갱신 알림
		OnCombatTimerUpdated.Broadcast(CombatTimeout);

		UE_LOG(LogTemp, Log, TEXT("[CombatManager] Combat Timer Reset (%.1fs)"), CombatTimeout);
		return;
	}

	// 전투 시작
	bIsInCombat = true;

	// 자동 종료 타이머 시작
	World->GetTimerManager().SetTimer(
		CombatTimeoutHandle,
		this,
		&UCombatManagerSubsystem::EndCombat,
		CombatTimeout,
		false // 반복 안 함
	);

	// 이벤트 브로드캐스트
	OnCombatStateChanged.Broadcast(true);
	OnCombatTimerUpdated.Broadcast(CombatTimeout); // UI에 초기 타이머 알림

	UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Combat Started (Timeout: %.1fs)"), CombatTimeout);
}


void UCombatManagerSubsystem::EndCombat()
{
	// 평화 상태면 무시
	if (!bIsInCombat)
	{
		return;
	}

	// 타이머 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CombatTimeoutHandle);
	}

	// 평화 상태로 변경
	bIsInCombat = false;

	// 이벤트 브로드캐스트
	OnCombatStateChanged.Broadcast(false);

	UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Combat Ended"));
}