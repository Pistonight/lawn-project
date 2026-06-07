#pragma once

#ifdef PISTON_PATCH
#include <LawnProject/ConstEnums.h>
#include <SexyAppFramework/Graphics.h>
#else
#include "../ConstEnums.h"
#include "../SexyAppFramework/Graphics.h"
#endif

using namespace Sexy;

class LawnApp;
class Board;

class GameObject
{
  public:
	LawnApp *mApp;
	Board *mBoard;
	int mX;
	int mY;
	int mWidth;
	int mHeight;
	bool mVisible;
	int mRow;
	int mRenderOrder;

  public:
	/*inline*/ GameObject();
	/*inline*/ bool BeginDraw(Graphics *g);
	/*inline*/ void EndDraw(Graphics *g);
	/*inline*/ void MakeParentGraphicsFrame(Graphics *g);
};
