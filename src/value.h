#ifndef VALUE_H
#define VALUE_H

typedef enum {
    VAL_NONE,
    VAL_INT,
    VAL_FLOAT,
    VAL_STRING
} ValueType;

typedef struct {
    ValueType type;
    union {
        int int_val;
        double float_val;
        char *string_val;
    };
} Value;

void value_print(Value v);
void value_free(Value v);

#endif
