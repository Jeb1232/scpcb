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
#include "Room_tnnl_nuke_2.h"

namespace CBN {

// Functions.
void FillRoom_tnnl_nuke_2(Room* r) {
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

    //"tuulikaapin" ovi
    d = CreateDoor(r->x + 576.f * RoomScale, 0.f, r->z - 152.f * RoomScale, 90, r, false, DOOR_TYPE_DEF, r->roomTemplate->name);
    d->autoClose = false;
    d->open = false;
    bbPositionEntity(d->buttons[0], r->x + 608.f * RoomScale, bbEntityY(d->buttons[0],true), r->z - 284.f * RoomScale,true);
    bbPositionEntity(d->buttons[1], r->x + 544.f * RoomScale, bbEntityY(d->buttons[1],true), r->z - 284.f * RoomScale,true);

    d = CreateDoor(r->x - 544.f * RoomScale, 1504.f*RoomScale, r->z + 738.f * RoomScale, 90, r, false, DOOR_TYPE_DEF, r->roomTemplate->name);
    d->autoClose = false;
    d->open = false;
    bbPositionEntity(d->buttons[0], bbEntityX(d->buttons[0],true), bbEntityY(d->buttons[0],true), r->z + 608.f * RoomScale,true);
    bbPositionEntity(d->buttons[1], bbEntityX(d->buttons[1],true), bbEntityY(d->buttons[1],true), r->z + 608.f * RoomScale,true);

    //yl�kerran hissin ovi
    r->doors[0] = CreateDoor(r->x + 1192.f * RoomScale, 0.f, r->z, 90, r, true);
    r->doors[0]->autoClose = false;
    r->doors[0]->open = true;
    //yl�kerran hissi
    r->objects[4] = bbCreatePivot();
    bbPositionEntity(r->objects[4], r->x + 1496.f * RoomScale, 240.f * RoomScale, r->z);
    bbEntityParent(r->objects[4], r->obj);
    //alakerran hissin ovi
    r->doors[1] = CreateDoor(r->x + 680.f * RoomScale, 1504.f * RoomScale, r->z, 90, r);
    r->doors[1]->autoClose = false;
    r->doors[1]->open = false;
    //alakerran hissi
    r->objects[5] = bbCreatePivot();
    bbPositionEntity(r->objects[5], r->x + 984.f * RoomScale, 1744.f * RoomScale, r->z);
    bbEntityParent(r->objects[5], r->obj);

    int n;
    for (n = 0; n <= 1; n++) {
        r->levers[n] = CreateLever();

        bbScaleEntity(r->levers[n]->obj, 0.04, 0.04, 0.04);
        bbScaleEntity(r->levers[n]->baseObj, 0.04, 0.04, 0.04);
        bbPositionEntity(r->levers[n]->obj, r->x - 975.f * RoomScale, r->y + 1712.f * RoomScale, r->z - (502.f-132.f*n) * RoomScale, true);
        bbPositionEntity(r->levers[n]->baseObj, r->x - 975.f * RoomScale, r->y + 1712.f * RoomScale, r->z - (502.f-132.f*n) * RoomScale, true);

        bbEntityParent(r->levers[n]->obj, r->obj);
        bbEntityParent(r->levers[n]->baseObj, r->obj);

        bbRotateEntity(r->levers[n]->baseObj, 0, -90-180, 0);
        bbRotateEntity(r->levers[n]->obj, 10, -90 - 180-180, 0);

        //EntityPickMode(r\levers[n]\obj, 2)
        bbEntityPickMode(r->levers[n]->obj, 1, false);
        bbEntityRadius(r->levers[n]->obj, 0.1);
        //makecollbox(r\levers[n]\obj)
    }

    it = CreatePaper("docWar", r->x - 768.f * RoomScale, r->y + 1684.f * RoomScale, r->z - 768.f * RoomScale);
    bbEntityParent(it->collider, r->obj);

    it = CreateItem("vest", r->x - 944.f * RoomScale, r->y + 1652.f * RoomScale, r->z - 656.f * RoomScale);
    bbEntityParent(it->collider, r->obj);
    bbRotateEntity(it->collider, 0, -90, 0);

    it = CreatePaper("drL5", r->x + 800.f * RoomScale, 88.f * RoomScale, r->z + 256.f * RoomScale);
    bbEntityParent(it->collider, r->obj);

    sc = CreateSecurityCam(r->x+624.f*RoomScale, r->y+1888.f*RoomScale, r->z-312.f*RoomScale, r);
    sc->angle = 90;
    sc->turn = 45;
    bbTurnEntity(sc->cameraObj, 20, 0, 0);
    sc->id = 6;
}

void UpdateEvent_tnnl_nuke_2(Event* e) {
    float dist;
    int i;
    int temp;
    int pvt;
    String strtemp;
    int j;
    int k;

    Particle* p;
    NPC* n;
    Room* r;
    Event* e2;
    Item* it;
    Emitter* em;
    SecurityCam* sc;
    SecurityCam* sc2;

    String CurrTrigger = "";

    float x;
    float y;
    float z;

    float angle;

    //[Block]
    if (mainPlayer->currRoom == e->room) {
        e->eventState2 = UpdateElevators(e->eventState2, e->room->doors[0], e->room->doors[1], e->room->objects[4], e->room->objects[5], e);

        e->eventState = e->room->levers[0]->succ;
    }
    //[End Block]
}

}
