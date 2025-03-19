# WeakSpot: Weak Memory Model Checker

WeakSpot is a model checker for weak memory models, designed to identify potential weak spots in programs that may exhibit incorrect behavior under different memory consistency models. It supports:
- **Sequential Consistency (SC)**
- **Total Store Order (TSO)**
- **Partial Store Order (PSO)**
- **Release-Acquire (RA)**

It executes programs written in a custom assembly-like language and allows for both interactive execution and exhaustive model checking.

## Features
- **Interactive execution**: Step through execution by choosing thread actions manually.
- **Model checking**: Explore all possible executions to detect incorrect behaviors.
- **Support for multiple memory models**: Test programs under different consistency models.
- **Docker support**: Easy setup and execution within a container.

## Installation & Usage

The recommended way to run the checker is via Docker:

```bash
git clone https://github.com/mishok2503/wmm-model-checker.git
cd wmm-model-checker
sudo docker build -t model-checker .
sudo docker run -it model-checker
```

Inside container:
```bash
cd /build
./executor --help
./executor --memory-model=ra -i rel_sequence.txt # interactive mode
./executor --memory-model=pso -c dekkers_lock.txt # model checking
```

## Input Format

The input program is written in a custom assembly-like language, where each line contains a single instruction. The tokenizer processes the file line by line, skipping empty lines and stopping at a delimiter line containing `-----`.

### Syntax
Each instruction consists of tokens separated by spaces. The key elements of the language are:

- **Registers**: Named `r0` to `r15`, used for storing values.
- **Labels**: Identified by a token ending with `:`. Used as jump targets.
- **Memory addresses**: Represented as `#rX`, where `rX` contains the address.

### Supported Instructions

#### Arithmetic Operations
- `rX = op rY rZ` — Perform operation `op` (`+`, `-`, `*`, `/`, `^`) on `rY` and `rZ`, storing the result in `rX`.

#### Memory Operations
- `load ORDER #rX rY` — Load value from memory at `#rX` into `rY`.
- `store ORDER #rX rY` — Store value from `rY` into memory at `#rX`.
- `ORDER` can be one of: `RLX`, `REL`, `ACQ`, `REL_ACQ`, `SEQ_CST`.

#### Control Flow
- `if rX goto LABEL` — Jump to `LABEL` if `rX` is nonzero.
- `LABEL:` — Defines a jump target.

#### Synchronization & Atomic Operations
- `fence ORDER` — Memory barrier with the specified order.
- `rX := fai ORDER #rY rZ` — Fetch-and-increment at `#rY` with `ORDER`.
- `rX := cas ORDER #rY rZ rW` — Compare-and-swap at `#rY`.

#### Special Instructions
- `finish` — End execution.
- `fail` — Indicate a failure.

## Interactive Execution

WeakSpot supports an interactive mode where the user can manually step through execution, view memory, and inspect the state of registers.

WIP

## Supported Memory Models

### Sequential Consistency (SC)
SC enforces a strict global order on all operations, ensuring that all threads observe memory modifications in the same order. This is the simplest and most intuitive model but is often expensive to implement in hardware.

### Total Store Order (TSO)
TSO allows stores (writes) to be buffered before they become visible to other threads. However, loads (reads) are not reordered with respect to other loads. This is the model used by x86 processors.

### Partial Store Order (PSO)
PSO extends TSO by allowing writes to be reordered with respect to other writes, meaning a store can become visible to different threads at different times. This provides additional flexibility in memory optimization at the cost of increased complexity.

### Release-Acquire (RA)
RA is a synchronization-focused model that ensures proper ordering when using explicit synchronization primitives like `release` and `acquire`. It allows relaxed accesses but guarantees correct ordering when using synchronization mechanisms.

## License
This project is licensed under the MIT License.
