#include <bbblitz3d.h>
#include <bbaudio.h>
#include <bbmath.h>

#include "NPCs.h"
#include "../Config/INI.h"
#include "../GameMain.h"
#include "../Menus/Menu.h"
#include "../AssetMgmt/Audio.h"
#include "../Map/MapSystem.h"
#include "../Player/Player.h"
#include "../MathUtils/MathUtils.h"
#include "../Config/Difficulty.h"
#include "../Map/Doors.h"
#include "../Map/Particles.h"
#include "../Menus/Console/Console.h"
#include "NPCtypeMTF.h"

namespace CBN {

// Globals.
float MTFtimer;
Room* MTFrooms[10];
int MTFroomState[10];

// Functions.
void InitializeNPCtypeMTF(NPC* n) {
    n->nvName = "Human";
    n->collider = bbCreatePivot();
    bbEntityRadius(n->collider, 0.2f);
    //EntityRadius(mainPlayer\collider, 0.15f, 0.30f)
    bbEntityType(n->collider, HIT_PLAYER);
    //EntityPickMode(n\collider, 1)

    NPC* n2;
    for (int iterator138 = 0; iterator138 < NPC::getListSize(); iterator138++) {
        n2 = NPC::getObject(iterator138);

        if (n2->npcType == n->npcType && n2->obj != 0) {
            n->obj = bbCopyMeshModelEntity(n2->obj);
            break;
        }
    }

    if (n->obj == 0) {
        n->obj = bbLoadAnimMesh("GFX/NPCs/MTF/mtf.b3d");
    }

    n->speed = (getINIFloat("Data/NPCs.ini", "MTF", "speed") / 100.f);

    float temp = (getINIFloat("Data/NPCs.ini", "MTF", "scale") / 2.5f);

    bbScaleEntity(n->obj, temp, temp, temp);

    bbMeshCullBox(n->obj, -bbMeshWidth(n->obj), -bbMeshHeight(n->obj), -bbMeshDepth(n->obj), bbMeshWidth(n->obj)*2, bbMeshHeight(n->obj)*2, bbMeshDepth(n->obj)*2);

    //TODO; Re-implement with MTF struct.
    //If (MTFSFX(0)=0) Then
    //    MTFSFX(0)=LoadSound("SFX/Character/MTF/ClassD1.ogg")
    //    MTFSFX(1)=LoadSound("SFX/Character/MTF/ClassD2.ogg")
    //    MTFSFX(2)=LoadSound("SFX/Character/MTF/ClassD3.ogg")
    //    MTFSFX(3)=LoadSound("SFX/Character/MTF/ClassD4.ogg")
    //    MTFSFX(4)=LoadSound("SFX/Character/MTF/Tesla0.ogg")
    //    MTFSFX(5)=LoadSound("SFX/Character/MTF/Beep.ogg")
    //    MTFSFX(6)=LoadSound("SFX/Character/MTF/Breath.ogg")
    //EndIf

    Room* r;
    if (MTFrooms[6]==nullptr) {
        for (int iterator139 = 0; iterator139 < Room::getListSize(); iterator139++) {
            r = Room::getObject(iterator139);

            // TODO: Fix
            // switch (r->roomTemplate->name.toLower()) {
            //     case "room106": {
            //         MTFrooms[0] = r;
            //     }
            //     case "roompj": {
            //         MTFrooms[1] = r;
            //     }
            //     case "room079": {
            //         MTFrooms[2] = r;
            //     }
            //     case "room2poffices": {
            //         MTFrooms[3] = r;
            //     }
            //     case "914": {
            //         MTFrooms[4] = r;
            //     }
            //     case "coffin": {
            //         MTFrooms[5] = r;
            //     }
            //     case "start": {
            //         MTFrooms[6] = r;
            //     }
            // }
        }
    }
}

void UpdateNPCtypeMTF(NPC* n) {
    float x;
    float y;
    float z;
    int tmp;
    Room* r = nullptr;
    float prevDist;
    float newDist;
    int prev;
    NPC* n2 = nullptr;

    Particle* p = nullptr;
    Pivot* target = nullptr;
    float dist = 0.f;
    float dist2;
    WayPoint* wp = nullptr;
    int foundChamber;
    Pivot* pvt = nullptr;
    int temp;
    float soundVol173;
    float angle;
    float curr173Dist;
    float tempDist;

    if (n->isDead) {
        n->blinkTimer = -1.f;
        SetNPCFrame(n, 532);
        return;
    }

    n->maxGravity = 0.03f;

    n->blinkTimer = n->blinkTimer - timing->tickDuration;
    if (n->blinkTimer<=-5.f) {
        //only play the "blinking" sound clip if searching/containing 173
        if (n->state == 2) {
            if (OtherNPCSeesMeNPC(Curr173,n)) {
                PlayMTFSound(LoadTempSound("SFX/Character/MTF/173/BLINKING.ogg"),n);
            }
        }
        n->blinkTimer = 70.f*bbRnd(10.f,15.f);
    }

    n->reload = n->reload - timing->tickDuration;

    float prevFrame = n->frame;

    n->boneToManipulate = "";
    n->boneToManipulate2 = "";
    n->manipulateBone = false;
    n->manipulationType = 0;
    n->npcNameInSection = "MTF";

    if ((int)(n->state) != 1) {
        n->prevState = 0;
    }

    if (n->idle>0.f) {
        FinishWalking(n,488,522,0.015f*26);
        n->idle = n->idle-timing->tickDuration;
        if (n->idle<=0.f) {
            n->idle = 0.f;
        }
    } else {
        //what is this MTF doing)
        switch ((int)(n->state)) {
            case 0: {

                n->speed = 0.015f;
                //update path
                if (n->pathTimer<=0.f) {
                    //i'll follow the leader
                    if (n->mtfLeader!=nullptr) {
                        //whatever you say boss
                        n->pathStatus = FindPath(n,bbEntityX(n->mtfLeader->collider,true),bbEntityY(n->mtfLeader->collider,true)+0.1f,bbEntityZ(n->mtfLeader->collider,true));
                        //i am the leader
                    } else {
                        if (Curr173->idle!=2) {
                            for (int iterator140 = 0; iterator140 < Room::getListSize(); iterator140++) {
                                r = Room::getObject(iterator140);

                                if (((abs(r->x-bbEntityX(n->collider,true))>12.f) || (abs(r->z-bbEntityZ(n->collider,true))>12.f)) && (bbRand(1,(int)(Max(4-(int)(abs(r->z-bbEntityZ(n->collider,true)/8.f)),2.f)))==1)) {
                                    x = r->x;
                                    y = 0.1f;
                                    z = r->z;
                                    std::cout << r->roomTemplate->name;
                                    break;
                                }
                            }
                        } else {
                            tmp = false;
                            if (bbEntityDistance(n->collider,Curr173->collider)>4.f) {
                                if (!bbEntityVisible(n->collider,Curr173->collider)) {
                                    tmp = true;
                                }
                            }

                            if (!tmp) {
                                for (int iterator141 = 0; iterator141 < Room::getListSize(); iterator141++) {
                                    r = Room::getObject(iterator141);

                                    if (r->roomTemplate->name.equals("start")) {
                                        foundChamber = false;
                                        pvt = bbCreatePivot();
                                        bbPositionEntity(pvt,bbEntityX(r->obj,true)+4736*RoomScale,0.5f,bbEntityZ(r->obj,true)+1692*RoomScale);

                                        if (Distance(bbEntityX(pvt),bbEntityZ(pvt),bbEntityX(n->collider),bbEntityZ(n->collider))<3.5f) {
                                            foundChamber = true;
                                            std::cout << String(Distance(bbEntityX(pvt),bbEntityZ(pvt),bbEntityX(n->collider),bbEntityZ(n->collider)));
                                        }

                                        if (Curr173->idle == 3 && Distance(bbEntityX(pvt),bbEntityZ(pvt),bbEntityX(n->collider),bbEntityZ(n->collider)) > 4.f) {
                                            /*if (r->doors[1]->open == true) {
                                                UseDoor(r->doors[1],false);
                                            }
                                            TODO: IHH*/
                                        }

                                        bbFreeEntity(pvt);

                                        if (Distance(bbEntityX(n->collider),bbEntityZ(n->collider),bbEntityX(r->obj,true)+4736*RoomScale,bbEntityZ(r->obj,true)+1692*RoomScale)>1.6f && (!foundChamber)) {
                                            x = bbEntityX(r->obj,true)+4736*RoomScale;
                                            y = 0.1f;
                                            z = bbEntityZ(r->obj,true)+1692*RoomScale;
                                            std::cout << "Move to 173's chamber";
                                            break;
                                        } else if ((Distance(bbEntityX(n->collider),bbEntityZ(n->collider),bbEntityX(r->obj,true)+4736*RoomScale,bbEntityZ(r->obj,true)+1692*RoomScale)>1.6f && foundChamber)) {
                                            n->pathX = bbEntityX(r->obj,true)+4736*RoomScale;
                                            n->pathZ = bbEntityZ(r->obj,true)+1692*RoomScale;
                                            std::cout << "Move inside 173's chamber";
                                            break;
                                        } else {
                                            Curr173->idle = 3;
                                            Curr173->target = nullptr;
                                            Curr173->isDead = true;
                                            if (n->sounds[0] != 0) {
                                                bbFreeSound(n->sounds[0]);
                                                n->sounds[0] = 0;
                                            }
                                            n->sounds[0] = bbLoadSound("SFX/Character/MTF/173/Cont"+String(bbRand(1,4))+".ogg");
                                            PlayMTFSound(n->sounds[0], n);
                                            std::cout << "173 contained";
                                            break;
                                        }
                                    }
                                }
                            } else {
                                x = bbEntityX(Curr173->collider);
                                y = 0.1f;
                                z = bbEntityZ(Curr173->collider);
                                std::cout << "Going back to 173's cage";
                            }
                        }
                        //we're going to this room for no particular reason
                        if (n->pathX==0) {
                            n->pathStatus = FindPath(n,x,y,z);
                        }
                    }
                    if (n->pathStatus == 1) {
                        while (n->path[n->pathLocation]==nullptr) {
                            if (n->pathLocation>19) {
                                break;
                            }
                            n->pathLocation = n->pathLocation+1;
                        }
                        if (n->pathLocation<19) {
                            if (n->path[n->pathLocation]!=nullptr && n->path[n->pathLocation+1]!=nullptr) {
                                //If (n\path[n\pathLocation]\door=Null) Then ;TODO: fix?
                                if (abs(bbDeltaYaw(n->collider,n->path[n->pathLocation]->obj))>abs(bbDeltaYaw(n->collider,n->path[n->pathLocation+1]->obj))) {
                                    n->pathLocation = n->pathLocation+1;
                                }
                                //EndIf
                            }
                        }
                    }
                    //search again after 6-10 seconds
                    n->pathTimer = 70.f * bbRnd(6.f,10.f);
                } else if ((n->pathTimer<=70.f * 2.5f) && (n->mtfLeader==nullptr)) {
                    n->pathTimer = n->pathTimer-timing->tickDuration;
                    n->currSpeed = 0.f;
                    if (bbRand(1,35)==1) {
                        bbRotateEntity(n->collider,0.f,bbRnd(360.f),0.f,true);
                    }
                    FinishWalking(n,488,522,n->speed*26);
                    n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                    bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                } else {
                    if (n->pathStatus==2) {
                        //timer goes down fast
                        n->pathTimer = n->pathTimer-(timing->tickDuration*2.f);
                        n->currSpeed = 0.f;
                        if (bbRand(1,35)==1) {
                            bbRotateEntity(n->collider,0.f,bbRnd(360.f),0.f,true);
                        }
                        FinishWalking(n,488,522,n->speed*26);
                        n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                        bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                    } else if ((n->pathStatus==1)) {
                        if (n->path[n->pathLocation]==nullptr) {
                            if (n->pathLocation > 19) {
                                n->pathLocation = 0;
                                n->pathStatus = 0;
                            } else {
                                n->pathLocation = n->pathLocation + 1;
                            }
                        } else {
                            prevDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                            bbPointEntity(n->collider,n->path[n->pathLocation]->obj);
                            bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);

                            RotateToDirection(n);

                            n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);

                            bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);

                            n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);
                            //MoveEntity(n\collider, 0, 0, n\currSpeed * timing\tickDuration)

                            bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                            AnimateNPC(n,488, 522, n->currSpeed*26);

                            newDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                            //TODO: fix
                            //If (newDist<1.f And n\path[n\pathLocation]\door<>Null) Then
                            //	;open the door and make it automatically close after 5 seconds
                            //	If (Not n\path[n\pathLocation]\door\open)
                            //		PlayMTFSound(MTFSFX(5),n)
                            //	EndIf
                            //	n\path[n\pathLocation]\door\open = True
                            //	If n\path[n\pathLocation]\door\mtfClose
                            //		n\path[n\pathLocation]\door\timerstate = 70.f*5.f
                            //	EndIf
                            //EndIf

                            if (newDist<0.2f || (prevDist<newDist && prevDist<1.f)) {
                                n->pathLocation = n->pathLocation+1;
                            }
                        }
                        //timer goes down slow
                        n->pathTimer = n->pathTimer-timing->tickDuration;
                    } else if ((n->pathX!=0.f)) {
                        pvt = bbCreatePivot();
                        bbPositionEntity(pvt,n->pathX,0.5f,n->pathZ);

                        bbPointEntity(n->collider,pvt);
                        bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                        n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                        bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);

                        n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);
                        bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                        AnimateNPC(n,488, 522, n->currSpeed*26);

                        if (Distance(bbEntityX(n->collider),bbEntityZ(n->collider),n->pathX,n->pathZ)<0.2f) {
                            n->pathX = 0.f;
                            n->pathZ = 0.f;
                            n->pathTimer = 70.f * bbRnd(6.f,10.f);
                        }

                        bbFreeEntity(pvt);
                    } else {
                        //timer goes down fast
                        n->pathTimer = n->pathTimer-(timing->tickDuration*2.f);
                        if (n->mtfLeader == nullptr) {
                            if (bbRand(1,35)==1) {
                                bbRotateEntity(n->collider,0.f,bbRnd(360.f),0.f,true);
                            }
                            FinishWalking(n,488,522,n->speed*26);
                            n->currSpeed = 0.f;
                        } else if ((bbEntityDistance(n->collider,n->mtfLeader->collider)>1.f)) {
                            bbPointEntity(n->collider,n->mtfLeader->collider);
                            bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);

                            n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);
                            bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                            AnimateNPC(n,488, 522, n->currSpeed*26);
                        } else {
                            if (bbRand(1,35)==1) {
                                bbRotateEntity(n->collider,0.f,bbRnd(360.f),0.f,true);
                            }
                            FinishWalking(n,488,522,n->speed*26);
                            n->currSpeed = 0.f;
                        }
                        n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                        bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                    }
                }

                temp = MeNPCSeesPlayer(n);

                if (console->noTarget) {
                    temp = false;
                }

                if (temp == 1) {
                    if (n->lastSeen > 0 && n->lastSeen < 70*15) {
                        if (temp < 2) {
                            if (n->sounds[0] != 0) {
                                bbFreeSound(n->sounds[0]);
                                n->sounds[0] = 0;
                            }
                            n->sounds[0] = bbLoadSound("SFX/Character/MTF/ThereHeIs"+String(bbRand(1,6))+".ogg");
                            PlayMTFSound(n->sounds[0], n);
                        }
                    } else {
                        if (temp) {
                            if (n->sounds[0] != 0) {
                                bbFreeSound(n->sounds[0]);
                                n->sounds[0] = 0;
                            }
                            n->sounds[0] = bbLoadSound("SFX/Character/MTF/bbStop"+String(bbRand(1,6))+".ogg");
                            PlayMTFSound(n->sounds[0], n);
                        } else if (temp == 2) {
                            //If (n\sounds[0] <> 0) Then FreeSound(n\sounds[0]
                            //n->sounds[0] = 0);
                            //n\sounds[0] = MTFSFX(Rand(0,3))
                            //PlayMTFSound(n\sounds[0], n)
                            //PlayMTFSound(MTFSFX(Rand(0,3)),n)
                        }
                    }

                    n->lastSeen = (int)(70*bbRnd(30,40));
                    n->lastDist = 1;

                    n->state = 1;
                    n->enemyX = bbEntityX(mainPlayer->collider,true);
                    n->enemyY = bbEntityY(mainPlayer->collider,true);
                    n->enemyZ = bbEntityZ(mainPlayer->collider,true);
                    //give up after 15 seconds (30 seconds if detected by loud noise, over camera: 45)
                    n->state2 = 70.f*(15.f*temp);
                    std::cout << "player spotted :"+String(n->state2);
                    n->pathTimer = 0.f;
                    n->pathStatus = 0;
                    n->reload = 200.f-(100.f*SelectedDifficulty->aggressiveNPCs);

                    //If EntityDistance(n\collider,mainPlayer\collider)>HideDistance*0.7f
                    //	TeleportMTFGroup(n)
                    //EndIf
                }

                //B3D doesn't do short-circuit evaluation, so this retarded nesting is an optimization
                if (Curr173->idle<2) {
                    soundVol173 = Max(Min((Distance(bbEntityX(Curr173->collider), bbEntityZ(Curr173->collider), Curr173->prevX, Curr173->prevZ) * 2.5f), 1.f), 0.f);
                    if (OtherNPCSeesMeNPC(Curr173,n) || (soundVol173>0.f && bbEntityDistance(n->collider,Curr173->collider)<6.f)) {
                        if (bbEntityVisible(n->collider,Curr173->collider) || soundVol173>0.f) {
                            n->state = 2;
                            n->enemyX = bbEntityX(Curr173->collider,true);
                            n->enemyY = bbEntityY(Curr173->collider,true);
                            n->enemyZ = bbEntityZ(Curr173->collider,true);
                            //give up after 15 seconds
                            n->state2 = (int)(70.f*15.f);
                            n->state3 = 0.f;
                            n->pathTimer = 0.f;
                            n->pathStatus = 0;
                            std::cout << "173 spotted :"+String(n->state2);
                            if (n->sounds[0] != 0) {
                                bbFreeSound(n->sounds[0]);
                                n->sounds[0] = 0;
                            }
                            n->sounds[0] = bbLoadSound("SFX/Character/MTF/173/Spotted"+String(bbRand(1,2))+".ogg");
                            PlayMTFSound(n->sounds[0], n);
                        }
                    }
                }

                if (Curr106->state <= 0) {
                    if (OtherNPCSeesMeNPC(Curr106,n) || bbEntityDistance(n->collider,Curr106->collider)<3.f) {
                        if (bbEntityVisible(n->collider,Curr106->collider)) {
                            n->state = 4;
                            n->enemyX = bbEntityX(Curr106->collider,true);
                            n->enemyY = bbEntityY(Curr106->collider,true);
                            n->enemyZ = bbEntityZ(Curr106->collider,true);
                            n->state2 = 70*15.f;
                            n->state3 = 0.f;
                            n->pathTimer = 0.f;
                            n->pathStatus = 0;
                            n->target = Curr106;
                            std::cout << "106 spotted :"+String(n->state2);
                            //If n\mtfLeader=Null
                            if (n->sounds[0] != 0) {
                                bbFreeSound(n->sounds[0]);
                                n->sounds[0] = 0;
                            }
                            n->sounds[0] = bbLoadSound("SFX/Character/MTF/106/Spotted"+String(bbRand(1,3))+".ogg");
                            PlayMTFSound(n->sounds[0], n);
                            //EndIf
                        }
                    }
                }

                if (Curr096 != nullptr) {
                    if (OtherNPCSeesMeNPC(Curr096,n)) {
                        if (bbEntityVisible(n->collider,Curr096->collider)) {
                            n->state = 8;
                            n->enemyX = bbEntityX(Curr096->collider,true);
                            n->enemyY = bbEntityY(Curr096->collider,true);
                            n->enemyZ = bbEntityZ(Curr096->collider,true);
                            n->state2 = 70*15.f;
                            n->state3 = 0.f;
                            n->pathTimer = 0.f;
                            n->pathStatus = 0;
                            std::cout << "096 spotted :"+String(n->state2);
                            //If n\mtfLeader=Null
                            if (n->sounds[0] != 0) {
                                bbFreeSound(n->sounds[0]);
                                n->sounds[0] = 0;
                            }
                            n->sounds[0] = bbLoadSound("SFX/Character/MTF/096/Spotted"+String(bbRand(1,2))+".ogg");
                            PlayMTFSound(n->sounds[0], n);
                            //EndIf
                        }
                    }
                }

                for (int iterator142 = 0; iterator142 < NPC::getListSize(); iterator142++) {
                    n2 = NPC::getObject(iterator142);

                    if (n2->npcType == NPCtype049) {
                        if (OtherNPCSeesMeNPC(n2,n)) {
                            if (bbEntityVisible(n->collider,n2->collider)) {
                                n->state = 4;
                                n->enemyX = bbEntityX(n2->collider,true);
                                n->enemyY = bbEntityY(n2->collider,true);
                                n->enemyZ = bbEntityZ(n2->collider,true);
                                n->state2 = 70*15.f;
                                n->state3 = 0.f;
                                n->pathTimer = 0.f;
                                n->pathStatus = 0;
                                n->target = n2;
                                std::cout << "049 spotted :"+String(n->state2);
                                //If n\mtfLeader=Null
                                //	If (n\sounds[0] <> 0) Then FreeSound(n\sounds[0]
                                //n->sounds[0] = 0);
                                //	n\sounds[0] = LoadSound("SFX/Character/MTF/"
                                //	PlayMTFSound(n\sounds[0], n)
                                //EndIf
                                break;
                            }
                        }
                    } else if ((n2->npcType == NPCtypeZombie && n2->isDead == false)) {
                        if (OtherNPCSeesMeNPC(n2,n)) {
                            if (bbEntityVisible(n->collider,n2->collider)) {
                                n->state = 9;
                                n->enemyX = bbEntityX(n2->collider,true);
                                n->enemyY = bbEntityY(n2->collider,true);
                                n->enemyZ = bbEntityZ(n2->collider,true);
                                n->state2 = 70*15.f;
                                n->state3 = 0.f;
                                n->pathTimer = 0.f;
                                n->pathStatus = 0;
                                n->target = n2;
                                n->reload = 70*5;
                                std::cout << "049-2 spotted :"+String(n->state2);
                                if (n->sounds[0] != 0) {
                                    bbFreeSound(n->sounds[0]);
                                    n->sounds[0] = 0;
                                }
                                n->sounds[0] = bbLoadSound("SFX/Character/MTF/049/Player0492_1.ogg");
                                PlayMTFSound(n->sounds[0], n);
                                break;
                            }
                        }
                    }
                }

                //searching for player
            }
            case 1: {

                n->speed = 0.015f;
                n->state2 = n->state2-timing->tickDuration;
                if (MeNPCSeesPlayer(n) == 1) {

                    //if close enough, start shooting at the player
                    if (n->playerDistance < 4.f) {

                        angle = bbVectorYaw(bbEntityX(mainPlayer->collider)-bbEntityX(n->collider),0,bbEntityZ(mainPlayer->collider)-bbEntityZ(n->collider));

                        bbRotateEntity(n->collider, 0, CurveAngle(angle, bbEntityYaw(n->collider), 10.f), 0, true);
                        n->angle = bbEntityYaw(n->collider);

                        if (n->reload <= 0 && (!mainPlayer->dead)) {
                            if (bbEntityVisible(n->collider, mainPlayer->cam)) {
                                angle = WrapAngle(angle - bbEntityYaw(n->collider));
                                if (angle < 5 || angle > 355) {
                                    prev = (!mainPlayer->dead);

                                    PlayRangedSound_SM(sndMgmt->gunshot[0], mainPlayer->cam, n->collider, 15);

                                    pvt = bbCreatePivot();

                                    bbRotateEntity(pvt, bbEntityPitch(n->collider), bbEntityYaw(n->collider), 0, true);
                                    bbPositionEntity(pvt, bbEntityX(n->obj), bbEntityY(n->obj), bbEntityZ(n->obj));
                                    bbMoveEntity(pvt,0.8f*0.079f, 10.75f*0.079f, 6.9f*0.079f);

                                    Shoot(bbEntityX(pvt),bbEntityY(pvt),bbEntityZ(pvt),5.f/n->playerDistance, false);
                                    n->reload = 7;

                                    bbFreeEntity(pvt);

                                    //DeathMSG = "Subject D-9341. Died of blood loss after being shot by Nine-Tailed Fox.";

                                    //player killed -> "target terminated"
                                    if ((!prev) && mainPlayer->dead) {
                                        //DeathMSG = "Subject D-9341. Terminated by Nine-Tailed Fox.";
                                        PlayMTFSound(LoadTempSound("SFX/Character/MTF/Targetterminated"+String(bbRand(1,4))+".ogg"),n);
                                    }
                                }
                            }
                        }

                        for (int iterator143 = 0; iterator143 < NPC::getListSize(); iterator143++) {
                            n2 = NPC::getObject(iterator143);

                            if (n2->npcType == NPCtypeMTF && n2 != n) {
                                if (n2->state == 0) {
                                    if (bbEntityDistance(n->collider,n2->collider)<6.f) {
                                        n->prevState = 1;
                                        n2->lastSeen = (int)(70*bbRnd(30,40));
                                        n2->lastDist = 1;

                                        n2->state = 1;
                                        n2->enemyX = bbEntityX(mainPlayer->collider,true);
                                        n2->enemyY = bbEntityY(mainPlayer->collider,true);
                                        n2->enemyZ = bbEntityZ(mainPlayer->collider,true);
                                        n2->state2 = n->state2;
                                        n2->pathTimer = 0.f;
                                        n2->pathStatus = 0;
                                        n2->reload = 200.f-(100.f*SelectedDifficulty->aggressiveNPCs);
                                        n2->prevState = 0;
                                    }
                                }
                            }
                        }

                        if (n->prevState == 1) {
                            SetNPCFrame(n,423);
                            n->prevState = 2;
                        } else if ((n->prevState==2)) {
                            if (n->frame>200) {
                                n->currSpeed = CurveValue(0, n->currSpeed, 20.f);
                                AnimateNPC(n, 423, 463, 0.4f, false);
                                if (n->frame>462.9f) {
                                    n->frame = 78;
                                }
                            } else {
                                AnimateNPC(n, 78, 193, 0.2f, false);
                                n->currSpeed = CurveValue(0, n->currSpeed, 20.f);
                            }
                        } else {
                            if (n->frame>958) {
                                AnimateNPC(n, 1374, 1383, 0.3f, false);
                                n->currSpeed = CurveValue(0, n->currSpeed, 20.f);
                                if (n->frame>1382.9f) {
                                    n->frame = 78;
                                }
                            } else {
                                AnimateNPC(n, 78, 193, 0.2f, false);
                                n->currSpeed = CurveValue(0, n->currSpeed, 20.f);
                            }
                        }
                    } else {
                        bbPositionEntity(n->obj,n->enemyX,n->enemyY,n->enemyZ,true);
                        bbPointEntity(n->collider,n->obj);
                        bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                        n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                        bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);

                        n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);
                        bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                        AnimateNPC(n,488, 522, n->currSpeed*26);
                    }
                } else {
                    n->lastSeen = (int)(n->lastSeen - timing->tickDuration);

                    n->reload = 200.f-(100.f*SelectedDifficulty->aggressiveNPCs);

                    //update path
                    if (n->pathTimer<=0.f) {
                        n->pathStatus = FindPath(n,n->enemyX,n->enemyY+0.1f,n->enemyZ);
                        //search again after 6 seconds
                        n->pathTimer = 70.f * bbRnd(6.f,10.f);
                    } else if ((n->pathTimer<=70.f * 2.5f)) {
                        n->pathTimer = n->pathTimer-timing->tickDuration;
                        n->currSpeed = 0.f;
                        if (bbRand(1,35)==1) {
                            bbRotateEntity(n->collider,0.f,bbRnd(360.f),0.f,true);
                        }
                        FinishWalking(n,488,522,n->speed*26);
                        n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                        bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                    } else {
                        if (n->pathStatus==2) {
                            //timer goes down fast
                            n->pathTimer = n->pathTimer-(timing->tickDuration*2.f);
                            n->currSpeed = 0.f;
                            if (bbRand(1,35)==1) {
                                bbRotateEntity(n->collider,0.f,bbRnd(360.f),0.f,true);
                            }
                            FinishWalking(n,488,522,n->speed*26);
                            n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                            bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                        } else if ((n->pathStatus==1)) {
                            if (n->path[n->pathLocation]==nullptr) {
                                if (n->pathLocation > 19) {
                                    n->pathLocation = 0;
                                    n->pathStatus = 0;
                                } else {
                                    n->pathLocation = n->pathLocation + 1;
                                }
                            } else {
                                prevDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                                bbPointEntity(n->collider,n->path[n->pathLocation]->obj);
                                bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                                n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                                bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);

                                n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);

                                bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                                AnimateNPC(n,488, 522, n->currSpeed*26);

                                newDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                                //TODO: fix
                                //If (newDist<1.f And n\path[n\pathLocation]\door<>Null) Then
                                //	;open the door and make it automatically close after 5 seconds
                                //	If (Not n\path[n\pathLocation]\door\open)
                                //		sound = 0
                                //		If (n\path[n\pathLocation]\door\dir = 1) Then sound = 0 Else sound=Rand(0, 2)
                                //		;PlayRangedSound(OpenDoorSFX(n\path[n\pathLocation]\door\dir,sound),mainPlayer\cam,n\path[n\pathLocation]\door\obj)
                                //		PlayMTFSound(MTFSFX(5),n)
                                //	EndIf
                                //	n\path[n\pathLocation]\door\open = True
                                //	If n\path[n\pathLocation]\door\mtfClose
                                //		n\path[n\pathLocation]\door\timerstate = 70.f*5.f
                                //	EndIf
                                //EndIf

                                if (newDist<0.2f || (prevDist<newDist && prevDist<1.f)) {
                                    n->pathLocation = n->pathLocation+1;
                                }
                            }
                            //timer goes down slow
                            n->pathTimer = n->pathTimer-timing->tickDuration;
                        } else {
                            bbPositionEntity(n->obj,n->enemyX,n->enemyY,n->enemyZ,true);
                            if (Distance(bbEntityX(n->collider,true),bbEntityZ(n->collider,true),n->enemyX,n->enemyZ)<0.2f || !bbEntityVisible(n->obj,n->collider)) {
                                if (bbRand(1,35)==1) {
                                    bbRotateEntity(n->collider,0.f,bbRnd(360.f),0.f,true);
                                }
                                FinishWalking(n,488,522,n->speed*26);
                                if (bbRand(1,35)==1) {
                                    for (int iterator144 = 0; iterator144 < WayPoint::getListSize(); iterator144++) {
                                        wp = WayPoint::getObject(iterator144);

                                        if (bbRand(1,3)==1) {
                                            if (bbEntityDistance(wp->obj,n->collider)<6.f) {
                                                n->enemyX = bbEntityX(wp->obj,true);
                                                n->enemyY = bbEntityY(wp->obj,true);
                                                n->enemyZ = bbEntityZ(wp->obj,true);
                                                n->pathTimer = 0.f;
                                                break;
                                            }
                                        }
                                    }
                                }
                                //timer goes down slow
                                n->pathTimer = n->pathTimer-timing->tickDuration;
                            } else {
                                bbPointEntity(n->collider,n->obj);
                                bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                                n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                                bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);

                                n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);
                                bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                                AnimateNPC(n,488, 522, n->currSpeed*26);
                            }
                        }
                    }

                    if (n->mtfLeader==nullptr && n->lastSeen<70*30 && n->lastSeen+timing->tickDuration>=70*30) {
                        if (bbRand(2)==1) {
                            PlayMTFSound(LoadTempSound("SFX/Character/MTF/Searching"+String(bbRand(1,6))+".ogg"),n);
                        }
                    }

                    //If EntityDistance(n\collider,mainPlayer\collider)>HideDistance*0.7f
                    //	TeleportMTFGroup(n)
                    //EndIf
                }

                if (n->state2<=0.f && n->state2+timing->tickDuration >0.f) {
                    if (n->mtfLeader == nullptr) {
                        std::cout << "targetlost: "+String(n->state2);
                        PlayMTFSound(LoadTempSound("SFX/Character/MTF/Targetlost"+String(bbRand(1,3))+".ogg"),n);
                    }
                    n->state = 0;
                }

                //B3D doesn't do short-circuit evaluation, so this retarded nesting is an optimization
                if (Curr173->idle<2) {
                    soundVol173 = Max(Min((Distance(bbEntityX(Curr173->collider), bbEntityZ(Curr173->collider), Curr173->prevX, Curr173->prevZ) * 2.5f), 1.f), 0.f);
                    if (OtherNPCSeesMeNPC(Curr173,n) || (soundVol173>0.f && bbEntityDistance(n->collider,Curr173->collider)<6.f)) {
                        if (bbEntityVisible(n->collider,Curr173->collider) || soundVol173>0.f) {
                            n->state = 2;
                            n->enemyX = bbEntityX(Curr173->collider,true);
                            n->enemyY = bbEntityY(Curr173->collider,true);
                            n->enemyZ = bbEntityZ(Curr173->collider,true);
                            //give up after 15 seconds
                            n->state2 = 70.f*15.f;
                            std::cout << "173 spotted :"+String(n->state2);
                            if (n->sounds[0] != 0) {
                                bbFreeSound(n->sounds[0]);
                                n->sounds[0] = 0;
                            }
                            n->sounds[0] = bbLoadSound("SFX/Character/MTF/173/Spotted3.ogg");
                            PlayMTFSound(n->sounds[0], n);
                            n->state3 = 0.f;
                            n->pathTimer = 0.f;
                            n->pathStatus = 0;
                        }
                    }
                }

                if (Curr106->state <= 0) {
                    if (OtherNPCSeesMeNPC(Curr106,n) || bbEntityDistance(n->collider,Curr106->collider)<3.f) {
                        if (bbEntityVisible(n->collider,Curr106->collider)) {
                            n->state = 4;
                            n->enemyX = bbEntityX(Curr106->collider,true);
                            n->enemyY = bbEntityY(Curr106->collider,true);
                            n->enemyZ = bbEntityZ(Curr106->collider,true);
                            n->state2 = 70*15.f;
                            n->state3 = 0.f;
                            n->pathTimer = 0.f;
                            n->pathStatus = 0;
                            n->target = Curr106;
                            std::cout << "106 spotted :"+String(n->state2);
                            if (n->mtfLeader==nullptr) {
                                if (n->sounds[0] != 0) {
                                    bbFreeSound(n->sounds[0]);
                                    n->sounds[0] = 0;
                                }
                                n->sounds[0] = bbLoadSound("SFX/Character/MTF/106/Spotted4.ogg");
                                PlayMTFSound(n->sounds[0], n);
                            }
                        }
                    }
                }

                if (Curr096 != nullptr) {
                    if (OtherNPCSeesMeNPC(Curr096,n)) {
                        if (bbEntityVisible(n->collider,Curr096->collider)) {
                            n->state = 8;
                            n->enemyX = bbEntityX(Curr096->collider,true);
                            n->enemyY = bbEntityY(Curr096->collider,true);
                            n->enemyZ = bbEntityZ(Curr096->collider,true);
                            n->state2 = 70*15.f;
                            n->state3 = 0.f;
                            n->pathTimer = 0.f;
                            n->pathStatus = 0;
                            std::cout << "096 spotted :"+String(n->state2);
                            if (n->mtfLeader==nullptr) {
                                if (n->sounds[0] != 0) {
                                    bbFreeSound(n->sounds[0]);
                                    n->sounds[0] = 0;
                                }
                                n->sounds[0] = bbLoadSound("SFX/Character/MTF/096/Spotted"+String(bbRand(1,2))+".ogg");
                                PlayMTFSound(n->sounds[0], n);
                            }
                        }
                    }
                }

                for (int iterator145 = 0; iterator145 < NPC::getListSize(); iterator145++) {
                    n2 = NPC::getObject(iterator145);

                    if (n2->npcType == NPCtype049) {
                        if (OtherNPCSeesMeNPC(n2,n)) {
                            if (bbEntityVisible(n->collider,n2->collider)) {
                                n->state = 4;
                                n->enemyX = bbEntityX(n2->collider,true);
                                n->enemyY = bbEntityY(n2->collider,true);
                                n->enemyZ = bbEntityZ(n2->collider,true);
                                n->state2 = 70*15.f;
                                n->state3 = 0.f;
                                n->pathTimer = 0.f;
                                n->pathStatus = 0;
                                n->target = n2;
                                std::cout << "049 spotted :"+String(n->state2);
                                //If n\mtfLeader=Null
                                //	If (n\sounds[0] <> 0) Then FreeSound(n\sounds[0]
                                //n->sounds[0] = 0);
                                //	n\sounds[0] = LoadSound("SFX/Character/MTF/"
                                //	PlayMTFSound(n\sounds[0], n)
                                //EndIf
                                break;
                            }
                        }
                    } else if ((n2->npcType == NPCtypeZombie && n2->isDead == false)) {
                        if (OtherNPCSeesMeNPC(n2,n)) {
                            if (bbEntityVisible(n->collider,n2->collider)) {
                                n->state = 9;
                                n->enemyX = bbEntityX(n2->collider,true);
                                n->enemyY = bbEntityY(n2->collider,true);
                                n->enemyZ = bbEntityZ(n2->collider,true);
                                n->state2 = 70*15.f;
                                n->state3 = 0.f;
                                n->pathTimer = 0.f;
                                n->pathStatus = 0;
                                n->target = n2;
                                n->reload = 70*5;
                                std::cout << "049-2 spotted :"+String(n->state2);
                                //If n\mtfLeader=Null
                                if (n->sounds[0] != 0) {
                                    bbFreeSound(n->sounds[0]);
                                    n->sounds[0] = 0;
                                }
                                n->sounds[0] = bbLoadSound("SFX/Character/MTF/049/Player0492_1.ogg");
                                PlayMTFSound(n->sounds[0], n);
                                //EndIf
                                break;
                            }
                        }
                    }
                }

                //DebugLog(Distance(EntityX(n\collider,True),EntityZ(n\collider,True),n\enemyX,n\enemyZ))


                //searching for/looking at 173
            }
            case 2: {

                if (Curr173->idle == 2) {
                    n->state = 0;
                } else {
                    for (int iterator146 = 0; iterator146 < NPC::getListSize(); iterator146++) {
                        n2 = NPC::getObject(iterator146);

                        if (n2!=n) {
                            if (n2->npcType == NPCtypeMTF) {
                                n2->state = 2;
                            }
                        }
                    }

                    curr173Dist = Distance(bbEntityX(n->collider,true),bbEntityZ(n->collider,true),bbEntityX(Curr173->collider,true),bbEntityZ(Curr173->collider,true));

                    if (curr173Dist<5.f) {
                        if (Curr173->idle != 2) {
                            Curr173->idle = true;
                        }
                        n->state2 = 70.f*15.f;
                        n->pathTimer = 0.f;
                        tempDist = 1.f;
                        if (n->mtfLeader!=nullptr) {
                            tempDist = 2.f;
                        }
                        if (curr173Dist<tempDist) {
                            if (n->mtfLeader == nullptr) {
                                n->state3 = n->state3+timing->tickDuration;
                                std::cout << "CONTAINING 173: "+String(n->state3);
                                //If (n\state3>=70.f*10.f) Then
                                if (n->state3>=70.f*15.f) {
                                    Curr173->idle = 2;
                                    if (n->mtfLeader == nullptr) {
                                        Curr173->target = n;
                                    }
                                    if (n->sounds[0] != 0) {
                                        bbFreeSound(n->sounds[0]);
                                        n->sounds[0] = 0;
                                    }
                                    n->sounds[0] = bbLoadSound("SFX/Character/MTF/173/Box"+String(bbRand(1,3))+".ogg");
                                    PlayMTFSound(n->sounds[0], n);
                                }
                            }
                            bbPositionEntity(n->obj,bbEntityX(Curr173->collider,true),bbEntityY(Curr173->collider,true),bbEntityZ(Curr173->collider,true),true);
                            bbPointEntity(n->collider,n->obj);
                            bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                            n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                            FinishWalking(n,488,522,n->speed*26);
                            bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                        } else {
                            bbPositionEntity(n->obj,bbEntityX(Curr173->collider,true),bbEntityY(Curr173->collider,true),bbEntityZ(Curr173->collider,true),true);
                            bbPointEntity(n->collider,n->obj);
                            bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                            n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                            bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);

                            n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);
                            bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                            AnimateNPC(n,488, 522, n->currSpeed*26);
                        }
                    } else {
                        if (Curr173->idle != 2) {
                            Curr173->idle = false;
                        }
                        //update path
                        if (n->pathTimer<=0.f) {
                            n->pathStatus = FindPath(n,bbEntityX(Curr173->collider,true),bbEntityY(Curr173->collider,true)+0.1f,bbEntityZ(Curr173->collider,true));
                            //search again after 6 seconds
                            n->pathTimer = 70.f * bbRnd(6.f,10.f);
                        } else if ((n->pathTimer<=70.f * 2.5f)) {
                            n->pathTimer = n->pathTimer-timing->tickDuration;
                            n->currSpeed = 0.f;
                            if (bbRand(1,35)==1) {
                                bbRotateEntity(n->collider,0.f,bbRnd(360.f),0.f,true);
                            }
                            FinishWalking(n,488,522,n->speed*26);
                            n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                            bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                        } else {
                            if (n->pathStatus==2) {
                                //timer goes down fast
                                n->pathTimer = n->pathTimer-(timing->tickDuration*2.f);
                                n->currSpeed = 0.f;
                                if (bbRand(1,35)==1) {
                                    bbRotateEntity(n->collider,0.f,bbRnd(360.f),0.f,true);
                                }
                                FinishWalking(n,488,522,n->speed*26);
                                n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                                bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                            } else if ((n->pathStatus==1)) {
                                if (n->path[n->pathLocation]==nullptr) {
                                    if (n->pathLocation > 19) {
                                        n->pathLocation = 0;
                                        n->pathStatus = 0;
                                    } else {
                                        n->pathLocation = n->pathLocation + 1;
                                    }
                                } else {
                                    prevDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                                    bbPointEntity(n->collider,n->path[n->pathLocation]->obj);
                                    bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                                    n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                                    bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);

                                    n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);

                                    bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                                    AnimateNPC(n,488, 522, n->currSpeed*26);

                                    newDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                                    //TODO: fix
                                    //If (newDist<1.f And n\path[n\pathLocation]\door<>Null) Then
                                    //	;open the door and make it automatically close after 5 seconds
                                    //	If (Not n\path[n\pathLocation]\door\open)
                                    //		sound = 0
                                    //		If (n\path[n\pathLocation]\door\dir = 1) Then sound = 0 Else sound=Rand(0, 2)
                                    //		;PlayRangedSound(OpenDoorSFX(n\path[n\pathLocation]\door\dir,sound),mainPlayer\cam,n\path[n\pathLocation]\door\obj)
                                    //		PlayMTFSound(MTFSFX(5),n)
                                    //	EndIf
                                    //	n\path[n\pathLocation]\door\open = True
                                    //	If n\path[n\pathLocation]\door\mtfClose
                                    //		n\path[n\pathLocation]\door\timerstate = 70.f*5.f
                                    //	EndIf
                                    //EndIf

                                    if (newDist<0.2f || (prevDist<newDist && prevDist<1.f)) {
                                        n->pathLocation = n->pathLocation+1;
                                    }
                                }
                                //timer goes down slow
                                n->pathTimer = n->pathTimer-timing->tickDuration;
                            } else {
                                //timer goes down fast
                                n->pathTimer = n->pathTimer-(timing->tickDuration*2.f);
                                n->currSpeed = 0.f;
                                if (bbRand(1,35)==1) {
                                    bbRotateEntity(n->collider,0.f,bbRnd(360.f),0.f,true);
                                }
                                FinishWalking(n,488,522,n->speed*26);
                                n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                                bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                            }
                        }
                    }
                }

                //following a path
            }
            case 3: {


                n->angle = CurveValue(0,n->angle,40.f);

                if (n->pathStatus == 2) {
                    n->state = 5;
                    n->currSpeed = 0;
                } else if ((n->pathStatus == 1)) {
                    if (n->path[n->pathLocation]==nullptr) {
                        if (n->pathLocation > 19) {
                            n->pathLocation = 0;
                            n->pathStatus = 0;
                            if (n->lastSeen > 0) {
                                n->state = 5;
                            }
                        } else {
                            n->pathLocation = n->pathLocation + 1;
                        }
                    } else {
                        //TODO: fix
                        //If (n\path[n\pathLocation]\door <> Null) Then
                        //	If (n\path[n\pathLocation]\door\open = False) Then
                        //		n\path[n\pathLocation]\door\open = True
                        //		n\path[n\pathLocation]\door\timerstate = 8.f*70.f
                        //		PlayMTFSound(MTFSFX(5),n)
                        //	EndIf
                        //EndIf

                        if (dist < HideDistance*0.7f) {
                            dist2 = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                            //If (Rand(5)=1) Then
                            //	For n2.NPC = Each NPC
                            //		If (n2\npctype = n\npcType And n2<>n) Then
                            //			If (EntityDistance(n\collider, n2\collider)<2) Then
                            //				n\idle = 150
                            //			EndIf
                            //		EndIf
                            //	Next
                            //EndIf

                            bbPointEntity(n->obj, n->path[n->pathLocation]->obj);

                            bbRotateEntity(n->collider, 0, CurveAngle(bbEntityYaw(n->obj), bbEntityYaw(n->collider), 10.f), 0);
                            if (n->idle == 0) {
                                n->currSpeed = CurveValue(n->speed*Max(Min(dist2,1.f),0.1f), n->currSpeed, 20.f);
                                bbMoveEntity(n->collider, 0, 0, n->currSpeed * timing->tickDuration);

                                //If (dist2 < (0.25f+((n\path[Min(n\pathLocation+1,19)]=Null)*0.3f * (n\id Mod 3)))) Then
                                if (bbEntityDistance(n->collider,n->path[n->pathLocation]->obj)<0.5f) {
                                    n->pathLocation = n->pathLocation + 1;
                                }
                            }
                        } else {
                            if (bbRand(20)==1) {
                                bbPositionEntity(n->collider, bbEntityX(n->path[n->pathLocation]->obj,true),bbEntityY(n->path[n->pathLocation]->obj,true)+0.25f,bbEntityZ(n->path[n->pathLocation]->obj,true),true);
                                n->pathLocation = n->pathLocation + 1;
                                bbResetEntity(n->collider);
                            }
                        }

                    }
                } else {
                    n->currSpeed = 0;
                    n->state = 5;
                }


                if (n->idle == 0 && n->pathStatus == 1) {
                    if (dist < HideDistance) {
                        if (n->frame>959) {
                            AnimateNPC(n, 1376, 1383, 0.2f, false);
                            if (n->frame >1382.9f) {
                                n->frame = 488;
                            }
                        } else {
                            AnimateNPC(n, 488, 522, n->currSpeed*30);
                        }
                    }
                } else {
                    if (dist < HideDistance) {
                        if (n->lastSeen > 0) {
                            AnimateNPC(n, 78, 312, 0.2f, true);
                        } else {
                            if (n->frame<962) {
                                if (n->frame>487) {
                                    n->frame = 463;
                                }
                                AnimateNPC(n, 463, 487, 0.3f, false);
                                if (n->frame>486.9f) {
                                    n->frame = 962;
                                }
                            } else {
                                AnimateNPC(n, 962, 1259, 0.3f);
                            }
                        }
                    }

                    n->currSpeed = CurveValue(0, n->currSpeed, 20.f);
                }

                n->angle = bbEntityYaw(n->collider);

                //SCP-106/049 detected
            }
            case 4: {

                n->speed = 0.03f;
                n->state2 = n->state2-timing->tickDuration;
                if (n->state2 > 0.f) {
                    if (OtherNPCSeesMeNPC(n->target,n)) {
                        n->state2 = 70*15;
                    }

                    if (bbEntityDistance(n->target->collider,n->collider)>HideDistance) {
                        if (n->state2 > 70) {
                            n->state2 = 70;
                        }
                    }

                    if (bbEntityDistance(n->target->collider,n->collider)<3.f && n->state3 >= 0.f) {
                        n->state3 = 70*5;
                    }

                    if (n->state3 > 0.f) {
                        n->pathStatus = 0;
                        n->pathLocation = 0;
                        n->speed = 0.02f;
                        bbPointEntity(n->collider,n->target->collider);
                        bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                        n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                        bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                        n->currSpeed = CurveValue(-n->speed,n->currSpeed,20.f);
                        bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                        AnimateNPC(n,522, 488, n->currSpeed*26);

                        n->pathTimer = 1.f;

                        n->state3 = Max(n->state3-timing->tickDuration,0);

                        bbHideEntity(n->collider);
                        bbTurnEntity(n->collider,0,180,0);
                        bbEntityPick(n->collider, 1.f);
                        if (bbPickedEntity() != 0) {
                            n->state3 = -70*2;
                        }
                        bbShowEntity(n->collider);
                        bbTurnEntity(n->collider,0,180,0);
                    } else if ((n->state3 < 0.f)) {
                        n->state3 = Min(n->state3+timing->tickDuration,0);
                    }

                    if (n->pathTimer<=0.f) {
                        if (n->mtfLeader!=nullptr) {
                            n->pathStatus = FindPath(n,bbEntityX(n->mtfLeader->collider,true),bbEntityY(n->mtfLeader->collider,true)+0.1f,bbEntityZ(n->mtfLeader->collider,true));
                        } else {
                            for (int iterator147 = 0; iterator147 < Room::getListSize(); iterator147++) {
                                r = Room::getObject(iterator147);

                                if (((abs(r->x-bbEntityX(n->collider,true))>12.f) || (abs(r->z-bbEntityZ(n->collider,true))>12.f)) && (bbRand(1,(int)(Max(4-(int)(abs(r->z-bbEntityZ(n->collider,true)/8.f)),2.f)))==1)) {
                                    if (bbEntityDistance(r->obj,n->target->collider)>6.f) {
                                        x = r->x;
                                        y = 0.1f;
                                        z = r->z;
                                        std::cout << r->roomTemplate->name;
                                        break;
                                    }
                                }
                            }
                            n->pathStatus = FindPath(n,x,y,z);
                        }
                        if (n->pathStatus == 1) {
                            while (n->path[n->pathLocation]==nullptr) {
                                if (n->pathLocation>19) {
                                    break;
                                }
                                n->pathLocation = n->pathLocation+1;
                            }
                            if (n->pathLocation<19) {
                                if (n->path[n->pathLocation]!=nullptr && n->path[n->pathLocation+1]!=nullptr) {
                                    //If (n\path[n\pathLocation]\door=Null) Then ;TODO: fix?
                                    if (abs(bbDeltaYaw(n->collider,n->path[n->pathLocation]->obj))>abs(bbDeltaYaw(n->collider,n->path[n->pathLocation+1]->obj))) {
                                        n->pathLocation = n->pathLocation+1;
                                    }
                                    //EndIf
                                }
                            }
                        }
                        n->pathTimer = 70*10;
                    } else {
                        if (n->pathStatus==1) {
                            if (n->path[n->pathLocation]==nullptr) {
                                if (n->pathLocation > 19) {
                                    n->pathLocation = 0;
                                    n->pathStatus = 0;
                                } else {
                                    n->pathLocation = n->pathLocation + 1;
                                }
                            } else {
                                prevDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                                bbPointEntity(n->collider,n->path[n->pathLocation]->obj);
                                bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                                n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                                bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);

                                n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);
                                bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                                //Placeholder (until running animation has been implemented)
                                AnimateNPC(n,488, 522, n->currSpeed*26);

                                newDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                                //TODO: fix and remove duplicates maybe???
                                //If (newDist<2.f And n\path[n\pathLocation]\door<>Null) Then
                                //	If (Not n\path[n\pathLocation]\door\open)
                                //		sound = 0
                                //		If (n\path[n\pathLocation]\door\dir = 1) Then sound = 0 Else sound=Rand(0, 2)
                                //		;PlayRangedSound(OpenDoorSFX(n\path[n\pathLocation]\door\dir,sound),mainPlayer\cam,n\path[n\pathLocation]\door\obj)
                                //		PlayMTFSound(MTFSFX(5),n)
                                //	EndIf
                                //	n\path[n\pathLocation]\door\open = True
                                //	If n\path[n\pathLocation]\door\mtfClose
                                //		n\path[n\pathLocation]\door\timerstate = 70.f*5.f
                                //	EndIf
                                //EndIf

                                if (newDist<0.2f || (prevDist<newDist && prevDist<1.f)) {
                                    n->pathLocation = n->pathLocation+1;
                                }
                            }
                            n->pathTimer = n->pathTimer-timing->tickDuration;
                        } else {
                            n->pathTimer = 0.f;
                        }
                    }
                } else {
                    n->state = 0;
                }

                //looking at some other target than the player
            }
            case 5: {

                target = bbCreatePivot();
                bbPositionEntity(target, n->enemyX, n->enemyY, n->enemyZ, true);

                if (dist<HideDistance) {
                    AnimateNPC(n, 346, 351, 0.2f, false);
                }

                if (abs(bbEntityX(target)-bbEntityX(n->collider)) < 55.f && abs(bbEntityZ(target)-bbEntityZ(n->collider)) < 55.f && abs(bbEntityY(target)-bbEntityY(n->collider))< 20.f) {

                    bbPointEntity(n->obj, target);
                    bbRotateEntity(n->collider, 0, CurveAngle(bbEntityYaw(n->obj),bbEntityYaw(n->collider),30.f), 0, true);

                    if (n->pathTimer == 0) {
                        n->pathStatus = bbEntityVisible(n->collider,target);
                        n->pathTimer = (float)bbRand(100,200);
                    } else {
                        n->pathTimer = Min(n->pathTimer-timing->tickDuration,0.f);
                    }

                    if (n->pathStatus == 1 && n->reload <= 0) {
                        dist = Distance(bbEntityX(target),bbEntityZ(target),bbEntityX(n->collider),bbEntityZ(n->collider));

                        //If (dist<20.f) Then
                        //	pvt = CreatePivot()
                        //
                        //	PositionEntity(pvt, EntityX(n\obj),EntityY(n\obj), EntityZ(n\obj))
                        //	RotateEntity(pvt, EntityPitch(n\collider), EntityYaw(n\collider),0)
                        //	MoveEntity(pvt,0.8f*0.079f, 10.75f*0.079f, 6.9f*0.079f)
                        //
                        //	If (WrapAngle(EntityYaw(pvt)-EntityYaw(n\collider))<5) Then
                        //		PlayRangedSound(GunshotSFX, mainPlayer\cam, n\collider, 20)
                        //		p.Particle = CreateParticle(EntityX(n\obj, True), EntityY(n\obj, True), EntityZ(n\obj, True), 1, 0.2f, 0.f, 5)
                        //		PositionEntity(p\pvt, EntityX(pvt), EntityY(pvt), EntityZ(pvt))
                        //
                        //		neload = 7
                        //	EndIf
                        //
                        //	FreeEntity(pvt)
                        //EndIf
                    }
                }

                bbFreeEntity(target);

                n->angle = bbEntityYaw(n->collider);

                //seeing the player as a 049-2 instance
            }
            case 6: {


                bbPointEntity(n->obj,mainPlayer->collider);

                bbRotateEntity(n->collider,0,CurveAngle(bbEntityYaw(n->obj),bbEntityYaw(n->collider),20.f),0);
                n->angle = bbEntityYaw(n->collider);

                AnimateNPC(n, 346, 351, 0.2f, false);

                if (n->reload <= 0 && (!mainPlayer->dead)) {
                    if (bbEntityVisible(n->collider, mainPlayer->collider)) {
                        //angle# = WrapAngle(angle - EntityYaw(n\collider))
                        //If (angle < 5 Or angle > 355) Then
                        if (abs(bbDeltaYaw(n->collider,mainPlayer->collider))<50.f) {
                            //prev% = KillTimer

                            PlayRangedSound_SM(sndMgmt->gunshot[0], mainPlayer->cam, n->collider, 15);

                            pvt = bbCreatePivot();

                            bbRotateEntity(pvt, bbEntityPitch(n->collider), bbEntityYaw(n->collider), 0, true);
                            bbPositionEntity(pvt, bbEntityX(n->obj), bbEntityY(n->obj), bbEntityZ(n->obj));
                            bbMoveEntity(pvt,0.8f*0.079f, 10.75f*0.079f, 6.9f*0.079f);

                            Shoot(bbEntityX(pvt),bbEntityY(pvt),bbEntityZ(pvt),0.9f, false);
                            n->reload = 7;

                            bbFreeEntity(pvt);

                            //If (prev => 0 And KillTimer < 0) Then
                            //DeathMSG="Subject D-9341. Terminated by Nine-Tailed Fox."
                            //If (n\mtfLeader = Null) Then PlayMTFSound(LoadTempSound("SFX/Character/MTF/049/Player0492_2.ogg"),n)
                            //EndIf
                        }
                    }
                }


                //just shooting
            }
            case 7: {

                AnimateNPC(n, 346, 351, 0.2f, false);

                bbRotateEntity(n->collider,0,CurveAngle(n->state2,bbEntityYaw(n->collider),20),0);
                n->angle = bbEntityYaw(n->collider);

                if (n->reload <= 0) {
                    //LightVolume = TempLightVolume*1.2f
                    PlayRangedSound_SM(sndMgmt->gunshot[0], mainPlayer->cam, n->collider, 20);

                    pvt = bbCreatePivot();

                    bbRotateEntity(pvt, bbEntityPitch(n->collider), bbEntityYaw(n->collider), 0, true);
                    bbPositionEntity(pvt, bbEntityX(n->obj), bbEntityY(n->obj), bbEntityZ(n->obj));
                    bbMoveEntity(pvt,0.8f*0.079f, 10.75f*0.079f, 6.9f*0.079f);

                    p = CreateParticle(bbEntityX(pvt), bbEntityY(pvt), bbEntityZ(pvt), PARTICLE_FLASH, bbRnd(0.08f,0.1f), 0.f, 5);
                    bbTurnEntity(p->sprite, 0,0,bbRnd(360));
                    p->aChange = -0.15f;

                    bbFreeEntity(pvt);
                    n->reload = 7;
                }

                //SCP-096 spotted
            }
            case 8: {

                n->speed = 0.015f;
                n->boneToManipulate = "head";
                n->manipulateBone = true;
                n->manipulationType = 2;
                //update path
                if (n->pathTimer<=0.f) {
                    //i'll follow the leader
                    if (n->mtfLeader!=nullptr) {
                        //whatever you say boss
                        n->pathStatus = FindPath(n,bbEntityX(n->mtfLeader->collider,true),bbEntityY(n->mtfLeader->collider,true)+0.1f,bbEntityZ(n->mtfLeader->collider,true));
                        //i am the leader
                    } else {
                        for (int iterator148 = 0; iterator148 < Room::getListSize(); iterator148++) {
                            r = Room::getObject(iterator148);

                            if (((abs(r->x-bbEntityX(n->collider,true))>12.f) || (abs(r->z-bbEntityZ(n->collider,true))>12.f)) && (bbRand(1,(int)(Max(4-(int)(abs(r->z-bbEntityZ(n->collider,true)/8.f)),2.f)))==1)) {
                                x = r->x;
                                y = 0.1f;
                                z = r->z;
                                std::cout << r->roomTemplate->name;
                                break;
                            }
                        }
                        //we're going to this room for no particular reason
                        n->pathStatus = FindPath(n,x,y,z);
                    }
                    if (n->pathStatus == 1) {
                        while (n->path[n->pathLocation]==nullptr) {
                            if (n->pathLocation>19) {
                                break;
                            }
                            n->pathLocation = n->pathLocation+1;
                        }
                        if (n->pathLocation<19) {
                            if (n->path[n->pathLocation]!=nullptr && n->path[n->pathLocation+1]!=nullptr) {
                                //If (n\path[n\pathLocation]\door=Null) Then ;TODO: fix?
                                if (abs(bbDeltaYaw(n->collider,n->path[n->pathLocation]->obj))>abs(bbDeltaYaw(n->collider,n->path[n->pathLocation+1]->obj))) {
                                    n->pathLocation = n->pathLocation+1;
                                }
                                //EndIf
                            }
                        }
                    }
                    //search again after 6-10 seconds
                    n->pathTimer = 70.f * bbRnd(6.f,10.f);
                } else if ((n->pathTimer<=70.f * 2.5f) && (n->mtfLeader==nullptr)) {
                    n->pathTimer = n->pathTimer-timing->tickDuration;
                    n->currSpeed = 0.f;
                    //If (Rand(1,35)=1) Then
                    //	RotateEntity(n\collider,0.f,Rnd(360.f),0.f,True)
                    //EndIf
                    FinishWalking(n,488,522,n->speed*26);
                    n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                    bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                } else {
                    if (n->pathStatus==2) {
                        //timer goes down fast
                        n->pathTimer = n->pathTimer-(timing->tickDuration*2.f);
                        n->currSpeed = 0.f;
                        //If (Rand(1,35)=1) Then
                        //	RotateEntity(n\collider,0.f,Rnd(360.f),0.f,True)
                        //EndIf
                        FinishWalking(n,488,522,n->speed*26);
                        n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                        bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                    } else if ((n->pathStatus==1)) {
                        if (n->path[n->pathLocation]==nullptr) {
                            if (n->pathLocation > 19) {
                                n->pathLocation = 0;
                                n->pathStatus = 0;
                            } else {
                                n->pathLocation = n->pathLocation + 1;
                            }
                        } else {
                            prevDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                            bbPointEntity(n->collider,n->path[n->pathLocation]->obj);
                            bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                            n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                            bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);

                            n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);
                            //MoveEntity(n\collider, 0, 0, n\currSpeed * timing\tickDuration)
                            bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                            AnimateNPC(n,488, 522, n->currSpeed*26);

                            newDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                            //TODO: fix AND REMOVE DUPLICATES OMG
                            //If (newDist<1.f And n\path[n\pathLocation]\door<>Null) Then
                            //	;open the door and make it automatically close after 5 seconds
                            //	If (Not n\path[n\pathLocation]\door\open)
                            //		sound = 0
                            //		If (n\path[n\pathLocation]\door\dir = 1) Then sound = 0 Else sound=Rand(0, 2)
                            //		;PlayRangedSound(OpenDoorSFX(n\path[n\pathLocation]\door\dir,sound),mainPlayer\cam,n\path[n\pathLocation]\door\obj)
                            //		PlayMTFSound(MTFSFX(5),n)
                            //	EndIf
                            //	n\path[n\pathLocation]\door\open = True
                            //	If n\path[n\pathLocation]\door\mtfClose
                            //		n\path[n\pathLocation]\door\timerstate = 70.f*5.f
                            //	EndIf
                            //EndIf

                            if (newDist<0.2f || (prevDist<newDist && prevDist<1.f)) {
                                n->pathLocation = n->pathLocation+1;
                            }
                        }
                        //timer goes down slow
                        n->pathTimer = n->pathTimer-timing->tickDuration;
                    } else {
                        //timer goes down fast
                        n->pathTimer = n->pathTimer-(timing->tickDuration*2.f);
                        if (n->mtfLeader == nullptr) {
                            //If (Rand(1,35)=1) Then
                            //	RotateEntity(n\collider,0.f,Rnd(360.f),0.f,True)
                            //EndIf
                            FinishWalking(n,488,522,n->speed*26);
                            n->currSpeed = 0.f;
                        } else if ((bbEntityDistance(n->collider,n->mtfLeader->collider)>1.f)) {
                            bbPointEntity(n->collider,n->mtfLeader->collider);
                            bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);

                            n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);
                            bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                            AnimateNPC(n,488, 522, n->currSpeed*26);
                        } else {
                            //If (Rand(1,35)=1) Then
                            //	RotateEntity(n\collider,0.f,Rnd(360.f),0.f,True)
                            //EndIf
                            FinishWalking(n,488,522,n->speed*26);
                            n->currSpeed = 0.f;
                        }
                        n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                        bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);
                    }
                }

                if ((!bbEntityVisible(n->collider,Curr096->collider)) || bbEntityDistance(n->collider,Curr096->collider)>6.f) {
                    n->state = 0;
                }

                //SCP-049-2 spotted
            }
            case 9: {

                if (bbEntityVisible(n->collider, n->target->collider)) {
                    bbPointEntity(n->obj,n->target->collider);
                    bbRotateEntity(n->collider,0,CurveAngle(bbEntityYaw(n->obj),bbEntityYaw(n->collider),20.f),0);
                    n->angle = bbEntityYaw(n->collider);

                    if (bbEntityDistance(n->target->collider,n->collider)<1.3f) {
                        n->state3 = 70*2;
                    }

                    if (n->state3 > 0.f) {
                        n->pathStatus = 0;
                        n->pathLocation = 0;
                        n->speed = 0.02f;
                        n->currSpeed = CurveValue(-n->speed,n->currSpeed,20.f);
                        bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                        AnimateNPC(n,522, 488, n->currSpeed*26);

                        n->pathTimer = 1.f;

                        n->state3 = Max(n->state3-timing->tickDuration,0);
                    } else {
                        n->state3 = 0;
                        AnimateNPC(n, 346, 351, 0.2f, false);
                    }
                    if (n->reload <= 0 && n->target->isDead == false) {
                        //angle# = WrapAngle(angle - EntityYaw(n\collider))
                        //If (angle < 5 Or angle > 355) Then
                        if (abs(bbDeltaYaw(n->collider,n->target->collider))<50.f) {
                            //prev% = KillTimer

                            PlayRangedSound_SM(sndMgmt->gunshot[0], mainPlayer->cam, n->collider, 15);

                            pvt = bbCreatePivot();

                            bbRotateEntity(pvt, bbEntityPitch(n->collider), bbEntityYaw(n->collider), 0, true);
                            bbPositionEntity(pvt, bbEntityX(n->obj), bbEntityY(n->obj), bbEntityZ(n->obj));
                            bbMoveEntity(pvt,0.8f*0.079f, 10.75f*0.079f, 6.9f*0.079f);

                            p = CreateParticle(bbEntityX(pvt), bbEntityY(pvt), bbEntityZ(pvt), PARTICLE_FLASH, bbRnd(0.08f,0.1f), 0.f, 5);
                            bbTurnEntity(p->sprite, 0,0,bbRnd(360));
                            p->aChange = -0.15f;
                            if (n->target->hp > 0) {
                                n->target->hp = (int)(Max((float)(n->target->hp-bbRand(5,10)),0.f));
                            } else {
                                if (!n->target->isDead) {
                                    if (n->sounds[0] != 0) {
                                        bbFreeSound(n->sounds[0]);
                                        n->sounds[0] = 0;
                                    }
                                    n->sounds[0] = bbLoadSound("SFX/Character/MTF/049/Player0492_2.ogg");
                                    PlayMTFSound(n->sounds[0], n);
                                }
                                SetNPCFrame(n->target,133);
                                n->target->isDead = true;
                                n->state = 0;
                            }
                            n->reload = 7;

                            bbFreeEntity(pvt);
                        }
                    }
                    n->pathStatus = 0;
                } else {
                    if (n->pathTimer<=0.f) {
                        n->pathStatus = FindPath(n,bbEntityX(n->target->collider),bbEntityY(n->target->collider),bbEntityZ(n->target->collider));
                        if (n->pathStatus == 1) {
                            while (n->path[n->pathLocation]==nullptr) {
                                if (n->pathLocation>19) {
                                    break;
                                }
                                n->pathLocation = n->pathLocation+1;
                            }
                            if (n->pathLocation<19) {
                                if (n->path[n->pathLocation]!=nullptr && n->path[n->pathLocation+1]!=nullptr) {
                                    //If (n\path[n\pathLocation]\door=Null) Then ;TODO: fix
                                    if (abs(bbDeltaYaw(n->collider,n->path[n->pathLocation]->obj))>abs(bbDeltaYaw(n->collider,n->path[n->pathLocation+1]->obj))) {
                                        n->pathLocation = n->pathLocation+1;
                                    }
                                    //EndIf
                                }
                            }
                        }
                        n->pathTimer = 70*10;
                    } else {
                        if (n->pathStatus==1) {
                            if (n->path[n->pathLocation]==nullptr) {
                                if (n->pathLocation > 19) {
                                    n->pathLocation = 0;
                                    n->pathStatus = 0;
                                } else {
                                    n->pathLocation = n->pathLocation + 1;
                                }
                            } else {
                                prevDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                                bbPointEntity(n->collider,n->path[n->pathLocation]->obj);
                                bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                                n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);
                                bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);

                                n->currSpeed = CurveValue(n->speed,n->currSpeed,20.f);
                                bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90.f)*n->currSpeed * timing->tickDuration, true);
                                AnimateNPC(n,488, 522, n->currSpeed*26);

                                newDist = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                                //TODO: fix
                                //If (newDist<1.f And n\path[n\pathLocation]\door<>Null) Then
                                //	If (Not n\path[n\pathLocation]\door\open)
                                //		PlayMTFSound(MTFSFX(5),n)
                                //	EndIf
                                //	n\path[n\pathLocation]\door\open = True
                                //	If n\path[n\pathLocation]\door\mtfClose
                                //		n\path[n\pathLocation]\door\timerstate = 70.f*5.f
                                //	EndIf
                                //EndIf

                                if (newDist<0.2f || (prevDist<newDist && prevDist<1.f)) {
                                    n->pathLocation = n->pathLocation+1;
                                }
                            }
                            n->pathTimer = n->pathTimer-timing->tickDuration;
                        } else {
                            n->pathTimer = 0.f;
                        }
                    }
                }


            }
        }

        if (n->currSpeed > 0.01f) {
            if (prevFrame > 500 && n->frame<495) {
                PlayRangedSound(sndMgmt->footstepMetal[bbRand(0,7)]->internal, mainPlayer->cam, n->collider, 8.f, bbRnd(0.5f,0.7f));
            } else if ((prevFrame < 505 && n->frame>=505)) {
                PlayRangedSound(sndMgmt->footstepMetal[bbRand(0,7)]->internal, mainPlayer->cam, n->collider, 8.f, bbRnd(0.5f,0.7f));
            }
        }

        if (console->noTarget && n->state == 1) {
            n->state = 0;
        }

        if (n->state != 3 && n->state != 5 && n->state != 6 && n->state != 7) {
            if (n->mtfLeader!=nullptr) {
                if (bbEntityDistance(n->collider,n->mtfLeader->collider)<0.7f) {
                    bbPointEntity(n->collider,n->mtfLeader->collider);
                    bbRotateEntity(n->collider,0.f,bbEntityYaw(n->collider,true),0.f,true);
                    n->angle = CurveAngle(bbEntityYaw(n->collider,true),n->angle,20.f);

                    bbTranslateEntity(n->collider, bbCos(bbEntityYaw(n->collider,true)-45)* 0.01f * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)-45)* 0.01f * timing->tickDuration, true);
                }
            } else {
                for (int iterator149 = 0; iterator149 < NPC::getListSize(); iterator149++) {
                    n2 = NPC::getObject(iterator149);

                    if (n2!=n && n2->isDead==false) {
                        if (abs(bbDeltaYaw(n->collider,n2->collider))<80.f) {
                            if (bbEntityDistance(n->collider,n2->collider)<0.7f) {
                                bbTranslateEntity(n2->collider, bbCos(bbEntityYaw(n->collider,true)+90)* 0.01f * timing->tickDuration, 0, bbSin(bbEntityYaw(n->collider,true)+90)* 0.01f * timing->tickDuration, true);
                            }
                        }
                    }
                }
            }
        }

        //teleport back to the facility if fell through the floor
        if (n->state != 6 && n->state != 7) {
            if (bbEntityY(n->collider) < -10.f) {
                TeleportCloser(n);
            }
        }

        bbRotateEntity(n->obj,-90.f,n->angle,0.f,true);

        bbPositionEntity(n->obj,bbEntityX(n->collider,true),bbEntityY(n->collider,true)-0.15f,bbEntityZ(n->collider,true),true);

    }
}

void UpdateMTF() {
    if (mainPlayer->currRoom == nullptr) {
        return;
    }
    if (mainPlayer->currRoom->roomTemplate->name.equals("gateaentrance")) {
        return;
    }

    Room* r;
    NPC* n;
    NPC* leader = nullptr;
    int i;
    Room* entrance;

    //mtf ei vielä spawnannut, spawnataan jos pelaaja menee tarpeeksi lähelle gate b:tä
    if (MTFtimer == 0) {
        if (bbRand(30)==1 && !mainPlayer->currRoom->roomTemplate->name.equals("dimension1499")) {

            entrance = nullptr;
            for (int iterator150 = 0; iterator150 < Room::getListSize(); iterator150++) {
                r = Room::getObject(iterator150);
                // TODO: Probably isn't 'gateaentrance'. Infact why is there code here for gate a?
                if (r->roomTemplate->name.toLower().equals("gateaentrance")) {
                    entrance = r;
                    break;
                }
            }

            if (entrance != nullptr) {
                if (abs(bbEntityZ(entrance->obj)-bbEntityZ(mainPlayer->collider))<30.f) {
                    //If (mainPlayer\currRoomoomTemplate\name<>"room860" And mainPlayer/currRoom/RoomTemplate/Name<>"pocketdimension") Then
                    if (PlayerInReachableRoom()) {
                        PlaySound2(LoadTempSound("SFX/Character/MTF/Announc.ogg"));
                    }

                    MTFtimer = 1;

                    for (i = 0; i <= 2; i++) {
                        n = CreateNPC(NPCtypeMTF, bbEntityX(entrance->obj)+0.3f*(i-1), 1.f,bbEntityZ(entrance->obj)+8.f);

                        if (i == 0) {
                            leader = n;
                        } else {
                            n->mtfLeader = leader;
                        }

                        n->prevX = (float)i;
                    }
                }
            }
        }
    }

}

}
