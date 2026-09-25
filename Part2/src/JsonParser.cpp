#include "JsonParser.h"

#include <cstdio>
#include <cerrno>

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

JsonObject* ParseJsonObject(FILE *file) 
{
    JsonObject *object = (JsonObject*)malloc(sizeof(JsonObject));
    JsonPair *currentPair = object->pairs;

    char c;
    while((c = fgetc(file)) != '}')
    {
        RemoveWhitespace(file, c);

        if (c == '\n' || c == '\t') continue;

        currentPair = (JsonPair*)malloc(sizeof(JsonPair));
        currentPair->key = ParseJsonString(file);
        c = fgetc(file);

        RemoveWhitespace(file, c);

        if (c != ':') 
        {
            printf("ERROR::Invalid JSON object.\n");
            return nullptr;
        }

        currentPair->value = ParseJsonValue(file);
        currentPair = currentPair->next;
        object->count++;
    }

    return object;
}

JsonValue* ParseJsonValue(FILE *file) 
{
    JsonValue *jsonValue = (JsonValue*)malloc(sizeof(JsonValue));

    char c;
    while( (c = fgetc(file)) != EOF ) 
    {
        RemoveWhitespace(file, c);

        if (c == '\n' || c == '\t') continue;


        if (c == '{')
        {
            jsonValue->type = Object;
            jsonValue->object = ParseJsonObject(file);
            break;
        }
        else if (c == '[')
        {
            // Parse array
        }
        else if (c == '"')
        {
            // Parse string
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


JsonValue* DeserializeJson(const char* jsonFile) {

    // 1. Open file
    FILE* file = std::fopen(jsonFile, "r");
    if (!file) {
        printf("ERROR::%d - Could not open file %s", errno, jsonFile);
        return nullptr;
    }

    // Recursively parse JSON 
    return ParseJsonValue(file);
}

