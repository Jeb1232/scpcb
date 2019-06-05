#include <bbblitz3d.h>
#include <bbmath.h>

#include "../MapSystem.h"
#include "../Doors.h"
#include "../Items/Item.h"
#include "../Decals.h"
#include "Room_srvr_pc_2.h"

namespace CBN {

// Functions.
void FillRoom_srvr_pc_2(Room* r) {
    Door* d = CreateDoor(r->x + 264.f * RoomScale, 0.f, r->z + 672.f * RoomScale, 270, r, false, DOOR_TYPE_DEF, r->roomTemplate->name);
    bbPositionEntity(d->buttons[0], r->x + 224.f * RoomScale, bbEntityY(d->buttons[0],true), r->z + 880.f * RoomScale, true);
    bbPositionEntity(d->buttons[1], r->x + 304.f * RoomScale, bbEntityY(d->buttons[1],true), r->z + 840.f * RoomScale, true);
    bbTurnEntity(d->buttons[1],0,0,0,true);
    d = CreateDoor(r->x -512.f * RoomScale, -768.f*RoomScale, r->z -336.f * RoomScale, 0, r, false, DOOR_TYPE_DEF, r->roomTemplate->name);
    d = CreateDoor(r->x -509.f * RoomScale, -768.f*RoomScale, r->z -1037.f * RoomScale, 0, r, false, DOOR_TYPE_DEF, r->roomTemplate->name);
    d->locked = true;

    Item* it = CreateItem("nvgoggles", r->x + 56.0154f * RoomScale, r->y - 648.f * RoomScale, r->z + 749.638f * RoomScale);
    it->state = 20;
    bbRotateEntity(it->collider, 0, r->angle+bbRand(245), 0);
    bbEntityParent(it->collider, r->obj);
}

}
