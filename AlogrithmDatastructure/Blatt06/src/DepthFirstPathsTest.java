import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

import java.util.Arrays;

class DepthFirstPathsTest {

	@Test
	void testDfs() {
		Graph tester = new Graph(6);
		tester.addEdge(0,1);
		tester.addEdge(4,3);
		tester.addEdge(3,5);
		tester.addEdge(1,4);
		tester.addEdge(2,3);

		tester.addEdge(0,2);
		tester.addEdge(0,3);
		tester.addEdge(4,5);

		System.out.println(tester.toString());

		DepthFirstPaths dfsTest = new DepthFirstPaths( tester,0);
		dfsTest.dfs(tester);
		System.out.println(dfsTest.pre());
		System.out.println(dfsTest.post());
		System.out.println(Arrays.toString(dfsTest.edge()));
		System.out.println(Arrays.toString(dfsTest.dist()));

//		[0, 1, 4, 3, 5, 2]
//		[5, 2, 3, 4, 1, 0]
//		[0, 0, 3, 4, 1, 3]
//		[0, 1, 4, 3, 2, 4]

//
//		for(int s : dfsTest.pre()) {
//			System.out.println(s);
//		}

	}

	@Test
	void testNonrecursiveDFS() {

		System.out.println("Non Recursive");
		Graph tester = new Graph(6);
		tester.addEdge(0,1);
		tester.addEdge(4,3);
		tester.addEdge(3,5);
		tester.addEdge(1,4);
		tester.addEdge(2,3);

		tester.addEdge(0,2);
		tester.addEdge(0,3);
		tester.addEdge(4,5);

		DepthFirstPaths dfsTest2 = new DepthFirstPaths( tester,0);

		dfsTest2.nonrecursiveDFS(tester);
		System.out.println(dfsTest2.pre());
		System.out.println(dfsTest2.post());
		System.out.println(Arrays.toString(dfsTest2.edge()));
		System.out.println(Arrays.toString(dfsTest2.dist()));
	}

	@Test
	void testPathTo() {

		Graph tester = new Graph(6);
		tester.addEdge(0,1);
		tester.addEdge(4,3);
		tester.addEdge(3,5);
		tester.addEdge(1,4);
		tester.addEdge(2,3);

		tester.addEdge(0,2);
		tester.addEdge(0,3);
		tester.addEdge(4,5);

		System.out.println(tester);
		DepthFirstPaths dfsTest = new DepthFirstPaths( tester,0);
		dfsTest.dfs(tester);
		System.out.println(dfsTest.pre());
		System.out.println(dfsTest.post());
		System.out.println(Arrays.toString(dfsTest.dist()));

		System.out.println(Arrays.toString(dfsTest.edge()));
		System.out.println(dfsTest.pathTo(5));

	}

}

