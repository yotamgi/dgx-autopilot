#include "direction_presenter.h"

int main () {

	DirectionPresenter a;
	angle3d_t angle;

	angle.ax = 0.f;
	angle.ay = 3.0f;
	angle.az = 20.0f;

	a.run();
	while (true) {
//	a.set_angle(angle);
	}
	return 0;
}
