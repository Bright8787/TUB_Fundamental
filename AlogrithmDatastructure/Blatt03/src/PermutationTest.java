import java.util.LinkedList;
import java.util.Arrays;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

class PermutationTest {
	PermutationVariation p1;
	PermutationVariation p2;
	public int n1;
	public int n2;
	int cases=1;
	
	void initialize() {
		n1=4;
		n2=6;
		Cases c= new Cases();
		p1= c.switchforTesting(cases, n1);
		p2= c.switchforTesting(cases, n2);
	}
	///////////////////////////////////////////////////////////////
	int factorial(int n){
		int product = 1;
		if(n == 1) return product;
		for(int i = n ; i>0; i--){
			product = product * i;
		}
		return product;
	}
	@Test
	void testPermutation() {
		initialize();
		// TODO
		boolean hasDuplicateP1 = false;
		boolean hasDuplicateP2 = false;


		//test size of original array
		assertEquals(p1.original.length, n1);
		assertEquals(p2.original.length, n2);
		//check if contains duplicate
		System.out.println(Arrays.toString(p1.original));
		Arrays.sort(p1.original);
		System.out.println(Arrays.toString(p1.original));
		for (int i = 0; i < p1.original.length - 1; i++) {
			if (p1.original[i] == p1.original[i + 1]) {
				assertNotEquals(p1.original[i], p1.original[i + 1]);
				break;
			}
		}
		//check if the list is not declared
		try{
			assertNotNull(p1.allDerangements);
		} catch(RuntimeException e){
			assertTrue(true);
		}

		//check if allDearangement empty or not
		assertTrue(p1.allDerangements.isEmpty());

		System.out.println(Arrays.toString(p2.original));
		Arrays.sort(p2.original);
		System.out.println(Arrays.toString(p2.original));
		for (int i = 0; i < p2.original.length - 1; i++) {
			if (p2.original[i] == p2.original[i + 1]) {
				assertNotEquals(p2.original[i], p2.original[i + 1]);
				break;
			}
			//debug
			//System.out.println(p1.allDerangements.isEmpty());
			//System.out.println(p2.allDerangements.isEmpty());
		}
		//check if the list is not declared
		try{
			assertNotNull(p2.allDerangements);
		} catch(RuntimeException e){
			assertTrue(true);
		}

		//check if allDearangement empty or not
		assertTrue(p2.allDerangements.isEmpty());
	}

	@Test
	void testDerangements(){
		initialize();
		//in case there is something wrong with the constructor
		fixConstructor();

		// TODO
		double numberOfFreeFixedPermutation_n2 = Math.floor(factorial(n2) / Math.E) ;
		double numberOfFreeFixedPermutation_n1 = Math.floor(factorial(n1) / Math.E) ;

//		p1 = new Permutation(n1);
//		p2 = new Permutation1(n2);
		//permutate the list
		p1.derangements();
		p2.derangements();

		//debug
//		System.out.println(Arrays.toString(p2.original));
//		for(int[] element: p1.allDerangements) System.out.println(Arrays.toString(element));
//		for(int[] element: p2.allDerangements) System.out.println(Arrays.toString(element));

		//check if number of permutation is equal.
//		System.out.println(p1.allDerangements.size());
//		System.out.println(p2.allDerangements.size());
		assertEquals(p1.allDerangements.size()-1,(int)numberOfFreeFixedPermutation_n1);
		assertEquals(p2.allDerangements.size()-1,(int)numberOfFreeFixedPermutation_n2);

		//check if all answers are fixedpointFreedom
		for(int[] element: p1.allDerangements){
			for(int i = 0;i<n1;i++){
				assertNotEquals(element[i],p1.original[i]);
			}
		}
		for(int[] element: p2.allDerangements){
			for(int i = 0;i<n2;i++){
				assertNotEquals(element[i],p2.original[i]);
			}
		}

	}
	
	@Test
	void testsameElements() {
		initialize();
		//in case there is something wrong with the constructor
		fixConstructor();
		// TODO
		System.out.println(Arrays.toString(p2.original));

		p1.derangements();
		p2.derangements();


		//check if the array is permutated
		assertFalse(p1.allDerangements.isEmpty());
		assertFalse(p2.allDerangements.isEmpty());
		//compared when both are sorted
		for(int[] element: p1.allDerangements){
			Arrays.sort(element);
			assertArrayEquals(element,p1.original);
		}



		for(int[] element: p2.allDerangements){
			Arrays.sort(element);
			assertArrayEquals(element,p2.original);
		}

	}
	
	void setCases(int c) {
		this.cases=c;
	}
	
	public void fixConstructor() {
		//in case there is something wrong with the constructor
		p1.allDerangements=new LinkedList<int[]>();
		for(int i=0;i<n1;i++)
			p1.original[i]=2*i+1;
		
		p2.allDerangements=new LinkedList<int[]>();
		for(int i=0;i<n2;i++)
			p2.original[i]=i+1;
	}
}


