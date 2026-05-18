# C++ Coverage-Guided Fuzzer

A lightweight coverage-guided fuzzer written in C++.

This project implements the core concepts behind modern fuzzing: mutation-based input generation,
Clang sanitizer coverage instrumentation, shared memory coverage tracking, corpus management, and crash detection — all in a small,
readable codebase.

## Features

- **Two fuzzing modes** — random and coverage-guided
- **Coverage tracking** via Clang `-fsanitize-coverage=trace-pc-guard` instrumentation
- **Corpus management** — inputs that trigger new code paths are saved and prioritised
- **Crash detection** — SIGSEGV and SIGABRT are detected and the crashing input is saved
- **Timeout handling** — targets that hang are killed after a fixed time window
- **Clean shutdown** — Ctrl+C and Docker stop both clean up shared memory safely
- **CI pipeline** — GitHub Actions builds the image and runs a Trivy security scan on each push

## Running with Docker (recommended)
### Adding your own target


Place your target source file (`.c` or `.cpp`) in the `target/` directory.
(An example target file is already included.)

Only one target file is supported at a time.

Place one or more seed input files in the `seed/` directory.
(An example seed file is already included.)

The Dockerfile automatically compiles the target with coverage instrumentation.


### Quick build and run
```bash
docker build -t fuzzer .
docker run -it --name fuzru fuzzer
```
If you get a "name already in use" error, remove the old container first: ```bash docker rm fuzru ```

When the fuzzer starts, select the fuzzing mode and logging mode:

```text
Select Fuzzing mode:
1 = random
2 = coverage guided

Select Logging mode:
1 = normal
2 = debug
```

## Crash Output

Crash files are stored inside the container at:

```
/fuzz/crashes
```

To retrieve crash files after execution, copy them from the container to your host machine:

```bash
docker cp fuzru:/fuzz/crashes ./crashes
```

## Project Structure

```bash
fuzzer/
├── main.cpp        — Main fuzzing loop
├── executor.cpp    — Process isolation, fork/exec, crash detection
├── corpus.cpp      — Corpus loading, input selection, warmup
├── mutator.cpp     — Input mutation strategies
├── coverage.cpp    — Clang sanitizer coverage hooks (shared memory)
├── helpers.cpp     — Shared memory setup, file I/O, signal handling
├── target/         — Place your target source file here
├── seed/           — Place seed input files here
├── crashes/        — Crashing inputs are saved here automatically
├── Dockerfile      — Builds fuzzer and target in Ubuntu 22.04
└── .github/
    └── workflows/
        └── ci.yml  — Builds Docker image and runs Trivy scan on push


```
