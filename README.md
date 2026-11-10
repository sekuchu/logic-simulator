# logic-simulator

**A small digital circuit simulator, written in C11.**

Gates are independent objects. Pins carry signals. Circuits connect them and advance their state one tick at a time.

No graphical interface or visual editor. Just the logic, the wiring, and what happens when the circuit runs.

### Getting it running

```bash
make
make run
```

### What's inside

| File | Purpose |
|---|---|
| `pin.h / pin.c` | Signal states, input and output pins, default OFF source |
| `gate.h / gate.c` | Gate definitions, factory functions and evaluation callbacks |
| `circuit.h / circuit.c` | Gate storage, connections and tick execution |
| `main.c` | AND → NOT circuit demonstration |
| `Makefile` | Build, run and clean |

### The interesting part: time

The simulator doesn't evaluate everything and immediately overwrite its current state.

Each tick is split into two stages:

1. **Evaluate:** Gates read their inputs from front buffers and write their results into back buffers.
2. **Commit:** All output back buffers become the new front buffers.

This means a gate cannot see another gate's newly calculated output during the same tick. Changes propagate through the circuit over successive ticks.

Every gate also lives in its own heap allocation. The circuit keeps an array of `Gate*` pointers, allowing the container to grow without relocating existing gate instances.

Unconnected inputs receive a default OFF signal. Disconnecting an input also removes its subscription from the source output, keeping the wiring relationships consistent.

### See it in action

The included demonstration connects two inputs to an AND gate, then routes its output into a NOT gate.

The driver changes the input signals and advances the circuit. The AND gate reacts first. The NOT gate follows one tick later.

A small example, but it demonstrates the central idea behind the simulator: **logic has state, and state advances in steps.**
