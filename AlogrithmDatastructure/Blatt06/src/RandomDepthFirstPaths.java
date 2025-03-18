import org.junit.jupiter.params.shadow.com.univocity.parsers.annotations.Copy;

import java.util.*;

public class RandomDepthFirstPaths {
    private boolean[] marked;    // marked[v] = is there an s-v path?
    private int[] edgeTo;        // edgeTo[v] = last edge on s-v path
    private int[] distTo;      // distTo[v] = number of edges s-v path
    private final int s;
    public Queue<Integer> preorder;   // vertices in preorder
    public Queue<Integer> postorder;// source vertex

    /**
     * Computes a path between {@code s} and every other vertex in graph {@code G}.
     *
     * @param G the graph
     * @param s the source vertex
     * @throws IllegalArgumentException unless {@code 0 <= s < V}
     */
    public RandomDepthFirstPaths(Graph G, int s) {
        this.s = s;
        edgeTo = new int[G.V()];
        marked = new boolean[G.V()];
        validateVertex(s);

        distTo = new int[G.V()];
        postorder = new LinkedList<Integer>();
        preorder = new LinkedList<Integer>();
    }

    public void randomDFS(Graph G) {
        randomDFS(G, s);
    }

    // depth first search from v
    private void randomDFS(Graph G, int v) {
        // TODO
        randomdfs(G, v, 0);
    }

    private void randomdfs(Graph G, int v, int track) {
        //TODO: Zeilen hinzufuegen
        if (marked[v] == true)
            distTo[v] -= 1;
        marked[v] = true;
        distTo[v] = track;
        track++;
        Collections.shuffle(G.adj(v));
        for (int w : G.adj(v)) {
            if (!marked[w]) {
                preorder.add(v);
                edgeTo[w] = v;
                randomdfs(G, w, track);
            }
        }
        postorder.add(v);

    }

    public void randomNonrecursiveDFS(Graph G) {
        // TODO

        Iterator<Integer>[] adj = (Iterator<Integer>[]) new Iterator[G.V()];
        for (int v = 0; v < G.V(); v++){
            LinkedList<Integer> Copy = (LinkedList<Integer>) G.adj(v).clone();
            Collections.shuffle(Copy);
            adj[v] = Copy.iterator();
        }
        // depth-first search using an explicit stack
        Stack<Integer> stack = new Stack<>();
        marked[s] = true;
        preorder.add(s);
        stack.push(s);

        while (!stack.isEmpty()) {
            int v = stack.peek();
            if (adj[v].hasNext()) {
                int w = adj[v].next();
                if (!marked[w]) {
                    // discovered vertex w for the first time
                    marked[w] = true;
                    preorder.add(w);
                    edgeTo[w] = v;
                    distTo[w] = stack.size();
                    stack.push(w);
                }
            } else {
                postorder.add(stack.pop());
            }
        }
    }


    /**
     * Is there a path between the source vertex {@code s} and vertex {@code v}?
     * @param v the vertex
     * @return {@code true} if there is a path, {@code false} otherwise
     * @throws IllegalArgumentException unless {@code 0 <= v < V}
     */
    public boolean hasPathTo(int v) {
        validateVertex(v);
        return marked[v];
    }

    /**
     * Returns a path between the source vertex {@code s} and vertex {@code v}, or
     * {@code null} if no such path.
     * @param  v the vertex
     * @return the sequence of vertices on a path between the source vertex
     *         {@code s} and vertex {@code v}, as an Iterable
     * @throws IllegalArgumentException unless {@code 0 <= v < V}
     * 
     * This method is different compared to the original one.
     */
    public List<Integer> pathTo(int v) {
		// TODO
        ArrayList<Integer> path = new ArrayList<>();
        path.add(v);
        int temp = 0;
        while(v != s){
            path.add(edgeTo[v]);
//            System.out.println(edgeTo[v]);
            temp = edgeTo[v];
            v = temp;
        }
        return  path;
    }
    
    public int [] edge() {
    	return edgeTo;
    }  

    // throw an IllegalArgumentException unless {@code 0 <= v < V}
    private void validateVertex(int v) {
        int V = marked.length;
        if (v < 0 || v >= V)
            throw new IllegalArgumentException("vertex " + v + " is not between 0 and " + (V-1));
    }

}

