// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"

#include "Enemy.generated.h"

class UAttributeComponent;
class UHealthBarComponent;
class AAIController;

UCLASS()
class SLASHSTUDY_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void DirectionalHitReact(const FVector& ImpactPoint);
	void Die();
	bool IsInTargetRange(AActor* Target, float Radius);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float CombatRadius = 500.f;

	/**
	* AI Navigation
	*/
	UPROPERTY()
	TObjectPtr<AAIController> EnemyController;

	// Current Patrol Target
	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float PatrolRadius = 200.f;

	/**
	* Play Montage Functions
	*/
	void PlayHitReactMontage(const FName& SectionName);

private:

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Montages")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Custom|Sounds")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = "Custom|Visual Effects")
	TObjectPtr<UParticleSystem> HitParticle;

	UPROPERTY()
	TObjectPtr<AActor> CombatTarget;
};
