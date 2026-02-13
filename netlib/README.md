# Netlib LP Test Problem Collection

This directory contains the Netlib LP (Linear Programming) test problem collection along with tools for processing and converting these problems.

## Directory Structure

- **`mps/`**: Original MPS format files (94 problems)
- **`normalized/`**: Normalized MPS files
- **`csc/`**: Compressed Sparse Column format files
- **`emps/`**: Compressed/encoded format files
- **`presolved/`**: Presolved problem files
- **`src/`**: C source code for the normalization tool
- **`scripts/`**: Utility scripts for processing and profiling
- **`problems.txt`**: Complete list of all 94 problems with statistics
- **`bound-types.txt`**: Bound type information for each problem
- **`readme.txt`**: Original Netlib documentation

## Building

To build the normalization tool:

```bash
make all
```

This creates the `bin/normalize` executable.

## Normalization Tool

The normalization tool processes MPS files and can convert them to different formats.

### Usage

**Single file normalization:**
```bash
./bin/normalize input.mps output.mps
```

**Convert to CSC format:**
```bash
./bin/normalize --csc input.mps output.csc
```

**Directory processing:**
```bash
./bin/normalize input_directory/ output_directory/
./bin/normalize --csc mps/ csc/
```

### What Does Normalization Do?

The normalization process:
1. Parses the MPS file
2. Standardizes the problem representation
3. Optimizes the data structure for efficient processing
4. Optionally converts to CSC format for sparse matrix operations

## Scripts

### `profile.sh`

Performance profiling script using dtrace. Example usage:

```bash
./scripts/profile.sh ./bin/normalize mps/afiro.mps normalized/afiro.mps
```

Generates a flame graph in `log/flamegraph.svg`.

### `compare.py`

Python script to compare different problem representations.

### `compare_all.sh`

Batch comparison script for all problems.

## Problem Statistics

The collection contains 94 problems with the following characteristics:

### Problem Sizes
- **Smallest**: AFIRO (28 rows, 32 columns, 88 nonzeros)
- **Most rows**: FIT2P (3,001 rows, 13,525 columns)
- **Most columns**: FIT2D (26 rows, 10,500 columns)
- **Most nonzeros**: MAROS-R7 (3,137 rows, 9,408 columns, 151,120 nonzeros)

### Bound Types
Problems may contain:
- **UP**: Upper bounds
- **LO**: Lower bounds
- **FX**: Fixed variables
- **FR**: Free variables
- **PL**: Plus infinity (only in PILOT4)

See `bound-types.txt` for which problems use which bound types.

## File Formats

### MPS Format
The MPS (Mathematical Programming System) format is a standard text-based format for LP problems. It contains sections for:
- NAME: Problem name
- ROWS: Constraint types
- COLUMNS: Variable coefficients
- RHS: Right-hand side values
- BOUNDS: Variable bounds
- ENDATA: End marker

### CSC Format
The Compressed Sparse Column format stores sparse matrices efficiently:
- Column pointers array
- Row indices array
- Nonzero values array

This format is particularly efficient for GPU processing.

## About the Netlib Collection

The Netlib LP test set is a standard benchmark collection used worldwide for evaluating and comparing linear programming solvers. These problems come from real-world applications in:

- Transportation and logistics
- Production planning
- Network flow optimization
- Economic modeling
- Resource allocation

## Source

Original problems sourced from: http://www.netlib.org/lp/data/

For complete details about the original collection, see `readme.txt`.

## Cleaning Up

To remove build artifacts:

```bash
make clean
```

This removes the `build/` and `bin/` directories.
