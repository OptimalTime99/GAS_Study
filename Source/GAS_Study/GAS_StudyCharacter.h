// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "GAS_StudyCharacter.generated.h"

class UCharacterAttributeSet;
class UGameplayAbility;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AGAS_StudyCharacter : public ACharacter, public IAbilitySystemInterface
{
public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
    GENERATED_BODY()

    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

protected:
    /** Jump Input Action */
    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* JumpAction;

    /** Move Input Action */
    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* MoveAction;

    /** Look Input Action */
    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* LookAction;

    /** Mouse Look Input Action */
    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* MouseLookAction;

    /** Attack Input Action */
    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* LightAttackAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* MeleeAttackAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* HeavyAttackAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* ComboAttackAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* DefenseAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* RollAction;

public:
    /** Constructor */
    AGAS_StudyCharacter();

protected:
    /** Initialize input action bindings */
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    /** Called for movement input */
    void Move(const FInputActionValue& Value);

    /** Called for looking input */
    void Look(const FInputActionValue& Value);

public:
    /** Handles move inputs from either controls or UI interfaces */
    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoMove(float Right, float Forward);

    /** Handles look inputs from either controls or UI interfaces */
    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoLook(float Yaw, float Pitch);

    /** Handles jump pressed inputs from either controls or UI interfaces */
    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoJumpStart();

    /** Handles jump pressed inputs from either controls or UI interfaces */
    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoJumpEnd();

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoLightAttack();

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoMeleeAttack();

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoHeavyAttack();

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoComboAttack();

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoDefenseStart();

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoDefenseEnd();

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoRollStart();

public:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Abilities")
    TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

private:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "GAS|Abilities", meta=(AllowPrivateAccess = "true"))
    UAbilitySystemComponent* ASC;

    void GiveDefaultAbilities();

    // 컨트롤러가 캐릭터에 빙의될 때 호출되는 엔진 함수 오버라이드
    virtual void PossessedBy(AController* NewController) override;

    virtual void OnRep_PlayerState() override;

    // ASC 초기화 (PlayerState에서 가져와서 InitAbilityActorInfo 호출)
    virtual void InitializeAbilitySystem();

    UPROPERTY()
    TObjectPtr<UCharacterAttributeSet> AttributeSet;

    // AttributeSet 접근자 (PlayerState에서 가져옴)
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    UCharacterAttributeSet* GetAttributeSet() const;

public:
    // UI 위젯을 등록하기 위한 함수 (블루프린트 BeginPlay 등에서 설정)
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetMainHUDWidget(UUserWidget* InWidget);

    // 실제 체력을 업데이트하는 함수
    void UpdatePlayerHUD_EnemyHP(float CurrentHP, float MaxHP);

public:
    // 블루프린트(BP_PlayerCharacter)의 BeginPlay에서 생성된 WBP_Player를 여기 넣어줄 겁니다.
    UPROPERTY(BlueprintReadWrite, Category = "UI")
    TObjectPtr<UUserWidget> MainHUDWidget;

    // GA에서 호출할 UI 업데이트 함수
    void UpdateEnemyHPOnHUD(float CurrentHP, float MaxHP);
    
protected:
    virtual void BeginPlay() override;

    /** 에디터에서 생성할 WBP_Player 블루프린트 클래스를 선택할 수 있도록 노출 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> PlayerHUDClass;
};
