// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkDefinition.h"

FName UHyperlinkDefinition::GetBodyPattern() const
{
	return TEXT(R"(.*)");
}

FName UHyperlinkDefinition::GetHyperlinkBase() const
{
	// TODO
	return TEXT("unreal://TODO");
}
