from typing import List, TypeAlias, Sequence
from ..sq.types import Square
from numpy import ndarray

BitBoard : TypeAlias = int | Sequence

def as_array(bb : BitBoard) -> ndarray | List : ...
def more_then_one(bb : BitBoard) -> bool : ...
def has_two_bits(bb : BitBoard) -> bool : ...
def get_tsb(bb : BitBoard) -> int : ...
def get_lsb(bb : BitBoard) -> int : ...
def is_filled(bb : BitBoard) -> bool : ...
def from_array(array_like : Sequence) -> int : ...
def rook_attacks(square : Square, occupancy : BitBoard) -> int : ...
def bishop_attacks(square : Square, occupancy : BitBoard) -> int : ...
def queen_attacks(square : Square, occupancy : BitBoard) -> int : ...
def king_attacks(square : Square) -> int : ...
def knight_attacks(square : Square) -> int : ...
def pawn_attacks(square : Square, white : bool) -> int : ...
def rook_mask(square : Square) -> int : ...
def bishop_mask(square : Square) -> int : ...
def rook_relevant(square : Square) -> int : ...
def bishop_relevant(square : Square) -> int : ...
def rook_magic(square : Square) -> int : ...
def bishop_magic(square : Square) -> int : ...
def to_indeices(bb : BitBoard) -> List[int] : ...
