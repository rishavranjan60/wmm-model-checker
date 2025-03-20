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

Program launch inside container:
```bash
cd /build
./executor --help
./executor --memory-model=ra -i rel_sequence.txt # interactive mode
./executor --memory-model=pso -c dekkers_lock.txt # model checking
```
Tests launch inside container:
```bash
cd /build/tests
./test # tests launch
```

## Building from Source

If you prefer to build manually:
```
mkdir build && cd build
cmake ..
make
./executor --help
```

## Input Format

The input program is written in a custom assembly-like language, where each line contains a single instruction. The tokenizer processes the file line by line, skipping empty lines and stopping at a delimiter line containing `-----`.

### Syntax
Each instruction consists of tokens separated by spaces. The key elements of the language are:

- **Registers**: Named `r0` to `r15`, used for storing values.
- **Labels**: Identified by a token ending with `:`. Used as jump targets.
- **Memory addresses**: Represented as `#rX`, where `rX` contains the address.

All registers and memory locations are initialized to zero at the beginning of execution. However, **r15 is preloaded with the thread identifier** for each thread.

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

## Supported Memory Models

### Sequential Consistency (SC)

SC enforces a strict global order on all operations, ensuring that all threads observe memory modifications in the same order. This is the simplest and most intuitive model but is often expensive to implement in hardware.

**Implementation:**  
Memory is represented as a simple array that all threads read from and write to. Each operation is executed atomically and immediately visible to all threads in the exact order it was issued.

### Total Store Order (TSO)

TSO allows stores (writes) to be buffered before they become visible to other threads. Reads, however, always execute in program order and are never reordered with respect to each other. This is the memory model used by x86 processors.

**Implementation:**  
Each thread has its own store buffer where writes are temporarily stored before being propagated to main memory. When a thread performs a read, it first checks its store buffer for the most recent write to the requested address. If no such entry is found, the value is read from main memory.

Additionally, **fence** instructions force all pending writes in the store buffer to be committed to memory. However, writes can also be propagated at any moment, even without a fence. This introduces **silent steps** in interactive mode, where instead of executing an instruction, the user can choose to propagate a store buffer entry to memory.

In interactive mode, each thread's state also includes its **memory view**, displaying the contents of its store buffer.

![image](https://github.com/user-attachments/assets/d304f4ca-1516-4a29-87ba-8a1aa76c9434)


### Partial Store Order (PSO)

PSO further relaxes memory ordering by allowing stores (writes) to different memory locations to be reordered. Unlike TSO, where each thread has a single store buffer, in PSO, each memory location has its own independent store buffer per thread. This means that writes to different addresses can be observed by other threads in different orders.

**Implementation:**  
Each thread maintains a separate store buffer for every memory location it writes to. When a thread performs a read, it first checks the corresponding buffer for the latest write. If no entry is found, it reads from main memory.

**Fence** instructions commit all store buffers at once.  
**Fetch-and-increment (fai) and compare-and-swap (cas)** operations first propagate their respective memory location before execution.

In interactive mode silent steps now allow selecting which specific store to propagate.  

### The Release-Acquire (RA) 

In the RA model, memory accesses are annotated with synchronization labels—release for writes and acquire for reads—to control the visibility of memory updates across threads. A release operation guarantees that all preceding memory writes are visible to any thread that later performs a corresponding acquire operation on the same synchronization variable. Conversely, an acquire operation ensures that it sees not only the data it directly reads but also the effects of all preceding releases from other threads.

RA can be classified into two variants based on the strictness of these ordering guarantees:

Strong RA:
In the strong variant, the ordering guarantees are enforced very strictly. When a thread performs a release operation, all memory writes preceding it are guaranteed to be visible to any thread that subsequently performs an acquire. This means that the synchronization boundary established by a release-acquire pair is robust: once a thread completes an acquire operation, it is guaranteed to see the complete set of memory updates that happened before the corresponding release. This strong ordering is essential for ensuring the correctness of many concurrent algorithms where a precise and total view of memory changes is required.

Weak RA:
The weak variant relaxes some of these guarantees to allow for more efficient execution on certain architectures. While a release-acquire pair in weak RA still ensures that a thread performing an acquire will see some of the effects of a release, it does not necessarily enforce a complete and rigid ordering of all intermediate operations. This relaxation means that the visibility of memory updates can be more flexible, potentially allowing some reordering or partial propagation of updates, which can lead to performance optimizations. However, this increased flexibility requires more careful reasoning by developers, as the guarantees provided are less stringent than in strong RA.

Implementation Overview:
Memory is modeled using message memory where each memory cell is a list of messages. Each message holds a value, a copy of a memory view, and a flag indicating if further insertions are allowed before it. A thread’s view is a vector of iterators (timestamps) pointing to the currently visible message for each memory cell.

Current view (cur): The thread’s present snapshot of memory.
Acquire view (acq): Accumulates versions from subsequent loads.
Release view (rel): Captures the view at the time of a store with release semantics.

Load Operation:
When a thread performs a load (with ACQ or REL_ACQ order), it advances the timestamp for the targeted address through its message list, joining (merging) the local acquire view with the view from the new message. For acquire loads, the current view is updated to reflect the acquire view.

Store Operation:
For stores with release semantics, a new message is inserted after the current timestamp in the target memory cell. Before insertion, the operation may advance the timestamp if later messages exist; in the weak RA variant, the implementation may offer an interactive choice (via silent steps) on whether to skip or increase the timestamp. After insertion, the local release view is updated accordingly.

Fence Operations:
Fences synchronize the views. A release fence will update the release view to match the current view, while an acquire fence updates the current view from the acquire view. More strict fences (REL_ACQ and SEQ_CST) combine these steps and, in the case of SEQ_CST, also update the global view.

Differences Between Strong and Weak RA:

Strong RA:
Enforces a more rigid ordering by automatically propagating timestamps and ensuring that no interactive intervention can delay the advancement of the memory view. The store and load operations proceed deterministically with full synchronization via fences.

Weak RA:
Allows some flexibility in the ordering. For instance, during a store operation, if there is a subsequent message in the memory cell and it isn’t “hooked” (fixed), the implementation may let the user decide whether to insert the new message immediately or to delay the insertion. This interactive choice reflects the “weakness” in ordering, where some reordering is permitted.

<img width="853" alt="image" src="https://github.com/user-attachments/assets/08dfb8fd-01ad-4c37-ac98-7f147e6d01c7" />


## License
This project is licensed under the MIT License.
