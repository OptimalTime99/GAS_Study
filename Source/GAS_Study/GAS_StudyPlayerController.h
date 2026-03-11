// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GAS_StudyPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AGAS_StudyPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    /** Input Mapping Contexts */
    UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
    TArray<UInputMappingContext*> DefaultMappingContexts;

    /** Input Mapping Contexts */
    UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
    TArray<UInputMappingContext*> MobileExcludedMappingContexts;

    /** Mobile controls widget to spawn */
    UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
    TSubclassOf<UUserWidget> MobileControlsWidgetClass;

    /** Pointer to the mobile controls widget */
    TObjectPtr<UUserWidget> MobileControlsWidget;

    /** Gameplay initialization */
    virtual void BeginPlay() override;

    /** Input mapping context setup */
    virtual void SetupInputComponent() override;

protected:
    // 블루프린트에서 WBP_Player를 할당할 도면 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UPlayerHUDWidget> HUDWidgetClass;

    // 화면에 띄워둔 실제 UI 포인터
    UPROPERTY()
    UPlayerHUDWidget* ActiveHUD;

    // 클라이언트(멀티플레이) 측에서 빙의를 인지했을 때 호출됨
    virtual void AcknowledgePossession(APawn* P) override;

private:
    // 빙의할 때마다 UI에 새로운 ASC를 꽂아주는 공통 처리 함수
    void UpdateHUD(APawn* InPawn);
};
