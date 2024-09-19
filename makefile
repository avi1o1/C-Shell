# Compiler
CC = gcc

# Directories
SRC_DIR = code
OUT_DIR = .

# Output file name
OUT_FILE = $(OUT_DIR)/avilol

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(SRCS:.c=.o)

# Compiler flags
CFLAGS = -Wall -Wextra -g -O0

# Default target to build and run the executable
all: run

# Rule to build the final executable
$(OUT_FILE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile the .c files into .o files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the executable
run: $(OUT_FILE)
	@./$(OUT_FILE)

# Clean up the generated files
clean:
	rm -f $(OBJS) $(OUT_FILE)

# Phony targets
.PHONY: clean run

