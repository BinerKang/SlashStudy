// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	// Add SpringArm And Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	
	// Camera Control
	CameraBoom->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	//Groom Hair Eyebrows
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");
	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashMappingContext, 0);
		}
	}
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), MovementVector.Y);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), MovementVector.X);
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void ASlashCharacter::DoJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		ACharacter::Jump();
	}
}

void ASlashCharacter::EKeyPressed(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		if (TObjectPtr<AWeapon> OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
		{
			OverlappingWeapon->Equip(this->GetMesh(), FName(RIGHT_HAND_SOCKET));
			this->CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			this->OverlappingItem = nullptr;
			this->EquippedWeapon = OverlappingWeapon;
		}
		else
		{
			if (CanDisarm())
			{
				this->PlayEquipMontage(FName("Unequip"));
				this->CharacterState = ECharacterState::ECS_Unequipped;
				this->ActionState = EActionState::EAS_Equipping;
			}
			else if (CanArm())
			{
				this->PlayEquipMontage(FName("Equip"));
				this->CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
				this->ActionState = EActionState::EAS_Equipping;
			}
		}
	}
}

void ASlashCharacter::Attack(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		if (CanAttack())
		{
			ActionState = EActionState::EAS_Attacking;
			PlayAttackMontage();
		}
	}
}

void ASlashCharacter::PlayAttackMontage()
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		FName SectionName;
		const int32 i = FMath::RandRange(1, 2);
		switch (i)
		{
		case 1: SectionName = FName("Attack1"); break;
		case 2: SectionName = FName("Attack2"); break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		this->ActionState = EActionState::EAS_Equipping;
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void ASlashCharacter::Arm()
{
	if (this->EquippedWeapon)
	{
		this->EquippedWeapon->AttachMeshToSocket(this->GetMesh(), FName(RIGHT_HAND_SOCKET));
	}
}

void ASlashCharacter::Disarm()
{
	if (this->EquippedWeapon)
	{
		this->EquippedWeapon->AttachMeshToSocket(this->GetMesh(), FName(SPINE_SOCKET));
	}
}

void ASlashCharacter::FinishEquipping()
{
	this->ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComp->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::DoJump);
		EnhancedInputComp->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComp->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
	}
}

