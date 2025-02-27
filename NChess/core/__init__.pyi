import numpy as np
from typing import Sequence, overload, TypeAlias, Iterable, Tuple

Square : TypeAlias = int | str

class Move(int):
    """
    The Move class is represented as a 16-bit unsigned integer (uint16), allowing for efficient encoding
    of chess moves. The bits are structured as follows:

    - Bits 0-5: Destination square (0-63)
    - Bits 6-11: Origin square (0-63)
    - Bits 12-13: Promotion piece type (0 for knight, 1 for bishop, 2 for rook, 3 for queen)
    - Bits 14-15: Special move flags:
        - 0: Normal move
        - 1: Promotion
        - 2: En passant
        - 3: Castling

    This encoding method enables compact storage and quick access to the essential information 
    regarding each move, similar to the approach used in the Stockfish chess engine.
    """

    @property
    def from_(self) -> int:
        """
        Returns the source square of the move.
        """
        ...
    
    @property
    def to_(self) -> int:
        """
        Returns the destination square of the move.
        """
        ...
    
    @property
    def pro_piece(self) -> int:
        """
        Returns the promotion piece of the move.
        """
        ...

    @property
    def move_type(self) -> int:
        """
        Returns the type of the move.
        """
        ...

    @property
    def is_castle(self) -> bool: 
        """
        Returns True if the move type is castling; otherwise, returns False.
        """
        ...
    
    @property
    def is_enpassant(self) -> bool:
        """
        Returns True if the move type is en passant; otherwise, returns False.
        """
        ...
    
    @property
    def is_promotion(self) -> bool:
        """
        Returns True if the move type is promotion; otherwise, returns False.
        """
        ...

    @property
    def is_normal(self) -> bool:
        """
        Returns True if the move type is normal; otherwise, returns False.
        """
        ...

    @property
    def is_valid(self) -> bool:
        """
        Checks the validity of the source and destination squares.
        Returns True if both squares are valid; otherwise, returns False.
        """
        ...


class BitBoard(int):
    """
    The BitBoard class represents a chess board using a 64-bit integer, where each bit corresponds 
    to a square on the chess board (0 for empty, 1 for occupied). This allows for efficient 
    manipulation and querying of board states. The class provides various methods to convert the 
    bitboard to different representations (e.g., arrays).
    """
    
    def as_array(self, shape: Sequence[int] = None, reversed: bool = False, as_list: bool = False) -> np.ndarray | list:
        """
        Returns a NumPy array or a list representation of the bitboard.

        Parameters:
            shape (Sequence[int], optional): The shape of the returned array. The total number of elements 
                                             must be equal to 64 . By default, the shape is (64,).
            reversed (bool, optional): If set to True, the bitboard is read in reverse.
            as_list (bool, optional): If set to True, returns a Python list instead of a NumPy array.

        Returns:
            np.ndarray | list: The bitboard representation as an array or list.
        """
        ...
    
    def more_than_one(self) -> bool: 
        """
        Returns True if there is more than one bit set in the bitboard; otherwise, returns False.
        
        Returns:
            bool: True if more than one bit is set, False otherwise.
        """
        ...
    
    def has_two_bits(self) -> bool:
        """
        Returns True if the bitboard has exactly two bits set; otherwise, returns False.
        
        Returns:
            bool: True if exactly two bits are set, False otherwise.
        """
        ...
    
    def get_last_bit(self) -> int:
        """
        Returns the index of the last (highest) bit set in the bitboard.

        Returns:
            int: The index of the last bit set.
        """
        ...
    
    def count_bits(self) -> int:
        """
        Returns the number of bits set in the bitboard.

        Returns:
            int: The count of bits set to 1.
        """
        ...
    
    def is_filled(self, square: str | int) -> bool:
        """
        Checks whether the bitboard contains a specific square.

        Parameters:
            square (Square): The square to check.

        Returns:
            bool: True if the square is in the bitboard, False otherwise.
        """
        ...
    
    def to_squares(self) -> list[int]:
        """
        Returns a list of indices representing squares set in the bitboard.

        Returns:
            list[int]: A list of square indices.
        """
        ...
    
    def set_square(self, square: str | int) -> BitBoard:
        """
        Sets a specific square in the bitboard to 1.

        Parameters:
            square (Square): The square to set.

        Returns:
            BitBoard: The updated bitboard with the square set.
        """
        ...
    
    def remove_square(self, square: str | int) -> BitBoard:
        """
        Removes a specific square from the bitboard (sets it to 0).

        Parameters:
            square (Square): The square to remove.

        Returns:
            BitBoard: The updated bitboard with the square removed.
        """
        ...
    
    def __iter__(self) -> Iterable[Tuple[Square]]:
        """
        Allows iteration over the squares set in the bitboard.

        Returns:
            Iterable[Tuple[Square]]: An iterable of square tuples.
        """
        ...


class Board:
    """
    Represents a chessboard using bitboards for piece positions and additional properties for game state.
    """

    def __init__(self, fen: str = None):
        """
        Initializes the board from stadart starting posistion or
        from a FEN (Forsyth-Edwards Notation) string if fen parameter is not None.

        The function deals with FEN dynamically and could deal with extra white spaces.
        FEN has to contain board pieces, side to play and castle rights. Rest (en passant square,
        fifty count, nmoves) are optional and if not set they will be by default zeros.

        Parameters:
            fen (str, optional): The FEN string representing the board state.
                                 If None, initializes to the standard starting position.
        """
        ...

    @property
    def white_pawns(self) -> BitBoard:
        """
        Returns the bitboard representing the white pawns.

        Returns:
            BitBoard: The bitboard for white pawns.
        """
        ...
    
    @property
    def white_knights(self) -> BitBoard:
        """
        Returns the bitboard representing the white knights.

        Returns:
            BitBoard: The bitboard for white knights.
        """
        ...
    
    @property
    def white_bishops(self) -> BitBoard:
        """
        Returns the bitboard representing the white bishops.

        Returns:
            BitBoard: The bitboard for white bishops.
        """
        ...
    
    @property
    def white_rooks(self) -> BitBoard:
        """
        Returns the bitboard representing the white rooks.

        Returns:
            BitBoard: The bitboard for white rooks.
        """
        ...
    
    @property
    def white_queens(self) -> BitBoard:
        """
        Returns the bitboard representing the white queens.

        Returns:
            BitBoard: The bitboard for white queens.
        """
        ...
    
    @property
    def white_king(self) -> BitBoard:
        """
        Returns the bitboard representing the white king.

        Returns:
            BitBoard: The bitboard for the white king.
        """
        ...

    @property
    def black_pawns(self) -> BitBoard:
        """
        Returns the bitboard representing the black pawns.

        Returns:
            BitBoard: The bitboard for black pawns.
        """
        ...
    
    @property
    def black_knights(self) -> BitBoard:
        """
        Returns the bitboard representing the black knights.

        Returns:
            BitBoard: The bitboard for black knights.
        """
        ...
    
    @property
    def black_bishops(self) -> BitBoard:
        """
        Returns the bitboard representing the black bishops.

        Returns:
            BitBoard: The bitboard for black bishops.
        """
        ...
    
    @property
    def black_rooks(self) -> BitBoard:
        """
        Returns the bitboard representing the black rooks.

        Returns:
            BitBoard: The bitboard for black rooks.
        """
        ...
    
    @property
    def black_queens(self) -> BitBoard:
        """
        Returns the bitboard representing the black queens.

        Returns:
            BitBoard: The bitboard for black queens.
        """
        ...
    
    @property
    def black_king(self) -> BitBoard:
        """
        Returns the bitboard representing the black king.

        Returns:
            BitBoard: The bitboard for the black king.
        """
        ...

    @property
    def white_occ(self) -> BitBoard:
        """
        Returns the bitboard representing all occupied squares by white pieces.

        Returns:
            BitBoard: The bitboard for white-occupied squares.
        """
        ...

    @property
    def black_occ(self) -> BitBoard:
        """
        Returns the bitboard representing all occupied squares by black pieces.

        Returns:
            BitBoard: The bitboard for black-occupied squares.
        """
        ...

    @property
    def all_occ(self) -> BitBoard:
        """
        Returns the bitboard representing all occupied squares on the board.

        Returns:
            BitBoard: The bitboard for all occupied squares.
        """
        ...

    @property
    def castles(self) -> int:
        """
        Returns an integer representing the castling rights.

        Returns:
            int: The castling rights as a bitmask.
        """
        ...

    @property
    def castles_str(self) -> str:
        """
        Returns a string representation of the castling rights in FEN format.

        Returns:
            str: The castling rights string.
        """
        ...
    
    @property
    def nmoves(self) -> int:
        """
        Returns the number of moves made in the game.

        Returns:
            int: The total move count.
        """
        ...

    @property
    def fifty_counter(self) -> int:
        """
        Returns the number of half-moves since the last capture or pawn advance.

        Returns:
            int: The half-move counter for the fifty-move rule.
        """
        ...

    @property
    def en_passant_sqr(self) -> int:
        """
        Returns the square index where an en passant capture is possible, or -1 if none.

        Returns:
            int: The index of the en passant target square, or -1 if unavailable.
        """
        ...

    def step(self, step: Move | str | int) -> None:
        """
        Executes a move on the board.

        Parameters:
            step (Move | str | int): The move to be played.
                - If `Move`, it represents a move object.
                - If `str`, it is the UCI (Universal Chess Interface) representation of the move.
                - If `int`, it represents a move encoded as an integer.
        """
        ...

    def undo(self) -> None:
        """
        Undoes the last move and restores the previous board state.
        """
        ...

    def perft(self, deep: int, pretty: bool = False, no_print: bool = False) -> int:
        """
        Performs a performance test (perft) by counting all legal moves up to a given depth.

        Parameters:
            deep (int): The depth of the perft search.
            pretty (bool, optional): If True, numbers printed to the console will include commas (e.g., 1,000,000).
            no_print (bool, optional): If True, the function will not print results to the console.

        Returns:
            int: The total number of legal moves at the given depth.
        """
        ...

    def generate_legal_moves(self) -> list[Move]:
        """
        Generates all legal moves for the current position.

        Returns:
            list[Move]: A list of all legal moves available.
        """
        ...

    def as_array(self, shape: Sequence[int] = None, reversed: bool = False, as_list: bool = False) -> list | np.ndarray:
        """
        Converts the board into an array representation where each bitboard is expanded into an array.

        Parameters:
            shape (Sequence[int], optional): The shape of the returned array. Defaults to (64, 12),
                where each of the 12 bitboards is represented as a 64-length array.
            reversed (bool, optional): If True, each bitboard is read in reverse.
            as_list (bool, optional): If True, returns a Python list instead of a NumPy array.

        Returns:
            list | np.ndarray: A representation of the board.
        """
        ...

    def as_table(self, shape: Sequence[int] = None, reversed: bool = False, as_list: bool = False) -> list | np.ndarray:
        """
        Converts the board into a array of 64 element where each element represents a piece.

        Parameters:
            shape (Sequence[int], optional): The shape of the returned array. Defaults to (64,).
            reversed (bool, optional): If True, the board representation is reversed.
            as_list (bool, optional): If True, returns a Python list instead of a NumPy array.

        Returns:
            list | np.ndarray: A array where each item represents a piece on the board.
        """
        ...

    def on_square(self, square: str | int) -> int:
        """
        Returns the piece located on the given square.

        Parameters:
            square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

        Returns:
            int: The piece occupying the square, or NO_PIECE if empty.
        """
        ...

    def owned_by(self, square: str | int) -> int:
        """
        Determines which player owns the piece on a given square.

        Parameters:
            square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

        Returns:
            int: WHITE if the piece belongs to White, BLACK if it belongs to Black,
            and NO_SIDE if the square is empty.
        """
        ...

    def get_played_moves(self) -> list[Move]:
        """
        Returns a list of all moves that have been played in the game.

        Returns:
            list[Move]: A list of moves played so far.
        """
        ...

    def reset(self) -> None:
        """
        Resets the board to the starting position.
        """
        ...

    def is_check(self) -> bool:
        """
        Checks whether the current player's king is in check.

        Returns:
            bool: True if the king of the player to move is in check, False otherwise.
        """
        ...

    def is_insufficient_material(self) -> bool:
        """
        Checks if the position meets the criteria for an insufficient material draw.

        Returns:
            bool: True if neither side has enough material to checkmate, False otherwise.
        """
        ...

    def is_threefold(self) -> bool:
        """
        Checks if the current position has occurred three times, leading to a draw by repetition.

        Returns:
            bool: True if the same position has been repeated three times, False otherwise.
        """
        ...

    def is_fifty_moves(self) -> bool:
        """
        Checks if the fifty-move rule applies, meaning no pawn move or capture has occurred in the last 50 moves.

        Returns:
            bool: True if the fifty-move rule applies, False otherwise.
        """
        ...

    def get_attackers_map(self, square: str | int) -> BitBoard:
        """
        Returns a bitboard representing all pieces attacking the given square.

        Parameters:
            square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

        Returns:
            BitBoard: A bitboard where set bits indicate attacking pieces.
        """
        ...

    def get_moves_of(self, square: str | int) -> list[Move]:
        """
        Returns all legal moves for the piece located on the given square.

        Parameters:
            square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

        Returns:
            list[Move]: A list of legal moves for the piece on the given square.
        """
        ...

    def copy(self) -> Board:
        """
        Creates a deep copy of the current board.

        Returns:
            Board: A new board instance identical to the current state.
        """
        ...

    def get_game_state(self, can_move: bool) -> int:
        """
        Determines the current game state.

        Parameters:
            can_move (bool): Whether the player to move has any legal moves.

        Returns:
            int: The game state code (e.g., 0 for ongoing, 1 for checkmate, 2 for stalemate).
        """
        ...

    def find(self, piece: int) -> list[int]:
        """
        Finds all squares where a specific piece is located.

        Parameters:
            piece (int): The piece to search for.

        Returns:
            list[int]: A list of squares (0-63) where the given piece is located.
        """
        ...


def square_from_uci(uci: str) -> int:
    """
    Converts a UCI square notation (e.g., "e4") to its corresponding index (0-63).

    Parameters:
        uci (str): The UCI representation of the square (e.g., "e4").

    Returns:
        int: The index of the square (0-63).
    """ 
    ...

def square_file(square: str | int) -> int:
    """
    Retrieves the file (column) of a given square.

    Parameters:
        square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        int: The file of the square (0-7), where 0 represents file 'a' and 7 represents file 'h'.
    """
    ...

def square_rank(square: str | int) -> int:
    """
    Retrieves the rank (row) of a given square.

    Parameters:
        square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        int: The rank of the square (0-7), where 0 represents rank '1' and 7 represents rank '8'.
    """
    ...

def square_distance(square1: Square, square2: Square) -> int:
    """
    Computes the Manhattan distance between two squares.

    Parameters:
        square1 (str | int): The first square, given as a UCI string (e.g., "e4") or an index (0-63).
        square2 (str | int): The second square, given as a UCI string (e.g., "d6") or an index (0-63).

    Returns:
        int: The Manhattan distance between the two squares.
    """
    ...

def square_mirror(square: str | int, vertical: bool = False) -> int:
    """
    Mirrors a square either horizontally or vertically.

    Parameters:
        square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).
        vertical (bool): If True, mirrors the square vertically; otherwise, mirrors it horizontally.

    Returns:
        int: The index of the mirrored square (0-63).
    """
    ...


def move_from_uci(uci: str) -> Move:
    """
    Converts a UCI move notation (e.g., "e2e4" or "e7e8q") into a Move object.

    Parameters:
        uci (str): The UCI representation of the move (e.g., "e2e4" for a normal move or "e7e8q" for promotion).

    Returns:
        Move: The corresponding Move object.
    """
    ...

def move(from_: Square, to_: Square, promote: int = 0, move_type: int = 0) -> Move:
    """
    Creates a Move object from the given parameters.

    Parameters:
        from_ (str | int): The starting square, given as a UCI string (e.g., "e2") or an index (0-63).
        to_ (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).
        promote (int, optional): The piece type to promote to (if applicable). Defaults to 0 (no promotion).
        move_type (int, optional): The type of move (e.g., normal, en passant, castling). Defaults to 0.

    Returns:
        Move: The constructed Move object.
    """
    ...

def bb_from_array(arr: np.ndarray | Sequence) -> BitBoard:
    """
    Converts a NumPy array or a sequence into a BitBoard representation.

    Parameters:
        arr (np.ndarray | Sequence): The input array or sequence to convert.
                                      The shape of the sequence does not matter,
                                      but the number of elements must be 64.

    Returns:
        BitBoard: The corresponding BitBoard representation.
    """
    ...

def bb_rook_attacks(square: str | int, occ: BitBoard) -> BitBoard:
    """
    Calculates the attack positions of a rook from a given square.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).
        occ (BitBoard): A BitBoard representing the current occupancy of the board.

    Returns:
        BitBoard: A BitBoard with the rook's attack positions.
    """
    ...

def bb_bishop_attacks(square: str | int, occ: BitBoard) -> BitBoard:
    """
    Calculates the attack positions of a bishop from a given square.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).
        occ (BitBoard): A BitBoard representing the current occupancy of the board.

    Returns:
        BitBoard: A BitBoard with the bishop's attack positions.
    """
    ...

def bb_queen_attacks(square: str | int, occ: BitBoard) -> BitBoard:
    """
    Calculates the attack positions of a queen from a given square.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).
        occ (BitBoard): A BitBoard representing the current occupancy of the board.

    Returns:
        BitBoard: A BitBoard with the queen's attack positions.
    """
    ...

def bb_king_attacks(square: str | int) -> BitBoard:
    """
    Calculates the attack positions of a king from a given square.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        BitBoard: A BitBoard with the king's attack positions.
    """
    ...

def bb_knight_attacks(square: str | int) -> BitBoard:
    """
    Calculates the attack positions of a knight from a given square.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        BitBoard: A BitBoard with the knight's attack positions.
    """
    ...

def bb_pawn_attacks(square: str | int, color: int) -> BitBoard:
    """
    Calculates the attack positions of a pawn from a given square based on its color.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).
        color (int): The color of the pawn (e.g., 0 for white, 1 for black).

    Returns:
        BitBoard: A BitBoard with the pawn's attack positions.
    """
    ...

def bb_rook_mask(square: str | int) -> BitBoard:
    """
    Generates a mask for rook moves from a given square.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        BitBoard: A BitBoard representing the rook's mask.
    """
    ...

def bb_bishop_mask(square: str | int) -> BitBoard:
    """
    Generates a mask for bishop moves from a given square.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        BitBoard: A BitBoard representing the bishop's mask.
    """
    ...

def bb_rook_relevant(square: str | int) -> int:
    """
    Calculates the relevant index for rook magic bitboards from a given square.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        int: The relevant index for the rook magic bitboard.
    """
    ...

def bb_bishop_relevant(square: str | int) -> int:
    """
    Calculates the relevant index for bishop magic bitboards from a given square.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        int: The relevant index for the bishop magic bitboard.
    """
    ...

def bb_rook_magic(square: str | int) -> int:
    """
    Retrieves the magic number for rook moves from a given square.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        int: The magic number for rook moves.
    """
    ...

def bb_bishop_magic(square: str | int) -> int:
    """
    Retrieves the magic number for bishop moves from a given square.

    Parameters:
        square (str | int): The starting square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        int: The magic number for bishop moves.
    """
    ...
