# GPU-Accelerated Simplex Solver Data

This repository contains the Netlib LP test problem collection and tools for processing linear programming problems for use with GPU-accelerated simplex solvers.

## Overview

The repository includes:
- **Netlib LP Test Problems**: A comprehensive collection of 94 linear programming test problems from the Netlib repository
- **Normalization Tool**: A C-based utility to normalize MPS files and convert them to different formats
- **Data Formats**: Support for MPS (Mathematical Programming System) format and CSC (Compressed Sparse Column) format

## Repository Structure

```
.
├── netlib/                  # Netlib LP test problem collection
│   ├── mps/                 # Original MPS format files
│   ├── normalized/          # Normalized MPS files
│   ├── csc/                 # CSC format files
│   ├── emps/                # Compressed format files
│   ├── presolved/           # Presolved problem files
│   ├── src/                 # C source code for normalization tool
│   ├── scripts/             # Utility scripts
│   ├── README.md            # Netlib-specific documentation
│   ├── problems.txt         # List of all problems with statistics
│   ├── bound-types.txt      # Bound types for each problem
│   └── Makefile             # Build configuration
└── README.md                # This file
```

## Netlib LP Test Problems

The Netlib LP collection contains 94 linear programming problems of varying sizes and characteristics:

- **Small problems**: AFIRO (28 rows, 32 columns)
- **Large problems**: FIT2D (26 rows, 10,500 columns), FIT2P (3,001 rows, 13,525 columns)
- **Different bound types**: Upper bounds (UP), lower bounds (LO), fixed variables (FX), free variables (FR)

For a complete list of problems with their dimensions, nonzeros, and optimal values, see `netlib/problems.txt`.

### Data Formats

#### MPS Format
The Mathematical Programming System (MPS) format is a standard text-based format for representing linear programming problems. The original MPS files are stored in the `netlib/mps/` directory.

#### CSC Format
The Compressed Sparse Column (CSC) format is a more efficient representation for sparse matrices, particularly suited for GPU processing.

## Building the Normalization Tool

### Prerequisites
- GCC compiler
- Make

### Build Instructions

```bash
cd netlib
make all
```

This will compile the normalization tool and place the binary in `netlib/bin/normalize`.

## Usage

### Normalizing a Single File

To normalize a single MPS file:

```bash
./netlib/bin/normalize input.mps output.mps
```

To convert to CSC format:

```bash
./netlib/bin/normalize --csc input.mps output.csc
```

### Batch Processing

To normalize all files in a directory:

```bash
./netlib/bin/normalize input_directory/ output_directory/
```

To convert a directory of MPS files to CSC format:

```bash
./netlib/bin/normalize --csc input_directory/ output_directory/
```

### Example Workflow

```bash
# Build the tool
cd netlib
make all

# Normalize a single problem
./bin/normalize mps/afiro.mps normalized/afiro.mps

# Convert to CSC format
./bin/normalize --csc mps/afiro.mps csc/afiro.csc

# Process entire directory
./bin/normalize mps/ normalized/
```

## Utility Scripts

The repository includes several utility scripts in `netlib/scripts/`:

- **`profile.sh`**: Performance profiling using dtrace and flame graphs
- **`compare.py`**: Compare different problem representations
- **`compare_all.sh`**: Batch comparison of all problems

### Profiling Example

```bash
cd netlib
./scripts/profile.sh ./bin/normalize mps/afiro.mps normalized/afiro.mps
```

This generates a flame graph in `netlib/log/flamegraph.svg` showing performance hotspots.

## Cleaning Up

To clean build artifacts:

```bash
cd netlib
make clean
```

This removes the `build/` and `bin/` directories.

## About the Netlib Collection

The Netlib LP test set is a widely-used benchmark collection for evaluating linear programming solvers. These problems come from various real-world applications including:

- Operations research
- Economic modeling
- Network optimization
- Resource allocation
- Production planning

The problems vary significantly in:
- Size (rows and columns)
- Density (number of nonzeros)
- Numerical properties
- Presence of special structures

For more detailed information about individual problems, refer to the original Netlib documentation in `netlib/readme.txt`.

## License

This repository contains data from the Netlib repository. Please refer to the original Netlib documentation for licensing and attribution information.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## References

- [Netlib Repository](http://www.netlib.org/)
- [Netlib LP Test Problems](http://www.netlib.org/lp/data/)
- MPS Format: "Advanced Linear Programming" by Bruce A. Murtagh, McGraw-Hill, 1981

## Contact

For questions or issues, please open an issue in this repository.
