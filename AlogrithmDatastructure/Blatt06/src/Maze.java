import java.util.*;

/**
 * Class that represents a maze with N*N junctions.
 * 
 * @author Vera Röhr
 */
public class Maze {
    private final int N;
    private Graph M;    //Maze
    public int startnode;

    public Maze(int N, int startnode) {

        if (N < 0) throw new IllegalArgumentException("Number of vertices in a row must be nonnegative");
        this.N = N;
        this.M = new Graph(N * N);
        this.startnode = startnode;
        buildMaze();
    }

    public Maze(In in) {
        this.M = new Graph(in);
        this.N = (int) Math.sqrt(M.V());
        this.startnode = 0;
    }


    /**
     * Adds the undirected edge v-w to the graph M.
     *
     * @param v one vertex in the edge
     * @param w the other vertex in the edge
     * @throws IllegalArgumentException unless both {@code 0 <= v < V} and {@code 0 <= w < V}
     */
    public void addEdge(int v, int w) {
        // TODO
        M.addEdge(v, w);
    }



    /**
     * Returns true if there is an edge between 'v' and 'w'
     *
     * @param v one vertex
     * @param w another vertex
     * @return true or false
     */
    public boolean hasEdge(int v, int w) {
        // TODO
        int V = N*N;
        //check if in range
        if (v < 0 || v >= V)
            return false;
        if (w < 0 || w >= V)
           return false;
        //use standard Library
        //check reflex
        if(v == w) return true;
        else{
            return M.adj(v).contains(w) || M.adj(w).contains(v) ;
        }
    }

    /**
     * Builds a grid as a graph.
     *
     * @return Graph G -- Basic grid on which the Maze is built
     */
    public Graph mazegrid() {
        int numberOfVertices = N * N;
        Graph G = new Graph(numberOfVertices);

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int index = i * N + j;
                if (j < N - 1) {
                    // Connect to the right neighbor
                    G.addEdge(index, index + 1);
                }
                if (i < N - 1) {
                    // Connect to the bottom neighbor
                    G.addEdge(index, index + N);
                }
            }
        }

        return G;
    }


    /**
     * Builds a random maze as a graph.
     * The maze is build with a randomized DFS as the Graph M.
     */
    private void buildMaze() {
        // TODO
        Graph G = mazegrid();
        //randomFirstPath on nxn Grid;
        RandomDepthFirstPaths Labyrith = new RandomDepthFirstPaths(G, startnode);
        Labyrith.randomNonrecursiveDFS(G);
        //get the sequence from randomDFS and Add it to graph M
        int[] path = Labyrith.edge();
       for(int i = 1; i < Labyrith.edge().length ; i++) {
            if ( !(hasEdge(i, path[i])) && path[i] != i  && Labyrith.hasPathTo(i) ) {
                System.out.println(path[i]);
                M.addEdge(i, path[i]);
            }
        }
        System.out.println(M);
    }
        /**
         * @return Graph M
         */

        public Graph M () {
            return M;
        }

    /**
     * Find a path from node v to w
     * @param v start node
     * @param w end node
     * @return List<Integer> -- a list of nodes on the path from v to w (both included) in the right order.
     */

    public List<Integer> findWay(int v, int w){
        // Breitensuch
        boolean[] marked = new boolean[this.M.V()];
        int[] edgeTo = new int[this.M.V()];
        Queue<Integer> Q = new LinkedList<Integer>();

        marked[v] = true;
        Q.add(v);
//        System.out.println(M.toString());
        while (!Q.isEmpty()) {
            int x = Q.remove();
            for (int y: M.adj(x)) {
                if (!marked[y] && hasEdge(x,y)) {
                    edgeTo[y] = x;
                    marked[y] = true;
                    Q.add(y);
                }
            }
        }

//       System.out.println(Arrays.toString(edgeTo));

        List<Integer> path = new ArrayList<>();
        path.add(w); // Beginning
        int move = edgeTo[w];
        while (move != v) {
            path.add(move);
            move = edgeTo[move];
        }
        path.add(v); // End
        return path.reversed();
    }
//    public List<Integer> findWay ( int v, int w){
//        // TODO
//        //same approach as DFS
//        Iterator<Integer>[] adj = (Iterator<Integer>[]) new Iterator[M.V()];
//        for (int i = 0; i < M.V(); i++)
//            adj[i] = M.adj(i).iterator();
//
//        boolean[] marked = new boolean[M.V()];
//        ArrayList preorder = new ArrayList();
//
//        // depth-first search using an explicit stack
//        Stack<Integer> stack = new Stack<Integer>();
//        marked[v] = true;
//        //and the first vertex thorugh the list
//        preorder.add(v);
//        stack.push(v);//            Sys
//        //iterate through all vertices and add it to the list if not visited
//        while (!stack.isEmpty()) {
//            int i = stack.peek();
//            if (adj[i].hasNext()) {
//                int j = adj[i].next();
//                if (!marked[j]) {
//                    // discovered vertex w for the first time
//                    preorder.add(j);
//                    marked[j] = true;
//                    stack.push(j);
//                }
//                if(j == w){
//                    break;
//                }
//            }
//            else {
//                stack.pop();
//            }
//        }
//
//        //add the w vertex to the list
//        return preorder;
//    }

        public static void main (String[]args){
            // FOR TESTING
            Maze maze = new Maze(20, 0);
            GridGraph vis = new GridGraph(maze.M);


        }
}

