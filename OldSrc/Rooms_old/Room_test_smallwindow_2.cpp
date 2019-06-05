#include <bbblitz3d.h>

#include "../GameMain.h"
#include "../MapSystem.h"
#include "../Doors.h"
#include "../Items/Item.h"
#include "../Decals.h"
#include "../Particles.h"
#include "../Events.h"
#include "../Player.h"
#include "../NPCs/NPCs.h"
#include "../Audio.h"
#include "../MathUtils/MathUtils.h"
#include "Room_test_smallwindow_2.h"

namespace CBN {

// Constants.
const int ROOM_TSW2_173SPAWN = 0;
const int ROOM_TSW2_173JUMP = 1;
const int ROOM_TSW2_GLASS = 2;
const int EVENT_TSW2_173SPAWNED = 0;
const int EVENT_TSW2_173JUMPTIMER = 0;

// Functions.
void FillRoom_test_smallwindow_2(Room* r) {
    r->objects[ROOM_TSW2_173SPAWN] = bbCreatePivot();
    bbPositionEntity(r->objects[0], r->x - 640.f * RoomScale, 0.5f, r->z - 912.f * RoomScale);
    bbEntityParent(r->objects[0], r->obj);

    r->objects[ROOM_TSW2_173JUMP] = bbCreatePivot();
    bbPositionEntity(r->objects[1], r->x - 632.f * RoomScale, 0.5f, r->z - 16.f * RoomScale);
    bbEntityParent(r->objects[1], r->obj);

    Texture* Glasstex = bbLoadTexture("GFX/Map/Textures/glass.png",1+2);
    r->objects[ROOM_TSW2_GLASS] = bbCreateSprite();
    bbEntityTexture((Sprite*)r->objects[ROOM_TSW2_GLASS],Glasstex);
    bbSpriteViewMode((Sprite*)r->objects[ROOM_TSW2_GLASS],2);
    bbScaleSprite((Sprite*)r->objects[ROOM_TSW2_GLASS],182.f*RoomScale*0.5f, 192.f*RoomScale*0.5f);
    bbPositionEntity(r->objects[ROOM_TSW2_GLASS], r->x - 595.f * RoomScale, 224.f*RoomScale, r->z - 208.f * RoomScale);
    bbTurnEntity(r->objects[ROOM_TSW2_GLASS],0,180,0);
    bbEntityParent(r->objects[ROOM_TSW2_GLASS], r->obj);
    bbHideEntity(r->objects[ROOM_TSW2_GLASS]);

    bbFreeTexture(Glasstex);

    r->doors[0] = CreateDoor(r->x - 240.f * RoomScale, 0.f, r->z + 640.f * RoomScale, 90, r, false, DOOR_TYPE_DEF, r->roomTemplate->name);
    r->doors[0]->autoClose = false;
    r->doors[0]->open = false;

    Door* d = CreateDoor(r->x - 512.f * RoomScale, 0.f, r->z + 384.f * RoomScale, 0, r);
    d->autoClose = false;
    d->open = false;

    d = CreateDoor(r->x - 816.f * RoomScale, 0.f, r->z - 208.f * RoomScale, 0, r);
    d->autoClose = false;
    d->open = false;
    bbFreeEntity(d->buttons[0]);
    d->buttons[0] = 0;
    bbFreeEntity(d->buttons[1]);
    d->buttons[1] = 0;

    Item* it = CreateItem("navigator", r->x - 312.f * RoomScale, r->y + 264.f * RoomScale, r->z + 176.f * RoomScale);
    bbEntityParent(it->collider, r->obj);
}

void UpdateEventTestroom173(Event* e) {
    if (mainPlayer->currRoom == e->room) {
        if (Curr173->idle == 0) {
            if (!e->intState[EVENT_TSW2_173SPAWNED]) {
                if (e->room->doors[0]->open) {
                    bbPositionEntity(Curr173->collider, bbEntityX(e->room->objects[0], true), 0.5f, bbEntityZ(e->room->objects[0], true));
                    bbResetEntity(Curr173->collider);
                    e->intState[EVENT_TSW2_173SPAWNED] = true;
                }
            } else {
                bbShowEntity(e->room->objects[ROOM_TSW2_GLASS]);
                //start a timer for 173 breaking through the window
                e->floatState[EVENT_TSW2_173JUMPTIMER] = e->floatState[EVENT_TSW2_173JUMPTIMER]+timing->tickDuration;
                float dist = bbEntityDistance(mainPlayer->collider, e->room->objects[1]);
                if (dist<1.f) {
                    //if close, increase the timer so that 173 is ready to attack
                    e->floatState[EVENT_TSW2_173JUMPTIMER] = Max(e->floatState[EVENT_TSW2_173JUMPTIMER], 70*12);
                } else if ((dist > 1.4f)) {
                    //if the player moves a bit further and blinks, 173 attacks
                    if (e->floatState[EVENT_TSW2_173JUMPTIMER] > 70*12 && mainPlayer->blinkTimer <= -10) {
                        if (bbEntityDistance(Curr173->collider, e->room->objects[ROOM_TSW2_173SPAWN]) > 5.f) {
                            //if 173 is far away from the room (perhaps because the player
                            //left and 173 moved to some other room?) -> disable the event
                            RemoveEvent(e);
                        } else {
                            PlayRangedSound(LoadTempSound("SFX/General/GlassBreak.ogg"), mainPlayer->cam, Curr173->obj);
                            bbFreeEntity(e->room->objects[ROOM_TSW2_GLASS]);
                            bbPositionEntity(Curr173->collider, bbEntityX(e->room->objects[ROOM_TSW2_173SPAWN], true), 0.5f, bbEntityZ(e->room->objects[1], true));
                            bbResetEntity(Curr173->collider);
                            RemoveEvent(e);
                        }
                    }
                }
            }
        }
    }

}

}
