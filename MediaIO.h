#ifndef __MEDIA_IO__
#define __MEDIA_IO__

#include <string>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <queue>
#include <thread>
#include <cstring>
#include "MediaCut.h"

extern "C"{
  
#include "libavformat/avformat.h"
}
class MediaIO{
  
  AVFormatContext *inFmtCtx;
  char* outFile;
  std::queue<AVPacket*> *vQueue;
  std::queue<AVPacket*> *aQueue;
  int inVideoIndex, inAudioIndex;
  
  int openInput(const char* fileUrl);
  int openOutput();
  MediaCut *mediaCut;
public:
  void process();
  MediaIO(const char* fileUrl, std::queue<AVPacket*> *vQueue, std::queue<AVPacket*> *aQueue);
  void start();
  void getStreams(int* streams);
  AVFormatContext* getInFmtCtx();
  void setMediaCut(MediaCut *mediaCut);
  virtual ~MediaIO(){};
};

void MediaIO::setMediaCut(MediaCut* mediaCut)
{
  this->mediaCut = mediaCut;
};

MediaIO::MediaIO(const char* fileUrl, std::queue<AVPacket*> *vQueue, std::queue<AVPacket*> *aQueue)
{
  av_register_all();
  openInput(fileUrl);
  
  this->vQueue=vQueue;
  this->aQueue=aQueue;
}

int MediaIO::openInput(const char* fileUrl){
  
  int ret = 0;
  inFmtCtx = avformat_alloc_context();
  assert(inFmtCtx != nullptr);
  
  ret = avformat_open_input(&inFmtCtx, fileUrl, nullptr, nullptr);
  assert(ret==0);
  
  ret = avformat_find_stream_info(inFmtCtx, nullptr);
  assert(ret>=0);
  
  for (int i=0; i<inFmtCtx->nb_streams; i++){
    if (inFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
      inVideoIndex = i;
    } else {
      inAudioIndex = i;
    }
  }
}

void MediaIO::getStreams(int* streams)
{
  streams[0] = inVideoIndex;
  streams[1] = inAudioIndex;
}

AVFormatContext* MediaIO::getInFmtCtx()
{
  return this->inFmtCtx;
}

void MediaIO::process()
{
  int ret=0;
  
  while(true){
    AVPacket *packet;
    packet = av_packet_alloc();
    assert(packet!=nullptr);
    std::memset(packet, 0, sizeof(AVPacket));
    
    if (av_read_frame(inFmtCtx, packet)<0){
      std::cout<<"end of file or error."<<std::endl;
      if (this->mediaCut != nullptr){
	this->mediaCut.end();
      }
      break;
    }
    
    if (packet->stream_index == inVideoIndex){
      vQueue->push(packet);
    } else if(packet->stream_index == inAudioIndex){
      aQueue->push(packet);
    }
    
    if ((!vQueue->empty()) && (!aQueue->empty())){
      //wait and notify
    }
  }
}

void MediaIO::start()
{
  std::thread t(&MediaIO::process , this);
  t.join();
}

#endif