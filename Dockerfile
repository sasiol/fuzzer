# Base system (empty Linux)
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    clang \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set working directory inside container
WORKDIR /fuzz

# Copy  project files into container
COPY . .

# build target automatically
RUN set -e && \
    TARGET_FILE=$(find target -maxdepth 1 -name "*.c" | head -n 1) && \
    test -n "$TARGET_FILE" && \
    TARGET_NAME=$(basename "$TARGET_FILE" .c) && \
    echo "Found target: $TARGET_FILE" && \
    clang++ "$TARGET_FILE" coverage.cpp \
        -o "target/$TARGET_NAME" \
        -fsanitize-coverage=trace-pc-guard \
        -O0 -g

# Build fuzzer
RUN g++ -std=c++17 \
    main.cpp mutator.cpp executor.cpp corpus.cpp coverage.cpp \
    -o fuzzer

#  Make sure seed folder exists
RUN mkdir -p seed && echo "test" > seed/hello

# Default command when container runs
CMD ["./fuzzer"]