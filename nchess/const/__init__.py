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