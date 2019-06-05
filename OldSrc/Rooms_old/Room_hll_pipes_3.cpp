#include <bbblitz3d.h>
#include <bbmath.h>
#include <bbaudio.h>

#include "../GameMain.h"
#include "../MapSystem.h"
#include "../Decals.h"
#include "../Particles.h"
#include "../Events.h"
#include "../Player.h"
#include "../NPCs/NPCs.h"
#include "../Audio.h"
#include "../MathUtils/MathUtils.h"
#include "Room_hll_pipes_3.h"

namespace CBN {

// Functions.
void UpdateEvent106sinkhole(Event* e) {
    if (e->eventState==0) {
        Decal* de = CreateDecal(DECAL_CORROSION, bbEntityX(e->room->obj)+bbRnd(-0.5f,0.5f), 0.01f, bbEntityZ(e->room->obj)+bbRnd(-0.5f,0.5f), 90, bbRand(360), 0);
        //
        de->size = 2.5f;
        bbScaleSprite(de->obj, de->size, de->size);

        e->eventState = 1;
    } else if ((mainPlayer->currRoom == e->room)) {
        if (e->sounds[0]==0) {
            e->sounds[0] = bbLoadSound("SFX/Room/Sinkhole.ogg");
        } else {
            e->soundChannels[0] = LoopRangedSound(e->sounds[0], e->soundChannels[0], mainPlayer->cam, e->room->obj, 4.5f, 1.5f);
        }
        float dist = Distance(bbEntityX(mainPlayer->collider),bbEntityZ(mainPlayer->collider),bbEntityX(e->room->obj),bbEntityZ(e->room->obj));
        if (dist < 2.f) {
            mainPlayer->footstepOverride = 1;
            mainPlayer->moveSpeed = CurveValue(0.f, mainPlayer->moveSpeed, Max(dist*50,1.f));
            mainPlayer->crouchState = (2.f-dist)/2.f;

            if (dist<0.5f) {
                if (e->eventState2 == 0) {
                    PlaySound2(LoadTempSound("SFX/Room/SinkholeFall.ogg"));
                }

                mainPlayer->moveSpeed = CurveValue(0.f, mainPlayer->moveSpeed, Max(dist*50,1.f));

                float x = CurveValue(bbEntityX(e->room->obj),bbEntityX(mainPlayer->collider),10.f);
                float y = CurveValue(bbEntityY(e->room->obj)-e->eventState2,bbEntityY(mainPlayer->collider),25.f);
                float z = CurveValue(bbEntityZ(e->room->obj),bbEntityZ(mainPlayer->collider),10.f);
                bbPositionEntity(mainPlayer->collider, x, y, z, true);

                mainPlayer->dropSpeed = 0;

                bbResetEntity(mainPlayer->collider);

                e->eventState2 = Min(e->eventState2+timing->tickDuration/200.f,2.f);

                //LightBlink = Min(e\eventState2*5,10.f)
                mainPlayer->blurTimer = e->eventState2*500;

                if (e->eventState2 == 2.f) {
                    MoveToPocketDimension();
                }
            }
        }
    } else {
        e->eventState2 = 0;
    }
}

}
