#pragma once
#include "rtweekend.h"

class camera {
public:
	camera() {
		auto aspect_ratio = 16.0 / 9.0;
		auto viewport_height = 2.0;
		auto viewport_width = aspect_ratio * viewport_height;
		auto focal_lenght = 1.0;

		origin = point3(0, 0, 0);
		horizontal = vec3(viewport_width, 0, 0);
		vertical = vec3(0, viewport_height, 0);
		lower_left_corner = point3(origin - horizontal / 2.0 - vertical / 2.0 - vec3(0, 0, focal_lenght));
	}
	ray get_ray(double u, double v)const {
		return ray(origin, lower_left_corner + u * horizontal + v * vertical-origin);
	}

private:
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
};