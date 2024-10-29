// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_EnemyAnimInstance.h"
#include "../SG_Enemy.h"
#include "SG_EnemyMagazine.h"

void USG_EnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Me = Cast<ASG_Enemy>(TryGetPawnOwner());
	check(Me);	if (nullptr == Me) return;
}

void USG_EnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	
}

void USG_EnemyAnimInstance::PlayMontage(UAnimMontage* MontageToPlay, float PlayRate /*= 1.0f*/)
{
	Montage_Play(MontageToPlay, PlayRate);
}

void USG_EnemyAnimInstance::AnimNotify_DettachMagazine()
{
	check(MagazineFactory); if (nullptr == MagazineFactory) return;

	GrabMagazine = GetWorld()->SpawnActor<ASG_EnemyMagazine>(MagazineFactory);

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

	GrabMagazine->AttachToComponent(Me->GetMesh(), rules, TEXT("MagazineSocket"));
	Me->HideWeaponMagazine();
}

void USG_EnemyAnimInstance::AnimNotify_ThrowMagazine()
{
	check(GrabMagazine); if (nullptr == GrabMagazine) return;

	GrabMagazine->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	GrabMagazine->MagazineMesh->SetSimulatePhysics(true);

	FTimerHandle destroyHandle;
	GetWorld()->GetTimerManager().SetTimer(destroyHandle, [&]()
		{
			if (GrabMagazine) GrabMagazine->Destroy();
		}, 5.0f, false);

}

void USG_EnemyAnimInstance::AnimNotify_GrabNewMagazine()
{
	check(MagazineFactory); if (nullptr == MagazineFactory) return;

	GrabMagazine = GetWorld()->SpawnActor<ASG_EnemyMagazine>(MagazineFactory);
	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	GrabMagazine->AttachToComponent(Me->GetMesh(), rules, TEXT("MagazineSocket"));
}

void USG_EnemyAnimInstance::AnimNotify_CoupleMagazine()
{
	check(GrabMagazine); if (nullptr == GrabMagazine) return;

	GrabMagazine->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	GrabMagazine->Destroy();
	Me->ShowWeaponMagazine();
}
