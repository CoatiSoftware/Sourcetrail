class Point
{
public:
	Point(int x, int y)
		: x(x)
		, y(y)
	{
	}

	int x;
	int y;
};


class Shape
{
public:
	virtual void draw() = 0;
};


class Line
	: public Shape
{
public:
	Line(Point start, Point end)
		: start(start)
		, end(end)
	{
	}

	void draw()
	{
		// line drawing
	}

private:
	Point start;
	Point end;
};


class Circle
	: public Shape
{
public:
	Circle(Point center, int radius)
		: center(center)
		, radius(radius)
	{}

	void draw()
	{
		// circle drawing
	}

private:
	Point center;
	int radius;
};


class Image
{
public:
	Image(int width, int height)
		: width(width)
		, height(height)
		, idx(0)
	{
		shapes = new Shape*[10];
	}

	~Image()
	{
		for (int i = 0; i < idx; i++)
		{
			delete shapes[i];
		}

		delete [] shapes;
	}

	void addShape(Shape* shape)
	{
		if (idx < 10)
		{
			shapes[idx] = shape;
			idx++;
		}
	}

	void draw()
	{
		for (int i = 0; i < idx; i++)
		{
			shapes[i]->draw();
		}
	}

private:
	Shape** shapes;
	int idx;

	int width;
	int height;
};


void drawStickMan()
{
	Image stickman(100, 200);

	// head
	stickman.addShape(new Circle(Point(50, 50), 25));

	// torso
	stickman.addShape(new Line(Point(50, 75), Point(50, 180)));

	// arms
	stickman.addShape(new Line(Point(50, 100), Point(10, 125)));
	stickman.addShape(new Line(Point(50, 100), Point(90, 125)));

	// legs
	stickman.addShape(new Line(Point(50, 100), Point(10, 125)));
	stickman.addShape(new Line(Point(50, 100), Point(90, 125)));

	stickman.draw();
}
