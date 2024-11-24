// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/CustomizingButton.h"

#include "Components/Button.h"
#include "CSW/CustomModel.h"
#include "Kismet/GameplayStatics.h"

void UCustomizingButton::NativeConstruct()
{
	Super::NativeConstruct();

	FButtonStyle style = Button->GetStyle();
	
	FSlateBrush brush;
	brush.SetResourceObject(Texture);
	brush.SetImageSize(FVector2D(Texture->GetSizeX(), Texture->GetSizeY()));

	style.SetDisabled(brush);
	style.SetNormal(brush);
	style.SetHovered(brush);
	style.SetPressed(brush);

	Button->SetStyle(style);

	Button->OnClicked.AddDynamic(this, &UCustomizingButton::OnClickButton);
}

void UCustomizingButton::OnClickButton()
{
	auto* model = Cast<ACustomModel>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomModel::StaticClass()));
	auto* gi = Cast<UCSWGameInstance>(GetGameInstance());
	if (model)
	{
		model->SetCustomizingPath(Path, Category);
		gi->SetCustomizingPath(Path, Category);
	}
}
