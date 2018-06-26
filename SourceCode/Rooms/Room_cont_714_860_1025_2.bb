Function FillRoom_cont_714_860_1025_2(r.Room)
    Local d.Door, d2.Door, sc.SecurityCam, de.Decal, r2.Room, sc2.SecurityCam
    Local it.Item, i%
    Local xtemp%, ytemp%, ztemp%

    Local t1%;, Bump

    d = CreateDoor(r\x + 264.0 * RoomScale, 0, r\z, 90, r, True, DOOR_TYPE_DEF, r\roomTemplate\name)
    d\autoClose = False : d\open = False
    PositionEntity(d\buttons[0], r\x + 320.0 * RoomScale, EntityY(d\buttons[0],True), EntityZ(d\buttons[0],True), True)
    PositionEntity(d\buttons[1], r\x + 224.0 * RoomScale, EntityY(d\buttons[1],True), EntityZ(d\buttons[1],True), True)

    d = CreateDoor(r\x - 264.0 * RoomScale, 0, r\z+32*RoomScale, 270, r, True, DOOR_TYPE_DEF, r\roomTemplate\name)
    d\autoClose = False : d\open = False
    PositionEntity(d\buttons[0], r\x - 320.0 * RoomScale, EntityY(d\buttons[0],True), EntityZ(d\buttons[0],True), True)
    PositionEntity(d\buttons[1], r\x - 224.0 * RoomScale, EntityY(d\buttons[1],True), EntityZ(d\buttons[1],True), True)

    r\doors[1] = CreateDoor(r\x-560.0 * RoomScale, 0, r\z - 240.0 * RoomScale, 0, r, True, DOOR_TYPE_DEF, r\roomTemplate\name)
    r\doors[1]\autoClose = False : r\doors[1]\open = False

    r\doors[2] = CreateDoor(r\x + 560.0 * RoomScale, 0, r\z - 272.0 * RoomScale, 180, r, True, DOOR_TYPE_DEF, r\roomTemplate\name)
    r\doors[2]\autoClose = False : r\doors[2]\open = False

    r\doors[3] = CreateDoor(r\x + 560.0 * RoomScale, 0, r\z + 272.0 * RoomScale, 180, r, True, DOOR_TYPE_DEF, r\roomTemplate\name)
    r\doors[3]\autoClose = False : r\doors[3]\open = False

    it = CreateItem("scp420j", r\x - 552.0 * RoomScale, r\y + 220.0 * RoomScale, r\z - 728.0 * RoomScale)
    EntityParent(it\collider, r\obj)

    it = CreateItem("scp860", r\x + 568.0 * RoomScale, r\y + 178.0 * RoomScale, r\z + 760.0 * RoomScale)
    EntityParent(it\collider, r\obj)

    sc = CreateSecurityCam(r\x + 560.0 * RoomScale, r\y + 386 * RoomScale, r\z - 416.0 * RoomScale, r)
    sc\angle = 180 : sc\turn = 30
    TurnEntity(sc\cameraObj, 30, 0, 0)
    EntityParent(sc\obj, r\obj)

    sc = CreateSecurityCam(r\x - 560.0 * RoomScale, r\y + 386 * RoomScale, r\z - 416.0 * RoomScale, r)
    sc\angle = 180 : sc\turn = 30
    TurnEntity(sc\cameraObj, 30, 0, 0)
    EntityParent(sc\obj, r\obj)
End Function

;~IDEal Editor Parameters:
;~C#Blitz3D