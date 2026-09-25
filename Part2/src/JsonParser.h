
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

struct JsonArray {
    int length;
    JsonValue *elements;
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


JsonObject* ParseJsonObject(FILE *file);
JsonValue* ParseJsonValue(FILE *file);
JsonValue* DeserializeJson(const char* jsonFile);
