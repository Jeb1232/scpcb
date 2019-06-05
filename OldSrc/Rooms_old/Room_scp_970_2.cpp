#include <bbblitz3d.h>
#include <bbmath.h>
#include <bbaudio.h>

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
#include "Room_scp_970_2.h"

namespace CBN {

// Functions.
void FillRoom_scp_970_2(Room* r) {
    r->doors[0] = CreateDoor(r->x - 1288.f * RoomScale, 0, r->z, 270, r);
    r->doors[1] = CreateDoor(r->x - 760.f * RoomScale, 0, r->z, 270, r);
    r->doors[2] = CreateDoor(r->x - 264.f * RoomScale, 0, r->z, 270, r);
    r->doors[3] = CreateDoor(r->x + 264.f * RoomScale, 0, r->z, 270, r);
    r->doors[4] = CreateDoor(r->x + 760.f * RoomScale, 0, r->z, 270, r);
    r->doors[5] = CreateDoor(r->x + 1288.f * RoomScale, 0, r->z, 270, r);

    for (int i = 0; i < 6; i++) {
        bbMoveEntity(r->doors[i]->buttons[0], 0,0,-8.f);
        bbMoveEntity(r->doors[i]->buttons[1], 0,0,-8.f);
        r->doors[i]->autoClose = false;
        r->doors[i]->open = false;
    }

    Item* it = CreatePaper("doc939", r->x + 352.f * RoomScale, r->y + 176.f * RoomScale, r->z + 256.f * RoomScale);
    bbRotateEntity(it->collider, 0, r->angle+4, 0);
    bbEntityParent(it->collider, r->obj);

    it = CreateItem("battery", r->x + 352.f * RoomScale, r->y + 112.f * RoomScale, r->z + 448.f * RoomScale);
    bbEntityParent(it->collider, r->obj);

    it = CreateItem("cup", r->x-672*RoomScale, 240*RoomScale, r->z+288.f*RoomScale);
    bbEntityParent(it->collider, r->obj);
}

void UpdateEvent_scp_970_2(Event* e) {
    if (mainPlayer->currRoom == e->room) {
        if (e->eventState2 <= 0) {
            e->room->doors[1]->locked = false;
            e->room->doors[4]->locked = false;

            if (bbEntityDistance(mainPlayer->collider, Curr173->obj)<8.f || bbEntityDistance(mainPlayer->collider, Curr106->obj)<8.f) {
                e->room->doors[1]->locked = true;
                e->room->doors[4]->locked = true;
            } else {
                for (int i = 0; i < NPC::getListSize(); i++) {
                    NPC* n = NPC::getObject(i);

                    if (n->npcType == NPCtypeMTF) {
                        if (bbEntityDistance(mainPlayer->collider, Curr173->obj)<8.f) {
                            e->room->doors[1]->locked = true;
                            e->room->doors[4]->locked = true;
                            break;
                        }
                    }
                }
            }
            e->eventState2 = 70*5;
        } else {
            e->eventState2 = e->eventState2 - timing->tickDuration;
        }

        //LightVolume = TempLightVolume*0.5f

        bbTFormPoint(bbEntityX(mainPlayer->collider),bbEntityY(mainPlayer->collider),bbEntityZ(mainPlayer->collider),0,e->room->obj);

        bool temp = false;
        if (bbTFormedX()>730) {
            bbUpdateWorld();
            bbTFormPoint(bbEntityX(mainPlayer->collider),bbEntityY(mainPlayer->collider),bbEntityZ(mainPlayer->collider),0,e->room->obj);

            //1->3, 2->4
            //3->0, 4->0
            for (int i = 1; i < 3; i++) {
                e->room->doors[i]->open = e->room->doors[i+2]->open;
                e->room->doors[i]->openstate = e->room->doors[i+2]->openstate;
                bbPositionEntity(e->room->doors[i]->obj, bbEntityX(e->room->doors[i+2]->obj),bbEntityY(e->room->doors[i+2]->obj),bbEntityZ(e->room->doors[i+2]->obj));
                bbPositionEntity(e->room->doors[i]->obj2, bbEntityX(e->room->doors[i+2]->obj2),bbEntityY(e->room->doors[i+2]->obj2),bbEntityZ(e->room->doors[i+2]->obj2));

                e->room->doors[i+2]->open = false;
                e->room->doors[i+2]->openstate = 0;
                bbPositionEntity(e->room->doors[i+2]->obj, bbEntityX(e->room->doors[0]->obj),bbEntityY(e->room->doors[0]->obj),bbEntityZ(e->room->doors[0]->obj));
                bbPositionEntity(e->room->doors[i+2]->obj2, bbEntityX(e->room->doors[0]->obj2),bbEntityY(e->room->doors[0]->obj2),bbEntityZ(e->room->doors[0]->obj2));
            }

            bbTFormPoint(bbTFormedX()-1024, bbTFormedY(), bbTFormedZ(),e->room->obj,0);
            bbHideEntity(mainPlayer->collider);
            bbPositionEntity(mainPlayer->collider, bbTFormedX(), bbEntityY(mainPlayer->collider), bbTFormedZ(), true);
            bbShowEntity(mainPlayer->collider);
            std::cout << "tformedx()>720";
            temp = true;

        } else if ((bbTFormedX()<-730)) {
            bbUpdateWorld();
            bbTFormPoint(bbEntityX(mainPlayer->collider),bbEntityY(mainPlayer->collider),bbEntityZ(mainPlayer->collider),0,e->room->obj);

            //3->1, 4->2
            //1->0, 2->0
            for (int i = 1; i < 3; i++) {
                e->room->doors[i+2]->open = e->room->doors[i]->open;
                e->room->doors[i+2]->openstate = e->room->doors[i]->openstate;
                bbPositionEntity(e->room->doors[i+2]->obj, bbEntityX(e->room->doors[i]->obj),bbEntityY(e->room->doors[i]->obj),bbEntityZ(e->room->doors[i]->obj));
                bbPositionEntity(e->room->doors[i+2]->obj2, bbEntityX(e->room->doors[i]->obj2),bbEntityY(e->room->doors[i]->obj2),bbEntityZ(e->room->doors[i]->obj2));

                e->room->doors[i]->open = false;
                e->room->doors[i]->openstate = 0;
                bbPositionEntity(e->room->doors[i]->obj, bbEntityX(e->room->doors[0]->obj),bbEntityY(e->room->doors[0]->obj),bbEntityZ(e->room->doors[0]->obj));
                bbPositionEntity(e->room->doors[i]->obj2, bbEntityX(e->room->doors[0]->obj2),bbEntityY(e->room->doors[0]->obj2),bbEntityZ(e->room->doors[0]->obj2));
            }

            bbTFormPoint(bbTFormedX()+1024, bbTFormedY(), bbTFormedZ(),e->room->obj,0);
            bbHideEntity(mainPlayer->collider);
            bbPositionEntity(mainPlayer->collider, bbTFormedX(), bbEntityY(mainPlayer->collider), bbTFormedZ(), true);
            bbShowEntity(mainPlayer->collider);

            std::cout << "tformedx()<720";

            temp = true;
        }

        if (temp) {

            //Rand(1,2)
            e->eventState = e->eventState+1;

            for (int i = 0; i < Item::getListSize(); i++) {
                Item* it = Item::getObject(i);

                if (bbEntityDistance(it->collider,mainPlayer->collider)<5.f) {

                    bbTFormPoint(bbEntityX(it->collider),bbEntityY(it->collider),bbEntityZ(it->collider),0,e->room->obj);
                    float x = bbTFormedX();
                    float y = bbTFormedY();
                    float z = bbTFormedZ();
                    if (bbTFormedX()>264) {
                        bbTFormPoint(x-1024,y,z,e->room->obj,0);
                        bbPositionEntity(it->collider, bbTFormedX(), bbTFormedY(), bbTFormedZ());
                        bbResetEntity(it->collider);
                    } else if ((bbTFormedX()<-264)) {
                        bbTFormPoint(x+1024,y,z,e->room->obj,0);
                        bbPositionEntity(it->collider, bbTFormedX(), bbTFormedY(), bbTFormedZ());
                        bbResetEntity(it->collider);
                    }

                }
            }

            // FIXME
            switch ((int)e->eventState) {
                case 2: {
                    int i = bbRand(mainPlayer->inventory->size);
                    if (mainPlayer->inventory->items[i]!=nullptr) {
                        RemoveItem(mainPlayer->inventory->items[i]);
                    }
                } break;
                case 5: {
                    mainPlayer->injuries = mainPlayer->injuries + 0.3f;
                } break;
                case 10: {
                    Decal* de = CreateDecal(DECAL_BLOOD_SPLATTER, bbEntityX(e->room->obj)+bbCos(e->room->angle-90)*760*RoomScale, 0.0005f, bbEntityZ(e->room->obj)+bbSin(e->room->angle-90)*760*RoomScale,90,bbRnd(360),0);
                } break;
                case 14: {
                    for (int i = 0; i < mainPlayer->inventory->size; i++) {
                        if (mainPlayer->inventory->items[i]!= nullptr) {
                            // TODO: Use Paper class.
                            if (mainPlayer->inventory->items[i]->itemTemplate->name.equals("paper")) {
                                RemoveItem(mainPlayer->inventory->items[i]);
                                for (int iterator194 = 0; iterator194 < ItemTemplate::getListSize(); iterator194++) {
                                    ItemTemplate* itt = ItemTemplate::getObject(iterator194);

                                    if (itt->name.equals("paper") && bbRand(6)==1) {
                                        mainPlayer->inventory->items[i] = CreateItem(itt->name, 1,1,1);
                                        bbHideEntity(mainPlayer->inventory->items[i]->collider);
                                        mainPlayer->inventory->items[i]->picked = true;
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }
                } break;
                case 18: {
                    bbTFormPoint(-344,176, 272, e->room->obj,0);
                    Item* it = CreatePaper("docStrange", bbTFormedX(), bbTFormedY(), bbTFormedZ());
                    bbEntityType(it->collider, HIT_ITEM);
                } break;
                case 25: {
                    e->room->npc[0] = CreateNPC(NPCtypeD, bbEntityX(e->room->obj)+bbCos(e->room->angle-90)*760*RoomScale, 0.35f, bbEntityZ(e->room->obj)+bbSin(e->room->angle-90)*760*RoomScale);
                    bbRotateEntity(e->room->npc[0]->collider, 0, e->room->angle-200, 0, true);
                    Texture* tex = bbLoadTexture("GFX/NPCs/corpse.jpg");
                    e->room->npc[0]->texture = "GFX/NPCs/corpse.jpg";
                    bbEntityTexture(e->room->npc[0]->obj, tex);
                    bbFreeTexture(tex);
                    bbSetAnimTime(e->room->npc[0]->obj,80);
                    e->room->npc[0]->state = 10;
                } break;
                case 30: {
                    int i = bbRand(0,mainPlayer->inventory->size);
                    if (mainPlayer->inventory->items[i]!=nullptr) {
                        RemoveItem(mainPlayer->inventory->items[i]);
                    }
                    mainPlayer->inventory->items[i] = CreatePaper("docStrange", 1,1,1);
                    bbHideEntity(mainPlayer->inventory->items[i]->collider);
                    mainPlayer->inventory->items[i]->picked = true;
                } break;
                case 35: {
                    for (int i = 0; i < 4; i++) {
                        Decal* de = CreateDecal(DECAL_BLOOD_POOL, e->room->x+bbRnd(-2,2), 700*RoomScale, e->room->z+bbRnd(-2,2), 270, bbRand(360), 0);
                        de->size = 0.05f;
                        de->sizeChange = 0.0005f;
                        bbEntityAlpha(de->obj, 0.8f);
                        UpdateDecals();
                    }
                } break;
                case 40: {
                    PlaySound2(LoadTempSound("SFX/radio/franklin4.ogg"));
                } break;
                case 50: {
                    e->room->npc[1] = CreateNPC(NPCtypeGuard, bbEntityX(e->room->obj)+bbCos(e->room->angle+90)*600*RoomScale, 0.35f, bbEntityZ(e->room->obj)+bbSin(e->room->angle+90)*600*RoomScale);
                    e->room->npc[1]->state = 7;
                } break;
                case 52: {
                    if (e->room->npc[1] != nullptr) {
                        RemoveNPC(e->room->npc[1]);
                        e->room->npc[1] = nullptr;
                    }
                } break;
                case 60: {
                    if (!HalloweenTex) {
                        Texture* tex970 = bbLoadTexture("GFX/npcs/173h.pt", 1);
                        bbEntityTexture(Curr173->obj, tex970, 0, 0);
                        bbFreeTexture(tex970);
                    }
                } break;
            }

            //TODO: Reimplement
            //If (Rand(10)=1) Then
            //	temp = Rand(0,2)
            //	PlaySound2(AmbientSFX(temp, Rand(0,AmbientSFXAmount(temp)-1)))
            //EndIf
        } else {
            if (e->room->npc[0] != nullptr) {
                if (bbEntityDistance(mainPlayer->collider, e->room->npc[0]->collider)<3.f) {
                    if (bbEntityInView(e->room->npc[0]->obj, mainPlayer->cam)) {
                        mainPlayer->camZoom = (bbSin((float)(TimeInPosMilliSecs())/20.f)+1.f)*15.f;
                        //HeartBeatVolume = Max(CurveValue(0.3f, HeartBeatVolume, 2.f), HeartBeatVolume)
                        mainPlayer->heartbeatIntensity = Max(mainPlayer->heartbeatIntensity, 120);
                    }
                }
            }

            if (e->room->npc[1] != nullptr) {
                bbPointEntity(e->room->npc[1]->obj, mainPlayer->collider);
                bbRotateEntity(e->room->npc[1]->collider, 0, CurveAngle(bbEntityYaw(e->room->npc[1]->obj),bbEntityYaw(e->room->npc[1]->collider),35),0);
            }

            //If (abs(TFormedX())<264) Then
            for (int i = 0; i < Item::getListSize(); i++) {
                Item* it = Item::getObject(i);

                if ((it->dropped==1 && abs(bbTFormedX())<264) || it->dropped==-1) {
                    //DebugLog("dropping/picking: "+it\dropped+" - "+EntityX(it\collider)+", "+EntityY(it\collider)+", "+EntityZ(it\collider))

                    bbTFormPoint(bbEntityX(it->collider),bbEntityY(it->collider),bbEntityZ(it->collider),0,e->room->obj);
                    float x = bbTFormedX();
                    float y = bbTFormedY();
                    float z = bbTFormedZ();

                    if (it->dropped==1) {
                        for (int j = - 1; j <= 1; j += 2) {
                            bbTFormPoint(x+1024*j,y,z,e->room->obj,0);
                            Item* it2 = CreateItem(it->itemTemplate->name, bbTFormedX(), bbEntityY(it->collider), bbTFormedZ());
                            bbRotateEntity(it2->collider, bbEntityPitch(it->collider),bbEntityYaw(it->collider),0);
                            bbEntityType(it2->collider, HIT_ITEM);
                        }
                    } else {
                        for (int i = 0; i < Item::getListSize(); i++) {
                            Item* it2 = Item::getObject(i);

                            if (it2!=it && it2->dist < 15.f) {

                                bbTFormPoint(bbEntityX(it2->collider),bbEntityY(it2->collider),bbEntityZ(it2->collider),0,e->room->obj);
                                //DebugLog(TFormedZ()+" - "+z)

                                if (bbTFormedZ()==z) {
                                    RemoveItem(it2);
                                    std::cout << "item removed";
                                }
                            }
                        }
                    }

                    break;
                }
            }
            //EndIf
        }

    }


    if (e->eventState > 26) {
        if (abs(bbEntityX(mainPlayer->collider)-e->room->x)<8.f) {
            if (abs(bbEntityZ(mainPlayer->collider)-e->room->z)<8.f) {
                if (e->sounds[0] == 0) {
                    e->sounds[0] = bbLoadSound("SFX/SCP/970/Corpse.ogg");
                }
                //
                e->soundChannels[0] = LoopRangedSound(e->sounds[0], e->soundChannels[0], mainPlayer->cam, e->room->npc[0]->obj);
                if (e->eventState < 30) {
                    //LightVolume = TempLightVolume*0.4f
                } else if ((e->eventState > 60)) {
                    AnimateNPC(e->room->npc[0], 80, 61, -0.02f, false);

                    e->room->npc[0]->dropSpeed = 0;
                    float y = CurveValue(1.5f+bbSin((float)(TimeInPosMilliSecs())/20.f)*0.1f,bbEntityY(e->room->npc[0]->collider),50.f);

                    bbPositionEntity(e->room->npc[0]->collider,bbEntityX(e->room->npc[0]->collider),y,bbEntityZ(e->room->npc[0]->collider));
                    bbTurnEntity(e->room->npc[0]->collider,0,0.1f*timing->tickDuration,0);
                }
            }

        }
    }



}

}
