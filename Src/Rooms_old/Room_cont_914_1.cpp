#include <bbblitz3d.h>
#include <bbaudio.h>

#include "../GameMain.h"
#include "../MapSystem.h"
#include "../Doors.h"
#include "../Items/Item.h"
#include "../Decals.h"
#include "../Particles.h"
#include "../Events.h"
#include "../Options.h"
#include "../Player.h"
#include "../Audio.h"
#include "../MathUtils/MathUtils.h"
#include "../Menus/Menu.h"
#include "Room_cont_914_1.h"

namespace CBN {

// Functions.
void FillRoom_cont_914_1(Room* r) {
    r->doors[2] = CreateDoor(r->x, 0, r->z-368.f*RoomScale, 0, r, true, DOOR_TYPE_CONT, r->roomTemplate->name);
    bbPositionEntity(r->doors[2]->buttons[0], r->x - 496.f * RoomScale, 0.7f, r->z - 272.f * RoomScale, true);
    bbTurnEntity(r->doors[2]->buttons[0], 0, 90, 0);

    r->objects[0] = bbLoadMesh("GFX/Map/914key.b3d");
    r->objects[1] = bbLoadMesh("GFX/Map/914knob.b3d");

    for (int i = 0; i < 2; i++) {
        bbScaleEntity(r->objects[i], RoomScale, RoomScale, RoomScale);
        bbEntityPickMode(r->objects[i], 2);
    }

    bbPositionEntity(r->objects[0], r->x, r->y + 190.f * RoomScale, r->z + 374.f * RoomScale);
    bbPositionEntity(r->objects[1], r->x, r->y + 230.f * RoomScale, r->z + 374.f * RoomScale);
    bbEntityParent(r->objects[0], r->obj);
    bbEntityParent(r->objects[1], r->obj);

    Door* d = CreateDoor(r->x - 624.f * RoomScale, 0.f, r->z + 528.f * RoomScale, 180, r, true);
    bbFreeEntity(d->obj2);
    d->obj2 = 0;
    bbFreeEntity(d->buttons[0]);
    d->buttons[0] = 0;
    bbFreeEntity(d->buttons[1]);
    d->buttons[1] = 0;
    r->doors[0] = d;
    d->autoClose = false;

    d = CreateDoor(r->x + 816.f * RoomScale, 0.f, r->z + 528.f * RoomScale, 180, r, true);
    bbFreeEntity(d->obj2);
    d->obj2 = 0;
    bbFreeEntity(d->buttons[0]);
    d->buttons[0] = 0;
    bbFreeEntity(d->buttons[1]);
    d->buttons[1] = 0;
    r->doors[1] = d;
    d->autoClose = false;

    r->objects[2] = bbCreatePivot();
    r->objects[3] = bbCreatePivot();
    bbPositionEntity(r->objects[2], r->x - 712.f * RoomScale, 0.5f, r->z + 640.f * RoomScale);
    bbPositionEntity(r->objects[3], r->x + 728.f * RoomScale, 0.5f, r->z + 640.f * RoomScale);
    bbEntityParent(r->objects[2], r->obj);
    bbEntityParent(r->objects[3], r->obj);

    Item* it = CreatePaper("914_organic", r->x +954.f * RoomScale, r->y +228.f * RoomScale, r->z + 127.f * RoomScale);
    bbEntityParent(it->collider, r->obj);

    it = CreateItem("firstaid", r->x + 960.f * RoomScale, r->y + 112.f * RoomScale, r->z - 40.f * RoomScale);
    bbEntityParent(it->collider, r->obj);
    bbRotateEntity(it->collider, 0, 90, 0);

    it = CreatePaper("drL4", r->x - 928.f * RoomScale, 160.f * RoomScale, r->z - 160.f * RoomScale);
    bbEntityParent(it->collider, r->obj);
}

void UpdateEvent_cont_914_1(Event* e) {
    if (mainPlayer->currRoom == e->room) {
        if (!e->loaded) {
            e->sounds[0] = bbLoadSound("SFX/SCP/914/Refining.ogg");
            e->sounds[1] = bbLoadSound("SFX/SCP/914/PlayerUse.ogg");
            e->sounds[2] = bbLoadSound("SFX/SCP/914/PlayerDeath.ogg");

            e->loaded = true;
        }

        bbEntityPick(mainPlayer->cam, 1.f);
        if (bbPickedEntity() == e->room->objects[0]) {
            DrawHandIcon = true;
            if (MouseHit1) {
                mainPlayer->grabbedEntity = e->room->objects[0];
            }
        } else if ((bbPickedEntity() == e->room->objects[1])) {
            DrawHandIcon = true;
            if (MouseHit1) {
                mainPlayer->grabbedEntity = e->room->objects[1];
            }
        }

        if (MouseDown1 || MouseHit1) {
            //avain
            if (mainPlayer->grabbedEntity != 0) {
                if (mainPlayer->grabbedEntity == e->room->objects[0]) {
                    if (e->eventState == 0) {
                        DrawHandIcon = true;
                        bbTurnEntity(mainPlayer->grabbedEntity, 0, 0, -mouse_x_speed_1 * 2.5f);

                        float angle = WrapAngle(bbEntityRoll(e->room->objects[0]));
                        if (angle > 181) {
                            mainPlayer->drawDirectionalArrow[3] = true;
                        }
                        mainPlayer->drawDirectionalArrow[1] = true;

                        if (angle < 90) {
                            bbRotateEntity(mainPlayer->grabbedEntity, 0, 0, 361.f);
                        } else if (angle < 180) {
                            bbRotateEntity(mainPlayer->grabbedEntity, 0, 0, 180);
                        }

                        if (angle < 181 && angle > 90) {
                            for (int iterator167 = 0; iterator167 < Item::getListSize(); iterator167++) {
                                Item* it = Item::getObject(iterator167);

                                if (it->collider != 0 && it->picked == false) {
                                    if (abs(bbEntityX(it->collider) - (e->room->x - 712.f * RoomScale)) < 200.f) {
                                        if (abs(bbEntityY(it->collider) - (e->room->y + 648.f * RoomScale)) < 104.f) {
                                            e->eventState = 1;
                                            e->soundChannels[0] = PlayRangedSound(e->sounds[0], mainPlayer->cam, e->room->objects[1]);
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else if ((mainPlayer->grabbedEntity == e->room->objects[1])) {
                    if (e->eventState == 0) {
                        DrawHandIcon = true;
                        bbTurnEntity(mainPlayer->grabbedEntity, 0, 0, -mouse_x_speed_1 * 2.5f);

                        float angle = WrapAngle(bbEntityRoll(e->room->objects[1]));
                        mainPlayer->drawDirectionalArrow[3] = true;
                        mainPlayer->drawDirectionalArrow[1] = true;

                        if (angle > 90) {
                            if (angle < 180) {
                                bbRotateEntity(mainPlayer->grabbedEntity, 0, 0, 90.f);
                            } else if (angle < 270) {
                                bbRotateEntity(mainPlayer->grabbedEntity, 0, 0, 270);
                            }
                        }

                    }
                }
            }
        } else {
            mainPlayer->grabbedEntity = 0;
        }

        String setting = ""; // TODO: Enum.
        if (mainPlayer->grabbedEntity != e->room->objects[1]) {
            float angle = WrapAngle(bbEntityRoll(e->room->objects[1]));
            if (angle < 22.5f) {
                angle = 0;
                setting = "1:1";
            } else if (angle < 67.5f) {
                angle = 40;
                setting = "coarse";
            } else if (angle < 180) {
                angle = 90;
                setting = "rough";
            } else if (angle > 337.5f) {
                angle = 359 - 360;
                setting = "1:1";
            } else if (angle > 292.5f) {
                angle = 320 - 360;
                setting = "fine";
            } else {
                angle = 270 - 360;
                setting = "very fine";
            }
            bbRotateEntity(e->room->objects[1], 0, 0, CurveValue(angle, bbEntityRoll(e->room->objects[1]), 20));
        }

        for (int i = 0; i <= 1; i++) {
            if (mainPlayer->grabbedEntity == e->room->objects[i]) {
                if (!bbEntityInView(e->room->objects[i], mainPlayer->cam)) {
                    mainPlayer->grabbedEntity = 0;
                } else if ((bbEntityDistance(e->room->objects[i], mainPlayer->cam) > 1.f)) {
                    mainPlayer->grabbedEntity = 0;
                }
            }
        }

        if (e->eventState > 0) {
            e->eventState = e->eventState + timing->tickDuration;

            e->room->doors[1]->open = false;
            if (e->eventState > 70 * 2) {
                e->room->doors[0]->open = false;
            }

            if (Distance(bbEntityX(mainPlayer->collider), bbEntityZ(mainPlayer->collider), bbEntityX(e->room->objects[2], true), bbEntityZ(e->room->objects[2], true)) < (170.f * RoomScale)) {

                if (setting.equals("rough") || setting.equals("coarse")) {
                    if (e->eventState > 70 * 2.6f && e->eventState - timing->tickDuration < 70 * 2.6f) {
                        PlaySound2(e->sounds[2]);
                    }
                }

                if (e->eventState > 70 * 3) {
                    if (setting.equals("rough")) {
                        Kill(mainPlayer);
                        mainPlayer->blinkTimer = -10;
                        if (e->soundChannels[0] != 0) {
                            bbStopChannel(e->soundChannels[0]);
                        }
                        DeathMSG = "\"A heavily mutilated corpse found inside the output booth of SCP-914. DNA testing identified the corpse as Class D Subject D-9341. ";
                        DeathMSG = DeathMSG + "The subject had obviously been \"refined\" by SCP-914 on the \"Rough\" setting, but we are still confused as to how he ";
                        DeathMSG = DeathMSG + "ended up inside the intake booth and who or what wound the key.\"";
                    } else if (setting.equals("coarse")) {
                        mainPlayer->blinkTimer = -10;
                        if (e->eventState - timing->tickDuration < 70 * 3) {
                            PlaySound2(e->sounds[1]);
                        }
                    } else if (setting.equals("1:1")) {
                        mainPlayer->blinkTimer = -10;
                        if (e->eventState - timing->tickDuration < 70 * 3) {
                            PlaySound2(e->sounds[1]);
                        }
                    } else if (setting.equals("fine") || setting.equals("very fine")) {
                        mainPlayer->blinkTimer = -10;
                        if (e->eventState - timing->tickDuration < 70 * 3) {
                            PlaySound2(e->sounds[1]);
                        }
                    }
                }
            }

            if (e->eventState > (6 * 70)) {
                bbRotateEntity(e->room->objects[0], bbEntityPitch(e->room->objects[0]), bbEntityYaw(e->room->objects[0]), CurveAngle(0, bbEntityRoll(e->room->objects[0]),10.f));
            } else {
                bbRotateEntity(e->room->objects[0], bbEntityPitch(e->room->objects[0]), bbEntityYaw(e->room->objects[0]), 180);
            }

            if (e->eventState > (12 * 70)) {
                for (int iterator168 = 0; iterator168 < Item::getListSize(); iterator168++) {
                    Item* it = Item::getObject(iterator168);

                    if (it->collider != 0 && it->picked == false) {
                        if (Distance(bbEntityX(it->collider), bbEntityZ(it->collider), bbEntityX(e->room->objects[2], true), bbEntityZ(e->room->objects[2], true)) < (180.f * RoomScale)) {
                            //Use914(it, setting, EntityX(e\room\objects[3], True), EntityY(e\room\objects[3], True), EntityZ(e\room\objects[3], True))

                        }
                    }
                }

                if (Distance(bbEntityX(mainPlayer->collider), bbEntityZ(mainPlayer->collider), bbEntityX(e->room->objects[2], true), bbEntityZ(e->room->objects[2], true)) < (160.f * RoomScale)) {
                    if (setting.equals("coarse")) {
                        mainPlayer->injuries = 4.f;
                        Msg = "You notice countless small incisions all around your body. They are bleeding heavily.";
                        MsgTimer = 70*8;
                    } else if (setting.equals("1:1")) {
                        userOptions->invertMouseY = (!userOptions->invertMouseY);
                    } else if (setting.equals("fine") || setting.equals("very fine")) {
                        mainPlayer->superMan = 1.f;
                    }
                    mainPlayer->blurTimer = 1000;
                    bbPositionEntity(mainPlayer->collider, bbEntityX(e->room->objects[3], true), bbEntityY(e->room->objects[3], true) + 1.f, bbEntityZ(e->room->objects[3], true));
                    bbResetEntity(mainPlayer->collider);
                    mainPlayer->dropSpeed = 0;
                }

                e->room->doors[0]->open = true;
                e->room->doors[1]->open = true;
                bbRotateEntity(e->room->objects[0], 0, 0, 0);
                e->eventState = 0;
            }
        }
    }

}

}
