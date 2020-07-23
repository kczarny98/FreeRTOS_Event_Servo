#include "string.h"

#define NULL 				0
#define LAST_TETRAD 0x0F

void CopyString(char pcSource[], char pcDestination[]) 
{
	unsigned char ucCounter;

	for (ucCounter = 0; pcSource[ucCounter] != NULL; ucCounter++) 
	{
		pcDestination[ucCounter] = pcSource[ucCounter];
	}
	pcDestination[ucCounter] = NULL;
}

enum CompResult eCompareString(char pcStr1[], char pcStr2[]) 
{
	unsigned char ucCounter;
	
	for (ucCounter = 0; pcStr1[ucCounter] != NULL; ucCounter++) 
	{
		if (pcStr1[ucCounter] != pcStr2[ucCounter])
		{
			return DIFFERENT;
		}
	}
	
	if (pcStr1[ucCounter] != pcStr2[ucCounter])
	{
		return DIFFERENT;
	}
	else
	{
		return EQUAL;
	}
}

void AppendString(char pcSourceStr[], char pcDestinationStr[]) 
{
	unsigned char ucCounter;
	
	for (ucCounter = 0; pcDestinationStr[ucCounter] != NULL; ucCounter++) {}
	CopyString(pcSourceStr, pcDestinationStr + ucCounter);
}

void ReplaceCharactersInString(char pcString[], char cOldChar, char cNewChar) 
{
	unsigned char ucCounter;

	for (ucCounter = 0; pcString[ucCounter] != NULL; ucCounter++) 
	{
		if (pcString[ucCounter] == cOldChar)
		{
			pcString[ucCounter] = cNewChar;
		}
	}
}

void UIntToHexStr(unsigned int uiValue, char pcStr[]) 
{
	unsigned char ucTetradCounter;
	unsigned char ucCurrentTetrad;

	pcStr[0] = '0';
	pcStr[1] = 'x';
	pcStr[6] = NULL;
	for (ucTetradCounter = 0; ucTetradCounter < 4; ucTetradCounter++) 
	{
		ucCurrentTetrad = (uiValue >> (ucTetradCounter * 4)) & LAST_TETRAD;
		if (ucCurrentTetrad < 10) 
		{
			pcStr[5 - ucTetradCounter] = '0' + ucCurrentTetrad;
		}
		else 
		{
			pcStr[5 - ucTetradCounter] = 'A' + (ucCurrentTetrad - 10);
		}
	}
}

void AppendUIntToString (unsigned int uiValue, char pcDestinationStr[]) 
{
	unsigned char ucCharacterCounter;

	for (ucCharacterCounter = 0; pcDestinationStr[ucCharacterCounter] != NULL; ucCharacterCounter++) {}
	UIntToHexStr(uiValue, pcDestinationStr + ucCharacterCounter);
}

enum Result eHexStringToUInt(char pcStr[], unsigned int *puiValue) 
{
	unsigned char ucCharacterCounter;
	unsigned char ucCurrentCharacter;

	*puiValue = 0;

	if (pcStr[0] != '0' || pcStr[1] != 'x' || pcStr[2] == NULL) 
	{
		return ERROR;
	}
	for (ucCharacterCounter = 2; ucCharacterCounter < 6; ucCharacterCounter++) 
	{
		ucCurrentCharacter = pcStr[ucCharacterCounter];
		if (ucCurrentCharacter == NULL) 
		{
			return OK;
		}
		*puiValue = *puiValue << 4;
		if (ucCurrentCharacter <= '9' && ucCurrentCharacter >= '0') 
		{
			ucCurrentCharacter = ucCurrentCharacter - '0';
		}
		else if (ucCurrentCharacter <= 'F' && ucCurrentCharacter >= 'A') 
		{
			ucCurrentCharacter = ucCurrentCharacter - 'A' + 10;
		}
		else 
		{
			return ERROR;
		}
		*puiValue = *puiValue | ucCurrentCharacter;
	}
	if (pcStr[ucCharacterCounter] == NULL) 
	{
		return OK;
	}
	else 
	{
		return ERROR;
	}
}
