#ifndef SNDHEAD_H
#define SNDHEAD_H

struct SNDHEAD
{
  i8  byte0[4];           //"RIFF"
  i32 Size;               //  6076  // #bytes after this integer
  i8  byte8[8];           //"WAVEfmt "
  i32 int16;              //    18
  i16 wFormatTag;         //     1
  i16 nChannels;          //     1
  i32 nSamplesPerSecond;  // 11025
  i32 nAvgBytesPerSec;    // 11025 // important
  i16 nBlockAlign;        //     1 // 2 for 16-bit
  i16 wBitsPerSample;     //     8
  i16 cbSize;             //    40
  i8  byte38[4];          // "fact"
  i32 int42;              //     4
  i32 numBytes46;         //
  i8  byte50[4];          // "data"
  i32 numSamples54;       //
  i8  sample58[1];
}
#ifdef   __GNUC__
	__attribute__((packed))
#endif //__GNUC__
;

#endif /* SNDHEAD_H */
