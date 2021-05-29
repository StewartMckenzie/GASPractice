// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttributes.h"

FGameplayAttribute UPlayerAttributes::SpeedAttribute()
{
	static FProperty* Property = FindFieldChecked<FProperty>(UPlayerAttributes::StaticClass(),GET_MEMBER_NAME_CHECKED(UPlayerAttributes,Speed));
	return FGameplayAttribute(Property);
}

FGameplayAttribute UPlayerAttributes::HealthAttribute()
{
	static FProperty* Property = FindFieldChecked<FProperty>(UPlayerAttributes::StaticClass(),GET_MEMBER_NAME_CHECKED(UPlayerAttributes,Health));
	return FGameplayAttribute(Property);
}

FGameplayAttribute UPlayerAttributes::DamageModAttribute()
{
	static FProperty* Property = FindFieldChecked<FProperty>(UPlayerAttributes::StaticClass(),GET_MEMBER_NAME_CHECKED(UPlayerAttributes,DamageMod));
	return FGameplayAttribute(Property);
}
