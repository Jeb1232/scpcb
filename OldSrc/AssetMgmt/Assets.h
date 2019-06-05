#ifndef ASSETS_H_INCLUDED
#define ASSETS_H_INCLUDED
#include <StringType.h>
#include <vector>

class Texture;
class bbImage;
class MeshModel;
class Object;
class gxFont;

namespace CBN {

// Constants.
const float ASSET_DECAY_TIMER = 2.f * 70;
const int BLEND_ADD = 3;
const int GORE_PIC_COUNT = 6;
const int HAND_ICON_TOUCH = 0;
const int HAND_ICON_GRAB = 1;

// Structs.
struct UIAssets {
public:
	UIAssets();
	~UIAssets();

    //Misc. Interface
    bbImage* back;
	bbImage* scpText;
	bbImage* tileWhite;
	bbImage* tileBlack;
	bbImage* scp173;

	bbImage* pauseMenuBG;

	bbImage* cursorIMG;
	bbImage* arrow[4];

	gxFont* font[4];
	gxFont* consoleFont;

    //HUD
	bbImage* sprintIcon;
	bbImage* blinkIcon;
	bbImage* crouchIcon;
	bbImage* handIcon[2];
	bbImage* blinkBar;
	bbImage* staminaBar;
	bbImage* keypadHUD;
};
extern UIAssets* uiAssets;

class AssetWrap {
public:
    virtual void drop() =0;

    static void update();
protected:
    float decayTimer;

    int grabCount;

    String file;

    AssetWrap();
};

class TextureAssetWrap : public AssetWrap {
public:
    virtual void drop();

    Texture* getTexture();

    static TextureAssetWrap* grab(const String& filePath, int flag = 1);

    static void update();
private:
    static std::vector<TextureAssetWrap*> list;

    int flags;
    Texture* texture;

    TextureAssetWrap(const String& filePath, int flag = 1);
    ~TextureAssetWrap();
};

class ImageAssetWrap : public AssetWrap {
public:
    virtual void drop();

    bbImage* getImage();

    static ImageAssetWrap* grab(const String& filePath);

    static void update();
private:
    static std::vector<ImageAssetWrap*> list;

    bbImage* image;

    ImageAssetWrap(const String& filePath);
    ~ImageAssetWrap();
};

class MeshAssetWrap : public AssetWrap {
public:
    virtual void drop();

    MeshModel* getMesh();

    static MeshAssetWrap* grab(const String& filePath, bool isAnimated = false);

    static void update();
private:
    static std::vector<MeshAssetWrap*> list;

    bool animated;
    MeshModel* mesh;

    MeshAssetWrap(const String& filePath, bool isAnimated);
    ~MeshAssetWrap();
};

// Functions.
void LoadEntities();

void InitNewGame();

void InitLoadGame();

void NullGame();

String GetImagePath(const String& path);

}
#endif // ASSETS_H_INCLUDED
