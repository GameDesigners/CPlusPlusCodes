#include <stdlib.h>  //NULL,strtod(),malloc(),free()
#include <errno.h>   //errno,ERANGE
#include <math.h>    //HUGE_VAL
#include <string.h>  //memcpy()
#include "LJson.h"

#ifndef LJSON_PARSE_STACK_INIT_SIZE
#define LJSON_PARSE_STACK_INIT_SIZE 256
#endif // !LJSON_PARSE_STACK_INIT_SIZE


#define Except(c,ch)       do{assert(*(c->json)==(ch));c->json++;}while(0)
#define IsDigit(ch)        ((ch)>='0' && (ch)<='9')
#define IsDigit1To9(ch)    ((ch)>='1' && (ch)<='9')
#define PushC(c,ch)        do{*(char*)LJsonContentPush(c,sizeof(char))=(ch);}while(0)  
#define StringError(c,ret) do{c->top=head;return ret;}while(0)

typedef struct
{
	const char* json;
	char*       stack;
	size_t      size;
	size_t      top;
}LJsonContent;

static void* LJsonContentPush(LJsonContent* c, size_t size)
{
	void* ret;
	assert(size > 0);
	if (c->top + size >= c->size)
	{
		if (c->size == 0)
			c->size = LJSON_PARSE_STACK_INIT_SIZE;
		while (c->top + size >= c->size)
			c->size += c->size >> 1;
		c->stack = (char*)realloc(c->stack, c->size);
	}
	ret = c->stack + c->top;
	c->top += size;
	return ret;
}

static void* LJsonContentPop(LJsonContent* c, size_t size)
{
	assert(c->top >= size);
	return c->stack + (c->top -= size);
}

//跳过空白符
//--------------------------------------------------------------
static void LJsonParseWhitespace(LJsonContent* c)
{
	const char* p = c->json;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
		p++;

	c->json = p;
}

/*
//解析True
//--------------------------------------------------------------
int LJsonParseTrue(LJsonContent* c, LJsonValue* pValue)
{
	Except(c, 't');
	if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
		return LJsonParse_Invalid_Value;
	c->json += 3;
	pValue->type = LTrue;
}

//解析True
//--------------------------------------------------------------
int LJsonParseFalse(LJsonContent* c, LJsonValue* pValue)
{
	Except(c, 'f');
	if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
		return LJsonParse_Invalid_Value;
	c->json += 4;
	pValue->type = LFalse;
	return LJsonParse_Ok;
}


//解析Null
//--------------------------------------------------------------
static int LJsonParseNull(LJsonContent* c, LJsonValue* pValue)
{
	Except(c, 'n');
	if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
		return LJsonParse_Invalid_Value;

	c->json += 3;
	pValue->type = LNull;
	return LJsonParse_Ok;
}
*/

static int LJsonParseLiteral(LJsonContent* c, LJsonValue* pValue, const char* literal, LJsonType type)
{
	size_t i;
	Except(c, literal[0]);
	for (i = 0; literal[i + 1] != '\0'; i++)
		if (c->json[i] != literal[i + 1])
			return LJsonParse_Invalid_Value;

	c->json += i;
	pValue->type = type;
	return LJsonParse_Ok;
}


//解析数字
//--------------------------------------------------------------
static int LJsonParseNumber(LJsonContent* c, LJsonValue* pValue)
{
	char* p = c->json;

	// 校验数字字符串是否合法
	//  number = [ "-" ] int [ frac ] [ exp ]
	//  int = "0" / digit1 - 9 * digit
    //  frac = "." 1 * digit
    //  exp = ("e" / "E")["-" / "+"] 1 * digit
	//-----------------------------------------
	if (*p == '-') p++;
	if (*p == '0') p++;
	else
	{
		if (!IsDigit1To9(*p))
			return LJsonParse_Invalid_Value;
		for (p++; IsDigit(*p); p++);
	}

	if (*p == '.')
	{
		p++;
		if (!IsDigit(*p)) 
			return LJsonParse_Invalid_Value;
		for (p++; IsDigit(*p); p++);
	}

	if (*p == 'e' || *p == 'E')
	{
		p++;
		if (*p == '+' || *p == '-') 
			p++;
		if (!IsDigit(*p))
			return LJsonParse_Invalid_Value;
		for (p++; IsDigit(*p); p++);
	}

	errno = 0;
	pValue->n = strtod(c->json, NULL);
	if (errno == ERANGE && (pValue->n >= HUGE_VAL || pValue->n <= -HUGE_VAL))
		return LJsonParse_Number_Too_Big;
	c->json = p;
	pValue->type = LNumber;
	return LJsonParse_Ok;
}

//字符串
//--------------------------------------------------------------
void LJsonFree(LJsonValue* pValue)
{
	assert(pValue != NULL);
	if (pValue->type == LString)
		free(pValue->str.data);
	pValue->type = LNull;
}

void LJsonSetString(LJsonValue* pValue, const char* s, size_t len)
{
	assert(pValue != NULL && (s != NULL || len == 0));
	LJsonFree(pValue);
	pValue->str.data = (char*)malloc(sizeof(char) * (len + 1));
	assert(pValue->str.data!=NULL);
	memcpy(pValue->str.data, s, len);
	pValue->str.data[len] = '\0';
	pValue->str.len = len;
	pValue->type = LString;
}

static const char* LJsonParseHex4(const char* p, unsigned* u)
{
	int i;
	*u = 0;
	for (i = 0; i < 4; i++)
	{
		char ch = *p++;
		*u << 4;
		if (ch >= '0' && ch <= '9')  *u |= ch - '0';
		else if (ch >= 'A' && ch <= 'F')  *u |= ch - ('A' - 10);
		else if (ch >= 'a' && ch <= 'f')  *u |= ch - ('a' - 10);
		else return NULL;
	}
	return p;
}

static void LJsonEncode_UTF8(LJsonContent* c, unsigned u)
{
	if (u <= 0x7F)
		PUTC(c, u & 0xFF);
	else if (u <= 0x7FF) {
		PUTC(c, 0xC0 | ((u >> 6) & 0xFF));
		PUTC(c, 0x80 | (u & 0x3F));
	}
	else if (u <= 0xFFFF) {
		PUTC(c, 0xE0 | ((u >> 12) & 0xFF));
		PUTC(c, 0x80 | ((u >> 6) & 0x3F));
		PUTC(c, 0x80 | (u & 0x3F));
	}
	else {
		assert(u <= 0x10FFFF);
		PUTC(c, 0xF0 | ((u >> 18) & 0xFF));
		PUTC(c, 0x80 | ((u >> 12) & 0x3F));
		PUTC(c, 0x80 | ((u >> 6) & 0x3F));
		PUTC(c, 0x80 | (u & 0x3F));
	}
}

static int LJsonParseStringRaw(LJsonContent* c, char** str, size_t* len)
{
	size_t head = c->top;
	unsigned u, u2;
	const char* p;
	Except(c, '\"');
	p = c->json;
	for (;;)
	{
		char ch = *p++;
		switch (ch)
		{
		case '\"':
			*len = c->top - head;
			*str = LJsonContentPop(c, *len);
			c->json = p;
			return LJsonParse_Ok;

		case '\0':
			c->top = head;
			return LJsonParse_Miss_Quotation_Mark;

		case '\\':
			switch (*p++)
			{
			case '\"': PushC(c, '\"'); break;
			case '\\': PushC(c, '\\'); break;
			case '/':  PushC(c, '/');  break;
			case 'b':  PushC(c, '\b'); break;
			case 'f':  PushC(c, '\f'); break;
			case 'n':  PushC(c, '\n'); break;
			case 'r':  PushC(c, '\r'); break;
			case 't':  PushC(c, '\t'); break;
			case 'u':
				if (!(p = LJsonParseHex4(p, &u)))
					StringError(c, LJsonParse_Invalid_Unicode_Hex);

				if (u >= 0xD800 && u <= 0xDBFF) { /* surrogate pair */
					if (*p++ != '\\')
						StringError(c, LJsonParse_Invalid_Unicode_Surrogate);
					if (*p++ != 'u')
						StringError(c, LJsonParse_Invalid_Unicode_Surrogate);
					if (!(p = LJsonParseHex4(p, &u2)))
						StringError(c, LJsonParse_Invalid_Unicode_Hex);
					if (u2 < 0xDC00 || u2 > 0xDFFF)
						StringError(c, LJsonParse_Invalid_Unicode_Surrogate);
					u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
				}
				LJsonEncode_UTF8(c, u);
				break;
			default:
				StringError(c, LJsonParse_Invalid_String_Escape);
			}
			break;
		default:
			if ((unsigned char)ch < 0x20)
				StringError(c, LJsonParse_Invalid_String_Char);
			PushC(c, ch);
		}
	}
}

static int LJsonParseString(LJsonContent* c, LJsonValue* pValue)
{
	int ret;
	char* s;
	size_t len;
	if ((ret = LJsonParseStringRaw(c, &s, &len)) == LJsonParse_Ok)
		LJsonSetString(pValue, s, len);
	return ret;
}

static LJsonParseArray(LJsonContent* c, LJsonValue* pValue)
{
	size_t i, size = 0;
	int ret;
	Except(c, '[');
	LJsonParseWhitespace(c);

	if (*c->json == ']')
	{
		c->json++;
		pValue->type = LArray;
		pValue->arr.elem = NULL;
		pValue->arr.size = 0;
		return LJsonParse_Ok;
	}

	for (;;)
	{
		LJsonValue e;
		LJsonInitValue(&e);
		if ((ret = LJsonParseValue(c, &e)) != LJsonParse_Ok)
			break;

		memcpy(LJsonContentPush(c, sizeof(LJsonValue)), &e, sizeof(LJsonValue));
		size++;
		LJsonParseWhitespace(c);
		if (*c->json == ',') {
			c->json++;
			LJsonParseWhitespace(c);
		}
		else if (*c->json == ']')
		{
			c->json++;
			pValue->type = LArray;
			pValue->arr.size = size;
			size *= sizeof(LJsonValue);
			memcpy(pValue->arr.elem = (LJsonValue*)malloc(size), LJsonContentPop(c, size), size);
			return LJsonParse_Ok;
		}
		else
		{
			ret = LJsonParse_Miss_Comma_Or_Square_Bracket;
			break;
		}
	}

	for (i = 0; i < size; i++)
		LJsonFree((LJsonValue*)LJsonContentPop(c, sizeof(LJsonValue)));
	return ret;
}

static int LJsonParseObject(LJsonContent* c, LJsonValue* pValue)
{
	size_t i, size;
	LJsonPair pair;
	int ret;
	Except(c, '{');
	LJsonParseWhitespace(c);
	if (*c->json == '}')
	{
		c->json++;
		pValue->type = LObject;
		pValue->obj.pairs = NULL;
		pValue->obj.size = 0;
		return LJsonParse_Ok;
	}

	pair.key = NULL;
	for (;;)
	{
		char* str;
		LJsonInitValue(&pair.value);

		//解析键值
		if (*c->json != '"') {
			ret = LJsonParse_Miss_Object_Key;
			break;
		}
		if ((ret = LJsonParseStringRaw(c, &str, &pair.klen)) != LJsonParse_Ok)
			break;
		memcpy(pair.key = (char*)malloc(sizeof(char) * (pair.klen + 1)), str, pair.klen);
		pair.key[pair.klen] = '\0';

		//解析":"
		LJsonParseWhitespace(c);
		if (*c->json != ':')
		{
			ret = LJsonParse_Miss_COLON;
			break;
		}
		c->json++;

		//解析值
		LJsonParseWhitespace(c);
		if ((ret = LJsonParseValue(c, &pair.value)) == LJsonParse_Ok)
			break;

		//压入栈中存储
		memcpy(LJsonContentPush(c, sizeof(LJsonPair)), &pair, sizeof(LJsonPair));
		size++;

		//清理此前记录
		pair.key = NULL;
		LJsonParseWhitespace(c);

		if (*c->json == ',')
		{
			c->json++;
			LJsonParseWhitespace(c);
		}
		else if (*c->json == '}')
		{
			size_t bytes = sizeof(LJsonPair) * size;
			c->json++;
			pValue->type = LObject;
			pValue->obj.size = size;
			memcpy(pValue->obj.pairs = (LJsonPair*)malloc(bytes), LJsonContentPop(c, bytes), bytes);
			return LJsonParse_Ok;
		}
		else
		{
			ret = LJsonParse_Miss_Comma_Or_Curly_Bucket;
			break;
		}
	}

	free(pair.key);
	for (i = 0; i < size; i++)
	{
		LJsonPair* pair = LJsonContentPop(c, sizeof(LJsonPair));
		free(pair->key);
		LJsonFree(&pair->value);
	}
	pValue->type = LNull;
	return ret;
}




//解析值
//--------------------------------------------------------------
static int LJsonParseValue(LJsonContent* c, LJsonValue* pValue)
{
	switch (*c->json)
	{
	case 't' :  return LJsonParseLiteral(c, pValue, "true", LTrue);
	case 'f' :  return LJsonParseLiteral(c, pValue,"false",LFalse);
	case 'n' :  return LJsonParseLiteral(c, pValue, "null", LNull);
	default  :  return LJsonParseNumber(c, pValue);
	case '"' :  return LJsonParseString(c, pValue);
	case '[' :  return LJsonParseArray(c, pValue);
	case '{' :  return LJsonParseObject(c, pValue);
	case '\0':  return LJsonParse_Expect_Value;
	}
}

//！Json解析的入口
//--------------------------------------------------------------
int LJsonParse(LJsonValue* pValue, const char* json)
{
	LJsonContent c;
	int ret;
	assert(pValue != NULL);
	c.json = json;
	c.stack = NULL;
	c.size = c.top = 0;

	LJsonInitValue(pValue);
	LJsonParseWhitespace(&c);

	if ((ret = LJsonParseValue(&c, pValue)) == LJsonParse_Ok)
	{
		LJsonParseWhitespace(&c);
		if (*c.json != '\0')
			ret = LJsonParse_Root_Not_Singular;
	}

	assert(c.top == 0);
	free(c.stack);
	return ret;
}

LJsonType LJsonGetType(const LJsonValue* pValue)
{
	assert(pValue != NULL);
	return pValue->type;
}

double LJsonGetNumber(const LJsonValue* pValue)
{
	assert(pValue != NULL && pValue->type == LNumber);
	return pValue->n;
}

void LJsonSetNumber(LJsonValue* pValue, double n)
{

}

int LJsonGetBoolean(const LJsonValue* pValue)
{
	return 0;
}

const char* LJsonGetString(const LJsonValue* pValue)
{
	assert(pValue != NULL && pValue->type == LString);
	return pValue->str.data;
}

void LJsonSetString(LJsonValue* pValue, const char* s)
{
}

size_t LJsonGetStringLength(const LJsonValue* pValue)
{
	assert(pValue != NULL && pValue->type == LString);
	return pValue->str.len;
}

size_t LJsonGetArraySize(const LJsonValue* pValue)
{
	assert(pValue != NULL && pValue == LArray);
	return pValue->arr.size;
}

LJsonValue* LJsonGetArrayElement(const LJsonValue* pValue, size_t index)
{
	assert(pValue != NULL && pValue == LArray);
	assert(index < pValue->arr.size);
	return &pValue->arr.elem[index];
}

LJsonValue* LJsonGetArrayElements(const LJsonValue* pValue)
{
	assert(pValue != NULL && pValue == LArray);
	return pValue->arr.elem;
}

size_t LJsonGetObjectMemberCount(const LJsonValue* pValue)
{
	assert(pValue != NULL && pValue == LObject);
	return pValue->obj.size;
}

const char* LJsonGetObjectMemberKey(const LJsonValue* pValue, size_t index)
{
	assert(pValue != NULL && pValue == LArray);
	assert(index < pValue->obj.size);
	return pValue->obj.pairs[index].key;
}

size_t LJsonGetObjectMemberKeyLength(const LJsonValue* pValue, size_t index)
{
	assert(pValue != NULL && pValue == LArray);
	assert(index < pValue->obj.size);
	return pValue->obj.pairs[index].klen;
}

LJsonValue* LJsonGetObjectMemberValue(const LJsonValue* pValue, size_t index)
{
	assert(pValue != NULL && pValue == LArray);
	assert(index < pValue->obj.size);
	return &pValue->obj.pairs[index].value;
}

