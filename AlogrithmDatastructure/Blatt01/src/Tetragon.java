// TODO
public class Tetragon extends ConvexPolygon {

    //constructor
    public Tetragon(Vector2D a, Vector2D b, Vector2D c, Vector2D d) {
        super(new Vector2D[]{a, b, c, d});
    }

    public static void main(String[] args) {
        Polygon tetra = new Tetragon(new Vector2D(1,2),new Vector2D(1,2),new Vector2D(1,2),new Vector2D(1,2));
        System.out.println(tetra);
    }
}
