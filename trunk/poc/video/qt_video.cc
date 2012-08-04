#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#include <libswscale/swscale.h>
}

#include <QtGui>
#include <boost/thread/mutex.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>

class VideoWidget: public QWidget {
public:
	VideoWidget() {
		//
		// init decoder stuff
		//

		/* must be called before using avcodec lib */
		avcodec_init();

		/* register all the codecs */
		avcodec_register_all();
		av_register_all();

		av_init_packet(&m_avpkt);

		/* find the mpeg1 video decoder */
		AVCodec *de_codec = avcodec_find_decoder(CODEC_ID_MPEG1VIDEO);
		if (!de_codec) {
			fprintf(stderr, "codec not found\n");
			exit(1);
		}

		m_de_ctx = avcodec_alloc_context();
		m_picture = avcodec_alloc_frame();

		if (de_codec->capabilities & CODEC_CAP_TRUNCATED)
			m_de_ctx->flags |= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

		/* For some codecs, such as msmpeg4 and mpeg4, width and height
		 MUST be initialized there because this information is not
		 available in the bitstream. */

		/* open it */
		if (avcodec_open(m_de_ctx, de_codec) < 0) {
			fprintf(stderr, "could not open codec\n");
			exit(1);
		}

		// start the update timer
		startTimer(0.033);
	}

	/**
	 * get a new chunk of video data
	 */
	void set_data(std::string chunk) {
		while (!m_lock.try_lock()) {
			sleep(100);
		}
		m_new_chunk = chunk;
		m_lock.unlock();
	}

	void timerEvent() {
		std::cout << "Got timer event" << std::endl;

		// update the new chunk
		while (!m_lock.try_lock()) {
			sleep(100);
		}
		if (m_curr_chunk != m_new_chunk) {
			m_curr_chunk = m_new_chunk;
		}
		m_lock.unlock();

		// call the paint event
		this->repaint();
	}

	void paintEvent(QPaintEvent * e) {

		//
		// decode the video
		//
		std::string decoded_frame;

		memcpy(m_avpkt.data, m_curr_chunk.data(), m_curr_chunk.size());
		while (m_avpkt.size > 0) {
			int got_picture;
			int len = avcodec_decode_video2(m_de_ctx, m_picture, &got_picture,
					&m_avpkt);
			if (len < 0) {
				fprintf(stderr, "Error while decoding frame\n");
				exit(1);
			}
			if (got_picture) {
				std::cout << "Decoded frame" << std::endl;

				// create the picture as a PPM format
				std::string header = (boost::format("P5\n%d %d\n%d\n") % m_de_ctx->width % m_de_ctx->height % 255).str();
				decoded_frame = header + std::string(reinterpret_cast<const char*>(m_picture->data[0]), m_de_ctx->width*m_de_ctx->height);
			}
			m_avpkt.size -= len;
			m_avpkt.data += len;
		}

		//
		// paint the video
		//

		QPainter painter(this);
		if (decoded_frame != "") {
			QImage i = QImage::fromData(reinterpret_cast<const uchar*>(decoded_frame.data()), decoded_frame.size(), "PPM");
			painter.drawImage(QPoint(0, 0), i);
		}
	}

private:
	std::string m_curr_chunk;
	std::string m_new_chunk;

	boost::mutex m_lock;

	// decoding stuff
	AVPacket m_avpkt;
	AVFrame *m_picture;
	AVCodecContext *m_de_ctx;
};

void video_player(std::string filename, VideoWidget* video) {
	const size_t INBUF_SIZE = 1000;

	FILE* f = fopen(filename.c_str(), "rb");
	if (!f) {
		fprintf(stderr, "could not open\n");
		exit(1);
	}

	while (true) {
		std::stringstream ss;
		char* buf = new char[INBUF_SIZE];
        fread(buf, 1, INBUF_SIZE, f);
        ss << buf;
        video->set_data(buf);
        usleep(100000);
	}
}

int main(int argc, char** argv) {
	// Create the QApplication
	QApplication application(argc, argv);

	// Create the main window
	QMainWindow *mainWindow;
	mainWindow = new QMainWindow();

	// Create the central widget to be put into the main window
	VideoWidget *centralWidget;
	centralWidget = new VideoWidget();
	mainWindow->setCentralWidget(centralWidget);

	// Resize and then show the main window
	mainWindow->resize(800, 600);
	mainWindow->show();

	boost::thread worker(video_player, "out.mpg", centralWidget);

	application.exec();

	return 0;
}
