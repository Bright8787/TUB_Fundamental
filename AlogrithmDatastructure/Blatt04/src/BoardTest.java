import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

class BoardTest {

	@Test
	void testBoard() {
		Board board = new Board(3);

	}

	@Test
	void testGetN() {
		Board board  = new Board(3);
		assertEquals(3,board.getN());
	}

	@Test
	void testNFreeFields() {
		Board board = new Board(3);
		//all Free space

		board.print();
		assertEquals(9,board.nFreeFields());
		//1 Free space
		for (int i = 0; i < board.getN(); i++) {
			for (int j = 0; j < board.getN(); j++) {
				board.board[i][j] = 1;
			}
		}

//		board.board[0][0] = 0;
		board.print();
		assertEquals(0,board.nFreeFields());

	}

	@Test
	void testGetField() {
		Board board = new Board(3);
		//all Free space
		for (int i = 0; i < board.getN(); i++) {
			for (int j = 0; j < board.getN(); j++) {
				board.board[i][j] = 0;
			}
		}
		assertEquals(0,board.getField(new Position(0,2)));
		//change 1 grid
		board.board[0][2] = -1;
		assertEquals(-1,board.getField(new Position(0,2)));
		//out of range
//		assertNotEquals(-1,board.getField(new Position(0,5)));

	}

	@Test
	void testSetField() {
		Board board = new Board(3);
		board.setField(new Position(0,2),-1);
		assertEquals(board.board[0][2],-1);
		board.setField(new Position(2,2),0);
		assertEquals(board.board[2][2],0);
	}

	@Test
	void testDoMove() {
		fail("Not yet implemented");
	}

	@Test
	void testUndoMove() {
		fail("Not yet implemented");
	}

	@Test
	void testIsGameWon() {
		Board board = new Board(3);
		//win vertical
		board.board[0][0] = 1;
		board.board[1][0] = 1;
		board.board[2][0] = 1;
//		board.print();
		assertTrue(board.isGameWon(),"win vertical 1");

		board.board[0][0] = -1;
		board.board[1][0] = -1;
		board.board[2][0] = -1;
		assertTrue(board.isGameWon(),"win vertical -1");
		//reset board
		board.board[0][0] = 0;
		board.board[1][0] = 0;
		board.board[2][0] = 0;

		//win horizontal
		board.board[0][0] = 1;
		board.board[0][1] = 1;
		board.board[0][2] = 1;
//		board.print();

		assertTrue(board.isGameWon(),"win horizontal 1");

		board.board[0][0] = -1;
		board.board[0][1] = -1;
		board.board[0][2] = -1;
		assertTrue(board.isGameWon(),"win horizontal -1");
		//reset board
		board.board[0][0] = 0;
		board.board[0][1] = 0;
		board.board[0][2] = 0;

		//win diagonally
		board.board[0][0] = 1;
		board.board[1][1] = 1;
		board.board[2][2] = 1;
//		board.print();
		assertTrue(board.isGameWon(),"win left Right Diagonal 1");

		board.board[0][0] = -1;
		board.board[1][1] = -1;
		board.board[2][2] = -1;
		assertTrue(board.isGameWon(),"win left Right Diagonal -1");
		//reset board
		board.board[0][0] = 0;
		board.board[1][1] = 0;
		board.board[2][2] = 0;

		board.board[0][2] = 1;
		board.board[1][1] = 1;
		board.board[2][0] = 1;
		assertTrue(board.isGameWon(),"win Right Left Diagonal 1");

		board.board[0][2] = -1;
		board.board[1][1] = -1;
		board.board[2][0] = -1;
		assertTrue(board.isGameWon(),"win Right Left Diagonal -1");

		//Random board
		board.board[0][0] = 1;
		board.board[0][1] = -1;
		board.board[0][2] = 0;

		board.board[1][0] = 0;
		board.board[1][1] = 1;
		board.board[1][2] = 1;

		board.board[2][0] = 0;
		board.board[2][1] = -1;
		board.board[2][2] = 1;
		board.print();
		assertTrue(board.isGameWon(),"win Random Diagonally -1");


	}

	@Test
	void testValidMoves() {
		fail("Not yet implemented");
	}

	@Test
	void testPrint() {
		fail("Not yet implemented");
	}

}


