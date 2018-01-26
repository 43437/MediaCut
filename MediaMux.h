#ifndef __MEDIA_MUX__
#define __MEDIA_MUX__

#include <queue>
#include <thread>
#include <cassert>

extern "C"{
#include "libavformat/avformat.h"
}

class MediaMux{
  
  AVFormatContext *outFmtCtx;
  std::queue<AVPacket*> *vQueue;
  std::queue<AVPacket*> *aQueue;
  int64_t vpts=0, apts=0;
  int inVideoIndex, inAudioIndex;
  bool isStop = false;
  
  int openOutput(const char* fileUrl, AVFormatContext *inFmtCtx, int inVideoIndex,int inAudioIndex);
  void process();
public:
  MediaMux(const char* fileUrl, AVFormatContext *inFmtCtx, int inVideoIndex, int inAudioIndex);
  void setQueue(std::queue<AVPacket*> *vQueue, std::queue<AVPacket*> *aQueue);
  void start();
  void stop();
};

void MediaMux::setQueue(std::queue<AVPacket*> *vQueue, std::queue<AVPacket*> *aQueue){
  this->vQueue=vQueue;
  this->aQueue=aQueue;
}

MediaMux::MediaMux(const char* fileUrl, AVFormatContext* inFmtCtx, int inVideoIndex, int inAudioIndex)
{
  openOutput(fileUrl, inFmtCtx, inVideoIndex, inAudioIndex);
}


int MediaMux::openOutput(const char* fileUrl, AVFormatContext *inFmtCtx, int inVideoIndex,int inAudioIndex){
  
  int ret=0;
  this->inVideoIndex = inVideoIndex;
  this->inAudioIndex = inAudioIndex;
  
  ret = avformat_alloc_output_context2(&outFmtCtx, nullptr, nullptr, fileUrl);
  assert(ret>=0);
  
  ret=avio_open2(&outFmtCtx->pb, fileUrl, AVIO_FLAG_WRITE, nullptr, nullptr);
  assert(ret>=0);
  
  AVStream *outVideoStream = avformat_new_stream(outFmtCtx, inFmtCtx->streams[inVideoIndex]->codec->codec);
  AVStream *outAudioStream = avformat_new_stream(outFmtCtx, inFmtCtx->streams[inAudioIndex]->codec->codec);
  
  ret=avcodec_copy_context(outVideoStream->codec, inFmtCtx->streams[inVideoIndex]->codec);
  assert(ret==0);
  ret=avcodec_copy_context(outAudioStream->codec, inFmtCtx->streams[inAudioIndex]->codec);
  assert(ret==0);
  
}

void MediaMux::process()
{
  avformat_write_header(outFmtCtx, nullptr);
  
  while(!isStop){
    
    if (av_compare_ts(vpts, outFmtCtx->streams[inVideoIndex]->time_base, apts, outFmtCtx->streams[inAudioIndex]->time_base)<=0){
      //video write
      if(vQueue->empty()){
	//wait and notify
      }
      AVPacket *packet = vQueue->front();
      vpts=packet->pts;
      av_interleaved_write_frame(outFmtCtx, packet);
      vQueue->pop();
      av_packet_free(&packet);
      
    } else {
      //audio write
      if (aQueue->empty()){
	//wait and notify
      }
      AVPacket *packet = aQueue->front();
      apts=packet->pts;
      av_interleaved_write_frame(outFmtCtx, packet);
      aQueue->pop();
      av_packet_free(&packet);
    }
  }
  
  av_write_trailer(outFmtCtx);
}

void MediaMux::start()
{
  std::thread t(&MediaMux::process, this);
  t.join();
}

void MediaMux::stop()
{
  isStop = true;
}

#endif __MEDIA_MUX__