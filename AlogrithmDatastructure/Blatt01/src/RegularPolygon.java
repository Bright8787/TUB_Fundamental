// Diese Klasse implementiert nur *zentrierte* reguläre Polygone, also mit midpoint = (0, 0).

import java.util.Arrays;

public class RegularPolygon extends ConvexPolygon {

    // TODO
    private  int N;
    private double radius;

    //constructor
    public RegularPolygon(int N, double radius) {
        // TODO
        super();

        this.N = N;
        this.radius = radius;
        double degree = 360.00/N;
        double radiant = Math.PI * degree / 180.00 ;
        double incremetDegree = 0;

        Vector2D[] ArrayOfVertexes = new Vector2D[N];
        for(int i = 0; i < N; i++){
            double x = radius * Math.cos(incremetDegree);
            double y = radius * Math.sin(incremetDegree);
            ArrayOfVertexes[i] = new Vector2D(x,y);
            incremetDegree = incremetDegree + radiant;
            System.out.println(Math.toDegrees(incremetDegree));
        }
        //debug
//        for(Vector2D x : ArrayOfVertexes) {
//            System.out.println(x);
//        }
        this.vertices = ArrayOfVertexes;

    }
    //constructor with input
    public RegularPolygon(RegularPolygon polygon) {
        // TODO
        super(polygon.vertices) ;
        N = polygon.N;
        radius = polygon.radius;

    }
    public void resize(double newradius) {
        // TODO
        double degree = 360.00/N;
        double radiant = Math.PI * degree / 180.00 ;

        radius = newradius;
                double incremetDegree = 0.00;
                Vector2D[] ArrayOfVertexes = new Vector2D[N];
                for(int i = 0; i < N; i++){
                    ArrayOfVertexes[i] = new Vector2D(radius * Math.cos(incremetDegree),radius * Math.sin(incremetDegree));
                    incremetDegree = incremetDegree + radiant;
                   System.out.println(Math.toDegrees(incremetDegree));
                }
                vertices = ArrayOfVertexes;
    }
    @Override
    public String toString() {
        // TODO
        return "RegularPolygon{" + "N=" + N +", radius="+ radius + "}";
    }
    public static void main(String[] args) {
        RegularPolygon pentagon = new RegularPolygon(5, 1);
        System.out.println("Der Flächeninhalt des " + pentagon + " beträgt " + pentagon.area() + " LE^2.");
//        RegularPolygon otherpentagon = pentagon;      // Dies funktioniert nicht!
        RegularPolygon otherpentagon = new RegularPolygon(pentagon);
        pentagon.resize(10);
        System.out.println("Nach Vergrößerung: " + pentagon + " mit Fläche " + pentagon.area() + " LE^2.");
        System.out.println("Die Kopie: " + otherpentagon + " mit Fläche " + otherpentagon.area() + " LE^2.");
        /*
        Die erwartete Ausgabe ist:
        Der Flächeninhalt des RegularPolygon{N=5, radius=1.0} beträgt 2.377641290737883 LE^2.
        Nach Vergrößerung: RegularPolygon{N=5, radius=10.0} mit Fläche 237.7641290737884 LE^2.
        Die Kopie: RegularPolygon{N=5, radius=1.0} mit Fläche 2.377641290737883 LE^2.
         */
    }
}
