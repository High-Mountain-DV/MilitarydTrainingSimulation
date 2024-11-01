// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/JsonParseLib.h"

FString UJsonParseLib::ParseJson(const FString& json)
{
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(json);
	TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
	FString returnValue;

	// if (FJsonSerializer::Deserialize(reader, result))
	// {
	// 	TArray<TSharedPtr<FJsonValue>> parseDataList = result->GetArrayField(TEXT("items"));
	// 	for (TSharedPtr<FJsonValue> data : parseDataList)
	// 	{
	// 		FString bookName = data->AsObject()->GetStringField("bk_nm");
	// 		FString authorName = data->AsObject()->GetStringField("aut_nm");
	// 		returnValue.Append(FString::Printf(TEXT("BookName : %s / AuthorName : %s\n"), *bookName, *authorName));
	// 	}
	// }

	return returnValue;
}

FString UJsonParseLib::MakeJson(const TMap<FString, FString>& source)
{
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());

	for (TPair<FString, FString> pair : source)
	{
		jsonObject->SetStringField(pair.Key, pair.Value);
	}

	FString json;
	TSharedRef<TJsonWriter<TCHAR>> writer = TJsonWriterFactory<TCHAR>::Create(&json);
	FJsonSerializer::Serialize(jsonObject.ToSharedRef(), writer);
	
	return json;
}
