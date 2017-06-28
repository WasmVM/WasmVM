# Wasm_interpret

###### Run a wasm program on local environment.
---

## How to play 
- Step 1: [install guideline : wabt](https://github.com/WebAssembly/wabt)
  - You can go here and download the source code of wabt
  - And then compile the exec file for your environment.

- Step 2: After you finish compiling work, using wast2wasm (mostly in wabt/out/...) to translate your wast code into wasm.

- Step 3: clone our repository and go into the repo root and run `make`, and generate exec file - `main`

- Step 4: usage of our program:
  - `./main <your wasm program file>`: you can see what's going on.
  
## Current support
- `NumericInst.h`: the instruction we have implemented.
- `Decode.cpp`: the instruction we support.
