#pragma once
#include <math.h>

class Vector2f
{
public:
	Vector2f()
	{
		this->x = 0;
		this->y = 0;
	}

	Vector2f(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Vector2f(float ang)
	{
		this->x = cos(ang);
		this->y = sin(ang);
	}

	float ScalProd(Vector2f v1, Vector2f v2)
	{
		return (v1.x * v2.x + v1.y * v2.y);
	}

	Vector2f Norm()
	{
		return Vector2f(x / this->Length(), y / this->Length());
	}

	float Length()
	{
		return (sqrt(x * x + y * y));
	}

	~Vector2f()
	{
		//std::cout << "destructor\n";
	}

	float x;
	float y;
};	

Vector2f operator +(Vector2f v1, Vector2f v2)
{
	return Vector2f(v1.x + v2.x, v1.y + v2.y);
}

Vector2f operator *(Vector2f vec, double value)
{
	return Vector2f(vec.x * value, vec.y * value);
}

Vector2f operator *(float value, Vector2f vec)
{
    return Vector2f(vec.x * value, vec.y * value);
}

Vector2f operator -(Vector2f v1, Vector2f v2)
{
	return Vector2f(v1.x - v2.x, v1.y - v2.y);
}

float operator ^ (Vector2f v1, Vector2f v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}
