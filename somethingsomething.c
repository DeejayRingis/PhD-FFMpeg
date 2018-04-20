
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<libavcodec/avcodec.h>
#include<libavformat/avformat.h>


static char *sourceFilename =NULL;
static AVFormatContext *formatContext = NULL;
static AVCodec *videoCodec = NULL;
static AVCodecParameters *videoCodecParameters=NULL;
static AVCodecContext *videoCodecContext = NULL;
static AVPacket *inputPacket;
static AVFrame *inputVideoFrame;

int decode(AVCodecContext *avContext, AVFrame *frame, int *got_frame, AVPacket *pkt){
	int ret;

	*got_frame=0;

	if(pkt){
		ret=avcodec_send_packet(avContext,pkt);

		if (ret<0)
			return ret == AVERROR_EOF ? 0 : ret;

	}

	ret= avcodec_receive_frame(avContext,frame);
	if(ret<0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
		return ret;
	if (ret>=0)
		*got_frame =1;
	return 0;
}


int encode(AVCodecContext *avContext, AVPacket *pkt, int *got_packet, AVFrame *frame){
	int ret;

	*got_packet =0;

	ret=avcodec_send_frame(avContext,frame);
	if(ret<0)
		return ret;

	ret= avcodec_receive_packet(avContext,pkt);
	if(!ret)
		*got_packet =1;
	if (ret==AVERROR(EAGAIN))
		return 0;

	return ret;
}



int main (int argc, char **argv){
	int ret =0;

	sourceFilename =argv[1];

	av_register_all();
	//Open an input stream and read the header. returns 0 on success, negative on failure,
	//might need to check the 4th parameter with setting a dictionary
	if (avformat_open_input(&formatContext,sourceFilename,NULL,NULL)<0){
		printf("Couldnt open source file%s \n",sourceFilename);
	}

	//Reads the file for the stream info
	if (avformat_find_stream_info(formatContext, NULL)<0){
		printf("Couldnt find stream info \n ");
	}

	int videoStreamIndex = -1;

	for (int i =0; i < formatContext->nb_streams;i++){
		AVCodecParameters *tempCodecParameters = NULL;
		tempCodecParameters = formatContext->streams[i]->codecpar;
		printf("searching for the video stream \n");
		AVCodec *tempCodec = NULL;
		tempCodec =avcodec_find_decoder(tempCodecParameters->codec_id);

		if(tempCodec==NULL){
			printf("unsupported codec");
			return -1;
		}

		if(tempCodecParameters->codec_type==AVMEDIA_TYPE_VIDEO){
			videoStreamIndex=i;
			videoCodec=tempCodec;
			videoCodecParameters=tempCodecParameters;
		}


	}

	av_dump_format(formatContext,0,sourceFilename,0);


	videoCodecContext= avcodec_alloc_context3(videoCodec);

	if(avcodec_open2(videoCodecContext,videoCodec,NULL)<0){
		printf("unable to open codec context\n");
	}

	inputPacket=av_packet_alloc();
	if(!inputPacket)
		printf("failed to init input packet \n");

	inputVideoFrame =av_frame_alloc();
	if(!inputVideoFrame){
		printf("Couldnt allocate frame \n");
	}




}
