# Base system (empty Linux)
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    clang \
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
    clang -x c "$TARGET_FILE" -c -o target.o && \
    clang++ -std=c++17 coverage.cpp -c -o coverage.o && \
    clang++ target.o coverage.o -o "target/$TARGET_NAME"

# Build fuzzer
RUN g++ -std=c++17 \
    main.cpp mutator.cpp executor.cpp corpus.cpp coverage.cpp helpers.cpp \
    -o fuzzer

#  Make sure seed folder exists
RUN mkdir -p seed && echo "test" > seed/hello

# Default command when container runs
CMD ["./fuzzer"]