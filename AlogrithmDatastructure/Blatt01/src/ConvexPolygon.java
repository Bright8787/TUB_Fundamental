import javax.annotation.processing.Generated;
import java.util.Arrays;


public class ConvexPolygon extends Polygon {
    // TODO
    @Override
    public String toString() {
        return "ConvexPolygon(" +
                  Arrays.toString(vertices) +
                ')';
    }

    //Default constructor
     public ConvexPolygon(){

    }
    //Constructor with vertexes
    public ConvexPolygon(Vector2D[] vertices){
        this.vertices = vertices;
    }

    @Override
     public double area(){
        double area = 0;
        //Edges from 0 to n-1 Vertex
        for(int i = 0 ; i<vertices.length-2; i++ ){
            //Last Vertex to i Vertex
            Vector2D vektor1 = new Vector2D(vertices[vertices.length-1].getX() - vertices[i].getX(),
                    vertices[vertices.length-1].getY() - vertices[i].getY());
            //Last Vertex to i+1 Vertex
            Vector2D vektor2 = new Vector2D(vertices[vertices.length-1].getX() - vertices[i+1].getX(),
                    vertices[vertices.length-1].getY() - vertices[i+1].getY());
            //i Vertex to i+1 Vertex
            Vector2D vektor3 = new Vector2D(vertices[i].getX() - vertices[i+1].getX(),
                    vertices[i].getY() - vertices[i+1].getY());
            //all edges
            double edge1 = vektor1.length();
            double edge2 = vektor2.length();
            double edge3 = vektor3.length();
            //Hero Formular
            double s = (edge1 + edge2 + edge3)/2;
             area += Math.sqrt(s * (s-edge1) * (s-edge2) * (s-edge3));
        }
        return area;
    }

    @Override
    public double perimeter(){
        double perim = 0;
        //Edges from 0 to n-1 Vertex
        for(int i = 0 ; i<vertices.length-1; i++ ){
            Vector2D edge = new Vector2D(vertices[i].getX() - vertices[i+1].getX(),
                    vertices[i].getY() - vertices[i+1].getY());
            perim = perim + edge.length();
        }
        //Edge from n to 0 Vertex
        Vector2D startAndEndEdge = new Vector2D(vertices[vertices.length-1].getX() - vertices[0].getX(),
                vertices[vertices.length-1].getY() - vertices[0].getY());
        perim = perim + startAndEndEdge.length();
        return perim;
    }
    public static Polygon[] somePolygons(){
        Polygon[] Polygons = new Polygon[4];
        //Triangle
        Polygons[0] = new ConvexPolygon(new Vector2D[] {new Vector2D(0,0), new Vector2D(10,0), new Vector2D(5,5)});
        //Square
        Polygons[1] = new ConvexPolygon(new Vector2D[] {new Vector2D(0,0), new Vector2D(10,-5), new Vector2D(12,2) ,new Vector2D(3,17)});
        //pentagon
        Polygons[2] = new RegularPolygon(5,1);
        //hexagon
//        Polygons[3] = new ConvexPolygon(new Vector2D[] {
//                new Vector2D(1,0),
//                new Vector2D(Math.cos(Math.toRadians(60)),Math.sin(Math.toRadians(60))),
//                new Vector2D(Math.cos(Math.toRadians(120)),Math.sin(Math.toRadians(120))) ,
//                new Vector2D(Math.cos(Math.toRadians(180)),Math.sin(Math.toRadians(180))),
//                new Vector2D(Math.cos(Math.toRadians(240)),Math.sin(Math.toRadians(240))),
//                new Vector2D(Math.cos(Math.toRadians(300)),Math.sin(Math.toRadians(300))),
//                });
        Polygons[3] =  new RegularPolygon(6,1);

        return Polygons;
    }
        public static double totalArea(Polygon[] polygons) {

        double totalArea = 0;
        for(Polygon polygon : polygons ){
            totalArea += polygon.area();
        }
        return totalArea;
    }
    public static void main(String[] args) {

        Vector2D a = new Vector2D(0,0);
        Vector2D b = new Vector2D(10,0);
        Vector2D c = new Vector2D(5,5);
        //Square
//        Vector2D d = new Vector2D(1,1);
//        Vector2D e = new Vector2D(-1,1);
//        Vector2D f = new Vector2D(1,-1);
//        Vector2D g = new Vector2D(-1,-1);

        Polygon polygon = new ConvexPolygon(new Vector2D[] {a, b, c});
        //Square
//        Polygon square = new ConvexPolygon(new Vector2D[] {d, e, f,g});

//        Polygon[] polygons = somePolygons();
//        double totalAreaOfPolygons = totalArea(polygons);
        Polygon[] x = somePolygons();
//        System.out.println(polygon);
//        System.out.println(square.area());
//        System.out.println(polygon.perimeter());
//        System.out.println(polygon.area());
//        for(Polygon element: x){
//            System.out.println(element);
//        }
//        System.out.println(totalAreaOfPolygons);
    }
}


//ConvexPolygon([(1.0, 0.0), (0.30901699437494745, 0.9510565162951535), (-0.8090169943749473, 0.5877852522924732), (-0.8090169943749476, -0.587785252292473), (0.30901699437494723, -0.9510565162951536)])
//ConvexPolygon([(1.0, 0.0), (0.30901699437494745, 0.9510565162951535), (-0.8090169943749473, 0.5877852522924732), (-0.8090169943749476, -0.587785252292473), (-0.30901699437494723, -0.9510565162951536)])
