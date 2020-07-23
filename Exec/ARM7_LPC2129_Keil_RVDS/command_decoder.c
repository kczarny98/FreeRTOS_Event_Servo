#include "command_decoder.h"
#include "string.h"

#define NULL 										0
#define MAX_TOKEN_NR 						3
#define MAX_KEYWORD_STRING_LTH 	10
#define MAX_KEYWORD_NR 					5
#define DELIMITER_CHARACTER 		' '

enum TokensState {TOKEN, DELIMITER};

struct Keyword 
{
	enum KeywordCode eCode;
	char cString[MAX_KEYWORD_STRING_LTH];
};

struct Keyword asKeywordList[MAX_KEYWORD_NR] = 
{
	{ID, "id"},
	{CL, "callib"},
	{GT, "goto"},
	{BT, "button"},
	{ST, "status"}
};

struct Token asToken[MAX_TOKEN_NR];
unsigned char ucTokenNr = 0;

unsigned char ucFindTokensInString(char *pcString) 
{
	enum TokensState eCurrentTokenType = DELIMITER;
	unsigned char ucCharacterCounter;
	char cCurrentCharacter;

	ucTokenNr = 0;

	for (ucCharacterCounter = 0; ;ucCharacterCounter++) 
	{
		cCurrentCharacter = pcString[ucCharacterCounter];
		switch (eCurrentTokenType) 
		{
			case TOKEN:
				if (ucTokenNr == MAX_TOKEN_NR) 
				{
					return ucTokenNr;
				}
				else if (cCurrentCharacter == DELIMITER_CHARACTER) 
				{
					eCurrentTokenType = DELIMITER;
				}
				else if (cCurrentCharacter == NULL) 
				{
					return ucTokenNr;
				}
				else 
				{
					eCurrentTokenType = TOKEN;
				}
			break;
				
			case DELIMITER:
				if (cCurrentCharacter == NULL) 
				{
					return ucTokenNr;
				}
				else if (cCurrentCharacter != DELIMITER_CHARACTER) 
				{
					eCurrentTokenType = TOKEN;
					asToken[ucTokenNr].uValue.pcString = pcString + ucCharacterCounter;
					ucTokenNr++;
				}
				else 
				{
					eCurrentTokenType = DELIMITER;
				}
			break;
		}
	}
}

enum Result eStringToKeyword (char pcStr[], enum KeywordCode *peKeywordCode) 
{
	unsigned char ucKeywordIndex;

	for (ucKeywordIndex = 0; ucKeywordIndex < MAX_KEYWORD_NR; ucKeywordIndex++) 
	{
		if (eCompareString(pcStr, asKeywordList[ucKeywordIndex].cString) == EQUAL) 
		{
			*peKeywordCode = asKeywordList[ucKeywordIndex].eCode;
			return OK;
		}
	}
	return ERROR;
}

void DecodeTokens(void) 
{
	unsigned char ucTokenIndex;
	enum KeywordCode eCurrentKeywordCode;
	unsigned int uiCurrentValue;
	struct Token *psCurrentToken;

	for (ucTokenIndex = 0; ucTokenIndex < ucTokenNr; ucTokenIndex++) 
	{
		psCurrentToken = &asToken[ucTokenIndex];
		if (eStringToKeyword(psCurrentToken->uValue.pcString, &eCurrentKeywordCode) == OK) 
		{
			psCurrentToken->eType = KEYWORD;
			psCurrentToken->uValue.eKeyword = eCurrentKeywordCode;
		}
		else if (eHexStringToUInt(psCurrentToken->uValue.pcString, &uiCurrentValue) == OK) 
		{
			psCurrentToken->eType = NUMBER;
			psCurrentToken->uValue.uiNumber = uiCurrentValue;
		}
		else 
		{
			psCurrentToken->eType = STRING;
		}
	}
}

void DecodeMsg(char *pcString) 
{
	ucFindTokensInString(pcString);
	ReplaceCharactersInString(pcString, DELIMITER_CHARACTER, NULL);
	DecodeTokens();
}
