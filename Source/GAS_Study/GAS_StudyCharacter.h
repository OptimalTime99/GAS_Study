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
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
    {
        return ASC;
    }

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

    UPROPERTY()
    const UCharacterAttributeSet* AttributeSet;
};
