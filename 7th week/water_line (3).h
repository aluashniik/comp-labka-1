//
//  water_line.h
//  firstSketch
//
//  Created by ¹Îµ¿Çö on 26/09/2019.
//

#ifndef water_line_h
#define water_line_h

#include "ofMain.h"

#define EPSILON 0.01

struct Dot {
	int x1;
	int y1;
};

struct LineSegment {
	int x1;
	int y1;
	int x2;
	int y2;
	double slope;
	double x_coef;
	double y_coef;
	double constant;
};


class WaterLine {

public:
	WaterLine(int num_of_line);
	~WaterLine();

	void reset();
	void update();
	void draw();
	void calculate_path(LineSegment *lineseg, int num_of_line);

	Dot* path;

	float uniqueColor_r, uniqueColor_g, uniqueColor_b;
	float scale;
	int hexcolor;

	int path_idx;
	int draw_complete;
	int calc_complete;
	ofPoint start_dot;
	float dot_radius;

};

#endif /* water_line_h */

