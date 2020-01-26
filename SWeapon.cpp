// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/ArrowComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "NomadCPP.h"
#include "TimerManager.h"

// Sets default values
ASWeapon::ASWeapon()
{
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
    
    MuzzleSocketName = "MuzzleSocket";
    TracerTargetName = "Target";
    
    BaseDamage = 20.0f;
    BulletSpread = 2.0f;
    RateOfFire = 600;
    WeaponRange = 2000.0f;
    
    ClipSize = 30;
    MaxAmmo = 300;
    AmmoInClip = ClipSize;

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
    
    TimeBetweenShots = 60 / RateOfFire;
	
}

void ASWeapon::Fire()
{
    // Trace the world, from pawn eyes to crosshair location
    AActor* MyOwner = GetOwner();
    if (MyOwner)
    {
        FVector EyeLocation;
        FRotator EyeRotation;
        MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
        
    
        FVector ShotDirection = EyeRotation.Vector();

        // Bullet Spread
        float HalfRad = FMath::DegreesToRadians(BulletSpread);
        ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

        FVector TraceEnd = EyeLocation + (ShotDirection * WeaponRange);

        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(MyOwner);
        QueryParams.AddIgnoredActor(this);
        QueryParams.bTraceComplex = true;
        QueryParams.bReturnPhysicalMaterial = true;

        // Particle "Target" parameter
        FVector TracerEndPoint = TraceEnd;

        SurfaceType = SurfaceType_Default;
        
        if(AmmoInClip > 0)
        {
            AmmoInClip -= 1;
            
        FHitResult Hit;
        if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
        {
            // Blocking hit! Process damage
            AActor* HitActor = Hit.GetActor();

            SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
            
            float ActualDamage = BaseDamage;
            if (SurfaceType == SURFACE_FLESHVULNERABLE)
            {
                ActualDamage *= 4.0f;
            }

            UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

            PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

            TracerEndPoint = Hit.ImpactPoint;

        }

        PlayFireEffects(TracerEndPoint);

        LastFireTime = GetWorld()->TimeSeconds;
            
            if(FireSound)
            {
                PlaySoundEffects(FireSound);
            }
        }
    }
    
    else
    {
        Reload();
    }
}

void ASWeapon::PlayFireEffects(FVector TraceEnd)
{
    
    if (MuzzleEffect)
    {
        UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
    }

    if (TracerEffect)
    {
        FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

        UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
        if (TracerComp)
        {
            TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
        }
    }

    APawn* MyOwner = Cast<APawn>(GetOwner());
    if (MyOwner)
    {
        APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
        if (PC)
        {
            PC->ClientPlayCameraShake(FireCamShake);
        }
    }
}

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
    UParticleSystem* SelectedEffect = nullptr;
    switch (SurfaceType)
    {
    case SURFACE_FLESHDEFAULT:
    case SURFACE_FLESHVULNERABLE:
        SelectedEffect = FleshImpactEffect;
        break;
    default:
        SelectedEffect = DefaultImpactEffect;
        break;
    }

    if (SelectedEffect)
    {
        FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

        FVector ShotDirection = ImpactPoint - MuzzleLocation;
        ShotDirection.Normalize();

        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
    }
}


void ASWeapon::StartFire()
{
    float FirstDelay = FMath::Max(LastFireTime  + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

    GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
   
    GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::Reload()
{
    AmmoDiffrence = ClipSize - AmmoInClip;
    
    if(MaxAmmo == 0 && AmmoInClip == 0)
    {
        if(OutOfAmmoSound)
        {
            PlaySoundEffects(OutOfAmmoSound);
            return;
        }
    }
    
    if( AmmoInClip >= 0 && AmmoInClip < ClipSize)
    {
        if( MaxAmmo <= AmmoDiffrence)
        {
            AmmoInClip += MaxAmmo;
            MaxAmmo -= AmmoInClip;
        }
        else
        {
            AmmoInClip += AmmoDiffrence;
            MaxAmmo -= AmmoDiffrence;
        }
        
        if(ReloadSound)
        {
            PlaySoundEffects(ReloadSound);
        }
        
    }
}
    

   
