import numpy as np
from typing import Sequence, overload, TypeAlias, Iterable, Tuple, Optional

Square : TypeAlias = int | str
BitBoardLike : TypeAlias = int | Sequence | np.ndarray | BitBoard

class Move(int):
    """
    The Move class is represented as a 16-bit unsigned integer (uint16), allowing for efficient encoding
    of chess moves.
    This encoding method enables compact storage and quick access to the essential information 
    regarding each move, similar to the approach used in the Stockfish chess engine.
    """

    def __init__(self, move: int | str):
        """
        Initializes a Move instance.

        Parameters:
        -----------
        move : int | str
            - If an integer (uint16), it should follow the move encoding format:
            - Bits 0-5: Destination square (0-63)
            - Bits 6-11: Origin square (0-63)
            - Bits 12-13: Promotion piece type (0=Knight, 1=Bishop, 2=Rook, 3=Queen)
            - Bits 14-15: Move type (0=Normal, 1=Promotion, 2=En Passant, 3=Castling)
            - If a string, it should be in UCI format (e.g., "e2e4" for a normal move or "e7e8q" for a promotion).

        Raises:
        -------
        ValueError:
            - If the integer encoding is out of range.
            - If the string format is invalid or not a valid UCI move.
        """
        ...


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

    def __init__(self, bitboard: BitBoardLike = 0):
        """
        Initializes a BitBoard instance.

        Parameters:
            bitboard (BitBoardLike, optional): The initial value for the bitboard.
                                               It can be an integer, a sequence, or a NumPy array.
                                               Defaults to 0 (empty board).
        """
        ...
    
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
            square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

        Returns:
            bool: True if the square is in the bitboard, False otherwise.
        """
        ...
    
    def to_squares(self, as_set : bool = False) -> list[int] | set[int]:
        """
        Returns a list of indices representing squares set in the bitboard.

        Parameters:
            as_set: returns a set of squares if set to True.
        
        Returns:
            list[int]: A list of square indices.
        """
        ...
    
    def set_square(self, square: str | int) -> BitBoard:
        """
        Sets a specific square in the bitboard to 1.

        Parameters:
            square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

        Returns:
            BitBoard: The updated bitboard with the square set.
        """
        ...
    
    def remove_square(self, square: str | int) -> BitBoard:
        """
        Removes a specific square from the bitboard (sets it to 0).

        Parameters:
            square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

        Returns:
            BitBoard: The updated bitboard with the square removed.
        """
        ...
    
    def __iter__(self) -> Iterable[Tuple[int]]:
        """
        Allows iteration over the squares set in the bitboard.

        Returns:
            Iterable[Tuple[int]]: An iterable of square tuples.
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

    @property
    def side(self) -> int:
        """
        Returns the side to play 0 for white and 1 for black

        Returns:
            int: side to play.
        """
        ...

    @property
    def captured_piece(self) -> int:
        """
        Returns the piece has been captured within the last move. if no piece
        has been captured returns 0.
        Note:
            A pawn caputred with an en passant is not stored in board as captured
            piece but the flag is_captured would be turn on.
            in other words if last move was en passant the function will
            return 0 but if you call is_capture it will return True.

        Returns:
            int: piece has been captured
        """
        ...

    @property
    def is_check(self) -> bool:
        """
        Checks whether the current player's king is in check.

        Returns:
            bool: True if the king of the player to move is in check, False otherwise.
        """
        ...
    
    @property
    def is_double_check(self) -> bool:
        """
        Checks whether the current player's king is in double check.

        Returns:
            bool: True if the king of the player to move is in double check, False otherwise.
        """
        ...

    @property
    def is_pawn_moved(self) -> bool:
        """
        Checks whether a pawn moved or not in the last move.

        Returns:
            bool: True if the last move is a pawn push, False otherwise.
        """
        ...

    @property
    def is_capture(self) -> bool:
        """
        Checks whether a piece has been captured or not in the last move.

        Returns:
            bool: True if the last move captured a piece, False otherwise.
        """
        ...

    @property
    def is_insufficient_material(self) -> bool:
        """
        Checks if the position meets the criteria for an insufficient material draw.

        Returns:
            bool: True if neither side has enough material to checkmate, False otherwise.
        """
        ...

    @property
    def is_threefold(self) -> bool:
        """
        Checks if the current position has occurred three times, leading to a draw by repetition.

        Returns:
            bool: True if the same position has been repeated three times, False otherwise.
        """
        ...

    @property
    def is_fifty_moves(self) -> bool:
        """
        Checks if the fifty-move rule applies, meaning no pawn move or capture has occurred in the last 50 moves.

        Returns:
            bool: True if the fifty-move rule applies, False otherwise.
        """
        ...

    @property
    def can_move(self) -> bool:
        """
        Checks the player who has to play has any legal moves or not.

        Returns:
            bool: True if there is legal moves in the position and False if not.
        """
        ...


    def step(self, move: Move | str | int) -> bool:
        """
        Executes a move on the board.

        Parameters:
            move (Move | str | int): The move to be played.
                - If `Move`, it represents a move object.
                - If `str`, it is the UCI (Universal Chess Interface) representation of the move.
                - If `int`, it represents a move encoded as an integer.
        
        Returns:
            True if the move has been played and False if not.
        """
        ...

    def undo(self) -> None:
        """
        Undoes the last move and restores the previous board state.
        """
        ...

    def is_move_legal(self, move: Move | str | int) -> bool:
        """
        Check whether a move is legal or not. MoveType does not matter.

        Parameters:
            move (Move | str | int): The move to be played.
                - If `Move`, it represents a move object.
                - If `str`, it is the UCI (Universal Chess Interface) representation of the move.
                - If `int`, it represents a move encoded as an integer.
        
        Returns:
            bool: True if the move is legal and False if not.
        """
        ...

    def make_move_legal(self, move: Move | str | int) -> Tuple[bool, Move]:
        """
        Converts an illegal move to a legal one if possible.

        Parameters:
            move (Move | str | int): The move to be converted.
                - If `Move`, it represents a move object.
                - If `str`, it is the UCI (Universal Chess Interface) representation of the move.
                - If `int`, it represents a move encoded as an integer.
        Returns:
            None | Move: A legal Move object if the conversion is possible, otherwise None.
        """
        ...


    def perft(self, deep: int, pretty: bool = False, no_print: bool = False) -> int:
        """
        Performs a performance test (perft) by counting all legal moves up to a given depth.

        Parameters:
            deep (int): The depth of the perft search.
            pretty (bool, optional): If True, numbers printed to the console will include commas (e.g., 1,000,000).
            no_print (bool, optional): If True, the function will not print results to the console.

        Note:
            on Jupyter Notebook it prints nothing.

        Returns:
            int: The total number of legal moves at the given depth.
        """
        ...

    def perft_moves(self, deep: int) -> dict[Move, int]:
        """
        Performs a performance test (perft) and returns a dictionary mapping each legal move
        to the number of positions reachable from that move at the given depth.

        Parameters:
            deep (int): The depth of the perft search.

        Returns:
            dict[Move, int]: A dictionary where keys are Move objects and values are the
                            number of legal positions reachable from each move.
        """
        ...

    def generate_legal_moves(self, as_set : bool = False) -> list[Move] | set[Move]:
        """
        Generates all legal moves for the current position.

        Parameters:
            as_set: returns a set of moves if set to True.

        Returns:
            list[Move]: A list of all legal moves available.
        """
        ...

    def as_array(self, shape: Sequence[int] = None, reversed: bool = False, as_list: bool = False) -> list | np.ndarray:
        """
        Converts the board into an array representation where each bitboard is expanded into an array.

        Parameters:
            shape (Sequence[int], optional): The shape of the returned array. Defaults to (12, 64),
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
            int: The piece occupying the square, or 0 if empty.
        """
        ...

    def owned_by(self, square: str | int) -> int:
        """
        Determines which player owns the piece on a given square.

        Parameters:
            square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

        Returns:
            int: 0 if the piece belongs to White, 1 if it belongs to Black,
            and -1 if the square is empty.
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
        Resets the board to the starting position. Similar to undoing all played moves.
        """
        ...

    def get_attackers_map(self, square: str | int, attacker_side : int = None) -> BitBoard:
        """
        Returns a bitboard representing all pieces by the attacker_side attacking the given square.
        attacker_side is the side how does have the turn to play if the paramater is not set to anything.
        otherwise it will be the set given by the parameter attacker_side.

        Parameters:
            square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).
            attacker_side: The side how is attacking the square. if None it will be the side how does not
                           have the turn to play. if set to 0 it will be white and if set to 1 it will be black.
                           if set to 2 it will consider both sides.

        Returns:
            BitBoard: A bitboard where set bits indicate attacking pieces.
        """
        ...

    def get_moves_of(self, square: str | int, as_set : bool = False) -> list[Move] | set[Move]:
        """
        Returns all legal moves for the piece located on the given square.

        Parameters:
            square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).
            as_set: returns a set of moves if set to True.

        Returns:
            list[Move]: A list of legal moves for the piece on the given square.
        """
        ...

    def get_occ(self, side : int) -> BitBoard:
        """
        Returns an occupancy by the side.

        Parameters:
            side (int) : 0 for white or 1 for black, 2 for both sides.

        Returns:
            BitBoard: given side occupancy as BitBoard object
        """
        ...

    def copy(self) -> Board:
        """
        Creates a deep copy of the current board.

        Returns:
            Board: A new board instance identical to the current state.
        """
        ...

    def get_game_state(self, can_move: Optional[bool]) -> int:
        """
        Determines the current game state.

        Parameters:
            can_move (bool): if set to True or False the board won't generate legal
                            moves on its own to check if the player how has the play could
                            move or not and it will use the parameter can_move.

        Returns:
            int: The game state code (e.g., 0 for playing, 1 for white win).
        """
        ...

    def find(self, piece: int | str) -> list[int]:
        """
        Finds all squares where a specific piece is located.

        Parameters:
            piece (int | str): The piece to search for. could be int between (0, 11)
                               or a string with one single char of these 'PNBRQKpnbrqk'

        Returns:
            list[int]: A list of squares (0-63) where the given piece is located.
        """
        ...

    def fen(self) -> str:
        """
        Generates the FEN representation of the board, including all standard parameters  
        (piece placement, turn, castling rights, en passant target, fifty moves and fullmove number).  

        Returns:
            str: The FEN representation of the board.
        """
        ...

    def _makemove(self, move: int | str) -> None:
        """
        Privately applies a move to the board without legality checks.

        This is a private function, intended for internal use. However, it is provided 
        for programmers who may need finer control over move execution. Unlike `step`, 
        which ensures the move is legal before applying it, `_makemove` blindly executes 
        the given move without any validation.

        This makes `_makemove` useful in scenarios where legality is already ensured, 
        such as when selecting a move from a pre-generated list of legal moves. Since it 
        skips legality checks, it is technically faster than `step`.

        Example use case:
            If you generate legal moves and are certain that the selected move is valid, 
            you can use `_makemove` instead of `step` for better performance:
            
            ```python
            legal_moves = board.generate_legal_moves(as_set=True)
            move = some_selection_function(legal_moves)
            board._makemove(move)  # Faster than board.step(move) since legality is known
            ```

        Parameters:
            move (int | str): The move to be applied.
                - If `int`, it represents an encoded move.
                - If `str`, it is the UCI (Universal Chess Interface) representation of the move.

        Returns:
            None
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

def square_column(square: str | int) -> int:
    """
    Retrieves the column (column) of a given square.

    Parameters:
        square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        int: The column of the square (0-7), where 0 represents column 'h' and 7 represents column 'a'.
    """
    ...

def square_row(square: str | int) -> int:
    """
    Retrieves the row (row) of a given square.

    Parameters:
        square (str | int): The target square, given as a UCI string (e.g., "e4") or an index (0-63).

    Returns:
        int: The row of the square (0-7), where 0 represents row '1' and 7 represents row '8'.
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


def move_from_uci(uci: str, move_type: int = None) -> Move:
    """
    Converts a UCI move notation (e.g., "e2e4" or "e7e8q") into a Move object.

    # Note:
        The move type cannot be determined solely from the UCI string.
        UCI moves may represent Normal or Promotion types. However, if the move 
        is an En Passant or Castling move, the type must be specified manually. 

        The `Board.step` function does not require an explicit move type; it only 
        considers the source and destination squares and automatically determines 
        legality. If the type is not provided, `Board.step` will still execute the 
        move correctly.

        However, `Board._makemove` requires the correct move type. This means that 
        while a UCI string is sufficient for `Board.step`, it is not enough for 
        `Board._makemove`. If legal moves are generated and include Castling or 
        En Passant moves, constructing a move solely from the UCI string will 
        not detect these special moves, even if they share the same source and 
        destination squares.

        ## Example:
        ```python
        move1 = move_from_uci("e5d6", move_type=MOVE_ENPASSANT)
        move2 = move_from_uci("e5d6")

        # move1 is not equal to move2
        # However, both board.step(move1) and board.step(move2) will produce 
        # the same result, whereas board._makemove(move1)
        # and board._makemove(move2) will not.

        # Additionally, if we generate legal moves as a set and assume there is 
        # an En Passant move among them:
        legal_moves = board.generate_legal_moves(as_set=True)

        print(move1 in legal_moves)  # True
        print(move2 in legal_moves)  # False
        ```
            
    Parameters:
        uci (str): The UCI representation of the move (e.g., "e2e4" for a normal move 
                   or "e7e8q" for promotion).
        move_type (int, optional): The type of move, required for En Passant and Castling. 

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

def bb_from_squares(squares: Sequence[int | str]) -> BitBoard:
    """
    Converts a sequence of squares to a BitBoard object.
    squares could be given as a UCI string (e.g., "e4") or an index (0-63).

    Parameters:
        squares: a sequence of squares.

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
