#include <bbblitz3d.h>
#include <bbmath.h>

#include "../MapSystem.h"
#include "../Items/Item.h"
#include "../Particles.h"
#include "../Events.h"
#include "../NPCs/NPCs.h"
#include "Room_tnnl_plain_4.h"

namespace CBN {

// Functions.
void UpdateEvent_tnnl_plain_4(Event* e) {
    if (e->room->dist < 10.f && e->room->dist > 0) {
        e->room->npc[0] = CreateNPC(NPCtypeD, bbEntityX(e->room->obj,true)+1.f, 0.5f, bbEntityZ(e->room->obj,true)+1.f);
        e->room->npc[0]->texture = "GFX/npcs/body1.jpg";
        Texture* tex = bbLoadTexture(e->room->npc[0]->texture);
        bbEntityTexture(e->room->npc[0]->obj, tex);
        bbFreeTexture(tex);

        bbRotateEntity(e->room->npc[0]->collider, 0, bbEntityYaw(e->room->obj)-(bbRand(20,60)),0, true);

        SetNPCFrame(e->room->npc[0], 19);
        e->room->npc[0]->state = 8;

        //Delete e
        RemoveEvent(e);
    }
}

}
