#include "SysFont.h"
#include "GPUImage.h"
#include "SexyAppBase.h"
#include "Graphics.h"
#include "ImageFont.h"
#include "MemoryImage.h"
#include "Renderer.h"
#include "WidgetManager.h"
#include "SexyMatrix.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include FT_OUTLINE_H
#include FT_SYNTHESIS_H
#if SEXY_USE_OPENGL
#include "OpenGL/OpenGLRenderer.h"
#endif
#if SEXY_USE_SDL3_RENDERER
#include "SDL3Renderer/SDL3Renderer.h"
#endif


using namespace Sexy;

static std::string ResolveFontFile(const std::string &theFace)
{
	namespace fs = std::filesystem;

	if (fs::exists(theFace) && fs::is_regular_file(theFace))
		return theFace;

	std::string aBasePath = "fonts/" + theFace;
	if (fs::exists(aBasePath) && fs::is_regular_file(aBasePath))
		return aBasePath;

	std::string aTtfExt = aBasePath + ".ttf";
	if (fs::exists(aTtfExt) && fs::is_regular_file(aTtfExt))
		return aTtfExt;

	std::string aLower = theFace;
	std::transform(aLower.begin(), aLower.end(), aLower.begin(), [](unsigned char c) { return std::tolower(c); });
	std::string aLowerPath = "fonts/" + aLower + ".ttf";
	if (fs::exists(aLowerPath) && fs::is_regular_file(aLowerPath))
		return aLowerPath;

	try
	{
		for (const auto &anEntry : fs::directory_iterator("fonts"))
		{
			if (!anEntry.is_regular_file())
				continue;
			std::string aFileName = anEntry.path().filename().string();
			if (aFileName.size() < 4)
				continue;
			std::string aStem = anEntry.path().stem().string();
			if (aStem.size() == 0)
				continue;
			std::string aStemLower = aStem;
			std::transform(aStemLower.begin(), aStemLower.end(), aStemLower.begin(),
						   [](unsigned char c) { return std::tolower(c); });
			if (aStemLower == aLower)
				return anEntry.path().string();
		}
	}
	catch (...)
	{
	}

	std::string aFallback = "fonts/arial.ttf";
	if (fs::exists(aFallback) && fs::is_regular_file(aFallback))
		return aFallback;

	return "";
}


// The OpenGL renderer's BltRawTexture ignores theClipRect, so the atlas font
// must clip glyph quads itself. The atlas blit is 1:1 (dest size == src size),
// so each clipped destination edge shifts the source rect by the same amount.
// Coordinates are in the destination image space (same space as Graphics
// mClipRect, which is already intersected against theX + mTransX). Returns
// false when the glyph is fully outside the clip rect and should be skipped.
static bool ClipGlyphRects(const Rect &theClipRect, Rect &theDestRect, Rect &theSrcRect)
{
	int aClipLeft = theClipRect.mX;
	int aClipTop = theClipRect.mY;
	int aClipRight = theClipRect.mX + theClipRect.mWidth;
	int aClipBottom = theClipRect.mY + theClipRect.mHeight;

	if (theDestRect.mX < aClipLeft)
	{
		int aDelta = aClipLeft - theDestRect.mX;
		theDestRect.mX += aDelta;
		theDestRect.mWidth -= aDelta;
		theSrcRect.mX += aDelta;
		theSrcRect.mWidth -= aDelta;
	}
	if (theDestRect.mY < aClipTop)
	{
		int aDelta = aClipTop - theDestRect.mY;
		theDestRect.mY += aDelta;
		theDestRect.mHeight -= aDelta;
		theSrcRect.mY += aDelta;
		theSrcRect.mHeight -= aDelta;
	}
	if (theDestRect.mX + theDestRect.mWidth > aClipRight)
	{
		int aDelta = theDestRect.mX + theDestRect.mWidth - aClipRight;
		theDestRect.mWidth -= aDelta;
		theSrcRect.mWidth -= aDelta;
	}
	if (theDestRect.mY + theDestRect.mHeight > aClipBottom)
	{
		int aDelta = theDestRect.mY + theDestRect.mHeight - aClipBottom;
		theDestRect.mHeight -= aDelta;
		theSrcRect.mHeight -= aDelta;
	}

	return theDestRect.mWidth > 0 && theDestRect.mHeight > 0;
}

SysFont::SysFont(const std::string &theFace, int thePointSize, bool bold, bool italics, bool underline)
{
	Init(gSexyAppBase, theFace, thePointSize, bold, italics, underline, false);
}

SysFont::SysFont(SexyAppBase *theApp,
				 const std::string &theFace,
				 int thePointSize,
				 bool bold,
				 bool italics,
				 bool underline)
{
	Init(theApp, theFace, thePointSize, bold, italics, underline, true);
}

void SysFont::Init(SexyAppBase *theApp,
				   const std::string &theFace,
				   int thePointSize,
				   bool bold,
				   bool italics,
				   bool underline,
				   bool useDevCaps)
{
	mApp = theApp;
	mApp->mRenderer->mSysFonts.insert(this);
	mBold = bold;
	mItalic = italics;
	mUnderlined = underline;

	std::string aResolvedPath = ResolveFontFile(theFace);
	if (aResolvedPath.empty())
	{
		aResolvedPath = "fonts/arial.ttf";
	}

	FT_Face aFontFace = nullptr;
	FT_Error anError = FT_New_Face(mApp->mFreeTypeLib, aResolvedPath.c_str(), 0, &aFontFace);
	if (anError || !aFontFace)
	{
#ifdef WIN32
		anError = FT_New_Face(mApp->mFreeTypeLib, ("C:/Windows/Fonts/" + theFace + ".ttf").c_str(), 0, &aFontFace);
#else
		mFontData = nullptr;
		mAscent = 0;
		mHeight = 0;
		mDrawShadow = false;
		mFontName = theFace;
		return;
#endif
	}

	FT_Select_Charmap(aFontFace, FT_ENCODING_UNICODE);
	if (mItalic)
	{
		FT_Matrix matrix = {1 << 16, (FT_Fixed)(0.3 * (1 << 16)), 0, 1 << 16};
		FT_Set_Transform(aFontFace, &matrix, nullptr);
	}

	mFontData = new TrueTypeData(this, aFontFace, thePointSize);

	if (aFontFace->size)
	{
		mAscent = aFontFace->size->metrics.ascender >> 6;
		mHeight = (aFontFace->size->metrics.ascender - aFontFace->size->metrics.descender) >> 6;
	}
	else
	{
		mAscent = 0;
		mHeight = 0;
	}

	mDrawShadow = false;
	mFontName = theFace;
}

void SysFont::Reinit()
{
	if (!mFontData || !mFontData->mFace)
		return;

	FT_Face aFontFace = nullptr;
	bool aPrevFlags = mFontData->mFace->style_flags;

	std::string aResolvedPath = ResolveFontFile(mFontName);
	if (aResolvedPath.empty())
		aResolvedPath = "fonts/arial.ttf";

	FT_Error anError = FT_New_Face(mApp->mFreeTypeLib, aResolvedPath.c_str(), 0, &aFontFace);
	if (anError || !aFontFace)
		anError = FT_New_Face(mApp->mFreeTypeLib, "fonts/arial.ttf", 0, &aFontFace);

	if (anError || !aFontFace)
		return;

	aFontFace->style_flags = aPrevFlags;

	int aOldSize = mFontData->mSize;
	delete mFontData;
	mFontData = nullptr;
	mFontData = new TrueTypeData(this, aFontFace, aOldSize);

	if (aFontFace->size)
	{
		mAscent = aFontFace->size->metrics.ascender >> 6;
		mHeight = (aFontFace->size->metrics.ascender - aFontFace->size->metrics.descender) >> 6;
	}
}

SysFont::SysFont(const SysFont &theSysFont)
{
	mDrawShadow = false;
	if (theSysFont.mFontData == nullptr)
	{
		mApp = theSysFont.mApp;
		mHeight = theSysFont.mHeight;
		mAscent = theSysFont.mAscent;
		mFontData = nullptr;
		mBold = theSysFont.mBold;
		mItalic = theSysFont.mItalic;
		mUnderlined = theSysFont.mUnderlined;
		mFontName = theSysFont.mFontName;
		return;
	}
	Init(theSysFont.mApp, theSysFont.mFontName, theSysFont.mFontData->mSize,
		 theSysFont.mBold, theSysFont.mItalic, theSysFont.mUnderlined, true);
}

SysFont::~SysFont()
{
	if (mFontData)
		delete mFontData;
	mApp->mRenderer->mSysFonts.erase(this);
}

ImageFont *SysFont::CreateImageFont()
{ 
	//todo: uuhhh implement?
	return nullptr;
}
int SysFont::CharWidth(SexyChar theChar) {
    if (mFontData == nullptr) return 0;
    uint32_t c = (uint32_t)(unsigned char)theChar;
    mFontData->EnsureGlyph(c);
    auto it = mFontData->mAtlas.mGlyphs.find(c);
    return it != mFontData->mAtlas.mGlyphs.end() ? it->second.mAdvance : 0;
}

int SysFont::StringWidth(const SexyString &theString)
{
	if (mFontData == nullptr)
	{
		return 0;
	}

	int aWidth = 0;
	auto it = theString.begin();
	auto end = theString.end();
	while (it != end)
	{
		uint32_t c = utf8::next(it, end);
		mFontData->EnsureGlyph(c);
		aWidth += mFontData->mAtlas.mGlyphs[c].mAdvance;
	}
	return aWidth;
}

void SysFont::DrawString(
	Graphics *g, int theX, int theY, const SexyString &theString, const Color &theColor, const Rect &theClipRect)
{
	if (mFontData == nullptr)
	{
		return;
	}

	// Pass 1: ensure all glyphs are in the CPU buffer
	{
		auto it = theString.begin();
		auto end = theString.end();
		while (it != end)
		{
			mFontData->EnsureGlyph(utf8::next(it, end));
		}
	}
	mFontData->FlushAtlas();

	// The mStaticImage sentinel has no real backing surface (its Blt* are
	// no-ops), so route those straight to the renderer; everything else goes
	// through mDestImage, which forwards screen images to the renderer itself.
	// Either way the renderer ignores theClipRect, so clip the quads here.
	bool aToStatic = (g->mDestImage == &Graphics::mStaticImage);

	// Pass 2: draw one full run of glyphs with a uniform pixel offset and color.
	// theOfs is 0 for the main text, 1 for the offset drop shadow.
	auto aDrawRun = [&](int theOfs, const Color &theGlyphColor)
	{
		int posX = theX;
		int posY = theY;
		auto it = theString.begin();
		auto end = theString.end();
		while (it != end)
		{
			uint32_t c = utf8::next(it, end);
			auto glyphIt = mFontData->mAtlas.mGlyphs.find(c);
			if (glyphIt == mFontData->mAtlas.mGlyphs.end()) { posX += mHeight / 2; continue; }
			const GlpyhAtlasEntry &aGlyph = glyphIt->second;

			// aDrawX/Y = top-left of glyph bitmap. posY is the baseline; bearingY
			// is pixels above the baseline → subtract to get the top. Graphics
			// carries mTransX/mTransY (the widget's accumulated screen offset);
			// just like Graphics::DrawImage, we add it before blitting so the
			// destination receives absolute coordinates -- whether mDestImage is
			// the real screen GPUImage or an offscreen image.
			int aDrawX = posX + aGlyph.mBearingX + g->mTransX + theOfs;
			int aDrawY = posY - aGlyph.mBearingY + g->mTransY + theOfs;

			Rect aDestRect(aDrawX, aDrawY, aGlyph.mWidth, aGlyph.mHeight);
			Rect aSrcRect(aGlyph.mX, aGlyph.mY, aGlyph.mWidth, aGlyph.mHeight);
			if (ClipGlyphRects(theClipRect, aDestRect, aSrcRect))
			{
				if (aToStatic)
					mApp->mRenderer->BltRawTexture(
						mFontData->mAtlas.mAtlas, mFontData->mAtlas.mWidth, mFontData->mAtlas.mHeight,
						aDestRect, aSrcRect, theClipRect, theGlyphColor, 0);
				else
					g->mDestImage->BltRawTexture(
						mFontData->mAtlas.mAtlas, mFontData->mAtlas.mWidth, mFontData->mAtlas.mHeight,
						aDestRect, aSrcRect, theClipRect, theGlyphColor, 0);
			}

			posX += aGlyph.mAdvance;
		}
	};

	// Draw the entire shadow run first so no later glyph's shadow lands on top
	// of an earlier glyph's body (matches the engine's order-based ImageFont
	// rendering), then draw the main text over it.
	if (mDrawShadow)
		aDrawRun(1, Color(0, 0, 0, 200));
	aDrawRun(0, theColor);
}

Font *SysFont::Duplicate()
{
	return new SysFont(*this);
}

bool SysFont::DrawStringMatrix(Graphics *g, const SexyMatrix3 &theMatrix, const SexyString &theString, const Color &theColor)
{
	if (mFontData == nullptr)
		return true;
	// Extract translation from the 3x3 matrix (m02 = tx, m12 = ty).
	// Rotation/scale are not currently supported for atlas-based fonts.
	//
	// m12 is the text baseline -- matching the ImageFont matrix path, where
	// glyphs render at mOffset.mY - mAscent relative to this origin -- and
	// DrawString already treats posY as the baseline, so pass it through
	// (do NOT add mAscent, which would double-count and push text down).
	//
	// The matrix carries absolute coordinates: TodBltMatrix blits the
	// ImageFont matrix path WITHOUT adding g->mTransX/mTransY. DrawString,
	// however, DOES add g->mTransX/mTransY (the Font::DrawString contract,
	// same as ImageFont::DrawString -> g->DrawImage). Subtract them here so
	// DrawString re-adds them and the net position stays absolute.
	int posX = (int)theMatrix.m02 - g->mTransX;
	int posY = (int)theMatrix.m12 - g->mTransY;
	DrawString(g, posX, posY, theString, theColor, g->mClipRect);
	return true;
}
void TrueTypeData::Init()
{
	if (mFace == nullptr)
	{
		return;
	}

	FT_Set_Pixel_Sizes(mFace, 0, mSize);

	if (mAtlas.mAtlas != nullptr)
	{
		mFont->mApp->mRenderer->DeleteTexture(mAtlas.mAtlas);
	}
	mAtlas.mAtlas     = nullptr;
	mAtlas.mDirty     = false;
	mAtlas.mCursorX   = mAtlas.mPadding;
	mAtlas.mCursorY   = mAtlas.mPadding;
	mAtlas.mRowHeight = 0;
	mAtlas.mGlyphs.clear();
	mAtlas.mPixels.assign(mAtlas.mWidth * mAtlas.mHeight, 0);

	// for (uint32_t c = ' '; c <= '~'; c++)
	// {
	// 	EnsureGlyph(c);
	// }
	// for (uint32_t c = 0x00A0; c < 0x024F; c++)
	// {
	// 	EnsureGlyph(c);
	// }

	// CJK glyphs loaded on demand in DrawString; FlushAtlas deferred to render thread
}

void TrueTypeData::EnsureGlyph(uint32_t c)
{
	if (mAtlas.mGlyphs.find(c) != mAtlas.mGlyphs.end())
	{
		return;
	}

	FT_Load_Char(mFace, c, FT_LOAD_RENDER);
	if (mFont->mBold)
	{
		FT_GlyphSlot_Embolden(mFace->glyph);
	}

	FT_Bitmap &aBitmap = mFace->glyph->bitmap;

	GlpyhAtlasEntry aEntry;
	aEntry.mWidth    = aBitmap.width;
	aEntry.mHeight   = aBitmap.rows;
	aEntry.mBearingX = mFace->glyph->bitmap_left;
	aEntry.mBearingY = mFace->glyph->bitmap_top;
	aEntry.mAdvance  = mFace->glyph->advance.x >> 6;

	if (mAtlas.mCursorX + aEntry.mWidth + mAtlas.mPadding > mAtlas.mWidth)
	{
		mAtlas.mCursorX   = mAtlas.mPadding;
		mAtlas.mCursorY  += mAtlas.mRowHeight + mAtlas.mPadding;
		mAtlas.mRowHeight  = 0;
	}

	if (mAtlas.mCursorY + aEntry.mHeight + mAtlas.mPadding > mAtlas.mHeight)
	{
		aEntry.mX = 0;
		aEntry.mY = 0;
		mAtlas.mGlyphs[c] = aEntry;
		return;
	}

	aEntry.mX = mAtlas.mCursorX;
	aEntry.mY = mAtlas.mCursorY;

	for (int y = 0; y < aEntry.mHeight; y++)
	{
		for (int x = 0; x < aEntry.mWidth; x++)
		{
			uint8_t anAlpha = aBitmap.buffer[y * aBitmap.pitch + x];
			mAtlas.mPixels[(aEntry.mY + y) * mAtlas.mWidth + (aEntry.mX + x)] =
                (anAlpha << 24) | 0x00FFFFFF;
		}
	}

	mAtlas.mCursorX  += aEntry.mWidth + mAtlas.mPadding;
	mAtlas.mRowHeight  = std::max(mAtlas.mRowHeight, aEntry.mHeight);
	mAtlas.mDirty      = true;
	mAtlas.mGlyphs[c]  = aEntry;
}

void TrueTypeData::FlushAtlas()
{
	if (!mAtlas.mDirty)
	{
		return;
	}

	if (mAtlas.mAtlas != nullptr)
	{
		mFont->mApp->mRenderer->DeleteTexture(mAtlas.mAtlas);
	}

	mAtlas.mAtlas = mFont->mApp->mRenderer->CreateTexture(
		mAtlas.mPixels.data(), mAtlas.mWidth, mAtlas.mHeight, RawPixelFormat::RAW_FORMAT_RGBA, 1);
	mAtlas.mDirty = false;
}

TrueTypeData::~TrueTypeData()
{
	if (mAtlas.mAtlas != nullptr)
	{
		mFont->mApp->mRenderer->DeleteTexture(mAtlas.mAtlas);
	}
	mAtlas.mGlyphs.clear();
	FT_Done_Face(mFace);
}
