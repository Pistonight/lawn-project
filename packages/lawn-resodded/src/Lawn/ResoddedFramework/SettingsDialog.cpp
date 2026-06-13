#include "SettingsDialog.h"
#ifdef _WIN32
#include <ShlObj_core.h>
#endif
#include <Lawn/LawnCommon.h>
#include <LawnProject/LawnApp.h>
#include <Piston/TodLib/Resources.h>
#include <SexyAppFramework/Font.h>
#include <SexyAppFramework/Renderer.h>
#include <SexyAppFramework/Checkbox.h>
#include <SexyAppFramework/Window.h>
#include <SexyAppFramework/ListWidget.h>
#include <Piston/BuildInfo.h>

static int gUserListWidgetColors[][3] = {
	{23, 24, 35},
	{0, 0, 0},
	{235, 225, 180},
	{255, 255, 255},
	{20, 180, 15}};

static int gDefaultWindowSizes[3][2] = {
    // 4:3
	{800, 600},
	{1200, 900},
	{1600, 1200},

	// // 16:9
	// {1280, 720},
	// {1600, 900},
	// {1920, 1080},
	//
	// // 16:10
	// {1280, 800},
	// {1680, 1050},
};


SettingsDialog::SettingsDialog(LawnApp *theApp)
	: LawnDialog(theApp, DIALOG_SETTINGS, true, "[SETTINGS_HEADER]", "", "", BUTTONS_NONE)
{
	mApp = theApp;

	
	SDL_Rect aUsableBounds{};
	SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(mApp->mWindow->mInternalWindow), &aUsableBounds);

	const int aPadding = 64;

	for (const auto &aSize : gDefaultWindowSizes)
	{
		if (aSize[0] <= aUsableBounds.w - aPadding && aSize[1] <= aUsableBounds.h - aPadding)
		{
			mValidSizes.push_back({aSize[0], aSize[1]});
		}
	}

	mOptionsSlider = new LawnScrollbar(mApp);
	mOptionsSlider->mSliderHeightPercent = 0.57f;
	mOptionsSlider->mStepMultiplier = 1.0f;
	mOptionsSlider->Resize(500, 90, 8, 140);
	mOptionsSlider->mScrollMultiplier = 0.09f;

	mFullscreenCheckbox = MakeNewCheckbox(SettingsDialog::SETTINGS_FULLSCREEN, this, !theApp->mIsWindowed);

	mVSyncCheckbox = MakeNewCheckbox(SettingsDialog::SETTINGS_VSYNC, this, theApp->mWaitForVSync);

	mHighQualityCheckbox = MakeNewCheckbox(SettingsDialog::SETTINGS_HIGHQUALITY, this, theApp->mIs3D);

	mSaveFileButton = MakeButton(SETTINGS_OPEN_SAVE_FOLDER, this, "[SETTINGS_OPEN_SAVE_FOLDER]");

	mApplyButton = MakeButton(SETTINGS_BACK, this, "[SETTINGS_BACK]");
	
	mRendererList = new ListWidget(SETTINGS_RENDER_LIST, Sexy::FONT_PICO129, this);
	mRendererList->SetColors(gUserListWidgetColors, LENGTH(gUserListWidgetColors));
	mRendererList->mDrawOutline = true;
	mRendererList->mJustify = ListWidget::JUSTIFY_CENTER;
	mRendererList->mItemHeight = 24;
	for (int i = RenderingBackend::BACKEND_NONE + 1; i < RenderingBackend::NUM_BACKENDS; i++)
	{
		for (auto backend : gRenderBackends)
		{
			if (backend.first == i)
				mRendererList->AddLine(backend.second, false);

		}
	}
	mRendererList->SetSelect(mApp->mDesiredBackend - 1);
	
	mSizesList = new ListWidget(SETTINGS_WINDOW_SIZES, Sexy::FONT_PICO129, this);
	mSizesList->SetColors(gUserListWidgetColors, LENGTH(gUserListWidgetColors));
	mSizesList->mDrawOutline = true;
	mSizesList->mJustify = ListWidget::JUSTIFY_CENTER;
	mSizesList->mItemHeight = 24;
	int aSelectedIndex = mValidSizes.size();
	int anIndex = 0;
	for (const auto &aSize : mValidSizes)
	{
		mSizesList->AddLine(StrFormat("%dx%d", aSize.mWidth, aSize.mHeight), false);
		if (mApp->mRenderer->mPresentationRect.mWidth == aSize.mWidth && mApp->mRenderer->mPresentationRect.mHeight == aSize.mHeight)
			aSelectedIndex = anIndex;
		anIndex++;
	}
	mSizesList->AddLine("Custom", false);
	mSizesList->SetSelect(aSelectedIndex);

    const auto* aUpscaler = mApp->mRenderer->GetUpscaler();
    if (aUpscaler) {
        mShaderList = new ListWidget(SETTINGS_SHADER, Sexy::FONT_PICO129, this);
        mShaderList->SetColors(gUserListWidgetColors, LENGTH(gUserListWidgetColors));
        mShaderList->mDrawOutline = true;
        mShaderList->mJustify = ListWidget::JUSTIFY_CENTER;
        mShaderList->mItemHeight = 24;
        mShaderList->AddLine("Nearest", false);
        mShaderList->AddLine("Bilinear", false);
        mShaderList->AddLine("FSR", false);
        int aSelectedShader = static_cast<int>(mApp->GetUpscaleMode());
        mShaderList->SetSelect(aSelectedShader);
    }

	CalcSize(211, 214);
}

SettingsDialog::~SettingsDialog()
{
	delete mOptionsSlider;
	delete mApplyButton;
	delete mSaveFileButton;
	delete mVSyncCheckbox;
	delete mFullscreenCheckbox;
	delete mHighQualityCheckbox;
	delete mRendererList;
	delete mSizesList;
    delete mShaderList;
}

void SettingsDialog::Draw(Graphics* g)
{
	LawnDialog::Draw(g);

	int aMaxContentHeight = 250;
	float aMaxScroll = std::max(0.0f, (float)aMaxContentHeight - mOptionsSlider->mAllowedMouseZone.mHeight);

	float aScrollOffset = mOptionsSlider->GetValue() * aMaxScroll;

	g->PushState();
	g->Translate(mApplyButton->mX, mApplyButton->mY);
	mApplyButton->Draw(g);
	g->PopState();

	g->PushState();
	g->Translate(mSaveFileButton->mX, mSaveFileButton->mY);
	mSaveFileButton->Draw(g);
	g->PopState();

	g->PushState();
	g->SetClipRect(Rect(mOptionsSlider->mAllowedMouseZone.mX - mX,
						mOptionsSlider->mAllowedMouseZone.mY - mY,
						mOptionsSlider->mAllowedMouseZone.mWidth,
						mOptionsSlider->mAllowedMouseZone.mHeight));
	g->Translate(35, 120 - aScrollOffset);

    int aScroll = aScrollOffset - GetTop();
    int aX = 0;
	int aY = 0;

    DrawCheckbox(g, 40 + aX, aY, aScroll, *mVSyncCheckbox, "[SETTINGS_VSYNC]");
    aX += 120;
    DrawCheckbox(g, 40 + aX, aY, aScroll, *mFullscreenCheckbox, "[SETTINGS_FULLSCREEN]");
    aX += 120;
    DrawCheckbox(g, 40 + aX, aY, aScroll , *mHighQualityCheckbox, "[SETTINGS_HIGHQUALITY]");

	aY += 65;
    aX = 0;

    DrawList(g, 40 + aX, aY, aScroll, 2, *mRendererList, "[SETTINGS_RENDERER_BACKEND]");
    aX += 140;
    DrawList(g, 40 + aX, aY, aScroll, mValidSizes.size() + 1, *mSizesList, "[SETTINGS_WINDOW_SIZE]");
    if (mShaderList) {
        aX += 140;
        DrawList(g, 40 + aX, aY, aScroll, static_cast<int>(piston::UpscaleMode::NumUpscalers), *mShaderList, "[SETTINGS_SHADER]");
    }

	SexyString aVersionString = "ResoddedFramework " + LawnApp::gResoddedVersion.toString();

	TodDrawString(g, aVersionString,
				  mOptionsSlider->mAllowedMouseZone.mWidth - Sexy::FONT_BRIANNETOD12->StringWidth(aVersionString) - 27,
				  aMaxContentHeight - Sexy::FONT_BRIANNETOD12->GetHeight(), Sexy::FONT_BRIANNETOD12, Color::White,
				  DrawStringJustification::DS_ALIGN_LEFT);

#if GIT_AVAILABLE

	SexyString aHash = GIT_HASH;

	SexyString aGitString = "Git: Hash (" + aHash + ")" + (GIT_IS_DIRTY ? " WORK IN PROGRESS" : "");

	TodDrawString(g, aGitString,
				  mOptionsSlider->mAllowedMouseZone.mWidth - Sexy::FONT_BRIANNETOD12->StringWidth(aGitString) - 27,
				  aMaxContentHeight, Sexy::FONT_BRIANNETOD12, Color::White, DrawStringJustification::DS_ALIGN_LEFT);

#endif

	g->PopState();
}

void SettingsDialog::DrawCheckbox(Graphics* g, int theX, int theY, int theScroll, Sexy::Checkbox& checkbox, const SexyString& title) {
	checkbox.Resize(theX, theY - theScroll, 46, 45);
	checkbox.mDisabled =
		(checkbox.mY + mY + checkbox.mHeight) < mOptionsSlider->mAllowedMouseZone.mY ||
		(checkbox.mY + mY) > (mOptionsSlider->mAllowedMouseZone.mY + mOptionsSlider->mAllowedMouseZone.mHeight);
	TodDrawString(g, title, checkbox.mX + 10, theY + 33, Sexy::FONT_BRIANNETOD12, Color::White,
				  DrawStringJustification::DS_ALIGN_LEFT);
}

void SettingsDialog::DrawList(Graphics* g, int theX, int theY, int theScroll, int listLen, Sexy::ListWidget& list, const SexyString& title) {
	list.Resize(theX, theY - theScroll, 130, 26 * listLen);
	list.mDisabled =
		(list.mY + mY + list.mHeight) < mOptionsSlider->mAllowedMouseZone.mY ||
		(list.mY + mY) > (mOptionsSlider->mAllowedMouseZone.mY + mOptionsSlider->mAllowedMouseZone.mHeight);
	TodDrawString(g, title, list.mX - 30, theY, Sexy::FONT_BRIANNETOD12, Color::White,
				  DrawStringJustification::DS_ALIGN_LEFT);
}

void SettingsDialog::AddedToManager(WidgetManager *theWidgetManager)
{
	LawnDialog::AddedToManager(theWidgetManager);
	AddWidget(mOptionsSlider);
	AddWidget(mApplyButton);
	AddWidget(mVSyncCheckbox);
	AddWidget(mFullscreenCheckbox);
	AddWidget(mSaveFileButton);
	AddWidget(mHighQualityCheckbox);
	AddWidget(mRendererList);
	AddWidget(mSizesList);
    if (mShaderList) {
        AddWidget(mShaderList);
    }
}

//0x45D8E0
void SettingsDialog::RemovedFromManager(WidgetManager *theWidgetManager)
{
	LawnDialog::RemovedFromManager(theWidgetManager);
	RemoveWidget(mOptionsSlider);
	RemoveWidget(mApplyButton);
	RemoveWidget(mVSyncCheckbox);
	RemoveWidget(mFullscreenCheckbox);
	RemoveWidget(mSaveFileButton);
	RemoveWidget(mHighQualityCheckbox);
	RemoveWidget(mRendererList);
	RemoveWidget(mSizesList);
    if (mShaderList) {
        RemoveWidget(mShaderList);
    }
}

void SettingsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	LawnDialog::Resize(theX, theY, theWidth, theHeight);
	mOptionsSlider->Resize(mWidth - 60, 110, 8, 200);
	mOptionsSlider->mAllowedMouseZone = Rect(mX + 35, mY + 120, mWidth - 70, mHeight - 240);
	mApplyButton->Resize(350, 331, 209, 46);
	mSaveFileButton->Resize(35, 331, 330, 46);
	SetWidgetClipping(Rect(35, 120, mWidth - 70, mHeight - 240));
}

void SettingsDialog::MouseWheel(int theDelta)
{
	LawnDialog::MouseWheel(theDelta);
	mOptionsSlider->MouseWheel(theDelta);
}

void SettingsDialog::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_GRAVEBUTTON);
}

void SettingsDialog::ButtonDepress(int theId)
{
	switch (theId)
	{
		case SettingsDialog::SETTINGS_BACK:
		{
			mApp->KillDialog(mId);
			break;
		}
		case SettingsDialog::SETTINGS_OPEN_SAVE_FOLDER:
		{
			SexyString aSaveFileFolder = GetAppDataFolder();
			#ifdef _WIN32
			ShellExecuteA(NULL, "open", aSaveFileFolder.c_str(), NULL, NULL, SW_SHOWDEFAULT);
			#else
			SexyString aFailString = StrFormat("Couldn't open the folder on this platform.\nThe path is: \n%s", aSaveFileFolder.c_str());

			mApp->DoDialog(Dialogs::DIALOG_INFO, true, "Failed", aFailString, "OK", Dialog::BUTTONS_FOOTER);
			#endif
			break;
		}
	}
}

void SettingsDialog::ListClicked(int theId, int theIdx, int theClickCount)
{
	if (theId == SETTINGS_RENDER_LIST)
	{
		if (theIdx != mApp->mDesiredBackend - 1)
		{
			mRendererList->SetSelect(theIdx);
			mApp->mDesiredBackend = (RenderingBackend)(theIdx + 1);
			SexyString aBackendName = "";
			for (int i = RenderingBackend::BACKEND_NONE + 1; i < RenderingBackend::NUM_BACKENDS; i++)
			{
				for (auto backend : gRenderBackends)
				{
					if (backend.first == mApp->mDesiredBackend)
						aBackendName = backend.second;
				}
			}
			mApp->RegistryWriteInteger("DesiredBackend", mApp->mDesiredBackend);
			mApp->WriteToRegistry();

			if (mApp->mDesiredBackend != mApp->mRenderer->mCurrentBackend)
			{
				SexyString anInfoString =
					StrFormat("Rendering Backend has been changed to %s\nRestart the game to apply the changes",
							  aBackendName.c_str());
				mApp->DoDialog(Dialogs::DIALOG_INFO, true, "", anInfoString, "OK", Dialog::BUTTONS_FOOTER);
			}
		}
	}
	else if (theId == SETTINGS_WINDOW_SIZES)
	{

		if (theIdx < mValidSizes.size())
		{
			SDL_RestoreWindow(mApp->mWindow->mInternalWindow);
			SDL_SetWindowSize(mApp->mWindow->mInternalWindow, mValidSizes[theIdx].mWidth, mValidSizes[theIdx].mHeight);
			int aCurrentX;
			int aCurrentY;
			SDL_GetWindowPosition(mApp->mWindow->mInternalWindow, &aCurrentX, &aCurrentY);
			int aNewX = aCurrentX - ((mValidSizes[theIdx].mWidth - mApp->mRenderer->mPresentationRect.mWidth) / 2);
			int aNewY = aCurrentY - ((mValidSizes[theIdx].mHeight - mApp->mRenderer->mPresentationRect.mHeight) / 2);
			SDL_Rect aUsableBounds{};
			SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(mApp->mWindow->mInternalWindow), &aUsableBounds);
			if (aNewX < 0)
				aNewX = aUsableBounds.x + 50;
			if (aNewY < 0)
				aNewY = aUsableBounds.y + 50;
			SDL_SetWindowPosition(mApp->mWindow->mInternalWindow, aNewX, aNewY);

			//Send events to the app to update the engine as a whole
			SDL_Event aSizeEvent = {SDL_EVENT_WINDOW_RESIZED};
			aSizeEvent.window.data1 = mValidSizes[theIdx].mWidth;
			aSizeEvent.window.data2 = mValidSizes[theIdx].mHeight;
			aSizeEvent.window.windowID = SDL_GetWindowID(mApp->mWindow->mInternalWindow);
			SDL_Event aPosEvent = {SDL_EVENT_WINDOW_MOVED};
			aPosEvent.window.data1 = aNewX;
			aPosEvent.window.data2 = aNewY;
			aPosEvent.window.windowID = SDL_GetWindowID(mApp->mWindow->mInternalWindow);
			SDL_PushEvent(&aPosEvent);
			SDL_PushEvent(&aSizeEvent);
		}
		mSizesList->SetSelect(theIdx);

	}
	else if (theId == SETTINGS_SHADER)
	{

		if (theIdx < static_cast<int>(piston::UpscaleMode::NumUpscalers))
		{
            mApp->SetUpscaleMode(static_cast<piston::UpscaleMode>(theIdx));
            mShaderList->SetSelect(theIdx);
		}

	}

}

bool SettingsDialog::IsCurrentWindowSizeInList()
{
	for (const auto &aSize : mValidSizes)
	{
		if (mApp->mRenderer->mPresentationRect.mWidth == aSize.mWidth &&
			mApp->mRenderer->mPresentationRect.mWidth == aSize.mHeight)
			return true;
	}

	return false;
}


void SettingsDialog::CheckboxChecked(int theId, bool checked)
{
	switch (theId)
	{
	case SettingsDialog::SETTINGS_VSYNC:
		{
			mApp->mWaitForVSync = checked;
			RendererError anError = mApp->mRenderer->UpdateVSync();
			if (anError == RendererError::ERROR_VSYNC)
			{
				mVSyncCheckbox->SetChecked(!checked, false);
				SexyString aFailString = StrFormat("V-Sync couldn't be toggled %s\n\nYour video card does not\nmeet the "
												   "minimum requirements\nfor this feature.",
												   (checked ? "on" : "off"));
				mApp->DoDialog(Dialogs::DIALOG_INFO, true, "Failed", aFailString, "OK", Dialog::BUTTONS_FOOTER);
			}
		}
		break;
	case SettingsDialog::SETTINGS_FULLSCREEN:
		if (!checked && mApp->mForceFullscreen)
		{
			mApp->DoDialog(Dialogs::DIALOG_COLORDEPTH_EXP, true, "No Windowed Mode",
						   "Windowed mode is only available if your desktop was running in either\n"
						   "16 bit or 32 bit color mode when you started the game.\n\n"
						   "If you'd like to run in Windowed mode then you need to quit the game and switch your "
						   "desktop to 16 or 32 bit color mode.",
						   "OK", Dialog::BUTTONS_FOOTER);

			mFullscreenCheckbox->SetChecked(true, false);
		}
		else
		{
			mApp->SwitchScreenMode(!mFullscreenCheckbox->IsChecked(), true, false);
		}
		break;
	
	case SettingsDialog::SETTINGS_HIGHQUALITY:
		mApp->mIs3D = mHighQualityCheckbox->IsChecked();
		break;
	}

}
