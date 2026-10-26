# Weak Memory Model Checker

A C++20 executor and model checker for concurrent programs under weak memory models. The command-line interface accepts `seq_cst`, `tso`, `pso`, `ra` and `sra` memory model names.

## Build locally

Prerequisites: a C++20-capable compiler, CMake 3.14 or newer, Git and network access to fetch Catch2.

```bash
git clone https://github.com/rishavranjan60/wmm-model-checker.git
cd wmm-model-checker
cmake -S . -B build -DENABLE_TESTS=ON
cmake --build build
```

## Run

From the repository root on a Unix-like system:

```bash
./build/executor --help
./build/executor --memory-model=ra -i examples/rel_sequence.txt
./build/executor --memory-model=pso -c examples/dekkers_lock.txt
```

`-i` enables interactive execution; `-c` explores executions for model checking. Example input programs are in `examples/`.

## Tests

The CMake test target is named `test`. After building, run its executable directly:

```bash
./build/tests/test
```

Multi-configuration generators may place executables in an additional configuration directory such as `Debug/`.

## Docker

Build the image and run the test executable:

```bash
docker build -t model-checker .
docker run --rm model-checker
```

For the command-line help, override the default test command with `docker run --rm model-checker ./executor --help`.

## Related repository

The previous installation instructions referenced [mishok2503/wmm-model-checker](https://github.com/mishok2503/wmm-model-checker). The clone command above checks out this repository.
