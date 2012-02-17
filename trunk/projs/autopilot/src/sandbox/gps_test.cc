#include <iostream>
#include <string>
#include <platform/hw/gps.h>


template <typename T>
class PushPrinter : public stream::DataPushStream<T> {
public:
	PushPrinter(std::string name): m_name(name) {}

	void set_data(const T& t){
		std::cout << m_name << ": " << t << std::endl;
	}

private:
	std::string m_name;
};

int main() {
	boost::shared_ptr<PushPrinter<lin_algebra::vec3f> > pos(new PushPrinter<lin_algebra::vec3f>("Position"));
	boost::shared_ptr<PushPrinter<float> > dir(new PushPrinter<float>("Direction"));
	boost::shared_ptr<PushPrinter<float> > speed(new PushPrinter<float>("Speed"));

	autopilot::Gps gps;
	gps.set_pos_reciever_stream(pos);
	gps.set_speed_mag_reciever_stream(speed);
	gps.set_speed_dir_reciever_stream(dir);

	while (true) {}
	return 0;
}
