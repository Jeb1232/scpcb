#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include <vector>
#include <StringType.h>

class bbImage;
class MeshModel;
class Texture;
class Pivot;

namespace CBN {

class Inventory;

// TODO: MOVE SOMEWHERE RELEVANT
enum class Setting914 {
    Rough,
    Coarse,
    OneToOne,
    Fine,
    VeryFine
};

const int WORNITEM_SLOT_COUNT = 3;
enum class WornItemSlot {
    None = WORNITEM_SLOT_COUNT,
    Head = 0,
    Body = 1,
    Hand = 2
};

enum class ItemPickSound {
    Tiny = 0,
    Metallic = 1,
    Heafty = 2,
    Paper = 3
};

class Item {
private:
    static int itemDistanceTimer;
    static std::vector<Item*> list;

    static int ID;
    void addID();
    void setID(int id);

    bool needsInvImg;

protected:
    Item(const String& meshPath, float scale, ItemPickSound sound, WornItemSlot slot = WornItemSlot::None);
    void setCoords(float x, float y, float z);

    int id;
    bool markedForRemoval;
    bool visible;

    std::vector<String> tags;

    String meshPath;
    MeshModel* mesh;
    float scale;
    float dist;

public:
    ~Item();

    Pivot* collider;
    bbImage* invImg;
    float dropSpeed;

    void setVisibility(bool visible);

    ItemPickSound pickSound;
    WornItemSlot wornSlot;

    virtual String getType()=0;
    virtual String getInvName()=0;

    void generateInvImg();

    void assignTag(const String& tag);
    bool hasTag(const String& tag);
    void removeTag(const String& tag);

    virtual void onPick();
    virtual void onUse();
    virtual void on914Use(Setting914 setting);
    virtual void combineWith(Item* other);

    virtual void update();
    virtual void draw();
    // Functions to call when the item's selected.
    // (I.E. the first aid kit.)
    virtual void updateUse();
    virtual void drawUse();

    // TODO: tinyxml2
    virtual void saveXML();

    static void updateAll();
    static void drawAll();
};

const String ITEM_TAG_OMNI("omni"); // TODO: Move to keycards.

}
#endif // ITEM_H_INCLUDED
