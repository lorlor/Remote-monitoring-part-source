#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>

#define DECODED_OUTPUT_FORMAT  AV_PIX_FMT_YUV420P
#define INPUT_FILE_NAME "encode.h264"
#define OUTPUT_FILE_NAME "decode.yuv"
#define IMAGE_WIDTH  320
#define IMAGE_HEIGHT 240

void
error_handle(const char *errorInfo ){
	printf("%s error!\n",errorInfo);
	exit(EXIT_FAILURE);
}


int
main(int argc,char ** argv){
	int  write_fd,ret,videoStream;
	AVFormatContext * formatContext=NULL;
	AVCodec * codec;
	AVCodecContext * codecContext;
	AVFrame * decodedFrame;
	AVPacket packet;
	uint8_t *decodedBuffer;
	unsigned int decodedBufferSize;
	int finishedFrame;


	av_register_all();


	write_fd=open(OUTPUT_FILE_NAME,O_RDWR | O_CREAT,0666);
	if(write_fd<0){
		perror("open");
		exit(1);
	}

	ret=avformat_open_input(&formatContext, INPUT_FILE_NAME, NULL,NULL);
	if(ret<0)
		error_handle("avformat_open_input error");

	ret=avformat_find_stream_info(formatContext,NULL);
	if(ret<0)
		error_handle("av_find_stream_info");


	videoStream=0;
	codecContext=formatContext->streams[videoStream]->codec;

	codec=avcodec_find_decoder(AV_CODEC_ID_H264);
	if(codec==NULL)
		error_handle("avcodec_find_decoder error!\n");

	ret=avcodec_open2(codecContext,codec,NULL);
	if(ret<0)
		error_handle("avcodec_open2");

	decodedFrame=avcodec_alloc_frame();
	if(!decodedFrame)
		error_handle("avcodec_alloc_frame!");

	decodedBufferSize=avpicture_get_size(DECODED_OUTPUT_FORMAT,IMAGE_WIDTH,IMAGE_HEIGHT);
	decodedBuffer=(uint8_t *)malloc(decodedBufferSize);
	if(!decodedBuffer)
		error_handle("malloc decodedBuffer error!");

	av_init_packet(&packet);
	while(av_read_frame(formatContext,&packet)>=0){
			ret=avcodec_decode_video2(codecContext,decodedFrame,&finishedFrame,&packet);
			if(ret<0)
				error_handle("avcodec_decode_video2 error!");
			if(finishedFrame){
				avpicture_layout((AVPicture*)decodedFrame,DECODED_OUTPUT_FORMAT,IMAGE_WIDTH,IMAGE_HEIGHT,decodedBuffer,decodedBufferSize);
				ret=write(write_fd,decodedBuffer,decodedBufferSize);
				if(ret<0)
					error_handle("write yuv stream error!");
			}

		av_free_packet(&packet);
	}

	while(1){
		packet.data=NULL;
		packet.size=0;
		ret=avcodec_decode_video2(codecContext,decodedFrame,&finishedFrame,&packet);
		if(ret<=0 && (finishedFrame<=0))
			break;
		if(finishedFrame){
			avpicture_layout((AVPicture*)decodedFrame,DECODED_OUTPUT_FORMAT,IMAGE_WIDTH,IMAGE_HEIGHT,decodedBuffer,decodedBufferSize);
			ret=write(write_fd,decodedBuffer,decodedBufferSize);
			if(ret<0)
				error_handle("write yuv stream error!");
		}

		av_free_packet(&packet);
	}


	avformat_close_input(&formatContext);
	free(decodedBuffer);
	av_free(decodedFrame);
	avcodec_close(codecContext);

	return 0;
}

