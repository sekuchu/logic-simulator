#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stddef.h>

#include "gate.h"

typedef struct Circuit {
    Gate** gates;
    size_t gate_count;
    size_t capacity;
} Circuit;

void circuit_init(Circuit* circuit);
void circuit_destroy(Circuit* circuit);
int circuit_add_gate(Circuit* circuit, Gate* gate);
Gate* circuit_gate_at(const Circuit* circuit, size_t index);

#endif
