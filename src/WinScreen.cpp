
#include "stdafx.h"

#ifdef _MSVC_INTEL // Windows screen handling


#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "data.h"
#include <io.h>
#include <FCNTL.h>
#include <SYS\STAT.h>


extern i32 WindowX;
extern i32 WindowY;
extern i32 VBLMultiplier;
extern i16 globalPalette[16];
extern bool fullscreenRequested;
extern bool fullscreenActive;
extern bool overlayActive;
extern i32 xGraphicJitter;
extern i32 xOverlayJitter;
extern i32 yGraphicJitter;
extern bool jitterChanged;
extern ui32 dumpWindow;
//extern CDC *OnDrawDC;
ui32 STBLTCount=0;

i32 screenSize=2;
i16 palette16[16];
i16 bitmap[320*4*200*4];
i16 counter;
i16 bitSkip;
i32 dstLineLen;
BITMAPINFO bitmapInfo;
i16 palette1[16];
i16 palette2[16];
i16 oldPalette1[16];
i16 oldPalette2[16];
ui8 black[320]; //All zeros for one line of overlay
pnt logbase(void);
bool screenInconsistent = true;


bool IsTextScrollArea(i32, i32)
{
  return false;
}

void SwapTextZOrder(void)
{
  return;
}


bool HasPaletteChanged(i16 *palette, i16 *oldpalette)
{
  bool change=false;
  for (i32 i=0; i<16; i++)
  {
    if (oldpalette[i] != palette[i])
    {
      oldpalette[i]=palette[i];
      change=true;
    };
  };
  return change;
}


bool HasScreenChanged(i8 *STScreen,    // First byte to test
                      i32 lineLen,     // quadwords
                      i32 numLine,
                      i32 lineIncrement,
                      i32 *pOldChecksum)
{
  bool result;
  __asm
  {
    sub edx,edx   // The summation
    sub ebx,ebx
    mov esi, STScreen
    mov edi,numLine //
do1line:
    mov ecx,lineLen
    cmp ecx,4
do16Bytes:
    jc do4Bytes
    lodsd
    add edx,eax
    lodsd
    add ebx,eax
    lodsd
    add edx,eax
    lodsd
    add ebx,eax
    sub ecx,4
    cmp ecx,4
    jnc do16Bytes
    or  cx,cx
    jz  lineDone
do4Bytes:
    rol edx,1
    lodsd
    //add ebx,eax
    //ror ebx,1
    add edx,eax
    loop do4Bytes
lineDone:
    or  edx,edx //clear carry
    rcl edx,1
    jnc edxzero
    xor edx,0x8d
edxzero:
    or  ebx,ebx //clear carry
    rcl ebx,1
    jnc ebxzero
    xor ebx,0x8d
ebxzero:
    add esi,lineIncrement
    dec edi
    jnz do1line
    rol ebx,1
    add edx,ebx
    mov esi,pOldChecksum
    mov result,0
    cmp edx,[esi]
    jz  ret0
    mov result,1
ret0:
    mov [esi],edx
  };
  return result;
}


/*
void createPalette32(i16 *palette)
{ // Create 32-bit color palette from the ST 9-bit palette
  for (i32 i=0; i<16; i++)
  {
    ASSERT(palette[i]<=0x777);
    palette32[i]  = (((palette[i]>>8) & 7)*0xff/0x7)<<16;
    palette32[i] |= (((palette[i]>>4) & 7)*0xff/0x7)<< 8;
    palette32[i] |= (((palette[i]>>0) & 7)*0xff/0x7)<< 0;
  };
}
*/

void createPalette16(i16 *palette)
{ // Create 16-bit color palette from the ST 9-bit palette
  for (i32 i=0; i<16; i++)
  {
    ASSERT(palette[i]<=0x777,"palette");
    palette16[i]=0;
    palette16[i] |= (((palette[i]>>8) & 7)*0x1f/0x7)<<10;//red
    palette16[i] |= (((palette[i]>>4) & 7)*0x1f/0x7)<< 5;//green
    palette16[i] |= (((palette[i]>>0) & 7)*0x1f/0x7)<< 0;//blue
  };
}


bool ForcedScreenDraw = false;

void ForceScreenDraw(void)
{
  ForcedScreenDraw = true;
}

void Unpack(char *src, unsigned char *dst, i32 shift, i32 count)
{
  _asm
  {
    mov esi,src;
    mov ax,[esi+6]
    mov bx,[esi+4]
    mov cx,[esi+2]
    mov dx,[esi+0]
    rol ax,8
    rol bx,8
    rol cx,8
    rol dx,8
    rol ecx,16
    mov esi,ecx
    mov ecx,shift
    rol ax,cl
    rol bx,cl
    rol esi,cl
    rol dx,cl
    mov edi,dst
next:
    sub ecx,ecx
    rcl ax,1
    rcl ecx,1
    rcl bx,1
    rcl ecx,1
    rcl esi,1
    rcl ecx,1
    rcl dx,1
    rcl ecx,1
    mov [edi],cl
    inc edi
    dec count
    jnz next
  };
}

// Don't warning about modifing EBP
#if defined(_MSC_VER)
#pragma warning( disable : 4731)
#endif

void BLT1(unsigned char *src, 
          i16 *dst, 
          i32 num, 
          i16 *palette,
          ui8 *overlay)
{
  _asm
  {
    mov esi,src
    mov edi,dst
    mov ecx,num
    mov edx,overlay
    mov ebp,palette
    sub ebx,ebx
next:
    movzx bx,[edx] //overlay byte
    inc edx
    shl bx,4
    or  bl,[esi]   //Graphic nibble
    inc esi
    mov ax,[ebp+ebx*2]
    mov [edi],ax   //to screen bitmap
    add edi,2
    loop next
  };
}

//void BLT2(unsigned char *src, i16 *dst, i32 num)
void BLT2(unsigned char *src, 
          i16 *dst, 
          i32 num, 
          i16 *palette,
          ui8 *overlay)
{
  _asm
  {
    mov esi,src
    mov edi,dst
    mov ecx,num
    mov edx,overlay
    mov ebp,palette
    sub ebx,ebx
next:
    movzx bx,[edx] //overlay byte
    inc edx
    shl bx,4
    or  bl,[esi]   //Graphic nibble
    inc esi
    mov ax,[ebp+ebx*2]
    //shr ax,1
    //and ax,0x3def

    mov [edi],ax
    mov [edi+320*2*4],ax
    add edi,2
    mov [edi],ax
    mov [edi+320*2*4],ax
    add edi,2
    loop next
  };
}

//void BLT3(unsigned char *src, i16 *dst, i32 num)
void BLT3(unsigned char *src, 
          i16 *dst, 
          i32 num, 
          i16 *palette,
          ui8 *overlay)
{
  _asm
  {
    mov esi,src
    mov edi,dst
    mov ecx,num
    mov edx,overlay
    mov ebp,palette
    sub ebx,ebx
next:
    movzx bx,[edx] //overlay byte
    inc edx
    shl bx,4
    or  bl,[esi]   //Graphic nibble
    inc esi
    mov ax,[ebp+ebx*2]
    mov [edi],ax
    mov [edi+1*320*2*4],ax
    mov [edi+2*320*2*4],ax
    add edi,2
    mov [edi],ax
    mov [edi+1*320*2*4],ax
    mov [edi+2*320*2*4],ax
    add edi,2
    mov [edi],ax
    mov [edi+1*320*2*4],ax
    mov [edi+2*320*2*4],ax
    add edi,2
    loop next
  };
}

#ifdef BLUR


#define mv(x,y) mov [edi+2*x+y*320*2*4],ax
#define dv _asm shr ax,1 _asm and ax,0x3def
#define dvu _asm mov cx,ax _asm shr ax,1 _asm and ax,0x3def _asm and cx,0x421 _asm add ax,cx 
#define ad(x,y) add [edi+2*x+y*320*2*4],ax

//void BLT4(unsigned char *src, i16 *dst, i32 num)
void BLT4(unsigned char *src, 
          i16 *dst, 
          i32 num, 
          i16 *palette,
          ui8 *overlay)
{
  _asm
  {
    mov esi,src
    mov edi,dst
    mov ecx,num
    mov edx,overlay
    mov ebp,palette
    sub ebx,ebx
next:
    movzx bx,[edx] //overlay byte
    shl bx,4
    or  bl,[esi]   //Graphic nibble
    mov ax,[ebp+ebx*2]
    mv(0,0)
/*
    mov ax,0
    mv(0,1)
    mv(0,2)
    mv(0,3)
    mv(1,0)
    mv(1,1)
    mv(1,2)
    mv(1,3)
    mv(2,0)
    mv(2,1)
    mv(2,2)
    mv(2,3)
    mv(3,0)
    mv(3,1)
    mv(3,2)
    mv(3,3)
*/
    inc edx
    inc esi
    add edi,8
    loop next
  };
}



//void BLS4(i16 *bmp)
void BLS4(i16 *bmp)
{
  _asm
  {
    mov edi,bmp
    mov ax,[edi]
    cmp ax,[edi+8]
    jnz hard
    cmp ax,[edi+4*320*2*4]
    jnz hard
    cmp ax,[edi+4*320*2*4+8]
    jnz hard
    mv(0,1)
    mv(0,2)
    mv(0,3)
    mv(1,0)
    mv(1,1)
    mv(1,2)
    mv(1,3)
    mv(2,0)
    mv(2,1)
    mv(2,2)
    mv(2,3)
    mv(3,0)
    mv(3,1)
    mv(3,2)
    mv(3,3)
    jmp xit
hard:
    dvu            // 8/16 A
    mv(1,0)
    mv(2,0)
    mv(0,1)
    mv(1,1)
    mv(0,2)
    dvu            // 4/16 A
    ad(1,0)
    mv(3,0)
    ad(0,1)
    mv(2,1)
    mv(1,2)
    mv(2,2)
    mv(0,3)
    dvu            // 2/16 A
    ad(2,1)
    mv(3,1)
    ad(1,2)
    mv(3,2)
    mv(1,3)
    mv(2,3)
    dvu           // 1/16 A
    ad(1,1)
    ad(3,1)
    ad(1,3)
    mv(3,3)


    mov ax,[edi+8]
    dv            // 8/16 B
    ad(3,0)
    ad(2,0)
    ad(3,1)
    dv            // 4/16 B
    ad(3,0)
    ad(1,0)
    ad(2,1)
    ad(3,2)
    ad(2,2)
    dv            // 2/16 B
    ad(2,1)
    ad(1,1)
    ad(1,2)
    ad(3,2)
    ad(3,3)
    ad(2,3)
    dv            // 1/16 B
    ad(3,1)
    ad(1,1)
    ad(3,3)
    ad(1,3)


    mov ax,[edi+4*320*2*4]
    dv            // 8/16 C
    ad(0,3)
    ad(1,3)
    ad(0,2)
    dv            // 4/16 C
    ad(0,3)
    ad(2,3)
    ad(1,2)
    ad(2,2)
    ad(0,1)
    dv            // 2/16 C
    ad(2,3)
    ad(3,3)
    ad(1,2)
    ad(3,2)
    ad(1,1)
    ad(2,1)
    dv            // 1/16 C
    ad(1,3)
    ad(3,3)
    ad(1,1)
    ad(3,1)

    mov ax,[edi+4*320*2*4+8]
    dvu            // 8/16 D
    ad(3,3)
    dv            // 4/16 D
    ad(2,3)
    ad(3,2)
    ad(2,2)
    dvu            // 2/16 D
    ad(2,3)
    ad(1,3)
    ad(3,2)
    ad(1,2)
    ad(3,1)
    ad(2,1)
    dvu            // 1/16 D
    ad(3,3)
    ad(1,3)
    ad(3,1)
    ad(1,1)

xit:  
  };
}

#else

//void BLT4(unsigned char *src, i16 *dst, i32 num)

void BLT4(unsigned char *src, 
          i16 *dst, 
          i32 num, 
          i16 *palette,
          ui8 *overlay)
{
  _asm
  {
    mov esi,src
    mov edi,dst
    mov ecx,num
    mov edx,overlay
    mov ebp,palette
    sub ebx,ebx
next:
    movzx bx,[edx] //overlay byte
    inc edx
    shl bx,4
    or  bl,[esi]   //Graphic nibble
    inc esi
    mov ax,[ebp+ebx*2]
    mov [edi],ax
    mov [edi+1*320*2*4],ax
    mov [edi+2*320*2*4],ax
    mov [edi+3*320*2*4],ax
    add edi,2
    mov [edi],ax
    mov [edi+1*320*2*4],ax
    mov [edi+2*320*2*4],ax
    mov [edi+3*320*2*4],ax
    add edi,2
    mov [edi],ax
    mov [edi+1*320*2*4],ax
    mov [edi+2*320*2*4],ax
    mov [edi+3*320*2*4],ax
    add edi,2
    mov [edi],ax
    mov [edi+1*320*2*4],ax
    mov [edi+2*320*2*4],ax
    mov [edi+3*320*2*4],ax
    add edi,2
    loop next
  };
}
#endif


int updateScreenAreaEnterCount = 0;
int updateScreenAreaLeaveCount = 0;

void UpdateScreenArea(
                      i8  *STScreen,
                      i32  x0,
                      i32  y0,
                      i32  width,
                      i32  height,
                      i32  dstX,
                      i32  dstY,
                      i16 *palette,
                      bool paletteChanged,
                      i32 *pOldChecksum,
                      i32  size,
                      bool useOverlay)
{
  bool overlayChanged = false;
  i32 firstNibble[7];
  i32 firstOverlay[7];
  i32 segWidth[7];
  i32 LineEnd   = (((x0&0xff0)+width+15)/16) * 8;
  i32 LineStart = (x0/16) * 8;
  unsigned char nibbles[320];
  unsigned char *pNibbles = nibbles;
  i32 line, segment, xgj, xoj, ygj, currentGraphicLine, lastGraphicLine;
  i32 numPixel, skipPixel, n;
  //i16 *pPalette;
  char *pFirstGroup;
  updateScreenAreaEnterCount++;
  overlayChanged = useOverlay && currentOverlay.m_change;
  currentOverlay.m_change = false;
  if (!paletteChanged && !overlayChanged && !jitterChanged) 
  {
    if (!HasScreenChanged(STScreen+160*y0+LineStart,
                          (LineEnd-LineStart) / 4,
                          height,
                          160-(LineEnd-LineStart),
                          pOldChecksum)) 
    {
      updateScreenAreaLeaveCount++;
      return;
    };
  };

  //createPalette16(palette);
  //if (pOverlayData != NULL)
  //{
  //  pOverlayData->CreateOverlayTable(palette16);
  //  pPalette = pOverlayData->m_table;
  //}
  //else
  //{
  //  pPalette = palette16;
  //};
  jitterChanged = false;
  currentOverlay.Allocate();
  currentOverlay.CreateOverlayTable(palette, useOverlay);
  //pPalette = pOverlayData->m_table;

  STBLTCount++;
  if (useOverlay)
  {
    xgj = xGraphicJitter;
    xoj = xOverlayJitter;
    ygj = yGraphicJitter;
  }
  else
  {
    xgj = 0;
    xoj = 0;
    ygj = 0;
  };
  if (ygj >= 0)
  {
    currentGraphicLine = -ygj;
    lastGraphicLine = height - ygj - 1;
  }
  else
  {
    currentGraphicLine = -ygj;
    lastGraphicLine = height;
  };

/* There are 13 cases
 *    xGraphicJitter  xOverlayJitter
 *         -10            -15    GO-G-B
 *         -10            -10    GO-B
 *         -10             -5    GO-O-B
 *         -10              0    GO-O
 *         -10             10    G-GO-O
 *           0            -10    GO-G
 *           0              0    GO
 *           0             10    G-GO
 *          10            -10    O-GO-G
 *          10              5    B-O-GO
 *          10              0    O-GO
 *          10             10    B-GO
 *          10             15    B-G-GO
 *
 *    Altogether we have seven segments in each line
 *        n0 B
 *        n1 G
 *        n2 O
 *        n3 GO
 *        n4 G
 *        n5 O
 *        n6 B
 *
 *
 * Now we will determine all the parameters for displaying a
 * line for each of 13 possible cases.
 */
  if (xgj < 0)
  {
    if (xoj < 0)
    {
      NotImplemented(0x777107);
    }
    else if (xoj == 0)
    {
      segWidth[0] = 0; 
      segWidth[1] = 0;
      segWidth[2] = 0;
      firstNibble[3] = -xgj;
      firstOverlay[3] = 0;
      segWidth[3] = width + xgj;
      segWidth[4] = 0;
      segWidth[5] = 0;
      segWidth[6] = -xgj;
      firstNibble[6] = -1;
      firstOverlay[6] = width + xgj;
    }
    else
    {
      NotImplemented(0x777109);
    };
  }
  else if (xgj == 0)
  {
    if (xoj < 0)
    {
      NotImplemented(0x777101);
    }
    else if (xoj == 0)
    {
      segWidth[0] = 0;
      segWidth[1] = 0;
      segWidth[2] = xgj;
      firstNibble[2] = -1;
      firstOverlay[2] = 0;
      firstNibble[3] = 0;
      firstOverlay[3] = firstOverlay[2]+xgj;
      segWidth[3] = width - xgj;
      segWidth[4] = 0;
      segWidth[5] = 0;
      segWidth[6] = 0;
    }
    else
    {
      NotImplemented(0x777104);
    };
  }
  else
  {
    if (xoj == 0)
    {
      segWidth[0] = 0;
      segWidth[1] = 0;
      segWidth[2] = xgj;
      firstOverlay[2] = 0;
      firstNibble[2] = -1;
      firstNibble[3] = 0;
      firstOverlay[3] = xgj;
      segWidth[3] = width - xgj;
      segWidth[4] = 0;
      segWidth[5] = 0;
      segWidth[6] = 0;
    }
    else if (xoj < 0)
    {
      NotImplemented(0x777111);
    }
    else
    {
      NotImplemented(0x777112);
    };
  };



  for (line=0; line<height; line++, currentGraphicLine++)
  {
    i32 currentPixel;
    if ((currentGraphicLine < 0) || (currentGraphicLine > lastGraphicLine))
    {
      memset (nibbles,0,width);
    }
    else
    {
      pFirstGroup = STScreen + 160*(y0+currentGraphicLine) + LineStart;
      numPixel = width;
      skipPixel = x0 & 15;
      pNibbles = nibbles;
      if (skipPixel != 0)
      {
        n = 16 - skipPixel;
        if (n > numPixel) n = numPixel;
        Unpack(pFirstGroup, pNibbles, skipPixel, n);
        pNibbles += n;
        numPixel -= n;
        pFirstGroup += 8;
      };
      while (numPixel > 0)
      {
        n = 16;
        if (n > numPixel) n = numPixel;
        Unpack(pFirstGroup, pNibbles, 0, n);
        pNibbles += n;
        numPixel -= n;
        pFirstGroup += 8;
      };
    };

    currentPixel = 0;

    for (segment=0; segment<7; segment++)
    {
      unsigned char *pNibbles;
      ui8 *pOverlay;
      if (segWidth[segment] == 0) continue;
      pNibbles = (firstNibble[segment] < 0) ? black : nibbles + firstNibble[segment]; 
      pOverlay =   (useOverlay && overlayActive)
                 ? currentOverlay.m_overlay+224*(135-line) + firstOverlay[segment]
                 : black;
      switch (size)
      {
      case 1:
        BLT1(pNibbles, 
            bitmap + 1*(320*4*line + currentPixel), 
            segWidth[segment], 
            currentOverlay.m_table, 
            pOverlay);
        break;
      case 2:
        BLT2(pNibbles,
            bitmap + 2*(320*4*line + currentPixel),
            segWidth[segment],
            currentOverlay.m_table, 
            pOverlay);
        break;
      case 3:
        BLT3(pNibbles,
            bitmap + 3*(320*4*line + currentPixel),
            segWidth[segment],
            currentOverlay.m_table, 
            pOverlay);
        break;
      case 4:
        BLT4(pNibbles,
            bitmap + 4*(320*4*line + currentPixel),
            segWidth[segment],
            currentOverlay.m_table, 
            pOverlay);
        break;
      default: break;
      };
      currentPixel += segWidth[segment];
    };
  };
#ifdef BLUR
  if (size == 4)
  {
    int line, x;
    for (line=0; line<height-1; line++)
    {
      for (x=0; x<width-1; x++)
      {
        BLS4(bitmap + 4*320*4*line + 4*x);
      };
    };
  };
#endif
  bitmapInfo.bmiHeader.biSize=0x28;
  bitmapInfo.bmiHeader.biWidth=320*4;
  bitmapInfo.bmiHeader.biHeight=-height * size;
  bitmapInfo.bmiHeader.biPlanes=1;
  bitmapInfo.bmiHeader.biBitCount=16;
  bitmapInfo.bmiHeader.biCompression=BI_RGB;
  bitmapInfo.bmiHeader.biSizeImage=0;
  bitmapInfo.bmiHeader.biXPelsPerMeter=0;
  bitmapInfo.bmiHeader.biYPelsPerMeter=0;
  bitmapInfo.bmiHeader.biClrUsed=0;
  bitmapInfo.bmiHeader.biClrImportant=0;
  UI_SetDIBitsToDevice(
                      // handle to device context
    dstX,             // x-coordinate of upper-left corner of
                      // dest. rect.
    dstY,             // y-coordinate of upper-left corner of
                      // dest. rect.
    width*size,       // source rectangle width
    height*size,      // source rectangle height
    0,                // x-coordinate of lower-left corner of
                      // source rect.
    0,                // y-coordinate of lower-left corner of
                      // source rect.
    0,                // first scan line in array
    height*size,      // number of scan lines
    (char *)bitmap,   // address of array with DIB bits
    &bitmapInfo,      // address of structure with bitmap info.
    DIB_RGB_COLORS    // RGB or palette indexes
    );
  updateScreenAreaLeaveCount++;
};


void MakeBMPPalette(RGBQUAD *colors, PALETTE *stpalette)
{
  // Create RGB color palette from the ST 9-bit palette
  ui32 i;
  for (i=0; i<16; i++)
  {
    ASSERT(stpalette->color[i]<=0x777,"palette");
    colors[i].rgbRed   = (ui8)((((stpalette->color[i]>>8) & 7)*0xff)/0x7);//red
    colors[i].rgbGreen = (ui8)((((stpalette->color[i]>>4) & 7)*0xff)/0x7);//green
    colors[i].rgbBlue  = (ui8)((((stpalette->color[i]>>0) & 7)*0xff)/0x7);//blue
  };
}


void MakeBMPBitmap(ui16 *src, ui8 *dst)
{
  ui32 k[16];
  i32 i, j, line;
  ui16 s[4];
  src += 200 * 80;
  for (line=0; line<200; line++)
  {
    src -= 80;
    for (i=0; i<20; i++)
    {
      //Convert 16 pixels at a time 
      s[0] = LE16(src[0]);
      s[1] = LE16(src[1]);
      s[2] = LE16(src[2]);
      s[3] = LE16(src[3]);
      for (j=0; j<16; j++)
      {
        k[j] = (s[0] >> 15) & 1;
        k[j] |= (s[1] >> 14) & 2;
        k[j] |= (s[2] >> 13) & 4;
        k[j] |= (s[3] >> 12) & 8;
        s[0] <<= 1;
        s[1] <<= 1;
        s[2] <<= 1;
        s[3] <<= 1;
      };
      for (j=0; j<8; j++)
      {
        dst[j] = (ui8)((k[j*2]<<4) + k[j*2+1]);
      };
      src += 4;
      dst += 8;
    };
    src -= 80;
  };
}

void DumpWindow(int FH)
{
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;
  RGBQUAD colors[16];
  ui8    bitmap[320*200/2];
  memset(&bmfh,0,sizeof(bmfh));
  memset(&bmih,0,sizeof(bmih));
  bmih.biSize = sizeof (bmih);
  bmih.biWidth = 320;
  bmih.biHeight = 200;
  bmih.biPlanes = 1;
  bmih.biBitCount = 4;
  bmih.biCompression = BI_RGB;
  bmih.biSizeImage = 0;
  bmih.biClrUsed = 16;
  bmih.biClrImportant = 16;
  *((ui8 *)&bmfh+0) = 'B';
  *((ui8 *)&bmfh+1) = 'M';
  bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(bmih) + 320*200/2;
  bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + bmih.biSize + sizeof(colors);
  _write(FH,&bmfh,sizeof(BITMAPFILEHEADER));
  _write(FH,&bmih,sizeof(BITMAPINFOHEADER));
  MakeBMPPalette(colors, d.DynamicPaletteSwitching?&d.Palette11946:(PALETTE *)&globalPalette);
  _write(FH,colors, sizeof(colors));
  MakeBMPBitmap((ui16 *)physbase(), bitmap);
  _write(FH,bitmap,sizeof(bitmap));
}

/*
void DumpImages(void)
{
  static ui64 prevTime = 0;
  static int count = 0;
  ui64 curTime;
  curTime = UI_GetSystemTime();
  if (curTime > prevTime)
  {
    //for (;;)
    {
      int FH;
      char name[100];
      sprintf(name,"ScreenDumps.bin");
      if (prevTime == 0) 
      {
        prevTime = curTime;
        FH = _open(name, _O_WRONLY|_O_CREAT|_O_TRUNC|O_SEQUENTIAL|_O_BINARY,_S_IWRITE);
      }
      else
      {
        FH = _open(name, _O_WRONLY|_O_APPEND|O_SEQUENTIAL|_O_BINARY);
        //append stops working at 2**32 bytes!!!!
        _lseeki64(FH,32118I64 * count,SEEK_SET);
      };
      prevTime += 167;
      DumpWindow(FH);
      count++;
      _close(FH);
    };
  }
}

*/

bool pc1, pc2;
void display (void){
  static i32 oldChecksumA;
  static i32 oldChecksumB;
  static i32 oldChecksumC;
  static i32 oldChecksumD;
  static i32 numDisplay = 0;
  static bool initialized = false;
  if (!initialized)
  {
    memset(black,0,320);
    initialized = true;
  };
  if (dumpWindow == 1)
  {
    dumpWindow = 0;
    int FH;
    FH = _open("window.bmp", _O_WRONLY|_O_CREAT|O_SEQUENTIAL|_O_BINARY,_S_IWRITE);
    if (FH != -1)
    {
      DumpWindow(FH);
      _close(FH);
    };
  };
  numDisplay++;
  if ((VBLMultiplier!=1) && ((d.Time&0xf)!=0) && (VBLMultiplier!=99)) return;
  if (d.DynamicPaletteSwitching)
  {
    memcpy(palette1,&d.Palette11978, 32);
    memcpy(palette2,&d.Palette11946, 32);
    memcpy(globalPalette,&d.Palette11978,32);
  }
  else
  {
    memcpy(palette1,globalPalette, 32);
    memcpy(palette2,globalPalette, 32);
  };
  pc1 = HasPaletteChanged(palette1, oldPalette1);
  pc2 = HasPaletteChanged(palette2, oldPalette2);
  pc1 = pc1 || ForcedScreenDraw;
  pc2 = pc2 || ForcedScreenDraw;
  ForcedScreenDraw = false;
  {
    if (!fullscreenActive)
    {
      i32 size = screenSize;
      UpdateScreenArea(         //Viewport
                       physbase(),//STScreen,
                       0,
                       0x21,
                       0xe0,
                       0xa9-0x21,
                       0*size,
                       0x21*size,
                       palette2,
                       pc2,
                       &oldChecksumA,
                       size,
                       videoMode==VM_ADVENTURE);
      UpdateScreenArea(   //Text scrolling area
                       physbase(),//STScreen,
                       0,
                       0xa9,
                       320,
                       0xc8-0xa9,
                       0*size,
                       0xa9*size,
                       palette1,
                       pc1,
                       &oldChecksumB,
                       size,
                       false);
      UpdateScreenArea(             //portrait area
                       physbase(),//STScreen,
                       0,
                       0,
                       320,
                       0x21,
                       0*size,
                       0*size,
                       palette1,
                       pc1,
                       &oldChecksumC,
                       size,
                       false);
      UpdateScreenArea(              //spells,weapons,moves
                       physbase(),//STScreen,
                       0xe0,
                       0x21,
                       0x140-0xe0,
                       0xa9-0x21,
                       0xe0*size,
                       0x21*size,
                       palette2,
                       pc2,
                       &oldChecksumD,
                       size,
                       false);
    }
    else
    {
      if (videoSegSize[0] != 0)
      {
        UpdateScreenArea(         //Viewport
                         physbase(),//STScreen,
                         videoSegSrcX[0],
                         videoSegSrcY[0],
                         videoSegWidth[0],
                         videoSegHeight[0],
                         videoSegX[0],
                         videoSegY[0],
                         palette2,
                         pc2,
                         &oldChecksumA,
                         videoSegSize[0],
                         true);
      };
      if (videoSegSize[1] != 0)
      {
        UpdateScreenArea(             //portrait area
                         physbase(),//STScreen,
                         videoSegSrcX[1],
                         videoSegSrcY[1],
                         videoSegWidth[1],
                         videoSegHeight[1],
                         videoSegX[1],
                         videoSegY[1],
                         palette1,
                         pc1,
                         &oldChecksumC,
                         videoSegSize[1],
                         false);
      };
      if (videoSegSize[2] != 0)
      {
        UpdateScreenArea(              //spells,weapons,moves
                         physbase(),//STScreen,
                         videoSegSrcX[2],
                         videoSegSrcY[2],
                         videoSegWidth[2],
                         videoSegHeight[2],
                         videoSegX[2],
                         videoSegY[2],
                         palette2,
                         pc2,
                         &oldChecksumD,
                         videoSegSize[2],
                         false);
      };
      if (videoSegSize[3] != 0)
      {
        UpdateScreenArea(   //Text scrolling area
                         physbase(),//STScreen,
                         videoSegSrcX[3],
                         videoSegSrcY[3],
                         videoSegWidth[3],
                         videoSegHeight[3],
                         videoSegX[3],
                         videoSegY[3],
                         palette1,
                         pc1,
                         &oldChecksumB,
                         videoSegSize[3],
                         false);
      };
    }
  };
#ifdef _MOVIE
  UI_SetDIBitsToDevice(-1,0,0,0,0,0,0,0,0,0,0);
#endif
}


#endif