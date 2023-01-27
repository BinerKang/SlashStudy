// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "BaseCharacter.h"
#include "Interfaces/PickupInterface.h"

#include "SlashCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class ASoul;
class USlashOverlay;

UCLASS()
class SLASHSTUDY_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(int32 SoulCount) override;
	virtual void AddGold(int32 GoldCount) override;
	
	FORCEINLINE ECharacterState GetCharacterState() const { return this->CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return this->ActionState; }

protected:
	virtual void BeginPlay() override;
	void InitializeSlashOverlay(APlayerController* PlayerController);

	virtual void Die() override;

	UPROPERTY(EditAnywhere, Category = "Custom|Input")
	TObjectPtr<UInputMappingContext> SlashMappingContext;

	UPROPERTY(EditAnywhere, Category = "Custom|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Custom|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Custom|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Custom|Input")
	TObjectPtr<UInputAction> EKeyAction;

	UPROPERTY(EditAnywhere, Category = "Custom|Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, Category = "Custom|Input")
	TObjectPtr<UInputAction> DodgeAction;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void SetHUDHealth();

	/** 
	* Callback for Input
	*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void DoJump(const FInputActionValue& Value);
	void EKeyPressed(const FInputActionValue& Value);
	virtual void Attack(const FInputActionValue& Value) override;
	void Dodge(const FInputActionValue& Value);

	/**
	* Play Montage Functions
	*/
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;

	void PlayEquipMontage(const FName& SectionName);
	bool CanDisarm();
	bool CanArm();
	bool IsUnoccupied();

	UFUNCTION(BlueprintCallable)
	void Arm();
	UFUNCTION(BlueprintCallable)
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactionEnd();

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGroomComponent> Hair;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGroomComponent> Eyebrows;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> HairMesh;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappingItem;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Montages")
	TObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY()
	TObjectPtr<USlashOverlay> SlashOverlay;

	void DisableMeshCollision();
};

