// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkerCharacter.h"

#include "EnhancedInputComponent.h"	
#include "EnhancedInputSubsystems.h"

#include "Camera/CameraComponent.h"

#include "AIActor.h"
#include "PathFindingSubsystem.h"

// Sets default values
AWalkerCharacter::AWalkerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->SetupAttachment(RootComponent);

	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetOwnerNoSee(false);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(RootComponent);

	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->bEnableFirstPersonFieldOfView = true;
	CameraComponent->bEnableFirstPersonScale = true;
	CameraComponent->FirstPersonFieldOfView = 65.0f;
	CameraComponent->FirstPersonScale = 0.7f;

	
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;

}

// Called when the game starts or when spawned
void AWalkerCharacter::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAIActor* AI = GetWorld()->SpawnActor<AAIActor>(AAIActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);

	if (!AI)
	{
		UE_LOG(LogTemp, Error, TEXT("AI is nullptr. "));
		return;
	}

	UPathFindingSubsystem* PFSubsystem = GetWorld()->GetSubsystem<UPathFindingSubsystem>();

	if (!PFSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Pathfinding Subsystem is nullptr. AAIActor::BeginPlay "));
		return;
	}

	FVector2D Min = PFSubsystem->MeshMin;
	FVector2D Max = PFSubsystem->MeshMax;

	int32 MaxAgentNum = 32;

	for (int32 i = 1; i < MaxAgentNum; i++)
	{

		AI = GetWorld()->SpawnActor<AAIActor>(AAIActor::StaticClass(), GetRandomVector(Min.X, Max.X, Min.Y, Max.Y, 90.0f, 90.0f), FRotator::ZeroRotator, Params);


	}
	
}

// Called every frame
void AWalkerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWalkerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PC = Cast<APlayerController>(Controller);

	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Controller is Invalid. "));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> (PC->GetLocalPlayer());

	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Subsystem Is Invalid. "));
		return;
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EIC)
	{
		UE_LOG(LogTemp, Error, TEXT("EIC is Invalid. "));
		return;
	}

	Subsystem->AddMappingContext(DefaultMappingContext, 0);
	Subsystem->AddMappingContext(CameraMappingContext, 1);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWalkerCharacter::DoMove);

	EIC->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AWalkerCharacter::DoMouseLook);

	EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &AWalkerCharacter::JumpStart);
	EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &AWalkerCharacter::JumpEnd);

}

void AWalkerCharacter::DoMove(const FInputActionValue& Value)
{
	Move(Value.Get<FVector2D>());
}


void AWalkerCharacter::DoMouseLook(const FInputActionValue& Value)
{
	MouseLook(Value.Get<FVector2D>());
}

void AWalkerCharacter::Move(FVector2D MovementVector)
{
	if (!GetController())
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is Invalid. Couldn't Move. "));
		return;
	}

	AddMovementInput(GetActorForwardVector(), MovementVector.X);
	AddMovementInput(GetActorRightVector(), MovementVector.Y);

}

void AWalkerCharacter::MouseLook(FVector2D LookAxis)
{

	if (!GetController())
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is Invalid. Couldn't Look Around. "));
		return;
	}

	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(-LookAxis.Y);
}

void AWalkerCharacter::JumpStart()
{
	Jump();
}

void AWalkerCharacter::JumpEnd()
{
	StopJumping();
}