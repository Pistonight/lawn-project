#pragma once

#include <Lawn/Widget/GameButton.h>
#include <Lawn/Widget/LawnDialog.h>
#include <Piston/System.h>
#include <Piston/Upscaler.h>
#include <SexyAppFramework/CheckboxListener.h>
#include <SexyAppFramework/ListListener.h>
#include <memory>

class LawnApp;
class LawnScrollbar;
namespace Sexy {
class ListWidget;
};

class SettingsDialog : public LawnDialog, public Sexy::CheckboxListener, public Sexy::ListListener {
protected:
    enum class WindowSize {
        Small,
        Medium,
        Large,
        Custom,
        Fullscreen,
    };
    enum {
        SETTINGS_BACK,
        SETTINGS_VSYNC,
        SETTINGS_FULLSCREEN,
        SETTINGS_HIGHQUALITY,
        SETTINGS_CYCLE_FILTER_MODE,
        SETTINGS_CYCLE_WINDOW_SIZE,
        SETTINGS_CYCLE_LANGUAGE,
        SETTINGS_OPEN_SAVE_FOLDER,
    };

private:
    LawnApp* mApp;
    std::unique_ptr<LawnStoneButton> mApplyButton;
    std::unique_ptr<LawnStoneButton> mSaveFileButton;
    std::unique_ptr<Sexy::Checkbox> mVSyncCheckbox;
    std::unique_ptr<Sexy::Checkbox> mHighQualityCheckbox;
    std::unique_ptr<LawnStoneButton> mFilterModeButton;
    std::unique_ptr<LawnStoneButton> mWindowSizeButton;
    std::unique_ptr<LawnStoneButton> mLanguageButton;
    std::unique_ptr<LawnScrollbar> mOptionsSlider;
    bool mIsUpdatingWidgets{};

public:
    // LawnApp accesses it for some reason
    std::unique_ptr<Sexy::Checkbox> mFullscreenCheckbox;

public:
    SettingsDialog(LawnApp* theApp);
    virtual ~SettingsDialog() = default;
    virtual void Draw(Graphics* g);
    virtual void AddedToManager(WidgetManager* theWidgetManager);
    virtual void RemovedFromManager(WidgetManager* theWidgetManager);
    virtual void Resize(int theX, int theY, int theWidth, int theHeight);
    virtual void MouseWheel(int theDelta);
    virtual void ButtonPress(int theId);
    virtual void ButtonDepress(int theId);

private:
    void UpdateWidgets();
    void CycleFilterMode();
    WindowSize GetCurrentWindowSize();
    bool IsWindowSizeSupported(WindowSize size);
    void CycleWindowSize();

    void CheckboxChecked(int theId, bool checked);
    void DrawCheckbox(Graphics* g, Sexy::Checkbox& theCheckbox, const std::string& theTitle,
                      int theX, int theY, Font& theFont);
    void UpdateButtonPosition(LawnStoneButton& theButton, int theX, int theY);

    static const char* GetFilterModeText(Sexy::OutputFilteringMode appMode,
                                         Piston::UpscaleMode upscaleMode);
    static const char* GetWindowSizeText(WindowSize size);
    static std::pair<int, int> GetWindowSize(WindowSize size);
    static const char* GetLanguageText(Piston::Language language);
};
