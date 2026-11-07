#include <stdio.h>

#include "circuit.h"

static void report_state(size_t tick,
                         const OutputPin* input_a,
                         const OutputPin* input_b,
                         const Gate* and_gate,
                         const Gate* not_gate)
{
    printf("t=%zu A=%s B=%s and=%s not=%s\n",
           tick,
           signal_state_name(input_a->front_buffer),
           signal_state_name(input_b->front_buffer),
           signal_state_name(and_gate->outputs[0].front_buffer),
           signal_state_name(not_gate->outputs[0].front_buffer));
}

int main(void)
{
    Circuit circuit;
    OutputPin input_a;
    OutputPin input_b;
    Gate* and_gate = NULL;
    Gate* not_gate = NULL;
    int status = 1;

    circuit_init(&circuit);
    pin_output_init(&input_a, "A");
    pin_output_init(&input_b, "B");

    and_gate = gate_create_and("and1", 2);
    if (and_gate == NULL) {
        goto cleanup;
    }

    if (!circuit_add_gate(&circuit, and_gate)) {
        gate_destroy(and_gate);
        and_gate = NULL;
        goto cleanup;
    }

    not_gate = gate_create_not("not1");
    if (not_gate == NULL) {
        goto cleanup;
    }

    if (!circuit_add_gate(&circuit, not_gate)) {
        gate_destroy(not_gate);
        not_gate = NULL;
        goto cleanup;
    }

    if (!circuit_connect(&input_a, &and_gate->inputs[0]) ||
        !circuit_connect(&input_b, &and_gate->inputs[1]) ||
        !circuit_connect(&and_gate->outputs[0], &not_gate->inputs[0])) {
        goto cleanup;
    }

    circuit_tick(&circuit);
    report_state(0, &input_a, &input_b, and_gate, not_gate);

    pin_output_set(&input_a, SIGNAL_HIGH);
    circuit_tick(&circuit);
    report_state(1, &input_a, &input_b, and_gate, not_gate);

    pin_output_set(&input_b, SIGNAL_HIGH);
    circuit_tick(&circuit);
    report_state(2, &input_a, &input_b, and_gate, not_gate);

    circuit_tick(&circuit);
    report_state(3, &input_a, &input_b, and_gate, not_gate);

    pin_output_set(&input_a, SIGNAL_LOW);
    circuit_tick(&circuit);
    report_state(4, &input_a, &input_b, and_gate, not_gate);

    circuit_tick(&circuit);
    report_state(5, &input_a, &input_b, and_gate, not_gate);

    status = 0;

cleanup:
    circuit_destroy(&circuit);
    pin_output_release(&input_a);
    pin_output_release(&input_b);
    return status;
}
