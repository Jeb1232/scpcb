Function FillRoom_srvr_096_2(r.Rooms)
    Local d.Doors, d2.Doors, sc.SecurityCams, de.Decals, r2.Rooms, sc2.SecurityCams
	Local it.Items, i%
	Local xtemp%, ytemp%, ztemp%
	
	Local t1;, Bump
    
    d.Doors = CreateDoor(0, r\x,0,r\z, 0, r, False, 2, False)
    d\locked = True
    
    r\RoomDoors[0] = CreateDoor(r\zone, r\x - 208.0 * RoomScale, 0.0, r\z - 736.0 * RoomScale, 90, r, True, False)
    r\RoomDoors[0]\AutoClose=False
    r\RoomDoors[1] = CreateDoor(r\zone, r\x - 208.0 * RoomScale, 0.0, r\z + 736.0 * RoomScale, 90, r, True, False)
    r\RoomDoors[1]\AutoClose=False
    
    r\RoomDoors[2] = CreateDoor(r\zone, r\x - 672.0 * RoomScale, 0.0, r\z - 1024.0 * RoomScale, 0, r, False, False, False, "GEAR")
    r\RoomDoors[2]\AutoClose=False : r\RoomDoors[2]\DisableWaypoint = True 
    FreeEntity(r\RoomDoors[2]\buttons[0]) : r\RoomDoors[2]\buttons[0]=0
    FreeEntity(r\RoomDoors[2]\buttons[1]) : r\RoomDoors[2]\buttons[1]=0
    
    For n% = 0 To 2
        r\Objects[n * 2] = CopyEntity(LeverBaseOBJ)
        r\Objects[n * 2 + 1] = CopyEntity(LeverOBJ)
        
        r\Levers[n] = r\Objects[n * 2 + 1]
        
        For i% = 0 To 1
            ScaleEntity(r\Objects[n * 2 + i], 0.03, 0.03, 0.03)
            
            Select n
                Case 0 ;power switch
                    PositionEntity (r\Objects[n * 2 + i], r\x - 1260.0 * RoomScale, r\y + 234.0 * RoomScale, r\z + 750 * RoomScale, True)	
                Case 1 ;generator fuel pump
                    PositionEntity (r\Objects[n * 2 + i], r\x - 920.0 * RoomScale, r\y + 164.0 * RoomScale, r\z + 898 * RoomScale, True)
                Case 2 ;generator on/off
                    PositionEntity (r\Objects[n * 2 + i], r\x - 837.0 * RoomScale, r\y + 152.0 * RoomScale, r\z + 886 * RoomScale, True)
            End Select
            
            EntityParent(r\Objects[n * 2 + i], r\obj)
        Next
        ;RotateEntity(r\Objects[n * 2], 0, -90, 0)
        RotateEntity(r\Objects[n*2+1], 81, -180, 0)
        
        ;EntityPickMode(r\Objects[n * 2 + 1], 2)
        EntityPickMode r\Objects[n * 2 + 1], 1, False
        EntityRadius r\Objects[n * 2 + 1], 0.1
        ;makecollbox(r\Objects[n * 2 + 1])
    Next
    
    RotateEntity(r\Objects[2+1], -81, -180, 0)
    RotateEntity(r\Objects[4+1], -81, -180, 0)
    
    ;096 spawnpoint
    r\Objects[6]=CreatePivot(r\obj)
    PositionEntity(r\Objects[6], r\x - 848*RoomScale, 0.5, r\z-576*RoomScale, True)
    ;guard spawnpoint
    r\Objects[7]=CreatePivot(r\obj)
    PositionEntity(r\Objects[7], r\x - 1328.0 * RoomScale, 0.5, r\z + 528*RoomScale, True)
    ;the point where the guard walks to
    r\Objects[8]=CreatePivot(r\obj)
    PositionEntity(r\Objects[8], r\x - 1376.0 * RoomScale, 0.5, r\z + 32*RoomScale, True)
    
    r\Objects[9]=CreatePivot(r\obj)
    PositionEntity(r\Objects[9], r\x - 848*RoomScale, 0.5, r\z+576*RoomScale, True)
End Function


Function UpdateEvent_srvr_096_2(e.Events)
	Local dist#, i%, temp%, pvt%, strtemp$, j%, k%

	Local p.Particles, n.NPCs, r.Rooms, e2.Events, it.Items, em.Emitters, sc.SecurityCams, sc2.SecurityCams

	Local CurrTrigger$ = ""

	Local x#, y#, z#

	Local angle#

	;[Block]
	If e\EventState=0 Then
		If mainPlayer\currRoom = e\room Then
		 ;close the doors when the player enters the room
			UseDoor(e\room\RoomDoors[0],False)
			e\room\RoomDoors[0]\locked = True
			UseDoor(e\room\RoomDoors[1],False)
			e\room\RoomDoors[1]\locked = True
			
			If Curr096=Null Then
				Curr096 = CreateNPC(NPCtype096, EntityX(e\room\Objects[6],True),EntityY(e\room\Objects[6],True)+0.1,EntityZ(e\room\Objects[6],True))
			Else
				PositionEntity Curr096\Collider, EntityX(e\room\Objects[6],True),EntityY(e\room\Objects[6],True)+0.1,EntityZ(e\room\Objects[6],True),True
			EndIf
			
			RotateEntity Curr096\Collider, 0, e\room\angle, 0, True
			ResetEntity Curr096\Collider
			Curr096\State=2
			Curr096\State2=70*10
			
			LoadEventSound(e,"SFX\Character\Guard\096ServerRoom1.ogg")
			e\SoundCHN = PlaySound_Strict (e\Sound)
			
			e\room\NPC[0]=CreateNPC(NPCtypeGuard, EntityX(e\room\Objects[7],True),EntityY(e\room\Objects[7],True),EntityZ(e\room\Objects[7],True))
			
			e\EventState=1
		EndIf
	ElseIf e\EventState < 70*45
		If	Rand(200)<5 And mainPlayer\currRoom = e\room Then 
			;LightBlink = Rnd(1.0,2.0)
			If Rand(5)=1 Then PlaySound2(IntroSFX(Rand(10,12)), mainPlayer\cam, e\room\obj, 8.0, Rnd(0.1,0.3))
		EndIf
		
		e\EventState=Min(e\EventState+timing\tickDuration,70*43)
		
		If e\room\NPC[0]<>Null Then
			If e\EventState < 70*13 Then
				
				If e\EventState > 70*8 Then
					Curr096\State=Min(Max(2,Curr096\State),3)
					Curr096\State2=70*10
				Else
					Curr096\State = 5
					PointEntity Curr096\Collider, e\room\Objects[9]
					RotateEntity Curr096\Collider, 0, EntityYaw(Curr096\Collider,True),0,True								
					If EntityDistance(Curr096\Collider, e\room\Objects[8])<2.4 Then
						Curr096\State2 = 0
					Else
						Curr096\State2 = 1001
					EndIf	
				EndIf
				
				e\room\NPC[0]\State=8
				SetAnimTime e\room\NPC[0]\obj, 115
				PointEntity e\room\NPC[0]\Collider, Curr096\Collider								
			ElseIf e\EventState-timing\tickDuration =< 70*15 Then ;walk to the doorway
				If e\EventState > 70*15 Then
					e\room\NPC[0]\State=3
					e\room\NPC[0]\PathStatus = FindPath(e\room\NPC[0], EntityX(e\room\Objects[8],True),0.5,EntityZ(e\room\Objects[8],True))
					e\room\NPC[0]\PathTimer=300
				EndIf
			ElseIf e\EventState<70*20 Then
				If e\room\NPC[0]\PathStatus=0 Then  
					e\room\RoomDoors[2]\open = False
					
					e\room\NPC[0]\State=7
					;SetAnimTime e\room\NPC[0]\obj, 115
					PointEntity e\room\NPC[0]\obj, Curr096\Collider
					RotateEntity (e\room\NPC[0]\Collider, 0, CurveAngle(EntityYaw(e\room\NPC[0]\obj),EntityYaw(e\room\NPC[0]\Collider),30),0)
					
				EndIf
			Else ;start walking away
				
				If Curr096\State = 4 Then ;shoot at 096 when it starts attacking
					Curr096\LastSeen=1
					
					e\room\NPC[0]\State = 2
					PointEntity e\room\NPC[0]\obj, Curr096\Collider
					RotateEntity (e\room\NPC[0]\Collider, 0, CurveAngle(EntityYaw(e\room\NPC[0]\obj),EntityYaw(e\room\NPC[0]\Collider),30),0)
					;If mainPlayer\currRoom = e\room Then LightBlink = (e\room\NPC[0]\Reload)+Rnd(0.5,2.0)
					Curr096\Target = e\room\NPC[0]
				Else
					If e\EventState>70*22 Then Curr096\State = 4
					If e\room\NPC[0]\State=7 Then
						e\room\NPC[0]\State=3
						e\room\NPC[0]\PathStatus = FindPath(e\room\NPC[0], EntityX(e\room\obj,True),0.4,EntityZ(e\room\obj,True))
						e\room\NPC[0]\PathTimer=300
					EndIf
				EndIf
			EndIf
			
			Curr096\Target = e\room\NPC[0]
			If AnimTime(Curr096\obj)>25 And AnimTime(Curr096\obj)<150 Then
				FreeSound_Strict e\Sound : e\Sound = 0
				e\Sound=LoadSound_Strict("SFX\Character\Guard\096ServerRoom2.ogg")
				e\SoundCHN=PlaySound_Strict(e\Sound)
				
				Curr096\CurrSpeed = 0
				
				For i = 0 To 6
					If e\room\angle = 0 Or e\room\angle = 180 Then
						de.Decals = CreateDecal(Rand(2,3), e\room\x-Rnd(197,199)*Cos(e\room\angle)*RoomScale, 1.0, e\room\z+(140.0*(i-3))*RoomScale,0,e\room\angle+90,Rnd(360))
						de\size = Rnd(0.8,0.85) : de\sizechange = 0.001
						de.Decals = CreateDecal(Rand(2,3), e\room\x-Rnd(197,199)*Cos(e\room\angle)*RoomScale, 1.0, e\room\z+(140.0*(i-3))*RoomScale,0,e\room\angle-90,Rnd(360))
						de\size = Rnd(0.8,0.85) : de\sizechange = 0.001
					Else
						de.Decals = CreateDecal(Rand(2,3), e\room\x+(140.0*(i-3))*RoomScale, 1.0, e\room\z-Rnd(197,199)*Sin(e\room\angle)*RoomScale-Rnd(0.001,0.003),0,e\room\angle+90,Rnd(360))
						de\size = Rnd(0.8,0.85) : de\sizechange = 0.001
						de.Decals = CreateDecal(Rand(2,3), e\room\x+(140.0*(i-3))*RoomScale, 1.0, e\room\z-Rnd(197,199)*Sin(e\room\angle)*RoomScale-Rnd(0.001,0.003),0,e\room\angle-90,Rnd(360))
						de\size = Rnd(0.8,0.85) : de\sizechange = 0.001
					EndIf
					de.Decals = CreateDecal(Rand(2,3), EntityX(e\room\NPC[0]\Collider)+Rnd(-2,2),Rnd(0.001,0.003),EntityZ(e\room\NPC[0]\Collider)+Rnd(-2,2),90,Rnd(360),0)
					
				Next
				de\Size = Rnd(0.5,0.7)
				ScaleSprite(de\obj, de\Size,de\Size)
				
				Curr096\State=5
				
				RemoveNPC(e\room\NPC[0])
				e\room\NPC[0]=Null
				
			EndIf
		Else
			
			If e\EventState >= 70*40 And e\EventState-timing\tickDuration < 70*40 Then ;open them again to let the player in
				e\room\RoomDoors[0]\locked=False
				e\room\RoomDoors[1]\locked=False
				UseDoor(e\room\RoomDoors[0],False)
				UseDoor(e\room\RoomDoors[1],False)
				FreeSound_Strict e\Sound : e\Sound = 0
				e\room\RoomDoors[0]\locked=True
				e\room\RoomDoors[1]\locked=True
			EndIf
			
			If mainPlayer\currRoom = e\room Then
				If e\SoundCHN<>0 Then
					If	ChannelPlaying(e\SoundCHN) Then 
						;LightBlink = Rnd(0.5,6.0)
						If Rand(50)=1 Then PlaySound2(IntroSFX(Rand(10,12)), mainPlayer\cam, e\room\obj, 8.0, Rnd(0.1,0.3))
					EndIf
				EndIf						
				
				If (e\room\angle = 0 Or e\room\angle = 180) Then ;lock the player inside
					If Abs(EntityX(mainPlayer\collider)-EntityX(e\room\obj,True))> 1.3 Then 
						e\EventState = 70*50
						e\Sound=0
					EndIf
				Else
					If Abs(EntityZ(mainPlayer\collider)-EntityZ(e\room\obj,True))> 1.3 Then 
						e\EventState = 70*50
						e\Sound=0
					EndIf
				EndIf	
			EndIf
			
		EndIf
		
	ElseIf mainPlayer\currRoom = e\room
		temp = UpdateLever(e\room\Objects[1]) ;power switch
		x = UpdateLever(e\room\Objects[3]) ;fuel pump
		z = UpdateLever(e\room\Objects[5]) ;generator
		
		;fuel pump on
		If x Then
			e\EventState2 = Min(1.0, e\EventState2+timing\tickDuration/350)
			
			;generator on
			If z Then
				If e\Sound2=0 Then LoadEventSound(e,"SFX\General\GeneratorOn.ogg",1)
				e\EventState3 = Min(1.0, e\EventState3+timing\tickDuration/450)
			Else
				e\EventState3 = Min(0.0, e\EventState3-timing\tickDuration/450)
			EndIf
		Else
			e\EventState2 = Max(0, e\EventState2-timing\tickDuration/350)
			e\EventState3 = Max(0, e\EventState3-timing\tickDuration/450)
		EndIf
		
		If e\EventState2>0 Then e\SoundCHN=LoopSound2(RoomAmbience[8], e\SoundCHN, mainPlayer\cam, e\room\Objects[3], 5.0, e\EventState2*0.8)
		If e\EventState3>0 Then e\SoundCHN2=LoopSound2(e\Sound2, e\SoundCHN2, mainPlayer\cam, e\room\Objects[5], 6.0, e\EventState3)
		
		If temp=0 And x And z Then
			e\room\RoomDoors[0]\locked = False
			e\room\RoomDoors[1]\locked = False
		Else
			;If Rand(200)<5 Then LightBlink = Rnd(0.5,1.0)
			
			If e\room\RoomDoors[0]\open Then 
				e\room\RoomDoors[0]\locked = False
				UseDoor(e\room\RoomDoors[0],False) 
			EndIf
			If e\room\RoomDoors[1]\open Then 
				e\room\RoomDoors[1]\locked = False
				UseDoor(e\room\RoomDoors[1],False)
			EndIf
			e\room\RoomDoors[0]\locked=True
			e\room\RoomDoors[1]\locked=True							
		EndIf 
	EndIf
	
	;[End Block]
End Function
