#include <stdio.h>
#include <stdlib.h>
#include "value.h"

void value_print(Value v) {
    switch (v.type) {
        case VAL_INT: printf("%d", v.int_val); break;
        case VAL_FLOAT: printf("%f", v.float_val); break;
        case VAL_STRING: printf("%s", v.string_val); break;
        case VAL_NONE: printf("None"); break;
    }
}

void value_free(Value v) {
    if (v.type == VAL_STRING && v.string_val) {
        free(v.string_val);
    }
}
