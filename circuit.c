#include "circuit.h"

#include <stdint.h>
#include <stdlib.h>

void circuit_init(Circuit* circuit)
{
    if (circuit == NULL) {
        return;
    }

    circuit->gates = NULL;
    circuit->gate_count = 0;
    circuit->capacity = 0;
}

void circuit_destroy(Circuit* circuit)
{
    size_t index = 0;

    if (circuit == NULL) {
        return;
    }

    for (index = 0; index < circuit->gate_count; index++) {
        gate_destroy(circuit->gates[index]);
    }

    free(circuit->gates);
    circuit->gates = NULL;
    circuit->gate_count = 0;
    circuit->capacity = 0;
}

int circuit_add_gate(Circuit* circuit, Gate* gate)
{
    Gate** resized = NULL;
    size_t next_capacity = 0;

    if (circuit == NULL || gate == NULL) {
        return 0;
    }

    if (circuit->gate_count == circuit->capacity) {
        if (circuit->capacity == 0) {
            next_capacity = 8;
        } else {
            next_capacity = circuit->capacity * 2;
        }

        if (next_capacity < circuit->capacity) {
            return 0;
        }
        if (next_capacity > SIZE_MAX / sizeof(Gate*)) {
            return 0;
        }

        resized = realloc(circuit->gates, next_capacity * sizeof(Gate*));
        if (resized == NULL) {
            return 0;
        }

        circuit->gates = resized;
        circuit->capacity = next_capacity;
    }

    circuit->gates[circuit->gate_count] = gate;
    circuit->gate_count++;
    return 1;
}

Gate* circuit_gate_at(const Circuit* circuit, size_t index)
{
    if (circuit == NULL || index >= circuit->gate_count) {
        return NULL;
    }

    return circuit->gates[index];
}
