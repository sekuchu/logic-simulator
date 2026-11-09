#ifndef PIN_H
#define PIN_H

#include <stddef.h>

typedef enum {
    SIGNAL_LOW = 0,
    SIGNAL_HIGH = 1,
    SIGNAL_HIGH_Z = 2,
    SIGNAL_UNKNOWN_X = 3
} SignalState;

typedef struct InputPin InputPin;
typedef struct OutputPin OutputPin;

struct OutputPin {
    char name[32];
    SignalState front_buffer;
    SignalState back_buffer;
    InputPin** subscribers;
    size_t subscriber_count;
};

struct InputPin {
    char name[32];
    OutputPin* source_pin;
};

OutputPin* pin_default_off(void);

void pin_output_init(OutputPin* pin, const char* name);
void pin_input_init(InputPin* pin, const char* name);

void pin_output_set(OutputPin* pin, SignalState state);
void pin_output_set_back(OutputPin* pin, SignalState state);
void pin_output_commit(OutputPin* pin);
SignalState pin_read(const InputPin* pin);

int pin_output_attach(OutputPin* pin, InputPin* subscriber);
void pin_output_detach(OutputPin* pin, InputPin* subscriber);
void pin_output_detach_all(OutputPin* pin);
void pin_output_release(OutputPin* pin);

int pin_input_connect(InputPin* pin, OutputPin* source);
void pin_input_disconnect(InputPin* pin);

const char* signal_state_name(SignalState state);

#endif
