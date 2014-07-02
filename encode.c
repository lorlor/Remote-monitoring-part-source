#include <stdint.h>
#include <x264.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define CLEAR(x) (memset(&x),sizeof(0))

#define WIDTH 640
#define HEIGHT 480

int main(int argc,char **argv)
{
	int fps = 25;
	int yuv_size = WIDTH * HEIGHT * 3 / 2;
	
	x264_param_t *param;
	param = (x264_param_t *)malloc(sizeof(x264_param_t));
	x264_t *encoder;
	x264_picture_t pic_in,pic_out;

	int inf,outf;

	unsigned char *yuv_buffer;
	assert(param);

	x264_param_default_preset(param,"veryfast","zerolatency");

	param->i_threads = X264_SYNC_LOOKAHEAD_AUTO;
	param->i_width = WIDTH;
	param->i_height = HEIGHT;
	param->i_keyint_max = 25;
	param->b_intra_refresh = 1;
	param->i_frame_total = 0;

	param->i_bframe = 5;
	param->b_open_gop = 0;
	param->i_bframe_pyramid = 0;
	param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;

	param->i_log_level = X264_LOG_DEBUG;

	param->rc.i_bitrate = 1024*10;
	param->i_fps_den = 1;
	param->i_fps_num = 25;
	param->i_timebase_den = param->i_fps_num;
	param->i_timebase_num = param->i_fps_den;

	param->b_annexb = 1;

	x264_param_apply_profile(param,"baseline");

	encoder = x264_encoder_open(param);	

	x264_picture_alloc(&pic_in,X264_CSP_I420,WIDTH,HEIGHT);
	yuv_buffer = malloc(yuv_size);

	pic_in.img.plane[0] = yuv_buffer;
	pic_in.img.plane[1] = pic_in.img.plane[0] + WIDTH * HEIGHT;
	pic_in.img.plane[2] = pic_in.img.plane[1] + WIDTH * HEIGHT / 4;

	inf = open(argv[1],O_RDONLY);
	if(inf < 0) {
		return -1;			
	}

	outf = open(argv[2],O_CREAT | O_WRONLY,444);
	if(outf < 0) {
		return -1;	
	}

	long i_pts = 0;

	x264_nal_t *nals;
	int nnal;
	while(read(inf,yuv_buffer,yuv_size) > 0){
		pic_in.i_pts = i_pts++;
		x264_encoder_encode(encoder,&nals,&nnal,&pic_in,&pic_out);
		x264_nal_t *nal;
		for(nal = nals; nal < nals + nnal; nal++){
			write(outf,nal->p_payload,nal->i_payload);		
		}
	}

	x264_encoder_close(encoder);
	close(inf);
	close(outf);
	free(yuv_buffer);
	
	return 0;
}
