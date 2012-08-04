#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
}

#include <iostream>
#include <fstream>

int main(int, char**)
{

    //
    // Init FFMpeg Capturing
    //

    /* must be called before using avcodec lib */
    avcodec_init();

    /* register all the codecs */
    avcodec_register_all();
    av_register_all();
    avdevice_register_all();
    AVInputFormat *p = NULL;

    AVFormatContext *cap_format_ctx = NULL;
    AVDictionary* cap_format_params = NULL;
    AVInputFormat *cap_format = NULL;
    //av_dict_set(&cap_format_params, "pix_format", "yuv420p", 0); // <-- I wish...
    av_dict_set(&cap_format_params, "video_size", "352x288", 0);
    cap_format = av_find_input_format("video4linux2");
    if (cap_format == NULL) {
        printf("No format found\n");
        return -2;
    }

    int ans= avformat_open_input(&cap_format_ctx, "/dev/video0", cap_format, &cap_format_params);
    if (ans != 0) {
        printf("Error");
        return -2;
    }
    int i;
    if(av_find_stream_info(cap_format_ctx)<0)
        return -1; // Couldn't find stream information
    av_dump_format(cap_format_ctx, 0, "/dev/video0", 0);

    //
    // Init FFMpeg Encoding
    //

    /* find the mpeg1 video encoder */
    AVCodec *en_codec = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
    if (!en_codec) {
        fprintf(stderr, "encoding codec not found\n");
        exit(1);
    }

    AVCodecContext *en_ctx = avcodec_alloc_context();
    AVFrame *en_picture = avcodec_alloc_frame();

    /* put sample parameters */
    en_ctx->bit_rate = 400000;
    /* resolution must be a multiple of two */
    en_ctx->width = 352;
    en_ctx->height = 288;
    /* frames per second */
    en_ctx->time_base= (AVRational){1,25};
    en_ctx->gop_size = 10; /* emit one intra frame every ten frames */
    en_ctx->max_b_frames=1;
    en_ctx->pix_fmt = PIX_FMT_YUV420P;

    /* open it */
    if (avcodec_open2(en_ctx, en_codec, NULL) < 0) {
        fprintf(stderr, "could not open encoding codec\n");
        exit(1);
    }

    /* alloc image and output buffer */
    const size_t outbuf_size = 100000;
    uint8_t* outbuf = (uint8_t*)malloc(outbuf_size);
    const size_t size = en_ctx->width * en_ctx->height;
    uint8_t* picture_buf = (uint8_t*)malloc((size * 3) / 2); /* size for YUV 420 */

    en_picture->data[0] = picture_buf;
    en_picture->data[1] = en_picture->data[0] + size;
    en_picture->data[2] = en_picture->data[1] + size / 4;
    en_picture->linesize[0] = en_ctx->width;
    en_picture->linesize[1] = en_ctx->width / 2;
    en_picture->linesize[2] = en_ctx->width / 2;


    // create the scale ctx
    SwsContext* sws_ctx = sws_getContext(
    		/* FROM: */ 352, 			288, 				PIX_FMT_YUYV422,
    		/* TO:   */ en_ctx->width, 	en_ctx->height, 	PIX_FMT_YUV420P,
    		SWS_BICUBIC, NULL, NULL, NULL);

//    //
//    // Init FFMpeg Decoding
//    //
//
//    AVPacket avpkt;
//    av_init_packet(&avpkt);
//
//    /* find the mpeg1 video decoder */
//    AVCodec *de_codec = avcodec_find_decoder(CODEC_ID_MPEG1VIDEO);
//    if (!de_codec) {
//        fprintf(stderr, "codec not found\n");
//        exit(1);
//    }
//
//    AVCodecContext *de_c= avcodec_alloc_context();
//    AVFrame *de_picture= avcodec_alloc_frame();
//
//    if(de_codec->capabilities&CODEC_CAP_TRUNCATED)
//        de_c->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
//
//    /* For some codecs, such as msmpeg4 and mpeg4, width and height
//       MUST be initialized there because this information is not
//       available in the bitstream. */
//
//    /* open it */
//    if (avcodec_open(de_c, de_codec) < 0) {
//        fprintf(stderr, "could not open codec\n");
//        exit(1);
//    }


    //
    // The capture loop
    //

    FILE* f = fopen("out.mpg", "wb");
    if (!f) {
        fprintf(stderr, "could not open file");
        exit(1);
    }

    for(size_t i=0;; i++)
    {
    	// capture
		AVPacket pckt;
    	av_read_frame(cap_format_ctx, &pckt);

    	// convert the yuyv422 camera pix format to the yuv420p format for the mpeg encoder
    	int srcstride = 352 * 2;
    	sws_scale(sws_ctx, &pckt.data, &srcstride, 0, 288, en_picture->data, en_picture->linesize);

    	// encode the picture
        size_t out_size = avcodec_encode_video(en_ctx, outbuf, outbuf_size, en_picture);
        std::cout << "Encoded to size " << out_size << std::endl;

        // save it
		fwrite(outbuf, 1, out_size, f);

//        // decode
//        if (out_size != 0) {
//			int finished_frame;
//			avpkt.data = outbuf;
//			avpkt.size = out_size;
//			out_size = avcodec_decode_video2(de_c, de_picture, &finished_frame, &avpkt);
//			std::cout << "Here and there" << std::endl;
//	        for(size_t y=0;y<en_c->height;y++) {
//	            for(size_t x=0;x<en_c->width;x++) {
//	            	frame_ycrcb.data[(x + y *en_c->height)*3 + 0] = de_picture->data[0];
//	            	frame_ycrcb.data[(x + y *en_c->height)*3 + 1] = de_picture->data[1];
//	            	frame_ycrcb.data[(x + y *en_c->height)*3 + 2] = de_picture->data[2];
//	            }
//	        }
//			frame_ycrcb.data = de_picture->data[0];
//			cvtColor(frame_ycrcb, frame, CV_YCrCb2RGB);
//        }

        // present
//        if (out_size != 0) {
//			imshow("edges", frame);
//			if(waitKey(30) >= 0) break;
//        }

        av_free_packet(&pckt);
    }

    return 0;
}
