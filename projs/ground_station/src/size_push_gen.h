#ifndef SIZE_PUSH_GEN_H_
#define SIZE_PUSH_GEN_H_

#include <stream/data_push_stream.h>
#include <boost/shared_ptr.hpp>
#include <QtGui/QtGui>

namespace gs {


class SizePushGen : public QWidget {
	Q_OBJECT
public:
	typedef stream::DataPushStream<float> size_stream;

	enum Style {
		VERTICAL_DIAGRAM,
		HORIZONTAL_DIAGRAM,
	};

	SizePushGen(boost::shared_ptr<size_stream> size_stream,
				   std::string stream_name,
	   	   	   	   float max_size,
	   	   	   	   float min_size,
	   	   	   	   float start_from,
	   	   	   	   Style style = VERTICAL_DIAGRAM);

	float get_value();
	void set_value(float value);

public slots:

	void value_changed(int value);
	void input_value_changed();

private:

	boost::shared_ptr<size_stream> m_size_stream;

	float m_coeffecient;
	float m_min;
	QSlider* m_slider;
	QDoubleSpinBox* m_input;
};

} // namespace gs


#endif /* SIZE_PUSH_GEN_H_ */
