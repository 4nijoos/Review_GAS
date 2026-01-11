// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Cue/GCNA_BurnEffect.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AGCNA_BurnEffect::AGCNA_BurnEffect()
{
	GameplayCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Burn"));
	//bAutoDestroyOnRemove = true;
}

bool AGCNA_BurnEffect::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	//if (MyTarget && BurnVFX)
	//{
	//	SpawnedVFX = UNiagaraFunctionLibrary::SpawnSystemAttached(
	//		BurnVFX, MyTarget->GetRootComponent(), NAME_None,
	//		FVector::ZeroVector, FRotator::ZeroRotator,
	//		EAttachLocation::SnapToTarget, true);
	//	return true;
	//}
	//return false;

	if (MyTarget)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BurnVFX,
			MyTarget->GetActorLocation(),
			MyTarget->GetActorRotation()
		);
		return true;
	}
	return false;
}

bool AGCNA_BurnEffect::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	//if (SpawnedVFX.IsValid())
	//{
	//	SpawnedVFX->Deactivate();
	//	SpawnedVFX = nullptr;
	//}
	return true;
}