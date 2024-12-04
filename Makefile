# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -O2 -fopenmp

# Target executables
TARGETS = sdp_serial sdp_mm_parallel sdp_bmm sdp_online_softmax sdp_fused

# Source files
SRC_SERIAL = sdp_serial.cpp
SRC_MM_PARALLEL = sdp_mm_parallel.cpp
SRC_BMM = sdp_bmm.cpp
SRC_ONLINE_SOFTMAX = sdp_online_softmax.cpp
SRC_FUSED = sdp_fused.cpp

# Default target: compile all executables
all: $(TARGETS)

# Rules for compiling each target
sdp_serial: $(SRC_SERIAL)
	$(CXX) $(CXXFLAGS) -o sdp_serial $(SRC_SERIAL)

sdp_mm_parallel: $(SRC_MM_PARALLEL)
	$(CXX) $(CXXFLAGS) -o sdp_mm_parallel $(SRC_MM_PARALLEL)

sdp_bmm: $(SRC_BMM)
	$(CXX) $(CXXFLAGS) -o sdp_bmm $(SRC_BMM)

sdp_online_softmax: $(SRC_ONLINE_SOFTMAX)
	$(CXX) $(CXXFLAGS) -o sdp_online_softmax $(SRC_ONLINE_SOFTMAX)

sdp_fused: $(SRC_FUSED)
	$(CXX) $(CXXFLAGS) -o sdp_fused $(SRC_FUSED)


run: run_serial run_parallel run_bmm run_online_softmax run_fused

# Run individual executables
run_serial: sdp_serial
	./sdp_serial

run_parallel: sdp_mm_parallel
	./sdp_mm_parallel

run_bmm: sdp_bmm
	./sdp_bmm

run_online_softmax: sdp_online_softmax
	./sdp_online_softmax

run_fused: sdp_fused
	./sdp_fused

# Clean up the build files
clean:
	rm -f $(TARGETS)

.PHONY: all clean
