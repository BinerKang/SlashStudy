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

UCLASS()
class SLASHSTUDY_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void MoveToTarget(AActor* Target, float AcceptanceRadius = 15.f);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	bool IsInTargetRange(AActor* Target, float Radius);
	void GoToChasing();
	void GoToPatrolling();

	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	virtual void Die() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensingComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float CombatRadius = 800.f;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float AttackRadius = 150.f;
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

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float PatrolWaitMinSecond = 5.f;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float PatrolWaitMaxSecond = 10.f;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Custom|AI Navigation")
	float PatrollingSpeed = 125.f;

	FTimerHandle PatrolWaitTimerHandle;
	void PatrolTimerFinished();

	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	UPROPERTY(VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(EditAnywhere, Category = "Custom")
	TSubclassOf<AWeapon> WeaponClass;

private:
	UPROPERTY()
	TObjectPtr<AActor> CombatTarget;

};
