#include <bbblitz3d.h>
#include <bbaudio.h>
#include <bbmath.h>

#include "NPCs.h"
#include "../GameMain.h"
#include "../Menus/Menu.h"
#include "../AssetMgmt/Audio.h"
#include "../Map/MapSystem.h"
#include "../Player/Player.h"
#include "../MathUtils/MathUtils.h"
#include "NPCtypeApache.h"

namespace CBN {

// Functions.
void InitializeNPCtypeApache(NPC* n) {
    n->nvName = "Human";
    n->gravityMult = 0.f;
    n->maxGravity = 0.f;
    n->collider = bbCreatePivot();
    bbEntityRadius(n->collider, 0.2f);

    NPC* n2;
    for (int iterator136 = 0; iterator136 < NPC::getListSize(); iterator136++) {
        n2 = NPC::getObject(iterator136);

        if (n->npcType == n2->npcType && n != n2) {
            n->obj = bbCopyMeshModelEntity(n2->obj);
            n->obj2 = bbCopyMeshModelEntity(n2->obj2);
            n->obj3 = bbCopyMeshModelEntity(n2->obj3);
            break;
        }
    }

    if (n->obj == 0) {
        n->obj = bbLoadAnimMesh("GFX/NPCs/apache/apache.b3d");
        n->obj2 = bbLoadAnimMesh("GFX/NPCs/apache/apacherotor.b3d");
        n->obj3 = bbLoadAnimMesh("GFX/NPCs/apache/apacherotor2.b3d");
    }

    bbEntityParent(n->obj2, n->obj);
    bbEntityParent(n->obj3, n->obj);

    n->sounds[0] = bbLoadSound("SFX/Character/Apache/Propeller.ogg");
    n->sounds[1] = bbLoadSound("SFX/Character/Apache/Alarm.ogg");

    int i;
    MeshModel* rotor2;
    for (i = -1; i <= 1; i += 2) {
        rotor2 = bbCopyMeshModelEntity(n->obj2,n->obj2);
        bbRotateEntity(rotor2,0,4.f*i,0);
        bbEntityAlpha(rotor2, 0.5f);
    }

    bbPositionEntity(n->obj3, 0.f, 2.15f, -5.48f);

    bbEntityType(n->collider, HIT_APACHE);
    bbEntityRadius(n->collider, 3.f);

    float temp = 0.6f;
    bbScaleEntity(n->obj, temp, temp, temp);
}

void UpdateNPCtypeApache(NPC* n) {
#if 0
    float dist2;
    float dist;
    Pivot* target = nullptr;
    Pivot* pvt = nullptr;

    if (n->playerDistance<60.f) {
        if (mainPlayer->currRoom->roomTemplate->name.equals("exit1")) {
            dist2 = Max(Min(bbEntityDistance(n->collider, mainPlayer->currRoom->objects[3])/(8000.f*RoomScale),1.f),0.f);
        } else {
            dist2 = 1.f;
        }

        n->soundChannels[0] = LoopRangedSound(n->sounds[0], n->soundChannels[0], mainPlayer->cam, n->collider, 25.f, dist2);
    }

    n->dropSpeed = 0;

    switch ((int)n->state) {
        case 0:
        case 1: {
            bbTurnEntity(n->obj2,0,20.f*timing->tickDuration,0);
            bbTurnEntity(n->obj3,20.f*timing->tickDuration,0,0);

            if (n->state==1 && (!console->noTarget)) {
                if (abs(bbEntityX(mainPlayer->collider)-bbEntityX(n->collider))< 30.f) {
                    if (abs(bbEntityZ(mainPlayer->collider)-bbEntityZ(n->collider))<30.f) {
                        if (abs(bbEntityY(mainPlayer->collider)-bbEntityY(n->collider))<20.f) {
                            if (bbRand(20)==1) {
                                if (bbEntityVisible(mainPlayer->collider, n->collider)) {
                                    n->state = 2;
                                    PlayRangedSound(n->sounds[1], mainPlayer->cam, n->collider, 50, 1.f);
                                }
                            }
                        }
                    }
                }
            }
            //player located -> attack
        }
        case 2:
        case 3: {

            if (n->state == 2) {
                target = mainPlayer->collider;
            } else if ((n->state == 3)) {
                target = bbCreatePivot();
                bbPositionEntity(target, n->enemyX, n->enemyY, n->enemyZ, true);
            }

            if (console->noTarget && n->state == 2) {
                n->state = 1;
            }

            bbTurnEntity(n->obj2,0,20.f*timing->tickDuration,0);
            bbTurnEntity(n->obj3,20.f*timing->tickDuration,0,0);

            if (abs(bbEntityX(target)-bbEntityX(n->collider)) < 55.f) {
                if (abs(bbEntityZ(target)-bbEntityZ(n->collider)) < 55.f) {
                    if (abs(bbEntityY(target)-bbEntityY(n->collider))< 20.f) {
                        bbPointEntity(n->obj, target);
                        bbRotateEntity(n->collider, CurveAngle(Min(WrapAngle(bbEntityPitch(n->obj)),40.f),bbEntityPitch(n->collider),40.f), CurveAngle(bbEntityYaw(n->obj),bbEntityYaw(n->collider),90.f), bbEntityRoll(n->collider), true);
                        bbPositionEntity(n->collider, bbEntityX(n->collider), CurveValue(bbEntityY(target)+8.f,bbEntityY(n->collider),70.f), bbEntityZ(n->collider));

                        dist = Distance(bbEntityX(target),bbEntityZ(target),bbEntityX(n->collider),bbEntityZ(n->collider));

                        n->currSpeed = CurveValue(Min(dist-6.5f,6.5f)*0.008f, n->currSpeed, 50.f);

                        //If (Distance(EntityX(mainPlayer\collider),EntityZ(mainPlayer\collider),EntityX(n\collider),EntityZ(n\collider)) > 6.5f) Then
                        //	n\currspeed = CurveValue(0.08f,n\currspeed,50.f)
                        //Else
                        //	n\currspeed = CurveValue(0.f,n\currspeed,30.f)
                        //EndIf
                        bbMoveEntity(n->collider, 0,0,n->currSpeed*timing->tickDuration);


                        if (n->pathTimer == 0) {
                            n->pathStatus = bbEntityVisible(n->collider,target);
                            n->pathTimer = bbRand(100,200);
                        } else {
                            n->pathTimer = Min(n->pathTimer-timing->tickDuration,0.f);
                        }

                        //player visible
                        if (n->pathStatus == 1) {
                            bbRotateEntity(n->collider, bbEntityPitch(n->collider), bbEntityYaw(n->collider), CurveAngle(0, bbEntityRoll(n->collider),40), true);

                            if (n->reload <= 0) {
                                if (dist<20.f) {
                                    pvt = bbCreatePivot();

                                    bbPositionEntity(pvt, bbEntityX(n->collider),bbEntityY(n->collider), bbEntityZ(n->collider));
                                    bbRotateEntity(pvt, bbEntityPitch(n->collider), bbEntityYaw(n->collider),bbEntityRoll(n->collider));
                                    //2.3f
                                    bbMoveEntity(pvt, 0, 8.87f*(0.21f/9.f), 8.87f*(1.7f/9.f));
                                    bbPointEntity(pvt, target);

                                    if (WrapAngle(bbEntityYaw(pvt)-bbEntityYaw(n->collider))<10) {
                                        PlayRangedSound_SM(sndMgmt->gunshot[1], mainPlayer->cam, n->collider, 20);

                                        DeathMSG = "\"CH-2 to control. Shot down a runaway Class D at Gate B.\"";

                                        Shoot(bbEntityX(pvt),bbEntityY(pvt), bbEntityZ(pvt),((10/dist)*(1/dist))*(n->state = 2),(n->state==2));

                                        n->reload = 5;
                                    }

                                    bbFreeEntity(pvt);
                                }
                            }
                        } else {
                            bbRotateEntity(n->collider, bbEntityPitch(n->collider), bbEntityYaw(n->collider), CurveAngle(-20, bbEntityRoll(n->collider),40), true);
                        }
                        bbMoveEntity(n->collider, -bbEntityRoll(n->collider)*0.002f,0,0);

                        n->reload = n->reload-timing->tickDuration;


                    }
                }
            }

            if (n->state == 3) {
                bbFreeEntity(target);
            }
            //crash
        }
        case 4: {
            if (n->state2 < 300) {

                bbTurnEntity(n->obj2,0,20.f*timing->tickDuration,0);
                bbTurnEntity(n->obj3,20.f*timing->tickDuration,0,0);

                //Sin(TimeInPosMilliSecs()/40)*timing\tickDuration)
                bbTurnEntity(n->collider,0,-timing->tickDuration*7,0);
                n->state2 = n->state2+timing->tickDuration*0.3f;

                target = bbCreatePivot();
                bbPositionEntity(target, n->enemyX, n->enemyY, n->enemyZ, true);

                bbPointEntity(n->obj, target);
                bbMoveEntity(n->obj, 0,0,timing->tickDuration*0.001f*n->state2);
                bbPositionEntity(n->collider, bbEntityX(n->obj), bbEntityY(n->obj), bbEntityZ(n->obj));

                if (bbEntityDistance(n->obj, target) <0.3f) {
                    //TODO:
                    //                    If (TempSound2 <> 0) Then FreeSound(TempSound2
                    //TempSound2 = 0);
                    //                    TempSound2 = LoadSound("SFX/Character/Apache/Crash"+Rand(1,2)+".ogg")
                    //                    mainPlayer\camShake = Max(mainPlayer\camShake, 3.f)
                    //                    PlaySound2(TempSound2)
                    //                    n\state = 5
                }

                bbFreeEntity(target);
            }
        }
    }

    bbPositionEntity(n->obj, bbEntityX(n->collider), bbEntityY(n->collider), bbEntityZ(n->collider));
    bbRotateEntity(n->obj, bbEntityPitch(n->collider), bbEntityYaw(n->collider), bbEntityRoll(n->collider), true);
#endif
}

}
