#include <Lawn/LawnCommon.h>
#include <Lawn/ResoddedFramework/SettingsDialog.h>
#include <LawnApp/LawnApp.h>
#include <LawnApp/Resources.h>
#include <Sexy.TodLib/TodStringFile.h>
#include <SexyAppFramework/BuildInfo.h>
#include <SexyAppFramework/Checkbox.h>
#include <SexyAppFramework/Font.h>
#include <SexyAppFramework/ListWidget.h>
#include <SexyAppFramework/Renderer.h>
#include <SexyAppFramework/Window.h>

#include <Piston/AppMixin.h>
#include <Piston/Font.h>
#include <Piston/System.h>

static const int gUserListWidgetColors[][3] = {
    {23, 24, 35}, {0, 0, 0}, {235, 225, 180}, {255, 255, 255}, {20, 180, 15}};

SettingsDialog::SettingsDialog(LawnApp* theApp)
    : LawnDialog(theApp, DIALOG_SETTINGS, true, "[MOD_SETTINGS_HEADER]", "", "", BUTTONS_NONE) {
    mApp = theApp;

    mOptionsSlider = std::make_unique<LawnScrollbar>(mApp);
    mOptionsSlider->mSliderHeightPercent = 0.57f;
    mOptionsSlider->mMaxValue = 600;
    mOptionsSlider->Resize(500, 90, 8, 140);

    mFullscreenCheckbox.reset(
        MakeNewCheckbox(SettingsDialog::SETTINGS_FULLSCREEN, this, !theApp->mIsWindowed));
    mVSyncCheckbox.reset(
        MakeNewCheckbox(SettingsDialog::SETTINGS_VSYNC, this, theApp->mWaitForVSync));
    mHighQualityCheckbox.reset(
        MakeNewCheckbox(SettingsDialog::SETTINGS_HIGHQUALITY, this, theApp->mIs3D));

    auto appFilterMode = mApp->mScreenFiltering;
    auto upscaleMode = mApp->mPistonMixin.mUpscaleMode;
    mFilterModeButton.reset(MakeButton(SETTINGS_CYCLE_FILTER_MODE, this,
                                       GetFilterModeText(appFilterMode, upscaleMode)));
    mWindowSizeButton.reset(
        MakeButton(SETTINGS_CYCLE_WINDOW_SIZE, this, GetWindowSizeText(GetCurrentWindowSize())));
    mLanguageButton.reset(MakeButton(SETTINGS_CYCLE_LANGUAGE, this,
                                     GetLanguageText(Piston::System::Instance().GetLanguage())));

    mSaveFileButton.reset(
        MakeButton(SETTINGS_OPEN_SAVE_FOLDER, this, "[MOD_SETTINGS_OPEN_SAVE_FOLDER]"));
    mApplyButton.reset(MakeButton(SETTINGS_BACK, this, "[MOD_SETTINGS_BACK]"));

    CalcSize(211, 214);
}

void SettingsDialog::Draw(Graphics* g) {
    LawnDialog::Draw(g);

    int aMaxContentHeight = 880;
    float aMaxScroll =
        std::max(0.0f, (float)aMaxContentHeight - mOptionsSlider->mAllowedMouseZone.mHeight);
    mOptionsSlider->mMaxValue = aMaxScroll;
    float aScrollOffset = mOptionsSlider->GetValue();

    g->PushState();
    g->Translate(mSaveFileButton->mX, mSaveFileButton->mY);
    mSaveFileButton->Draw(g);
    g->PopState();

    g->PushState();
    g->Translate(mApplyButton->mX, mApplyButton->mY);
    mApplyButton->Draw(g);
    g->PopState();

    g->PushState();
    g->SetClipRect(
        Rect(mOptionsSlider->mAllowedMouseZone.mX - mX, mOptionsSlider->mAllowedMouseZone.mY - mY,
             mOptionsSlider->mAllowedMouseZone.mWidth, mOptionsSlider->mAllowedMouseZone.mHeight));
    g->Translate(35, 120 - aScrollOffset);

    Sexy::Font* aFont = Piston::GetSettingsDialogFont();

    int checkboxX = 40;
    constexpr int checkboxGap = 38;
    int checkboxY = 0;
    DrawCheckbox(g, *mFullscreenCheckbox, "[MOD_SETTINGS_FULLSCREEN]", checkboxX, checkboxY,
                 *aFont);
    checkboxY += checkboxGap;
    DrawCheckbox(g, *mVSyncCheckbox, "[MOD_SETTINGS_VSYNC]", checkboxX, checkboxY, *aFont);
    checkboxY += checkboxGap;
    DrawCheckbox(g, *mHighQualityCheckbox, "[MOD_SETTINGS_HIGHQUALITY]", checkboxX, checkboxY,
                 *aFont);

    int buttonX = 220;
    constexpr int buttonGap = 50;
    int buttonY = 0;
    UpdateButtonPosition(*mFilterModeButton, buttonX, buttonY);
    buttonY += buttonGap;
    UpdateButtonPosition(*mWindowSizeButton, buttonX, buttonY);
    buttonY += buttonGap;
    UpdateButtonPosition(*mLanguageButton, buttonX, buttonY);

    int aY = 0;

    aY += 20;

    aY += 50;

    aY += 85;
    //
    //     TodDrawString(g, "[SETTINGS_RENDERER_BACKEND]", 20, aY, aFont, Color::White,
    //                   DrawStringJustification::DS_ALIGN_LEFT);
    //
    //     aY += 12;
    //
    //     mRendererList->Resize(40, aY - aScrollOffset + GetTop(), 130, 70);
    //
    //     mRendererList->mDisabled =
    //         (mRendererList->mY + mY + mRendererList->mHeight) <
    //         mOptionsSlider->mAllowedMouseZone.mY || (mRendererList->mY + mY) >
    //             (mOptionsSlider->mAllowedMouseZone.mY +
    //             mOptionsSlider->mAllowedMouseZone.mHeight);
    //
    //     aY += mRendererList->mHeight + 30;
    //
    //     if (mApp->mRenderer->mCurrentBackend != mApp->mDesiredBackend) {
    //         TodDrawString(g, "[SETTINGS_RENDERER_RESTART_NOTIF]", 20, aY, aFont,
    //                       Color::White, DrawStringJustification::DS_ALIGN_LEFT);
    //
    //         aY += 20;
    //     } else
    //         aY += 12;
    //
    //     TodDrawString(g, "[SETTINGS_WINDOW_SIZE]", 20, aY, aFont, Color::White,
    //                   DrawStringJustification::DS_ALIGN_LEFT);
    //
    //     aY += 4;
    //
    //     mSizesList->Resize(40, aY - aScrollOffset + GetTop(), 130, 26 * (mValidSizes.size() +
    //     1));
    //
    //     mSizesList->mDisabled =
    //         (mSizesList->mY + mY + mSizesList->mHeight) < mOptionsSlider->mAllowedMouseZone.mY ||
    //         (mSizesList->mY + mY) >
    //             (mOptionsSlider->mAllowedMouseZone.mY +
    //             mOptionsSlider->mAllowedMouseZone.mHeight);
    //
    //     aY += mSizesList->mHeight + 40;
    //
    //     TodDrawString(g, "[SETTINGS_FILTERING_MODE]", 20, aY, aFont, Color::White,
    //                   DrawStringJustification::DS_ALIGN_LEFT);
    //
    //     aY += 4;
    //     mFilterList->Resize(40, aY - aScrollOffset + GetTop(), 130,
    //                         26 * (mFilterList->mLines.size() + 1));
    //
    //     mFilterList->mDisabled =
    //         (mFilterList->mY + mY + mFilterList->mHeight) < mOptionsSlider->mAllowedMouseZone.mY
    //         || (mFilterList->mY + mY) >
    //             (mOptionsSlider->mAllowedMouseZone.mY +
    //             mOptionsSlider->mAllowedMouseZone.mHeight);
    //
    //     aY += mFilterList->mHeight + 40;
    //
    //     TodDrawString(g, "[SETTINGS_MISC]", 20, aY, aFont, Color::White,
    //                   DrawStringJustification::DS_ALIGN_LEFT);
    //
    //     aY += 20;
    //
    //     mSaveFileButton->Resize(40, aY - aScrollOffset + GetTop(), 270, 46);
    //
    //     mSaveFileButton->mDisabled =
    //         (mSaveFileButton->mY + mY + mSaveFileButton->mHeight) <
    //             mOptionsSlider->mAllowedMouseZone.mY ||
    //         (mSaveFileButton->mY + mY) >
    //             (mOptionsSlider->mAllowedMouseZone.mY +
    //             mOptionsSlider->mAllowedMouseZone.mHeight);
    //
    //     SexyString aVersionString = "ResoddedFramework Release " +
    //     LawnApp::gResoddedVersion.toString();
    //
    //     TodDrawString(g, aVersionString,
    //                   mOptionsSlider->mAllowedMouseZone.mWidth -
    //                       aFont->StringWidth(aVersionString) - 27,
    //                   aMaxContentHeight - aFont->GetHeight(), aFont,
    //                   Color::White, DrawStringJustification::DS_ALIGN_LEFT);
    //
    // #if GIT_AVAILABLE
    //
    //     SexyString aHash = GIT_HASH;
    //
    //     SexyString aGitString = "Git: Hash (" + aHash + ")" + (GIT_IS_DIRTY ? " WORK IN PROGRESS"
    //     : "");
    //
    //     TodDrawString(g, aGitString,
    //                   mOptionsSlider->mAllowedMouseZone.mWidth -
    //                       aFont->StringWidth(aGitString) - 27,
    //                   aMaxContentHeight, aFont, Color::White,
    //                   DrawStringJustification::DS_ALIGN_LEFT);
    //
    // #endif

    g->PopState();
}

void SettingsDialog::AddedToManager(WidgetManager* theWidgetManager) {
    LawnDialog::AddedToManager(theWidgetManager);
    AddWidget(mOptionsSlider.get());
    AddWidget(mApplyButton.get());
    AddWidget(mVSyncCheckbox.get());
    AddWidget(mFullscreenCheckbox.get());
    AddWidget(mSaveFileButton.get());
    AddWidget(mHighQualityCheckbox.get());
    AddWidget(mFilterModeButton.get());
    AddWidget(mWindowSizeButton.get());
    AddWidget(mLanguageButton.get());
}

void SettingsDialog::RemovedFromManager(WidgetManager* theWidgetManager) {
    LawnDialog::RemovedFromManager(theWidgetManager);
    RemoveWidget(mOptionsSlider.get());
    RemoveWidget(mApplyButton.get());
    RemoveWidget(mVSyncCheckbox.get());
    RemoveWidget(mFullscreenCheckbox.get());
    RemoveWidget(mSaveFileButton.get());
    RemoveWidget(mHighQualityCheckbox.get());
    RemoveWidget(mFilterModeButton.get());
    RemoveWidget(mWindowSizeButton.get());
    RemoveWidget(mLanguageButton.get());
}

void SettingsDialog::Resize(int theX, int theY, int theWidth, int theHeight) {
    LawnDialog::Resize(theX, theY, theWidth, theHeight);
    mOptionsSlider->Resize(mWidth - 60, 110, 8, 200);
    mOptionsSlider->mAllowedMouseZone = Rect(mX + 35, mY + 120, mWidth - 70, mHeight - 240);
    mApplyButton->Resize(350, 331, 209, 46);
    mSaveFileButton->Resize(35, 331, 309, 46);
    SetWidgetClipping(Rect(35, 120, mWidth - 70, mHeight - 240));
}

void SettingsDialog::MouseWheel(int theDelta) {
    LawnDialog::MouseWheel(theDelta);
    mOptionsSlider->MouseWheel(theDelta);
}

void SettingsDialog::ButtonPress(int theId) {
    mApp->PlaySample(SOUND_GRAVEBUTTON);
}

void SettingsDialog::ButtonDepress(int theId) {
    switch (theId) {
    case SettingsDialog::SETTINGS_BACK: {
        mApp->KillDialog(mId);
        break;
    }
    case SettingsDialog::SETTINGS_OPEN_SAVE_FOLDER: {
        SexyString aSaveFileFolder = GetAppDataFolder();
#ifdef _WIN32
        ShellExecuteA(NULL, "open", aSaveFileFolder.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#else
        SexyString aFailString =
            StrFormat("Couldn't open the folder on this platform.\nThe path is: \n%s",
                      aSaveFileFolder.c_str());

        mApp->DoDialog(Dialogs::DIALOG_INFO, true, "Failed", aFailString, "OK",
                       Dialog::BUTTONS_FOOTER);
#endif
        break;
    }
    case SettingsDialog::SETTINGS_CYCLE_FILTER_MODE: {
        CycleFilterMode();
        UpdateWidgets();
        break;
    }
    case SettingsDialog::SETTINGS_CYCLE_WINDOW_SIZE: {
        CycleWindowSize();
        UpdateWidgets();
        break;
    }
    case SettingsDialog::SETTINGS_CYCLE_LANGUAGE: {
        auto& system = Piston::System::Instance();
        if (system.GetNextLanguage() == Piston::Language::ZH) {
            system.SetLanguageForNextTime(Piston::Language::EN);
        } else {
            system.SetLanguageForNextTime(Piston::Language::ZH);
        }
        if (system.GetNextLanguage() != system.GetLanguage()) {
            mApp->DoDialog(Dialogs::DIALOG_INFO, true, "[MOD_SETTINGS_NOTIF_TITLE]",
                           "[MOD_SETTINGS_RESTART_NOTIF]", "OK", Dialog::BUTTONS_FOOTER);
        }
        UpdateWidgets();
        break;
    }
    }
}
void SettingsDialog::CheckboxChecked(int theId, bool checked) {
    if (mIsUpdatingWidgets) {
        return; // prevent recursive updates
    }
    switch (theId) {
    case SettingsDialog::SETTINGS_VSYNC: {
        mApp->mWaitForVSync = checked;
        RendererError anError = mApp->mRenderer->UpdateVSync();
        if (anError == RendererError::ERROR_VSYNC) {
            mVSyncCheckbox->SetChecked(!checked, false);
            SexyString aFailString =
                StrFormat("V-Sync couldn't be toggled %s\n\nYour video card does not\nmeet the "
                          "minimum requirements\nfor this feature.",
                          (checked ? "on" : "off"));
            mApp->DoDialog(Dialogs::DIALOG_INFO, true, "Failed", aFailString, "OK",
                           Dialog::BUTTONS_FOOTER);
        }
        break;
    }
    case SettingsDialog::SETTINGS_FULLSCREEN: {
        if (!checked && mApp->mForceFullscreen) {
            mApp->DoDialog(Dialogs::DIALOG_COLORDEPTH_EXP, true, "No Windowed Mode",
                           "Windowed mode is only available if your desktop was running in either\n"
                           "16 bit or 32 bit color mode when you started the game.\n\n"
                           "If you'd like to run in Windowed mode then you need to quit the game "
                           "and switch your "
                           "desktop to 16 or 32 bit color mode.",
                           "OK", Dialog::BUTTONS_FOOTER);

            mFullscreenCheckbox->SetChecked(true, false);
        } else {
            mApp->SwitchScreenMode(!mFullscreenCheckbox->IsChecked(), mApp->mIs3D, false);
        }
        UpdateWidgets();
        break;
    }

    case SettingsDialog::SETTINGS_HIGHQUALITY: {
        mApp->mIs3D = mHighQualityCheckbox->IsChecked();
        UpdateWidgets();
        break;
    }
    }
}

// void SettingsDialog::ListClicked(int theId, int theIdx, int theClickCount) {
// if (theId == SETTINGS_RENDER_LIST) {
//     if (theIdx != mApp->mDesiredBackend - 1) {
//         mRendererList->SetSelect(theIdx);
//         mApp->mDesiredBackend = (RenderingBackend)(theIdx + 1);
//         SexyString aBackendName = "";
//         for (int i = RenderingBackend::BACKEND_NONE + 1; i < RenderingBackend::NUM_BACKENDS;
//              i++) {
//             for (auto backend : gRenderBackends) {
//                 if (backend.first == mApp->mDesiredBackend)
//                     aBackendName = backend.second;
//             }
//         }
//         mApp->RegistryWriteInteger("DesiredBackend", mApp->mDesiredBackend);
//         mApp->WriteToRegistry();
//
//         if (mApp->mDesiredBackend != mApp->mRenderer->mCurrentBackend) {
//             SexyString anInfoString = StrFormat("Rendering Backend has been changed to "
//                                                 "%s\nRestart the game to apply the changes",
//                                                 aBackendName.c_str());
//             mApp->DoDialog(Dialogs::DIALOG_INFO, true, "", anInfoString, "OK",
//                            Dialog::BUTTONS_FOOTER);
//         }
//     }
// } else if (theId == SETTINGS_FILTER_LIST) {
//     if (theIdx != mApp->mScreenFiltering) {
//         mFilterList->SetSelect(theIdx);
//         mApp->mScreenFiltering = (OutputFilteringMode)(theIdx);
//     }
// } else if (theId == SETTINGS_WINDOW_SIZES) {
//
//     if (theIdx < mValidSizes.size()) {
//     }
//     mSizesList->SetSelect(theIdx);
// }
// }
//
void SettingsDialog::UpdateWidgets() {
    mIsUpdatingWidgets = true;
    auto windowSize = GetCurrentWindowSize();
    bool isFullScreen = windowSize == WindowSize::Fullscreen;
    mFullscreenCheckbox->SetChecked(isFullScreen);
    // mVSyncCheckbox - can't check as app doesn't store the state
    mHighQualityCheckbox->SetChecked(mApp->mIs3D);
    auto appFilterMode = mApp->mScreenFiltering;
    auto upscaleMode = mApp->mPistonMixin.mUpscaleMode;
    mFilterModeButton->SetLabel(GetFilterModeText(appFilterMode, upscaleMode));
    mWindowSizeButton->mDisabled = isFullScreen;
    // we don't update the size label here because setting the window size is async,
    // so it must be set when cycling the size
    mLanguageButton->SetLabel(GetLanguageText(Piston::System::Instance().GetNextLanguage()));
    mIsUpdatingWidgets = false;
}

void SettingsDialog::CycleFilterMode() {
    switch (mApp->mPistonMixin.mUpscaleMode) {
    case Piston::UpscaleMode::None: {
        switch (mApp->mScreenFiltering) {
        case Sexy::OutputFilteringMode::MODE_NEAREST: {
            mApp->mScreenFiltering = OutputFilteringMode::MODE_LINEAR;
            mApp->mPistonMixin.mUpscaleMode = Piston::UpscaleMode::None;
            break;
        }
        default: {
            mApp->mScreenFiltering = OutputFilteringMode::MODE_NEAREST;
            mApp->mPistonMixin.mUpscaleMode = Piston::UpscaleMode::Fsr;
            break;
        }
        }
        break;
    }
    default: {
        mApp->mScreenFiltering = OutputFilteringMode::MODE_NEAREST;
        mApp->mPistonMixin.mUpscaleMode = Piston::UpscaleMode::None;
        break;
    }
    }
}

SettingsDialog::WindowSize SettingsDialog::GetCurrentWindowSize() {
    if (!mApp->mIsWindowed || mApp->mForceFullscreen) {
        return WindowSize::Fullscreen;
    }
    const auto& screen = mApp->mRenderer->mPresentationRect;
    auto largeSize = GetWindowSize(WindowSize::Large);
    if (screen.mWidth == largeSize.first && screen.mHeight == largeSize.second) {
        if (IsWindowSizeSupported(WindowSize::Large)) {
            return WindowSize::Large;
        } else if (IsWindowSizeSupported(WindowSize::Medium)) {
            return WindowSize::Medium;
        } else {
            return WindowSize::Small;
        }
    }
    auto mediumSize = GetWindowSize(WindowSize::Medium);
    if (screen.mWidth == mediumSize.first && screen.mHeight == mediumSize.second) {
        if (IsWindowSizeSupported(WindowSize::Medium)) {
            return WindowSize::Medium;
        } else {
            return WindowSize::Small;
        }
    }
    auto size = GetWindowSize(WindowSize::Small);
    if (screen.mWidth == size.first && screen.mHeight == size.second) {
        return WindowSize::Small;
    }
    return WindowSize::Custom;
}

bool SettingsDialog::IsWindowSizeSupported(WindowSize size) {
    if (size == WindowSize::Custom) {
        return true;
    }
    auto [width, height] = GetWindowSize(size);
    SDL_Rect aUsableBounds{};
    SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(mApp->mWindow->mInternalWindow),
                               &aUsableBounds);
    int aPadding = 64;
    return width <= aUsableBounds.w - aPadding && height <= aUsableBounds.h - aPadding;
}

void SettingsDialog::CycleWindowSize() {
    auto currentSize = GetCurrentWindowSize();
    if (currentSize == WindowSize::Fullscreen) {
        return;
    }
    auto nextWindowSize = WindowSize::Small;
    switch (currentSize) {
    case WindowSize::Small: {
        if (IsWindowSizeSupported(WindowSize::Medium)) {
            nextWindowSize = WindowSize::Medium;
        } else {
            return;
        }
        break;
    }
    case WindowSize::Medium: {
        if (IsWindowSizeSupported(WindowSize::Large)) {
            nextWindowSize = WindowSize::Large;
        } else {
            nextWindowSize = WindowSize::Small;
        }
        break;
    }
    default: {
        nextWindowSize = WindowSize::Small;
    }
    }
    mWindowSizeButton->SetLabel(GetWindowSizeText(nextWindowSize));
    auto [nextWidth, nextHeight] = GetWindowSize(nextWindowSize);
    if (nextWidth == 0 || nextHeight == 0) {
        return;
    }
    auto* window = mApp->mWindow->mInternalWindow;
    SDL_RestoreWindow(window);
    SDL_SetWindowSize(window, nextWidth, nextHeight);
    int aCurrentX = 0;
    int aCurrentY = 0;
    SDL_GetWindowPosition(window, &aCurrentX, &aCurrentY);
    int aNewX = aCurrentX - ((nextWidth - mApp->mRenderer->mPresentationRect.mWidth) / 2);
    int aNewY = aCurrentY - ((nextHeight - mApp->mRenderer->mPresentationRect.mHeight) / 2);
    SDL_Rect aUsableBounds{};
    SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(window), &aUsableBounds);
    if (aNewX <= aUsableBounds.x + 32) {
        aNewX = aUsableBounds.x + 32;
    }
    if (aNewX + nextWidth > aUsableBounds.w - 32) {
        aNewX = aUsableBounds.w - 32 - nextWidth;
    }
    if (aNewY <= aUsableBounds.y + 32) {
        aNewY = aUsableBounds.y + 32;
    }
    if (aNewY + nextHeight > aUsableBounds.h - 32) {
        aNewY = aUsableBounds.h - 32 - nextHeight;
    }
    SDL_SetWindowPosition(window, aNewX, aNewY);

    // Send events to the app to update the engine as a whole
    SDL_Event aSizeEvent = {SDL_EVENT_WINDOW_RESIZED};
    aSizeEvent.window.data1 = nextWidth;
    aSizeEvent.window.data2 = nextHeight;
    aSizeEvent.window.windowID = SDL_GetWindowID(mApp->mWindow->mInternalWindow);
    SDL_Event aPosEvent = {SDL_EVENT_WINDOW_MOVED};
    aPosEvent.window.data1 = aNewX;
    aPosEvent.window.data2 = aNewY;
    aPosEvent.window.windowID = SDL_GetWindowID(mApp->mWindow->mInternalWindow);
    SDL_PushEvent(&aPosEvent);
    SDL_PushEvent(&aSizeEvent);
}

void SettingsDialog::DrawCheckbox(Graphics* g, Sexy::Checkbox& theCheckbox,
                                  const std::string& theTitle, int theX, int theY, Font& theFont) {
    float aScrollOffset = mOptionsSlider->GetValue();
    theCheckbox.Resize(theX, theY - aScrollOffset + GetTop(), 46, 45);

    theCheckbox.mDisabled =
        (theCheckbox.mY + mY + theCheckbox.mHeight) < mOptionsSlider->mAllowedMouseZone.mY ||
        (theCheckbox.mY + mY) >
            (mOptionsSlider->mAllowedMouseZone.mY + mOptionsSlider->mAllowedMouseZone.mHeight);

    Sexy::Color aTextColor(107, 109, 145);
    TodDrawString(g, theTitle, theCheckbox.mX + 10, theY + 35, &theFont, aTextColor,
                  DrawStringJustification::DS_ALIGN_LEFT);
}

void SettingsDialog::UpdateButtonPosition(LawnStoneButton& theButton, int theX, int theY) {
    float aScrollOffset = mOptionsSlider->GetValue();
    theButton.Resize(theX, theY - aScrollOffset + GetTop(), 275, 46);
    theButton.mDisabled =
        (theButton.mY + mY + theButton.mHeight) < mOptionsSlider->mAllowedMouseZone.mY ||
        (theButton.mY + mY) >
            (mOptionsSlider->mAllowedMouseZone.mY + mOptionsSlider->mAllowedMouseZone.mHeight);
}

const char* SettingsDialog::GetFilterModeText(Sexy::OutputFilteringMode appMode,
                                              Piston::UpscaleMode upscaleMode) {
    switch (upscaleMode) {
    case Piston::UpscaleMode::Fsr: {
        return "[MOD_SETTINGS_FILTER_MODE_FSR]";
    }
    default: {
        break;
    }
    }
    switch (appMode) {
    case OutputFilteringMode::MODE_LINEAR:
        return "[MOD_SETTINGS_FILTER_MODE_LINEAR]";
    default:
        return "[MOD_SETTINGS_FILTER_MODE_NEAREST]";
    }
}

const char* SettingsDialog::GetWindowSizeText(SettingsDialog::WindowSize size) {
    switch (size) {
    case WindowSize::Small:
        return "[MOD_SETTINGS_WINDOW_SIZE_SMALL]";
    case WindowSize::Medium:
        return "[MOD_SETTINGS_WINDOW_SIZE_MEDIUM]";
    case WindowSize::Large:
        return "[MOD_SETTINGS_WINDOW_SIZE_LARGE]";
    default:
        return "[MOD_SETTINGS_WINDOW_SIZE_CUSTOM]";
    }
}

std::pair<int, int> SettingsDialog::GetWindowSize(SettingsDialog::WindowSize size) {
    switch (size) {
    case WindowSize::Small:
        return {800, 600};
    case WindowSize::Medium:
        return {1200, 900};
    case WindowSize::Large:
        return {1600, 1200};
    default:
        return {0, 0};
    }
}

const char* SettingsDialog::GetLanguageText(Piston::Language language) {
    switch (language) {
    case Piston::Language::ZH:
        return "[MOD_SETTINGS_LANGUAGE_CHINESE]";
    default:
        return "[MOD_SETTINGS_LANGUAGE_ENGLISH]";
    }
}
