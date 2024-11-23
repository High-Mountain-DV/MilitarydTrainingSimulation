// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ProfileCustomizing.h"

#include "Components/CheckBox.h"
#include "Components/WidgetSwitcher.h"

void UProfileCustomizing::NativeConstruct()
{
	Super::NativeConstruct();

	CheckBoxMark->OnCheckStateChanged.AddDynamic(this, &UProfileCustomizing::OnCheckMark);
	CheckBoxAcc->OnCheckStateChanged.AddDynamic(this, &UProfileCustomizing::OnCheckAcc);
	CheckBoxCream->OnCheckStateChanged.AddDynamic(this, &UProfileCustomizing::OnCheckCream);
}

void UProfileCustomizing::OnCheckMark(bool bIsChecked)
{
	if (bIsChecked)
	{
		WidgetSwitcher->SetActiveWidgetIndex(MARK_IDX);
		CheckBoxAcc->SetCheckedState(ECheckBoxState::Unchecked);
		CheckBoxCream->SetCheckedState(ECheckBoxState::Unchecked);
	}
	else
	{
		CheckBoxMark->SetCheckedState(ECheckBoxState::Checked);
	}
}

void UProfileCustomizing::OnCheckCream(bool bIsChecked)
{
	if (bIsChecked)
	{
		WidgetSwitcher->SetActiveWidgetIndex(CREAM_IDX);
		CheckBoxMark->SetCheckedState(ECheckBoxState::Unchecked);
		CheckBoxAcc->SetCheckedState(ECheckBoxState::Unchecked);
	}
	else
	{
		CheckBoxCream->SetCheckedState(ECheckBoxState::Checked);
	}
}

void UProfileCustomizing::OnCheckAcc(bool bIsChecked)
{
	if (bIsChecked)
	{
		WidgetSwitcher->SetActiveWidgetIndex(ACC_IDX);
		CheckBoxCream->SetCheckedState(ECheckBoxState::Unchecked);
		CheckBoxMark->SetCheckedState(ECheckBoxState::Unchecked);
	}
	else
	{
		CheckBoxAcc->SetCheckedState(ECheckBoxState::Checked);
	}
}
