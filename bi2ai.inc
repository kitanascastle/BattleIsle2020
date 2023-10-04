'Battle Isle 2020 - KI


'Wartet bis die Kampfanimation vorbei ist
SUB AIWaitEndCombat(chnr&)
  WHILE AnyPlayerHasPhase&(chnr&, %PHASE_COMBAT)
    SLEEP 10
    IF channels(chnr&).info.state >= %CHANNELSTATE_VICTORY AND channels(chnr&).info.state <= %CHANNELSTATE_DEFEAT THEN EXIT LOOP
  WEND
END SUB



'Wartet bis die Bewegungsanimation vorbei ist
SUB AIWaitEndMovement(chnr&)
  LOCAL plnr&

  DO
    'prüfen, ob noch irgendein Spieler in der Bewegungsphase (oder höher) ist
    FOR plnr& = 0 TO %MAXPLAYERS-1
      IF GetPhase&(chnr&, plnr&) >= %PHASE_UNITMOVING THEN EXIT FOR
    NEXT plnr&
    IF plnr& = %MAXPLAYERS THEN EXIT LOOP
    SLEEP 10
    IF channels(chnr&).info.state >= %CHANNELSTATE_VICTORY AND channels(chnr&).info.state <= %CHANNELSTATE_DEFEAT THEN EXIT LOOP
  LOOP
END SUB



'Wartet bis der lokale Spieler keine Spiel-Nachricht mehr angezeigt bekommt
SUB AIWaitEndMessage(chnr&)
  IF gameMode& <> %GAMEMODE_SINGLE THEN EXIT SUB
  WHILE messageOpenTime! > 0
    SLEEP 10
    IF channels(chnr&).info.state >= %CHANNELSTATE_VICTORY AND channels(chnr&).info.state <= %CHANNELSTATE_DEFEAT THEN EXIT LOOP
  WEND
END SUB



'Ermittelt den Kategorietyp, der für diese KI die Siegbedingungen markiert
FUNCTION AIGetVictoryCategory&(chnr&, plnr&)
  LOCAL allies&, humanmask&

  allies& = channels(chnr&).player(plnr&).allymask
  humanmask& = NOT channels(chnr&).info.aimask

  AIGetVictoryCategory& = IIF&((allies& AND humanmask&) = 0, 80, 48)
END FUNCTION



'Ermittelt den Kategorietyp, der für diese KI die Niederlagenbedingungen markiert
FUNCTION AIGetLossCategory&(chnr&, plnr&)
  LOCAL allies&, humanmask&

  allies& = channels(chnr&).player(plnr&).allymask
  humanmask& = NOT channels(chnr&).info.aimask

  AIGetLossCategory& = IIF&((allies& AND humanmask&) = 0, 48, 80)
END FUNCTION



'Prüft, ob der AI Spieler das Spiel jetzt gewinnen kann
SUB AICheckWinConditions(chnr&, plnr&)
  LOCAL vcat&, vc&, pl&, shopnr&, shopowner&

  vcat& = AIGetVictoryCategory&(chnr&, plnr&)

  FOR vc& = 0 TO channels(chnr&).info.nvictoryconditions-1
    IF channels(chnr&).actions(vc&).category = vcat& THEN
      SELECT CASE channels(chnr&).actions(vc&).actiontype
      CASE %ACTYPE_VC_SHOPOCCUPIED    'Shop erobert
        pl& = channels(chnr&).actions(vc&).actionparam
        shopnr& = channels(chnr&).actions(vc&).shop
        shopowner& = channels(chnr&).shops(shopnr&).owner
        IF pl& = plnr& AND (channels(chnr&).player(plnr&).allymask AND 2^shopowner&) = 0 THEN
          'AI muß diesen Shop erobern, um das Spiel zu gewinnen
          channels(chnr&).shops(shopnr&).prio = 30000
        END IF
      END SELECT
    END IF
  NEXT vc&
END SUB



'Prüft, ob der AI Spieler handeln muß, um seine unmittelbare Niederlage zu verhindern
SUB AICheckLossConditions(chnr&, plnr&)
  LOCAL vcat&, vc&, pl&, shopnr&, shopowner&, allies&, unitnr&, unittp&, transporter&, foundfields&, x&, y&, hp!
  LOCAL reachablefields&(), oldtargets$()
  DIM oldtargets$(%MAXPLAYERS-1)

  vcat& = AIGetLossCategory&(chnr&, plnr&)
  allies& = channels(chnr&).player(plnr&).allymask

  'alle Zielmarkierungen sichern
  FOR pl& = 0 TO %MAXPLAYERS-1
    oldtargets$(pl&) = PushTargets$(chnr&, pl&)
  NEXT pl&

  FOR vc& = 0 TO channels(chnr&).info.nvictoryconditions-1
    IF channels(chnr&).actions(vc&).category = vcat& THEN
      SELECT CASE channels(chnr&).actions(vc&).actiontype
      CASE %ACTYPE_VC_SHOPOCCUPIED    'Shop erobert
        pl& = channels(chnr&).actions(vc&).actionparam
        shopnr& = channels(chnr&).actions(vc&).shop
        shopowner& = channels(chnr&).shops(shopnr&).owner
        IF (allies& AND 2^pl&) = 0 THEN
          'ein Feind muß diesen Shop erobern, um das Spiel zu gewinnen
          FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
            IF channels(chnr&).units(unitnr&).owner = pl& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND UnitIsInShop&(chnr&, unitnr&) = -1 THEN
              unittp& = channels(chnr&).units(unitnr&).unittype
              IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_OCCUPY) <> 0 THEN
                'prüfen, ob diese Feindeinheit den Shop binnen einer Runde erreichen kann
                CALL ClearTargets(chnr&, pl&)
                foundfields& = GetTargets&(chnr&, unitnr&, 1, 0, reachablefields&())
                IF (channels(chnr&).player(pl&).targets(channels(chnr&).shops(shopnr&).position, channels(chnr&).shops(shopnr&).position2) AND %TG_MOVE) = %TG_MOVE THEN
                  'diesem Feind die höchste Priorität geben
                  transporter& = UnitIsInTransporter&(chnr&, unitnr&)
                  channels(chnr&).units(IIF&(transporter& < 0, unitnr&, transporter&)).aitargetpriority = 30000
                END IF
              END IF
            END IF
          NEXT unitnr&
        END IF

      CASE %ACTYPE_VC_UNITDEAD_PLAYER1 TO %ACTYPE_VC_UNITDEAD_PLAYER6  'alle Einheiten eines Typs eines Spielers vernichtet
        pl& = channels(chnr&).actions(vc&).shop
        unittp& = channels(chnr&).actions(vc&).actionparam
        IF pl& = plnr& THEN
          'alle eigenen Einheiten dieses Typs mit 50% Schaden oder mehr zur Reparatur schicken
          FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
            IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND UnitIsInShop&(chnr&, unitnr&) = -1 AND channels(chnr&).units(unitnr&).unittype = unittp& THEN
              hp! = channels(chnr&).units(unitnr&).groupsize / channelsnosave(chnr&).unitclasses(unittp&).groupsize
              IF hp! < 0.55 THEN
                CALL AIFindNearestRepairstation(chnr&, plnr&, unitnr&, x&, y&)
                IF x& >= 0 THEN
                  channels(chnr&).units(unitnr&).aicommand = %AIC_REPAIR
                  CALL AIMoveCloserTo(chnr&, unitnr&, x&, y&)
                ELSE
                  'wenn keine Möglichkeit zur Reparatur besteht, dann Abstand zur Front halten
                  channels(chnr&).units(unitnr&).aicommand = %AIC_ENDANGERED
                END IF
              ELSE
                'falls dies die einzige/letzte Einheit für die Bedingung ist und diese keine Panzerung hat (wie Kuhn Kaap), dann Einheit als "Gefährdet" markieren
                IF CountUnits&(chnr&, plnr&, unittp&) = 1 AND channelsnosave(chnr&).unitclasses(unittp&).armor < 320 THEN channels(chnr&).units(unitnr&).aicommand = %AIC_ENDANGERED
              END IF
            END IF
          NEXT unitnr&
        END IF

      END SELECT
    END IF
  NEXT vc&

  'alle Zielmarkierungen wieder herstellen
  FOR pl& = 0 TO %MAXPLAYERS-1
    CALL PopTargets(chnr&, pl&, oldtargets$(pl&))
  NEXT pl&
END SUB



'Prüft, ob eine Flugeinheit ihre Höhe ändern sollte
SUB AICheckClimb(chnr&, plnr&, unitnr&)
  IF channels(chnr&).units(unitnr&).aicommand = %AIC_REFUEL OR channels(chnr&).units(unitnr&).aicommand = %AIC_REPAIR THEN
    'absteigen, damit befüllt werden kann
    ChangeFlightHeight(chnr&, unitnr&, %UNITACTION_DESCEND)
  ELSE
    'aufsteigen
    ChangeFlightHeight(chnr&, unitnr&, %UNITACTION_ASCEND)
  END IF
END SUB



'Prüft, ob ein U-Boot seine Höhe ändern sollte
SUB AICheckDive(chnr&, plnr&, unitnr&)
  IF channels(chnr&).units(unitnr&).aicommand = %AIC_REFUEL OR channels(chnr&).units(unitnr&).aicommand = %AIC_REPAIR THEN
    'auftauchen, damit befüllt werden kann
    ChangeWaterHeight(chnr&, unitnr&, %UNITACTION_ASCEND)
  ELSE
    'abtauchen
    ChangeWaterHeight(chnr&, unitnr&, %UNITACTION_DESCEND)
  END IF
END SUB



'Einheit durch Versorgungsfahrzeug reparieren
SUB AIRepairUnit(chnr&, supportunitnr&, unitnr&)
  'prüfen, ob Zieleinheit direkt neben dem Versorgungsfahrzeug steht
  IF GetDistance&(channels(chnr&).units(supportunitnr&).xpos, channels(chnr&).units(supportunitnr&).ypos, channels(chnr&).units(unitnr&).xpos, channels(chnr&).units(unitnr&).ypos) > 1 THEN EXIT SUB

  'Einheit reparieren
  CALL Repair(chnr&, supportunitnr&, unitnr&)

  IF gameMode& = %GAMEMODE_SERVER THEN
    'Einheiten auf den Clients aktualisieren
    CALL UpdateUnitToAllClients(chnr&, unitnr&)
    CALL UpdateUnitToAllClients(chnr&, supportunitnr&)
  END IF
END SUB



'Einheit durch Versorgungsfahrzeug befüllen
SUB AIRefuelUnit(chnr&, supportunitnr&, unitnr&)
  'prüfen, ob Zieleinheit direkt neben dem Versorgungsfahrzeug steht
  IF GetDistance&(channels(chnr&).units(supportunitnr&).xpos, channels(chnr&).units(supportunitnr&).ypos, channels(chnr&).units(unitnr&).xpos, channels(chnr&).units(unitnr&).ypos) > 1 THEN EXIT SUB

  'Einheit befüllen
  CALL Refuel(chnr&, supportunitnr&, unitnr&)

  IF gameMode& = %GAMEMODE_SERVER THEN
    'Einheiten auf den Clients aktualisieren
    CALL UpdateUnitToAllClients(chnr&, unitnr&)
    CALL UpdateUnitToAllClients(chnr&, supportunitnr&)
  END IF
END SUB



'Befüllt alle Einheiten, die direkt an eine Versorgungsfahrzeug angrenzen
SUB AIRefuelNoMove(chnr&, plnr&)
  LOCAL i&, supportunitnr&, unittp&, supportunitmask&, friend&, friendtp&
  LOCAL xoff&(), yoff&()

  supportunitmask& = %UCF_REPAIR OR %UCF_REFUEL OR %UCF_REFUELAIR

  'alle Versorgungsfahrzeuge suchen
  FOR supportunitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(supportunitnr&).owner = plnr& AND UnitIsAlive&(chnr&, supportunitnr&) <> 0 AND (channels(chnr&).units(supportunitnr&).flags AND %US_DONE+%US_AI_IGNORE) = 0 AND UnitIsInShop&(chnr&, supportunitnr&) = -1 THEN
      unittp& = channels(chnr&).units(supportunitnr&).unittype
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND supportunitmask&) <> 0 THEN
        'prüfen, ob sich eigene Einheiten auf angrenzenden Feldern befinden
        CALL GetAdjacentFields(chnr&, channels(chnr&).units(supportunitnr&).xpos, channels(chnr&).units(supportunitnr&).ypos, xoff&(), yoff&())
        FOR i& = 0 TO 5
          IF xoff&(i&) >= 0 THEN
            friend& = channels(chnr&).zone3(xoff&(i&), yoff&(i&))
            IF friend& >= 0 AND channels(chnr&).units(friend&).owner = plnr& THEN
              friendtp& = channels(chnr&).units(friend&).unittype
              IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REPAIR) <> 0 AND (channelsnosave(chnr&).unitclasses(friendtp&).flags AND %UCF_PLANE) = 0 AND AIUnitNeedsRepair&(chnr&, friend&) <> 0 THEN
                'Einheit reparieren
                CALL AIRepairUnit(chnr&, supportunitnr&, friend&)
              END IF
              IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REFUEL) <> 0 AND ((channelsnosave(chnr&).unitclasses(friendtp&).flags AND %UCF_PLANE) = 0 OR (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REFUELAIR) <> 0) THEN
                'Einheit befüllen
                IF AIUnitLowOnAmmo&(chnr&, friend&) OR AIUnitLowOnFuel&(chnr&, friend&) THEN
                  CALL AIRefuelUnit(chnr&, supportunitnr&, friend&)
                END IF
              END IF
            END IF
          END IF
        NEXT i&
      END IF
      CALL AIWaitEndMessage(chnr&)
    END IF
  NEXT supportunitnr&
END SUB



'Bewegt alle Versorgungseinheiten zu Einheiten, die versorgt werden müssen
SUB AIRefuel(chnr&, plnr&)
  LOCAL i&, unitnr&, unittp&, supportunitmask&, usedmask&, friend&, friendtp&, tg&, x&, y&, targetunit&
  LOCAL xoff&(), yoff&()

  supportunitmask& = %UCF_REPAIR OR %UCF_REFUEL OR %UCF_REFUELAIR
  usedmask& = %US_MOVED OR %US_ATTACKED OR %US_DONE OR %US_AI_IGNORE OR %US_MOVEDFROMSHOP OR %US_ACTIONUSED

  'alle Versorgungsfahrzeuge suchen
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND (channels(chnr&).units(unitnr&).flags AND usedmask&) = 0 AND UnitIsInShop&(chnr&, unitnr&) = -1 THEN
      unittp& = channels(chnr&).units(unitnr&).unittype
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND supportunitmask&) <> 0 THEN
        'nach Versorgungszielen suchen
        tg& = AIFindTargets&(chnr&, unitnr&, 2)
        IF tg& >= 0 THEN
          channels(chnr&).units(unitnr&).aicommand = %AIC_REFUEL
          x& = tg& AND 255
          y& = INT(tg&/256) AND 255
          targetunit& = INT(tg&/65536)
          channels(chnr&).units(unitnr&).aitargetunit = targetunit&
          channels(chnr&).units(targetunit&).aicommand = %AIC_REFUELTARGET
          CALL AIMoveCloserTo(chnr&, unitnr&, x&, y&)
        ELSE
          'kein erreichbares Ziel
          IF AISupportAttack&(chnr&, plnr&, unitnr&) <> 0 THEN
            CALL UnitDone(chnr&, unitnr&)
          ELSE
            'Einheit zur nächsten Kampfgruppe bewegen
            tg& = AIFindTargets&(chnr&, unitnr&, 3)
            IF tg& >= 0 THEN
              channels(chnr&).units(unitnr&).aicommand = %AIC_REFUEL
              x& = tg& AND 255
              y& = INT(tg&/256) AND 255
              targetunit& = INT(tg&/65536)
              channels(chnr&).units(unitnr&).aitargetunit = targetunit&
              channels(chnr&).units(targetunit&).aicommand = %AIC_REFUELTARGET
              CALL AIMoveCloserTo(chnr&, unitnr&, x&, y&)
            ELSE
              CALL UnitDone(chnr&, unitnr&)
            END IF
          END IF
        END IF
      END IF
    END IF
  NEXT unitnr&
END SUB



'Mit Versorgungsfahrzeugen angreifen, die noch Aktionen durchführen können
FUNCTION AISupportAttack&(chnr&, plnr&, supportunitnr&)
  LOCAL foundtargets&, k&, enemy&, combatresult&, bestresult&, bestenemy&
  LOCAL targetunits&()

  'nur auf schwerer Schwierigkeit
  IF channels(chnr&).info.difficulty < %DIFFICULTY_NORMAL THEN EXIT FUNCTION

  'Angriffsziele suchen
  bestenemy& = -1
  bestresult& = 0
  foundtargets& = GetTargets&(chnr&, supportunitnr&, 2, 0, targetunits&())
  IF foundtargets& > 0 THEN
    FOR k& = 0 TO foundtargets&-1
      enemy& = targetunits&(k&)
      IF IsEnemyUnit&(chnr&, plnr&, enemy&) <> 0 THEN
        'nur Feinde betrachten, da GetTargets&() auch eigene zu versorgende Einheiten zurückliefert
        combatresult& = AISimulateCombat&(chnr&, supportunitnr&, enemy&)
        IF channels(chnr&).combat.params(5, 1) > 0 AND channels(chnr&).combat.params(5, 0) = 0 THEN
          IF channels(chnr&).units(enemy&).aitargetpriority > 9999 THEN combatresult& = combatresult&+channels(chnr&).units(enemy&).aitargetpriority/100
          IF combatresult& > bestresult& THEN
            bestresult& = combatresult&
            bestenemy& = enemy&
          END IF
        END IF
      END IF
    NEXT k&
  END IF

  'bestes Ziel angreifen
  IF bestenemy& >= 0 THEN
    CALL AIStartCombat(chnr&, plnr&, supportunitnr&, bestenemy&)
    AISupportAttack& = 1
  END IF
END FUNCTION



'Alle unbeweglichen Einheiten angreifen lassen
SUB AICheckUnmovableUnits(chnr&, plnr&, md&)
  LOCAL i&, unitnr&, unittp&, foundtargets&, enemy&, besttarget&, bestprio&
  LOCAL targetunits&()

  CALL AIResetIgnoreFlag(chnr&)
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND (channels(chnr&).units(unitnr&).flags AND %US_DONE) = 0 THEN
      unittp& = channels(chnr&).units(unitnr&).unittype
      IF channelsnosave(chnr&).unitclasses(unittp&).range = 0 OR channels(chnr&).units(unitnr&).aimissioncommand = %AIMC_NOMOVE THEN
        foundtargets& = GetTargets&(chnr&, unitnr&, 2, 0, targetunits&())
        IF foundtargets& > 0 THEN
          'Ziele gefunden -> bestes Ziel bestimmen
          bestprio& = -1
          FOR i& = 0 TO foundtargets&-1
            enemy& = targetunits&(i&)
            IF channels(chnr&).units(enemy&).aitargetpriority > bestprio& THEN
              bestprio& = channels(chnr&).units(enemy&).aitargetpriority
              besttarget& = enemy&
            END IF
          NEXT i&
          'Ziel angreifen
          CALL AIStartCombat(chnr&, plnr&, unitnr&, besttarget&)
        ELSE
          'Einheit kann diese Runde nichts machen
          IF md& = 0 THEN
            channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags OR %US_AI_IGNORE
          ELSE
            CALL UnitDone(chnr&, unitnr&)
          END IF
        END IF
        CALL AIWaitEndMessage(chnr&)
      END IF
    END IF
  NEXT unitnr&
END SUB



'Alle Einheiten verarbeiten, die nach der Bewegung nicht mehr angreifen können
SUB AICheckCooldownUnits(chnr&, plnr&)
  LOCAL i&, unitnr&, unittp&, foundtargets&, enemy&, besttarget&, bestprio&
  LOCAL targetunits&()

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND UnitIsInShop&(chnr&, unitnr&) = -1 AND (channels(chnr&).units(unitnr&).flags AND %US_DONE+%US_AI_IGNORE) = 0 THEN
      unittp& = channels(chnr&).units(unitnr&).unittype
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_COOLDOWN) <> 0 AND channels(chnr&).units(unitnr&).aicommand = %AIC_NONE THEN
        foundtargets& = GetTargets&(chnr&, unitnr&, 2, 0, targetunits&())
        IF foundtargets& > 0 THEN
          'Ziele gefunden -> bestes Ziel bestimmen
          bestprio& = -1
          FOR i& = 0 TO foundtargets&-1
            enemy& = targetunits&(i&)
            IF channels(chnr&).units(enemy&).aitargetpriority > bestprio& THEN
              bestprio& = channels(chnr&).units(enemy&).aitargetpriority
              besttarget& = enemy&
            END IF
          NEXT i&
          'Ziel angreifen
          CALL AIStartCombat(chnr&, plnr&, unitnr&, besttarget&)
        ELSE
          'Minen sollen dort bleiben, wo sie im Editor platziert worden sind
          IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_BADDEFENDER) <> 0 THEN CALL UnitDone(chnr&, unitnr&)
        END IF
        CALL AIWaitEndMessage(chnr&)
      END IF
    END IF
  NEXT unitnr&
END SUB



'Aldinium-Kristalle sammeln
SUB AICollectAldinium(chnr&, plnr&)
  LOCAL i&, k&, x&, y&, plmask&, crunit&, unitnr&, unittp&, ntransporters&, ncrystals&, bestdist&, bestcrystal&, besttransporter&, crunittype&, crweight&, collectmore&, shopnr&
  LOCAL fuellevel!
  LOCAL crystals$, transporters$
  LOCAL distances&()

  'Aldinium suchen (auf allen erforschten Feldern - eigentlich überwachten Feldern, aber so kann die KI auch ohne Aufklärer in der Nähe sammeln)
  plmask& = 2^plnr&
  FOR y& = 0 TO channels(chnr&).info.ysize-1
    FOR x& = 0 TO channels(chnr&).info.xsize-1
      IF (channels(chnr&).explored(x&, y&) AND plmask&) <> 0 THEN
        crunit& = channels(chnr&).zone3(x&, y&)
        IF crunit& >= 0 AND (channelsnosave(chnr&).unitclasses(channels(chnr&).units(crunit&).unittype).flags AND %UCF_CRYSTAL) <> 0 THEN crystals$ = crystals$+MKI$(crunit&)
      END IF
    NEXT x&
  NEXT y&
  ncrystals& = LEN(crystals$)/2
  crunit& = CVI(crystals$)

  'prüfen, ob Transporter mit geladenen Kristallen zum nächsten Shop fahren sollten oder noch Kristalle aus der unmittelbaren Umgebung mitnehmen
  FOR unitnr& = 0 TO channels(chnr&).info.nunits
    IF UnitIsAlive&(chnr&, unitnr&) <> 0 AND channels(chnr&).units(unitnr&).owner = plnr& AND channels(chnr&).units(unitnr&).aicommand = %AIC_NONE AND TransporterHasLoaded&(chnr&, unitnr&, channelsnosave(chnr&).crystalClass) <> 0 THEN
      collectmore& = 0
      IF ncrystals& > 0 AND TransporterCanLoadUnit&(chnr&, unitnr&, crunit&) THEN
        'alle Kristalle mit maximaler Entfernung von 4 Feldern ermitteln
        FOR i& = 0 TO ncrystals&
          crunit& = CVI(crystals$, i&*2+1)
          CALL CalculateUnitPath(chnr&, unitnr&, channels(chnr&).units(crunit&).xpos, channels(chnr&).units(crunit&).ypos, 0)
          IF channels(chnr&).player(plnr&).unitpathlen > 0 AND channels(chnr&).player(plnr&).unitpathlen < 6 THEN
            collectmore& = 1
            EXIT FOR
          END IF
        NEXT i&
      END IF
      'Shop zum Abliefern der Ladung suchen
      IF collectmore& = 0 THEN
        shopnr& = AIFindShopForCrystals&(chnr&, plnr&, unitnr&)
        IF shopnr& >= 0 THEN
          channels(chnr&).units(unitnr&).aicommand = %AIC_RETURNCRYSTALS
          channels(chnr&).units(unitnr&).aitargetshop = shopnr&
          CALL AIMoveCloserTo(chnr&, unitnr&, channels(chnr&).shops(shopnr&).position, channels(chnr&).shops(shopnr&).position2)
          CALL AIWaitEndMessage(chnr&)
        END IF
      END IF
    END IF
  NEXT unitnr&

  'Transporter suchen (mit ausreichend Treibstoff)
  IF ncrystals& = 0 THEN EXIT SUB
  FOR unitnr& = 0 TO channels(chnr&).info.nunits
    IF UnitIsAlive&(chnr&, unitnr&) <> 0 AND channels(chnr&).units(unitnr&).owner = plnr& AND channels(chnr&).units(unitnr&).aicommand = %AIC_NONE THEN
      unittp& = channels(chnr&).units(unitnr&).unittype
      fuellevel! = channels(chnr&).units(unitnr&).fuel/channelsnosave(chnr&).unitclasses(unittp&).fuel
      IF TransporterCanLoadUnit&(chnr&, unitnr&, crunit&) AND fuellevel! > 0.4 THEN transporters$ = transporters$+MKI$(unitnr&)
    END IF
  NEXT unitnr&
  ntransporters& = LEN(transporters$)/2
  IF ntransporters& = 0 THEN EXIT SUB

  'Abstand zwischen jedem Transporter und jedem Kristall ermitteln
  DIM distances&(ntransporters&-1, ncrystals&-1)
  FOR i& = 0 TO ntransporters&-1
    unitnr& = CVI(transporters$, i&*2+1)
    FOR k& = 0 TO ncrystals&-1
      crunit& = CVI(crystals$, k&*2+1)
      'prüfen, ob Kristall für diesen Transporter erreichbar ist (mit maximal 20 Feldern)
      CALL CalculateUnitPath(chnr&, unitnr&, channels(chnr&).units(crunit&).xpos, channels(chnr&).units(crunit&).ypos, 160)
      distances&(i&, k&) = channels(chnr&).player(plnr&).unitpathlen
    NEXT k&
  NEXT i&

  'besten Kristall für jeden Transporter ermitteln
  DO
    bestdist& = 255
    FOR i& = 0 TO ntransporters&-1
      FOR k& = 0 TO ncrystals&-1
        IF distances&(i&, k&) > 0 AND distances&(i&, k&) < bestdist& THEN
          bestdist& = distances&(i&, k&)
          besttransporter& = i&
          bestcrystal& = k&
        END IF
      NEXT k&
    NEXT i&
    IF bestdist& = 255 THEN EXIT LOOP
    'Transporter-Kristall-Paar gefunden
    unitnr& = CVI(transporters$, besttransporter&*2+1)
    crunit& = CVI(crystals$, bestcrystal&*2+1)
    channels(chnr&).units(unitnr&).aicommand = %AIC_COLLECT
    channels(chnr&).units(unitnr&).aitargetunit = crunit&
    CALL AIMoveCloserTo(chnr&, unitnr&, channels(chnr&).units(crunit&).xpos, channels(chnr&).units(crunit&).ypos)
    CALL AIWaitEndMessage(chnr&)
    'Transporter aus Matrix entfernen
    FOR i& = 0 TO ncrystals&-1
      distances&(besttransporter&, i&) = 0
    NEXT i&
    'Kristall aus Matrix entfernen
    FOR i& = 0 TO ntransporters&-1
      distances&(i&, bestcrystal&) = 0
    NEXT i&
  LOOP
END SUB



'Zielpriorität für einen Gegner ermitteln
SUB AISetTargetPriority(chnr&, plnr&, unitnr&)
  LOCAL unittp&, prio&

  unittp& = channels(chnr&).units(unitnr&).unittype
  prio& = 0
  IF UnitIsAlive&(chnr&, unitnr&) <> 0 AND IsEnemyUnit&(chnr&, plnr&, unitnr&) <> 0 THEN
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REPAIR) <> 0 THEN prio& = prio&+10000
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REFUEL) <> 0 THEN prio& = prio&+5000
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_OCCUPY) <> 0 THEN prio& = prio&+2000
    prio& = prio&+1000-channelsnosave(chnr&).unitclasses(unittp&).armor
    IF channels(chnr&).units(unitnr&).groupsize < 4 THEN prio& = prio&+1000
    IF channels(chnr&).units(unitnr&).xpos = channels(chnr&).player(plnr&).aiglobaltargetx AND channels(chnr&).units(unitnr&).ypos = channels(chnr&).player(plnr&).aiglobaltargety THEN prio& = prio&+25000
    IF channels(chnr&).units(unitnr&).aitargetpriority >= 25000 THEN prio& = channels(chnr&).units(unitnr&).aitargetpriority  'Ziele mit höchster Priorität beibehalten
  END IF
  channels(chnr&).units(unitnr&).aitargetpriority = prio&
END SUB



'Zielprioritäten für alle Gegner ermitteln
SUB AISetTargetPriorityAll(chnr&, plnr&)
  LOCAL unitnr&, plmask&, x&, y&

  plmask& = 2^plnr&
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF UnitIsAlive&(chnr&, unitnr&) <> 0 AND IsEnemyUnit&(chnr&, plnr&, unitnr&) <> 0 AND channels(chnr&).units(unitnr&).aitargetpriority = 0 THEN
      x& = channels(chnr&).units(unitnr&).xpos
      y& = channels(chnr&).units(unitnr&).ypos
      IF channels(chnr&).zone3(x&, y&) = unitnr& AND (channels(chnr&).vision(x&, y&) AND plmask&) <> 0 THEN CALL AISetTargetPriority(chnr&, plnr&, unitnr&)
    END IF
  NEXT unitnr&
END SUB



'Sucht das nächstgelegene Reparaturfahrzeug oder Shop für eine zu reparierende Einheit
SUB AIFindNearestRepairstation(BYVAL chnr&, BYVAL plnr&, BYVAL unitnr&, x&, y&)
  LOCAL shopnr&, shoptype&, unittp&, bestshopdist&, bestshop&, repairunitnr&, repairunittp&, bestunitdist&, bestunit&

  x& = -1
  unittp& = channels(chnr&).units(unitnr&).unittype
  channels(chnr&).player(plnr&).selectedunit = -1  'verhindern, daß Route angezeigt wird

  'nächsten Shop mit Material (oder Materialproduktion) suchen
  bestshop& = -1
  bestshopdist& = 255
  FOR shopnr& = 0 TO channels(chnr&).info.nshops-1
    IF channels(chnr&).shops(shopnr&).owner = plnr& AND UnitCanEnterBuilding&(chnr&, unitnr&, shopnr&) AND channels(chnr&).shops(shopnr&).material+channels(chnr&).shops(shopnr&).mplus > 0 THEN
      'prüfen, ob Shop für diese Einheit erreichbar ist (mit maximal 32 Feldern)
      CALL CalculateUnitPath(chnr&, unitnr&, channels(chnr&).shops(shopnr&).position, channels(chnr&).shops(shopnr&).position2, 254)
      IF channels(chnr&).player(plnr&).unitpathlen > 0 AND channels(chnr&).player(plnr&).unitpathlen < bestshopdist& THEN
        bestshopdist& = channels(chnr&).player(plnr&).unitpathlen
        bestshop& = shopnr&
      END IF
    END IF
  NEXT shopnr&

  'nächstes Reparaturfahrzeug suchen (nicht für Flugzeuge)
  bestunit& = -1
  bestunitdist& = 255
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) = 0 THEN
    FOR repairunitnr& = 0 TO channels(chnr&).info.nunits&-1
      IF channels(chnr&).units(repairunitnr&).owner = plnr& AND UnitIsAlive&(chnr&, repairunitnr&) <> 0 THEN
        repairunittp& = channels(chnr&).units(repairunitnr&).unittype
        IF (channelsnosave(chnr&).unitclasses(repairunittp&).flags AND %UCF_REPAIR) <> 0 AND GetUnitRepairMaterial&(chnr&, repairunitnr&) > 0 THEN
          'prüfen, ob Reparaturfahrzeug für diese Einheit erreichbar ist (mit maximal 32 Feldern)
          CALL CalculateUnitPath(chnr&, unitnr&, channels(chnr&).units(repairunitnr&).xpos, channels(chnr&).units(repairunitnr&).ypos, 254)
          IF channels(chnr&).player(plnr&).unitpathlen > 0 AND channels(chnr&).player(plnr&).unitpathlen < bestunitdist& THEN
            bestunitdist& = channels(chnr&).player(plnr&).unitpathlen
            bestunit& = repairunitnr&
          END IF
        END IF
      END IF
    NEXT repairunitnr&
  END IF

  'falls ein Reparaturfahrzeug oder Shop gefunden wurde, dann diesen Ort aufsuchen
  IF bestunit& >= 0 AND bestshop& >= 0 AND bestunitdist& <= bestshopdist&+5 THEN bestshop& = -1  'Reparaturfahrzeug bevorzugen (sofern nicht mehr als 5 Felder weiter als der beste Shop entfernt)
  IF bestshop& >= 0 THEN
    x& = channels(chnr&).shops(bestshop&).position
    y& = channels(chnr&).shops(bestshop&).position2
  ELSE
    IF bestunit& >= 0 THEN
      x& = channels(chnr&).units(bestunit&).xpos
      y& = channels(chnr&).units(bestunit&).ypos
    END IF
  END IF
END SUB



'Sucht das nächstgelegene Füllfahrzeug oder Shop für eine zu befüllende Einheit (nur Munition)
SUB AIFindNearestRefuelstation(chnr&, plnr&, unitnr&, x&, y&)
  LOCAL shopnr&, shoptype&, unittp&, bestshopdist&, bestshop&, supportunitnr&, supportunittp&, bestunitdist&, bestunit&

  x& = -1
  unittp& = channels(chnr&).units(unitnr&).unittype
  channels(chnr&).player(plnr&).selectedunit = -1  'verhindern, daß Route angezeigt wird

  'nächsten Shop mit Material (oder Materialproduktion) suchen
  bestshop& = -1
  bestshopdist& = 255
  FOR shopnr& = 0 TO channels(chnr&).info.nshops-1
    IF channels(chnr&).shops(shopnr&).owner = plnr& AND UnitCanEnterBuilding&(chnr&, unitnr&, shopnr&) AND channels(chnr&).shops(shopnr&).material+channels(chnr&).shops(shopnr&).mplus > 0 THEN
      'prüfen, ob Shop für diese Einheit erreichbar ist (mit maximal 32 Feldern)
      CALL CalculateUnitPath(chnr&, unitnr&, channels(chnr&).shops(shopnr&).position, channels(chnr&).shops(shopnr&).position2, 254)
      IF channels(chnr&).player(plnr&).unitpathlen > 0 AND channels(chnr&).player(plnr&).unitpathlen < bestshopdist& THEN
        bestshopdist& = channels(chnr&).player(plnr&).unitpathlen
        bestshop& = shopnr&
      END IF
    END IF
  NEXT shopnr&

  'nächstes Füllfahrzeug suchen
  bestunit& = -1
  bestunitdist& = 255
  FOR supportunitnr& = 0 TO channels(chnr&).info.nunits&-1
    IF channels(chnr&).units(supportunitnr&).owner = plnr& AND UnitIsAlive&(chnr&, supportunitnr&) <> 0 THEN
      supportunittp& = channels(chnr&).units(supportunitnr&).unittype
      IF (channelsnosave(chnr&).unitclasses(supportunittp&).flags AND %UCF_REFUEL) <> 0 AND GetUnitRechargeMaterial&(chnr&, supportunitnr&) > 0 THEN
        IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) = 0 OR (channelsnosave(chnr&).unitclasses(supportunittp&).flags AND %UCF_REFUELAIR) <> 0 THEN
          'prüfen, ob Füllfahrzeug für diese Einheit erreichbar ist (mit maximal 32 Feldern)
          CALL CalculateUnitPath(chnr&, unitnr&, channels(chnr&).units(supportunitnr&).xpos, channels(chnr&).units(supportunitnr&).ypos, 254)
          IF channels(chnr&).player(plnr&).unitpathlen > 0 AND channels(chnr&).player(plnr&).unitpathlen < bestunitdist& THEN
            bestunitdist& = channels(chnr&).player(plnr&).unitpathlen
            bestunit& = supportunitnr&
          END IF
        END IF
      END IF
    END IF
  NEXT supportunitnr&

  'falls ein Füllfahrzeug oder Shop gefunden wurde, dann diesen Ort aufsuchen
  IF bestunit& >= 0 AND bestshop& >= 0 AND bestunitdist& <= bestshopdist&+5 THEN bestshop& = -1  'Füllfahrzeug bevorzugen (sofern nicht mehr als 5 Felder weiter als der beste Shop entfernt)
  IF bestshop& >= 0 THEN
    x& = channels(chnr&).shops(bestshop&).position
    y& = channels(chnr&).shops(bestshop&).position2
  ELSE
    IF bestunit& >= 0 THEN
      x& = channels(chnr&).units(bestunit&).xpos
      y& = channels(chnr&).units(bestunit&).ypos
    END IF
  END IF

'IF unitnr& = 167 THEN CALL BILog("Refuel:" +FORMAT$(x&)+","+FORMAT$(y&)+" / "+FORMAT$(bestshop&), 0)
END SUB



'Prüft ob eine Einheit gar keine Munition mehr hat
FUNCTION AIUnitOutofAmmo&(chnr&, unitnr&)
  LOCAL unittp&, weaponnr&, ammomask&, ammo&
  LOCAL w AS TWeapon

  'alle Waffen prüfen
  ammomask& = %WP_HIGHAIR OR %WP_LOWAIR OR %WP_LAND OR %WP_WATER OR %WP_UNDERWATER
  unittp& = channels(chnr&).units(unitnr&).unittype
  FOR weaponnr& = 0 TO 3
    w = channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&)
    IF w.damage > 0 AND (w.targets AND ammomask&) <> 0 THEN ammo& = ammo&+w.ammo
  NEXT weaponnr&

  AIUnitOutofAmmo& = IIF&(ammo& = 0, -1, 0)
END FUNCTION



'Prüft ob eine Einheit noch (genug) Munition hat
FUNCTION AIUnitLowOnAmmo&(chnr&, unitnr&)
  LOCAL unittp&, weaponnr&, bestweapon&, bestdamage&

  'prüfen, ob die beste Waffe noch Munition hat
  bestweapon& = -1
  unittp& = channels(chnr&).units(unitnr&).unittype
  FOR weaponnr& = 0 TO 3
    IF channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).ammo > 0 AND channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).damage > bestdamage& THEN
      bestdamage& = channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).damage
      bestweapon& = weaponnr&
    END IF
  NEXT weaponnr&
  IF bestweapon& = -1 THEN EXIT FUNCTION

  AIUnitLowOnAmmo& = IIF&(channels(chnr&).units(unitnr&).ammo(bestweapon&) = 0, -1, 0)
END FUNCTION



'Prüft ob eine Einheit noch (genug) Treibstoff hat
FUNCTION AIUnitLowOnFuel&(chnr&, unitnr&)
  LOCAL unittp&, fuellevel!

  unittp& = channels(chnr&).units(unitnr&).unittype
  IF channelsnosave(chnr&).unitclasses(unittp&).fuel = 0 THEN
    fuellevel! = 1
  ELSE
    fuellevel! = channels(chnr&).units(unitnr&).fuel/channelsnosave(chnr&).unitclasses(unittp&).fuel
  END IF

  AIUnitLowOnFuel& = IIF&(fuellevel! < 0.7, -1, 0)
END FUNCTION



'Prüft ob eine Einheit repariert werden muß
FUNCTION AIUnitNeedsRepair&(chnr&, unitnr&)
  LOCAL unittp&

  unittp& = channels(chnr&).units(unitnr&).unittype
  AIUnitNeedsRepair& = IIF&(channels(chnr&).units(unitnr&).groupsize < channelsnosave(chnr&).unitclasses(unittp&).groupsize, -1, 0)
END FUNCTION



'Simuliert einen Kampf
FUNCTION AISimulateCombat&(chnr&, attacker&, defender&)
  LOCAL attackerunittp&, defenderunittp&, rating&, damagedone!, damagetaken!

  'Kampf simulieren
  CALL SetCombatData(chnr&, attacker&, defender&, -1, -1, 1.0)

  'Kampfergebnis auswerten
  attackerunittp& = channels(chnr&).units(attacker&).unittype
  defenderunittp& = channels(chnr&).units(defender&).unittype
  damagedone! = channels(chnr&).combat.params(5, 1)/channelsnosave(chnr&).unitclasses(defenderunittp&).groupsize
  damagetaken! = channels(chnr&).combat.params(5, 0)/channelsnosave(chnr&).unitclasses(attackerunittp&).groupsize
  rating& = damagedone!*100-damagetaken!*100  'prozentual genommenen Schaden ins Verhältnis zum prozentual zugefügten Schaden setzen
  IF channels(chnr&).combat.params(5, 1) >= channels(chnr&).units(defender&).groupsize THEN rating& = rating&+80  'wenn Gegner vernichtet wird, zählt dies wie 8 zusätzliche Punkte Schaden

  AISimulateCombat& = rating&
END FUNCTION



'Liefert den Gegner mit der höchsten Priorität für eine Einheit
FUNCTION AIGetPrioTarget&(chnr&, unitnr&)
  LOCAL enemy&, besttarget&, highestprio&, z&, unittp&, weaponnr&

  'alle sichtbaren Gegner prüfen
  besttarget& = -1
  FOR enemy& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(enemy&).aitargetpriority > highestprio& THEN
      'prüfen, ob diese Einheit eine passende Waffen hat, um den Gegner anzugreifen
      z& = 2^channels(chnr&).units(enemy&).zpos
      unittp& = channels(chnr&).units(unitnr&).unittype
      FOR weaponnr& = 0 TO 3
        IF channels(chnr&).units(unitnr&).ammo(weaponnr&) > 0 AND channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).damage > 0 AND (channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).targets AND z&) <> 0 THEN EXIT FOR
      NEXT weaponnr&
      IF weaponnr& < 4 AND AISimulateCombat&(chnr&, unitnr&, enemy&) >= 0 THEN
        highestprio& = channels(chnr&).units(enemy&).aitargetpriority
        besttarget& = enemy&
      END IF
    END IF
  NEXT enemy&

  AIGetPrioTarget& = besttarget&
END FUNCTION



'Liefert die dichteste eigene Einheit, die kein identisches Vorsorgungsfahrzeug in ihrer Nähe hat
FUNCTION AIFindOwnUnitToSupport&(chnr&, supportunitnr&)
  LOCAL unitnr&, unittp&, unr&, plnr&, supportx&, supporty&, x&, y&, dist&, bestdist&, bestunit&
  LOCAL targetarea?(), r AS RECT

  plnr& = channels(chnr&).units(supportunitnr&).owner
  unittp& = channels(chnr&).units(supportunitnr&).unittype
  supportx& = channels(chnr&).units(supportunitnr&).xpos
  supporty& = channels(chnr&).units(supportunitnr&).ypos
  bestdist& = 99
  bestunit& = -1

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND UnitIsInShop&(chnr&, unitnr&) = -1 THEN
      'prüfen, ob sich ein identisches Vorsorgungsfahrzeug in der Nähe befindet
      x& = channels(chnr&).units(unitnr&).xpos
      y& = channels(chnr&).units(unitnr&).ypos
      FOR unr& = 0 TO channels(chnr&).info.nunits-1
        IF channels(chnr&).units(unr&).owner = plnr& AND UnitIsAlive&(chnr&, unr&) <> 0 AND UnitIsInShop&(chnr&, unr&) = -1 AND unr& <> supportunitnr& AND channels(chnr&).units(unr&).unittype = unittp& THEN
          IF ABS(x&-channels(chnr&).units(unr&).xpos) < 5 AND ABS(y&-channels(chnr&).units(unr&).ypos) < 5 THEN EXIT FOR
        END IF
      NEXT unr&
      IF unr& = channels(chnr&).info.nunits THEN
        'eigene Einheit ohne Versorgungsfahrzeug gefunden
        dist& = GetDistance&(x&, y&, supportx&, supporty&)
        IF dist& < bestdist& THEN
          bestdist& = dist&
          bestunit& = unitnr&
        END IF
      END IF
    END IF
  NEXT unitnr&

  AIFindOwnUnitToSupport& = bestunit&
END FUNCTION



'Ermittelt das Feld, auf dem die AI Siegbedingung zu erfüllen ist
SUB AIFindVictoryPosition(BYVAL chnr&, BYVAL plnr&)
  LOCAL vcat&, vc&, pl&, shopnr&, shopowner&, unittp&, unitnr&

  'alle Siegbedingungen prüfen
  vcat& = AIGetVictoryCategory&(chnr&, plnr&)
  FOR vc& = 0 TO channels(chnr&).info.nvictoryconditions-1
    IF channels(chnr&).actions(vc&).category = vcat& THEN
      SELECT CASE channels(chnr&).actions(vc&).actiontype
      CASE %ACTYPE_VC_SHOPOCCUPIED    'Shop erobert
        pl& = channels(chnr&).actions(vc&).actionparam
        shopnr& = channels(chnr&).actions(vc&).shop
        shopowner& = channels(chnr&).shops(shopnr&).owner
        IF pl& = plnr& AND (channels(chnr&).player(plnr&).allymask AND 2^shopowner&) = 0 THEN
          'AI muß diesen Shop erobern, um das Spiel zu gewinnen
          channels(chnr&).player(plnr&).aiglobaltargetx = channels(chnr&).shops(shopnr&).position
          channels(chnr&).player(plnr&).aiglobaltargety = channels(chnr&).shops(shopnr&).position2
          EXIT SUB
        END IF

      CASE %ACTYPE_VC_UNITDEAD_PLAYER1 TO %ACTYPE_VC_UNITDEAD_PLAYER6  'bestimmte Einheit dieses Spielers vernichten
        pl& = channels(chnr&).actions(vc&).shop
        IF (channels(chnr&).player(plnr&).allymask AND 2^pl&) = 0 THEN
          unittp& = channels(chnr&).actions(vc&).actionparam
          'AI muß Einheit dieses Typs vernichten
          FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
            IF UnitIsAlive&(chnr&, unitnr&) <> 0 AND UnitIsInShop&(chnr&, unitnr&) = -1 AND channels(chnr&).units(unitnr&).owner = pl& AND channels(chnr&).units(unitnr&).unittype = unittp& THEN
              channels(chnr&).player(plnr&).aiglobaltargetx = channels(chnr&).units(unitnr&).xpos
              channels(chnr&).player(plnr&).aiglobaltargety = channels(chnr&).units(unitnr&).ypos
              EXIT SUB
            END IF
          NEXT unitnr&
        END IF
      END SELECT
    END IF
  NEXT vc&

  'kein Ziel gefunden
  channels(chnr&).player(plnr&).aiglobaltargetx = -1
END SUB



'Sucht nach Zielen für eine Einheit
'md& = 1 : Angriffsziele (die die Einheit in dieser Runde beschießen kann)
'md& = 2 : eigene zu versorgende Einheiten (in Reichweite der Tankfüllung)
'md& = 3 : eigene Einheiten vom selben Typ (Land/Wasser/Luft) (in Reichweite der Tankfüllung)
'md& = 4 : Angriffsziele ohne vorherige Bewegung
FUNCTION AIFindTargets&(chnr&, unitnr&, md&)
  LOCAL plnr&, foundfields&, foundtargets&, rangeoverwrite&
  LOCAL x&, y&, z&, i&, k&, v&, orgx&, orgy&, unittp&, combatresult&, bestresult&, bestposition&, bestenemy&, escortedunit&, friend&, friendtp&, supportresult&, enemy&, unexplored&, mattype&
  LOCAL oldzone3&, oldunitx&, oldunity&, supportunitmask&
  LOCAL reachablefields&(), targetunits&(), xoff&(), yoff&()

  plnr& = channels(chnr&).units(unitnr&).owner
  orgx& = channels(chnr&).units(unitnr&).xpos
  orgy& = channels(chnr&).units(unitnr&).ypos
  unittp& = channels(chnr&).units(unitnr&).unittype

  'falls Einheit als Begleitschutz markiert ist, dann nur Ziele um Umkreis um ihre zu beschützende Einheit berücksichtigen
  escortedunit& = -1
  IF channels(chnr&).units(unitnr&).aicommand = %AIC_ESCORT THEN escortedunit& = channels(chnr&).units(unitnr&).aitargetunit

  'alle erreichbaren Felder ermitteln
  IF md& = 2 OR md& = 3 THEN rangeoverwrite& = channels(chnr&).units(unitnr&).fuel*8/channelsnosave(chnr&).unitclasses(unittp&).fuelusage
  IF md& = 4 THEN
    DIM reachablefields&(0)
  ELSE
    foundfields& = GetTargets&(chnr&, unitnr&, 1, rangeoverwrite&, reachablefields&())
  END IF
  reachablefields&(foundfields&) = orgx&+orgy&*256
  foundfields& = foundfields&+1
  bestposition& = -1
  bestenemy& = -1
  bestresult& = -15
  FOR i& = 0 TO foundfields&-1
    x& = reachablefields&(i&) AND 255
    y& = INT(reachablefields&(i&)/256)
    IF channels(chnr&).zone3(x&, y&) = -1 OR channels(chnr&).zone3(x&, y&) = unitnr& THEN  'nur auf freie Felder ziehen (nicht in Shops/Transporter hinein ziehen)

      SELECT CASE md&
      CASE 1, 4:  'Angriffsziele
        IF channels(chnr&).info.difficulty = %DIFFICULTY_HARD THEN unexplored& = CountAdjacentUnexploredFields&(chnr&, plnr&, x&, y&)
        CALL MoveUnitForTesting(chnr&, unitnr&, x&, y&, oldzone3&, oldunitx&, oldunity&)  'Einheit temporär auf dieses Feld versetzen
        'alle Angriffsziele von dieser Position ermitteln
        foundtargets& = GetTargets&(chnr&, unitnr&, 2, 0, targetunits&())
        FOR k& = 0 TO foundtargets&-1
          enemy& = targetunits&(k&)
          IF IsEnemyUnit&(chnr&, plnr&, enemy&) <> 0 THEN
            'nur Feinde betrachten, da GetTargets&() auch eigene zu versorgende Einheiten zurückliefert
            IF escortedunit& = -1 OR GetDistance&(channels(chnr&).units(escortedunit&).xpos, channels(chnr&).units(escortedunit&).ypos, channels(chnr&).units(enemy&).xpos, channels(chnr&).units(enemy&).ypos) <= 3 THEN
              combatresult& = AISimulateCombat&(chnr&, unitnr&, enemy&)
              IF channels(chnr&).units(enemy&).aitargetpriority > 9999 AND channels(chnr&).combat.params(5, 1) > 0 THEN combatresult& = combatresult&+channels(chnr&).units(enemy&).aitargetpriority/100
              IF unexplored& > 0 THEN combatresult& = combatresult&/2-unexplored&*2  'Felder neben unerforschten Feldern schlechter bewerten
              IF combatresult& > bestresult& THEN
                bestresult& = combatresult&
                bestenemy& = enemy&
                bestposition& = reachablefields&(i&)
              END IF
            END IF
          END IF
        NEXT k&
        CALL UndoMoveUnit(chnr&, unitnr&, oldzone3&, oldunitx&, oldunity&)

      CASE 2:  'eigene zu versorgende Einheiten
        'prüfen, ob (bewaffnete) Feinde an dieses Feld angrenzen
        CALL GetAdjacentFields(chnr&, x&, y&, xoff&(), yoff&())
        v& = channels(chnr&).zone3(x&, y&)
        channels(chnr&).zone3(x&, y&) = unitnr&
        FOR k& = 0 TO 5
          enemy& = channels(chnr&).zone3(xoff&(k&), yoff&(k&))
          IF enemy& >= 0 AND IsEnemyUnit&(chnr&, plnr&, enemy&) <> 0 AND CanAttackField&(chnr&, enemy&, x&, y&) <> 0 THEN EXIT FOR
        NEXT k&
        channels(chnr&).zone3(x&, y&) = v&
        IF k& < 6 THEN ITERATE FOR
        'alle eigenen zu versorgenden Einheiten von dieser Position ermitteln
        mattype& = GetMaterialType&(chnr&, unitnr&)
        FOR k& = 0 TO 5
          IF xoff&(k&) >= 0 THEN
            friend& = channels(chnr&).zone3(xoff&(k&), yoff&(k&))
            IF friend& >= 0 AND channels(chnr&).units(friend&).owner = plnr& AND friend& <> unitnr& AND channels(chnr&).units(friend&).aicommand <> %AIC_REFUELTARGET THEN
              friendtp& = channels(chnr&).units(friend&).unittype
              IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REPAIR) <> 0 AND (channelsnosave(chnr&).unitclasses(friendtp&).flags AND %UCF_PLANE) = 0 AND AIUnitNeedsRepair&(chnr&, friend&) <> 0 THEN
                'Einheit reparieren
                supportresult& = (channelsnosave(chnr&).unitclasses(friendtp&).groupsize-channels(chnr&).units(friend&).groupsize)*10
                IF supportresult& > 0 THEN supportresult& = supportresult&+100-GetDistance&(orgx&, orgy&, xoff&(k&), yoff&(k&))
                IF supportresult& > bestresult& THEN
                  bestresult& = supportresult&
                  bestenemy& = friend&
                  bestposition& = reachablefields&(i&)
                END IF
              END IF
              IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REFUEL) <> 0 AND ((channelsnosave(chnr&).unitclasses(friendtp&).flags AND %UCF_PLANE) = 0 OR (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REFUELAIR) <> 0) THEN
                'Einheit befüllen
                supportresult& = 0
                IF (mattype& AND %WP_FUEL) <> 0 AND AIUnitLowOnFuel&(chnr&, friend&) <> 0 THEN supportresult& = supportresult&+10
                IF (mattype& AND %WP_AMMO) <> 0 AND AIUnitLowOnAmmo&(chnr&, friend&) <> 0 THEN supportresult& = supportresult&+10
                IF supportresult& > 0 THEN supportresult& = supportresult&+100-GetDistance&(orgx&, orgy&, xoff&(k&), yoff&(k&))
                IF supportresult& > 0 AND supportresult& > bestresult& THEN
                  bestresult& = supportresult&
                  bestenemy& = friend&
                  bestposition& = reachablefields&(i&)
                END IF
              END IF
            END IF
          END IF
        NEXT k&

      CASE 3:  'eigene Einheiten vom selben Typ
        z& =  channels(chnr&).units(unitnr&).zpos
        supportunitmask& = %UCF_REPAIR OR %UCF_REFUEL OR %UCF_REFUELAIR OR %UCF_BUILD OR %UCF_FORTIFY
        'prüfen, ob (bewaffnete) Feinde an dieses Feld angrenzen
        CALL GetAdjacentFields(chnr&, x&, y&, xoff&(), yoff&())
        v& = channels(chnr&).zone3(x&, y&)
        channels(chnr&).zone3(x&, y&) = unitnr&
        FOR k& = 0 TO 5
          enemy& = channels(chnr&).zone3(xoff&(k&), yoff&(k&))
          IF enemy& >= 0 AND IsEnemyUnit&(chnr&, plnr&, enemy&) <> 0 AND CanAttackField&(chnr&, enemy&, x&, y&) <> 0 THEN EXIT FOR
        NEXT k&
        channels(chnr&).zone3(x&, y&) = v&
        IF k& < 6 THEN ITERATE FOR
        'alle eigenen Kampf-Einheiten des selben Typs (Land/Wasser/Luft) von dieser Position ermitteln
        FOR k& = 0 TO 5
          IF xoff&(k&) >= 0 THEN
            friend& = channels(chnr&).zone3(xoff&(k&), yoff&(k&))
            IF friend& >= 0 AND channels(chnr&).units(friend&).owner = plnr& AND friend& <> unitnr& AND channels(chnr&).units(friend&).zpos = z& THEN
              friendtp& = channels(chnr&).units(friend&).unittype
              IF (channelsnosave(chnr&).unitclasses(friendtp&).flags AND supportunitmask&) = 0 THEN
                supportresult& = 100-GetDistance&(orgx&, orgy&, xoff&(k&), yoff&(k&))
                IF supportresult& > bestresult& THEN
                  bestresult& = supportresult&
                  bestenemy& = friend&
                  bestposition& = reachablefields&(i&)
                END IF
              END IF
            END IF
          END IF
        NEXT k&

      END SELECT
    END IF
  NEXT i&

  IF bestposition& >= 0 THEN bestposition& = bestposition&+bestenemy&*65536

  AIFindTargets& = bestposition&
END FUNCTION



'Alle Feinde in einem Umkreis um ein Feld suchen
FUNCTION AIFindTargetsInArea&(chnr&, cx&, cy&, range&, targetunits&())
  LOCAL x&, y&, enemy&, foundtargets&
  LOCAL targetarea?(), r AS RECT
  REDIM targetunits&(999)

  'alle Felder im Umkreis ermitteln
  CALL GetHexRange(chnr&, cx&, cy&, range&, targetarea?(), r)
  FOR y& = r.top TO r.bottom
    FOR x& = r.left TO r.right
      IF targetarea?(x&-r.left, y&-r.top) > 0 THEN
        enemy& = channels(chnr&).zone3(x&, y&)
        IF enemy& >= 0 AND channels(chnr&).units(enemy&).aitargetpriority > 0 AND foundtargets& < 1000 THEN
          targetunits&(foundtargets&) = enemy&
          foundtargets& = foundtargets&+1
        END IF
      END IF
    NEXT x&
  NEXT y&

  AIFindTargetsInArea& = foundtargets&
END FUNCTION



'Zählt alle eigenen Einheiten eines Typs in einem Umkreis um ein Feld (inklusive Einheiten in Shops)
FUNCTION AICountOwnUnitsOfType&(chnr&, plnr&, unittp&, cx&, cy&, range&)
  LOCAL x&, y&, n&, i&, unitnr&, shopnr&
  LOCAL targetarea?(), r AS RECT

  'alle Felder im Umkreis ermitteln
  CALL GetHexRange(chnr&, cx&, cy&, range&, targetarea?(), r)
  FOR y& = r.top TO r.bottom
    FOR x& = r.left TO r.right
      IF targetarea?(x&-r.left, y&-r.top) > 0 THEN
        unitnr& = channels(chnr&).zone3(x&, y&)
        IF unitnr& >= 0 AND channels(chnr&).units(unitnr&).owner = plnr& AND channels(chnr&).units(unitnr&).unittype = unittp& THEN n& = n&+1
        IF unitnr& < -1 THEN
          shopnr& = -2-unitnr&
          IF channels(chnr&).shops(shopnr&).owner = plnr& THEN
            FOR i& = 0 TO 15
              unitnr& = channels(chnr&).shops(shopnr&).content(i&)
              IF unitnr& >= 0 AND channels(chnr&).units(unitnr&).unittype = unittp& THEN n& = n&+1
            NEXT i&
          END IF
        END IF
      END IF
    NEXT x&
  NEXT y&

  AICountOwnUnitsOfType& = n&
END FUNCTION



'Kampf starten
SUB AIStartCombat(chnr&, plnr&, attacker&, defender&)
  LOCAL dist&

  dist& = GetDistance&(channels(chnr&).units(attacker&).xpos, channels(chnr&).units(attacker&).ypos, channels(chnr&).units(defender&).xpos, channels(chnr&).units(defender&).ypos)
  IF gameMode& = %GAMEMODE_SERVER THEN
    CALL SetCombatData(chnr&, attacker&, defender&, -1, IIF&(dist& = 1, -1, 0), 0)
    IF UnitCanRetreat&(chnr&, attacker&) = 0 THEN CALL UnitDone(chnr&, attacker&)
    CALL ResetClientAnimationFlag(chnr&)
    CALL SendPacketToChannel(chnr&, "ATCK", PEEK$(VARPTR(channels(chnr&).combat), SIZEOF(TCombatInfo)))
    CALL EndCombat(chnr&, channels(chnr&).combat)
    CALL WaitUntilClientsFinishedAnimation(chnr&)
  ELSE
    CALL StartCombat(chnr&, attacker&, defender&, -1, IIF&(dist& = 1, -1, 0), 0)
    CALL AIWaitEndCombat(chnr&)
    IF UnitCanRetreat&(chnr&, attacker&) = 0 THEN CALL UnitDone(chnr&, attacker&)
  END IF
  CALL AISetTargetPriority(chnr&, plnr&, attacker&)
  CALL AISetTargetPriority(chnr&, plnr&, defender&)
END SUB



'Einheit bewegen
SUB AIMoveUnit(chnr&, plnr&, unitnr&, newx&, newy&)
  LOCAL x&, y&, targetunit&, unittp&, friendtp&, shopnr&, transporter&, supportunitmask&, tg&

  'Einheit bewegen
  supportunitmask& = %UCF_REPAIR OR %UCF_REFUEL OR %UCF_REFUELAIR OR %UCF_BUILD OR %UCF_FORTIFY
  x& = channels(chnr&).units(unitnr&).xpos
  y& = channels(chnr&).units(unitnr&).ypos
  unittp& = channels(chnr&).units(unitnr&).unittype
  IF x& <> newx& OR y& <> newy& THEN
    CALL EnterSemaphore(semaphore_unitmoving&)
    channels(chnr&).player(plnr&).selectedunit = unitnr&
    CALL CalculateUnitPath(chnr&, unitnr&, newx&, newy&, 0)
    IF channels(chnr&).zone3(x&, y&) = unitnr& THEN
      channels(chnr&).zone3(x&, y&) = -1  'Einheit temporär von der Karte entfernen (wird am Ende der Animation wieder auf dem Zielfeld platziert)
    ELSE
      shopnr& = UnitIsInShop&(chnr&, unitnr&)
      IF shopnr& >= 0 THEN
        CALL RemoveUnitFromShop(chnr&, shopnr&, unitnr&)
      ELSE
        transporter& = UnitIsInTransporter&(chnr&, unitnr&)
        IF transporter& >= 0 THEN CALL TransportUnload(chnr&, transporter&, unitnr&)
      END IF
    END IF
    CALL SetPhase(chnr&, plnr&, %PHASE_UNITMOVING)
    CALL LeaveSemaphore(semaphore_unitmoving&)
    IF gameMode& = %GAMEMODE_SERVER THEN
'      IF channels(chnr&).zone3(newx&, newy&) = -1 THEN channels(chnr&).zone3(newx&, newy&) = unitnr&  'Einheit auf dem Server bereits auf dem Zielfeld platzieren, damit die Spieler nicht zeitgleich ebenfalls auf das Feld ziehen
      CALL ServerMoveUnit(-1, chnr&, plnr&, MKL$(unitnr&)+CHR$(newx&)+CHR$(newy&))
'      CALL ResetClientAnimationFlag(chnr&)
'      CALL SendPacketToChannel(chnr&, "MOVE", MKL$(unitnr&)+CHR$(newx&)+CHR$(newy&))
      CALL WaitUntilClientsFinishedAnimation(chnr&)
'      CALL EndMovement(chnr&, unitnr&, newx&, newy&)
    ELSE
      CALL AIWaitEndMovement(chnr&)
      CALL AIWaitEndMessage(chnr&)
    END IF
  END IF

  'Ziel angreifen
  IF (channels(chnr&).units(unitnr&).flags AND %US_DONE+%US_AI_IGNORE) = 0 THEN
    targetunit& = channels(chnr&).units(unitnr&).aitargetunit
    IF targetunit& = -1 AND channels(chnr&).units(unitnr&).aicommand = %AIC_NONE AND (channelsnosave(chnr&).unitclasses(unittp&).flags AND supportunitmask&) = 0 THEN
      'falls Einheit ohne Befehl seine Bewegung zufälligerweise neben einem Gegner beendet, dann prüfen, ob dieser angegriffen werden kann
      IF (channels(chnr&).units(unitnr&).flags AND %US_ATTACKED) = 0 AND (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_COOLDOWN) = 0 THEN
        tg& = AIFindTargets&(chnr&, unitnr&, 4)
        IF tg& >= 0 THEN
          channels(chnr&).units(unitnr&).aicommand = %AIC_ATTACK
          channels(chnr&).units(unitnr&).aitargetunit = INT(tg&/65536)
          targetunit& = channels(chnr&).units(unitnr&).aitargetunit
        END IF
      END IF
    END IF
    IF targetunit& >= 0 AND channels(chnr&).units(unitnr&).aicommand = %AIC_ATTACK THEN
      CALL AIStartCombat(chnr&, plnr&, unitnr&, targetunit&)
      IF GetPhase&(chnr&, plnr&) <= %PHASE_UNITSELECTED AND UnitIsAlive&(chnr&, unitnr&) <> 0 _
        AND (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_RETREAT) <> 0 AND channels(chnr&).units(unitnr&).moved < channelsnosave(chnr&).unitclasses(unittp&).range/8 THEN
        CALL AIRetreat(chnr&, unitnr&)
      END IF
    END IF
  END IF

  'Ziel versorgen
  IF targetunit& >= 0 AND channels(chnr&).units(unitnr&).aicommand = %AIC_REFUEL AND (channels(chnr&).units(unitnr&).flags AND %US_DONE+%US_AI_IGNORE) = 0 THEN
    friendtp& = channels(chnr&).units(targetunit&).unittype
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REPAIR) <> 0 AND (channelsnosave(chnr&).unitclasses(friendtp&).flags AND %UCF_PLANE) = 0 AND AIUnitNeedsRepair&(chnr&, targetunit&) <> 0 THEN
      'Einheit reparieren
      CALL AIRepairUnit(chnr&, unitnr&, targetunit&)
    ELSE
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REFUEL) <> 0 AND ((channelsnosave(chnr&).unitclasses(friendtp&).flags AND %UCF_PLANE) = 0 OR (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REFUELAIR) <> 0) THEN
        'Einheit befüllen
        IF AIUnitLowOnAmmo&(chnr&, targetunit&) OR AIUnitLowOnFuel&(chnr&, targetunit&) THEN CALL AIRefuelUnit(chnr&, unitnr&, targetunit&)
      END IF
    END IF
  END IF
END SUB



'Einheit näher an Zielkoordinaten heran bewegen
SUB AIMoveCloserTo(BYVAL chnr&, BYVAL unitnr&, BYVAL destx&, BYVAL desty&)
  LOCAL i&, plnr&, foundfields&, x&, y&, dist&, bestdist&, bestx&, besty&, validfield&
  LOCAL reachablefields&()

  'alle in dieser Runde erreichbaren Felder ermitteln
  plnr& = channels(chnr&).units(unitnr&).owner
  foundfields& = GetTargets&(chnr&, unitnr&, 1, 0, reachablefields&())

  'Feld ermitteln, was am dichtesten am Zielfeld liegt
  bestx& = -1
  bestdist& = GetDistance&(channels(chnr&).units(unitnr&).xpos, channels(chnr&).units(unitnr&).ypos, destx&, desty&)
  FOR i& = 0 TO foundfields&-1
    x& = reachablefields&(i&) AND 255
    y& = INT(reachablefields&(i&)/256)
    'nur auf freie Felder ziehen (nicht in Shops/Transporter hinein ziehen - es sei denn dies ist das Zielfeld)
    validfield& = 0
    IF channels(chnr&).zone3(x&, y&) = -1 THEN validfield& = 1
    IF x& = destx& AND y& = desty& THEN validfield& = 1
    IF channels(chnr&).zone3(x&, y&) < -1 AND channels(chnr&).units(unitnr&).aicommand = %AIC_OCCUPY THEN validfield& = 1
    IF channels(chnr&).info.difficulty = %DIFFICULTY_HARD AND CountAdjacentUnexploredFields&(chnr&, plnr&, x&, y&) > 0 THEN validfield& = 0  'im schweren Modus nicht auf Felder neben unerforschten Feldern ziehen
    IF validfield& <> 0 THEN
      dist& = GetDistance&(x&, y&, destx&, desty&)
      IF dist& < bestdist& THEN
        bestdist& = dist&
        bestx& = x&
        besty& = y&
      END IF
    END IF
  NEXT i&

  'Einheit auf das gefundene Feld bewegen
  IF bestx& >= 0 THEN
    CALL AIMoveUnit(chnr&, plnr&, unitnr&, bestx&, besty&)
  ELSE
    CALL UnitDone(chnr&, unitnr&)
  END IF
END SUB



'Aufklärer bewegen
SUB AIMoveRecon(chnr&, plnr&)
  LOCAL i&, k&, unitnr&, unittp&, x&, y&, enemy&, bestenemy&, enemyx&, enemyy&, dist&, bestdist&, bestpos&, foundfields&
  LOCAL reachablefields&(), xoff&(), yoff&()

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND (channels(chnr&).units(unitnr&).flags AND %US_DONE+%US_AI_IGNORE) = 0 _
       AND channels(chnr&).units(unitnr&).aicommand = %AIC_NONE AND UnitIsInShop&(chnr&, unitnr&) = -1 THEN
      unittp& = channels(chnr&).units(unitnr&).unittype
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_RECON) <> 0 AND channels(chnr&).units(unitnr&).fuel > 20 THEN
        'Aufklärer mit ausreichend Treibstoff gefunden
        foundfields& = GetTargets&(chnr&, unitnr&, 1, 0, reachablefields&())
        IF foundfields& = 0 THEN ITERATE FOR
        x& = channels(chnr&).units(unitnr&).xpos
        y& = channels(chnr&).units(unitnr&).ypos

        'dichtesten Gegner suchen (inklusive Gegner in Gebäuden)
        bestdist& = 999
        bestenemy& = -1
        FOR enemy& = 0 TO channels(chnr&).info.nunits-1
          IF UnitIsAlive&(chnr&, enemy&) <> 0 AND IsEnemyUnit&(chnr&, plnr&, enemy&) <> 0 THEN
            dist& = GetDistance&(x&, y&, channels(chnr&).units(enemy&).xpos, channels(chnr&).units(enemy&).ypos)
            IF dist& < bestdist& THEN
              bestdist& = dist&
              bestenemy& = enemy&
            END IF
          END IF
        NEXT enemy&
        IF bestenemy& = -1 THEN EXIT SUB

        'Aufklärer auf das Feld ziehen, was am dichtesten am nähesten Gegner ist, aber keinen Feind berührt
        enemyx& = channels(chnr&).units(bestenemy&).xpos
        enemyy& = channels(chnr&).units(bestenemy&).ypos
        bestdist& = 999
        bestpos& = -1
        FOR i& = 0 TO foundfields&-1
          x& = reachablefields&(i&) AND 255
          y& = INT(reachablefields&(i&)/256)
          IF channels(chnr&).zone3(x&, y&) = -1 THEN  'nicht in Gebäude oder Transporter hinein ziehen
            dist& = GetDistance&(x&, y&, enemyx&, enemyy&)
            IF dist& < bestdist& THEN
              'angrenzende Feinde prüfen
              CALL GetAdjacentFields(chnr&, x&, y&, xoff&(), yoff&())
              FOR k& = 0 TO 5
                IF xoff&(k&) >= 0 THEN
                  enemy& = channels(chnr&).zone3(xoff&(k&), yoff&(k&))
                  IF IsEnemyUnit&(chnr&, plnr&, enemy&) <> 0 THEN EXIT FOR
                END IF
              NEXT k&
              IF k& = 6 THEN
                bestdist& = dist&
                bestpos& = i&
              END IF
            END IF
          END IF
        NEXT i&

        'Aufklärer bewegen
        IF bestpos& >= 0 THEN
          x& = reachablefields&(bestpos&) AND 255
          y& = INT(reachablefields&(bestpos&)/256)
          channels(chnr&).units(unitnr&).aicommand = %AIC_RECON
          CALL AIMoveUnit(chnr&, plnr&, unitnr&, x&, y&)
        END IF
      END IF
    END IF
  NEXT unitnr&
END SUB



'Gefährdete Einheiten bewegen
SUB AIMoveEndangered(chnr&, plnr&)
  LOCAL i&, j&, k&, x&, y&, unitnr&, enemy&, foundfields&, enemyrange2&, enemyrange3&, bestx&, besty&, minenemies&
  LOCAL reachablefields&(), reachablearea?()
  LOCAL r AS RECT

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND (channels(chnr&).units(unitnr&).flags AND %US_DONE+%US_AI_IGNORE) = 0 _
       AND channels(chnr&).units(unitnr&).aicommand = %AIC_ENDANGERED AND UnitIsInShop&(chnr&, unitnr&) = -1 THEN
      'Einheit auf ein Feld bewegen, was im Umkreis von 2 Feldern keine Gegner hat, aber im Umkreis von 3 Feldern
      foundfields& = GetTargets&(chnr&, unitnr&, 1, 0, reachablefields&())
      FOR i& = 0 TO foundfields&-1
        x& = reachablefields&(i&) AND 255
        y& = INT(reachablefields&(i&)/256)
        IF channels(chnr&).zone3(x&, y&) = -1 THEN
          'kampfbereite Gegner im Umkreis von 2 Feldern zählen
          enemyrange2& = 0
          CALL GetHexRange(chnr&, x&, y&, 2, reachablearea?(), r)
          FOR k& = r.top TO r.bottom
            FOR j& = r.left TO r.right
              IF reachablearea?(j&-r.left, k&-r.top) > 0 THEN
                enemy& = channels(chnr&).zone3(j&, k&)
                IF enemy& >= 0 AND IsEnemyUnit&(chnr&, plnr&, enemy&) <> 0 AND CanAttackUnitIgnoreDistance&(chnr&, enemy&, unitnr&) <> 0 THEN enemyrange2& = enemyrange2&+1
              END IF
            NEXT j&
          NEXT k&

          'Gegner im Umkreis von 3 Feldern zählen
          enemyrange3& = 0
          CALL GetHexRange(chnr&, x&, y&, 3, reachablearea?(), r)
          FOR k& = r.top TO r.bottom
            FOR j& = r.left TO r.right
              IF reachablearea?(j&-r.left, k&-r.top) > 0 THEN
                enemy& = channels(chnr&).zone3(j&, k&)
                IF enemy& >= 0 AND IsEnemyUnit&(chnr&, plnr&, enemy&) <> 0 AND CanAttackUnitIgnoreDistance&(chnr&, enemy&, unitnr&) <> 0 THEN enemyrange3& = enemyrange3&+1
              END IF
            NEXT j&
          NEXT k&

          'wenn Feld die Kriterien erfüllt, dann Einheit dorthin bewegen
          IF enemyrange2& = 0 AND enemyrange3& > 0 THEN
            CALL AIMoveUnit(chnr&, plnr&, unitnr&, x&, y&)
            EXIT FOR
          END IF
        END IF
      NEXT i&

      'falls kein passendes Feld gefunden wurde, dann Einheit auf ein sicheres Feld bewegen
      IF i& = foundfields& THEN
        bestx& = -1
        minenemies& = 99
        FOR i& = 0 TO foundfields&-1
          x& = reachablefields&(i&) AND 255
          y& = INT(reachablefields&(i&)/256)
          IF channels(chnr&).zone3(x&, y&) = -1 THEN
            'kampfbereite Gegner im Umkreis von 3 Feldern zählen
            enemyrange3& = 0
            CALL GetHexRange(chnr&, x&, y&, 3, reachablearea?(), r)
            FOR k& = r.top TO r.bottom
              FOR j& = r.left TO r.right
                IF reachablearea?(j&-r.left, k&-r.top) > 0 THEN
                  enemy& = channels(chnr&).zone3(j&, k&)
                  IF enemy& >= 0 AND IsEnemyUnit&(chnr&, plnr&, enemy&) <> 0 AND CanAttackUnitIgnoreDistance&(chnr&, enemy&, unitnr&) <> 0 THEN enemyrange3& = enemyrange3&+1
                END IF
              NEXT j&
            NEXT k&
            IF enemyrange3& < minenemies& THEN
              minenemies& = enemyrange3&
              bestx& = x&
              besty& = y&
              IF minenemies& = 0 THEN EXIT FOR
            END IF
          END IF
        NEXT i&

        'Einheit auf das sicherste Feld bewegen
        IF bestx& >= 0 THEN CALL AIMoveUnit(chnr&, plnr&, unitnr&, bestx&, besty&)
      END IF
    END IF
  NEXT unitnr&
END SUB



'Eigene Einheit suchen, die ein Feld noch diese Runde im Nahkampf angreifen kann
FUNCTION AIFindUnitForMeleeAttackField&(chnr&, plnr&, xenemy&, yenemy&, xattackfromhere&, yattackfromhere&)
  LOCAL i&, unitnr&, unittp&, supportunitmask&, enemy&, oldzone3&, oldunitx&, oldunity&, foundfields&, hp!
  LOCAL reachablefields&()

  supportunitmask& = %UCF_REPAIR OR %UCF_REFUEL OR %UCF_REFUELAIR OR %UCF_BUILD OR %UCF_FORTIFY
  enemy& = channels(chnr&).zone3(xenemy&, yenemy&)

  'Kampfeinheit suchen, die noch mindestes 50% Lebenspunkte hat und diese Runde noch das Zielfeld angreifen kann
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND (channels(chnr&).units(unitnr&).flags AND %US_DONE+%US_AI_IGNORE) = 0 _
       AND channels(chnr&).units(unitnr&).aicommand = %AIC_NONE AND UnitIsInShop&(chnr&, unitnr&) = -1 THEN
      unittp& = channels(chnr&).units(unitnr&).unittype
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND supportunitmask&) = 0 THEN
        'Kampfeinheit gefunden
        hp! = channels(chnr&).units(unitnr&).groupsize / channelsnosave(chnr&).unitclasses(unittp&).groupsize
        IF hp! > 0.49 THEN
          'prüfen, ob diese Einheit sich auf das Angriffsfeld bewegen kann
          CALL ClearTargets(chnr&, plnr&)
          foundfields& = GetTargets&(chnr&, unitnr&, 1, 0, reachablefields&())
          FOR i& = 0 TO foundfields&-1
            IF reachablefields&(i&) = xattackfromhere&+yattackfromhere&*256 THEN EXIT FOR
          NEXT i&
          IF i& < foundfields& THEN
            'prüfen, of diese Einheit das Zielfeld angreifen kann
            CALL MoveUnitForTesting(chnr&, unitnr&, xattackfromhere&, yattackfromhere&, oldzone3&, oldunitx&, oldunity&)  'Einheit temporär auf dieses Feld versetzen
            IF CanAttackField&(chnr&, unitnr&, xenemy&, yenemy&) <> 0 AND AISimulateCombat&(chnr&, unitnr&, enemy&) >= 20 THEN
              CALL UndoMoveUnit(chnr&, unitnr&, oldzone3&, oldunitx&, oldunity&)
              AIFindUnitForMeleeAttackField& = unitnr&
              EXIT FUNCTION
            END IF
            CALL UndoMoveUnit(chnr&, unitnr&, oldzone3&, oldunitx&, oldunity&)
          END IF
        END IF
      END IF
    END IF
  NEXT unitnr&


  AIFindUnitForMeleeAttackField& = -1
END FUNCTION



'Position zum Klemmen suchen
FUNCTION AIFindHemPosition&(BYVAL chnr&, BYVAL unitnr&, BYREF hemx&, BYREF hemy&)
  LOCAL plnr&, i&, k&, x&, y&, foundfields&, enemy&, friend&, xbehind&, ybehind&, oldzone3&, oldunitx&, oldunity&
  LOCAL reachablefields&(), xoff&(), yoff&()

  plnr& = channels(chnr&).units(unitnr&).owner
  CALL ClearTargets(chnr&, plnr&)
  foundfields& = GetTargets&(chnr&, unitnr&, 1, 0, reachablefields&())

  'prüfen, ob eines der erreichbaren Felder an einen Gegner angrenzt, den diese Einheit angreifen könnte
  FOR i& = 0 TO foundfields&-1
    x& = reachablefields&(i&) AND 255
    y& = INT(reachablefields&(i&)/256)
    IF channels(chnr&).zone3(x&, y&) = -1 AND CountAdjacentUnexploredFields&(chnr&, plnr&, x&, y&) = 0 THEN  'nur auf freie Felder ziehen (nicht in Shops/Transporter hinein ziehen), die nicht an unerforschte Felder angrenzen
      CALL GetAdjacentFields(chnr&, x&, y&, xoff&(), yoff&())
      CALL MoveUnitForTesting(chnr&, unitnr&, x&, y&, oldzone3&, oldunitx&, oldunity&)  'Einheit temporär auf dieses Feld versetzen
      FOR k& = 0 TO 5
        enemy& = channels(chnr&).zone3(xoff&(k&), yoff&(k&))
        IF enemy& >= 0 AND IsEnemyUnit&(chnr&, plnr&, enemy&) <> 0 AND CanAttackField&(chnr&, unitnr&, xoff&(k&), yoff&(k&)) <> 0 THEN
          'weitere eigene Einheit suchen, die diesen Feind noch diese Runde im Nahkampf vom gegenüberliegenden Feld angreifen kann
          CALL GetFieldBehindField(chnr&, xoff&(k&), yoff&(k&), x&, y&, xbehind&, ybehind&)
          IF xbehind& >= 0 AND channels(chnr&).zone3(xbehind&, ybehind&) = -1 THEN
            friend& = AIFindUnitForMeleeAttackField&(chnr&, plnr&, xoff&(k&), yoff&(k&), xbehind&, ybehind&)
            IF friend& >= 0 THEN EXIT FOR
          END IF
        END IF
      NEXT k&
      CALL UndoMoveUnit(chnr&, unitnr&, oldzone3&, oldunitx&, oldunity&)
      'falls passendes Feld gefunden wurde, dann dieses zurückliefern
      IF k& < 6 THEN
        hemx& = xoff&(k&)
        hemy& = yoff&(k&)
        AIFindHemPosition& = -1
        EXIT FUNCTION
      END IF
    END IF
  NEXT i&

  AIFindHemPosition& = 0
END FUNCTION



'Einheit nach Kampf zurückziehen
SUB AIRetreat(chnr&, unitnr&)
  LOCAL plnr&, i&, k&, x&, y&, foundfields&, enemy&, friend&, xbehind&, ybehind&, oldzone3&, oldunitx&, oldunity&
  LOCAL reachablefields&(), xoff&(), yoff&()

  SELECT CASE channels(chnr&).info.difficulty
  CASE %DIFFICULTY_EASY
    'im leichten Schwierigkeitsgrad stehenbleiben

  CASE %DIFFICULTY_NORMAL
    'im normalen Schwierigkeitsgrad zur Ausgangsposition zurückziehen
    CALL AIMoveCloserTo(chnr&, unitnr&, channels(chnr&).units(unitnr&).xposold, channels(chnr&).units(unitnr&).yposold)

  CASE %DIFFICULTY_HARD
    'im schweren Modus Gegner klemmen oder zur Ausgangsposition zurückziehen
    IF AIFindHemPosition&(chnr&, unitnr&, x&, y&) <> 0 THEN
      CALL AIMoveCloserTo(chnr&, unitnr&, x&, y&)
    ELSE
      'kein Ziel gefunden, daher Einheit zur Ausgangsposition zurückziehen
      CALL AIMoveCloserTo(chnr&, unitnr&, channels(chnr&).units(unitnr&).xposold, channels(chnr&).units(unitnr&).yposold)
    END IF

  END SELECT

  CALL UnitDone(chnr&, unitnr&)
END SUB



'Generalangriff mit allen nicht-Versorgungseinheiten planen
SUB AIMainAttack(chnr&, plnr&)
  LOCAL unitnr&, unittp&, supportunitmask&, x&, y&, tg&, hp!, totalprogress&, curprogress&

  supportunitmask& = %UCF_REPAIR OR %UCF_REFUEL OR %UCF_REFUELAIR OR %UCF_BUILD OR %UCF_FORTIFY

  'eigene Einheiten ohne Befehl zählen
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND (channels(chnr&).units(unitnr&).flags AND %US_DONE+%US_AI_IGNORE) = 0 _
      AND channels(chnr&).units(unitnr&).aicommand = %AIC_NONE AND UnitIsInShop&(chnr&, unitnr&) = -1 THEN
      totalprogress& = totalprogress&+1
    END IF
  NEXT i&

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).info.state >= %CHANNELSTATE_VICTORY AND channels(chnr&).info.state <= %CHANNELSTATE_DEFEAT THEN EXIT SUB
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND (channels(chnr&).units(unitnr&).flags AND %US_DONE+%US_AI_IGNORE) = 0 _
      AND channels(chnr&).units(unitnr&).aicommand = %AIC_NONE AND UnitIsInShop&(chnr&, unitnr&) = -1 THEN
      unittp& = channels(chnr&).units(unitnr&).unittype
      curprogress& = curprogress&+1
      IF gameMode& = %GAMEMODE_SERVER THEN
        CALL SendAIProgress(chnr&, plnr&, curprogress&, totalprogress&)
      ELSE
        CALL ShowAIProgress(words$$(%WORD_WORKING_ON_UNIT)+" "+FORMAT$(unitnr&), channels(0).info.activeplayer, curprogress&, totalprogress&)
      END IF
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND supportunitmask&) = 0 THEN
        'Kampfeinheit gefunden
        hp! = channels(chnr&).units(unitnr&).groupsize / channelsnosave(chnr&).unitclasses(unittp&).groupsize
        IF hp! < 0.35 THEN
          'Einheit zum reparieren zurückziehen
          channels(chnr&).units(unitnr&).aicommand = %AIC_REPAIR
          CALL AIFindNearestRepairstation(chnr&, plnr&, unitnr&, x&, y&)
          IF x& >= 0 THEN
            CALL AIMoveCloserTo(chnr&, unitnr&, x&, y&)
          ELSE
            'Einheit von der Front zurückziehen oder noch einmal zum Klemmen benutzen
            IF channels(chnr&).info.difficulty = %DIFFICULTY_HARD AND AIFindHemPosition&(chnr&, unitnr&, x&, y&) <> 0 THEN
              CALL AIMoveCloserTo(chnr&, unitnr&, x&, y&)
            ELSE
              channels(chnr&).units(unitnr&).aicommand = %AIC_ENDANGERED
            END IF
          END IF
        ELSE
          IF AIUnitLowOnAmmo&(chnr&, unitnr&) <> 0 THEN
            'Einheit zum befüllen zurückziehen
            channels(chnr&).units(unitnr&).aicommand = %AIC_REFUEL
            CALL AIFindNearestRefuelstation(chnr&, plnr&, unitnr&, x&, y&)
            IF x& >= 0 THEN
              CALL AIMoveCloserTo(chnr&, unitnr&, x&, y&)
            ELSE
              IF AIUnitOutofAmmo&(chnr&, unitnr&) <> 0 THEN
                'Einheit hat keine Munition mehr und kann auch nicht befüllt werden -> Einheit stehen lassen
              ELSE
                GOTO AIMainAttack1
              END IF
            END IF
          ELSE
            'nach Zielen in Reichweite suchen
            AIMainAttack1:
            tg& = AIFindTargets&(chnr&, unitnr&, 1)
            IF tg& >= 0 THEN
              channels(chnr&).units(unitnr&).aicommand = %AIC_ATTACK
              x& = tg& AND 255
              y& = INT(tg&/256) AND 255
              channels(chnr&).units(unitnr&).aitargetunit = INT(tg&/65536)
              CALL AIMoveUnit(chnr&, plnr&, unitnr&, x&, y&)
            ELSE
              'Einheit dichter an die Front heran bewegen
              IF channels(chnr&).units(unitnr&).aimissioncommand = %AIMC_DEFEND THEN
                'Einheit wird nur bewegt, wenn Ziel in Angriffsreichweite ist
                CALL UnitDone(chnr&, unitnr&)
              ELSE
                tg& = AIGetPrioTarget&(chnr&, unitnr&)
                IF tg& >= 0 THEN
                  CALL AIMoveCloserTo(chnr&, unitnr&, channels(chnr&).units(tg&).xpos, channels(chnr&).units(tg&).ypos)
                ELSE
                  'Einheit dichter an globales Ziel heranbewegen
                  IF channels(chnr&).player(plnr&).aiglobaltargetx >= 0 THEN
                    CALL AIMoveCloserTo(chnr&, unitnr&, channels(chnr&).player(plnr&).aiglobaltargetx, channels(chnr&).player(plnr&).aiglobaltargety)
                  END IF
                END IF
              END IF
            END IF
          END IF
        END IF
      END IF
    END IF
  NEXT unitnr&
END SUB



'Feindliche und neutrale Shops ermitteln und bewerten
SUB AIRateShops(chnr&, plnr&)
  LOCAL shopnr&, shoptype&, owner&, prio&, slotnr&, unitnr&, unittp&

  FOR shopnr& = 0 TO channels(chnr&).info.nshops-1
    owner& = channels(chnr&).shops(shopnr&).owner
    shoptype& = channels(chnr&).shops(shopnr&).shoptype
    IF channels(chnr&).shops(shopnr&).prio = 0 AND shoptype& > 0 AND owner& < 7 AND (channels(chnr&).player(plnr&).allymask AND 2^owner&) = 0 THEN
      'Shop bewerten
      prio& = channels(chnr&).shops(shopnr&).eplus*5
      IF owner& = 6 THEN prio& = prio&+channels(chnr&).shops(shopnr&).energy
      IF shoptype& = %SHOPTYPE_AIRPORT THEN prio& = prio&+100
      IF shoptype& = %SHOPTYPE_HARBOUR THEN prio& = prio&+30
      IF shoptype& = %SHOPTYPE_FACTORY THEN prio& = prio&+50
      FOR slotnr& = 0 TO 15
        unitnr& = channels(chnr&).shops(shopnr&).content(slotnr&)
        IF unitnr& >= 0 THEN
          unittp& = channels(chnr&).units(unitnr&).unittype
          prio& = prio&+channelsnosave(chnr&).unitclasses(unittp&).costenergy
        END IF
      NEXT slotnr&
      'falls es eine Einheit gibt, die diesen Shop sofort einnehmen kann, den Shop höher einstufen
      unitnr& = AIGetUnitToOccupyShop&(chnr&, plnr&, shopnr&)
      IF unitnr& >= 0 THEN
        IF channels(chnr&).units(unitnr&).aicommand = %AIC_OCCUPY AND prio& > 0 THEN prio& = prio&+500
        channels(chnr&).units(unitnr&).aicommand = %AIC_NONE
        channels(chnr&).units(unitnr&).aitargetshop = -1
      END IF
      channels(chnr&).shops(shopnr&).prio = prio&
    END IF
  NEXT shopnr&
END SUB



'Sortiert die relevanten Shops nach Priorität
FUNCTION AISortShops$(chnr&)
  LOCAL i&, n&, shoplist$
  LOCAL prio&(), shopnr&()

  n& = channels(chnr&).info.nshops-1
  IF n& < 0 THEN EXIT FUNCTION
  DIM prio&(n&), shopnr&(n&)

  'Shops nach Priorität sortieren
  FOR i& = 0 TO n&
    prio&(i&) = channels(chnr&).shops(i&).prio
    shopnr&(i&) = i&
  NEXT i&
  ARRAY SORT prio&(0) FOR n&+1, TAGARRAY shopnr&(), DESCEND

  'Liste aus allen Shops mit Priorität größer 0 erstellen
  FOR i& = 0 TO n&
    IF prio&(i&) > 0 THEN shoplist$ = shoplist$+CHR$(shopnr&(i&))
  NEXT i&

  AISortShops$ = shoplist$
END FUNCTION



'Eigenen Shop suchen, zu dem ein Transporter Aldinium bringen kann
FUNCTION AIFindShopForCrystals&(chnr&, plnr&, transporter&)
  LOCAL shopnr&, bestshop&, bestscore&, score&, dist&, shoptype&

  bestshop& = -1
  FOR shopnr& = 0 TO channels(chnr&).info.nshops
    IF channels(chnr&).shops(shopnr&).owner = plnr& AND UnitCanEnterBuilding&(chnr&, transporter&, shopnr&) THEN
      CALL CalculateUnitPath(chnr&, transporter&, channels(chnr&).shops(shopnr&).position, channels(chnr&).shops(shopnr&).position2, 254)
      dist& = channels(chnr&).player(plnr&).unitpathlen
      IF dist& > 0 THEN
        'je dichter der Shop am Transporter ist, desto besser
        score& = 300-dist&*5
        'Produktionsgebäude bevorzugen
        shoptype& = channels(chnr&).shops(shopnr&).shoptype
        IF shoptype& = %SHOPTYPE_AIRPORT THEN score& = score&+500
        IF shoptype& = %SHOPTYPE_HARBOUR THEN score& = score&+400
        IF shoptype& = %SHOPTYPE_FACTORY THEN score& = score&+300
        'Shops mit wenig gelagertem Material bevorzugen
        score& = score&-channels(chnr&).shops(shopnr&).material

        IF score& > bestscore& THEN
          bestscore& = score&
          bestshop& = shopnr&
        END IF
      END IF
    END IF
  NEXT shopnr&

  AIFindShopForCrystals& = bestshop&
END FUNCTION



'Eigene Einheit suchen, die am dichtesten an einem Shop ist und ihn einnehmen kann
FUNCTION AIGetUnitToOccupyShop&(chnr&, plnr&, shopnr&)
  LOCAL bestunit&, bestdist&, unitnr&, unittp&, dist&, x&, y&, foundfields&
  LOCAL reachablefields&()

  bestunit& = -1
  bestdist& = 255
  x& = channels(chnr&).shops(shopnr&).position
  y& = channels(chnr&).shops(shopnr&).position2

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    unittp& = channels(chnr&).units(unitnr&).unittype
    IF channels(chnr&).units(unitnr&).owner = plnr& AND channels(chnr&).units(unitnr&).aicommand = %AIC_NONE _
      AND (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_OCCUPY) <> 0 AND channels(chnr&).units(unitnr&).fuel > 0 AND UnitIsAlive&(chnr&, unitnr&) <> 0 THEN
      CALL CalculateUnitPath(chnr&, unitnr&, x&, y&, 160)
      dist& = channels(chnr&).player(plnr&).unitpathlen
      IF dist& > 0 THEN
        'prüfen, ob Shop sofort eingenommen werden kann
        IF dist&*8 <= channelsnosave(chnr&).unitclasses(unittp&).range THEN
          CALL ClearTargets(chnr&, plnr&)
          foundfields& = GetTargets&(chnr&, unitnr&, 1, 0, reachablefields&())
          IF (channels(chnr&).player(plnr&).targets(x&, y&) AND %TG_MOVE) <> 0 THEN
            'diese Einheit kann den Shop noch diese Runde einnehmen
            channels(chnr&).units(unitnr&).aicommand = %AIC_OCCUPY
            channels(chnr&).units(unitnr&).aitargetshop = shopnr&
            AIGetUnitToOccupyShop& = unitnr&
            EXIT FUNCTION
          END IF
        END IF
        IF dist& <= 20 AND dist& < bestdist& THEN
          bestdist& = dist&
          bestunit& = unitnr&
        END IF
      END IF
    END IF
  NEXT unitnr&

  AIGetUnitToOccupyShop& = bestunit&
END FUNCTION



'Alle bekannten feindlichen Shops suchen
SUB AIFindShopsToOccupy(chnr&, plnr&)
  LOCAL shoplist$
  LOCAL i&, n&, shopnr&, owner&, plmask&, unitnr&, unittp&, x&, y&, dist&, bestdist&, bestunit&, bestescort&, weaponnr&, v&, bestvalue&, foundfields&, supportunitmask&
  LOCAL reachablefields&()

  'relevante Shops suchen und sortieren
  CALL AIRateShops(chnr&, plnr&)
  shoplist$ = AISortShops$(chnr&)

  supportunitmask& = %UCF_REPAIR OR %UCF_REFUEL OR %UCF_REFUELAIR OR %UCF_BUILD OR %UCF_FORTIFY
  plmask& = 2^plnr&
  n& = LEN(shoplist$)
  FOR i& = 1 TO n&
    shopnr& = ASC(shoplist$, i&)
    owner& = channels(chnr&).shops(shopnr&).owner
    IF channels(chnr&).shops(shopnr&).shoptype > 0 AND owner& < 7 AND (channels(chnr&).player(plnr&).allymask AND 2^owner&) = 0 THEN
      x& = channels(chnr&).shops(shopnr&).position
      y& = channels(chnr&).shops(shopnr&).position2
      IF (channels(chnr&).explored(x&, y&) AND plmask&) <> 0 OR channels(chnr&).info.difficulty > %DIFFICULTY_EASY THEN

        'eigene Einheit suchen, die am dichtesten an diesem Shop ist und ihn einnehmen kann
        bestunit& = AIGetUnitToOccupyShop&(chnr&, plnr&, shopnr&)

        IF bestunit& >= 0 AND channels(chnr&).units(bestunit&).aicommand = %AIC_NONE THEN
          'es wurde eine Einheit gefunden, die den Shop allerdings noch nicht diese Runde einnehmen kann
          channels(chnr&).units(bestunit&).aicommand = %AIC_OCCUPY
          channels(chnr&).units(bestunit&).aitargetshop = shopnr&
          x& = channels(chnr&).units(bestunit&).xpos
          y& = channels(chnr&).units(bestunit&).ypos
          'Begleitschutz für diese Einheit suchen (im Umkreis von 2 Feldern)
          bestescort& = -1
          bestvalue& = 0
          FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
            IF unitnr& <> bestunit& AND channels(chnr&).units(unitnr&).owner = plnr& AND channels(chnr&).units(unitnr&).aicommand = %AIC_NONE AND channels(chnr&).units(unitnr&).fuel > 20 AND UnitIsAlive&(chnr&, unitnr&) <> 0 THEN
              IF ABS(channels(chnr&).units(unitnr&).xpos-x&) <= 2 AND ABS(channels(chnr&).units(unitnr&).ypos-y&) <= 2 THEN
                unittp& = channels(chnr&).units(unitnr&).unittype
                IF channelsnosave(chnr&).unitclasses(unittp&).armor >= 200 AND channelsnosave(chnr&).unitclasses(unittp&).range >= 24 AND (channelsnosave(chnr&).unitclasses(unittp&).flags AND supportunitmask&) = 0 THEN
                  FOR weaponnr& = 0 TO 3
                    IF channels(chnr&).units(unitnr&).ammo(weaponnr&) > 0 AND channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).damage > 0 THEN v& = MAX&(v&, channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).damage)
                  NEXT weaponnr&
                  IF v& > 0 THEN
                    v& = v&+channelsnosave(chnr&).unitclasses(unittp&).armor
                    IF v& > bestvalue& THEN
                      bestvalue& = v&
                      bestescort& = unitnr&
                    END IF
                  END IF
                END IF
              END IF
            END IF
          NEXT unitnr&
          IF bestescort& >= 0 THEN
            channels(chnr&).units(bestescort&).aicommand = %AIC_ESCORT
            channels(chnr&).units(bestescort&).aitargetunit = bestunit&
            channels(chnr&).units(bestescort&).aitargetshop = shopnr&
          END IF
        END IF
      END IF
    END IF
  NEXT i&
END SUB



'Begleitschutz bewegen
SUB AIEscort(chnr&, plnr&)
  LOCAL i&, unitnr&, targetunit&, shopnr&, x&, y&, tg&, foundtargets&, targetunits&()

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND channels(chnr&).units(unitnr&).aicommand = %AIC_ESCORT AND (channels(chnr&).units(unitnr&).flags AND %US_DONE+%US_AI_IGNORE) = 0 THEN
      'Gegner suchen, die die zu beschützende Einheit bedrohen
      targetunit& = channels(chnr&).units(unitnr&).aitargetunit
      x& = channels(chnr&).units(targetunit&).xpos
      y& = channels(chnr&).units(targetunit&).ypos
      foundtargets& = AIFindTargetsInArea&(chnr&, x&, y&, 3, targetunits&())
      IF foundtargets& > 0 THEN
        'Gegner angreifen
        tg& = AIFindTargets&(chnr&, unitnr&, 1)
        IF tg& >= 0 THEN
          channels(chnr&).units(unitnr&).aicommand = %AIC_ATTACK
          x& = tg& AND 255
          y& = INT(tg&/256) AND 255
          channels(chnr&).units(unitnr&).aitargetunit = INT(tg&/65536)
          CALL AIMoveUnit(chnr&, plnr&, unitnr&, x&, y&)
          ITERATE FOR
        END IF
      END IF

      'Begleitschutz wenn möglich 1 Feld vor die zu schützende Einheit ziehen (in Richtung Shop)
      shopnr& = channels(chnr&).units(unitnr&).aitargetshop
      x& = x&+IIF&(channels(chnr&).shops(shopnr&).position < x&, -1, 1)
      y& = y&+IIF&(channels(chnr&).shops(shopnr&).position2 < y&, -1, 1)
      IF channels(chnr&).zone3(x&, y&) < -1 THEN
        'Shop erreicht
        CALL UnitDone(chnr&, unitnr&)
      ELSE
        'auf Zielfeld bewegen
        CALL AIMoveCloserTo(chnr&, unitnr&, x&, y&)
      END IF
    END IF
  NEXT unitnr&
END SUB



'Alle Einheiten bewegen, die Shops einnehmen sollen
SUB AIOccupyShops(chnr&, plnr&)
  LOCAL i&, unitnr&, shopnr&, contentcount&, enemy&, enemytp&, enemyowner&, shopx&, shopy&
  LOCAL reachablefields&(), oldtargets$()
  DIM oldtargets$(%MAXPLAYERS-1)

  'alle Zielmarkierungen sichern
  FOR enemyowner& = 0 TO %MAXPLAYERS-1
    oldtargets$(enemyowner&) = PushTargets$(chnr&, enemyowner&)
  NEXT enemyowner&

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).info.state >= %CHANNELSTATE_VICTORY AND channels(chnr&).info.state <= %CHANNELSTATE_DEFEAT THEN EXIT SUB
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND channels(chnr&).units(unitnr&).aicommand = %AIC_OCCUPY THEN
      shopnr& = channels(chnr&).units(unitnr&).aitargetshop
      shopx& = channels(chnr&).shops(shopnr&).position
      shopy& = channels(chnr&).shops(shopnr&).position2

      'prüfen, ob Shop Einheiten einhält (auch wenn Shop eigentlich nicht für diesen Spieler einsehbar ist)
      contentcount& = 0
      FOR i& = 0 TO 15
        IF channels(chnr&).shops(shopnr&).content(i&) >= 0 THEN contentcount& = contentcount&+1
      NEXT i&

      'prüfen, ob eine feindliche Einheit den Shop gleich wieder zurück erobern könnte (nur wenn Shop leer ist)
      IF contentcount& = 0 THEN
        FOR enemy& = 0 TO channels(chnr&).info.nunits-1
          enemytp& = channels(chnr&).units(enemy&).unittype
          IF channels(chnr&).units(enemy&).aitargetpriority > 0 AND (channelsnosave(chnr&).unitclasses(enemytp&).flags AND %UCF_OCCUPY) <> 0 THEN
            IF GetDistance&(channels(chnr&).units(enemy&).xpos, channels(chnr&).units(enemy&).ypos, shopx&, shopy&)*8 <= channelsnosave(chnr&).unitclasses(enemytp&).range THEN
              enemyowner& = channels(chnr&).units(enemy&).owner
              CALL ClearTargets(chnr&, enemyowner&)
              CALL GetTargets&(chnr&, enemy&, 1, 0, reachablefields&())
              IF (channels(chnr&).player(enemyowner&).targets(shopx&, shopy&) AND %TG_MOVE) <> 0 THEN
                'Feind kann Shop sofort zurück erobern -> Shop nicht einnehmen sondern nur dichter heran bewegen
                shopx& = shopx&+IIF&(channels(chnr&).units(unitnr&).xpos < shopx&, -1, 1)
                shopy& = shopy&+IIF&(channels(chnr&).units(unitnr&).ypos < shopy&, -1, 1)
                EXIT FOR
              END IF
            END IF
          END IF
        NEXT enemy&
      END IF

      'Shop einnehmen bzw. dichter heran bewegen
      CALL ClearTargets(chnr&, 0)
      CALL AIMoveCloserTo(chnr&, unitnr&, channels(chnr&).shops(shopnr&).position, channels(chnr&).shops(shopnr&).position2)
    END IF
  NEXT unitnr&

  'alle Zielmarkierungen wieder herstellen
  FOR enemyowner& = 0 TO %MAXPLAYERS-1
    CALL PopTargets(chnr&, enemyowner&, oldtargets$(enemyowner&))
  NEXT enemyowner&
END SUB



'Einheiten aus Shops herausbewegen
SUB AIUnloadShops(chnr&, plnr&)
  LOCAL shopnr&, slotnr&, unitnr&, unittp&, flags&, tg&, foundfields&, x&, y&
  LOCAL hp!, fuellevel!
  LOCAL reachablefields&()

  flags& = %US_MOVED OR %US_DONE OR %US_AI_IGNORE
  FOR shopnr& = 0 TO channels(chnr&).info.nshops-1
    IF channels(chnr&).shops(shopnr&).owner = plnr& AND channels(chnr&).shops(shopnr&).shoptype > %SHOPTYPE_UNIT THEN
      FOR slotnr& = 0 TO 15
        unitnr& = channels(chnr&).shops(shopnr&).content(slotnr&)
        IF unitnr& >= 0 AND (channels(chnr&).units(unitnr&).flags AND flags&) = 0 THEN
          'Einheit kann diese Runde noch bewegt werden
          CALL RepairInShop(chnr&, shopnr&, unitnr&)
          CALL RefuelInShop(chnr&, shopnr&, unitnr&)
          IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateUnitToAllClients(chnr&, unitnr&)
          unittp& = channels(chnr&).units(unitnr&).unittype
          hp! = channels(chnr&).units(unitnr&).groupsize/channelsnosave(chnr&).unitclasses(unittp&).groupsize
          fuellevel! = channels(chnr&).units(unitnr&).fuel/channelsnosave(chnr&).unitclasses(unittp&).fuel
          IF hp! > 0.65 AND fuellevel! >= 0.5 THEN
            tg& = AIGetPrioTarget&(chnr&, unitnr&)
            IF tg& >= 0 THEN
              'Kampfeinheit an die Front schicken
              CALL AIMoveCloserTo(chnr&, unitnr&, channels(chnr&).units(tg&).xpos, channels(chnr&).units(tg&).ypos)
            ELSE
              'Einheit aus dem Shop herausbewegen (maximal 3 Felder weit)
              channels(chnr&).units(unitnr&).moved = MAX&(0, channelsnosave(chnr&).unitclasses(unittp&).range/8-3)
              foundfields& = GetTargets&(chnr&, unitnr&, 1, 0, reachablefields&())
              IF foundfields& > 0 THEN
                x& = reachablefields&(0) AND 255
                y& = INT(reachablefields&(0)/256)
                CALL AIMoveUnit(chnr&, plnr&, unitnr&, x&, y&)
              END IF
            END IF
          END IF
        END IF
      NEXT slotnr&
    END IF
    IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateShopToClients(chnr&, shopnr&)
  NEXT shopnr&
  IF gameMode& = %GAMEMODE_SERVER THEN UpdatePlayerToClients(chnr&, plnr&)
END SUB



'Prüft, ob eine (zu produzierende) Einheit den Shop überhaupt verlassen kann
FUNCTION AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&)
  LOCAL productionPreviewUnit&, maxMovement&, foundFields&
  LOCAL reachablefields&()

  'Dummy-Einheit erzeugen
  productionPreviewUnit& = CreateUnit&(chnr&, channels(chnr&).shops(shopnr&).position, channels(chnr&).shops(shopnr&).position2, unittp&, plnr&, 1)
  maxMovement& = channelsnosave(chnr&).unitclasses(unittp&).range/8

  'erreichbare Felder ermitteln (unabhängig vom Tempo der Einheit)
  foundFields& = GetTargets&(chnr&, productionPreviewUnit&, 1, 250, reachablefields&())

  AIUnitCanExitShop& = IIF&(foundFields& < 10, 0, -1)
END FUNCTION



'Ermittelt die beste Einheitenklasse aus der Liste der produzierbaren Einheiten mit bestimmten Eigenschaften
'flags = erforderliche Eigenschaften (%UCF_REPAIR, %UCF_REFUEL, etc.)
'weapontype& = erforderliche Waffen (%WP_HIGHAIR, %WP_FUEL, %WP_AMMO, etc.)
FUNCTION AIGetBestProduction&(BYVAL chnr&, BYVAL shopproduction$, BYVAL flags&, BYVAL weapontype&, BYVAL ztype&, BYREF unittp&)
  LOCAL i&, tp&, weapons&, z&, besttp&, bestrating&, supportunitmask&
  LOCAL matchingtypes&(), ratings&(), nfound&
  DIM matchingtypes&(channelsnosave(chnr&).nunitclasses-1), ratings&(channelsnosave(chnr&).nunitclasses-1)

  supportunitmask& = %UCF_REPAIR OR %UCF_REFUEL OR %UCF_REFUELAIR

  'Einheitenklassen suchen, die den Anforderungen genügen
  FOR i& = 1 TO LEN(shopproduction$)
    tp& = ASC(shopproduction$, i&)
    weapons& = GetWeaponTypeByClass&(chnr&, tp&)
    IF (flags& = 0 AND (channelsnosave(chnr&).unitclasses(tp&).flags AND supportunitmask&) = 0) OR (flags& <> 0 AND (channelsnosave(chnr&).unitclasses(tp&).flags AND flags&) = flags&) THEN
      IF weapontype& = 0 OR (weapons& AND weapontype&) = weapontype& THEN
        z& = %UZ_GROUND
        IF (channelsnosave(chnr&).unitclasses(tp&).flags AND %UCF_PLANE) <> 0 THEN z& = %UZ_AIR
        IF (channelsnosave(chnr&).unitclasses(tp&).flags AND %UCF_SHIP) <> 0 THEN z& = %UZ_WATER
        IF ztype& = %UZ_ANY OR ztype& = z& THEN
          matchingtypes&(nfound&) = tp&
          ratings&(nfound&) = channelsnosave(chnr&).unitclasses(tp&).rating
          nfound& = nfound&+1
        END IF
      END IF
    END IF
  NEXT i&
  IF nfound& = 0 THEN EXIT FUNCTION

  'gefundene Einheitenklassen sortieren und entsprechend dem Schwierigkeitsgrad die beste, zweitbeste oder drittbeste Klasse zurückliefern
  ARRAY SORT ratings&(0) FOR nfound&, TAGARRAY matchingtypes&(), DESCEND
  IF nfound& = 1 OR channels(chnr&).info.difficulty = %DIFFICULTY_HARD THEN
    unittp& = matchingtypes&(0)
  ELSE
    IF nfound& = 2 OR channels(chnr&).info.difficulty = %DIFFICULTY_NORMAL THEN
      unittp& = matchingtypes&(1)
    ELSE
      unittp& = matchingtypes&(2)
    END IF
  END IF

  AIGetBestProduction& = -1
END FUNCTION



'Produziert eine Einheit
FUNCTION AIBuildUnit&(chnr&, shopnr&, unittp&)
  LOCAL newunit&, plnr&

  newunit& = BuildUnit&(chnr&, shopnr&, unittp&)
  IF newunit& >= 0 AND gameMode& = %GAMEMODE_SERVER THEN
    plnr& = channels(chnr&).shops(shopnr&).owner
    CALL UpdatePlayerToClients(chnr&, plnr&)
    CALL UpdateShopToClients(chnr&, shopnr&)
    CALL UpdateUnitToAllClients(chnr&, newunit&)
  END IF

  AIBuildUnit& = newunit&
END FUNCTION



'Einheiten produzieren
SUB AIProduceUnits(chnr&, plnr&)
  LOCAL a$$, i&, k&, z&, unitnr&, unittp&, weapons&, totalenergy&, ntotal&, buildmode&
  LOCAL ngroundrepair&, nwaterrepair&, ngroundrefuel&, nwaterrefuel&, nairrefuel&, ngroundrecharge&, nwaterrecharge&, nairrecharge&, nair&, nship&, nground&, noccupy&, unitsbuilt&, supportunitsbuilt&
  LOCAL enemybyweapon&(), vsweapon&()
  LOCAL reqgroundrepair&, reqwaterrepair&, reqgroundrefuel&, reqwaterrefuel&, reqairrefuel&, buildcombatunits&
  LOCAL shoplist$, shopnr&, shoptype&, shopx&, shopy&, nshops&, bestrating&, bestshop&, bestunittp&, shopproduction$
  DIM enemybyweapon&(4), vsweapon&(4)

  totalenergy& = channels(chnr&).player(plnr&).energy
  IF totalenergy& < 5 THEN EXIT SUB

  'eigene Einheiten zählen und Versorgung sowie Abwehr gegen die unterschiedlichen Klassen ermitteln
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 THEN
      unittp& = channels(chnr&).units(unitnr&).unittype
      weapons& = GetWeaponTypeByClass&(chnr&, unittp&)
      'Reparatureinheit
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REPAIR) <> 0 THEN
        IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN nwaterrepair& = nwaterrepair&+1 ELSE ngroundrepair& = ngroundrepair&+1
      END IF
      'Befülleinheit
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REFUEL) <> 0 THEN
        IF (weapons& AND %WP_FUEL) <> 0 THEN
          IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN
            nwaterrefuel& = nwaterrefuel&+1
          ELSE
            IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN nairrefuel& = nairrefuel&+1 ELSE ngroundrefuel& = ngroundrefuel&+1
          END IF
        END IF
        IF (weapons& AND %WP_AMMO) <> 0 THEN
          IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN
            nwaterrecharge& = nwaterrecharge&+1
          ELSE
            IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN nairrecharge& = nairrecharge&+1 ELSE ngroundrecharge& = ngroundrecharge&+1  'Munitionsflugzeug gibt es allerdings im unmodifizierten Spiel nicht
          END IF
        END IF
      END IF
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN nair& = nair&+1
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN nship& = nship&+1
      'Abwehr
      IF (weapons& AND %WP_HIGHAIR) <> 0 THEN vsweapon&(0) = vsweapon&(0)+1
      IF (weapons& AND %WP_LOWAIR) <> 0 THEN vsweapon&(1) = vsweapon&(1)+1
      IF (weapons& AND %WP_LAND) <> 0 THEN vsweapon&(2) = vsweapon&(2)+1
      IF (weapons& AND %WP_WATER) <> 0 THEN vsweapon&(3) = vsweapon&(3)+1
      IF (weapons& AND %WP_UNDERWATER) <> 0 THEN vsweapon&(4) = vsweapon&(4)+1
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_OCCUPY) <> 0 THEN noccupy& = noccupy&+1
      ntotal& = ntotal&+1
    END IF
  NEXT unitnr&
  nground& = ntotal&-nair&-nship&

  'feindliche Einheiten zählen
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF IsEnemyUnit&(chnr&, plnr&, unitnr&) <> 0 AND UnitIsAlive&(chnr&, unitnr&) <> 0 THEN
      unittp& = channels(chnr&).units(unitnr&).unittype
      weapons& = 2  'Bodeneinheit
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN
        IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_CLIMB) <> 0 THEN weapons& = 0 ELSE weapons& = 1
      ELSE
        IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN
          IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_DIVE) <> 0 THEN weapons& = 4 ELSE weapons& = 3
        END IF
      END IF
      enemybyweapon&(weapons&) = enemybyweapon&(weapons&)+1
    END IF
  NEXT unitnr&

  'benötigte Einheiten ermitteln
  reqgroundrepair& = INT((nground&+7)/8)
  reqwaterrepair& = INT((nship&+6)/7)
  reqgroundrefuel& = INT((nground&+7)/8)
  reqwaterrefuel& = INT((nship&+6)/7)
  reqairrefuel& = INT((nair&+6)/7)

  'Kampfeinheiten nicht jede Runde bauen
  IF RND(0, 3) = 0 OR totalenergy& >= 120 THEN buildcombatunits& = 1
  IF debugEnabled& THEN
    a$$ = "$COLOR"+FORMAT$(plnr&+1)+"$GRD="+FORMAT$(nground&)+", WTR="+FORMAT$(nship&)+", AIR="+FORMAT$(nair&) _
      + ", RPG="+FORMAT$(ngroundrepair&)+", RPW="+FORMAT$(nwaterrepair&) _
      + ", RFG="+FORMAT$(ngroundrefuel&)+"/"+FORMAT$(ngroundrecharge&)+", RFW="+FORMAT$(nwaterrefuel&)+"/"+FORMAT$(nwaterrecharge&)+", RFA="+FORMAT$(nairrefuel&)+"/"+FORMAT$(nairrecharge&) _
      + ", QRG="+FORMAT$(reqgroundrepair&)+", QRW="+FORMAT$(reqwaterrepair&)+", QFG="+FORMAT$(reqgroundrefuel&)+", QFW="+FORMAT$(reqwaterrefuel&)+", QFA="+FORMAT$(reqairrefuel&) _
      + ", VSH="+FORMAT$(vsweapon&(0))+"/"+FORMAT$(enemybyweapon&(0))+", VSA="+FORMAT$(vsweapon&(1))+"/"+FORMAT$(enemybyweapon&(1))+", VSG="+FORMAT$(vsweapon&(2))+"/"+FORMAT$(enemybyweapon&(2)) _
      + ", VSW="+FORMAT$(vsweapon&(3))+"/"+FORMAT$(enemybyweapon&(3))+", VSD="+FORMAT$(vsweapon&(4))+"/"+FORMAT$(enemybyweapon&(4)) _
      + "$COLOR0$"
'    CALL AddProtocol(a$$)
    a$$ = ""
  END IF

  'alle eigenen Produktionsgebäude suchen
  FOR shopnr& = 0 TO channels(chnr&).info.nshops-1
    IF channels(chnr&).shops(shopnr&).owner = plnr& AND channels(chnr&).shops(shopnr&).material > 4 THEN
      shoptype& = channels(chnr&).shops(shopnr&).shoptype
      IF shoptype& = %SHOPTYPE_AIRPORT OR shoptype& = %SHOPTYPE_HARBOUR OR shoptype& = %SHOPTYPE_FACTORY THEN shoplist$ = shoplist$+MKI$(shopnr&)
    END IF
  NEXT shopnr&
  IF shoplist$ = "" THEN EXIT SUB

  'Reihenfolge der Shops jede Runde ändern, damit Einheiten nicht immer im selben Shop produziert werden
  nshops& = LEN(shoplist$)/2
  i& = channels(chnr&).info.turn MOD nshops&
  shoplist$ = MID$(shoplist$, i&*2+1)+LEFT$(shoplist$, i&*2)

  'in allen eigenen Shops prüfen, ob Einheiten produziert werden sollten
  FOR i& = 0 TO nshops&-1
    FOR buildmode& = 0 TO 1
      shopnr& = CVI(shoplist$, i&*2+1)
      shopx& = channels(chnr&).shops(shopnr&).position
      shopy& = channels(chnr&).shops(shopnr&).position2
      shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)

      'Reparaturfahrzeug bauen
      z& = IIF&(buildmode& = 0, %UZ_GROUND, %UZ_WATER)
      IF reqgroundrepair& > ngroundrepair& AND AIGetBestProduction&(chnr&, shopproduction$, %UCF_REPAIR, 0, z&, unittp&) <> 0 THEN
        IF AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&) AND AICountOwnUnitsOfType&(chnr&, plnr&, unittp&, shopx&, shopy&, 10) < 2 THEN
          unitnr& = AIBuildUnit&(chnr&, shopnr&, unittp&)
          IF unitnr& < 0 THEN ITERATE FOR  'Shop ist voll
          unitsbuilt& = unitsbuilt&+1
          supportunitsbuilt& = supportunitsbuilt&+1
          reqgroundrepair& = reqgroundrepair&+1
          weapons& = GetWeaponTypeByUnit&(chnr&, unitnr&)
          IF (weapons& AND %WP_FUEL) <> 0 THEN ngroundrefuel& = ngroundrefuel&+1
          IF (weapons& AND %WP_AMMO) <> 0 THEN ngroundrecharge& = ngroundrecharge&+1
          shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)
          a$$ = a$$+channelsnosave(chnr&).unitclasses(unittp&).uname+"+RG,"
        END IF
      END IF

      'Reparaturschiff bauen
      z& = IIF&(buildmode& = 0, %UZ_WATER, %UZ_GROUND)
      IF reqwaterrepair& > nwaterrepair& AND AIGetBestProduction&(chnr&, shopproduction$, %UCF_REPAIR, 0, z&, unittp&) <> 0 THEN
        IF AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&) AND AICountOwnUnitsOfType&(chnr&, plnr&, unittp&, shopx&, shopy&, 10) < 2 THEN
          unitnr& = AIBuildUnit&(chnr&, shopnr&, unittp&)
          IF unitnr& < 0 THEN ITERATE FOR  'Shop ist voll
          unitsbuilt& = unitsbuilt&+1
          supportunitsbuilt& = supportunitsbuilt&+1
          nwaterrepair& = nwaterrepair&+1
          weapons& = GetWeaponTypeByUnit&(chnr&, unitnr&)
          IF (weapons& AND %WP_FUEL) <> 0 THEN nwaterrefuel& = nwaterrefuel&+1
          IF (weapons& AND %WP_AMMO) <> 0 THEN nwaterrecharge& = nwaterrecharge&+1
          shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)
          a$$ = a$$+channelsnosave(chnr&).unitclasses(unittp&).uname+"+RW,"
        END IF
      END IF

      'Munitionstransporter bauen
      z& = IIF&(buildmode& = 0, %UZ_GROUND, %UZ_WATER)
      IF reqgroundrefuel& > ngroundrecharge& AND AIGetBestProduction&(chnr&, shopproduction$, %UCF_REFUEL, %WP_AMMO, z&, unittp&) <> 0 THEN
        IF AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&) AND AICountOwnUnitsOfType&(chnr&, plnr&, unittp&, shopx&, shopy&, 10) < 2 THEN
          unitnr& = AIBuildUnit&(chnr&, shopnr&, unittp&)
          IF unitnr& < 0 THEN ITERATE FOR  'Shop ist voll
          unitsbuilt& = unitsbuilt&+1
          supportunitsbuilt& = supportunitsbuilt&+1
          ngroundrecharge& = ngroundrecharge&+1
          weapons& = GetWeaponTypeByUnit&(chnr&, unitnr&)
          IF (weapons& AND %WP_FUEL) <> 0 THEN ngroundrefuel& = ngroundrefuel&+1
          IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REPAIR) <> 0 THEN reqgroundrepair& = reqgroundrepair&+1
          shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)
          a$$ = a$$+channelsnosave(chnr&).unitclasses(unittp&).uname+"+CG,"
        END IF
      END IF

      'Munitionsschiff bauen
      z& = IIF&(buildmode& = 0, %UZ_WATER, %UZ_GROUND)
      IF reqwaterrefuel& > nwaterrecharge& AND AIGetBestProduction&(chnr&, shopproduction$, %UCF_REFUEL, %WP_AMMO, z&, unittp&) <> 0 THEN
        IF AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&) AND AICountOwnUnitsOfType&(chnr&, plnr&, unittp&, shopx&, shopy&, 10) < 2 THEN
          unitnr& = AIBuildUnit&(chnr&, shopnr&, unittp&)
          IF unitnr& < 0 THEN ITERATE FOR  'Shop ist voll
          unitsbuilt& = unitsbuilt&+1
          supportunitsbuilt& = supportunitsbuilt&+1
          nwaterrecharge& = nwaterrecharge&+1
          weapons& = GetWeaponTypeByUnit&(chnr&, unitnr&)
          IF (weapons& AND %WP_FUEL) <> 0 THEN nwaterrefuel& = nwaterrefuel&+1
          IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REPAIR) <> 0 THEN nwaterrepair& = nwaterrepair&+1
          shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)
          a$$ = a$$+channelsnosave(chnr&).unitclasses(unittp&).uname+"+CW,"
        END IF
      END IF

      'Munitionsflugzeug bauen (gibt es allerdings im unmodifizierten Spiel nicht)
      z& = IIF&(buildmode& = 0, %UZ_AIR, %UZ_ANY)
      IF reqairrefuel& > nairrecharge& AND AIGetBestProduction&(chnr&, shopproduction$, %UCF_REFUELAIR, %WP_AMMO, z&, unittp&) <> 0 THEN
        IF AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&) AND AICountOwnUnitsOfType&(chnr&, plnr&, unittp&, shopx&, shopy&, 10) < 2 THEN
          unitnr& = AIBuildUnit&(chnr&, shopnr&, unittp&)
          IF unitnr& < 0 THEN ITERATE FOR  'Shop ist voll
          unitsbuilt& = unitsbuilt&+1
          supportunitsbuilt& = supportunitsbuilt&+1
          nairrecharge& = nairrecharge&+1
          weapons& = GetWeaponTypeByUnit&(chnr&, unitnr&)
          IF (weapons& AND %WP_FUEL) <> 0 THEN nairrefuel& = nairrefuel&+1
          shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)
          a$$ = a$$+channelsnosave(chnr&).unitclasses(unittp&).uname+"!!,"
        END IF
      END IF

      'Tankfahrzeug bauen
      z& = IIF&(buildmode& = 0, %UZ_GROUND, %UZ_WATER)
      IF reqgroundrefuel& > ngroundrefuel& AND AIGetBestProduction&(chnr&, shopproduction$, %UCF_REFUEL, %WP_FUEL, z&, unittp&) <> 0 THEN
        IF AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&) AND AICountOwnUnitsOfType&(chnr&, plnr&, unittp&, shopx&, shopy&, 10) < 2 THEN
          unitnr& = AIBuildUnit&(chnr&, shopnr&, unittp&)
          IF unitnr& < 0 THEN ITERATE FOR  'Shop ist voll
          unitsbuilt& = unitsbuilt&+1
          supportunitsbuilt& = supportunitsbuilt&+1
          ngroundrefuel& = ngroundrefuel&+1
          weapons& = GetWeaponTypeByUnit&(chnr&, unitnr&)
          IF (weapons& AND %WP_AMMO) <> 0 THEN ngroundrecharge& = ngroundrecharge&+1
          IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REPAIR) <> 0 THEN reqgroundrepair& = reqgroundrepair&+1
          shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)
          a$$ = a$$+channelsnosave(chnr&).unitclasses(unittp&).uname+"+FG,"
        END IF
      END IF

      'Tankschiff bauen
      z& = IIF&(buildmode& = 0, %UZ_WATER, %UZ_GROUND)
      IF reqwaterrefuel& > nwaterrefuel& AND AIGetBestProduction&(chnr&, shopproduction$, %UCF_REFUEL, %WP_FUEL, z&, unittp&) <> 0 THEN
        IF AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&) AND AICountOwnUnitsOfType&(chnr&, plnr&, unittp&, shopx&, shopy&, 10) < 2 THEN
          unitnr& = AIBuildUnit&(chnr&, shopnr&, unittp&)
          IF unitnr& < 0 THEN ITERATE FOR  'Shop ist voll
          unitsbuilt& = unitsbuilt&+1
          supportunitsbuilt& = supportunitsbuilt&+1
          nwaterrefuel& = nwaterrefuel&+1
          weapons& = GetWeaponTypeByUnit&(chnr&, unitnr&)
          IF (weapons& AND %WP_AMMO) <> 0 THEN nwaterrecharge& = nwaterrecharge&+1
          IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REPAIR) <> 0 THEN nwaterrepair& = nwaterrepair&+1
          shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)
          a$$ = a$$+channelsnosave(chnr&).unitclasses(unittp&).uname+"+FW,"
        END IF
      END IF

      'Tankflieger bauen
      z& = IIF&(buildmode& = 0, %UZ_AIR, %UZ_ANY)
      IF reqairrefuel& > nairrefuel& AND AIGetBestProduction&(chnr&, shopproduction$, %UCF_REFUELAIR, %WP_FUEL, z&, unittp&) <> 0 THEN
        IF AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&) AND AICountOwnUnitsOfType&(chnr&, plnr&, unittp&, shopx&, shopy&, 10) < 2 THEN
          unitnr& = AIBuildUnit&(chnr&, shopnr&, unittp&)
          IF unitnr& < 0 THEN ITERATE FOR  'Shop ist voll
          unitsbuilt& = unitsbuilt&+1
          supportunitsbuilt& = supportunitsbuilt&+1
          nairrefuel& = nairrefuel&+1
          shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)
          a$$ = a$$+channelsnosave(chnr&).unitclasses(unittp&).uname+"+FA,"
        END IF
      END IF

      'Einheiten, die Gebäude einnehmen können
      IF buildmode& = 0 AND noccupy& < 4 AND AIGetBestProduction&(chnr&, shopproduction$, %UCF_OCCUPY, 0, %UZ_ANY, unittp&) <> 0 THEN
        IF AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&) THEN
          unitnr& = AIBuildUnit&(chnr&, shopnr&, unittp&)
          IF unitnr& < 0 THEN ITERATE FOR  'Shop ist voll
          unitsbuilt& = unitsbuilt&+1
          noccupy& = noccupy&+1
          shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)
          a$$ = a$$+channelsnosave(chnr&).unitclasses(unittp&).uname+"+OC,"
        END IF
      END IF

      'Kampfeinheiten bauen
      IF buildmode& = 0 AND buildcombatunits& = 1 THEN
        FOR k& = 0 TO 4
          IF enemybyweapon&(k&) > vsweapon&(k&) AND AIGetBestProduction&(chnr&, shopproduction$, 0, 2^k&, %UZ_ANY, unittp&) <> 0 THEN
            IF AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&) THEN
              unitnr& = AIBuildUnit&(chnr&, shopnr&, unittp&)
              IF unitnr& < 0 THEN ITERATE FOR  'Shop ist voll
              unitsbuilt& = unitsbuilt&+1
              weapons& = GetWeaponTypeByUnit&(chnr&, unitnr&)
              IF (weapons& AND %WP_HIGHAIR) <> 0 THEN vsweapon&(0) = vsweapon&(0)+1
              IF (weapons& AND %WP_LOWAIR) <> 0 THEN vsweapon&(1) = vsweapon&(1)+1
              IF (weapons& AND %WP_LAND) <> 0 THEN vsweapon&(2) = vsweapon&(2)+1
              IF (weapons& AND %WP_WATER) <> 0 THEN vsweapon&(3) = vsweapon&(3)+1
              IF (weapons& AND %WP_UNDERWATER) <> 0 THEN vsweapon&(4) = vsweapon&(4)+1
              shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)
              a$$ = a$$+channelsnosave(chnr&).unitclasses(unittp&).uname+"$$,"
            END IF
          END IF
        NEXT k&
      END IF

      'prüfen, ob Versorgungseinheiten gebaut wurden und erneuten Versuch auslassen, falls ja oder keine Versorgungseinheiten benötigt wurden
      IF supportunitsbuilt& > 0 THEN EXIT FOR
      IF reqgroundrepair& <= ngroundrepair& AND reqwaterrepair& <= nwaterrepair& AND reqgroundrefuel& <= ngroundrecharge& AND reqwaterrefuel& <= nwaterrecharge& _
        AND reqairrefuel& <= nairrecharge& AND reqgroundrefuel& <= ngroundrefuel& AND reqwaterrefuel& <= nwaterrefuel& AND reqairrefuel& <= nairrefuel& THEN EXIT FOR
    NEXT buildmode&

    'nächster Shop
  NEXT i&

  'falls keine Einheiten gebaut wurden (da alle benötigten Einheiten bereits vorhanden sind), beste Einheit aus allen Shops bauen
  IF buildcombatunits& = 1 AND unitsbuilt& = 0 THEN
    'Shop suchen, in dem die stärkste Einheit gebaut werden kann
    bestshop& = -1
    FOR i& = 0 TO nshops&-1
      shopnr& = CVI(shoplist$, i&*2+1)
      shopproduction$ = GetShopProductionMenu$(chnr&, shopnr&, 1, 1)
      IF AIGetBestProduction&(chnr&, shopproduction$, 0, 0, %UZ_ANY, unittp&) <> 0 THEN
        IF channelsnosave(chnr&).unitclasses(unittp&).rating > bestrating& AND AIUnitCanExitShop&(chnr&, plnr&, shopnr&, unittp&) <> 0 THEN
          bestrating& = channelsnosave(chnr&).unitclasses(unittp&).rating
          bestshop& = shopnr&
          bestunittp& = unittp&
        END IF
      END IF
    NEXT i&

    'die beste gefundene Einheit bauen
    IF bestshop& >= 0 THEN
      WHILE channels(chnr&).player(plnr&).energy >= channelsnosave(chnr&).unitclasses(bestunittp&).costenergy AND channels(chnr&).shops(bestshop&).material >= channelsnosave(chnr&).unitclasses(bestunittp&).costmaterial
        unitnr& = AIBuildUnit&(chnr&, bestshop&, bestunittp&)
        IF unitnr& < 0 THEN EXIT LOOP  'Shop ist voll
        a$$ = a$$+channelsnosave(chnr&).unitclasses(bestunittp&).uname+"??,"
      WEND
    END IF
  END IF

'  IF debugEnabled& <> 0 AND a$$ <> "" THEN CALL AddProtocol("$COLOR"+FORMAT$(plnr&+1)+"$Built: "+LEFT$(a$$, LEN(a$$)-1)+"$COLOR0$")
END SUB



'AI Spieler beendet seinen Zug (nur Server)
SUB AIEndTurn(chnr&, plnr&)
  channels(chnr&).player(plnr&).state = %PLAYERSTATE_READY
  IF AllPlayersFinishedTurn&(chnr&) THEN
    CALL EndTurn(chnr&)
    CALL SendPacketToChannel(chnr&, "ACTT", CHR$(channels(chnr&).info.activeteam))
  END IF
END SUB



'Alle AI Daten zurücksetzen
SUB AIResetData(chnr&)
  LOCAL unitnr&, unittp&, shopnr&

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    unittp& = channels(chnr&).units(unitnr&).unittype
    channels(chnr&).units(unitnr&).aitargetpriority = 0
    channels(chnr&).units(unitnr&).aicommand = IIF&((channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_CRYSTAL) = 0, %AIC_NONE, %AIC_RETURNCRYSTALS)
    channels(chnr&).units(unitnr&).aitargetunit = -1
    channels(chnr&).units(unitnr&).aitargetshop = -1
  NEXT unitnr&
  CALL AIResetIgnoreFlag(chnr&)

  FOR shopnr& = 0 TO channels(chnr&).info.nshops-1
    channels(chnr&).shops(shopnr&).prio = 0
  NEXT shopnr&
END SUB



'Ignorieren-Flag aller Einheiten zurücksetzen
SUB AIResetIgnoreFlag(chnr&)
  LOCAL unitnr&

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags AND NOT %US_AI_IGNORE
  NEXT unitnr&
END SUB



'AI Hauptfunktion
SUB AIMain(chnr&, plnr&)
  IF channels(chnr&).info.state >= %CHANNELSTATE_VICTORY AND channels(chnr&).info.state <= %CHANNELSTATE_DEFEAT THEN EXIT SUB
'IF gameMode& = %GAMEMODE_SERVER THEN PRINT"AIMain"
'CALL EndTurn(chnr&)
'EXIT SUB
  CALL UpdateVision(chnr&, plnr&)
  CALL AIResetData(chnr&)
  CALL AIFindVictoryPosition(chnr&, plnr&)
  CALL AICheckWinConditions(chnr&, plnr&)
  CALL AICheckLossConditions(chnr&, plnr&)
  CALL AISetTargetPriorityAll(chnr&, plnr&)
  CALL AIFindShopsToOccupy(chnr&, plnr&)
  CALL AIRefuelNoMove(chnr&, plnr&)
  CALL AICheckUnmovableUnits(chnr&, plnr&, 0)
  CALL AICheckCooldownUnits(chnr&, plnr&)
  CALL AIMoveRecon(chnr&, plnr&)
  CALL AICollectAldinium(chnr&, plnr&)
  CALL AIMainAttack(chnr&, plnr&)
  IF channels(chnr&).info.state >= %CHANNELSTATE_VICTORY AND channels(chnr&).info.state <= %CHANNELSTATE_DEFEAT THEN EXIT SUB
  CALL AIOccupyShops(chnr&, plnr&)
  IF channels(chnr&).info.state >= %CHANNELSTATE_VICTORY AND channels(chnr&).info.state <= %CHANNELSTATE_DEFEAT THEN EXIT SUB
  CALL AIMoveEndangered(chnr&, plnr&)
  CALL AIEscort(chnr&, plnr&)
  CALL AICheckUnmovableUnits(chnr&, plnr&, 1)
  CALL AIRefuel(chnr&, plnr&)
  CALL AIUnloadShops(chnr&, plnr&)
  CALL AIProduceUnits(chnr&, plnr&)

  IF gameMode& = %GAMEMODE_SERVER THEN
    CALL AIEndTurn(chnr&, plnr&)
  ELSE
    CALL EndTurn(chnr&)
  END IF
END SUB



'AI Thread
THREAD FUNCTION AIThread&(BYVAL chnr&)
  LOCAL plnr&, activeteam&

  DO
    IF gameMode& = %GAMEMODE_SINGLE THEN
      'prüfen, ob aktiver Spieler ein AI Spieler ist
      plnr& = channels(chnr&).info.activeplayer
      IF (channels(chnr&).info.aimask AND 2^plnr&) <> 0 AND replayMode&(chnr&) < %REPLAYMODE_PLAY THEN CALL AIMain(chnr&, plnr&)
    END IF
    IF gameMode& = %GAMEMODE_SERVER THEN
      'prüfen, ob ein AI Spieler im aktiven Team ist
      activeteam& = channels(chnr&).info.activeteam
      FOR plnr& = 0 TO %MAXPLAYERS-1
        IF (channels(chnr&).info.aimask AND 2^plnr&) <> 0 AND (channels(chnr&).info.aliveplayers AND 2^plnr&) <> 0 AND channels(chnr&).player(plnr&).team = activeteam& AND channels(chnr&).player(plnr&).state <> %PLAYERSTATE_READY THEN
          CALL AIMain(chnr&, plnr&)
        END IF
      NEXT plnr&
    END IF
    SLEEP 50
  LOOP WHILE exitprg& = 0 AND (gameMode& <> %GAMEMODE_SERVER OR channels(chnr&).info.state > %CHANNELSTATE_EMPTY)
END FUNCTION
