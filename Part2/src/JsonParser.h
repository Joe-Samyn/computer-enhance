
#include "Entry.h"

enum JsonType {
    Object,
    Array,
    String,
    Boolean,
    Number,
    Null
};

struct JsonValue {
    JsonType type;
    union {
        JsonObject object;
        JsonArray array;
        JsonNumber number;
        JsonString string;
    } value;
};

struct JsonPair {
    char* key;
    JsonValue* value;
};

struct JsonObject {
    JsonPair *pairs;
};

struct JsonArray {
    JsonValue *values;
    int length; 
};

struct JsonNumber {
    double dValue;
    int iValue;
    bool isDouble;
};

struct JsonString {
    char* value;
    int length;
};

struct JsonBoolean {
    bool value;
};


void DeserializePairs(const char* jsonFile, Pairs &pairs);
Entry DeserialzeEntry(const char* json);
