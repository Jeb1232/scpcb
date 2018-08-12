#include <bbblitz3d.h>
#include <bbmath.h>
#include <bbgraphics.h>
#include <bbaudio.h>

#include "../GameMain.h"
#include "../MapSystem.h"
#include "../Doors.h"
#include "../Items/Items.h"
#include "../Decals.h"
#include "../Particles.h"
#include "../Events.h"
#include "../Player.h"
#include "../NPCs/NPCs.h"
#include "../Audio.h"
#include "../MathUtils/MathUtils.h"
#include "../Menus/Menu.h"
#include "../Objects.h"
#include "Room_off_glss_3.h"

namespace CBN {

// Functions.
void FillRoom_off_glss_3(Room* r) {
    Door* d;
    Door* d2;
    SecurityCam* sc;
    Decal* de;
    Room* r2;
    SecurityCam* sc2;
    Item* it;
    int i;
    int xtemp;
    int ytemp;
    int ztemp;

    //, Bump
    int t1;

    d = CreateDoor(r->x + 736.f * RoomScale, 0.f, r->z + 240.f * RoomScale, 0, r, false, DOOR_TYPE_DEF, r->roomTemplate->name);
    bbPositionEntity(d->buttons[0], r->x + 892.f * RoomScale, bbEntityY(d->buttons[0],true), r->z + 224.f * RoomScale, true);
    bbPositionEntity(d->buttons[1], r->x + 892.f * RoomScale, bbEntityY(d->buttons[1],true), r->z + 255.f * RoomScale, true);
    bbFreeEntity(d->obj2);
    d->obj2 = 0;

    //r\objects[0] = LoadMesh("GFX/Map/room3offices_hb.b3d",r\obj)
    //EntityPickMode(r\objects[0],2)
    //EntityType(r\objects[0],HIT_MAP)
    //EntityAlpha(r\objects[0],0.f)
}

}
