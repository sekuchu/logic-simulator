#include "gate.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void copy_name(char destination[32], const char* source)
{
    size_t index = 0;

    if (source == NULL) {
        destination[0] = '\0';
        return;
    }

    while (source[index] != '\0' && index < 31) {
        destination[index] = source[index];
        index++;
    }
    destination[index] = '\0';
}

static int is_unknown(SignalState state)
{
    return state == SIGNAL_HIGH_Z || state == SIGNAL_UNKNOWN_X;
}

static SignalState invert(SignalState state)
{
    if (state == SIGNAL_LOW) {
        return SIGNAL_HIGH;
    }
    if (state == SIGNAL_HIGH) {
        return SIGNAL_LOW;
    }
    return SIGNAL_UNKNOWN_X;
}

static SignalState and_value(Gate* gate)
{
    size_t index = 0;
    SignalState result = SIGNAL_HIGH;

    for (index = 0; index < gate->input_count; index++) {
        SignalState current = pin_read(&gate->inputs[index]);

        if (current == SIGNAL_LOW) {
            return SIGNAL_LOW;
        }
        if (is_unknown(current)) {
            result = SIGNAL_UNKNOWN_X;
        }
    }

    return result;
}

static SignalState or_value(Gate* gate)
{
    size_t index = 0;
    SignalState result = SIGNAL_LOW;

    for (index = 0; index < gate->input_count; index++) {
        SignalState current = pin_read(&gate->inputs[index]);

        if (current == SIGNAL_HIGH) {
            return SIGNAL_HIGH;
        }
        if (is_unknown(current)) {
            result = SIGNAL_UNKNOWN_X;
        }
    }

    return result;
}

static SignalState xor_value(Gate* gate)
{
    size_t index = 0;
    size_t high_count = 0;

    for (index = 0; index < gate->input_count; index++) {
        SignalState current = pin_read(&gate->inputs[index]);

        if (is_unknown(current)) {
            return SIGNAL_UNKNOWN_X;
        }
        if (current == SIGNAL_HIGH) {
            high_count++;
        }
    }

    return (high_count % 2 == 0) ? SIGNAL_LOW : SIGNAL_HIGH;
}

static void evaluate_and(Gate* gate)
{
    if (gate == NULL || gate->output_count == 0) {
        return;
    }

    pin_output_set_back(&gate->outputs[0], and_value(gate));
}

static void evaluate_or(Gate* gate)
{
    if (gate == NULL || gate->output_count == 0) {
        return;
    }

    pin_output_set_back(&gate->outputs[0], or_value(gate));
}

static void evaluate_not(Gate* gate)
{
    if (gate == NULL || gate->output_count == 0 || gate->input_count == 0) {
        return;
    }

    pin_output_set_back(&gate->outputs[0], invert(pin_read(&gate->inputs[0])));
}

static void evaluate_nand(Gate* gate)
{
    if (gate == NULL || gate->output_count == 0) {
        return;
    }

    pin_output_set_back(&gate->outputs[0], invert(and_value(gate)));
}

static void evaluate_nor(Gate* gate)
{
    if (gate == NULL || gate->output_count == 0) {
        return;
    }

    pin_output_set_back(&gate->outputs[0], invert(or_value(gate)));
}

static void evaluate_xor(Gate* gate)
{
    if (gate == NULL || gate->output_count == 0) {
        return;
    }

    pin_output_set_back(&gate->outputs[0], xor_value(gate));
}

const char* gate_type_name(GateType type)
{
    switch (type) {
    case GATE_AND:
        return "AND";
    case GATE_OR:
        return "OR";
    case GATE_NOT:
        return "NOT";
    case GATE_NAND:
        return "NAND";
    case GATE_NOR:
        return "NOR";
    case GATE_XOR:
        return "XOR";
    default:
        return "UNKNOWN";
    }
}

static Gate* gate_create_internal(GateType type,
                                  const char* name,
                                  size_t input_count,
                                  void (*evaluate)(struct Gate*))
{
    Gate* gate = NULL;
    char pin_name[32];
    size_t index = 0;

    if (input_count == 0 || input_count > SIZE_MAX / sizeof(InputPin)) {
        return NULL;
    }

    gate = calloc(1, sizeof(Gate));
    if (gate == NULL) {
        return NULL;
    }

    gate->inputs = calloc(input_count, sizeof(InputPin));
    gate->outputs = calloc(1, sizeof(OutputPin));
    if (gate->inputs == NULL || gate->outputs == NULL) {
        free(gate->inputs);
        free(gate->outputs);
        free(gate);
        return NULL;
    }

    gate->type = type;
    copy_name(gate->name, name);
    if (gate->name[0] == '\0') {
        copy_name(gate->name, gate_type_name(type));
    }
    gate->input_count = input_count;
    gate->output_count = 1;
    gate->evaluate = evaluate;

    for (index = 0; index < input_count; index++) {
        snprintf(pin_name, sizeof(pin_name), "in%zu", index);
        pin_input_init(&gate->inputs[index], pin_name);
    }
    pin_output_init(&gate->outputs[0], "out");

    return gate;
}

Gate* gate_create_and(const char* name, size_t input_count)
{
    return gate_create_internal(GATE_AND, name, input_count, evaluate_and);
}

Gate* gate_create_or(const char* name, size_t input_count)
{
    return gate_create_internal(GATE_OR, name, input_count, evaluate_or);
}

Gate* gate_create_not(const char* name)
{
    return gate_create_internal(GATE_NOT, name, 1, evaluate_not);
}

Gate* gate_create_nand(const char* name, size_t input_count)
{
    return gate_create_internal(GATE_NAND, name, input_count, evaluate_nand);
}

Gate* gate_create_nor(const char* name, size_t input_count)
{
    return gate_create_internal(GATE_NOR, name, input_count, evaluate_nor);
}

Gate* gate_create_xor(const char* name, size_t input_count)
{
    return gate_create_internal(GATE_XOR, name, input_count, evaluate_xor);
}

void gate_destroy(Gate* gate)
{
    size_t index = 0;

    if (gate == NULL) {
        return;
    }

    for (index = 0; index < gate->input_count; index++) {
        pin_input_disconnect(&gate->inputs[index]);
    }

    for (index = 0; index < gate->output_count; index++) {
        pin_output_detach_all(&gate->outputs[index]);
        pin_output_release(&gate->outputs[index]);
    }

    free(gate->inputs);
    free(gate->outputs);
    free(gate);
}
