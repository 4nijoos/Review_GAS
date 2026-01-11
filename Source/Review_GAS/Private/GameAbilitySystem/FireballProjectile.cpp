// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/FireballProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AFireballProjectile::AFireballProjectile()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFireballProjectile::OnHit);
	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void AFireballProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(3.0f);
}

void AFireballProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Error, TEXT("충돌 발생! 부딪힌 놈: %s"), *OtherActor->GetName());

	if (OtherActor && OtherActor != GetInstigator())
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
		if (TargetASC)
		{
			if (DamageEffectClass)
			{
				FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
				Context.AddHitResult(Hit);
				TargetASC->ApplyGameplayEffectToSelf(DamageEffectClass.GetDefaultObject(), 1.0f, Context);
			}
			if (BurnEffectClass)
			{
				TargetASC->ApplyGameplayEffectToSelf(BurnEffectClass.GetDefaultObject(), 1.0f, TargetASC->MakeEffectContext());
			}
		}
		Destroy();
	}
}

