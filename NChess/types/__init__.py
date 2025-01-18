from typing import TypeAlias

Piece : TypeAlias = int

WHITE = 0
BLACK = 1
SIDES_NB = 2
NO_SIDE = 3

SIDE_NAMES = {
    WHITE : "white",
    BLACK : "black"
}

# pieces
PAWN = 0
KNIGHT = 1
BISHOP = 2
ROOK = 3
QUEEN = 4
KING = 5
PIECES_NB = 6
NO_PIECE = 7

PIECE_NAMES = {
    PAWN : "pawn",
    KNIGHT : "knight",
    BISHOP : "bishop",
    ROOK : "rook",
    QUEEN : "queen",
    KING : "king",
    PAWN + PIECES_NB : "pawn",
    KNIGHT + PIECES_NB : "knight",
    BISHOP + PIECES_NB : "bishop",
    ROOK + PIECES_NB : "rook",
    QUEEN + PIECES_NB : "queen",
    KING + PIECES_NB : "king",
}

PIECE_SYMBOLS = "PNBRQKpnbrqk"

PIECE_SYMBOLS_AS_PIECES = {
    "P" : PAWN,
    "N" : KNIGHT,
    "B" : BISHOP,
    "R" : ROOK,
    "Q" : QUEEN,
    "K" : KING,
    "p" : PAWN,
    "n" : KNIGHT,
    "b" : BISHOP,
    "r" : ROOK,
    "q" : QUEEN,
    "k" : KING
}

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

CASTLE_WK = 1
CASTLE_WQ = 2
CASTLE_BK = 4
CASTLE_BQ = 8

CASTLE_KINGSIDE = CASTLE_WK | CASTLE_BK
CASTLE_QUEENSIDE = CASTLE_WQ | CASTLE_BQ

NO_CASTLE = 0

STARTING_FEN= 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'

def piece_name(piece : int) -> str:
    return PIECE_NAMES[piece]

def piece_symbol(piece : int, side : int = WHITE) -> str:
    return PIECE_SYMBOLS[side * PIECES_NB + piece]

def piece_from_symbol(symbol : str) -> int:
    return PIECE_SYMBOLS_AS_PIECES[symbol]

def side_name(side : int) -> str:
    return SIDE_NAMES[side]

def square_name(square : int) -> str:
    return SQUARE_NAMES[square]