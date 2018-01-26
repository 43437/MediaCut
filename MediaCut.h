#ifndef __MEDIA_CUT__
#define __MEDIA_CUT__

#include <iostream>
#include <queue>
#include <cassert>

extern "C"{
#include "libavformat/avformat.h"
}

class MediaCut{
  
  int64_t vpts=0, apts=0;
  int openInput();
  int openOutput();
  std::queue<AVPacket*> vQueue;
  std::queue<AVPacket*> aQueue;
  AVFormatContext *inFmtCtx;
  AVFormatContext* outFmtCtx;
  int inVideoIndex, inAudioIndex;
public:
  MediaCut();
  void cut(int64_t begin, int64_t end);
};
#endif
