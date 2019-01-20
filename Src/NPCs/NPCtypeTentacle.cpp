#include <bbblitz3d.h>
#include <bbaudio.h>
#include <bbmath.h>

#include "NPCs.h"
#include "../Menus/Menu.h"
#include "../AssetMgmt/Audio.h"
#include "../Player/Player.h"
#include "../Items/Item.h"
#include "../MathUtils/MathUtils.h"
#include "NPCtypeTentacle.h"

namespace CBN {

// Functions.
void InitializeNPCtypeTentacle(NPC* n) {
    n->nvName = "Unidentified";

    n->collider = bbCreatePivot();

    NPC* n2;
    for (int iterator151 = 0; iterator151 < NPC::getListSize(); iterator151++) {
        n2 = NPC::getObject(iterator151);

        if (n->npcType == n2->npcType && n!=n2) {
            n->obj = bbCopyMeshModelEntity(n2->obj);
            break;
        }
    }

    if (n->obj == 0) {
        n->obj = bbLoadAnimMesh("GFX/NPCs/tentacle/tentacle.b3d");
        bbScaleEntity(n->obj, 0.065f,0.065f,0.065f);
    }

    n->sounds[0] = bbLoadSound("SFX/Room/035Chamber/TentacleSpawn.ogg");
    n->sounds[1] = bbLoadSound("SFX/Room/035Chamber/TentacleAttack1.ogg");
    n->sounds[2] = bbLoadSound("SFX/Room/035Chamber/TentacleAttack2.ogg");

    bbSetAnimTime(n->obj, 283);
}

void UpdateNPCtypeTentacle(NPC* n) {
    if (n->playerDistance < 8.f) {

        switch ((int)n->state) {
            case 0: {

                if (n->frame>283) {
                    mainPlayer->heartbeatIntensity = Max(CurveValue(1.f, mainPlayer->heartbeatIntensity, 50),mainPlayer->heartbeatIntensity);
                    //HeartBeatRate = Max(CurveValue(130, HeartBeatRate, 100),HeartBeatRate)

                    bbPointEntity(n->obj, mainPlayer->collider);
                    bbRotateEntity(n->collider, 0, CurveAngle(bbEntityYaw(n->obj),bbEntityYaw(n->collider),25.f), 0);

                    AnimateNPC(n, 283, 389, 0.3f, false);
                    //Animate2(n\obj, AnimTime(n\obj), 283, 389, 0.3f, False)

                    if (n->frame>388) {
                        n->state = 1;
                    }
                } else {
                    if (n->playerDistance < 2.5f) {
                        SetNPCFrame(n, 284);
                        PlaySound2(n->sounds[0]);
                    }
                }
                //spawn 283,389
                //attack 2, 32
                //idle 33, 174
                //idle
            }
            case 1: {
                bbPointEntity(n->obj, mainPlayer->collider);
                bbRotateEntity(n->collider, 0, CurveAngle(bbEntityYaw(n->obj),bbEntityYaw(n->collider),25.f), 0);

                AnimateNPC(n, 33, 174, 0.3f, true);
                //Animate2(n\obj, AnimTime(n\obj), 33, 174, 0.3f, True)
            }
            case 2: {

                //finish the idle animation before playing the attack animation
                if (n->frame>33 && n->frame<174) {
                    AnimateNPC(n, 33, 174, 2.f, false);
                    //Animate2(n\obj, AnimTime(n\obj), 33, 174, 2.f, False)
                } else {
                    bbPointEntity(n->obj, mainPlayer->collider);
                    bbRotateEntity(n->collider, 0, CurveAngle(bbEntityYaw(n->obj),bbEntityYaw(n->collider),10.f), 0);

                    if (n->frame>33) {
                        //SetAnimTime(n\obj,2)
                        n->frame = 2;
                        PlaySound2(n->sounds[bbRand(1,2)]);
                    }
                    AnimateNPC(n, 2, 32, 0.3f, false);
                    //Animate2(n\obj, AnimTime(n\obj), 2, 32, 0.3f, False)

                    if (n->frame>=5 && n->frame<6) {
                        if (n->playerDistance < 1.8f) {
                            if (abs(bbDeltaYaw(n->collider, mainPlayer->collider))<20) {
                                mainPlayer->blurTimer = 100;
                                mainPlayer->injuries = mainPlayer->injuries+bbRnd(1.f,1.5f);
                                PlaySound2(mainPlayer->damageSFX[bbRand(3,4)]);

                                if (mainPlayer->injuries > 3.f) {
                                    //DeathMSG = "\"We will need more than the regular cleaning team to care of this. ";
                                    //DeathMSG = DeathMSG + "Two large and highly active tentacle-like appendages seem ";
                                    //DeathMSG = DeathMSG + "to have formed inside the chamber. Their level of aggression is ";
                                    //DeathMSG = DeathMSG + "unlike anything we've seen before - it looks like they have ";
                                    //DeathMSG = DeathMSG + "beaten some unfortunate Class D to death at some point during the breach.\"";
                                    mainPlayer->kill();
                                }

                            }
                        }

                        n->frame = 6;
                        //SetAnimTime(n\obj, 6)
                    } else if ((n->frame==32)) {
                        n->state = 1;
                        n->frame = 173;
                        //SetAnimTime(n\obj, 173)
                    }
                }

            }
        }

        bbPositionEntity(n->obj, bbEntityX(n->collider), bbEntityY(n->collider), bbEntityZ(n->collider));
        bbRotateEntity(n->obj, bbEntityPitch(n->collider)-90, bbEntityYaw(n->collider)-180, bbEntityRoll(n->collider), true);

        n->dropSpeed = 0;

        bbResetEntity(n->collider);

    }
}

}
