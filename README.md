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

For the C-only version, clone the c-nchess folder and use the build.py script:

```bash
python build.py build
```

Available build commands:
```bash
python build.py build        # Build library (release mode)
python build.py build-debug  # Build library (debug mode)
python build.py test         # Compile and run tests
python build.py clean        # Remove build directory
```

You can also chain commands together:
```bash
python build.py clean build test  # Clean, build, then run tests
```

**Note:** The old makefile is still available but deprecated. The build.py script provides better flexibility and cross-platform support.

## Example Usage
There is no formal documentation for NChess, but all classes and functions are described in Python comments within their respective files.
Here’s an example usage in a Python:

### The Board Object

The `Board` object represents a chessboard and provides methods to play moves, retrieve game state information, and generate legal moves. It acts as the core of the nchess library, managing the game flow and enforcing chess rules.

```python
import nchess as nc

# Let's create a new board
board = nc.Board()
print(board)
# rnbqkbnr
# pppppppp
# ........
# ........
# ........
# ........
# PPPPPPPP
# RNBQKBNR

board.step("e2e4")
board.step("d7d5")
board.step("e4d5")

print(board)
# rnbqkbnr
# ppp.pppp
# ........
# ...P....
# ........
# ........
# PPPP.PPP
# RNBQKBNR

# The step function returns True if the move has been played, and False otherwise.
is_played = board.step("g8f6")
print(is_played)
# True

is_played = board.step("f1g2")  # The move is not legal, so it won't be played.
print(is_played)
# False

board.undo()  # Undo the last move ("g8f6").
print(board)  # Will print the same board as before.
# rnbqkbnr
# ppp.pppp
# ........
# ...P....
# ........
# ........
# PPPP.PPP
# RNBQKBNR

b2 = board.copy()  # Copies the board.
print(b2)
# rnbqkbnr
# ppp.pppp
# ........
# ...P....
# ........
# ........
# PPPP.PPP
# RNBQKBNR

board.reset()  # Resets the board, equivalent to undoing all played moves.
print(board)
# rnbqkbnr
# pppppppp
# ........
# ........
# ........
# ........
# PPPPPPPP
# RNBQKBNR

# We can create a board from a specific FEN string by passing it to the Board class.
new_board = nc.Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ")
print(new_board)
# r...k..r
# p.ppqpb.
# bn..pnp.
# ...PN...
# .p..P...
# ..N..Q.p
# PPPBBPPP
# R...K..R

# We can also retrieve the FEN of any given state:
fen = new_board.fen()
print(fen)
# r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1

# You might notice a difference between the input FEN and the generated FEN.
# This is because the input FEN did not contain information about the fifty-move rule
# and the move count, whereas the generated one does.
```

### The Move Object

The `Move` object represents a chess move.  
- In C, it is an `int16` or `short` type.  
- In Python, it is a subclass of `int`:  
```python
class Move(int):
    pass

```

Move Representation
Any function in nchess that takes a move as a parameter accepts it in three forms:

Move or int objects – These need to be created or decoded beforehand.
UCI string format – A standard string representation like "e2e4" or "b7b8q".

Usage Examples:
```python
import nchess as nc

board = nc.Board()

# Creating a move from the Move object
move = nc.Move("e2e4")

# Another way to declare a move using nc.move function
# where you set source and destination squares separately
move2 = nc.move("e2", "e4")

print(move == move2)
# True

# Playing a move
is_played = board.step(move)  # Same as board.step("e2e4")
print(is_played)
# True

# Move object has multiple methods to obtain information about the move
src_sqr = move.from_        # Source square
dst_sqr = move.to_          # Destination square
pro_piece = move.pro_piece  # Promotion piece type (0 if not a promotion)
move_type = move.move_type  # Move type (normal, castling, etc.)

print("Move info:")
print(f"Source square: {src_sqr}")          # 11
print(f"Destination square: {dst_sqr}")     # 27
print(f"Promotion piece type: {pro_piece}") # 0
print(f"Move type: {move_type}")            # 0

# Let's play some additional moves
board.step("d7d5")
board.step("e4d5")
board.step("d8d5")
board.step("b1c3")
board.step("d5a5")

# Get all played moves as a list
played_moves = board.get_played_moves()
print(played_moves)
# [Move("e2e4"), Move("d7d5"), Move("e4d5"), Move("d8d5"), Move("b1c3"), Move("d5a5")]

# Generate a list of legal moves in the current position
legal_moves = board.generate_legal_moves()
print(legal_moves)
# [Move("g1e2"), Move("g1h3"), Move("g1f3"), ..., Move("c3b5"), Move("e1e2")]

# Generate all possible moves from a specific square
legal_moves_g1 = board.get_moves_of("g1")
print(legal_moves_g1)
# [Move("g1e2"), Move("g1h3"), Move("g1f3")]

# Check if a move is legal without playing it
is_legal1 = board.is_move_legal("g1f3")
is_legal2 = board.is_move_legal("d1d2")

print(is_legal1) # True
print(is_legal2) # False
```

### The BitBoard Object
A BitBoard is an integer-based representation of a chessboard, where each bit corresponds to a specific square. It is inherited from int, just like the Move object, and is used for efficient bitwise operations to manipulate and analyze board positions. The BitBoard class provides various methods for bit manipulation, such as counting bits, checking occupancy, and retrieving attacking squares.

```python
import nchess as nc
from nchess import const

# Let's create a BitBoard and explore its functionalities
val = 1234  # 0x4d2 in hex and 0b10011010010 in binary
bb = nc.BitBoard(val)
print(bb)
# BitBoard(0x4d2)

# BitBoard comes with several bit manipulation functions

# Returns the number of set bits in the BitBoard
count = bb.bit_count()  # 5

# Checks if there is more than one bit set in the BitBoard
mt1 = bb.more_than_one()  # True

# Checks if the BitBoard contains exactly two bits
has2bit = bb.has_two_bits()  # False

# Checks if a specific square is contained in the BitBoard
has_g1 = bb.is_filled("g1")
has_b4 = bb.is_filled("b4")
print(has_g1)  # True  
print(has_b4)  # False

# Retrieves all occupied squares in a list format
bb_squares = bb.to_squares()
print(bb_squares)
# [1, 4, 6, 7, 10]

# You can also iterate through the occupied squares
for sqr in bb:
    # Process each occupied square
    continue

# Now, let's see how the BitBoard object is used within the Board class
board = nc.Board()

# Retrieve BitBoard representations of different piece types
wp = board.white_pawns  # Returns a BitBoard representing white pawns
print(wp)
# BitBoard(0xff00)

# Retrieve occupancy BitBoards:
white_occ = board.white_occ  # White pieces
black_occ = board.black_occ  # Black pieces
all_occ   = board.all_occ    # All pieces on the board

print(white_occ)  # BitBoard(0xffff)
print(black_occ)  # BitBoard(0xffff000000000000)
print(all_occ)    # BitBoard(0xffff00000000ffff)

# Checking if a square is attacked
# This function returns a BitBoard representing all squares attacking a given square
# Squares can be passed as strings ("f3") or numbers (e.g., 18)
attackers_bb = board.get_attackers_map(const.F3)
attacking_squares = attackers_bb.to_squares()
print(attacking_squares)
# []

# The list is empty because it's White's turn, and no Black piece is attacking f3 in the starting position.
# Let's check for f6 instead:
attackers_bb = board.get_attackers_map("f6")
attacking_squares = attackers_bb.to_squares(as_set=True)  # Returns a set this time
print(attacking_squares)
# {49, 51, 57}

# A set might be useful for quick lookups
print(const.C5 in attacking_squares)  # False
print(const.G7 in attacking_squares)  # True
```

### Array Conversions

One of the most efficient features of `nchess` is its ability to convert chess boards (`Board`) and bitboards (`BitBoard`) into NumPy arrays with minimal performance overhead. This allows for seamless integration with machine learning models, reinforcement learning algorithms, and other computational tasks requiring matrix operations.

### Converting a Board to a NumPy Array

A `Board` object can be converted into a NumPy array using the `as_array` method. This method returns a binary representation of the board, where each piece type is represented on its respective plane.

```python
import nchess as nc
import numpy as np

board = nc.Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ")
print(board)

arr = board.as_array()
print(arr.shape)  # (12, 64)
print(arr)
```

The default shape of the returned array is `(12, 64)`, representing 12 different piece types over a flattened 64-square board.

You can reshape the output into `(12, 8, 8)` to get a more intuitive 2D representation:

```python
arr2 = board.as_array((12, 8, 8))
print(arr2.shape)  # (12, 8, 8)
```

To return the board as a nested list instead of a NumPy array, set `as_list=True`:

```python
arr2 = board.as_array((12, 8, 8), as_list=True)
print(type(arr2))  # <class 'list'>
```

### Reversing the BitBoard Representation

For applications where the perspective of the bitboard matters (e.g., training a chess model from both player perspectives), as_array supports a reversed flag:

```python
arr3 = board.as_array(reversed=True)
print(arr3)
```

Reversing the bitboard flips the perspective, meaning bits would be read from right to left instead of left to right. For example, the bit representation 1001110 would become 0111001. However, this operation does not alter the order of the piece planes in the (12, 8, 8) array. The first plane will always represent white pawns, followed by white knights, bishops, rooks, queens, and kings, then their black counterparts in the same order.

### Converting a BitBoard to a NumPy Array

Bitboards in `nchess` also support direct conversion to NumPy arrays. For example, to get the white pawns' bitboard as an 8x8 matrix:

```python
bb = board.white_pawns
bb_arr = bb.as_array((8, 8))  # Default shape is (64,)
print(bb_arr)
```

Output:
```
[[0 0 0 0 0 0 0 0]
 [1 1 1 0 0 1 1 1]
 [0 0 0 0 0 0 0 0]
 [0 0 0 1 0 0 0 0]
 [0 0 0 0 1 0 0 0]
 [0 0 0 0 0 0 0 0]
 [0 0 0 0 0 0 0 0]
 [0 0 0 0 0 0 0 0]]
```

### Creating a BitBoard from an Array

We can also create a bitboard from a NumPy array using `bb_from_array`:

```python
our_bb = nc.bb_from_array(bb_arr)
print(bb == our_bb)  # True
```

### Using `as_table` for Piece Representation

If you need a representation of the board where each square contains an integer representing the piece type, use `as_table`:

```python
table = board.as_table(shape=(8, 8))  # Default shape is (64,)
print(table)
```

Example output:
```
[[ 4  0  0  6  0  0  0  4]
 [ 1  1  1  3  3  1  1  1]
 [ 7  0  5  0  0  2  0  0]
 [ 0  0  0  1  0  0  7  0]
 [ 0  0  0  2  1  0  0  0]
 [ 0  7  8  7  0  0  8  9]
 [ 0  9  7 11  7  7  0  7]
 [10  0  0 12  0  0  0 10]]
```

### Additional Functions And nchess.Const
nchess has additional functions outside it main classes (Board, BitBoard, Move)
here are they:

Square Functions
```python
import nchess as nc

sqr = "d3"
row = nc.square_row(sqr) # Retrieves the row index of a given square.
col = nc.square_column(sqr) # Retrieves the column index of a given square.

# Mirrors a square either horizontally or vertically.
m_v = nc.square_mirror(sqr, vertical=True) 
m_h = nc.square_mirror(sqr, vertical=False)

# Converts a UCI square notation (e.g., "e4") to its corresponding index (0-63).
sqr2 = nc.square_from_uci("b7")
print(sqr2) # 54

dist = nc.square_distance(sqr, sqr2) # Computes the Manhattan distance between two squares.
print(dist) # 4
```

BitBoard Functions
```python
import nchess as nc
from nchess import const
import numpy as np

# Initialize square and bitboard
sqr = "d4"
occ = nc.BitBoard(1234567910)
arr = occ.as_array((8, 8))
print(arr)
# [[0 1 1 0 0 1 1 1]
#  [0 1 0 0 0 0 0 0]
#  [0 1 1 0 1 0 0 1]
#  [1 0 0 1 0 0 1 0]
#  [0 0 0 0 0 0 0 0]
#  [0 0 0 0 0 0 0 0]
#  [0 0 0 0 0 0 0 0]
#  [0 0 0 0 0 0 0 0]]

# Create a bitboard from an array. The array can be a NumPy array
# or any Python sequence (list, tuple, etc.). The shape does not matter,
# but it must contain 64 elements.
our_bb = nc.bb_from_array(arr)
print(our_bb == occ)  # True

# We can also create a bitboard from a list of squares.
squares = our_bb.to_squares()
our_bb_from_squares = nc.bb_from_squares(squares)
print(our_bb_from_squares == our_bb)  # True


# Calculate the attack positions of a queen from a given square.
# Similar functions exist for bishop and rook.
queen_attack = nc.bb_queen_attacks(sqr, occ)
print(queen_attack.as_array((8, 8)))
# [[0 1 0 0 0 0 0 1]
#  [0 0 1 0 0 0 1 0]
#  [0 0 0 1 1 1 0 0]
#  [0 0 0 1 0 1 1 0]
#  [0 0 0 1 1 1 0 0]
#  [0 0 1 0 1 0 1 0]
#  [0 1 0 0 1 0 0 1]
#  [1 0 0 0 1 0 0 0]]

# The same applies for other pieces. Note that the attacks happen
# on empty boards, unlike sliding pieces where you must set an empty
# bitboard as the occupancy (e.g., nc.bb_queen_attacks(sqr, 0)).

king_attacks = nc.bb_king_attacks(sqr)
knight_attacks = nc.bb_knight_attacks(sqr)
pawn_attacks = nc.bb_pawn_attacks(sqr, const.WHITE)

# Mask functions return the attacking squares of sliding pieces
# on an empty board, without including the edges.
rook_mask = nc.bb_rook_mask(sqr)
bishop_mask = nc.bb_bishop_mask(sqr)

# Note that these functions don't return a BitBoard directly,
# but they are related to move generation techniques. They could be
# useful for programmers who want to explore such methods.

# Retrieve the magic number for a given square.
magic_rook = nc.bb_rook_magic(sqr)
magic_bishop = nc.bb_bishop_magic(sqr)

# Calculate the relevant index for rook and bishop magic bitboards
# from a given square.
relevant_rook = nc.bb_rook_relevant(sqr)
relevant_bishop = nc.bb_bishop_relevant(sqr)
```

And Lastle here is the nchess.Const variables and methods:
```python
STARTING_FEN= 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'

NO_SIDE = -1
WHITE = 0
BLACK = 1
SIDES_NB = 2

SIDE_NAMES = {
    WHITE : "white",
    BLACK : "black"
}

def side_name(side : int) -> str:
    return SIDE_NAMES[side]

NO_PIECE_TYPE = 0
PAWN = 1
KNIGHT = 2
BISHOP = 3
ROOK = 4
QUEEN = 5
KING = 6
PIECES_TYPE_NB = 7

NO_PIECE = NO_PIECE_TYPE
WHITE_PAWN = PAWN
WHITE_KNIGHT = KNIGHT
WHITE_BISHOP = BISHOP
WHITE_ROOK = ROOK
WHITE_QUEEN = QUEEN
WHITE_KING = KING
BLACK_PAWN = PAWN + PIECES_TYPE_NB - 1
BLACK_KNIGHT = KNIGHT + PIECES_TYPE_NB - 1
BLACK_BISHOP = BISHOP + PIECES_TYPE_NB - 1
BLACK_ROOK = ROOK + PIECES_TYPE_NB - 1
BLACK_QUEEN = QUEEN + PIECES_TYPE_NB - 1
BLACK_KING = KING + PIECES_TYPE_NB - 1
PIECES_NB = BLACK_KING + 1

PIECE_TYPES_NAMES = {
    NO_PIECE_TYPE : "null",
    PAWN : "pawn",
    KNIGHT : "knight",
    BISHOP : "bishop",
    ROOK : "rook",
    QUEEN : "queen",
    KING : "king"
}

PIECE_NAMES = {
    NO_PIECE_TYPE : "null",
    WHITE_PAWN : "white pawn",
    WHITE_KNIGHT : "white knight",
    WHITE_BISHOP : "white bishop",
    WHITE_ROOK : "white rook",
    WHITE_QUEEN : "white queen",
    WHITE_KING : "white king",
    BLACK_PAWN : "black pawn",
    BLACK_KNIGHT : "black knight",
    BLACK_BISHOP : "black bishop",
    BLACK_ROOK : "black rook",
    BLACK_QUEEN : "black queen",
    BLACK_KING : "black king"
}

PIECE_SYMBOLS = "PNBRQKpnbrqk"

PIECE_SYMBOLS_AS_PIECES = {
    "P" : WHITE_PAWN,
    "N" : WHITE_KNIGHT,
    "B" : WHITE_BISHOP,
    "R" : WHITE_ROOK,
    "Q" : WHITE_QUEEN,
    "K" : WHITE_KING,
    "p" : BLACK_PAWN,
    "n" : BLACK_KNIGHT,
    "b" : BLACK_BISHOP,
    "r" : BLACK_ROOK,
    "q" : BLACK_QUEEN,
    "k" : BLACK_KING
}

def piece_type(piece : int) -> int:
    return piece % PIECES_NB

def piece_type_name(piece_type : int) -> str:
    return PIECE_TYPES_NAMES[piece_type]

def piece_name(piece : int) -> str:
    return PIECE_NAMES[piece]

def piece_symbol(piece : int) -> str:
    return PIECE_SYMBOLS[piece]

def piece_from_symbol(symbol : str) -> int:
    return PIECE_SYMBOLS_AS_PIECES[symbol]

def piece_color(piece : int) -> int:
    return piece // PIECES_NB

# squares
H1, G1, F1, E1, D1, C1, B1, A1 =  0,  1,  2,  3,  4,  5,  6,  7
H2, G2, F2, E2, D2, C2, B2, A2 =  8,  9, 10, 11, 12, 13, 14, 15
H3, G3, F3, E3, D3, C3, B3, A3 = 16, 17, 18, 19, 20, 21, 22, 23
H4, G4, F4, E4, D4, C4, B4, A4 = 24, 25, 26, 27, 28, 29, 30, 31
H5, G5, F5, E5, D5, C5, B5, A5 = 32, 33, 34, 35, 36, 37, 38, 39
H6, G6, F6, E6, D6, C6, B6, A6 = 40, 41, 42, 43, 44, 45, 46, 47
H7, G7, F7, E7, D7, C7, B7, A7 = 48, 49, 50, 51, 52, 53, 54, 55
H8, G8, F8, E8, D8, C8, B8, A8 = 56, 57, 58, 59, 60, 61, 62, 63
SQUARES_NB = 64
NO_SQUARE = 65

SQUARE_NAMES = [
    "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1",
    "h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2",
    "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3",
    "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4",
    "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5",
    "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6",
    "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7",
    "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8"
]

def square_name(square : int) -> str:
    return SQUARE_NAMES[square]

CASTLE_WK = 1
CASTLE_WQ = 2
CASTLE_BK = 4
CASTLE_BQ = 8
NO_CASTLE = 0

CASTLE_KINGSIDE = CASTLE_WK | CASTLE_BK
CASTLE_QUEENSIDE = CASTLE_WQ | CASTLE_BQ

CASTLE_NAMES = {
    NO_CASTLE : "-",
    CASTLE_WK : "K",
    CASTLE_WQ : "Q",
    CASTLE_BK : "k",
    CASTLE_BQ : "q",
    CASTLE_WK | CASTLE_WQ : "KQ",
    CASTLE_WK | CASTLE_BK : "Kk",
    CASTLE_WK | CASTLE_BQ : "Kq",
    CASTLE_WQ | CASTLE_BK : "Qk",
    CASTLE_WQ | CASTLE_BQ : "Qq",
    CASTLE_BK | CASTLE_BQ : "kq",
    CASTLE_WK | CASTLE_WQ | CASTLE_BK : "KQk",
    CASTLE_WK | CASTLE_WQ | CASTLE_BQ : "KQq",
    CASTLE_WK | CASTLE_BK | CASTLE_BQ : "Kkq",
    CASTLE_WQ | CASTLE_BK | CASTLE_BQ : "Qkq",
    CASTLE_WK | CASTLE_WQ | CASTLE_BK | CASTLE_BQ : "KQkq"
}

def castle_name(castle : int) -> str:
    return CASTLE_NAMES[castle]

MOVE_NORMAL = 0
MOVE_CASTLE = 1
MOVE_EN_PASSANT = 2
MOVE_PROMOTION = 3

MOVE_NAMES = {
    MOVE_NORMAL : "normal",
    MOVE_CASTLE : "castle",
    MOVE_EN_PASSANT : "en_passant",
    MOVE_PROMOTION : "promotion",
}

def move_name(move : int) -> str:
    return MOVE_NAMES[move]

STATE_PLAYING = 0
STATE_WHITE_WIN = 1
STATE_BLACK_WIN = 2
STATE_STALEMATE = 3
STATE_THREEFOLD = 4
STATE_FIFTY_MOVES = 5
STATE_INSUFFICIENT_MATERIAL = 6

STATE_NAMES = {
    STATE_PLAYING : "playing",
    STATE_WHITE_WIN : "white_win",
    STATE_BLACK_WIN : "black_win",
    STATE_STALEMATE : "stalemate",
    STATE_THREEFOLD : "threefold",
    STATE_FIFTY_MOVES : "fifty_moves",
    STATE_INSUFFICIENT_MATERIAL : "insufficient_material"
}

def state_name(state : int) -> str:
    return STATE_NAMES[state]
```

## License
This project is licensed under the MIT License.
