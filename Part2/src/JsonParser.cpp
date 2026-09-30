#include "JsonParser.h"

#include <cstdio>
#include <cerrno>
#include <cstring>

void DisplayJsonValue(JsonValue *node, int depth);

void RemoveWhitespace(FILE *file, char &c) 
{
    while(c == ' ') {
        c = fgetc(file);
    }
}

/**
 * Parse string from JSON string. 
 * @note Strings are limited to 10 characters in this implementation because string values in this project do not contain 
 * more than 10 characters. This is not reasonable for a general JSON parser. 
 */
JsonString* ParseJsonString(FILE *file) 
{
    JsonString *str = (JsonString*)malloc(sizeof(JsonString));
    str->value = (char*)calloc(10, sizeof(char)); 

    int i = 0;

    char c;
    while ((c = fgetc(file)) != '"' && i < 9)
    {
        str->value[i] = c;
        i++;
    }

    str->value[i++] = '\0';
    str->length = i;
    return str;
}

/**
 * Parse boolean value from JSON string. 
 */
bool ParseBoolean(FILE *file)
{
    char b[6]; // The char array is capped to 6 since that is the max length of the boolean options (F A L S E \0)
    int i = 0;
    
    char c;
    while((c = fgetc(file)) != '\n')
    {
        b[i] = c;
    }

    b[i] = '\0';

    return strcmp(b, "false") == 0 ? false : true;
}

JsonNumber* ParseJsonNumber(FILE *file, char firstDigit)
{
    JsonNumber *number = (JsonNumber*)malloc(sizeof(JsonNumber));
    
    char strNumber[100] = {};
    strNumber[0] = firstDigit;
    int index = 1;
    char c = fgetc(file);
    while(isdigit(c) || c == '.')
    {
        strNumber[index++] = c;
        if (c == '.') number->isDouble = true;
        c = fgetc(file);
    }

    if (number->isDouble)
    {
        char* endptr;
        number->dValue = strtof(strNumber, &endptr);
        if (endptr == strNumber)
        {
            printf("ERROR::Could not parse double: %s", strNumber);
        }
    }
    else {
        number->iValue = std::stoi(strNumber);
    }

    return number;
}

JsonPair* ParseJsonPair(FILE *file, int &count)
{

    char c;
    while ((c = fgetc(file)) == ' ' || c == '\n' || c == '\t');

    if (c == '}') return nullptr;

    JsonPair *pair = (JsonPair*)malloc(sizeof(JsonPair));

    pair->key = ParseJsonString(file);
    c = fgetc(file);

    RemoveWhitespace(file, c);

    if (c != ':') 
    {
        printf("ERROR::Invalid JSON pair.\n");
        return nullptr;
    }

    pair->value = ParseJsonValue(file);
    pair->next = ParseJsonPair(file, ++count);
    return pair;
}


JsonObject* ParseJsonObject(FILE *file) 
{
    JsonObject *object = (JsonObject*)malloc(sizeof(JsonObject));
    int count = 0;
    object->pairs = ParseJsonPair(file, count);
    object->count = count;
    return object;
}

JsonArrayElement* ParseJsonArrayElement(FILE *file, int &count)
{
    char c;
    while((c = fgetc(file)) == ' ' || c == '\n' || c == '\t');

    if (c == ']') return nullptr;

    // TODO: This is a hack and needs to be fixed. 
    ungetc(c, file);
    JsonArrayElement *element = (JsonArrayElement*)malloc(sizeof(JsonArrayElement));
    element->value = ParseJsonValue(file);
    element->next = ParseJsonArrayElement(file, ++count);
    return element;
}

JsonArray* ParseJsonArray(FILE *file)
{
    JsonArray *array = (JsonArray*)malloc(sizeof(JsonArray));
    int count = 0;
    array->elements = ParseJsonArrayElement(file, count);
    array->length = count;
    return array;
}

JsonValue* ParseJsonValue(FILE *file) 
{
    JsonValue *jsonValue = (JsonValue*)malloc(sizeof(JsonValue));

    char c;
    while( (c = fgetc(file)) != EOF ) 
    {
        RemoveWhitespace(file, c);

        if (c == '\n' || c == '\t' || c == ',' || c == '}' || c == ']') continue;


        if (c == '{')
        {
            jsonValue->type = Object;
            jsonValue->object = ParseJsonObject(file);
            break;
        }
        else if (c == '[')
        {
            jsonValue->type = Array;
            jsonValue->array = ParseJsonArray(file);
            break;
        }
        else if (c == '"')
        {
            jsonValue->type = String;
            jsonValue->string = ParseJsonString(file);
            break;
        }
        else if (c == 't' || c == 'f')
        {
            // Parse boolean
        }
        else if (isdigit(c) || c == '-')
        {
            jsonValue->type = Number;
            jsonValue->number = ParseJsonNumber(file, c);
            break;
        }
        else if (c == 'n')
        {
            // Parse null
        }
    }

    return jsonValue;
}
void PrintDepth(int depth) 
{
    while (depth > 0)
    {
        printf("\t");
        depth--;
    }
}

void DisplayJsonNumber(JsonNumber *number, int depth)
{
    if (number->isDouble)
        printf("%f\n", number->dValue);
    else
        printf("%d\n", number->iValue);
}

void DisplayJsonString(JsonString *str, int depth)
{
    PrintDepth(depth);
    printf("%s\n", str->value);
}

void DisplayJsonPair(JsonPair *pair, int depth)
{
    if (!pair) return;

    PrintDepth(depth);
    printf("%s : ", pair->key->value);
    DisplayJsonValue(pair->value, depth);
    
    DisplayJsonPair(pair->next, depth);
}

void DisplayJsonObject(JsonObject *object, int depth)
{
    printf("{\n");
    DisplayJsonPair(object->pairs, ++depth);
    printf("}\n");
}

void DisplayJsonArrayElement(JsonArrayElement *element, int depth)
{
    if (element == nullptr) return;
    DisplayJsonValue(element->value, depth);
    DisplayJsonArrayElement(element->next, depth);
}

void DisplayJsonArray(JsonArray *array, int depth)
{
    printf("[\n");
    DisplayJsonArrayElement(array->elements, ++depth);
    printf("]\n");
}

void DisplayJsonValue(JsonValue *node, int depth)
{
    switch(node->type)
    {
        case Object:
            return DisplayJsonObject(node->object, depth);
            break;
        case Array:
            DisplayJsonArray(node->array, depth);
            break;
        case Number:
            DisplayJsonNumber(node->number, depth);
            break;
        case String:
            DisplayJsonString(node->string, depth);
            break;
        case Boolean:
            // print boolean
            break;
    }
}

void DisplayAST(JsonValue *root)
{
    DisplayJsonValue(root, -1);
}


JsonValue* DeserializeJson(const char* jsonFile) 
{

    // 1. Open file
    FILE* file = std::fopen(jsonFile, "r");
    if (!file) {
        printf("ERROR::%d - Could not open file %s", errno, jsonFile);
        return nullptr;
    }

    // Recursively parse JSON 
    JsonValue *result = ParseJsonValue(file);

    DisplayAST(result);

    return result;
}

