#include "JsonParser.h"
#include "PerformanceUtil.h"

#include <cstdio>
#include <cerrno>
#include <cstring>

void DisplayJsonValue(JsonValue *node, int depth);
JsonObject* ParseJsonObject(FILE *file, char &c);
JsonValue* ParseJsonValue(FILE *file, char &c);

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
JsonString* ParseJsonString(FILE *file, char &c) 
{
    JsonString *str = (JsonString*)malloc(sizeof(JsonString));
    str->value = (char*)calloc(10, sizeof(char)); 

    int i = 0;

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

JsonNumber* ParseJsonNumber(FILE *file, char &c)
{
    JsonNumber *number = (JsonNumber*)malloc(sizeof(JsonNumber));
    
    char strNumber[100] = {};
    int index = 0;
    while(isdigit(c) || c == '-' || c == '.')
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


JsonObject* ParseJsonObject(FILE *file, char &c) 
{
    JsonObject *object = (JsonObject*)malloc(sizeof(JsonObject));
    JsonPair *current = (JsonPair*)malloc(sizeof(JsonPair));
    object->pairs = current;

    c = fgetc(file);
    while(c != '}')
    {
        if (c == ' ' || c == '\n' || c == ',')
        {
            c = fgetc(file);
            continue;
        }

        JsonPair *pair = (JsonPair*)malloc(sizeof(JsonPair));

        pair->key = ParseJsonString(file, c);
        
        RemoveWhitespace(file, c);

        c = fgetc(file);
        if (c != ':')
        {
            // TODO: Log error, invalid JSON Pair
            break;
        }

        c = fgetc(file);
        RemoveWhitespace(file, c);

        pair->value = ParseJsonValue(file, c);

        current->next = pair; 
        current = pair;
        object->count++;
    }


    return object;
}

JsonArray* ParseJsonArray(FILE *file, char &c)
{
    JsonArray *array = (JsonArray*)malloc(sizeof(JsonArray));
    JsonArrayElement *current = (JsonArrayElement*)malloc(sizeof(JsonArrayElement));
    array->elements = current;

    while((c = fgetc(file)) != ']')
    {
        if (c == ' ' || c == '\n' || c == '\t' || c == ',') continue;

        JsonArrayElement *element = (JsonArrayElement*)malloc(sizeof(JsonArrayElement));
        element->value = ParseJsonValue(file, c);
        current->next = element;
        current = element;
        array->length++;
    }

    return array;
}


JsonValue* ParseJsonValue(FILE *file, char &c) 
{
    RemoveWhitespace(file, c);
    while (c == '\n' || c == '\t' || c == ',') c = fgetc(file);

    JsonValue *jsonValue = (JsonValue*)malloc(sizeof(JsonValue));

    if (c == '{')
    {
        jsonValue->type = Object;
        jsonValue->object = ParseJsonObject(file, c);
    }
    else if (c == '[')
    {
        jsonValue->type = Array;
        jsonValue->array = ParseJsonArray(file, c);
    }
    else if (c == '"')
    {
        jsonValue->type = String;
        jsonValue->string = ParseJsonString(file, c);
    }
    else if (c == 't' || c == 'f')
    {
        // Parse boolean
    }
    else if (isdigit(c) || c == '-')
    {
        jsonValue->type = Number;
        jsonValue->number = ParseJsonNumber(file, c);
    }
    else if (c == 'n')
    {
        // Parse null
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
    printf("%s = ", pair->key->value);

    if (pair->value->type == Array || pair->value->type == Object) 
    {
        printf("\n");
        depth++;
        DisplayJsonValue(pair->value, depth);
        DisplayJsonPair(pair->next, --depth);
    } 
    else 
    {
        DisplayJsonValue(pair->value, depth);
        DisplayJsonPair(pair->next, depth);
    }
}

void DisplayJsonObject(JsonObject *object, int depth)
{
    PrintDepth(depth);
    printf("Object:\n");
    // NOTE: First node in pairs list is sentinel node. 
    DisplayJsonPair(object->pairs->next, ++depth);
}

void DisplayJsonArrayElement(JsonArrayElement *element, int depth)
{
    if (element == nullptr) return;
    DisplayJsonValue(element->value, depth);
    DisplayJsonArrayElement(element->next, depth);
}

void DisplayJsonArray(JsonArray *array, int depth)
{
    PrintDepth(depth);
    printf("Array:\n");
    DisplayJsonArrayElement(array->elements->next, ++depth); // NOTE: array->elements is sentinel node, next=first real item in linked list
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
    DisplayJsonValue(root, 0);
}


JsonValue* DeserializeJson(const char* jsonFile) 
{

    fileOpenStartOS = GetOSTime();
    fileOpenStartCPU = GetCPUTime();

    // 1. Open file
    FILE* file = std::fopen(jsonFile, "r");
    if (!file) {
        printf("ERROR::%d - Could not open file %s", errno, jsonFile);
        return nullptr;
    }

    fileOpenEndOS = GetOSTime();
    fileOpenEndCPU = GetCPUTime();

    jsonParseStartOS = GetOSTime();
    jsonParseStartCPU = GetCPUTime();

    // Recursively parse JSON 
    char c;
    c = fgetc(file);
    JsonValue *result = ParseJsonValue(file, c);

    jsonParseEndOS = GetOSTime();
    jsonParseEndCPU = GetCPUTime();

    return result;
}

Entry DeserializeEntry(JsonObject *obj)
{
    JsonPair *node = obj->pairs->next;

    Entry entry = {};
    while(node)
    {
        char *key = node->key->value;
        if (strcmp(key, "x0") == 0)
            entry.x0 = node->value->number->dValue;
        else if (strcmp(key, "y0") == 0)
            entry.y0 = node->value->number->dValue;
        else if (strcmp(key, "x1") == 0)
            entry.x1 = node->value->number->dValue;
        else if (strcmp(key, "y1") == 0)
            entry.y1 = node->value->number->dValue;

            node = node->next;
    }

    return entry;
}

Entry* DeserialzeEntries(JsonArray *array)
{
    Entry *entries = (Entry*)malloc(sizeof(Entry) * array->length);
    JsonArrayElement *node = array->elements->next;

    int index = 0;
    while(node)
    {
        entries[index] = DeserializeEntry(node->value->object);
        node = node->next;
        index++;
    }

    return entries;
}

CoordinatePairs DeserializeCoordinatePairs(const char* jsonFile)
{
    JsonValue *ast = DeserializeJson(jsonFile);
    JsonObject *obj = ast->object;
    JsonPair *p = obj->pairs->next;


    CoordinatePairs pairs;
    pairs.entries = DeserialzeEntries(p->value->array);
    pairs.count = p->value->array->length;

    return pairs;
}