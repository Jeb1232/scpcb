#include <bbblitz3d.h>
#include <bbmath.h>

#include "NPCs.h"
#include "../Config/INI.h"
#include "../GameMain.h"
#include "../Menus/Menu.h"
#include "../AssetMgmt/Audio.h"
#include "../Map/MapSystem.h"
#include "../Player/Player.h"
#include "../MathUtils/MathUtils.h"
#include "../Items/Item.h"
#include "NPCtype966.h"

namespace CBN {

// Functions.
void InitializeNPCtype966(NPC* n) {
    int i = 1;
    NPC* n2;
    for (int iterator133 = 0; iterator133 < NPC::getListSize(); iterator133++) {
        n2 = NPC::getObject(iterator133);

        if ((n->npcType == n2->npcType) && (n!=n2)) {
            i = i + bbRand(1,3);
        }
    }
    n->nvName = "SCP-966-"+String(i);

    n->collider = bbCreatePivot();
    bbEntityRadius(n->collider,0.2f);

    for (int iterator134 = 0; iterator134 < NPC::getListSize(); iterator134++) {
        n2 = NPC::getObject(iterator134);

        if ((n->npcType == n2->npcType) && (n!=n2)) {
            n->obj = bbCopyMeshModelEntity(n2->obj);
            break;
        }
    }

    if (n->obj == 0) {
        n->obj = bbLoadAnimMesh("GFX/NPCs/scp966/scp-966.b3d");
    }

    bbEntityFX(n->obj,1);

    float temp = getINIFloat("Data/NPCs.ini", "SCP-966", "scale")/40.f;
    bbScaleEntity(n->obj, temp, temp, temp);

    //EntityColor(n\obj,Rnd(0,50),0,Rnd(50,100))

    bbSetAnimTime(n->obj,15.f);

    bbEntityType(n->collider,HIT_PLAYER);

    n->speed = 0.02f;
}

void UpdateNPCtype966(NPC* n) {
    int prevFrame;
    float temp;
    float angle;
    float dist2;
    NPC* n2;

    if (n->playerDistance<HideDistance) {

        //n\state = the "general" state (idle/wander/attack/echo etc)
        //n\state2 = timer for doing raycasts

        prevFrame = (int)(n->frame);

        if (n->sounds[0] > 0) {
            temp = 0.5f;
            //the ambient sound gets louder when the npcs are attacking
            if (n->state > 0) {
                temp = 1.f;
            }

            n->soundChannels[0] = LoopRangedSound(n->sounds[0], n->soundChannels[0], mainPlayer->cam, mainPlayer->cam, 10.f,temp);
        }

        temp = bbRnd(-1.f,1.f);
        bbPositionEntity(n->obj,bbEntityX(n->collider,true),bbEntityY(n->collider,true)-0.2f,bbEntityZ(n->collider,true));
        bbRotateEntity(n->obj,-90.f,bbEntityYaw(n->collider),0.f);

        if (!mainPlayer->isEquipped("nvgoggles")) {
            bbHideEntity(n->obj);
            // TODO:
            // if (n->playerDistance<1 && n->reload <= 0 && MsgTimer <= 0) {
            //     switch (bbRand(6)) {
            //         case 1: {
            //             Msg = "You feel something breathing right next to you.";
            //         }
            //         case 2: {
            //             Msg = "\"It feels like something's in this room with me.\"";
            //         }
            //         case 3: {
            //             Msg = "You feel like something is here with you, but you do not see anything.";
            //         }
            //         case 4: {
            //             Msg = "\"Is my mind playing tricks on me or is there someone else here?\"";
            //         }
            //         case 5: {
            //             Msg = "You feel like something is following you.";
            //         }
            //         case 6: {
            //             Msg = "You can feel something near you, but you are unable to see it. Perhaps its time is now.";
            //         }
            //     }
            //     n->reload = 20*70;
            //     MsgTimer = 8*70;
            // }
            n->reload = n->reload - timing->tickDuration;

        } else {
            bbShowEntity(n->obj);
        }

        if (n->state3>5*70) {
            //n\state = 1
            if (n->state3<1000.f) {
                for (int iterator135 = 0; iterator135 < NPC::getListSize(); iterator135++) {
                    n2 = NPC::getObject(iterator135);

                    if (n2->npcType == n->npcType) {
                        n2->state3 = 1000.f;
                    }
                }
            }

            n->state = Max(n->state,8);
            n->state3 = 1000.f;

        }

        if (mainPlayer->stamina<10) {
            n->state3 = n->state3+timing->tickDuration;
        } else if ((n->state3 < 900.f)) {
            n->state3 = Max(n->state3-timing->tickDuration*0.2f,0.f);
        }

        if (n->state != 10) {
            n->lastSeen = 0;
        }

        switch ((int)n->state) {
            case 0: {
                if (n->frame>2300.f) {
                    AnimateNPC(n, 2391, 2416, 1.f, false);
                    if (n->frame>2415.f) {
                        SetNPCFrame(n, 201);
                    }
                } else {
                    AnimateNPC(n, 201, 1015, 1.f, false);

                    //echo/stare/walk around periodically
                    if (n->frame>1014.f) {
                        if (bbRand(3)==1 && n->playerDistance<4) {
                            n->state = (float)bbRand(1,4);
                        } else {
                            n->state = (float)bbRand(5,6);
                        }
                    }

                    //echo if player gets close
                    if (n->playerDistance<2.f) {
                        n->state = (float)bbRand(1,4);
                    }
                }

                n->currSpeed = CurveValue(0.f, n->currSpeed, 10.f);

                bbMoveEntity(n->collider,0,0,n->currSpeed);

                //echo
            }
            case 1:
            case 2: {
                if (n->state==1) {
                    AnimateNPC(n, 1015, 1180, 1.f, false);
                    if (n->frame > 1179.f) {
                        n->state = 0;
                    }
                } else {
                    AnimateNPC(n, 1180, 1379, 1.f, false);
                    if (n->frame > 1378.f) {
                        n->state = 0;
                    }
                }

                if (n->frame>1029.f && prevFrame<=1029.f || n->frame>1203.f && prevFrame<=1203.f) {
                    PlayRangedSound(LoadTempSound("SFX/SCP/966/Echo"+String(bbRand(1,3))+".ogg"), mainPlayer->cam, n->collider);
                }

                angle = bbVectorYaw(bbEntityX(mainPlayer->collider)-bbEntityX(n->collider),0,bbEntityZ(mainPlayer->collider)-bbEntityZ(n->collider));
                bbRotateEntity(n->collider,0.f,CurveAngle(angle,bbEntityYaw(n->collider),20.f),0.f);

                if (n->state3<900) {
                    mainPlayer->blurTimer = ((bbSin(TimeInPosMilliSecs()/50.f)+1.f)*200.f)/n->playerDistance;

                    if (n->playerDistance<16) {
                        mainPlayer->blinkEffect = Max(mainPlayer->blinkEffect, 1.5f);
                        //BlinkEffectTimer = 1000

                        mainPlayer->staminaEffect = 2.f;
                        //StaminaEffectTimer = 1000

                        // TODO:
                        // if (MsgTimer<=0 && mainPlayer->staminaEffect<1.5f) {
                        //     switch (bbRand(4)) {
                        //         case 1: {
                        //             Msg = "You feel exhausted.";
                        //         }
                        //         case 2: {
                        //             Msg = "\"Could really go for a nap now...\"";
                        //         }
                        //         case 3: {
                        //             Msg = "\"If I wasn't in this situation I would take a nap somewhere.\"";
                        //         }
                        //         case 4: {
                        //             Msg = "You feel restless.";
                        //         }
                        //     }

                        //     MsgTimer = 7*70;
                        // }
                    }
                }

                //stare at player
            }
            case 3:
            case 4: {
                if (n->state==3) {
                    AnimateNPC(n, 1379.f, 1692.f, 1.f, false);

                    if (n->frame>1691.f) {
                        n->state = 0;
                    }
                } else {
                    AnimateNPC(n, 1692.f, 2156.f, 1.f, false);

                    if (n->frame>2155.f) {
                        n->state = 0;
                    }
                }

                if (n->frame>1393.f && prevFrame<=1393.f || n->frame>1589.f && prevFrame<=1589.f || n->frame>2000.f && prevFrame<=2000.f) {
                    PlayRangedSound(LoadTempSound("SFX/SCP/966/Idle"+String(bbRand(1,3))+".ogg"), mainPlayer->cam, n->collider);
                }

                angle = bbVectorYaw(bbEntityX(mainPlayer->collider)-bbEntityX(n->collider),0,bbEntityZ(mainPlayer->collider)-bbEntityZ(n->collider));
                bbRotateEntity(n->collider,0.f,CurveAngle(angle,bbEntityYaw(n->collider),20.f),0.f);
                //walking or chasing
            }
            case 5:
            case 6:
            case 8: {
                //start walking
                if (n->frame<2343.f) {
                    AnimateNPC(n, 2319, 2343, 0.5f, false);
                } else {
                    AnimateNPC(n, 2343, 2391, n->currSpeed*25.f);

                    //chasing the player
                    if (n->state == 8 && n->playerDistance<32) {
                        if (n->pathTimer <= 0) {
                            n->pathStatus = FindPath (n, bbEntityX(mainPlayer->collider,true), bbEntityY(mainPlayer->collider,true), bbEntityZ(mainPlayer->collider,true));
                            n->pathTimer = 40*10;
                            n->currSpeed = 0;
                        }
                        n->pathTimer = Max(n->pathTimer-timing->tickDuration,0);

                        if (!bbEntityVisible(n->collider,mainPlayer->collider)) {
                            if (n->pathStatus == 2) {
                                n->currSpeed = 0;
                                SetNPCFrame(n,201);
                                //SetAnimTime(n\obj,15)
                            } else if ((n->pathStatus == 1)) {
                                if (n->path[n->pathLocation]==nullptr) {
                                    if (n->pathLocation > 19) {
                                        n->pathLocation = 0;
                                        n->pathStatus = 0;
                                    } else {
                                        n->pathLocation = n->pathLocation + 1;
                                    }
                                } else {
                                    n->angle = bbVectorYaw(bbEntityX(n->path[n->pathLocation]->obj,true)-bbEntityX(n->collider),0,bbEntityZ(n->path[n->pathLocation]->obj,true)-bbEntityZ(n->collider));
                                    //RotateEntity(n\collider,0.f,CurveAngle(angle,EntityYaw(n\collider),10.f),0.f)

                                    dist2 = bbEntityDistance(n->collider,n->path[n->pathLocation]->obj);

                                    if (dist2 < 0.8f) {
                                        //TODO: fix
                                        //If (n\path[n\pathLocation]\door<>Null) Then
                                        //    If (Not n\path[n\pathLocation]\door\open) Then UseDoor(n\path[n\pathLocation]\door,False)
                                        //EndIf
                                        if (n->playerDistance < 0.2f) {
                                            n->pathLocation = n->pathLocation + 1;
                                        }
                                    }

                                }
                            } else if ((n->pathStatus == 0)) {
                                n->currSpeed = CurveValue(0,n->currSpeed,10.f);
                            }
                        } else {
                            n->angle = bbVectorYaw(bbEntityX(mainPlayer->collider)-bbEntityX(n->collider),0,bbEntityZ(mainPlayer->collider)-bbEntityZ(n->collider));

                            if (n->playerDistance<1.f) {
                                n->state = 10;
                            }

                        }

                        n->currSpeed = CurveValue(n->speed,n->currSpeed,10.f);
                    } else {
                        if (TimeInPosMilliSecs() > n->state2 && n->playerDistance<16.f) {
                            bbHideEntity(n->collider);
                            bbEntityPick(n->collider, 1.5f);
                            if (bbPickedEntity() != 0) {
                                n->angle = bbEntityYaw(n->collider)+bbRnd(80,110);
                            }
                            bbShowEntity(n->collider);

                            n->state2 = TimeInPosMilliSecs()+1000.f;

                            if (bbRand(5)==1) {
                                n->state = 0;
                            }
                        }

                        n->currSpeed = CurveValue(n->speed*0.5f, n->currSpeed, 20.f);

                    }

                    bbRotateEntity(n->collider, 0, CurveAngle(n->angle,bbEntityYaw(n->collider),30.f),0);

                    bbMoveEntity(n->collider,0,0,n->currSpeed);
                }
                //attack
            }
            case 10: {
                if (n->lastSeen==0) {
                    PlayRangedSound(LoadTempSound("SFX/SCP/966/Echo"+String(bbRand(1,3))+".ogg"), mainPlayer->cam, n->collider);
                    n->lastSeen = 1;
                }

                if (n->frame>2300.f) {
                    AnimateNPC(n, 2391, 2416, 1.f, false);
                    if (n->frame>2415.f) {
                        switch (bbRand(3)) {
                            case 1: {
                                SetNPCFrame(n, 2160);
                            }
                            case 2: {
                                SetNPCFrame(n, 2192);
                            }
                            case 3: {
                                SetNPCFrame(n, 2221);
                            }
                        }

                    }
                } else {
                    if (n->frame <= 2191) {
                        AnimateNPC(n, 2160, 2191, 0.3f, false);
                        if (n->frame > 2190) {
                            n->state = 8;
                        }
                    } else if ((n->frame <= 2220)) {
                        AnimateNPC(n, 2192, 2220, 0.3f, false);
                        if (n->frame > 2219) {
                            n->state = 8;
                        }
                    } else if ((n->frame <= 2260)) {
                        AnimateNPC(n, 2221, 2260, 0.3f, false);
                        if (n->frame > 2259) {
                            n->state = 8;
                        }
                    }
                }

                if (n->playerDistance<1.f) {
                    if (n->frame>2173.f && prevFrame<=2173.f || n->frame>2203.f && prevFrame<=2203.f || n->frame>2227.f && prevFrame<=2227.f) {
                        PlayRangedSound(LoadTempSound("SFX/General/Slash"+String(bbRand(1,2))+".ogg"), mainPlayer->cam, n->collider);
                        mainPlayer->injuries = mainPlayer->injuries + bbRnd(0.5f,1.f);
                    }
                }

                n->angle = bbVectorYaw(bbEntityX(mainPlayer->collider)-bbEntityX(n->collider),0,bbEntityZ(mainPlayer->collider)-bbEntityZ(n->collider));
                bbRotateEntity(n->collider, 0, CurveAngle(n->angle,bbEntityYaw(n->collider),30.f),0);

            }
        }
    } else {
        bbHideEntity(n->obj);
        if (bbRand(600)==1) {
            TeleportCloser(n);
        }
    }
}

}
