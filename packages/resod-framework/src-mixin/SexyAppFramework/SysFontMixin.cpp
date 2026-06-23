#include <SexyAppFramework/SysFont.h>
#include <SexyAppFramework/Renderer.h>
#include <SexyAppFramework/SexyAppBase.h>

#include <Piston/SysFont.h>

namespace Sexy {

void SysFont::Init(SexyAppBase* theApp, const std::string& theFace, int thePointSize, bool bold,
                   bool italics, bool underline, bool useDevCaps) {
    mApp = theApp;
    mApp->mRenderer->mSysFonts.insert(this);
    mBold = bold;
    mItalic = italics;
    mUnderlined = underline;

    mFontData = Piston::SysFont::MakeTrueTypeData(*theApp, theFace, thePointSize, bold);
    if (mFontData && mFontData->mFace) {
        FT_Face face = mFontData->mFace;
        FT_Select_Charmap(face, FT_ENCODING_UNICODE);
        if (mItalic) {
            FT_Matrix matrix = {1 << 16, (FT_Fixed)(0.3 * (1 << 16)), 0, 1 << 16};
            FT_Set_Transform(face, &matrix, nullptr);
        }
        if (face->size) {
            mAscent = face->size->metrics.ascender >> 6;
            mHeight = (face->size->metrics.ascender - face->size->metrics.descender) >> 6;
        } else {
            mAscent = 0;
            mHeight = 0;
        }
    }

    mDrawShadow = false;
    mFontName = theFace;
}

void SysFont::Reinit() {
    if (!mFontData || !mFontData->mFace)
        return;

    bool aPrevFlags = mFontData->mFace->style_flags;
    int aOldSize = mFontData->mSize;

    mFontData = Piston::SysFont::MakeTrueTypeData(*mApp, mFontName, aOldSize, mBold);
    if (!mFontData || !mFontData->mFace)
        return;

    FT_Face face = mFontData->mFace;
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    face->style_flags = aPrevFlags;
    if (face->size) {
        mAscent = face->size->metrics.ascender >> 6;
        mHeight = (face->size->metrics.ascender - face->size->metrics.descender) >> 6;
    }
}

}
