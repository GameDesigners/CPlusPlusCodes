#ifndef LEPTJSON_H
#define LEPTJSON_H

#include <assert.h>
#include <stdlib.h>

//Json的6种数据类型：null、boolean、number、string、array、object
typedef enum { LNull, LFalse, LTrue, LNumber, LString, LArray, LObject } LJsonType;

typedef struct LJsonValue LJsonValue;
typedef struct LJsonPair  LJsonPair;

struct LJsonValue
{
	union
	{
		struct { char* data; size_t len; } str;
		struct { LJsonValue* elem; size_t size; } arr;
		struct { LJsonPair* pairs; size_t size; } obj;
		double n;
	};
	LJsonType type;
};

struct LJsonPair
{
	char*      key;
	size_t     klen;
	LJsonValue value;
};

enum
{
	LJsonParse_Ok = 0,
	LJsonParse_Expect_Value,
	LJsonParse_Invalid_Value,
	LJsonParse_Root_Not_Singular,
	LJsonParse_Number_Too_Big,
	LJsonParse_Miss_Quotation_Mark,
	LJsonParse_Invalid_String_Escape,
	LJsonParse_Invalid_String_Char,
	LJsonParse_Invalid_Unicode_Hex,
	LJsonParse_Invalid_Unicode_Surrogate,
	LJsonParse_Miss_Comma_Or_Square_Bracket,
	LJsonParse_Miss_Object_Key,
	LJsonParse_Miss_COLON, //:
	LJsonParse_Miss_Comma_Or_Curly_Bucket,
};

#define LJsonInitValueValue(v)     do{(v)->type=LNull;}while(0)

int         LJsonParse(LJsonValue* pValue, const char* json);
char*       LJsonStringify(const LJsonValue* pValue, size_t* length);



LJsonType   LJsonGetType(const LJsonValue* pValue);

double      LJsonGetNumber(const LJsonValue* pValue);
void        LJsonSetNumber(LJsonValue* pValue, double n);

int         LJsonGetBoolean(const LJsonValue* pValue);
void        LJsonSetBoolean(LJsonValue* pValue, int b);

const char* LJsonGetString(const LJsonValue* pValue);
void        LJsonSetString(LJsonValue* pValue, const char* s);
size_t      LJsonGetStringLength(const LJsonValue* pValue);

size_t      LJsonGetArraySize(const LJsonValue* pValue);
LJsonValue* LJsonGetArrayElement(const LJsonValue* pValue, size_t index);
LJsonValue* LJsonGetArrayElements(const LJsonValue* pValue);

size_t      LJsonGetObjectMemberCount(const LJsonValue* pValue);
const char* LJsonGetObjectMemberKey(const LJsonValue* pValue, size_t index);
size_t      LJsonGetObjectMemberKeyLength(const LJsonValue* pValue, size_t index);
LJsonValue* LJsonGetObjectMemberValue(const LJsonValue* pValue, size_t index);



#endif //!LEPTJSON_H