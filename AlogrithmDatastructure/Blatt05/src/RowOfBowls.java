import java.sql.Array;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * This class implements a game of Row of Bowls.
 * For the games rules see Blatt05. The goal is to find an optimal strategy.
 */
public class RowOfBowls {
    public int[] input;
    private int[][] dp;

    public RowOfBowls() {;

    }
    public int totalArray(int[] values) {
        int totalArray = 0;

        for (int i = 0; i < values.length; i++) {
            totalArray += values[i];

        }
        return totalArray;
    }
    /**
     * Implements an optimal game using dynamic programming
     * @param values array of the number of marbles in each bowl
     * @return number of game points that the first player gets, provided both parties play optimally
     */
    public int maxGain(int[] values)
    {
        // TODO
        this.input = new int[values.length];
        System.arraycopy(values, 0, this.input, 0, values.length);
        this.dp = new int[values.length][values.length];
        // Base cases: When there's only one bowl, the player picks that bowl.
        for (int length = 1; length <= values.length; length++) {
            for (int i = 0; i <= values.length - length; i++) {
                int j = i + length - 1;
                if(length == 1){
                    this.dp[i][i] = values[i];
                }
                else{
                    this.dp[i][j] = Math.max(this.dp[i][i] - this.dp[i+1][j], this.dp[j][j] - this.dp[i][j-1]);
                }
            }
        }
        return this.dp[0][values.length - 1];
    }
        /**
         * Implements an optimal game recursively.
         *
         * @param values array of the number of marbles in each bowl
         * @return number of game points that the first player gets, provided both parties play optimally
         */
    public int maxGainRecursive(int[] values) {
        // TODO
        int score = 0;
        score = maxGainNegamax(1, values, 0, values.length - 1, Integer.MIN_VALUE, Integer.MAX_VALUE);
        System.out.println(score);
        return score;
    }

    public static int maxGainNegamax(int player, int[] values, int left, int right, int alpha, int beta) {
        if (left > right) {
            return 0;
        }
        int maxScore = Integer.MIN_VALUE;

        // Pick leftmost element
        int scoreLeft = values[left] - maxGainNegamax(-player, values, left + 1, right, -beta, -alpha);
        maxScore = Math.max(maxScore, scoreLeft);
        alpha = Math.max(alpha, scoreLeft);

        // Pick rightmost element
        int scoreRight = values[right] - maxGainNegamax(-player, values, left, right - 1, -beta, -alpha);
        maxScore = Math.max(maxScore, scoreRight);
        alpha = Math.max(alpha, scoreRight);

        if (alpha >= beta) {
            return maxScore;  // Alpha-beta pruning
        }

        return maxScore;
    }    /**
     * Calculates an optimal sequence of bowls using the partial solutions found in maxGain(int values)
     * @return optimal sequence of chosen bowls (represented by the index in the values array)
     */
    public Iterable<Integer> optimalSequence()
    {
        // TODO
        List<Integer> sequence = new ArrayList<>();
        int n = this.input.length;
        int posx = 0,posy = n-1;
        int best = this.dp[0][n-1];
        for (int i = 0; i < n; i++) {
            if(posx == posy) sequence.add(posx);
            else{
                if (this.dp[posy][posy] - this.dp[posx][posy - 1] == best) {
                    best = this.dp[posx][posy - 1];
                    sequence.add(posy);
                    posy = posy-1;
                } else  if(this.dp[posx][posx] - this.dp[posx+1][posy] == best){
                    best = this.dp[posx + 1][posy];
                    sequence.add(posx);
                    posx = posx+1;
                }
            }
        }
        return sequence;
    }

    private static void printMatrix(int[][] matrix, int n) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                System.out.print(matrix[i][j] + "\t");
            }
            System.out.println();
        }
    }

    public static void main(String[] args)
    {
        // For Testing
        
    }
}

