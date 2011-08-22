#include "direction_presenter.h"

int main () {

	DirectionPresenter a;
	angle3d_t angle;

	angle.ax = 0.f;
	angle.ay = 3.0f;
	angle.az = 20.0f;

	a.set_angle(angle);
	a.run();
	return 0;
}
