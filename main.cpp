#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

const int width = 1500;
const int height = 800;

const float dx = 0.1;
const float dt = dx;
const float s = 1 * dt * dt / (dx * dx);
const int len2 = 1000;
const int len = 250;

//negate whatever you want

const float lowerbound = dx * len / 2;

const float uscale = 1.1;
const float dscale = 0.9;

int t = 0;
using namespace sf;

float IC_U(float x)
{
	//shift to left with - lowerbound
	//return 5 * exp(-((x - lowerbound) * (x - lowerbound)));
	//return 5 * (x - lowerbound);
	//return 5 * exp(-((x - lowerbound) * (x - lowerbound))) * sin(5 * (x - lowerbound));
	return 0;
	//float h = 10;
	//return h - h / (lowerbound * lowerbound) * (x - lowerbound) * (x - lowerbound);
	//return 5 * exp(-(x - lowerbound) * (x - lowerbound)) * sin(x - lowerbound);
}

float IC_dU(float x)
{
	//return 30 * exp(-((x - lowerbound) * (x - lowerbound)));
	//return 2 * exp(-((x - lowerbound) * (x - lowerbound)));
	return 0;
}

float BC_0(float x, float t)
{
	//return 10 * exp(-0.1 * t) * sin(t);
	return sin(t);
	//return 0;
}

float BC_1(float x, float t)
{
	return -sin(0.9 * t);
	//return 0;
}

struct graph
{
	// t, x
	Vector2f points[len2][len];
	Vector2f c;

	graph() 
	{
		for (int i = 0; i < len; i++)
		{
			points[0][i] = Vector2f(i * dx /* - lowerbound */, IC_U(i * dx /* - lowerbound */));
		}

		for (int i = 0; i < len; i++)
		{
			// if (i > 0 && i < len - 1)
			// {
				points[1][i] = Vector2f(i * dx /* - lowerbound */, s / 2 * (IC_U((i + 1) * dx /* - lowerbound */) + IC_U((i - 1) * dx /* - lowerbound */)) + (1 - s) * (IC_U(i * dx) /* - lowerbound */) + IC_dU(i * dx /* - lowerbound */) * dt);
			// }
			// else if (i < 1)
			// {
			// 	points[1][i] = Vector2f(i * dx /* - lowerbound */, s / 2 (IC_U((i + 1) * dx /* - lowerbound */)) + (1 - s) * (IC_U(i * dx)) + IC_dU(i * dx) * dt);
			// }
			// else if (i > len - 2)
			// {
			// 	points[1][i] = Vector2f(i * dx /* - lowerbound */, s / 2 (IC_U((i - 1) * dx /* - lowerbound */)) + (1 - s) * (IC_U(i * dx)) + IC_dU(i * dx) * dt);
			// }
		}

		for (int t = 2; t < len2; t++)
		{
			for (int x = 0; x < len; x++)
			{
				// std::cout << t << " " << x << std::endl;
				if (x > 0 && x < len - 1)
				{
					points[t][x] = Vector2f(x * dx /* - lowerbound */, s * (points[t - 1][x + 1].y + points[t - 1][x - 1].y) + 2 * (1 - s) * points[t - 1][x].y - points[t - 2][x].y);
				}
				else if (x < 1)
				{
					points[t][x] = Vector2f(x * dx /* - lowerbound */, s * (points[t - 1][x + 1].y + BC_0(x * dx, t * dt)) + 2 * (1 - s) * points[t - 1][x].y - points[t - 2][x].y);
				}
				else if (x > len - 2)
				{
					points[t][x] = Vector2f(x * dx /* - lowerbound */, s * (BC_1(x * dx, t * dt) + points[t - 1][x - 1].y) + 2 * (1 - s) * points[t - 1][x].y - points[t - 2][x].y);
				}
			}
		}
	};

	void scale(float a)
	{			
		for (int t = 0; t < len2; t++)
		{
			for (int x = 0; x < len; x++)
			{
				Vector2f v(points[t][x]);
				points[t][x] = Vector2f(v.x * a, v.y * a);
			}
		}
	}
};

int main()
{
	sf::RenderWindow w(sf::VideoMode(width, height), "partial diff 1d");
	w.setFramerateLimit(60);

	Vector2f c(0, 0);

	graph g = graph();
	VertexArray line(Lines, 2);

	while (w.isOpen())
	{
		sf::Event event;

		while (w.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				w.close();

			// if (event.type == sf::Event::KeyPressed)
			// {
			// 	if (event.key.code == Keyboard::Up)
			// 	{
			// 		if (t < len2 - 1)
			// 			t++;
			// 	}
			// 	if (event.key.code == Keyboard::Down)
			// 	{
			// 		if (t > 0)
			// 			t--; 
			// 	}
			// }
		}

		if (Mouse::isButtonPressed(Mouse::Left)) { c.x = Mouse::getPosition(w).x - width / 2 - g.points[t][len / 2].x; c.y = Mouse::getPosition(w).y - height / 2; }

		if (Keyboard::isKeyPressed(Keyboard::Up)) 
		{ 
			if (t < len2 - 1)
				t++;
		}
		if (Keyboard::isKeyPressed(Keyboard::Down)) 
		{ 
			if (t > 0)
				t--; 
		}

		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
			g.scale(uscale);
		}
		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			g.scale(dscale);
		}
		
		w.clear();


		for (int i = 0; i < len - 1; i++)
		{
			line[0].position = Vector2f(g.points[t][i].x + width / 2 + c.x, -g.points[t][i].y + height / 2 + c.y);
			line[1].position = Vector2f(g.points[t][i + 1].x + width / 2 + c.x, -g.points[t][i + 1].y + height / 2 + c.y);

			w.draw(line);
		}

		w.display();
	}

	return 0;
}