# logic-simulator

Discrete digital logic simulator written in C11.

## Build

make

## Run

make run

## Layout

- `pin.h`, `pin.c`: signal states, input pins, output pins, default off source
- `gate.h`, `gate.c`: gate types, factory functions, evaluation callbacks
- `circuit.h`, `circuit.c`: gate container, wiring, two phase tick engine
- `main.c`: AND into NOT cascade demo
- `Makefile`: build, clean, run targets

## Model

Every gate is allocated separately on the heap. The circuit stores an array of
`Gate*` pointers, so growing the container does not move gate instances.

Each tick runs in two phases. Evaluation reads input front buffers and writes
output back buffers. Commit copies every output back buffer into its front
buffer for the next tick.

Unconnected inputs point at the default off pin. Disconnecting an input also
removes it from the source output subscriber list.

## Demo

The driver toggles two inputs through an AND gate wired into a NOT gate. The
NOT output changes one tick after the AND output, which shows the staged
evaluation and commit behavior.
