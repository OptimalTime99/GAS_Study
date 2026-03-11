// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_ApplyDamage.generated.h"

/**
 * 
 */
UCLASS()
class GAS_STUDY_API UAN_ApplyDamage : public UAnimNotify
{
    GENERATED_BODY()

public:
    UAN_ApplyDamage();

    virtual void Notify(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference) override;

    /** Sphere Trace 반경 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float TraceRadius;

    /** Trace 시작 거리 (캐릭터 전방) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float TraceStartDistance;

    /** Trace 종료 거리 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float TraceEndDistance;

    /** 적용할 Damage Effect 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    /** 디버그 드로잉 표시 여부 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bShowDebug;
};
