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

#include <iostream>

int main(int, char**)
{
    //
    // Init FFMpeg Decoding
    //

    /* must be called before using avcodec lib */
    avcodec_init();

    /* register all the codecs */
    avcodec_register_all();
    av_register_all();

    AVPacket avpkt;
    av_init_packet(&avpkt);

    /* find the mpeg1 video decoder */
    AVCodec *de_codec = avcodec_find_decoder(CODEC_ID_MPEG1VIDEO);
    if (!de_codec) {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }

    AVCodecContext *de_c= avcodec_alloc_context();
    AVFrame *de_picture= avcodec_alloc_frame();

    if(de_codec->capabilities&CODEC_CAP_TRUNCATED)
        de_c->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open(de_c, de_codec) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }


    //
    // The decoding loop
    //

    FILE* f = fopen("out.mpg", "wb");
    if (!f) {
        fprintf(stderr, "could not open file");
        exit(1);
    }

    for(size_t i=0;; i++)
    {

        // decode
		int finished_frame;
		avpkt.data = outbuf;
		avpkt.size = out_size;
		out_size = avcodec_decode_video2(de_c, de_picture, &finished_frame, &avpkt);
		std::cout << "Here and there" << std::endl;
		for(size_t y=0;y<en_c->height;y++) {
			for(size_t x=0;x<en_c->width;x++) {
				frame_ycrcb.data[(x + y *en_c->height)*3 + 0] = de_picture->data[0];
				frame_ycrcb.data[(x + y *en_c->height)*3 + 1] = de_picture->data[1];
				frame_ycrcb.data[(x + y *en_c->height)*3 + 2] = de_picture->data[2];
			}
		}
		frame_ycrcb.data = de_picture->data[0];
		cvtColor(frame_ycrcb, frame, CV_YCrCb2RGB);

        // present
//        if (out_size != 0) {
//			imshow("edges", frame);
//			if(waitKey(30) >= 0) break;
//        }

        av_free_packet(&pckt);
    }

    return 0;
}
