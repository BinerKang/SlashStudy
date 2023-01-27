// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"

#include "Enemy.generated.h"

class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;
class AWeapon;
class ASoul;

UCLASS()
class SLASHSTUDY_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */
	
	/** <APawn> */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/** </APawn> */

	/** <IHitInterface> */
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

protected:

	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float CombatRadius = 800.f;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float AttackRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float AcceptanceRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = "Custom")
	float DeadLifeSpan = 8.f;

	// Current Patrol Target
	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float PatrolWaitMinSecond = 5.f;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float PatrolWaitMaxSecond = 10.f;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = "Custom")
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Custom")
	TSubclassOf<ASoul> SoulClass;

	UPROPERTY(EditAnywhere, Category = "Custom|Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Custom|Combat")
	float AttackMax = 1.f;

	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die() override;
	void SpawnSoulActor();
	virtual void Attack(const FInputActionValue& Value) override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;
	/** </ABaseCharacter> */

private:
	/** AI Behavior */
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void MoveToTarget(AActor* Target);
	bool IsInTargetRange(AActor* Target, float Radius);
	void StartAttackTimer();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void GoToChasing();
	void GoToPatrolling();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsEngaged();
	void ClearPatrolWaitTimer();
	void ClearAttackTimer();
	void Attack();

	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	FTimerHandle PatrolWaitTimerHandle;
	FTimerHandle AttackTimer;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensingComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;

	UPROPERTY()
	TObjectPtr<AAIController> EnemyController;
	
};
