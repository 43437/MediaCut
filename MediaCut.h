#ifndef __MEDIA_CUT__
#define __MEDIA_CUT__
#include <iostream>
#include <queue>
#include <string>
#include <cassert>
#include "MediaIO.h"
#include "MediaMux.h"

extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
}
class MediaCut{
  
  const char* fileUrl = "test.mp4";
  std::queue<AVPacket*> vQuenu;
  std::queue<AVPacket*> aQuenu;
  void end();
  MediaIO *me;
  MediaMux *mux;
public:
  MediaCut(){};
  MediaCut(const char* fileUrl);
  int cut(int64_t beg, int64_t end);
};

MediaCut::MediaCut(const char* fileUrl){
  av_register_all();
}

void MediaCut::end()
{
  std::cout<<"cut end."<<std::endl;
  
}

int MediaCut::cut(int64_t begin, int64_t duration)
{
  me = new MediaIO(fileUrl, &vQuenu, &aQuenu);
  
  AVFormatContext *inFmtCtx= me->getInFmtCtx();
  int streams[2];
  me->getStreams(streams);
  me->setMediaCut(this);
  
  mux = new MediaMux("out.mp4", inFmtCtx, streams[0], streams[1]);
  mux->setQueue(&vQuenu, &aQuenu);
} 
#endif