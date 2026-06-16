#pragma once

#include <Lawn/Board.h>
#include <Lawn/Challenge.h>
#include <Lawn/GridItem.h>
#include <Lawn/LawnMower.h>
#include <Sexy.TodLib/Reanimator.h>
#include <Sexy.TodLib/TodParticle.h>
#include <Sexy.TodLib/Trail.h>
#include <fstream>
#include <map>
#include <string>

namespace Sexy {
class Image;
}
using namespace Sexy;

struct SaveSchemeEntry {
    size_t mOffset;
    size_t mSize;
};

class SaveContext {
public:
    std::ofstream* mWriter;
    size_t mCurrentOffset;
    std::string mSchema;
    std::ifstream* mBinaryReader;
    std::map<std::string, SaveSchemeEntry> mSchemeEntries;
    bool mReading;
    bool mFailed;

    template <typename T> void SyncVar(T& aValue, const std::string& aName);
    void SyncBytes(void* aValue, size_t aSize, const std::string& aName);
    void SyncReanimationDef(ReanimatorDefinition*& theDefinition, const std::string& theOwner);
    void SyncParticleDef(TodParticleDefinition*& theDefinition, const std::string& theOwner);
    void SyncTrailDef(TrailDefinition*& theDefinition, const std::string& theOwner);
    void SyncImage(Image*& theImage, const std::string& theOwner);
    void LoadScheme(std::string thePath);
};

bool LawnLoadGame(Board* theBoard, const std::string& theFilePath);
bool LawnSaveGame(Board* theBoard, const std::string& theFilePath);
