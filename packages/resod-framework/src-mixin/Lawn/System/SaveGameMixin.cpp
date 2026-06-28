#include <Lawn/System/SaveGame.h>
#include <LawnApp/Resources.h>

void SaveContext::SyncImage(Image*& theImage, const std::string& theOwner) {
    if (mReading) {
        ResourceId aResID;
        SyncVar((int&)aResID, StrFormat("IMAGE_%s", theOwner.c_str()));
        if (aResID == Sexy::ResourceId::LENGTH) {
            theImage = nullptr;
        } else {
            theImage = Sexy::GetImageById(aResID);
        }
    } else {
        ResourceId aResID = Sexy::GetIdByImage(theImage);
        SyncVar((int&)aResID, StrFormat("IMAGE_%s", theOwner.c_str()));
    }
}
