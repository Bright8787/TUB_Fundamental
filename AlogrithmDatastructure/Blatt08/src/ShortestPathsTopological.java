import java.util.Arrays;
import java.util.Stack;

public class ShortestPathsTopological {
    private int[] parent;
    private int s;
    private double[] dist;

    public ShortestPathsTopological(WeightedDigraph G, int s) {
        // TODO
        //set dist to almost Infinity
        dist = new double[G.V()];
        parent = new int[G.V()];

        for(int i = 0 ; i <G.V(); i++){
            dist[i] = Double.MAX_VALUE;
        }

        //call TopologicalWD to get the List instead of PriorityQueue
        TopologicalWD topologicalSort = new TopologicalWD(G);
        topologicalSort.dfs(s);
        Stack<Integer> order = topologicalSort.order();

        //start node
        dist[s] = 0.0;
        this.s = s;
        //dijkstra algo with topological order
        while(!order.isEmpty()){
            int v = order.pop();
//            System.out.println(v);
            for(DirectedEdge connectedEdge: G.incident(v)){
                relax(connectedEdge);
            }
        }
//        System.out.println(Arrays.toString(parent));

    }

    public void relax(DirectedEdge e) {
        // TODO
        int v = e.from();
        int w = e.to();
        if(dist[w] > dist[v] + e.weight()){
            dist[w] = dist[v] + e.weight();
            parent[w] = v;
        }
    }

    public boolean hasPathTo(int v) {
        return parent[v] >= 0;
    }

    public Stack<Integer> pathTo(int v) {
        if (!hasPathTo(v)) {
            return null;
        }
        Stack<Integer> path = new Stack<>();
        for (int w = v; w != s; w = parent[w]) {
            path.push(w);
        }
        path.push(s);
        return path;
    }


    public static void main(String[] args){


    }
}


