package ol69.ws3.shapes;

public class Circle extends Shape{
	private double radius;
	
	public Circle() {
		this.radius = 1.0;
	}
	
	public Circle(double radius) {
		this.radius = radius;
	}
	
	public Circle(double radius, String color, boolean filled) {
		super(color, filled);
		this.radius = radius;
	}

	public double getRadius() {
		return radius;
	}

	public void setRadius(double radius) {
		this.radius = radius;
	}
	
	public double getArea() {
		return Math.pow(radius, 2) * Math.PI; 
	}
	
	public double getPerimeter() {
		return radius * 2 * Math.PI;
	}
	
	@Override
	public String toString() {
		return "A Circle with radius " + this.radius + " ,which is a subclass of " + super.toString() ;
	}
}
