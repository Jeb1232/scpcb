#include <bbblitz3d.h>
#include <bbmath.h>
#include <bbaudio.h>

#include "../GameMain.h"
#include "../MapSystem.h"
#include "../Doors.h"
#include "../Decals.h"
#include "../Particles.h"
#include "../Events.h"
#include "../Player.h"
#include "../NPCs/NPCs.h"
#include "../Audio.h"
#include "../MathUtils/MathUtils.h"
#include "../Objects.h"
#include "Room_srvr_096_2.h"

namespace CBN {

// Functions.
void FillRoom_srvr_096_2(Room* r) {
    Door* d = CreateDoor(r->x,0,r->z, 0, r, false, DOOR_TYPE_HCZ);
    d->locked = true;

    r->doors[0] = CreateDoor(r->x - 208.f * RoomScale, 0.f, r->z - 736.f * RoomScale, 90, r, true);
    r->doors[0]->autoClose = false;
    r->doors[1] = CreateDoor(r->x - 208.f * RoomScale, 0.f, r->z + 736.f * RoomScale, 90, r, true);
    r->doors[1]->autoClose = false;

    r->doors[2] = CreateDoor(r->x - 672.f * RoomScale, 0.f, r->z - 1024.f * RoomScale, 0, r, false, DOOR_TYPE_DEF, "", "GEAR");
    r->doors[2]->autoClose = false;
    bbFreeEntity(r->doors[2]->buttons[0]);
    r->doors[2]->buttons[0] = 0;
    bbFreeEntity(r->doors[2]->buttons[1]);
    r->doors[2]->buttons[1] = 0;

    for (int n = 0; n < 3; n++) {
        r->levers[n] = CreateLever();

        bbScaleEntity(r->levers[n]->obj, 0.03f, 0.03f, 0.03f);
        bbScaleEntity(r->levers[n]->baseObj, 0.03f, 0.03f, 0.03f);

        switch (n) {
            case 0: {
                bbPositionEntity(r->levers[n]->obj, r->x - 1260.f * RoomScale, r->y + 234.f * RoomScale, r->z + 750 * RoomScale, true);
                bbPositionEntity(r->levers[n]->baseObj, r->x - 1260.f * RoomScale, r->y + 234.f * RoomScale, r->z + 750 * RoomScale, true);
                //generator fuel pump
            } break;
            case 1: {
                bbPositionEntity(r->levers[n]->obj, r->x - 920.f * RoomScale, r->y + 164.f * RoomScale, r->z + 898 * RoomScale, true);
                bbPositionEntity(r->levers[n]->baseObj, r->x - 920.f * RoomScale, r->y + 164.f * RoomScale, r->z + 898 * RoomScale, true);
                //generator on/off
            } break;
            case 2: {
                bbPositionEntity(r->levers[n]->obj, r->x - 837.f * RoomScale, r->y + 152.f * RoomScale, r->z + 886 * RoomScale, true);
                bbPositionEntity(r->levers[n]->baseObj, r->x - 837.f * RoomScale, r->y + 152.f * RoomScale, r->z + 886 * RoomScale, true);
            } break;
        }

        bbEntityParent(r->levers[n]->obj, r->obj);
        bbEntityParent(r->levers[n]->baseObj, r->obj);

        //RotateEntity(r\objects[n * 2], 0, -90, 0)
        bbRotateEntity(r->levers[n]->obj, 81, -180, 0);

        //EntityPickMode(r\levers[n]\obj, 2)
        bbEntityPickMode(r->levers[n]->obj, 1, false);
        bbEntityRadius(r->levers[n]->obj, 0.1f);
        //makecollbox(r\levers[n]\obj)
    }

    bbRotateEntity(r->levers[1]->obj, -81, -180, 0);
    bbRotateEntity(r->levers[2]->obj, -81, -180, 0);

    //096 spawnpoint
    r->objects[6] = bbCreatePivot(r->obj);
    bbPositionEntity(r->objects[6], r->x - 848*RoomScale, 0.5f, r->z-576*RoomScale, true);
    //guard spawnpoint
    r->objects[7] = bbCreatePivot(r->obj);
    bbPositionEntity(r->objects[7], r->x - 1328.f * RoomScale, 0.5f, r->z + 528*RoomScale, true);
    //the point where the guard walks to
    r->objects[8] = bbCreatePivot(r->obj);
    bbPositionEntity(r->objects[8], r->x - 1376.f * RoomScale, 0.5f, r->z + 32*RoomScale, true);

    r->objects[9] = bbCreatePivot(r->obj);
    bbPositionEntity(r->objects[9], r->x - 848*RoomScale, 0.5f, r->z+576*RoomScale, true);
}

void UpdateEvent_srvr_096_2(Event* e) {
    if (e->eventState==0) {
        if (mainPlayer->currRoom == e->room) {
            //close the doors when the player enters the room
            UseDoor(e->room->doors[0],false);
            e->room->doors[0]->locked = true;
            UseDoor(e->room->doors[1],false);
            e->room->doors[1]->locked = true;

            if (Curr096==nullptr) {
                Curr096 = CreateNPC(NPCtype096, bbEntityX(e->room->objects[6],true),bbEntityY(e->room->objects[6],true)+0.1f,bbEntityZ(e->room->objects[6],true));
            } else {
                bbPositionEntity(Curr096->collider, bbEntityX(e->room->objects[6],true),bbEntityY(e->room->objects[6],true)+0.1f,bbEntityZ(e->room->objects[6],true),true);
            }

            bbRotateEntity(Curr096->collider, 0, e->room->angle, 0, true);
            bbResetEntity(Curr096->collider);
            Curr096->state = 2;
            Curr096->state2 = 70*10;

            LoadEventSound(e,"SFX/Character/Guard/096ServerRoom1.ogg");
            e->soundChannels[0] = bbPlaySound(e->sounds[0]);

            e->room->npc[0] = CreateNPC(NPCtypeGuard, bbEntityX(e->room->objects[7],true),bbEntityY(e->room->objects[7],true),bbEntityZ(e->room->objects[7],true));

            e->eventState = 1;
        }
    } else if ((e->eventState < 70*45)) {
        if (bbRand(200)<5 && mainPlayer->currRoom == e->room) {
            //TODO: Light 1-3
            //LightBlink = Rnd(1.f,2.f)
            //If (Rand(5)=1) Then PlayRangedSound(IntroSFX(Rand(10,12)), mainPlayer\cam, e\room\obj, 8.f, Rnd(0.1f,0.3f))
        }

        e->eventState = Min(e->eventState+timing->tickDuration,70*43);

        if (e->room->npc[0]!=nullptr) {
            if (e->eventState < 70*13) {

                if (e->eventState > 70*8) {
                    Curr096->state = Min(Max(2,Curr096->state),3);
                    Curr096->state2 = 70*10;
                } else {
                    Curr096->state = 5;
                    bbPointEntity(Curr096->collider, e->room->objects[9]);
                    bbRotateEntity(Curr096->collider, 0, bbEntityYaw(Curr096->collider,true),0,true);
                    if (bbEntityDistance(Curr096->collider, e->room->objects[8])<2.4f) {
                        Curr096->state2 = 0;
                    } else {
                        Curr096->state2 = 1001;
                    }
                }

                e->room->npc[0]->state = 8;
                bbSetAnimTime(e->room->npc[0]->obj, 115);
                bbPointEntity(e->room->npc[0]->collider, Curr096->collider);
                //walk to the doorway
            } else if ((e->eventState-timing->tickDuration <= 70*15)) {
                if (e->eventState > 70*15) {
                    e->room->npc[0]->state = 3;
                    e->room->npc[0]->pathStatus = FindPath(e->room->npc[0], bbEntityX(e->room->objects[8],true),0.5f,bbEntityZ(e->room->objects[8],true));
                    e->room->npc[0]->pathTimer = 300;
                }
            } else if ((e->eventState<70*20)) {
                if (e->room->npc[0]->pathStatus==0) {
                    e->room->doors[2]->open = false;

                    e->room->npc[0]->state = 7;
                    //SetAnimTime(e\room\npc[0]\obj, 115)
                    bbPointEntity(e->room->npc[0]->obj, Curr096->collider);
                    bbRotateEntity(e->room->npc[0]->collider, 0, CurveAngle(bbEntityYaw(e->room->npc[0]->obj),bbEntityYaw(e->room->npc[0]->collider),30),0);

                }
                //start walking away
            } else {

                //shoot at 096 when it starts attacking
                if (Curr096->state == 4) {
                    Curr096->lastSeen = 1;

                    e->room->npc[0]->state = 2;
                    bbPointEntity(e->room->npc[0]->obj, Curr096->collider);
                    bbRotateEntity(e->room->npc[0]->collider, 0, CurveAngle(bbEntityYaw(e->room->npc[0]->obj),bbEntityYaw(e->room->npc[0]->collider),30),0);
                    //If (mainPlayer\currRoom = e\room) Then LightBlink = (e\room\npc[0]\reload)+Rnd(0.5f,2.f)
                    Curr096->target = e->room->npc[0];
                } else {
                    if (e->eventState>70*22) {
                        Curr096->state = 4;
                    }
                    if (e->room->npc[0]->state==7) {
                        e->room->npc[0]->state = 3;
                        e->room->npc[0]->pathStatus = FindPath(e->room->npc[0], bbEntityX(e->room->obj,true),0.4f,bbEntityZ(e->room->obj,true));
                        e->room->npc[0]->pathTimer = 300;
                    }
                }
            }

            Curr096->target = e->room->npc[0];
            if (bbAnimTime(Curr096->obj)>25 && bbAnimTime(Curr096->obj)<150) {
                bbFreeSound(e->sounds[0]);
                e->sounds[0] = 0;
                e->sounds[0] = bbLoadSound("SFX/Character/Guard/096ServerRoom2.ogg");
                e->soundChannels[0] = bbPlaySound(e->sounds[0]);

                Curr096->currSpeed = 0;

                // TODO: probably shouldn't randomize this since some angles let the player see deloading.
                Decal* de = nullptr;
                for (int i = 0; i < 7; i++) {
                    if (e->room->angle == 0 || e->room->angle == 180) {
                        de = CreateDecal(bbRand(DECAL_BLOOD_SPREAD, DECAL_BLOOD_SPLATTER), e->room->x-bbRnd(197,199)*bbCos(e->room->angle)*RoomScale, 1.f, e->room->z+(140.f*(i-3))*RoomScale,0,e->room->angle+90,bbRnd(360));
                        de->size = bbRnd(0.8f,0.85f);
                        de->sizeChange = 0.001f;
                        de = CreateDecal(bbRand(DECAL_BLOOD_SPREAD, DECAL_BLOOD_SPLATTER), e->room->x-bbRnd(197,199)*bbCos(e->room->angle)*RoomScale, 1.f, e->room->z+(140.f*(i-3))*RoomScale,0,e->room->angle-90,bbRnd(360));
                        de->size = bbRnd(0.8f,0.85f);
                        de->sizeChange = 0.001f;
                    } else {
                        de = CreateDecal(bbRand(DECAL_BLOOD_SPREAD, DECAL_BLOOD_SPLATTER), e->room->x+(140.f*(i-3))*RoomScale, 1.f, e->room->z-bbRnd(197,199)*bbSin(e->room->angle)*RoomScale-bbRnd(0.001f,0.003f),0,e->room->angle+90,bbRnd(360));
                        de->size = bbRnd(0.8f,0.85f);
                        de->sizeChange = 0.001f;
                        de = CreateDecal(bbRand(DECAL_BLOOD_SPREAD, DECAL_BLOOD_SPLATTER), e->room->x+(140.f*(i-3))*RoomScale, 1.f, e->room->z-bbRnd(197,199)*bbSin(e->room->angle)*RoomScale-bbRnd(0.001f,0.003f),0,e->room->angle-90,bbRnd(360));
                        de->size = bbRnd(0.8f,0.85f);
                        de->sizeChange = 0.001f;
                    }
                    de = CreateDecal(bbRand(DECAL_BLOOD_SPREAD, DECAL_BLOOD_SPLATTER), bbEntityX(e->room->npc[0]->collider)+bbRnd(-2,2),bbRnd(0.001f,0.003f),bbEntityZ(e->room->npc[0]->collider)+bbRnd(-2,2),90,bbRnd(360),0);

                }
                de->size = bbRnd(0.5f,0.7f);
                bbScaleSprite(de->obj, de->size,de->size);

                Curr096->state = 5;

                RemoveNPC(e->room->npc[0]);
                e->room->npc[0] = nullptr;

            }
        } else {

            //open them again to let the player in
            if (e->eventState >= 70*40 && e->eventState-timing->tickDuration < 70*40) {
                e->room->doors[0]->locked = false;
                e->room->doors[1]->locked = false;
                UseDoor(e->room->doors[0],false);
                UseDoor(e->room->doors[1],false);
                bbFreeSound(e->sounds[0]);
                e->sounds[0] = 0;
                e->room->doors[0]->locked = true;
                e->room->doors[1]->locked = true;
            }

            if (mainPlayer->currRoom == e->room) {
                if (e->soundChannels[0]!=0) {
                    if (bbChannelPlaying(e->soundChannels[0])) {
                        //TODO: Light 1-3
                        //LightBlink = Rnd(0.5f,6.f)
                        //If (Rand(50)=1) Then PlayRangedSound(IntroSFX(Rand(10,12)), mainPlayer\cam, e\room\obj, 8.f, Rnd(0.1f,0.3f))
                    }
                }

                //lock the player inside
                if (e->room->angle == 0 || e->room->angle == 180) {
                    if (abs(bbEntityX(mainPlayer->collider)-bbEntityX(e->room->obj,true))> 1.3f) {
                        e->eventState = 70*50;
                        e->sounds[0] = 0;
                    }
                } else {
                    if (abs(bbEntityZ(mainPlayer->collider)-bbEntityZ(e->room->obj,true))> 1.3f) {
                        e->eventState = 70*50;
                        e->sounds[0] = 0;
                    }
                }
            }

        }

    } else if ((mainPlayer->currRoom == e->room)) {
        //power switch
        bool powerOn = e->room->levers[0]->succ;
        //fuel pump
        bool fuelOn = e->room->levers[1]->succ;
        //generator
        bool genOn = e->room->levers[2]->succ;

        //fuel pump on
        if (fuelOn) {
            e->eventState2 = Min(1.f, e->eventState2+timing->tickDuration/350);

            //generator on
            if (fuelOn) {
                if (e->sounds[1]==0) {
                    LoadEventSound(e,"SFX/General/GeneratorOn.ogg",1);
                }
                e->eventState3 = Min(1.f, e->eventState3+timing->tickDuration/450);
            } else {
                e->eventState3 = Min(0.f, e->eventState3-timing->tickDuration/450);
            }
        } else {
            e->eventState2 = Max(0, e->eventState2-timing->tickDuration/350);
            e->eventState3 = Max(0, e->eventState3-timing->tickDuration/450);
        }

        if (e->eventState2>0) {
            e->soundChannels[0] = LoopRangedSound(RoomAmbience[8], e->soundChannels[0], mainPlayer->cam, e->room->levers[1]->obj, 5.f, e->eventState2*0.8f);
        }
        if (e->eventState3>0) {
            e->soundChannels[1] = LoopRangedSound(e->sounds[1], e->soundChannels[1], mainPlayer->cam, e->room->levers[2]->obj, 6.f, e->eventState3);
        }

        if (!powerOn && fuelOn && genOn) {
            e->room->doors[0]->locked = false;
            e->room->doors[1]->locked = false;
        } else {
            //If (Rand(200)<5) Then LightBlink = Rnd(0.5f,1.f)

            if (e->room->doors[0]->open) {
                e->room->doors[0]->locked = false;
                UseDoor(e->room->doors[0],false);
            }
            if (e->room->doors[1]->open) {
                e->room->doors[1]->locked = false;
                UseDoor(e->room->doors[1],false);
            }
            e->room->doors[0]->locked = true;
            e->room->doors[1]->locked = true;
        }
    }
}

}
