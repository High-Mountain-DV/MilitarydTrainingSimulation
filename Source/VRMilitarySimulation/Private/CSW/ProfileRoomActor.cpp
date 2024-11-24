// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ProfileRoomActor.h"
#include "CSW/VRWidgetActor.h"

void AProfileRoomActor::OnRoomIn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnRoomIn(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	UE_LOG(LogTemp, Warning, TEXT("RoomIn"));
	
	actors.Add(GetWorld()->SpawnActor(InfoFactory, &InfoSpawnPoint));
	actors.Add(GetWorld()->SpawnActor(ReportFactory, &ReportSpawnPoint));
	actors.Add(GetWorld()->SpawnActor(CustomizingFactory, &CustomizingSpawnPoint));
	actors.Add(GetWorld()->SpawnActor(ReplayFactory, &ReplaySpawnPoint));
}

void AProfileRoomActor::OnRoomOut(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnRoomOut(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	for (int i = 0; i < actors.Num(); i++)
		actors[i]->Destroy();
	actors.Empty();
}
