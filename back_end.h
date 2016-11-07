#pragma once
#include <math.h>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

const double pi = 3.141592653589793;

/*COMPLEX NUMBER CLASS*/
class cmplx {
public:
	double real, imag;

	cmplx() {
		real = 0.0;
		imag = 0.0;
	}

	cmplx(double r, double i) {
		real = r;
		imag = i;
	}

	cmplx(double theta) {
		real = cos(theta);
		imag = sin(theta);
	}

	cmplx operator+(cmplx other) {
		return cmplx(real + other.real, imag + other.imag);
	}

	cmplx operator-(cmplx other) {
		return cmplx(real - other.real, imag - other.imag);
	}

	cmplx operator*(cmplx other) {
		return cmplx(real * other.real - imag * other.imag, real * other.imag + imag * other.real);
	}

	cmplx operator/(cmplx other) {
		double absv = other.real * other.real + other.imag * other.imag;
		other.real /= absv;
		other.imag /= -absv;
		return cmplx(real * other.real - imag * other.imag, real * other.imag + imag * other.real);
	}

	double operator%(cmplx other) {
		return ((real - other.real)*(real - other.real) + (imag - other.imag)*(imag - other.imag));
	}

	double abs_squared() {
		return real*real + imag*imag;
	}
};

/*COMPLEX CONSTANTS*/
cmplx one(1.0, 0.0), zero;
/*END OF COMPLEX CONSTANTS*/


/*PLOTTING PARAMETERS*/
struct plot_param {
	string filename = "test.bmp";
	int height = 1080;
	int width = 1920;
	double minr = -3.2;
	double maxr = 3.2;
	double mini = -1.8;
	double maxi = 1.8;
	int maxiter = 200;
	double tolerance = 0.000001;
	vector<cmplx> roots = { cmplx(pi / 4),cmplx(3 * pi / 4),cmplx(5 * pi / 4),cmplx(7 * pi / 4) };
	vector<cmplx> powers = { one,one,one,one };
};
/*END OF PLOTTING PARAMETERS*/

/*HSL TO RGB FUNCTION FOR COLOURING*/
int hsl_to_rgb(double h, double sl, double l)
{
	double v;
	double r, g, b;
	char r1, g1, b1;
	r = l;
	g = l;
	b = l;
	v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);
	if (v > 0)
	{
		double m;
		double sv;
		int sextant;
		double fract, vsf, mid1, mid2;

		m = l + l - v;
		sv = (v - m) / v;
		h *= 6.0;
		sextant = (int)h;
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch (sextant)
		{
		case 0:
			r = v;
			g = mid1;
			b = m;
			break;
		case 1:
			r = mid2;
			g = v;
			b = m;
			break;
		case 2:
			r = m;
			g = v;
			b = mid1;
			break;
		case 3:
			r = m;
			g = mid2;
			b = v;
			break;
		case 4:
			r = mid1;
			g = m;
			b = v;
			break;
		case 5:
			r = v;
			g = m;
			b = mid2;
			break;
		}
	}
	r1 = (char)(r * 255);
	g1 = (char)(g * 255);
	b1 = (char)(b * 255);
	return 65536*r1 + 256*g1 + b1;
}

/*PLOTTING FUNCTION - EVERYTHING COMES TOGETHER HERE*/
void plot(plot_param params) {
	/*TEMPORARY VARIABLES*/
	cmplx z, delta;
	int numroots = params.roots.size();
	int x, y, i, j, root_found, pixel;
	char r, g, b, padding = 0;
	double lightness;
	/*END OF TEMPORARY VARIABLES*/

	/*BITMAP FILE HEADER*/
	int pad_width = params.width * 3;
	if (pad_width % 4) while (++pad_width % 4);
	int size = pad_width*params.height * 3 + 54;
	int pad_cells = pad_width - params.width * 3;

	char bitmap_header[54] = { 'B',  'M',  size % 256,  (size / 256) % 256,   (size / 65536) % 256,   (size / 16777216) % 256,   0,   0,   0,   0,  54,   0,   0,   0,  40,   0,
		0,   0,   params.width % 256,   params.width / 256,   0,   0,   params.height % 256,   params.height / 256,   0,   0,   1,   0,  24,   0,   0,   0,
		0,   0,   (size - 54) % 256,  ((size - 54) / 256) % 256,   ((size - 54) / 65536) % 256,   ((size - 54) / 16777216) % 256,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0
	};

	ofstream bitmap(params.filename, ios::binary);
	bitmap.write(bitmap_header, 54);
	/*END OF BITMAP FILE HEADER*/

	/*CALCULATION FOR EACH PIXEL*/
	for (y = 0; y < params.height; y++) {
		for (x = 0; x < params.width; x++) {
			z = cmplx(((double)x / params.width)*(params.maxr - params.minr) + params.minr, ((double)y / params.height)*(params.maxi - params.mini) + params.mini);
			root_found = numroots;

			for (i = 0; i < params.maxiter; i++) {
				delta = zero;
				for (j = 0; j < numroots; j++) {
					delta = delta + params.powers[j] / (z - params.roots[j]);
				}
				delta = one / delta;
				if (delta.abs_squared() < params.tolerance) {
					for (j = 0; j < numroots; j++) {
						if (z%params.roots[j] < params.tolerance) {
							root_found = j;
							break;
						}
					}
				}
				if (root_found != numroots) {
					break;
				}
				z = z - delta;
			}

			/*ASSIGN COLOR TO PIXEL BASED ON root_found*/
			if (root_found != numroots) lightness = 0.5 - pow(((double)i+(delta.abs_squared()/params.tolerance))/(params.maxiter),0.25)/2;
			else lightness = 0;
			pixel = hsl_to_rgb((double)root_found / numroots, 0.75, lightness);
			r = (pixel / 65536) % 256;
			g = (pixel / 256) % 256;
			b = pixel % 256;
			bitmap.write(&b, 1);
			bitmap.write(&g, 1);
			bitmap.write(&r, 1);
		}
		/*WRITE PADDING CELLS AFTER EACH ROW*/
		for (i = 0; i < pad_cells; i++) bitmap.write(&padding, 1);
		//VERBOSE - if (!(y % 50)) cout << 100 * (double)y / (params.height) << "% done..." << endl;
	}
	bitmap.close();
	/*END OF CALCULATION*/
}
/*END OF PLOTTING FUNCTION*/