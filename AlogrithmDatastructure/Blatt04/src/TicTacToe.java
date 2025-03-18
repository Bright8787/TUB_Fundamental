import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * This class implements and evaluates game situations of a TicTacToe game.
 */
public class TicTacToe {

    /**
     * Returns an evaluation for player at the current board state.
     * Arbeitet nach dem Prinzip der Alphabeta-Suche. Works with the principle of Alpha-Beta-Pruning.
     *
     * @param board     current Board object for game situation
     * @param player    player who has a turn
     * @return          rating of game situation from player's point of view
    **/

    public static int alphaBeta(Board board,int player) {
        // TODO
        return alphaBetaHelper(board, player, -Integer.MAX_VALUE, Integer.MAX_VALUE, board.nFreeFields()+1);

    }
    public static int alphaBetaHelper(Board board, int player, int alpha, int beta , int depth)
    {

            if(board.isGameWon() || depth == 0){
                //swap the player perspective
                 return -(board.nFreeFields()+1);
            }
            if(board.nFreeFields() == 0 ){
                return 0;
            }
            //iterate through all possible moves
            for(Position pos_element : board.validMoves()){
                board.doMove(pos_element,player);
                int score = -1 * alphaBetaHelper(board,-player,-beta ,-alpha,depth-1);
                board.undoMove(pos_element);
                if(score > alpha){
                    alpha = score;
                    if(alpha >= beta) break;
                }
            }
        return alpha;
    }


    /**
     * Vividly prints a rating for each currently possible move out at System.out.
     * (from player's point of view)
     * Uses Alpha-Beta-Pruning to rate the possible moves.
     * formatting: See "Beispiel 1: Bewertung aller Zugmöglichkeiten" (Aufgabenblatt 4).
     *
     * @param board     current Board object for game situation
     * @param player    player who has a turn
    **/
    public static void evaluatePossibleMoves(Board board, int player)
    {
        // TODO
        //try all possible move
        char symbol = ' ';
        if(player == 1) symbol = 'x';
        if(player == -1) symbol = 'o';

        //change 1 and -1 to player symbols
        for (int i = 0; i < board.getN(); i++) {
            for (int j = 0; j < board.getN(); j++) {
                if(board.board[i][j] == 1) Board.analyseBoard[i][j] = "x";
                else if(board.board[i][j] == -1) Board.analyseBoard[i][j] = "o";
            }
        }
        //iterate through possible moves
        for(Position pos_element : board.validMoves()){
            board.doMove(pos_element,player);
            int score = -alphaBeta(board,-player);
            board.undoMove(pos_element);
//            System.out.println(Integer.toString(score));
            Board.analyseBoard[pos_element.x][pos_element.y] =  Integer.toString(score);
        }

        System.out.println("Evaluation for player " + "'" + symbol + "'" + ":" );
        //print board
        for (int i = 0; i < board.getN(); i++) {
            for (int j = 0; j < board.getN(); j++) {
                    if(Board.analyseBoard[j][i].length() == 2){
                        System.out.print(" ");
                        System.out.print(Board.analyseBoard[j][i]);
                    }
                    else{
                        System.out.print("  ");
                        System.out.print(Board.analyseBoard[j][i]);
                    }
            }
            System.out.println();
        }

    }

    public static void main(String[] args)
    {
    }
}

