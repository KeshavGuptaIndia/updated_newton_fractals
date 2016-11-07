/*Made by Keshav Gupta!
www.keshlabs.in
info@keshlabs.in

See the function plot_frame_range to see instruction on changing plotting parameters*/

#include <iostream>
#include <thread>
#include <sstream>
#include <iomanip>
#include "back_end.h"

int frames_done = 0;

string zero_pad(int a) {
	stringstream ret;
	ret << setw(6) << setfill('0') << a;
	return ret.str();
}

void done() {
	cout << ++frames_done << " frames done...\n";
}

int plot_frame_range(int frame_begin, int frame_end, int frames) {
	plot_param param;
	double t;
//Set following define to LOW_QUALITY to get fast renders (to preview), or HIGH_QUALITY to get full HD render.
//Of course, edit these parameters if you want!
#define HIGH_QUALITY
#ifdef LOW_QUALITY
	//LOW QUALITY RENDER
	param.height = 100;
	param.width = 100;
	param.minr = -2;
	param.maxr = 2;
	param.mini = -2;
	param.maxi = 2;
#endif
#ifdef HIGH_QUALITY
	//HIGH QUALITY RENDER
	param.height = 1080;
	param.width = 1920;
	param.minr = -3.2;
	param.maxr = 3.2;
	param.mini = -1.8;
	param.maxi = 1.8;
#endif

	param.powers = { one,one,one,one };
	for (int frame = frame_begin; frame < frame_end; frame++) {
		//t goes from 0 to 1 as the frames progress. use t as a parameter below to configure plotting properties.
		t = (double)frame / frames;
		param.roots = { cmplx(pi*(1 - t / 3)),cmplx(pi*(1 + t / 3)) ,cmplx(pi*(2 * t / 3)) ,cmplx(pi*(-2 * t / 3)) };
		param.filename = "animation6\\frame" + zero_pad(frame) + ".bmp";
		plot(param);
		done();
	}
}

int main() {
	int frames = 1000;

	vector<thread> threads;
	for (int i = 0; i < 2; i++) {
		threads.push_back(thread(plot_frame_range, i*(frames / 2), (i + 1)*(frames / 2), frames));
	}

	for (auto it = threads.begin(); it != threads.end(); it++) {
		it->join();
	}
	return 0;
}
