#include "JsonParser.h"

#include <cstdio>
#include <cerrno>

/**
 * Parse string from JSON string. 
 * @note Strings are limited to 10 characters in this implementation because string values in this project do not contain 
 * more than 10 characters. This is not reasonable for a general JSON parser. 
 */
char* ParseString(FILE *file) 
{
    char* str = (char*)calloc(10, sizeof(char)); 
    int i = 0;

    char c;
    while ((c = fgetc(file)) != '"' && i < 9)
    {
        str[i] = c;
        i++;
    }

    str[i] = '\0';
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

Digit ParseDigit(FILE *file, char firstDigit)
{
    Digit digit = {};
    digit.value = (char*)calloc(100, sizeof(char));
    digit.value[0] = firstDigit;

    int i = 1;
    char c;
    while((c = fgetc(file)) && (isdigit(c) || c == '.')) 
    {
        digit.value[i] = c;

        if (c == '.') {
            digit.isFloat = true;
        }

        i++;
    }

    return digit;
}


void DeserializePairs(const char* jsonFile, Pairs &pairs) {

    // 1. Open file
    FILE* file = std::fopen(jsonFile, "r");
    if (!file) {
        printf("ERROR::%d - Could not open file %s", errno, jsonFile);
        return;
    }

    // 2. Loop over characters until EOF
    int c;
    while((c = fgetc(file)) != EOF) {
        char token = (char)c;
        // A. match character to value

        if (token == '\n' || token == ' ' || token == '\t' || token == ',') continue;

        switch(token) {
            case '{':
            {
                printf("Parsing object.\n");
            } break;
            case '}':
            {
                printf("End object\n");
            }
            case '[':
            {
                printf("Parsing array.\n");
            } break;
            case ']':
            {
                printf("End array.\n");
            } break;
            case '"':
            {
                char* str = ParseString(file);
                printf("%s ", str);
            } break;
            case 't':
            {   
                bool b = ParseBoolean(file);
                printf("Value: %d\n", b);
            } break;
            case 'f':
            {
                bool b = ParseBoolean(file);
                printf("Value: %d\n", b);
            } break;
            case 'n':
            {
                printf("Parsing null.\n");
            }
            case ':':
            {
                printf("= ");
            } break;
            default:
            {
                Digit digit = ParseDigit(file, token);
                if (digit.isFloat)
                {   
                    char* endptr;
                    float f = std::strtof(digit.value, &endptr);
                    if(endptr == digit.value || *endptr != '\0') 
                        printf("ERROR::failed to parse float: %s\n", digit.value);
                    else
                        printf("%f\n", f);
                } else 
                {
                    int i = std::stoi(digit.value);
                    printf("%d\n", i);
                }
            }
        }

    }

}
