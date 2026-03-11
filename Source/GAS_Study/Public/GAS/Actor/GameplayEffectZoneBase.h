// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayEffectZoneBase.generated.h"

class UBoxComponent;
class UGameplayEffect;

UCLASS()
class GAS_STUDY_API AGameplayEffectZoneBase : public AActor
{
    GENERATED_BODY()

public:
    AGameplayEffectZoneBase();

protected:
    virtual void BeginPlay() override;

    // 영역 설정을 위한 박스 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* ZoneBox;

    // 블루프린트에서 할당할 Gameplay Effect 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|GameplayEffects")
    TSubclassOf<UGameplayEffect> GameplayEffectClass;

    // Overlap 이벤트 바인딩용 함수
    UFUNCTION()
    virtual void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    virtual void OnOverlapEnd(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);

private:
    // 지속형(Infinite) 이펙트가 적용된 대상을 추적하고 이탈 시 해제하기 위한 저장소
    TMap<AActor*, FActiveGameplayEffectHandle> ActiveEffectHandles;
};
