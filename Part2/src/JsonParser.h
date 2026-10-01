
#include "Entry.h"

typedef struct JsonValue JsonValue;

enum JsonType {
    Object,
    Array,
    String,
    Boolean,
    Number,
    Null
};

struct JsonString {
    char* value;
    int length;
};

struct JsonPair {
    JsonString *key;
    JsonValue *value;
    JsonPair *next;
};

struct JsonObject {
    int count;
    JsonPair *pairs;
};

struct JsonNumber {
    double dValue;
    int iValue;
    bool isDouble;
};


struct JsonBoolean {
    bool value;
};

struct JsonArrayElement {
    JsonValue *value;
    JsonArrayElement *next;
};

struct JsonArray {
    int length;
    JsonArrayElement *elements;
};

struct JsonValue {
    JsonType type;
    union {
        JsonObject *object;
        JsonArray *array;
        JsonNumber *number;
        JsonString *string;
        JsonBoolean *boolean;
    };
};


JsonValue* DeserializeJson(const char* jsonFile);
void DisplayAST(JsonValue *root);
