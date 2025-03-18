import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

import java.util.Arrays;

class RandomDepthFirstPathsTest {

	@Test
	void testRandomDFS() {
		fail("Not yet implemented");
	}

	@Test
	void testRandomNonrecursiveDFS() {

		Graph tester = new Graph(6);
		tester.addEdge(0,1);
		tester.addEdge(1,4);
		tester.addEdge(4,3);
		tester.addEdge(3,5);
		tester.addEdge(2,3);

		tester.addEdge(0,2);
		tester.addEdge(0,3);
		tester.addEdge(4,5);

		RandomDepthFirstPaths testRandDFS = new RandomDepthFirstPaths(tester,0);

		testRandDFS.randomDFS(tester);
		System.out.println(tester);
		System.out.println(testRandDFS.preorder);
		System.out.println(testRandDFS.postorder);


	}

	@Test
	void testPathTo() {
		fail("Not yet implemented");
	}

}

