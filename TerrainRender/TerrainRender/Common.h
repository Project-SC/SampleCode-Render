#pragma once

inline float lerp(float a, float b, float t) 
{
	return a + t * (b - a);
}

inline float bilerp(float a, float b, float c, float d, float u, float v)
{
	return lerp(lerp(a, b, u), lerp(c, d, u), v);
}