#ifndef __SYSFONT_H__
#define __SYSFONT_H__

#include "Font.h"
#include <unordered_map>
#include <freetype/freetype.h>

namespace Sexy
{

class ImageFont;
class SexyAppBase;

struct GlpyhAtlasEntry
{
	int mX;
	int mY;
	int mWidth;
	int mHeight;
	int mBearingX;
	int mBearingY;
	int mAdvance;
};
struct GlyphAtlas
{
	int mPadding = 1;
	int mWidth = 4096;
	int mHeight = 2048;
	void *mAtlas = nullptr;
	std::unordered_map<uint32_t, GlpyhAtlasEntry> mGlyphs {};
	std::vector<uint32_t> mPixels {};
	bool mDirty = false;
	int mCursorX = 1;
    int mCursorY = 1;
    int mRowHeight = 0;
};


class TrueTypeData;

class SysFont : public Font
{
  public:
	TrueTypeData* mFontData;
	SexyAppBase *mApp;
	std::string mFontName;
	bool mDrawShadow;
	bool mBold;
	bool mItalic;
	bool mUnderlined;


	void Init(SexyAppBase *theApp,
			  const std::string &theFace,
			  int thePointSize,
			  bool bold,
			  bool italics,
			  bool underline,
			  bool useDevCaps);

	void Reinit();

  public:
	SysFont(
		const std::string &theFace, int thePointSize, bool bold = false, bool italics = false, bool underline = false);
	SysFont(SexyAppBase *theApp,
			const std::string &theFace,
			int thePointSize,
			bool bold = false,
			bool italics = false,
			bool underline = false);
	SysFont(const SysFont &theSysFont);

	virtual ~SysFont();

	ImageFont *CreateImageFont();
	virtual int StringWidth(const SexyString &theString);
	virtual int CharWidth(SexyChar theChar);
	virtual void DrawString(
		Graphics *g, int theX, int theY, const SexyString &theString, const Color &theColor, const Rect &theClipRect);

	virtual Font *Duplicate();
	virtual bool DrawStringMatrix(Graphics *g, const SexyMatrix3 &theMatrix, const SexyString &theString, const Color &theColor);

};

struct TrueTypeData
{
	GlyphAtlas mAtlas;
	SysFont *mFont;
	FT_Face mFace;
	int mSize;
	bool mIsDirty;

	TrueTypeData(SysFont *theFontPtr, FT_Face &theFace, int theSize) : mFont(theFontPtr), mFace(theFace), mSize(theSize)
	{
		Init();
	}

	~TrueTypeData();

	void Init();
	void EnsureGlyph(uint32_t c);
	void FlushAtlas();
};

} // namespace Sexy

#endif //__SYSFONT_H__
