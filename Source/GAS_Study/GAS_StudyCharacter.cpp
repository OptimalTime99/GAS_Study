// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS_StudyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "GAS_Study.h"
#include "GAS/GAS_StudyTags.h"
#include "GAS/Attributes/CharacterAttributeSet.h"

AGAS_StudyCharacter::AGAS_StudyCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 500.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

    AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("Attribute Set"));
}

void AGAS_StudyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGAS_StudyCharacter::DoJumpStart);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGAS_StudyCharacter::DoJumpEnd);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGAS_StudyCharacter::Move);
        EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AGAS_StudyCharacter::Look);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGAS_StudyCharacter::Look);

        // Attacking
        EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this,
                                           &AGAS_StudyCharacter::DoLightAttack);
        EnhancedInputComponent->BindAction(MeleeAttackAction, ETriggerEvent::Started, this,
                                           &AGAS_StudyCharacter::DoMeleeAttack);
        EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this,
                                           &AGAS_StudyCharacter::DoHeavyAttack);
    }
    else
    {
        UE_LOG(LogGAS_Study, Error,
               TEXT(
                   "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
               ), *GetNameSafe(this));
    }
}

void AGAS_StudyCharacter::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

    // route the input
    DoMove(MovementVector.X, MovementVector.Y);
}

void AGAS_StudyCharacter::Look(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    // route the input
    DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AGAS_StudyCharacter::DoMove(float Right, float Forward)
{
    if (GetController() != nullptr)
    {
        // find out which way is forward
        const FRotator Rotation = GetController()->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // get right vector 
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement 
        AddMovementInput(ForwardDirection, Forward);
        AddMovementInput(RightDirection, Right);
    }
}

void AGAS_StudyCharacter::DoLook(float Yaw, float Pitch)
{
    if (GetController() != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(Yaw);
        AddControllerPitchInput(Pitch);
    }
}

void AGAS_StudyCharacter::DoJumpStart()
{
    if (ASC)
    {
        // 찾고자 하는 태그를 컨테이너에 담습니다.
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(GAS_StudyTags::Ability_Action_Jump);

        // 해당 태그를 가진 어빌리티(GA_Jump)의 실행을 시도합니다.
        // bAllowRemoteActivation 매개변수는 기본값(true)을 사용하여 서버에도 실행을 요청합니다.
        ASC->TryActivateAbilitiesByTag(TagContainer);
    }
}

void AGAS_StudyCharacter::DoJumpEnd()
{
    // signal the character to stop jumping
    StopJumping();
}

void AGAS_StudyCharacter::DoLightAttack()
{
    if (ASC)
    {
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(GAS_StudyTags::Ability_Action_LightAttack);

        ASC->TryActivateAbilitiesByTag(TagContainer);
    }
}

void AGAS_StudyCharacter::DoMeleeAttack()
{
    if (ASC)
    {
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(GAS_StudyTags::Ability_Action_MeleeAttack);

        ASC->TryActivateAbilitiesByTag(TagContainer);
    }
}

void AGAS_StudyCharacter::DoHeavyAttack()
{
    if (ASC)
    {
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(GAS_StudyTags::Ability_Action_HeavyAttack);

        ASC->TryActivateAbilitiesByTag(TagContainer);
    }
}

void AGAS_StudyCharacter::GiveDefaultAbilities()
{
    // 서버(Authority)에서만, 그리고 ASC가 유효할 때만 실행합니다.
    if (!HasAuthority() || !ASC) return;

    // 🌟 핵심: 배열에 들어있는 모든 어빌리티를 꺼내서 자동으로 부여합니다.
    for (const TSubclassOf<UGameplayAbility>& StartupAbility : DefaultAbilities)
    {
        // 빈 슬롯(None)이 들어있을 수 있으므로 유효성 검사는 필수입니다.
        if (StartupAbility)
        {
            FGameplayAbilitySpec AbilitySpec(StartupAbility, 1, INDEX_NONE, this);
            ASC->GiveAbility(AbilitySpec);
        }
    }
}

void AGAS_StudyCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (ASC)
    {
        // 1. ASC에게 Owner와 Avatar가 이 캐릭터(this)임을 알려주어 초기화합니다. (매우 중요!)
        ASC->InitAbilityActorInfo(this, this);

        // 2. 어빌리티 부여 함수 호출
        GiveDefaultAbilities();
    }
}
