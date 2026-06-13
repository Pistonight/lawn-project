#ifndef __EFFECTSYSTEM_H__
#define __EFFECTSYSTEM_H__

#include <SexyAppFramework/SWTri.h>
#include <SexyAppFramework/Graphics.h>

#define MAX_TRIANGLES 256

class TodTriVertex
{
  public:
	float x;
	float y;
	float u;
	float v;
	unsigned long color;
};

class TodTriangleGroup
{
  public:
    Sexy::Image *mImage;
    Sexy::TriVertex mVertArray[MAX_TRIANGLES][3];
	int mTriangleCount;
	int mDrawMode;

	TodTriangleGroup();
	void DrawGroup(Sexy::Graphics *g);
	void AddTriangle(Sexy::Graphics *g,
					 Sexy::Image *theImage,
					 const Sexy::SexyMatrix3 &theMatrix,
					 const Sexy::Rect &theClipRect,
					 const Sexy::Color &theColor,
					 int theDrawMode,
					 const Sexy::Rect &theSrcRect);
};

extern bool gTodTriangleDrawAdditive; //0x6A9EF5

extern void TodDrawTriangle_8888_TEX1_TALPHA0_MOD0_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA0_MOD0_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA0_MOD0_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA0_MOD0_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA0_MOD1_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA0_MOD1_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA0_MOD1_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA0_MOD1_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA1_MOD0_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA1_MOD0_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA1_MOD0_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA1_MOD0_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA1_MOD1_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA1_MOD1_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA1_MOD1_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_8888_TEX1_TALPHA1_MOD1_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD0_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD0_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD0_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD0_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD1_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD1_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD1_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD1_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD0_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD0_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD0_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD0_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD1_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD1_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD1_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD1_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD0_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD0_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD0_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD0_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD1_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD1_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD1_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD1_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD0_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD0_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD0_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD0_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD1_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD1_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD1_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD1_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA0_MOD0_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA0_MOD0_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA0_MOD0_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA0_MOD0_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA0_MOD1_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA0_MOD1_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA0_MOD1_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA0_MOD1_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA1_MOD0_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA1_MOD0_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA1_MOD0_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA1_MOD0_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA1_MOD1_GLOB0_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA1_MOD1_GLOB0_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA1_MOD1_GLOB1_BLEND0(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0555_TEX1_TALPHA1_MOD1_GLOB1_BLEND1(Sexy::SWHelper::SWVertex *pVerts,
																void *pFrameBuffer,
																const unsigned int bytepitch,
																const Sexy::SWHelper::SWTextureInfo *textureInfo,
																Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD0_GLOB1_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD0_GLOB1_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD1_GLOB0_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD1_GLOB0_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD1_GLOB1_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA0_MOD1_GLOB1_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD0_GLOB0_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD0_GLOB0_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD0_GLOB1_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD0_GLOB1_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD1_GLOB0_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD1_GLOB0_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD1_GLOB1_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0888_TEX1_TALPHA1_MOD1_GLOB1_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD0_GLOB1_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD0_GLOB1_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD1_GLOB0_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD1_GLOB0_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD1_GLOB1_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA0_MOD1_GLOB1_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD0_GLOB0_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD0_GLOB0_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD0_GLOB1_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD0_GLOB1_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD1_GLOB0_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD1_GLOB0_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD1_GLOB1_BLEND0_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);
extern void TodDrawTriangle_0565_TEX1_TALPHA1_MOD1_GLOB1_BLEND1_ADDITIVE(Sexy::SWHelper::SWVertex *pVerts,
																		 void *pFrameBuffer,
																		 const unsigned int bytepitch,
																		 const Sexy::SWHelper::SWTextureInfo *textureInfo,
																		 Sexy::SWHelper::SWDiffuse &globalDiffuse);

class Reanimation;
class TodParticleHolder;
class TrailHolder;
class ReanimationHolder;
class AttachmentHolder;
class EffectSystem
{
  public:
	TodParticleHolder *mParticleHolder;
	TrailHolder *mTrailHolder;
	ReanimationHolder *mReanimationHolder;
	AttachmentHolder *mAttachmentHolder;

  public:
	EffectSystem()
		: mParticleHolder(nullptr), mTrailHolder(nullptr), mReanimationHolder(nullptr), mAttachmentHolder(nullptr)
	{
	}
	~EffectSystem()
	{
	}

	void EffectSystemInitialize();
	void EffectSystemDispose();
	void EffectSystemFreeAll();
	void ProcessDeleteQueue();
	void Update();
};
extern EffectSystem *gEffectSystem; //[0x6A9EB8]

#endif
