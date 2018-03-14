#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

static void save_gray_frame(unsigned char *buf, int wrap, int xsize, int ysize, char *filename);
static int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame);


int main(int argc, const char *argv[])
{
printf("initializing all containers, codecs and protocols\n");

av_register_all(); //allow for all codects to be used

AVFormatContext *pFormatContext = avformat_alloc_context();
if (!pFormatContext){
	printf("Error could not allocate memory for Format Context\n");
	return -1;
}

printf("Opening the video file", argv[1],"\n");


if (avformat_open_input(&pFormatContext,argv[1],NULL, NULL) !=0){
	printf("Error, could not open file");
	return -1;
}


printf("Format %s, duration %lld us", pFormatContext->iformat->long_name, pFormatContext->duration);

if (avformat_find_stream_info(pFormatContext, NULL)<0){
	printf("error, could not get stream info");
	return -1;
}

AVCodec *pCodec =NULL;
AVCodecParameters *pCodecParameters =NULL;
int video_stream_index = -1;

for (int i=0; i < pFormatContext->nb_streams;i++){
	AVCodecParameters *pLocalCodecParameters =  NULL;
	pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
	printf("looking for videostream \n");
	AVCodec *pLocalCodec = NULL;
 	pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

	if(pLocalCodec==NULL){
		printf("error, unsupported codec");
		return -1;
	}

	if (pLocalCodecParameters->codec_type== AVMEDIA_TYPE_VIDEO){
		video_stream_index =i;
		pCodec =pLocalCodec;
		pCodecParameters = pLocalCodecParameters;
		printf("Video Codec: resolution %d x %d", pLocalCodecParameters->width, pLocalCodecParameters->height);
	}


	printf("\tCodec %s ID %d bit_rate %lld", pLocalCodec->long_name, pLocalCodec->id, pCodecParameters->bit_rate);
}

AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
if(!pCodecContext)
{
	printf("failed to allocate memory");
	return -1;
}

if(avcodec_parameters_to_context(pCodecContext,pCodecParameters)<0){
	printf("failed to copy codec params to codec context");
	return -1;
}

if(avcodec_open2(pCodecContext, pCodec, NULL)>0){
	printf("failed to open codec");
	return -1;
}

AVFrame *pFrame =av_frame_alloc();
if(!pFrame){
	printf("failed to allocate mem for AVFrame");
	return -1;
}

AVPacket *pPacket = av_packet_alloc();
if(!pPacket){
	printf("failed to allocate memory for AVPacket");
	return -1;
}

int response =0;
int how_many_packets_to_process =8;

while (av_read_frame(pFormatContext,pPacket) >= 0){
	if(pPacket->stream_index==video_stream_index){
		printf("AvPacket->pts %" PRId64, pPacket->pts);
		response = decode_packet(pPacket,pCodecContext,pFrame);
		if(response<0)
			break;
		if(--how_many_packets_to_process <= 0) 
			break;
	}
	av_packet_unref(pPacket);
}

printf("releaseing all resources \n");

avformat_close_input(&pFormatContext);
avformat_free_contet(pFormatContext);
av_packet_free(&pPacket);
av_frame_free(&pFrame);
avcodec_free_context(&pCodecContext);
return 0;
}

static int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame)
{
	int response = avcodec_send_packet(pCodecContext, pPacket);

	if(response <0){
		printf("error while sending packet to decoded", av_err2str(response));
		return response;
	}

	while(response >= 0)
	{
		response = avcodec_receive_frame(pCodecContext,pFrame);
		if(response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
			break;
		} else if (response <0) {
			printf("Error while receiving a frame from the decoder: %s",av_err2str(response));
			return response;
		}

		if (response >= 0) {
		printf(
          "Frame %d (type=%c, size=%d bytes) pts %d key_frame %d [DTS %d]",
          pCodecContext->frame_number,
          av_get_picture_type_char(pFrame->pict_type),
          pFrame->pkt_size,
          pFrame->pts,
          pFrame->key_frame,
          pFrame->coded_picture_number
		);

		char frame_filename[1024];
		snprintf(frame_filename, sizeof(frame_filename), "%s-%d.pgm", "frame", pCodecContext->frame_number);
      // save a grayscale frame into a .pgm file
      		save_gray_frame(pFrame->data[0], pFrame->linesize[0], pFrame->width, pFrame->height, frame_filename);

		av_frame_unref(pFrame);
		}
	}
	return 0;
}



static void save_gray_frame(unsigned char *buf, int wrap, int xsize, int ysize, char *filename)
{
    FILE *f;
    int i;
    f = fopen(filename,"w");
    // writing the minimal required header for a pgm file format
    // portable graymap format -> https://en.wikipedia.org/wiki/Netpbm_format#PGM_example
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);

    // writing line by line
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}





	


