#ifndef GATE_H
#define GATE_H

#include <stddef.h>

#include "pin.h"

typedef enum {
    GATE_AND,
    GATE_OR,
    GATE_NOT,
    GATE_NAND,
    GATE_NOR,
    GATE_XOR
} GateType;

typedef struct Gate Gate;

struct Gate {
    GateType type;
    char name[32];
    InputPin* inputs;
    size_t input_count;
    OutputPin* outputs;
    size_t output_count;
    void (*evaluate)(struct Gate*);
};

Gate* gate_create_and(const char* name, size_t input_count);
Gate* gate_create_or(const char* name, size_t input_count);
Gate* gate_create_not(const char* name);
Gate* gate_create_nand(const char* name, size_t input_count);
Gate* gate_create_nor(const char* name, size_t input_count);
Gate* gate_create_xor(const char* name, size_t input_count);

void gate_destroy(Gate* gate);
const char* gate_type_name(GateType type);

#endif
