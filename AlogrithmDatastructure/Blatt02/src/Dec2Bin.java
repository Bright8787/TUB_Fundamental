import java.util.Stack;
/** * A class for constructing a Decimal-to-Binary Number- Converter; * contains a main method for demonstration. */
public class Dec2Bin {

    public Stack<Integer> binStack;  // We make it public to modify it in our tests.
    private int N;

    /**
     * Constructor of an empty object. Use method {@code convert()} to convert a number.
     */
    public Dec2Bin() {
        binStack = new Stack<>();
    }

    /**
     * Returns the number that is converted as {@code int}.
     *
     * @return the converted number
     */
    public int getN() {
        return N;
    }

    /**
     * Converts the given number into binary format, with each digit being represented in a
     * stack of {@code int}.
     *
     * @param N the number that is to be converted.
     */
    public void convert(int N) {
        // TODO implement this method
        this.N = N;
        binStack.clear();
        if(N==0)
        {
            binStack.push(0);
        }
        while(N>0){
            binStack.push(N % 2);
            N = N / 2;
        }
    }

    /**
     * Returns the digits that are stored in {@code binStack} as a string. To is the binary format of the
     * converted number.
     * For testing purpose, we require that the function works also, if the variable {@code binStack} is
     * modified externally.
     *
     * @return a string representation of the number in binary format.
     */
    @Override
    public String toString() {
        // Caution: Stack.toString() does NOT respect stack order. Do not use it.
        // TODO implement this method
        String reversed = "";
        char ch;
        String output = "";

        for(int element: binStack){
            System.out.println(element);
        }
        //Convert each element in stack to string reversely
        for(int element: binStack){
            reversed += Integer.toString(element);
        }
        for(int i = 0; i<reversed.length();i++){
            ch = reversed.charAt(i);
            output = ch+output;
        }
        return output;

    }

    public static void main(String[] args) {
        Dec2Bin dec2bin = new Dec2Bin();
        dec2bin.convert(0);

        System.out.println("Die Zahl " + dec2bin.getN() + " in Binärdarstellung: " + dec2bin);
        // Do it another time to demonstrate that toString does not erase the binStack.
        System.out.println("Die Zahl " + dec2bin.getN() + " in Binärdarstellung: " + dec2bin);

        dec2bin.convert(4);

        System.out.println("Die Zahl " + dec2bin.getN() + " in Binärdarstellung: " + dec2bin);
        // Do it another time to demonstrate that toString does not erase the binStack.
        System.out.println("Die Zahl " + dec2bin.getN() + " in Binärdarstellung: " + dec2bin);
    }
}

