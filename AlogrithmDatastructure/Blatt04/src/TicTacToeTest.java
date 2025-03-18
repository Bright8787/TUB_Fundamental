import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

class TicTacToeTest {

	@Test
	void testAlphaBetaBoardInt() {
		Board board0 = new Board(2);
		Board board = new Board(3);
		Board board1 = new Board(4);
		board1.board[0][0] = 1;
		board1.board[0][1] = 1;
		board1.board[0][2] = 0;
		board1.board[0][3] = 0;


		board1.board[1][0] = 1;
		board1.board[1][1] = 0;
		board1.board[1][2] = 0;
		board1.board[1][3] = 0;


		board1.board[2][0] = 0;
		board1.board[2][1] = 0;
		board1.board[2][2] = 0;
		board1.board[2][3] = 0;

		board1.board[3][0] = 0;
		board1.board[3][1] = 0;
		board1.board[3][2] = 0;
		board1.board[3][3] = 0;
		int result = TicTacToe.alphaBeta(board1,1);

		assertEquals(5,result);


		board.board[0][0] = 0;
		board.board[0][1] = 0;
		board.board[0][2] = 1;

		board.board[1][0] = -1;
		board.board[1][1] = 1;
		board.board[1][2] = 1;

		board.board[2][0] = -1;
		board.board[2][1] = -1;
		board.board[2][2] = 0;

		result = TicTacToe.alphaBeta(board,1);
		assertEquals(3,result);

		board.board[0][0] = 0;
		board.board[0][1] = 0;
		board.board[0][2] = 0;

		board.board[1][0] = 0;
		board.board[1][1] = 0;
		board.board[1][2] = 0;

		board.board[2][0] = 0;
		board.board[2][1] = 0;
		board.board[2][2] = 0;


		result = TicTacToe.alphaBeta(board,1);
		assertEquals(0,result);

		board.board[0][0] = 1;
		board.board[0][1] = -1;
		board.board[0][2] = -1;

		board.board[1][0] = 0;
		board.board[1][1] = 1;
		board.board[1][2] = 0;

		board.board[2][0] = 1;
		board.board[2][1] = 0;
		board.board[2][2] = -1;

		result = TicTacToe.alphaBeta(board,1);
		assertEquals(3,result);

		board0.board[0][0] = 0;
		board0.board[0][1] = 0;
		board0.board[1][0] = 0;
		board0.board[1][1] = 0;
		board0.print();
		result = TicTacToe.alphaBeta(board0,1);
		assertEquals(2,result);
	}

	@Test
	void testEvaluatePossibleMoves() {

		Board board = new Board(3);


		board.board[0][0] = 0;
		board.board[0][1] = -1;
		board.board[0][2] = 0;

		board.board[1][0] = 0;
		board.board[1][1] = 0;
		board.board[1][2] = 0;

		board.board[2][0] = 0;
		board.board[2][1] = 0;
		board.board[2][2] = 1;
		
		assertEquals(3, TicTacToe.alphaBeta(board,1));
		TicTacToe.evaluatePossibleMoves(board,1);


	}
}


