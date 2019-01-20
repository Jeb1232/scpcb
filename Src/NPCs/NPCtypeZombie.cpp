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
#include "../Menus/Console/Console.h"
#include "../MathUtils/MathUtils.h"
#include "NPCtypeZombie.h"

namespace CBN {

// Functions.
void InitializeNPCtypeZombie(NPC* n) {
    n->nvName = "Human";
    n->collider = bbCreatePivot();
    bbEntityRadius(n->collider, 0.2f);
    bbEntityType(n->collider, HIT_PLAYER);

    NPC* n2;
    for (int iterator152 = 0; iterator152 < NPC::getListSize(); iterator152++) {
        n2 = NPC::getObject(iterator152);

        if (n->npcType == n2->npcType && n!=n2) {
            n->obj = bbCopyMeshModelEntity(n2->obj);
            break;
        }
    }

    float temp;
    if (n->obj == 0) {
        n->obj = bbLoadAnimMesh("GFX/NPCs/zombie/zombie.b3d");

        temp = (getINIFloat("Data/NPCs.ini", "SCP-049-2", "scale") / 2.5f);
        bbScaleEntity(n->obj, temp, temp, temp);

        bbMeshCullBox(n->obj, -bbMeshWidth(n->obj), -bbMeshHeight(n->obj), -bbMeshDepth(n->obj), bbMeshWidth(n->obj)*2, bbMeshHeight(n->obj)*2, bbMeshDepth(n->obj)*2);
    }

    n->speed = (getINIFloat("Data/NPCs.ini", "SCP-049-2", "speed") / 100.f);

    bbSetAnimTime(n->obj, 107);

    n->sounds[0] = bbLoadSound("SFX/SCP/049/0492Breath.ogg");

    n->hp = 100;
}

void UpdateNPCtypeZombie(NPC* n) {
    float prevFrame = n->frame;

    if (abs(bbEntityY(mainPlayer->collider)-bbEntityY(n->collider))<4.f) {
        if (!n->isDead) {
            switch ((int)n->state) {
                case 0: {
                    AnimateNPC(n, 719, 777, 0.2f, false);

                    //Animate2(n\obj, AnimTime(n\obj), 719, 777, 0.2f, False)
                    if (n->frame==777) {
                        if (bbRand(700)==1) {
                            if (bbEntityDistance(mainPlayer->collider, n->collider)<5.f) {
                                n->frame = 719;
                                //SetAnimTime(n\obj, 719)
                                if (bbRand(3)==1) {
                                    n->state = 1;
                                    n->frame = 155;
                                    //SetAnimTime(n\obj, 155)
                                }
                            }
                        }
                    }
                    //stands up
                }
                case 1: {
                    if (n->frame>=682) {
                        AnimateNPC(n, 926, 935, 0.3f, false);
                        if (n->frame == 935) {
                            n->state = 2;
                        }

                        //Animate2(n\obj, AnimTime(n\obj), 926, 935, 0.3f, False)
                        //If (AnimTime(n\obj)=935) Then n\state = 2
                    } else {
                        AnimateNPC(n, 155, 682, 1.5f, false);
                        //Animate2(n\obj, AnimTime(n\obj), 155, 682, 1.5f, False)
                    }
                    //following the player
                }
                case 2: {
                    //check if the player is visible every three seconds
                    if (n->state3 < 0) {
                        if (bbEntityDistance(mainPlayer->collider, n->collider)<5.f) {
                            if (bbEntityVisible(mainPlayer->collider, n->collider)) {
                                n->state2 = 70*5;
                            }
                        }
                        n->state3 = 70*3;
                    } else {
                        n->state3 = n->state3-timing->tickDuration;
                    }

                    //player is visible -> attack
                    if (n->state2 > 0 && !console->noTarget) {
                        n->soundChannels[0] = LoopRangedSound(n->sounds[0], n->soundChannels[0], mainPlayer->cam, n->collider, 6.f, 0.6f);

                        n->pathStatus = 0;

                        bbPointEntity(n->obj, mainPlayer->collider);
                        bbRotateEntity(n->collider, 0, CurveAngle(bbEntityYaw(n->obj), bbEntityYaw(n->collider), 30.f), 0);

                        if (n->playerDistance < 0.7f) {
                            n->state = 3;
                            if (bbRand(2)==1) {
                                n->frame = 2;
                                //SetAnimTime(n\obj, 2)
                            } else {
                                n->frame = 66;
                                //SetAnimTime(n\obj, 66)
                            }
                        } else {
                            n->currSpeed = CurveValue(n->speed, n->currSpeed, 20.f);
                            bbMoveEntity(n->collider, 0, 0, n->currSpeed * timing->tickDuration);

                            AnimateNPC(n, 936, 1017, n->currSpeed*60);

                            //Animate2(n\obj, AnimTime(n\obj), 936, 1017, n\currSpeed*60)

                        }

                        n->state2 = n->state2-timing->tickDuration;
                    } else {
                        //path found
                        if (n->pathStatus == 1) {
                            if (n->path[n->pathLocation]==nullptr) {
                                if (n->pathLocation > 19) {
                                    n->pathLocation = 0;
                                    n->pathStatus = 0;
                                } else {
                                    n->pathLocation = n->pathLocation + 1;
                                }
                            } else {
                                bbPointEntity(n->obj, n->path[n->pathLocation]->obj);

                                bbRotateEntity(n->collider, 0, CurveAngle(bbEntityYaw(n->obj), bbEntityYaw(n->collider), 30.f), 0);
                                n->currSpeed = CurveValue(n->speed, n->currSpeed, 20.f);
                                bbMoveEntity(n->collider, 0, 0, n->currSpeed * timing->tickDuration);

                                AnimateNPC(n, 936, 1017, n->currSpeed*60);
                                //Animate2(n\obj, AnimTime(n\obj), 936, 1017, n\currSpeed*60)

                                if (bbEntityDistance(n->collider,n->path[n->pathLocation]->obj) < 0.2f) {
                                    n->pathLocation = n->pathLocation + 1;
                                }
                            }
                            //no path to the player, stands still
                        } else {
                            n->currSpeed = 0;
                            AnimateNPC(n, 778, 926, 0.1f);
                            //Animate2(n\obj, AnimTime(n\obj), 778, 926, 0.1f)

                            n->pathTimer = n->pathTimer-timing->tickDuration;
                            if (n->pathTimer <= 0) {
                                n->pathStatus = FindPath(n, bbEntityX(mainPlayer->collider),bbEntityY(mainPlayer->collider)+0.1f,bbEntityZ(mainPlayer->collider));
                                n->pathTimer = n->pathTimer+70*5;
                            }
                        }
                    }
                }
                case 3: {
                    if (console->noTarget) {
                        n->state = 2;
                    }
                    if (n->frame < 66) {
                        AnimateNPC(n, 2, 65, 0.7f, false);

                        //Animate2(n\obj, AnimTime(n\obj), 2, 65, 0.7f, False)
                        if (prevFrame < 23 && n->frame>=23) {
                            PlaySound2(mainPlayer->damageSFX[bbRand(5,8)]);
                            mainPlayer->injuries = mainPlayer->injuries+bbRnd(0.4f,1.f);
                            //DeathMSG = "Subject D-9341. Cause of death: multiple lacerations and severe blunt force trauma caused by an instance of SCP-049-2.";
                        } else if ((n->frame==65)) {
                            n->state = 2;
                        }
                    } else {
                        AnimateNPC(n, 66, 132, 0.7f, false);
                        //Animate2(n\obj, AnimTime(n\obj), 66, 132, 0.7f, False)
                        if (prevFrame < 90 && n->frame>=90) {
                            PlaySound2(mainPlayer->damageSFX[bbRand(5,8)]);
                            mainPlayer->injuries = mainPlayer->injuries+bbRnd(0.4f,1.f);
                            //DeathMSG = "Subject D-9341. Cause of death: multiple lacerations and severe blunt force trauma caused by an instance of SCP-049-2.";
                        } else if ((n->frame==132)) {
                            n->state = 2;
                        }
                    }

                }
            }
        } else {
            AnimateNPC(n, 133, 157, 0.5f, false);
        }

        bbPositionEntity(n->obj, bbEntityX(n->collider), bbEntityY(n->collider) - 0.2f, bbEntityZ(n->collider));

        bbRotateEntity(n->obj, -90, bbEntityYaw(n->collider), 0);
    }
}

}
