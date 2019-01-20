#include <bbblitz3d.h>
#include <bbmath.h>
#include <bbaudio.h>

#include "../MapSystem.h"
#include "../Decals.h"
#include "../Particles.h"
#include "../Events.h"
#include "../Player.h"
#include "../NPCs/NPCs.h"
#include "../Audio.h"
#include "Room_hll_toilets_2.h"

namespace CBN {

// Functions.
void FillRoom_hll_toilets_2(Room* r) {
    r->objects[0] = bbCreatePivot();
    bbPositionEntity(r->objects[0], r->x + 1040.f * RoomScale, 192.f * RoomScale, r->z);
    bbEntityParent(r->objects[0], r->obj);

    r->objects[1] = bbCreatePivot();
    bbPositionEntity(r->objects[1], r->x + 1312.f*RoomScale, 0.5f, r->z+448.f*RoomScale);
    bbEntityParent(r->objects[1], r->obj);

    r->objects[2] = bbCreatePivot();
    bbPositionEntity(r->objects[2], r->x + 1248.f*RoomScale, 0.01f, r->z+384.f*RoomScale);
    bbEntityParent(r->objects[2], r->obj);
}

void UpdateEventToiletguard(Event* e) {
    if (e->eventState == 0) {
        if (e->room->dist < 8.f  && e->room->dist > 0) {
            e->eventState = 1;
        }
    } else if ((e->eventState == 1)) {
        e->room->npc[0] = CreateNPC(NPCtypeGuard, bbEntityX(e->room->objects[1],true), bbEntityY(e->room->objects[1],true)+0.5f, bbEntityZ(e->room->objects[1],true));
        bbPointEntity(e->room->npc[0]->collider, e->room->obj);
        bbRotateEntity(e->room->npc[0]->collider, 0, bbEntityYaw(e->room->npc[0]->collider)-20,0, true);

        SetNPCFrame(e->room->npc[0], 906);
        e->room->npc[0]->state = 8;

        e->eventState = 2;
    } else {
        if (e->sounds[0] == 0) {
            e->sounds[0] = bbLoadSound("SFX/Character/Guard/SuicideGuard1.ogg");
        }
        if (e->room->dist < 15.f && e->room->dist >= 4.f) {
            e->soundChannels[0] = LoopRangedSound(e->sounds[0], e->soundChannels[0], mainPlayer->cam, e->room->npc[0]->collider, 15.f);

        } else if ((e->room->dist<4.f && mainPlayer->loudness > 1.f)) {
            Decal* de = CreateDecal(DECAL_BLOOD_SPLATTER,  bbEntityX(e->room->objects[2],true), 0.01f, bbEntityZ(e->room->objects[2],true),90,bbRnd(360),0);
            de->size = 0.3f;
            bbScaleSprite(de->obj, de->size, de->size);

            de = CreateDecal(DECAL_BLOOD_POOL, bbEntityX(e->room->objects[2],true), 0.01f, bbEntityZ(e->room->objects[2],true),90,bbRnd(360),0);
            de->size = 0.1f;
            de->maxSize = 0.45f;
            de->sizeChange = 0.0002f;
            UpdateDecals();

            //FreeSound(e\sounds[0])
            //StopChannel(e\soundChannels[0])
            //LoadEventSound(e,"SFX/Character/Guard/SuicideGuard2.ogg",1)
            if (e->soundChannels[1] == 0) {
                bbStopChannel(e->soundChannels[0]);
                bbFreeSound(e->sounds[0]);
                e->room->npc[0]->sounds[0] = bbLoadSound("SFX/Character/Guard/SuicideGuard2.ogg");
                e->soundChannels[1] = PlayRangedSound(e->room->npc[0]->sounds[0], mainPlayer->cam, e->room->npc[0]->collider, 15.f);
            }
            UpdateRangedSoundOrigin(e->soundChannels[1],mainPlayer->cam,e->room->npc[0]->collider,15.f);
            if (!bbChannelPlaying(e->soundChannels[1])) {
                RemoveEvent(e);
            }
        }
    }


}

void UpdateEventButtghost(Event* e) {
    if (mainPlayer->currRoom == e->room) {
        if (bbEntityDistance(mainPlayer->collider, e->room->objects[0]) < 1.8f) {
            if (e->eventState == 0) {
                e->soundChannels[0] = PlayRangedSound(LoadTempSound("SFX/SCP/Joke/789J.ogg"), mainPlayer->cam,e->room->objects[0]);
                e->eventState = 1;
            } else {
                if (!bbChannelPlaying(e->soundChannels[0])) {
                    RemoveEvent(e);
                }
            }
        }
    }

}

}
