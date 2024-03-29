// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"

#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;

UCLASS()
class SLASHSTUDY_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	FORCEINLINE EDeathPose GetDeathPose() const { return this->DeathPose; }

protected:
	virtual void BeginPlay() override;

	virtual void Attack(const FInputActionValue& Value);

	virtual void Die();

	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) override;

	bool IsAlive();

	void PlayHitReactMontage(const FName& SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticle(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	
	virtual bool CanAttack();

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	/**
	* Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Montages")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Montages")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Montages")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Montages")
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Montages")
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> CombatTarget;

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarp();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarp();

	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayMontageRandomSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	void PlayAttackMontage();
	void StopAttackMontage();
	void PlayDodgeMontage();
	virtual void PlayDeathMontage();

	UPROPERTY(EditAnywhere, Category = "Custom|Sounds")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = "Custom|Visual Effects")
	TObjectPtr<UParticleSystem> HitParticle;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY(EditAnywhere, Category = "Custom|Combat")
	double WarpTargetDistance = 75.f;

	UPROPERTY(EditAnywhere, Category = "Custom")
	bool bNeedPhysicalWhenDie = false;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;
};