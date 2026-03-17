// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WalkerCharacter.generated.h"

class UCameraComponent;
class UInputAction;

UCLASS()
class CITYWALK_API AWalkerCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction *MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction *JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction *MouseLookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* CameraMappingContext;

public:
	// Sets default values for this character's properties
	AWalkerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Move The Character with AddMovementInput */
	void Move(FVector2D MovementVector);

	/** Looking with mouse*/
	void MouseLook(FVector2D LookAxis);

public:

	UFUNCTION()
	void DoMove(const FInputActionValue& Value);

	UFUNCTION() 
	void DoMouseLook(const FInputActionValue& Value);

	/** Starting the jump */
	UFUNCTION()
	void JumpStart();

	/** Stopping the jump */
	UFUNCTION()
	void JumpEnd();


};
