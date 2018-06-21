Function FillRoom_hll_tsl(r.Rooms)
    Local d.Doors, d2.Doors, sc.SecurityCams, de.Decals, r2.Rooms, sc2.SecurityCams
    Local it.Items, i%
    Local xtemp%, ytemp%, ztemp%
    
    Local t1;, Bump
    
    r\Objects[0] = CreatePivot()
    PositionEntity(r\Objects[0], r\x - 114.0 * RoomScale, 0.0, r\z)
    EntityParent(r\Objects[0], r\obj)
    
    r\Objects[1] = CreatePivot()
    PositionEntity(r\Objects[1], r\x + 114.0 * RoomScale, 0.0, r\z)
    EntityParent(r\Objects[1], r\obj)			
    
    r\Objects[2] = CreatePivot()
    PositionEntity(r\Objects[2], r\x, 0.0, r\z)
    EntityParent(r\Objects[2], r\obj)	
    
    r\Objects[3] = CreateSprite()
    EntityTexture (r\Objects[3], TeslaTexture)
    SpriteViewMode(r\Objects[3],2) 
    ;ScaleSprite (r\Objects[3],((512.0 * RoomScale)/2.0),((512.0 * RoomScale)/2.0))
    EntityBlend (r\Objects[3], 3) 
    EntityFX(r\Objects[3], 1 + 8 + 16)
    
    PositionEntity(r\Objects[3], r\x, 0.8, r\z)
    
    HideEntity r\Objects[3]
    EntityParent(r\Objects[3], r\obj)
    
	;TODO: fix?
    ;w.waypoints = CreateWaypoint(r\x, r\y + 66.0 * RoomScale, r\z + 292.0 * RoomScale, Null, r)
    ;w2.waypoints = CreateWaypoint(r\x, r\y + 66.0 * RoomScale, r\z - 284.0 * RoomScale, Null, r)
    ;w\connected[0] = w2 : w\dist[0] = EntityDistance(w\obj, w2\obj)
    ;w2\connected[0] = w : w2\dist[0] = w\dist[0]
    
    r\Objects[4] = CreateSprite()
    PositionEntity(r\Objects[4], r\x - 32 * RoomScale, 568 * RoomScale, r\z)
    ScaleSprite(r\Objects[4], 0.03, 0.03)
    EntityTexture(r\Objects[4], LightSpriteTex(1))
    EntityBlend (r\Objects[4], 3)
    EntityParent(r\Objects[4], r\obj)
    HideEntity r\Objects[4]
    
    r\Objects[5] = CreatePivot()
    PositionEntity(r\Objects[5],r\x,0,r\z-800*RoomScale)
    EntityParent(r\Objects[5],r\obj)
    
    r\Objects[6] = CreatePivot()
    PositionEntity(r\Objects[6],r\x,0,r\z+800*RoomScale)
    EntityParent(r\Objects[6],r\obj)
End Function


Function UpdateEvent_tesla(e.Events)
	Local dist#, i%, temp%, pvt%, strtemp$, j%, k%

	Local p.Particles, n.NPCs, r.Rooms, e2.Events, it.Items, em.Emitters, sc.SecurityCams, sc2.SecurityCams

	Local CurrTrigger$ = ""

	Local x#, y#, z#

	Local angle#

	;[Block]
	temp = True
	If e\eventState2 > 70*3.5 And e\eventState2 < 70*90 Then temp = False
	
	If temp And EntityY(mainPlayer\collider, True) > EntityY(e\room\obj,True) And EntityY(mainPlayer\collider, True) < 4.0 Then
		If e\eventState = 0 Then
			If (TimeInPosMilliSecs() Mod 1500) < 800 Then
				ShowEntity e\room\Objects[4]
			Else
				HideEntity e\room\Objects[4]
			EndIf						
			
			If e\room\dist < 8 Then
				If (Not IsChannelPlaying(e\soundChannels[0])) Then
					e\soundChannels[0] = PlayRangedSound_SM(sndManager\teslaIdle, mainPlayer\cam, e\room\Objects[3],4.0,0.5)
				EndIf
			EndIf
			
			For i = 0 To 2
				If Distance(EntityX(mainPlayer\collider),EntityZ(mainPlayer\collider),EntityX(e\room\Objects[i],True),EntityZ(e\room\Objects[i],True)) < 300.0*RoomScale Then
					;play the activation sound
					If Not mainPlayer\dead Then 
						mainPlayer\loudness = Max(8.0,mainPlayer\loudness)
						StopChannel(e\soundChannels[0])
						e\soundChannels[0] = PlayRangedSound_SM(sndManager\teslaActive, mainPlayer\cam, e\room\Objects[3],4.0,0.5)
						e\eventState = 1
						Exit
					EndIf
				EndIf
			Next
			
			If Curr106\state < -10 And e\eventState = 0 Then 
				For i = 0 To 2
					If Distance(EntityX(Curr106\collider),EntityZ(Curr106\collider),EntityX(e\room\Objects[i],True),EntityZ(e\room\Objects[i],True)) < 300.0*RoomScale Then
						;play the activation sound
						If Not mainPlayer\dead Then 
							StopChannel(e\soundChannels[0])
							e\soundChannels[0] = PlayRangedSound_SM(sndManager\teslaActive, mainPlayer\cam, e\room\Objects[3],4.0,0.5)
							HideEntity e\room\Objects[4]
							e\eventState = 1
							Curr106\state = 70 * 60 * Rand(10,13)
							Exit
						EndIf
					EndIf
				Next
			EndIf
		Else
			e\eventState = e\eventState+timing\tickDuration
			If e\eventState =< 40 Then
				If (TimeInPosMilliSecs() Mod 100) < 50 Then
					ShowEntity e\room\Objects[4]
				Else
					HideEntity e\room\Objects[4]
				EndIf
			Else
				If e\room\dist < 2 Then
					If e\eventState-timing\tickDuration =< 40 Then PlaySound_SM(sndManager\teslaShock)	
				Else
					If e\eventState-timing\tickDuration =< 40 Then PlayRangedSound_SM(sndManager\teslaShock,mainPlayer\cam,e\room\Objects[2])
				EndIf
				If e\eventState < 70 Then 
					
					If Not mainPlayer\dead Then 
						For i = 0 To 2
							If Distance(EntityX(mainPlayer\collider),EntityZ(mainPlayer\collider),EntityX(e\room\Objects[i],True),EntityZ(e\room\Objects[i],True)) < 250.0*RoomScale Then
								ShowEntity mainPlayer\overlays[OVERLAY_WHITE]
								mainPlayer\lightFlash = 0.4
								mainPlayer\camShake = 1.0
								Kill(mainPlayer)
								DeathMSG = "Subject D-9341 killed by the Tesla gate at [REDACTED]."
							EndIf
						Next
					EndIf
					
					If Curr106\state < -10 Then
						For i = 0 To 2
							If Distance(EntityX(Curr106\collider),EntityZ(Curr106\collider),EntityX(e\room\Objects[i],True),EntityZ(e\room\Objects[i],True)) < 250.0*RoomScale Then
								ShowEntity mainPlayer\overlays[OVERLAY_WHITE]
								mainPlayer\lightFlash = 0.3
								For i = 0 To 10
									p.Particles = CreateParticle(EntityX(Curr106\collider, True), EntityY(Curr106\collider, True), EntityZ(Curr106\collider, True), 0, 0.015, -0.2, 250)
									p\size = 0.03
									p\gravity = -0.2
									p\lifetime = 200
									p\SizeChange = 0.005
									p\speed = 0.001
									RotateEntity(p\pvt, Rnd(360), Rnd(360), 0, True)
								Next
								Curr106\state = -20000
								TranslateEntity(Curr106\collider,0,-50.0,0,True)
							EndIf
						Next								
					EndIf
					
					HideEntity e\room\Objects[3]
					HideEntity e\room\Objects[4]
					
					If Rand(5)<5 Then 
						PositionTexture TeslaTexture,0.0,Rnd(0,1.0)
						ShowEntity e\room\Objects[3]								
					EndIf
				Else 
					If e\eventState-timing\tickDuration < 70 Then 
						StopChannel(e\soundChannels[0])	
						e\soundChannels[0] = PlayRangedSound_SM(sndManager\teslaPowerUp, mainPlayer\cam, e\room\Objects[3],4.0,0.5)
					EndIf 
					HideEntity e\room\Objects[3]
					
					If e\eventState > 150 Then e\eventState = 0
				EndIf
			EndIf
		EndIf
	EndIf
	
	If mainPlayer\currRoom\RoomTemplate\name <> "pocketdimension" And mainPlayer\currRoom\RoomTemplate\name <> "room860" Then
		If e\eventState2 = 0 Then
			If e\eventState3 <=0 Then 
				temp = False
				For n.NPCs = Each NPCs
					If n\npcType = NPCtypeMTF Then
						If Abs(EntityX(n\collider)-EntityX(e\room\obj,True))<4.0 Then
							If Abs(EntityZ(n\collider)-EntityZ(e\room\obj,True))<4.0 Then
								temp = True
								If e\eventState2 = 0 Then
									n\sounds[0] = LoadSound("SFX/Character/MTF/Tesla0.ogg")
									PlayMTFSound(n\sounds[0],n)
									
									LoadEventSound(e,"SFX/Character/MTF/Tesla1.ogg")
									e\soundChannels[0] = PlaySound(e\sounds[0])
									n\Idle = 70*10
									e\eventState2 = 70*100
								EndIf
							EndIf
						EndIf
					EndIf
				Next
				If temp = False Then e\eventState2=70*3.5
				e\eventState3=e\eventState3+140
			Else
				e\eventState3=e\eventState3-timing\tickDuration
			EndIf
		Else
			e\eventState2 = Max(e\eventState2-timing\tickDuration,0)
		EndIf					
	EndIf
	
	;[End Block]
End Function



;~IDEal Editor Parameters:
;~C#Blitz3D