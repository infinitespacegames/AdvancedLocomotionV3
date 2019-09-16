// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include <Components/ActorComponent.h>
#include "HealthComponent.generated.h"

/** Delegate for player health change */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FOnHealthChangedSignature, class UHealthComponent*, HealthComponent, float, Health, float, Shield, float, Damage, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);


/** 
  Implements player health component for tracking shields, health, and damage
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPBASE_API UHealthComponent : public UActorComponent {

	GENERATED_BODY()

public:

	/** Base constructor */
	UHealthComponent();

	/** Called every frame */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	/** Called when the game starts */
	virtual void BeginPlay() override;

	/** Play desired effects upon taking damage */
	virtual void PlayDamageEffects();

	/** Tracks and processes actor damage */
	UFUNCTION()
	void HandleDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/** Replicates properties on client */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;


	/** Player maximum health */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HealthComponent")
	float MaxHealth;

	/** Player maximum shield */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HealthComponent")
	float MaxShield;

	/** Player maximum health */
	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent")
	float CurrentHealth;

	/** Player maximum shield */
	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent")
	float CurrentShield;

	/** Flag to spawn with shields */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HealthComponent")
	bool bSpawnWithShields;

	/** Player current health */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "HealthComponent")
	float Health;

	/** Player current shield */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "HealthComponent")
	float Shield;

	/** Sound effect for taking damage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent|FX")
	USoundBase* DamageSFX = nullptr;

private:

	/** Animation speed for HUD */
	float dHdT = 100.0f;

public:

	/** Health changed delegate*/
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	/* Inline accessors */
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetHealthPct() const { return Health / MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE float GetShieldPct() const { return Shield / MaxShield; }
};

