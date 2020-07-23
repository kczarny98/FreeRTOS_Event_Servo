enum CompResult {DIFFERENT, EQUAL};
enum Result {OK, ERROR};
void CopyString(char pcSource[], char pcDestination[]);
void AppendString(char pcSourceStr[], char pcDestinationStr[]);
enum Result eHexStringToUInt(char pcStr[], unsigned int *puiValue);
void AppendUIntToString (unsigned int uiValue, char pcDestinationStr[]);
enum CompResult eCompareString(char pcStr1[], char pcStr2[]);
void ReplaceCharactersInString(char pcString[], char cOldChar, char cNewChar);
