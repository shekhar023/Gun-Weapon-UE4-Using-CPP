// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class ASWeapon;
class USoundCue;
class UAnimMontage;
class UAudioComponent;
class AActor;

UCLASS()
class NOMADCPP_API ASWeapon : public AActor
{
	GENERATED_BODY()
    
public:	
	// Sets default values for this actor's properties
	ASWeapon();
    
    UPROPERTY()
    TEnumAsByte<EPhysicalSurface> SurfaceType;

protected:

	virtual void BeginPlay() override;
    
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon")
    float BaseDamage;
    
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon")
    int32 AmmoDiffrence; //Spare Ammo
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Weapon", meta = (ClampMin=0.0f))
    int32 ClipSize; //Loaded Ammo
       
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Weapon", meta = (ClampMin=0.0f))
    int32 MaxAmmo; //Max Loaded Ammo
       
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Weapon", meta = (ClampMin=0.0f))
    int32 AmmoInClip; //Max Loaded Ammo
    
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon")
    float RateOfFire;
    
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon", meta = (ClampMin=0.0f))
    float BulletSpread;
    
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon")
    float WeaponRange;
    
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName TracerTargetName;
    
    FTimerHandle TimerHandle_TimeBetweenShots;

    float LastFireTime;
    
    // Derived from RateOfFire
    float TimeBetweenShots;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* MeshComp;
   
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName MuzzleSocketName;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* MuzzleEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* DefaultImpactEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* FleshImpactEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* TracerEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<UCameraShake> FireCamShake;
   
    void Fire();
    
    void PlayFireEffects(FVector TraceEnd);

    void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);
    
    
    
   
    /** single fire sound (bLoopedFireSound not set) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Sound")
    USoundCue* FireSound;
    
    /** finished burst sound (bLoopedFireSound set) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Sound")
    USoundCue* FireFinishSound;

    /** out of ammo sound */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Sound")
    USoundCue* OutOfAmmoSound;

    /** reload sound */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Sound")
    USoundCue* ReloadSound;

    /** reload animations */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    UAnimMontage* ReloadAnim;

    /** equip sound */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Sound")
    USoundCue* EquipSound;

    /** equip animations */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    UAnimMontage* EquipAnim;

    /** fire animations */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    UAnimMontage* FireAnim;
    
public:

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void StartFire();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void StopFire();
    
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Reload();
    
    //To play sound effects in blueprints
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void PlaySoundEffects(USoundCue* Sound);
    
};
