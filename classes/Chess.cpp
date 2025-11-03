#include "Chess.h"
#include <limits>
#include <cmath>
#include <sstream>
#include <cctype>
#include <vector>

Chess::Chess()
{
    _grid = new Grid(8, 8);
}

Chess::~Chess()
{
    delete _grid;
}

char Chess::pieceNotation(int x, int y) const
{
    const char *wpieces = { "0PNBRQK" };
    const char *bpieces = { "0pnbrqk" };
    Bit *bit = _grid->getSquare(x, y)->bit();
    char notation = '0';
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()-128];
    }
    return notation;
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);

    return bit;
}

void Chess::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;

    _grid->initializeChessSquares(pieceSize, "boardsquare.png");
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    startGame();
}

void Chess::FENtoBoard(const std::string& fen) {
    // convert a FEN string to a board
    // FEN is a space delimited string with 6 fields
    // 1: piece placement (from white's perspective)
    // 2: active color (W or B)
    // 3: castling availability (KQkq or -)
    // 4: en passant target square (in algebraic notation, or -)
    // 5: halfmove clock (number of halfmoves since the last capture or pawn advance)

    // --- (1) piece placement: clear board then fill from placement field ---
    // clear any existing pieces first
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });

    // Extract first token (piece placement). The input may be just the placement
    // or a full FEN; using istringstream lets us read the first whitespace
    // separated field easily.
    std::istringstream iss(fen);
    std::string placement;
    if (!(iss >> placement)) {
        return;
    }

    std::vector<std::string> ranks;
    {
        std::istringstream rp(placement);
        std::string rank;
        while (std::getline(rp, rank, '/')) {
            ranks.push_back(rank);
        }
    }

    if (ranks.size() != 8) return;

    for (size_t r = 0; r < 8; ++r) {
        const std::string &rankStr = ranks[r];
        int y = 7 - static_cast<int>(r);
        int x = 0;
        for (size_t i = 0; i < rankStr.size() && x < 8; ++i) {
            char c = rankStr[i];
            if (std::isdigit(static_cast<unsigned char>(c))) {
                x += c - '0';
                continue;
            }
            bool isWhite = std::isupper(static_cast<unsigned char>(c));
            char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            ChessPiece piece = NoPiece;
            switch (lower) {
                case 'p': piece = Pawn; break;
                case 'n': piece = Knight; break;
                case 'b': piece = Bishop; break;
                case 'r': piece = Rook; break;
                case 'q': piece = Queen; break;
                case 'k': piece = King; break;
                default: piece = NoPiece; break;
            }
            if (piece != NoPiece && x >= 0 && x < 8) {
                Bit* bit = PieceForPlayer(isWhite ? 0 : 1, piece);
                ChessSquare* square = _grid->getSquare(x, y);
                if (square) square->setBit(bit);
            }
            ++x;
        }
    }

    // --- (2)-(5) optional
    std::string activeColor;   // (2)
    std::string castling;      // (3)
    std::string enpassant;     // (4)
    std::string halfmove;      // (5)
    std::string fullmove;      // (6)

    if (iss >> activeColor) {

    }
    if (iss >> castling) {
        
    }
    if (iss >> enpassant) {
        
    }
    if (iss >> halfmove) {
        
    }
    if (iss >> fullmove) {
        
    }
}

bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // need to implement friendly/unfriendly in bit so for now this hack
    int currentPlayer = getCurrentPlayer()->playerNumber() * 128;
    int pieceColor = bit.gameTag() & 128;
    if (pieceColor == currentPlayer) return true;
    return false;
}

bool Chess::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    return true;
}

void Chess::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* Chess::ownerAt(int x, int y) const
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return nullptr;
    }

    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Chess::checkForWinner()
{
    return nullptr;
}

bool Chess::checkForDraw()
{
    return false;
}

std::string Chess::initialStateString()
{
    return stateString();
}

std::string Chess::stateString()
{
    std::string s;
    s.reserve(64);
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
            s += pieceNotation( x, y );
        }
    );
    return s;}

void Chess::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 8 + x;
        char playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit(PieceForPlayer(playerNumber - 1, Pawn));
        } else {
            square->setBit(nullptr);
        }
    });
}
