#define MAX_TOKEN_NR 3

enum TokenType {KEYWORD, NUMBER, STRING};
enum KeywordCode {ID, CL, GT, BT, ST};
union TokenValue 
{
	enum KeywordCode eKeyword;
	unsigned int uiNumber;
	char *pcString;
};
struct Token 
{
	enum TokenType eType;
	union TokenValue uValue;
};
extern struct Token asToken[MAX_TOKEN_NR];

void DecodeMsg(char *pcString);
