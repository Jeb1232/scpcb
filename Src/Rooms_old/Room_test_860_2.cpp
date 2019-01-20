#include <iostream>
#include <bbblitz3d.h>
#include <bbmath.h>
#include <bbgraphics.h>

#include "Room_test_860_2.h"
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
#include "../Menus/Menu.h"

namespace CBN {

// Structs.
std::vector<Forest*> Forest::list;
Forest::Forest() {
    list.push_back(this);
}
Forest::~Forest() {
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == this) {
            list.erase(list.begin() + i);
            break;
        }
    }
}
int Forest::getListSize() {
    return list.size();
}
Forest* Forest::getObject(int index) {
    return list[index];
}

// Globals.
int LastForestID = 0;

// Functions.
void FillRoom_test_860_2(Room* r) {
    //the wooden door
    r->objects[2] = bbLoadMesh("GFX/Map/forest/door_frame.b3d");
    bbPositionEntity(r->objects[2],r->x + 184.f * RoomScale,0,r->z,true);
    bbScaleEntity(r->objects[2],45.f*RoomScale,45.f*RoomScale,80.f*RoomScale,true);
    bbEntityParent(r->objects[2],r->obj);

    r->objects[3] = bbLoadMesh("GFX/Map/forest/door.b3d");
    bbPositionEntity(r->objects[3],r->x + 112.f * RoomScale,0,r->z+0.05f,true);
    bbEntityType(r->objects[3], HIT_MAP);

    bbScaleEntity(r->objects[3],46.f*RoomScale,45.f*RoomScale,46.f*RoomScale,true);
    bbEntityParent(r->objects[3],r->obj);

    r->objects[4] = bbCopyMeshModelEntity((MeshModel*)r->objects[3]);
    bbPositionEntity(r->objects[4],r->x + 256.f * RoomScale,0,r->z-0.05f,true);
    bbRotateEntity(r->objects[4], 0,180,0);
    bbScaleEntity(r->objects[4],46.f*RoomScale,45.f*RoomScale,46.f*RoomScale,true);
    bbEntityParent(r->objects[4],r->obj);

    //doors to observation booth
    Door* d = CreateDoor(r->x + 928.f * RoomScale,0,r->z + 640.f * RoomScale,0,r,false, DOOR_TYPE_DEF, "", "ABCD");
    d = CreateDoor(r->x + 928.f * RoomScale,0,r->z - 640.f * RoomScale,0,r,true, DOOR_TYPE_DEF, "","ABCD");
    d->autoClose = false;

    //doors to the room itself
    d = CreateDoor(r->x+416.f*RoomScale,0,r->z - 640.f * RoomScale,0,r,false, DOOR_TYPE_DEF, r->roomTemplate->name);
    d = CreateDoor(r->x+416.f*RoomScale,0,r->z + 640.f * RoomScale,0,r,false, DOOR_TYPE_DEF, r->roomTemplate->name);

    //the forest
    Forest* fr = new Forest();
    r->fr = fr;
    GenForestGrid(fr);
    PlaceForest(fr,r->x,r->y+30.f,r->z,r);
    //EntityParent(fr\forest_Pivot,r\obj)

    Item* it = CreatePaper("doc860", r->x + 672.f * RoomScale, r->y + 176.f * RoomScale, r->z + 335.f * RoomScale);
    bbRotateEntity(it->collider, 0, r->angle+10, 0);
    bbEntityParent(it->collider, r->obj);

    it = CreatePaper("doc8601", r->x + 1152.f * RoomScale, r->y + 176.f * RoomScale, r->z - 384.f * RoomScale);
    bbRotateEntity(it->collider, 0, r->angle+170, 0);
    bbEntityParent(it->collider, r->obj);
}

void UpdateEvent_test_860_2(Event* e) {
    Forest* fr = e->room->fr;

    if (mainPlayer->currRoom == e->room && fr!=nullptr) {

        //Local dp.DrawPortal

        //the player is in the forest
        // FIXME
        if (e->eventState==1.f) {
            mainPlayer->footstepOverride = 2;
            e->overwriteMusic = true;

            Curr106->idle = true;

            //ShowEntity(fr\detailEntities[0])
            //ShowEntity(fr\detailEntities[1])

            UpdateForest(fr, mainPlayer->collider);

            if (!e->loaded) {
                e->musicTrack = MUS_8601;
                //If (Music(12) = 0) Then Music(12) = LoadSound("SFX/Music/8601Cancer.ogg") ;TODO: fix
                if (e->room->npc[0]==nullptr) {
                    e->room->npc[0] = CreateNPC(NPCtype860, 0,0,0);
                }

                e->loaded = true;
            }

            if (e->room->npc[0]!=nullptr) {
                //the monster is chasing the player
                if (e->room->npc[0]->state2 == 1 && e->room->npc[0]->state>1) {
                    e->overwriteMusic = false;
                    musicMgmt->setNextMusicTrack(MUS_8602, false);
                } else {
                    e->overwriteMusic = true;
                }
            }

            //the player fell
            if (bbEntityY(mainPlayer->collider)<=28.5f) {
                Kill(mainPlayer);
                mainPlayer->blinkTimer = -2;
            } else if ((bbEntityY(mainPlayer->collider)>bbEntityY(fr->forest_Pivot,true)+0.5f)) {
                bbMoveEntity(mainPlayer->collider, 0, ((bbEntityY(fr->forest_Pivot,true)+0.5f) - bbEntityY(mainPlayer->collider))*timing->tickDuration, 0);
            }

            if (e->room->npc[0]!=nullptr) {
                if (e->room->npc[0]->state == 0 || bbEntityDistance(mainPlayer->collider, e->room->npc[0]->collider)>12.f) {
                    e->eventState3 = e->eventState3 + (1+mainPlayer->moveSpeed)* timing->tickDuration;
                    if (modFloat(e->eventState3, 500) < 10.f && modFloat(e->eventState3-timing->tickDuration, 500) > 490.f) {
                        //If (e\eventState3 > 3500 And Rnd(10000)<e\eventState3) Then
                        if (e->eventState3 > 3000 && bbRnd(10000) < e->eventState3) {
                            e->room->npc[0]->state = 2;
                            bbPositionEntity(e->room->npc[0]->collider, 0,-110,0);
                            //e\eventState3=e\eventState3-Rnd(2000,3000)
                            e->eventState3 = e->eventState3-bbRnd(1000,2000);
                            std::cout << "attack";
                        } else {
                            e->room->npc[0]->state = 1;
                            bbPositionEntity(e->room->npc[0]->collider, 0,-110,0);
                            std::cout << "spawn";
                        }
                    }
                }
            }

            //If (KeyHit(25)) Then
            //	e\room\npc[0]\state=2
            //	PositionEntity(e\room\npc[0]\collider, 0,-110,0)
            //	e\eventState3=e\eventState3-Rnd(2000,3000)
            //EndIf

            for (int i = 0; i < 2; i++) {
                if (bbEntityDistance(fr->door[i], mainPlayer->collider)<0.5f) {
                    if (bbEntityInView(fr->door[i], mainPlayer->cam)) {
                        DrawHandIcon = true;
                        if (MouseHit1) {
                            if (i==1) {
                                mainPlayer->blinkTimer = -10;

                                PlaySound2(LoadTempSound("SFX/Door/WoodenDoorOpen.ogg"));

                                bbRotateEntity(e->room->objects[3], 0, 0, 0);
                                bbRotateEntity(e->room->objects[4], 0, 180, 0);
                                //SetAnimTime(e\room\objects[3], 0.f)
                                //SetAnimTime(e\room\objects[4], 0.f)

                                //dp.DrawPortal=e\room\dp;Object.DrawPortal(e\room\objects[0])
                                bbPositionEntity(mainPlayer->collider, bbEntityX(e->room->objects[2],true),0.5f,bbEntityZ(e->room->objects[2],true));

                                bbRotateEntity(mainPlayer->collider, 0, bbEntityYaw(e->room->obj,true)+e->eventState2*180, 0);
                                bbMoveEntity(mainPlayer->collider, 0,0,1.5f);

                                bbResetEntity(mainPlayer->collider);

                                UpdateDoorsTimer = 0;
                                UpdateDoors();

                                e->eventState = 0.f;

                            } else {
                                PlaySound2(LoadTempSound("SFX/Door/WoodenDoorBudge.ogg"));
                                Msg = "The door will not budge.";
                                MsgTimer = 70*5;
                            }
                        }
                    }
                }
            }

            int x;
            if (e->room->npc[0]!=nullptr) {
                x = Max(1.f-(e->room->npc[0]->state3/300.f),0.1f);
            } else {
                x = 2.f;
            }

            bbCameraClsColor(mainPlayer->cam,98*x,133*x,162*x);
            bbCameraRange(mainPlayer->cam,RoomScale,8.5f);
            bbCameraFogRange(mainPlayer->cam,0.5f,8.f);
            bbCameraFogColor(mainPlayer->cam,98*x,133*x,162*x);

        } else {

            if (!Contained106) {
                Curr106->idle = false;
            }

            //dp.DrawPortal=e\room\dp;Object.DrawPortal(e\room\objects[0])

            //HideEntity(fr\detailEntities[0])
            //HideEntity(fr\detailEntities[1])

            if (bbEntityYaw(e->room->objects[3])==0.f) {
                bbHideEntity(fr->forest_Pivot);
                if (abs(Distance(bbEntityX(e->room->objects[3],true),bbEntityZ(e->room->objects[3],true),bbEntityX(mainPlayer->collider,true),bbEntityZ(mainPlayer->collider,true)))<1.f) {
                    DrawHandIcon = true;

                    if (mainPlayer->selectedItem == nullptr) {
                        if (MouseHit1) {
                            PlaySound2(LoadTempSound("SFX/Door/WoodenDoorBudge.ogg"));
                            Msg = "The door will not budge.";
                            MsgTimer = 5*70;
                        }
                    } else if (mainPlayer->selectedItem->itemTemplate->name.equals("scp860")) {
                        if (MouseHit1) {
                            PlaySound2(LoadTempSound("SFX/Door/WoodenDoorOpen.ogg"));
                            bbShowEntity(fr->forest_Pivot);
                            mainPlayer->selectedItem = nullptr;

                            mainPlayer->blinkTimer = -10;

                            e->eventState = 1.f;

                            bbPositionEntity(mainPlayer->collider,bbEntityX(fr->door[0],true),bbEntityY(fr->door[0],true)+bbEntityY(mainPlayer->collider,true)+0.5f,bbEntityZ(fr->door[0],true),true);

                            bbRotateEntity(mainPlayer->collider, 0.f, bbEntityYaw(fr->door[0],true)-180, 0.f, true);
                            bbMoveEntity(mainPlayer->collider, -0.5f,0.f,0.5f);



                            Pivot* pvt = bbCreatePivot();
                            bbPositionEntity(pvt, bbEntityX(mainPlayer->cam),bbEntityY(mainPlayer->cam),bbEntityZ(mainPlayer->cam));
                            bbPointEntity(pvt, e->room->obj);
                            float ang = WrapAngle(bbEntityYaw(pvt)-bbEntityYaw(e->room->obj,true));
                            if (ang > 90 && ang < 270) {
                                //TurnEntity(mainPlayer\collider,0,180+90,0,True)
                                e->eventState2 = 1;
                            } else {
                                bbTurnEntity(mainPlayer->collider,0,90,0,true);
                                //RotateEntity(mainPlayer\collider,0,EntityYaw(fr\door[0],True)+EntityYaw(mainPlayer\collider)-EntityYaw(dp\portal,True),0,True)
                                e->eventState2 = 0;
                            }
                            bbFreeEntity(pvt);

                            bbResetEntity(mainPlayer->collider);



                            //RotateEntity(e\room\objects[3], 0, 0.5f, 0)
                            //RotateEntity(e\room\objects[3], 0, 359.5f, 0)
                        }
                    }
                }
            }

        }

    } else {
        if (fr==nullptr) {
            RemoveEvent(e);
        } else {
            if (fr->forest_Pivot!=0) {
                bbHideEntity(fr->forest_Pivot);
            }
        }
    }



}

int move_forward(int dir, int pathx, int pathy, int retval) {
    //move 1 unit along the grid in the designated direction
    if (dir == 1) {
        if (retval==0) {
            return pathx;
        } else {
            return pathy+1;
        }
    }
    if (retval==0) {
        return pathx-1+dir;
    } else {
        return pathy;
    }
}

int chance(int chanc) {
    //perform a chance given a probability
    return bbRand(0,100) <= chanc;
}

int turn_if_deviating(int max_deviation_distance_, int pathx, int center_, int dir, int retval) {
    //check if deviating and return the answer. if deviating, turn around
    int current_deviation = center_ - pathx;
    int deviated = false;
    if ((dir == 0 && current_deviation >= max_deviation_distance_) || (dir == 2 && current_deviation <= -max_deviation_distance_)) {
        dir = (dir + 2) % 4;
        deviated = true;
    }
    if (retval==0) {
        return dir;
    } else {
        return deviated;
    }
}

void GenForestGrid(Forest* fr) {
    fr->id = LastForestID+1;
    LastForestID = LastForestID+1;

    int door1_pos;
    int door2_pos;
    int i;
    int j;
    int n;
    door1_pos = bbRand(3,7);
    door2_pos = bbRand(3,7);

    //clear the grid
    for (i = 0; i < gridsize; i++) {
        for (j = 0; j < gridsize; j++) {
            fr->grid[(j*gridsize)+i] = 0;
        }
    }

    //set the position of the concrete and doors
    //For i=0 To gridsize-1
    //	fr\grid[i]=2
    //	fr\grid[((gridsize-1)*gridsize)+i]=2
    //Next
    fr->grid[door1_pos] = 3;
    fr->grid[((gridsize-1)*gridsize)+door2_pos] = 3;

    //generate the path
    int pathx = door2_pos;
    int pathy = 1;
    //0 = left, 1 = up, 2 = right
    int dir = 1;
    fr->grid[((gridsize-1-pathy)*gridsize)+pathx] = 1;

    int deviated;

    while (pathy < gridsize -4) {
        //determine whether to go forward or to the side
        if (dir == 1) {
            if (chance(deviation_chance)) {
                //pick a branch direction
                dir = 2 * bbRand(0,1);
                //make sure you have not passed max side distance
                dir = turn_if_deviating(max_deviation_distance,pathx,center,dir);
                deviated = turn_if_deviating(max_deviation_distance,pathx,center,dir,1);
                if (deviated) {
                    fr->grid[((gridsize-1-pathy)*gridsize)+pathx] = 1;
                }
                pathx = move_forward(dir,pathx,pathy);
                pathy = move_forward(dir,pathx,pathy,1);
            }

        } else {
            //we are going to the side, so determine whether to keep going or go forward again
            dir = turn_if_deviating(max_deviation_distance,pathx,center,dir);
            deviated = turn_if_deviating(max_deviation_distance,pathx,center,dir,1);
            if (deviated || chance(return_chance)) {
                dir = 1;
            }

            pathx = move_forward(dir,pathx,pathy);
            pathy = move_forward(dir,pathx,pathy,1);
            //if we just started going forward go twice so as to avoid creating a potential 2x2 line
            if (dir==1) {
                fr->grid[((gridsize-1-pathy)*gridsize)+pathx] = 1;
                pathx = move_forward(dir,pathx,pathy);
                pathy = move_forward(dir,pathx,pathy,1);
            }
        }

        //add our position to the grid
        fr->grid[((gridsize-1-pathy)*gridsize)+pathx] = 1;

    }
    //finally, bring the path back to the door now that we have reached the end
    dir = 1;
    while (pathy < gridsize-2) {
        pathx = move_forward(dir,pathx,pathy);
        pathy = move_forward(dir,pathx,pathy,1);
        fr->grid[((gridsize-1-pathy)*gridsize)+pathx] = 1;
    }

    if (pathx!=door1_pos) {
        dir = 0;
        if (door1_pos>pathx) {
            dir = 2;
        }
        while (pathx!=door1_pos) {
            pathx = move_forward(dir,pathx,pathy);
            pathy = move_forward(dir,pathx,pathy,1);
            fr->grid[((gridsize-1-pathy)*gridsize)+pathx] = 1;
        }
    }

    //attempt to create new branches
    int new_y;
    int temp_y;
    int new_x;
    int branch_type;
    int branch_pos;
    int leftmost;
    int rightmost;
    //used for counting off; branches will only be considered once every 4 units so as to avoid potentially too many branches
    new_y = -3;
    while (new_y<gridsize-6) {
        new_y = new_y+4;
        temp_y = new_y;
        new_x = 0;
        if (chance(branch_chance)) {
            branch_type = -1;
            //If (chance(cobble_chance)) Then
            //	branch_type=-2
            //EndIf
            //create a branch at this spot
            //determine if on left or on right
            branch_pos = 2*bbRand(0,1);
            //get leftmost or rightmost path in this row
            leftmost = gridsize;
            rightmost = 0;
            for (i = 0; i <= gridsize; i++) {
                if (fr->grid[((gridsize-1-new_y)*gridsize)+i]==1) {
                    if (i<leftmost) {
                        leftmost = i;
                    }
                    if (i>rightmost) {
                        rightmost = i;
                    }
                }
            }
            if (branch_pos==0) {
                new_x = leftmost-1;
            } else {
                new_x = rightmost+1;
            }
            //before creating a branch make sure there are no 1's above or below
            if ((temp_y!=0 && fr->grid[((gridsize-1-temp_y+1)*gridsize)+new_x]==1) || fr->grid[((gridsize-1-temp_y-1)*gridsize)+new_x]==1) {
                //break simply to stop creating the branch
                break;
            }
            //make 4s so you don't confuse your branch for a path; will be changed later
            fr->grid[((gridsize-1-temp_y)*gridsize)+new_x] = branch_type;
            if (branch_pos==0) {
                new_x = leftmost-2;
            } else {
                new_x = rightmost+2;
            }
            //branch out twice to avoid creating an unwanted 2x2 path with the real path
            fr->grid[((gridsize-1-temp_y)*gridsize)+new_x] = branch_type;
            i = 2;
            while (i<branch_max_life) {
                i = i+1;
                if (chance(branch_die_chance)) {
                    break;
                }
                //have a higher chance to go up to confuse the player
                if (bbRand(0,3)==0) {
                    if (branch_pos == 0) {
                        new_x = new_x-1;
                    } else {
                        new_x = new_x+1;
                    }
                } else {
                    temp_y = temp_y+1;
                }

                //before creating a branch make sure there are no 1's above or below
                n = ((gridsize - 1 - temp_y + 1)*gridsize)+new_x;
                if (n < gridsize-1) {
                    if (temp_y != 0 && fr->grid[n]==1) {
                        break;
                    }
                }
                n = ((gridsize - 1 - temp_y - 1)*gridsize)+new_x;
                if (n>0) {
                    if (fr->grid[n]==1) {
                        break;
                    }
                }

                //If ((temp_y <> 0 And fr\grid[((gridsize - 1 - temp_y + 1)*gridsize)+new_x]=1) Or fr\grid[((gridsize - 1 - temp_y - 1)*gridsize)+new_x] = 1) Then
                //	Exit
                //EndIf
                //make 4s so you don't confuse your branch for a path; will be changed later
                fr->grid[((gridsize-1-temp_y)*gridsize)+new_x] = branch_type;
                if (temp_y>=gridsize-2) {
                    break;
                }
            }
        }
    }

    //change branches from 4s to 1s (they were 4s so that they didn't accidently create a 2x2 path unintentionally)
    for (i = 0; i <= gridsize-1; i++) {
        for (j = 0; j <= gridsize-1; j++) {
            if (fr->grid[(i*gridsize)+j]==-1) {
                fr->grid[(i*gridsize)+j] = 1;
            } else if ((fr->grid[(i*gridsize)+j]==-2)) {
                fr->grid[(i*gridsize)+j] = 1;
                //ElseIf fr\grid[(i*gridsize)+j]=0

            }
        }
    }

}

void PlaceForest(Forest* fr, float x, float y, float z, Room* r) {
    //local variables
    int tx;
    int ty;
    float tile_size = 12.f;
    int tile_type;
    MeshModel* tile_entity = nullptr;
    MeshModel* detail_entity = nullptr;

    float tempf1;
    float tempf2;
    float tempf3;
    int i;
    int angle;
    int itemPlaced[4];
    Item* it;
    int width;
    float tempf4;
    int lx;
    int ly;
    MeshModel* d;
    MeshModel* frame;

    if (fr->forest_Pivot!=0) {
        bbFreeEntity(fr->forest_Pivot);
        fr->forest_Pivot = 0;
    }
    for (i = 0; i < 4; i++) {
        if (fr->tileMesh[i]!=0) {
            bbFreeEntity(fr->tileMesh[i]);
            fr->tileMesh[i] = 0;
        }
    }
    for (i = 0; i < 5; i++) {
        if (fr->detailMesh[i]!=0) {
            bbFreeEntity(fr->detailMesh[i]);
            fr->detailMesh[i] = 0;
        }
    }

    fr->forest_Pivot = bbCreatePivot();
    bbPositionEntity(fr->forest_Pivot,x,y,z,true);

    //load assets

    bbImage* hmap[ROOM4];
    Texture* mask[ROOM4];
    Texture* GroundTexture = bbLoadTexture("GFX/Map/forest/forestfloor.jpg");
    //TextureBlend(GroundTexture, FE_ALPHACURRENT)
    Texture* PathTexture = bbLoadTexture("GFX/Map/forest/forestpath.jpg");
    //TextureBlend(PathTexture, FE_ALPHACURRENT)

    hmap[ROOM1] = bbLoadImage("GFX/Map/forest/forest1h.png");
    mask[ROOM1] = bbLoadTexture("GFX/Map/forest/forest1h_mask.png",1+2);

    hmap[ROOM2] = bbLoadImage("GFX/Map/forest/forest2h.png");
    mask[ROOM2] = bbLoadTexture("GFX/Map/forest/forest2h_mask.png",1+2);

    hmap[ROOM2C] = bbLoadImage("GFX/Map/forest/forest2Ch.png");
    mask[ROOM2C] = bbLoadTexture("GFX/Map/forest/forest2Ch_mask.png",1+2);

    hmap[ROOM3] = bbLoadImage("GFX/Map/forest/forest3h.png");
    mask[ROOM3] = bbLoadTexture("GFX/Map/forest/forest3h_mask.png",1+2);

    hmap[ROOM4] = bbLoadImage("GFX/Map/forest/forest4h.png");
    mask[ROOM4] = bbLoadTexture("GFX/Map/forest/forest4h_mask.png",1+2);

    for (i = ROOM1; i <= ROOM4; i++) {
        //TextureBlend(mask[i], FE_ALPHAMODULATE)

        fr->tileMesh[i] = load_terrain(hmap[i],0.03f,GroundTexture,PathTexture,mask[i]);
    }

    //detail meshes
    //fr\detailMesh[0]=LoadMesh("GFX/Map/forest/detail/860_1_tree1.b3d")
    //fr\detailMesh[1]=LoadMesh("GFX/Map/forest/detail/860_1_tree1_leaves.b3d")
    //1.b3d)
    fr->detailMesh[1] = bbLoadMesh("GFX/Map/forest/detail/treetest4.b3d");
    //EntityParent(fr\detailMesh[1],fr\detailMesh[0])
    fr->detailMesh[2] = bbLoadMesh("GFX/Map/forest/detail/rock.b3d");
    fr->detailMesh[3] = bbLoadMesh("GFX/Map/forest/detail/rock2.b3d");
    fr->detailMesh[4] = bbLoadMesh("GFX/Map/forest/detail/treetest5.b3d");
    fr->detailMesh[5] = bbLoadMesh("GFX/Map/forest/wall.b3d");

    for (i = ROOM1; i <= ROOM4; i++) {
        bbHideEntity(fr->tileMesh[i]);
    }
    for (i = 1; i <= 5; i++) {
        bbHideEntity(fr->detailMesh[i]);
    }

    tempf3 = bbMeshWidth(fr->tileMesh[ROOM1]);
    tempf1 = tile_size/tempf3;

    for (tx = 1; tx <= gridsize-1; tx++) {
        for (ty = 1; ty <= gridsize-1; ty++) {
            if (fr->grid[(ty*gridsize)+tx]==1) {

                tile_type = 0;
                if (tx+1<gridsize) {
                    tile_type = (fr->grid[(ty*gridsize)+tx+1]>0);
                }
                if (tx-1>=0) {
                    tile_type = tile_type+(fr->grid[(ty*gridsize)+tx-1]>0);
                }

                if (ty+1<gridsize) {
                    tile_type = tile_type+(fr->grid[((ty+1)*gridsize)+tx]>0);
                }
                if (ty-1>=0) {
                    tile_type = tile_type+(fr->grid[((ty-1)*gridsize)+tx]>0);
                }

                //fr\grid[(ty*gridsize)+tx]=tile_type

                angle = 0;
                switch (tile_type) {
                    case 1: {
                        tile_entity = bbCopyMeshModelEntity(fr->tileMesh[ROOM1]);

                        if (fr->grid[((ty+1)*gridsize)+tx]>0) {
                            angle = 180;
                        } else if ((fr->grid[(ty*gridsize)+tx-1]>0)) {
                            angle = 270;
                        } else if ((fr->grid[(ty*gridsize)+tx+1]>0)) {
                            angle = 90;
                        }

                        tile_type = ROOM1;
                    } break;
                    case 2: {
                        if (fr->grid[((ty-1)*gridsize)+tx]>0 && fr->grid[((ty+1)*gridsize)+tx]>0) {
                            tile_entity = bbCopyMeshModelEntity(fr->tileMesh[ROOM2]);
                            tile_type = ROOM2;
                        } else if ((fr->grid[(ty*gridsize)+tx+1]>0 && fr->grid[(ty*gridsize)+tx-1]>0)) {
                            tile_entity = bbCopyMeshModelEntity(fr->tileMesh[ROOM2]);
                            angle = 90;
                            tile_type = ROOM2;
                        } else {
                            tile_entity = bbCopyMeshModelEntity(fr->tileMesh[ROOM2C]);
                            if (fr->grid[(ty*gridsize)+tx-1]>0 && fr->grid[((ty+1)*gridsize)+tx]>0) {
                                angle = 180;
                            } else if ((fr->grid[(ty*gridsize)+tx+1]>0 && fr->grid[((ty-1)*gridsize)+tx]>0)) {

                            } else if ((fr->grid[(ty*gridsize)+tx-1]>0 && fr->grid[((ty-1)*gridsize)+tx]>0)) {
                                angle = 270;
                            } else {
                                angle = 90;
                            }
                            tile_type = ROOM2C;
                        }
                    } break;
                    case 3: {
                        tile_entity = bbCopyMeshModelEntity(fr->tileMesh[ROOM3]);

                        if (fr->grid[((ty-1)*gridsize)+tx]==0) {
                            angle = 180;
                        } else if ((fr->grid[(ty*gridsize)+tx-1]==0)) {
                            angle = 90;
                        } else if ((fr->grid[(ty*gridsize)+tx+1]==0)) {
                            angle = 270;
                        }

                        tile_type = ROOM3;
                    } break;
                    case 4: {
                        tile_entity = bbCopyMeshModelEntity(fr->tileMesh[ROOM4]);
                        tile_type = ROOM4;
                    } break;
                    default: {
                        std::cout << "tile_type: "+String(tile_type);
                    } break;
                }

                if (tile_type > 0) {
                    //2, 5, 8
                    it = nullptr;
                    if ((ty % 3)==2 && itemPlaced[(int)(bbFloor(ty/3))]==false) {
                        itemPlaced[(int)(bbFloor(ty/3))] = true;
                        //TODO: Rename the files.
                        //it.Item = CreateItem("Log #"+(int)(Floor(ty/3)+1), "paper", 0,0.5f,0)
                        bbEntityType(it->collider, HIT_ITEM);
                        bbEntityParent(it->collider, tile_entity);
                    }

                    //place trees and other details
                    //only placed on spots where the value of the heightmap is above 100
                    bbSetBuffer(bbImageBuffer(hmap[tile_type]));
                    width = bbImageWidth(hmap[tile_type]);
                    tempf4 = (tempf3/(float)(width));
                    for (lx = 3; lx <= width-2; lx++) {
                        for (ly = 3; ly <= width-2; ly++) {
                            bbGetColor(lx,width-ly);

                            if (bbColorRed()>bbRand(100,260)) {
                                switch (bbRand(0,7)) {
                                    case 0:
                                    case 1:
                                    case 2:
                                    case 3:
                                    case 4:
                                    case 5:
                                    case 6: {
                                        detail_entity = bbCopyMeshModelEntity(fr->detailMesh[1]);
                                        //EntityType(detail_entity,HIT_MAP)
                                        tempf2 = bbRnd(0.25f,0.4f);

                                        for (i = 0; i <= 3; i++) {
                                            d = bbCopyMeshModelEntity(fr->detailMesh[4]);
                                            //ScaleEntity(d,tempf2*1.1f,tempf2,tempf2*1.1f,True)
                                            bbRotateEntity(d, 0, 90*i+bbRnd(-20,20), 0);
                                            bbEntityParent(d,detail_entity);

                                            //+8
                                            bbEntityFX(d, 1);
                                        }

                                        bbScaleEntity(detail_entity,tempf2*1.1f,tempf2,tempf2*1.1f,true);
                                        bbPositionEntity(detail_entity,lx*tempf4-(tempf3/2.f),bbColorRed()*0.03f-bbRnd(3.f,3.2f),ly*tempf4-(tempf3/2.f),true);

                                        bbRotateEntity(detail_entity,bbRnd(-5,5),bbRnd(360.f),0.f,true);

                                        //EntityAutoFade(detail_entity,4.f,6.f)
                                        //add a rock
                                    } break;
                                    case 7: {
                                        detail_entity = bbCopyMeshModelEntity(fr->detailMesh[2]);
                                        //EntityType(detail_entity,HIT_MAP)
                                        tempf2 = bbRnd(0.01f,0.012f);
                                        //ScaleEntity(detail_entity,tempf2,tempf2*Rnd(1.f,2.f),tempf2,True)

                                        bbPositionEntity(detail_entity,lx*tempf4-(tempf3/2.f),bbColorRed()*0.03f-1.3f,ly*tempf4-(tempf3/2.f),true);

                                        bbEntityFX(detail_entity, 1);

                                        bbRotateEntity(detail_entity,0.f,bbRnd(360.f),0.f,true);
                                        //add a stump
                                    } break;
                                    /*case 6: {
                                        detail_entity = bbCopyMeshModelEntity(fr->detailMesh[4]);
                                        //EntityType(detail_entity,HIT_MAP)
                                        tempf2 = bbRnd(0.1f,0.12f);
                                        bbScaleEntity(detail_entity,tempf2,tempf2,tempf2,true);

                                        bbPositionEntity(detail_entity,lx*tempf4-(tempf3/2.f),bbColorRed()*0.03f-1.3f,ly*tempf4-(tempf3/2.f),true);
                                    }*/
                                }

                                bbEntityFX(detail_entity, 1);
                                //PositionEntity(detail_entity,Rnd(0.f,tempf3)-(tempf3/2.f),ColorRed()*0.03f-0.05f,Rnd(0.f,tempf3)-(tempf3/2.f),True)
                                bbEntityParent(detail_entity,tile_entity);
                            }
                        }
                    }
                    bbSetBuffer(bbBackBuffer());

                    bbTurnEntity(tile_entity, 0, angle, 0);

                    bbPositionEntity(tile_entity,x+(tx*tile_size),y,z+(ty*tile_size),true);

                    bbScaleEntity(tile_entity,tempf1,tempf1,tempf1);
                    bbEntityType(tile_entity,HIT_MAP);
                    bbEntityFX(tile_entity,1);
                    bbEntityParent(tile_entity,fr->forest_Pivot);

                    if (it!=nullptr) {
                        bbEntityParent(it->collider,0);
                    }

                    fr->tileEntities[tx+(ty*gridsize)] = tile_entity;
                } else {
                    std::cout << "INVALID TILE @ ("+String(tx)+", "+String(ty)+ "): "+String(tile_type);
                }
            }

        }
    }

    //place the wall
    for (i = 0; i <= 1; i++) {
        ty = ((gridsize-1)*i);

        for (tx = 1; tx <= gridsize-1; tx++) {
            if (fr->grid[(ty*gridsize)+tx]==3) {
                fr->detailEntities[i] = bbCopyMeshModelEntity(fr->detailMesh[5]);
                bbScaleEntity(fr->detailEntities[i],RoomScale,RoomScale,RoomScale);

                fr->door[i] = bbCopyMeshModelEntity((MeshModel*)r->objects[3]);
                bbPositionEntity(fr->door[i],72*RoomScale,32.f*RoomScale,0,true);
                bbRotateEntity(fr->door[i], 0,180,0);
                bbScaleEntity(fr->door[i],48*RoomScale,45*RoomScale,48*RoomScale,true);
                bbEntityParent(fr->door[i],fr->detailEntities[i]);
                //SetAnimTime(fr\door[i], 0)

                frame = bbCopyMeshModelEntity((MeshModel*)r->objects[2],fr->door[i]);
                bbPositionEntity(frame,0,32.f*RoomScale,0,true);
                bbScaleEntity(frame,48*RoomScale,45*RoomScale,48*RoomScale,true);
                bbEntityParent(frame,fr->detailEntities[i]);

                bbEntityType(fr->detailEntities[i],HIT_MAP);
                //EntityParent(frame,fr\detailEntities[i])

                bbPositionEntity(fr->detailEntities[i],x+(tx*tile_size),y,z+(ty*tile_size)+(tile_size/2)-(tile_size*i),true);
                bbRotateEntity(fr->detailEntities[i],0,180*i,0);

                bbEntityParent(fr->detailEntities[i],fr->forest_Pivot);
            }
        }
    }

}

void DestroyForest(Forest* fr) {
    int tx;
    int ty;
    for (tx = 0; tx <= gridsize-1; tx++) {
        for (ty = 0; ty <= gridsize-1; ty++) {
            if (fr->tileEntities[tx+(ty*gridsize)]!=0) {
                bbFreeEntity(fr->tileEntities[tx+(ty*gridsize)]);
                fr->tileEntities[tx+(ty*gridsize)] = 0;
                fr->grid[tx+(ty*gridsize)] = 0;
            }
        }
    }
    if (fr->door[0]!=0) {
        bbFreeEntity(fr->door[0]);
        fr->door[0] = 0;
    }
    if (fr->door[1]!=0) {
        bbFreeEntity(fr->door[1]);
        fr->door[1] = 0;
    }
    if (fr->detailEntities[0]!=0) {
        bbFreeEntity(fr->detailEntities[0]);
        fr->detailEntities[0] = 0;
    }
    if (fr->detailEntities[1]!=0) {
        bbFreeEntity(fr->detailEntities[1]);
        fr->detailEntities[1] = 0;
    }

    if (fr->forest_Pivot!=0) {
        bbFreeEntity(fr->forest_Pivot);
        fr->forest_Pivot = 0;
    }
    int i;
    for (i = 0; i <= 3; i++) {
        if (fr->tileMesh[i]!=0) {
            bbFreeEntity(fr->tileMesh[i]);
            fr->tileMesh[i] = 0;
        }
    }
    for (i = 0; i <= 4; i++) {
        if (fr->detailMesh[i]!=0) {
            bbFreeEntity(fr->detailMesh[i]);
            fr->detailMesh[i] = 0;
        }
    }

    //Delete fr
}

void UpdateForest(Forest* fr, Object* ent) {
    //local variables
    int tx;
    int ty;
    if (abs(bbEntityY(ent,true)-bbEntityY(fr->forest_Pivot,true))<12.f) {
        for (tx = 0; tx <= gridsize-1; tx++) {
            for (ty = 0; ty <= gridsize-1; ty++) {
                if (fr->tileEntities[tx+(ty*gridsize)]!=0) {
                    if (abs(bbEntityX(ent,true)-bbEntityX(fr->tileEntities[tx+(ty*gridsize)],true))<20.f) {
                        if (abs(bbEntityZ(ent,true)-bbEntityZ(fr->tileEntities[tx+(ty*gridsize)],true))<20.f) {
                            bbShowEntity(fr->tileEntities[tx+(ty*gridsize)]);
                        } else {
                            bbHideEntity(fr->tileEntities[tx+(ty*gridsize)]);
                        }
                    } else {
                        bbHideEntity(fr->tileEntities[tx+(ty*gridsize)]);
                    }
                }
            }
        }
    }
}

MeshModel* load_terrain(bbImage* hmap, float yscale, Texture* t1, Texture* t2, Texture* mask) {
    float maskX;
    float maskY;
    int RGB1;
    int r;
    float alpha;

    // store heightmap dimensions
    int x = bbImageWidth(hmap)-1;
    int y = bbImageHeight(hmap)-1;
    int lx;
    int ly;
    int index;

    // load texture and lightmaps
    if (t1 == 0) {
        throw ("load_terrain error: invalid texture 1");
    }
    if (t2 == 0) {
        throw ("load_terrain error: invalid texture 2");
    }
    if (mask == 0) {
        throw ("load_terrain error: invalid texture mask");
    }

    // auto scale the textures to the right size
    if (t1) {
        bbScaleTexture(t1,x/4,y/4);
    }
    if (t2) {
        bbScaleTexture(t2,x/4,y/4);
    }
    if (mask) {
        bbScaleTexture(mask,x,y);
    }

    // start building the terrain
    MeshModel* mesh = bbCreateMesh();
    Surface* surf = bbCreateSurface(mesh);

    // create some verts for the terrain
    for (ly = 0; ly <= y; ly++) {
        for (lx = 0; lx <= x; lx++) {
            bbAddVertex(surf,lx,0,ly,1.f/lx,1.f/ly);
        }
    }
    bbRenderWorld();

    // connect the verts with faces
    for (ly = 0; ly <= y-1; ly++) {
        for (lx = 0; lx <= x-1; lx++) {
            bbAddTriangle(surf,lx+((x+1)*ly),lx+((x+1)*ly)+(x+1),(lx+1)+((x+1)*ly));
            bbAddTriangle(surf,(lx+1)+((x+1)*ly),lx+((x+1)*ly)+(x+1),(lx+1)+((x+1)*ly)+(x+1));
        }
    }

    // position the terrain to center 0,0,0
    MeshModel* mesh2 = bbDeepCopyMesh(mesh,mesh);
    Surface* surf2 = bbGetSurface(mesh2,1);
    bbPositionMesh(mesh, -x/2.f,0,-y/2.f);
    bbPositionMesh(mesh2, -x/2.f,0.01f,-y/2.f);

    // alter vertice height to match the heightmap red channel
    bbLockBuffer(bbImageBuffer(hmap));
    bbLockBuffer(bbTextureBuffer(mask));
    //SetBuffer
    for (lx = 0; lx <= x; lx++) {
        for (ly = 0; ly <= y; ly++) {
            //using vertex alpha and two meshes instead of FE_ALPHAWHATEVER
            //it doesn't look perfect but it does the job
            //you might get better results by downscaling the mask to the same size as the heightmap
            maskX = Min(lx*(float)(bbTextureWidth(mask))/(float)(bbImageWidth(hmap)),bbTextureWidth(mask)-1);
            maskY = bbTextureHeight(mask)-Min(ly*(float)(bbTextureHeight(mask))/(float)(bbImageHeight(hmap)),bbTextureHeight(mask)-1);
            RGB1 = bbReadPixelFast((int)(Min(lx,x-1)),(int)(y-Min(ly,y-1)),bbImageBuffer(hmap));
            //separate out the red
            r = (RGB1 & 0xFF0000) >> 16;
            alpha = (((bbReadPixelFast((int)(Max(maskX-5,5)),(int)(Max(maskY-5,5)),bbTextureBuffer(mask)) & 0xFF000000) >>  24)/0xFF);
            alpha = alpha+(((bbReadPixelFast((int)(Min(maskX+5,bbTextureWidth(mask)-5)),(int)(Min(maskY+5,bbTextureHeight(mask)-5)),bbTextureBuffer(mask)) & 0xFF000000) >> 24)/0xFF);
            alpha = alpha+(((bbReadPixelFast((int)(Max(maskX-5,5)),(int)(Min(maskY+5,bbTextureHeight(mask)-5)),bbTextureBuffer(mask)) & 0xFF000000) >> 24)/0xFF);
            alpha = alpha+(((bbReadPixelFast((int)(Min(maskX+5,bbTextureWidth(mask)-5)),(int)(Max(maskY-5,5)),bbTextureBuffer(mask)) & 0xFF000000) >> 24)/0xFF);
            alpha = alpha*0.25f;
            alpha = bbSqr(alpha);

            index = lx + ((x+1)*ly);
            bbVertexCoords(surf, index , bbVertexX(surf,index), r*yscale,bbVertexZ(surf,index));
            bbVertexCoords(surf2, index , bbVertexX(surf2,index), r*yscale,bbVertexZ(surf2,index));
            bbVertexColor(surf2, index, 255.f,255.f,255.f,alpha);
            // set the terrain texture coordinates
            bbVertexTexCoords(surf,index,lx,-ly);
            bbVertexTexCoords(surf2,index,lx,-ly);
        }
    }
    bbUnlockBuffer(bbTextureBuffer(mask));
    bbUnlockBuffer(bbImageBuffer(hmap));

    bbUpdateNormals(mesh);
    bbUpdateNormals(mesh2);

    bbEntityTexture(mesh,t1,0,0);
    //EntityTexture(mesh,mask,0,1)
    //2
    bbEntityTexture(mesh2,t2,0,0);

    bbEntityFX(mesh, 1);
    bbEntityFX(mesh2, 1+2+32);

    return mesh;
}

}
