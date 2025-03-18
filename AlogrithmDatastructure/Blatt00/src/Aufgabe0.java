import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Scanner;

public class Aufgabe0 {


	public void print() throws FileNotFoundException {

		Scanner in = new Scanner(new FileInputStream("Abgaben.txt"));
		while (in.hasNext()) {
			System.out.println(in.nextLine());
		}
		in.close();
	}

	public static void main(String[] args){
		try{
			Aufgabe0 printText = new Aufgabe0();
			printText.print();
		}
		//if file is not found
		catch (FileNotFoundException ex){

		}
	}
}

