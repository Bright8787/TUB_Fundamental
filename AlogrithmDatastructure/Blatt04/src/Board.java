import jdk.jfr.Frequency;

import java.util.ArrayList;
import java.util.InputMismatchException;
import java.util.List;
import java.util.Stack;

import static java.lang.Math.abs;
/**
 * This class represents a generic TicTacToe game board.
 */
public class Board {
    private int n;
    private int player;
    public int[][] board;
    public static String[][] analyseBoard;

    //TODO

    /**
     * Creates Board object, am game board of size n * n with 1<=n<=10.
     */
    public Board(int n) {
        this.n = n;
        // TODO
        if (n >= 1 && n <= 10) {
            board = new int[n][n];
            analyseBoard = new String[n][n];


            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    board[i][j] = 0;
                }
            }
        } else {
            throw new InputMismatchException();
        }
    }

    /**
     * @return length/width of the Board object
     */
    public int getN() {
        return n;
    }

    /**
     * @return number of currently free fields
     */
    public int nFreeFields() {
        // TODO
        int freeField = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (board[i][j] == 0) {
                    freeField++;
                }
            }
        }
        return freeField;
    }

    /**
     * @return token at position pos
     */
    public int getField(Position pos) throws InputMismatchException {
        // TODO
        if (pos.x < 0 || pos.y < 0 || pos.x > n || pos.y > n) throw new InputMismatchException();
        else {
            if (board[pos.x][pos.y] == 1) return 1;
            else if (board[pos.x][pos.y] == -1) return -1;
            else return 0;
        }
    }

    /**
     * Sets the specified token at Position pos.
     */
    public void setField(Position pos, int token) throws InputMismatchException {
        // TODO
        if (pos.x < 0 || pos.y < 0 || pos.x > n || pos.y > n || token < -1 || token > 1)
            throw new InputMismatchException();
        else {
            board[pos.x][pos.y] = token;
        }

    }

    /**
     * Places the token of a player at Position pos.
     */
    public void doMove(Position pos, int player) {
        // TODO
        board[pos.x][pos.y] = player;
    }

    /**
     * Clears board at Position pos.
     */
    public void undoMove(Position pos) {
        // TODO
        board[pos.x][pos.y] = 0;

    }

    /**
     * @return true if game is won, false if not
     */
    public boolean isGameWon() {
        // TODO
        //CHECK ROWS AND COLS
        for (int j = 0; j < n; j++) {
            if (checkCols(1, j)) return true;
            if (checkCols(-1, j)) return true;

            if (checkRows(1, j)) return true;
            if (checkRows(-1, j)) return true;
        }

        if (checkDiagonal(1)) return true;
        if (checkDiagonal(-1)) return true;

        return false;

    }

    public boolean checkCols(int player, int cols) {

        for (int i = 0; i < n; i++) {
            if ((board[i][cols] != player)) {
                return false;
            }
        }
        return true;
    }

    public boolean checkRows(int player, int row) {

        for (int z = 0; z < n; z++) {
            if ((board[row][z] != player)) {
                return false;
            }
        }
        return true;
    }

    public boolean checkDiagonal(int player) {
        int isTrue_LR = 1;
        int isTrue_RL = 1;

        for (int i = 0; i < n; i++) {
            //top left to right bottom
            if (board[i][i] != player) {
                isTrue_LR = 0;
            }
            //bottom right to top left
            if (board[i][n - 1 - i] != player) {
                isTrue_RL = 0;
            }
        }

        return isTrue_LR == 1 || isTrue_RL == 1;
    }

    /**
     * @return set of all free fields as some Iterable object
     */
    public Iterable<Position> validMoves() {
        // TODO
        List<Position> freeSpace = new ArrayList<>();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (board[i][j] == 0) {
                    freeSpace.add(new Position(i, j));
                }
            }
        }
        return freeSpace;

    }

    /**
     * Outputs current state representation of the Board object.
     * Practical for debugging.
     */
    public void print() {
        // TODO
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                System.out.print("  " + board[i][j] + "  ");
            }
            System.out.println();
        }
    }
}







