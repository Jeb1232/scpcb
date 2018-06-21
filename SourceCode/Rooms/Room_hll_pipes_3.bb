

Function UpdateEvent106sinkhole(e.Events)
	Local dist#, i%, temp%, pvt%, strtemp$, j%, k%

	Local p.Particles, n.NPCs, r.Rooms, e2.Events, it.Items, em.Emitters, sc.SecurityCams, sc2.SecurityCams

	Local CurrTrigger$ = ""

	Local x#, y#, z#

	Local angle#

	;[Block]
	If e\eventState=0 Then
		de.Decals = CreateDecal(0, EntityX(e\room\obj)+Rnd(-0.5,0.5), 0.01, EntityZ(e\room\obj)+Rnd(-0.5,0.5), 90, Rand(360), 0)
		de\Size = 2.5 : ScaleSprite(de\obj, de\Size, de\Size);
		
		e\eventState=1
	ElseIf mainPlayer\currRoom = e\room Then
		If e\sounds[0]=0 Then
			e\sounds[0]=LoadSound("SFX/Room/Sinkhole.ogg")
		Else
			e\soundChannels[0] = LoopRangedSound(e\sounds[0], e\soundChannels[0], mainPlayer\cam, e\room\obj, 4.5, 1.5)
		EndIf
		dist = Distance(EntityX(mainPlayer\collider),EntityZ(mainPlayer\collider),EntityX(e\room\obj),EntityZ(e\room\obj))
		If dist < 2.0 Then
			mainPlayer\footstepOverride=1
			mainPlayer\moveSpeed = CurveValue(0.0, mainPlayer\moveSpeed, Max(dist*50,1.0))	
			mainPlayer\crouchState = (2.0-dist)/2.0
			
			If dist<0.5 Then
				If e\eventState2 = 0 Then
					PlaySound2(LoadTempSound("SFX/Room/SinkholeFall.ogg"))
				EndIf
				
				mainPlayer\moveSpeed = CurveValue(0.0, mainPlayer\moveSpeed, Max(dist*50,1.0))
				
				x = CurveValue(EntityX(e\room\obj),EntityX(mainPlayer\collider),10.0)
				y = CurveValue(EntityY(e\room\obj)-e\eventState2,EntityY(mainPlayer\collider),25.0)
				z = CurveValue(EntityZ(e\room\obj),EntityZ(mainPlayer\collider),10.0)
				PositionEntity mainPlayer\collider, x, y, z, True
				
				mainPlayer\dropSpeed = 0
				
				ResetEntity mainPlayer\collider
				
				e\eventState2=Min(e\eventState2+timing\tickDuration/200.0,2.0)
				
				;LightBlink = Min(e\eventState2*5,10.0)
				mainPlayer\blurTimer = e\eventState2*500
				
				If e\eventState2 = 2.0 Then MoveToPocketDimension()
			EndIf
		EndIf
	Else 
		e\eventState2=0
	EndIf
	
	;[End Block]
	
End Function


;~IDEal Editor Parameters:
;~C#Blitz3D