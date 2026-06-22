#include <SexyAppFramework/SysFont.h>
#include <SexyAppFramework/Renderer.h>
#include <SexyAppFramework/SexyAppBase.h>




namespace Sexy {


void SysFont::Init(SexyAppBase* theApp, const std::string& theFace, int thePointSize, bool bold,
                   bool italics, bool underline, bool useDevCaps) {
    mApp = theApp;
    mApp->mRenderer->mSysFonts.insert(this);
    mBold = bold;
    mItalic = italics;
    mUnderlined = underline;

    auto fontObj = std::make_shared<Piston::SysFont::FontObj>(mApp->mFreeTypeLib, theFace);
    auto fontFace = fontObj->GetFontFace();
    if (fontFace) {
        FT_Face face = *fontFace;
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
        mFontData = new TrueTypeData(this, face, thePointSize);
        mFontData->mFontObj = std::move(fontObj);
    }

    mDrawShadow = false;
    mFontName = theFace;
}

void SysFont::Reinit() {
    if (!mFontData || !mFontData->mFace)
        return;

    bool aPrevFlags = mFontData->mFace->style_flags;
    int aOldSize = mFontData->mSize;

    delete mFontData;
    mFontData = nullptr;

    auto fontObj = std::make_shared<Piston::SysFont::FontObj>(mApp->mFreeTypeLib, mFontName);
    auto fontFace = fontObj->GetFontFace();
    if (fontFace) {
        FT_Face face = *fontFace;
        FT_Select_Charmap(face, FT_ENCODING_UNICODE);
        face->style_flags = aPrevFlags;
        mFontData = new TrueTypeData(this, face, aOldSize);
        if (face->size) {
            mAscent = face->size->metrics.ascender >> 6;
            mHeight = (face->size->metrics.ascender - face->size->metrics.descender) >> 6;
        }
    }
}

SysFont::SysFont(const SysFont& theSysFont) {
    mApp = theSysFont.mApp;
    mHeight = theSysFont.mHeight;
    mAscent = theSysFont.mAscent;
    if (theSysFont.mFontData) {
        mFontData = new TrueTypeData(this, theSysFont.mFontData->mFace, theSysFont.mFontData->mSize);
    }
    mFontData->mFont = this;
    mBold = theSysFont.mBold;
    mItalic = theSysFont.mItalic;

    mDrawShadow = false;
}

TrueTypeData::~TrueTypeData() {
    if (mAtlas.mAtlas != nullptr) {
        mFont->mApp->mRenderer->DeleteTexture(mAtlas.mAtlas);
    }
    mAtlas.mGlyphs.clear();
    // may need to manually delete the font face obj if it's not associated with
    // our mixin FontObj
    if (!mFontObj && mFace) {
        FT_Done_Face(mFace);
    }
}

}
