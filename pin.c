#include "pin.h"

#include <stdint.h>
#include <stdlib.h>

static OutputPin DEFAULT_OFF = {
    "default_off",
    SIGNAL_LOW,
    SIGNAL_LOW,
    NULL,
    0
};

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

OutputPin* pin_default_off(void)
{
    return &DEFAULT_OFF;
}

void pin_output_init(OutputPin* pin, const char* name)
{
    if (pin == NULL) {
        return;
    }

    copy_name(pin->name, name);
    pin->front_buffer = SIGNAL_LOW;
    pin->back_buffer = SIGNAL_LOW;
    pin->subscribers = NULL;
    pin->subscriber_count = 0;
}

void pin_input_init(InputPin* pin, const char* name)
{
    if (pin == NULL) {
        return;
    }

    copy_name(pin->name, name);
    pin->source_pin = &DEFAULT_OFF;
}

void pin_output_set(OutputPin* pin, SignalState state)
{
    if (pin == NULL) {
        return;
    }

    pin->front_buffer = state;
    pin->back_buffer = state;
}

void pin_output_set_back(OutputPin* pin, SignalState state)
{
    if (pin == NULL) {
        return;
    }

    pin->back_buffer = state;
}

void pin_output_commit(OutputPin* pin)
{
    if (pin == NULL) {
        return;
    }

    pin->front_buffer = pin->back_buffer;
}

SignalState pin_read(const InputPin* pin)
{
    return pin->source_pin->front_buffer;
}

int pin_output_attach(OutputPin* pin, InputPin* subscriber)
{
    InputPin** resized = NULL;
    size_t index = 0;
    size_t next_count = 0;

    if (pin == NULL || subscriber == NULL || pin == &DEFAULT_OFF) {
        return 0;
    }

    for (index = 0; index < pin->subscriber_count; index++) {
        if (pin->subscribers[index] == subscriber) {
            return 1;
        }
    }

    if (pin->subscriber_count == SIZE_MAX) {
        return 0;
    }
    next_count = pin->subscriber_count + 1;
    if (next_count > SIZE_MAX / sizeof(InputPin*)) {
        return 0;
    }

    resized = realloc(pin->subscribers, next_count * sizeof(InputPin*));
    if (resized == NULL) {
        return 0;
    }

    pin->subscribers = resized;
    pin->subscribers[pin->subscriber_count] = subscriber;
    pin->subscriber_count = next_count;
    return 1;
}

void pin_output_detach(OutputPin* pin, InputPin* subscriber)
{
    size_t index = 0;
    int found = 0;

    if (pin == NULL || subscriber == NULL) {
        return;
    }

    for (index = 0; index < pin->subscriber_count; index++) {
        if (pin->subscribers[index] == subscriber) {
            found = 1;
            break;
        }
    }

    if (found) {
        for (; index + 1 < pin->subscriber_count; index++) {
            pin->subscribers[index] = pin->subscribers[index + 1];
        }
        pin->subscriber_count--;
    }

    if (subscriber->source_pin == pin) {
        subscriber->source_pin = &DEFAULT_OFF;
    }
}

void pin_output_release(OutputPin* pin)
{
    if (pin == NULL) {
        return;
    }

    free(pin->subscribers);
    pin->subscribers = NULL;
    pin->subscriber_count = 0;
}

int pin_input_connect(InputPin* pin, OutputPin* source)
{
    if (pin == NULL) {
        return 0;
    }

    if (source == NULL) {
        source = &DEFAULT_OFF;
    }

    if (pin->source_pin == source) {
        return 1;
    }

    pin_input_disconnect(pin);

    if (source == &DEFAULT_OFF) {
        return 1;
    }

    if (!pin_output_attach(source, pin)) {
        return 0;
    }

    pin->source_pin = source;
    return 1;
}

void pin_input_disconnect(InputPin* pin)
{
    OutputPin* source = NULL;

    if (pin == NULL) {
        return;
    }

    source = pin->source_pin;
    if (source != NULL && source != &DEFAULT_OFF) {
        pin_output_detach(source, pin);
    }
    pin->source_pin = &DEFAULT_OFF;
}

const char* signal_state_name(SignalState state)
{
    switch (state) {
    case SIGNAL_LOW:
        return "LOW";
    case SIGNAL_HIGH:
        return "HIGH";
    case SIGNAL_HIGH_Z:
        return "HIGH_Z";
    case SIGNAL_UNKNOWN_X:
        return "UNKNOWN_X";
    default:
        return "UNKNOWN_X";
    }
}
