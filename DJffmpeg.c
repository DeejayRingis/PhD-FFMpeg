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

printf("Opening the video file");


if (avformat_open_input(&pFormatContext,argv[1],NULL, NULL) !=0){
	printf("Error, could not open file");
	return -1;
}




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
int ret;

/*
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
*/
//new shizz
pFrameRGB=avcodec_alloc_frame();
if(pFrameRGB==NULL);
	return -1;

//numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	
//buffer=malloc(numBytes);
	
//avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

while (av_read_frame(pFormatContext,pPacket) >= 0){
	if(pPacket->stream_index==video_stream_index){
		//avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet.data, packet.size);
		ret=avcodec_receive_frame(pCodcCtx,pFrame);
		display_frame(pFrame,		

			
		
	
	
printf("releaseing all resources \n");

avformat_close_input(&pFormatContext);
avformat_free_context(pFormatContext);
av_packet_free(&pPacket);
av_frame_free(&pFrame);
avcodec_free_context(&pCodecContext);
return 0;
}
