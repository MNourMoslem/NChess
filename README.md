# NChess
NChess is a chess library designed for machine learning applications. It is written in both C and Python, providing a fast environment to train models efficiently. The library supports both Python bindings for ease of use and a C-only version for users who prefer working directly in C.

## Features
* Fast Chess Simulation: Built with performance in mind using C.
* Machine Learning Friendly: Ideal for integrating with ML workflows.
* Python Bindings: Easily accessible through Python for rapid development.
* C-Only Version: For use in non-Python environments.

## Installation
To install the Python package, use pip:

```bash
pip install nchess
```

For the C-only version, clone the c-nchess folder and use the makefile:
```bash
make
```
To build with debugging:
```bash
make debug
```
The makefile is written for GCC. If you wish to use another compiler, you may need to modify it manually.

## Example Usage
There is no formal documentation for NChess, but all classes and functions are described in Python comments within their respective files.
Here’s an example usage in a Python:

```python
import nchess as nc

board = nc.Board()
print(board)

# out:
# rnbqkbnr
# pppppppp
# ........
# ........
# ........
# ........
# PPPPPPPP
# RNBQKBNR

```

## License
This project is licensed under the MIT License.
