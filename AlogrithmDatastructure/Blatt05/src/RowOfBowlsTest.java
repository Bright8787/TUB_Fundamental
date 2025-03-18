import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

import java.util.List;

class RowOfBowlsTest {

	@Test
	void testRowOfBowls() {
		RowOfBowls testRowOfBowls = new RowOfBowls();
	}

	@Test
	void testMaxGain() {
		RowOfBowls gainRecursiv = new RowOfBowls();
//		int[] Eingabe = new  int[]{ 4,7,2,3 };
		int[] Eingabe = new  int[]{ 3,4,1,2,8,5 };

		System.out.println(gainRecursiv.maxGain(Eingabe));
	}

	@Test
	void testMaxGainRecursiveIntArray() {
		fail("Not yet implemented");
	}

	@Test
	void testMaxGainRecursiveIntInt() {
		int[] Eingabe = new  int[]{ 4,7,2,3 };
		RowOfBowls gainRecusiv = new RowOfBowls();
//		int[] Eingabe = new  int[]{ 3,4,1,2,8,5 };
		gainRecusiv.maxGainRecursive(Eingabe);
	}

	@Test
	void testOptimalSequence() {

		int[] Eingabe = new  int[]{ 4,7,2,3 };
//		int[] Eingabe = new  int[]{ 3,4,1,2,8,5 };

		RowOfBowls gainMax = new RowOfBowls();
		gainMax.maxGain(Eingabe);
		System.out.println(gainMax.optimalSequence());
	}

	@Test
	void testRandomSequence() {
		int[] Eingabe = new  int[]{ 3,4,1,2,8,5 };
		RowOfBowls gainMax = new RowOfBowls();
		gainMax.maxGain(Eingabe);
		System.out.println(gainMax.optimalSequence());
	}

}

