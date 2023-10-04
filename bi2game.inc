'Battle Isle 2020 - Spiellogik


'Prüft, ob eine Einheit vernichtet wurde
FUNCTION UnitIsAlive&(BYVAL chnr&, BYVAL unitnr&)
  UnitIsAlive& = IIF&(channels(chnr&).units(unitnr&).groupsize = 0 OR channels(chnr&).units(unitnr&).groupsize > 10, 0, -1)
END FUNCTION



'Prüft, ob eine Einheit sich in einem Shop befindet
'Liefert die Shop-Nummer oder -1 falls sich die Einheit nicht in einem Shop befindet
FUNCTION UnitIsInShop&(BYVAL chnr&, BYVAL unitnr&)
  LOCAL unr&, shopnr&

  shopnr& = -1
  unr& = channels(chnr&).zone3(channels(chnr&).units(unitnr&).xpos, channels(chnr&).units(unitnr&).ypos)
  IF unr& < -1 THEN shopnr& = -2-unr&

  UnitIsInShop& = shopnr&
END FUNCTION



'Prüft, ob eine Einheit sich in einem Transporter befindet
'Liefert die Einheiten-Nummer des Transporters oder -1 falls sich die Einheit nicht in einem Transporter befindet
FUNCTION UnitIsInTransporter&(BYVAL chnr&, BYVAL unitnr&)
  LOCAL unr&, transporter&

  transporter& = -1
  unr& = channels(chnr&).zone3(channels(chnr&).units(unitnr&).xpos, channels(chnr&).units(unitnr&).ypos)
  IF unr >= 0 AND unr& <> unitnr& THEN
    IF (channelsnosave(chnr&).unitclasses(channels(chnr&).units(unr&).unittype).flags AND %UCF_TRANSPORTER) <> 0 THEN transporter& = unr&
  END IF

  UnitIsInTransporter& = transporter&
END FUNCTION



'Prüft, ob eine Einheit einem Spieler in einem gegnerischen Team gehört
FUNCTION IsEnemyUnit&(BYVAL chnr&, BYVAL plnr&, BYVAL unitnr&)
  LOCAL r&, owner&

  IF unitnr& >= 0 THEN
    owner& = channels(chnr&).units(unitnr&).owner
    IF owner& < 6 AND (channels(chnr&).player(plnr&).allymask AND 2^owner&) = 0 THEN r& = -1
  END IF

  IsEnemyUnit& = r&
END FUNCTION



'Liefert die Spielphase in der sich ein Spieler befindet
FUNCTION GetPhase&(BYVAL chnr&, BYVAL plnr&)
  GetPhase& = channels(chnr&).player(plnr&).phase
END FUNCTION



'Prüft ob sich mindestens ein Spieler in einer bestimmten Phase befindet
FUNCTION AnyPlayerHasPhase&(BYVAL chnr&, BYVAL phase&)
  LOCAL plnr&

  FOR plnr& = 0 TO %MAXPLAYERS-1
    IF GetPhase&(chnr&, plnr&) = phase& THEN
      AnyPlayerHasPhase& = -1
      EXIT FUNCTION
    END IF
  NEXT plnr&

  AnyPlayerHasPhase& = 0
END FUNCTION



'Setzt die Spielphase für einen Spieler
SUB SetPhase(BYVAL chnr&, BYVAL plnr&, BYVAL newphase&)
  channels(chnr&).player(plnr&).phase = newphase&
END SUB



'Setzt die Spielphase für alle Spieler
SUB SetPhaseAll(BYVAL chnr&, BYVAL newphase&, BYVAL oldphase&)
  LOCAL plnr&

  FOR plnr& = 0 TO %MAXPLAYERS-1
    IF oldphase& = -1 OR GetPhase&(chnr&, plnr&) = oldphase& THEN CALL SetPhase(chnr&, plnr&, newphase&)
  NEXT plnr&
END SUB



'Weist jedem Spieler auf der Karte eine eindeutige Team-ID für seine Allianzmaske zu
SUB SetTeams(BYVAL chnr&)
  LOCAL plnr&, team&, mask&, nmasks&, i&, allymasks&()
  DIM allymasks&(%MAXPLAYERS-1)

  team& = 0
  FOR plnr& = 0 TO %MAXPLAYERS-1
    mask& = channels(chnr&).player(plnr&).allymask
    ARRAY SCAN allymasks&(0) FOR nmasks&, =mask&, TO i&
    IF nmasks& = 0 OR i& = 0 THEN
      team& = team&+1
      allymasks&(nmasks&) = mask&
      nmasks& = nmasks&+1
      channels(chnr&).player(plnr&).team = team&
    ELSE
      channels(chnr&).player(plnr&).team = i&
    END IF
  NEXT plnr&
END SUB



'Prüft, ob ein Team noch lebende Spieler hat
FUNCTION TeamIsAlive&(chnr&, team&)
  LOCAL plnr&, n&

  FOR plnr& = 0 TO %MAXPLAYERS-1
    IF (channels(chnr&).info.aliveplayers AND 2^plnr&) <> 0 AND channels(chnr&).player(plnr&).team = team& THEN n& = n&+1
  NEXT plnr&

  TeamIsAlive& = n&
END FUNCTION



'Prüft, ob ein bestimmter Spieler am Zug ist
FUNCTION IsActivePlayer&(chnr&, plnr&)
  LOCAL r&

  IF gameMode& = %GAMEMODE_SINGLE THEN
    IF channels(chnr&).info.activeplayer = plnr& THEN r = -1
  ELSE
    IF plnr& >= 0 AND plnr& < 6 AND channels(chnr&).info.activeteam = channels(chnr&).player(plnr&).team THEN r& = -1
  END IF

  IsActivePlayer& = r&
END FUNCTION



'Prüft, in welchem Spielmdous das Spiel läuft
'2 = Battle Isle 2
'3 = Battle Isle 3
FUNCTION CompatibilityMode&(chnr&)
  LOCAL md&

  md& = 2
  IF channels(chnr&).campaign.episode = 5 THEN md& = 3

  CompatibilityMode& = md&
END FUNCTION



'Zählt die freien Slots in einem Transporter
FUNCTION CountFreeTransportSlots&(chnr&, transporter&)
  LOCAL i&, n&

  FOR i& = 0 TO 7
    IF channels(chnr&).units(transporter&).transportcontent(i&) = -1 THEN n& = n&+1
  NEXT slotnr&

  CountFreeTransportSlots& = n&
END FUNCTION



'Prüft, ob ein Transporter eine bestimmte Einheitenklasse geladen hat
FUNCTION TransporterHasLoaded&(chnr&, transporter&, cargotype&)
  LOCAL slotnr&, unr&

  FOR slotnr& = 0 TO 7
    unr& = channels(chnr&).units(transporter&).transportcontent(slotnr&)
    IF unr& >= 0 AND channels(chnr&).units(unr&).unittype = cargotype& THEN
      TransporterHasLoaded& = -1
      EXIT FUNCTION
    END IF
  NEXT slotnr&

  TransporterHasLoaded& = 0
END FUNCTION



'Ermittelt das Gesamtgewicht der transportierten Einheiten in einem Transporter
FUNCTION GetTransportWeight&(chnr&, transporter&)
  LOCAL slotnr&, unr&, totalweight&

  FOR slotnr& = 0 TO 7
    unr& = channels(chnr&).units(transporter&).transportcontent(slotnr&)
    IF unr& >= 0 THEN totalweight& = totalweight&+channelsnosave(chnr&).unitclasses(channels(chnr&).units(unr&).unittype).weight
  NEXT slotnr&

  GetTransportWeight& = totalweight&
END FUNCTION



'Prüft, ob eine Einheit in einen Transporter geladen werden kann
FUNCTION TransporterCanLoadUnit&(chnr&, transporter&, unitnr&)
  LOCAL i&, transportertp&, unittp&, unr&, freeslots&, availableweight&, requiredslots&, requiredweight&

  'Transportklasse prüfen
  transportertp& = channels(chnr&).units(transporter&).unittype
  unittp& = channels(chnr&).units(unitnr&).unittype
  IF (channelsnosave(chnr&).unitclasses(transportertp&).transportmask AND channelsnosave(chnr&).unitclasses(unittp&).transportclass) = 0 THEN EXIT FUNCTION

  'freie Plätze und verfügbares Gewicht ermitteln
  availableweight& = channelsnosave(chnr&).unitclasses(transportertp&).transportvolume
  FOR i& = 0 TO 7
    unr& = channels(chnr&).units(transporter&).transportcontent(i&)
    IF unr& = -1 THEN
      freeslots& = freeslots&+1
    ELSE
      availableweight& = availableweight&-channelsnosave(chnr&).unitclasses(channels(chnr&).units(unr&).unittype).weight
    END IF
  NEXT i&

  'benötigte Plätze und Gewicht ermitteln
  requiredslots& = 1
  requiredweight& = channelsnosave(chnr&).unitclasses(unittp&).weight
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_TRANSPORTER) <> 0 THEN
    FOR i& = 0 TO 7
      unr& = channels(chnr&).units(unitnr&).transportcontent(i&)
      IF unr& >= 0 THEN
        requiredslots& = requiredslots&+1
        requiredweight& = requiredweight&+channelsnosave(chnr&).unitclasses(channels(chnr&).units(unr&).unittype).weight
      END IF
    NEXT i&
  END IF

  TransporterCanLoadUnit& = (requiredslots& <= freeslots&) AND (requiredweight& <= availableweight&)
END FUNCTION



'Einheit in Transporter laden
SUB TransportLoad(chnr&, transporter&, unitnr&)
  LOCAL slotnr&, unr&

  'freien Slot suchen
  FOR slotnr& = 0 TO 7
    IF channels(chnr&).units(transporter&).transportcontent(slotnr&) = -1 THEN EXIT FOR
  NEXT slotnr&
  IF slotnr& = 8 THEN EXIT SUB

  'Einheit im Transporter platzieren
  channels(chnr&).units(transporter&).transportcontent(slotnr&) = unitnr&

  'Besitzer der geladenen Einheit auf Besitzer des Transporters ändern
  channels(chnr&).units(unitnr&).owner = channels(chnr&).units(transporter&).owner

  'falls Einheit selber ein Transporter ist, dann ihren Inhalt in den neuen Transporter umladen
  IF (channelsnosave(chnr&).unitclasses(channels(chnr&).units(unitnr&).unittype).flags AND %UCF_TRANSPORTER) <> 0 THEN
    FOR slotnr& = 0 TO 7
      unr& = channels(chnr&).units(unitnr&).transportcontent(slotnr&)
      IF unr& >= 0 THEN
        CALL TransportLoad(chnr&, transporter&, unr&)
        channels(chnr&).units(unitnr&).transportcontent(slotnr&) = -1
      END IF
    NEXT slotnr&
  END IF
END SUB



'Einheit aus Transporter ausladen
SUB TransportUnload(chnr&, transporter&, unitnr&)
  LOCAL slotnr&

  'Einheit im Transporter suchen
  FOR slotnr& = 0 TO 7
    IF channels(chnr&).units(transporter&).transportcontent(slotnr&) = unitnr& THEN channels(chnr&).units(transporter&).transportcontent(slotnr&) = -1
  NEXT slotnr&
  channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags OR %US_MOVEDFROMSHOP
END SUB



'Ermittelt den Besitzer einer Einheit/Gebäudes aus dem Code
FUNCTION GetOwnerFromCode&(c&)
  LOCAL owner&

  SELECT CASE c&
  CASE 256 TO 511: owner& = 0
  CASE 512 TO 1023: owner& = 1
  CASE 1024 TO 2047: owner& = 2
  CASE 2048 TO 4095: owner& = 3
  CASE 4096 TO 8191: owner& = 4
  CASE 8192 TO 16383: owner& = 5
  CASE ELSE: owner& = 6
  END SELECT

  GetOwnerFromCode& = owner&
END FUNCTION



'Liefert den ersten freien Slot in einem Shop zurück
FUNCTION GetFreeShopSlot&(chnr&, shopnr&)
  LOCAL i&

  FOR i& = 0 TO 15
    IF channels(chnr&).shops(shopnr&).content(i&) = -1 THEN
      GetFreeShopSlot& = i&
      EXIT FUNCTION
    END IF
  NEXT i&

  GetFreeShopSlot& = -1
END FUNCTION



'Zählt die freien Slots in einem Shop
FUNCTION CountFreeShopSlots&(chnr&, shopnr&)
  LOCAL i&, n&

  FOR i& = 0 TO 15
    IF channels(chnr&).shops(shopnr&).content(i&) = -1 THEN n& = n&+1
  NEXT i&

  CountFreeShopSlots& = n&
END FUNCTION



'Alle Einheiten zerstören, die auf Felder stehen, die sie nicht mehr betreten können
SUB RemoveUnitsOnInvalidTerrain(chnr&, startrow&, endrow&)
  LOCAL x&, y&, unitnr&, unittp&, terrain&

  FOR y& = startrow& TO endrow&
    FOR x& = 0 TO channels(chnr&).info.xsize-1
      unitnr& = channels(chnr&).zone3(x&, y&)
      IF unitnr& >= 0 THEN
        unittp& = channels(chnr&).units(unitnr&).unittype
        terrain& = channels(chnr&).zone1(x&, y&)
        IF (channelsnosave(chnr&).terraindef(terrain&).typemask AND %TERRAIN_WATER) <> 0 AND CanEnterTerrain&(chnr&, x&, y&, channelsnosave(chnr&).unitclasses(unittp&).terrain) = 0 THEN
          CALL KillUnit(chnr&, unitnr&, -1, %KILLREASON_SUNKEN, 0)
        END IF
      END IF
    NEXT x&
  NEXT y&
END SUB



'Wetter anwenden
SUB ApplyWeather(chnr&, BYVAL newweather&, BYVAL startrow&, BYVAL endrow&)
  LOCAL a$, x&, y&, sprnr&, unittp&

  IF newweather& < 0 OR newweather& > 4 THEN EXIT SUB
  startrow& = MIN&(channels(chnr&).info.ysize-1, MAX&(0, startrow&))
  endrow& = MIN&(channels(chnr&).info.ysize-1, MAX&(0, endrow&))

  IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateWeatherToClients(chnr&, newweather&, startrow&, endrow&)

  'Sprites ändern
  FOR y& = startrow& TO endrow&
    FOR x& = 0 TO channels(chnr&).info.xsize-1
      sprnr& = channels(chnr&).zone1(x&, y&)
      channels(chnr&).zone1(x&, y&) = channelsnosave(chnr&).terraindef(sprnr&).weathermapping(newweather&)
      sprnr& = channels(chnr&).zone2(x&, y&)
      IF sprnr& >= 0 THEN channels(chnr&).zone2(x&, y&) = channelsnosave(chnr&).terraindef(sprnr&).weathermapping(newweather&)
    NEXT x&
  NEXT y&

  'bei Tauwetter alle Einheiten zerstören, die nun auf Felder stehen, die sie nicht mehr betreten können
  IF channels(chnr&).info.weather = %WEATHER_HEAVYSNOW AND newweather& <> %WEATHER_HEAVYSNOW THEN CALL RemoveUnitsOnInvalidTerrain(chnr&, startrow&, endrow&)

  'neues Wetter vermerken
  channels(chnr&).info.weather = newweather&
END SUB



'Zählt die Einheiten eines Spielers
FUNCTION CountUnits&(chnr&, plnr&, unittp&)
  LOCAL unitnr&, n&

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF UnitIsAlive&(chnr&, unitnr&) <> 0 AND channels(chnr&).units(unitnr&).owner = plnr& AND (unittp& = -1 OR channels(chnr&).units(unitnr&).unittype = unittp&) THEN n& = n&+1
  NEXT unitnr&

  CountUnits& = n&
END FUNCTION



'Erstellt eine Liste mit allen Einheiten und Erfahrungspunkten sowie eine Liste mit allen Einheiten nach Typ
SUB CreateUnitListsForPlayer(chnr&, plnr&)
  LOCAL unitnr&, unittp&, n&, cnt&, totalunits&
  LOCAL unr&(), xp&()

  'Liste mit Einheiten nach Erfahrungspunkten
  totalunits& = channels(chnr&).info.nunits-1
  DIM unr&(totalunits&), xp&(totalunits&)
  FOR unitnr& = 0 TO totalunits&
    IF UnitIsAlive&(chnr&, unitnr&) <> 0 AND channels(chnr&).units(unitnr&).owner = plnr& THEN
      unr&(n&) = channels(chnr&).units(unitnr&).unittype
      xp&(n&) = channels(chnr&).units(unitnr&).experience
      n& = n&+1
    END IF
  NEXT unitnr&
  ARRAY SORT xp&(0) FOR n&, TAGARRAY unr&(), DESCEND
  unitListByXP$ = ""
  FOR unitnr& = 0 TO n&-1
    unitListByXP$ = unitListByXP$+CHR$(unr&(unitnr&))+CHR$(xp(unitnr&))
  NEXT unitnr&

  'Liste mit Einheiten nach Typ
  n& = 0
  FOR unittp& = 0 TO channelsnosave(chnr&).nunitclasses-1
    cnt& = CountUnits&(chnr&, plnr&, unittp&)
    IF cnt& > 0 THEN
      unr&(n&) = unittp&
      xp&(n&) = cnt&
      n& = n&+1
    END IF
  NEXT i&
  ARRAY SORT xp&(0) FOR n&, TAGARRAY unr&()
  unitListByType$ = ""
  FOR unitnr& = 0 TO n&-1
    unitListByType$ = unitListByType$+CHR$(unr&(unitnr&))+CHR$(xp(unitnr&))
  NEXT unitnr&
END SUB



'Aktionsstufe aus Kategorie ermitteln
FUNCTION GetActionLevel&(BYVAL category&)
  LOCAL level&

  IF category& < 48 THEN level& = category&
  IF category& > 48 AND category& < 80 THEN level& = category&-48
  IF category& > 80 THEN level& = category&-80

  GetActionLevel& = level&
END FUNCTION



'Prüft alle Aktionen (außer Siegbedingungen)
SUB CheckActions(chnr&)
  LOCAL p&, now&, plnr&, msgmask&, startrow&, endrow&

  IF gameMode& <> %GAMEMODE_SERVER AND gameState& <> %GAMESTATE_INGAME THEN EXIT SUB
  p& = channels(chnr&).info.actionposition
  now& = channels(chnr&).info.turn*10000+channels(chnr&).info.movement
  IF gameMode& = %GAMEMODE_CLIENT THEN EXIT SUB
  FOR plnr& = 0 TO %MAXPLAYERS-1
    IF GetPhase&(chnr&, plnr&) >= %PHASE_SURPRISEATTACK THEN EXIT SUB
  NEXT plnr&
'CALL BILog(FORMAT$(p&)+" , "+FORMAT$(now&)+" , "+FORMAT$(menuOpenTime!)+" , "+FORMAT$(dialogueClosing&), 0)
  IF menuOpenTime! > 0 AND menuSelectedEntry& <> -2 AND now& > 0 THEN EXIT SUB
  IF dialogueClosing& <> 0 AND now& > 0 THEN EXIT SUB

  WHILE p& < channels(chnr&).info.nactions AND channels(chnr&).actions(p&).turn*10000+channels(chnr&).actions(p&).movement <= now&
'CALL BILog("Turn "+FORMAT$(channels(chnr&).actions(p&).turn)+" , Move "+FORMAT$(channels(chnr&).actions(p&).movement)+" , Action "+FORMAT$(channels(chnr&).actions(p&).actiontype)+" , Param "+FORMAT$(channels(chnr&).actions(p&).actionparam), 0)
    IF GetActionLevel&(channels(chnr&).actions(p&).category) = 0 THEN
      'Aktion ausführen
      SELECT CASE channels(chnr&).actions(p&).actiontype
      CASE %ACTYPE_WEATHER
        CALL ApplyWeather(chnr&, channels(chnr&).actions(p&).actionparam, 0, channels(chnr&).info.ysize-1)

      CASE %ACTYPE_MESSAGE
        plnr& = LOG2(channels(chnr&).actions(p&).player)
        IF (msgmask& AND channels(chnr&).actions(p&).player) <> 0 THEN EXIT LOOP
        IF IsVideoMessage&(chnr&, channels(chnr&).actions(p&).actionparam) THEN
          CALL ShowVideoMessage(chnr&, plnr&, channels(chnr&).actions(p&).actionparam)
        ELSE
          CALL ShowGameMessage(chnr&, plnr&, channels(chnr&).actions(p&).actionparam)
        END IF
        msgmask& = msgmask& OR channels(chnr&).actions(p&).player

      CASE %ACTYPE_DFLAYER
        CALL ActivateDFLayer(chnr&, channels(chnr&).actions(p&).actionparam)

      CASE %ACTYPE_DFLAYER_DIFFICULTY1, %ACTYPE_DFLAYER_DIFFICULTY2, %ACTYPE_DFLAYER_DIFFICULTY3
        IF channels(chnr&).info.difficulty = channels(chnr&).actions(p&).actiontype-%ACTYPE_DFLAYER_DIFFICULTY1 THEN CALL ActivateDFLayer(chnr&, channels(chnr&).actions(p&).actionparam)

      CASE %ACTYPE_ALLIANCE
        plnr& = LOG2(channels(chnr&).actions(p&).player)
        CALL ChangeAlliance(chnr&, plnr&, channels(chnr&).actions(p&).newally)

      CASE %ACTYPE_CUSTOM_MESSAGE
        plnr& = LOG2(channels(chnr&).actions(p&).player)
        IF (msgmask& AND plnr&) <> 0 THEN EXIT LOOP
        CALL ShowGameMessage(chnr&, plnr&, channels(chnr&).actions(p&).actionparam+512)
        msgmask& = msgmask& OR plnr&

      CASE %ACTYPE_LOCALWEATHER
        startrow& = channels(chnr&).actions(p&).shop AND 255
        endrow& = INT(channels(chnr&).actions(p&).shop/256)
        CALL ApplyWeather(chnr&, channels(chnr&).actions(p&).actionparam, startrow&, endrow&)

      END SELECT
    END IF

    p& = p&+1
    channels(chnr&).info.actionposition = p&
  WEND

  IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateInfoToClients(chnr&, VARPTR(channels(chnr&).info.actionposition)-VARPTR(channels(chnr&).info), MKL$(channels(chnr&).info.actionposition))
END SUB



'Prüft alle Siegbedingungen
SUB CheckVictoryConditions(chnr&)
  LOCAL vc&, r&, plnr&, pl&, team&, lastteam&, n&, unittp&, shopnr&, nvictory&, ndefeat&, bonus&, winningteam&, humanmask&
  LOCAL missionnr&, relativemissionnr&, episode&

  IF gameMode& = %GAMEMODE_CLIENT THEN EXIT SUB

  'alle Siegbedingungen prüfen (diese stehen am Anfang der Aktionsliste)
  humanmask& = channels(chnr&).info.originalplayers AND NOT channels(chnr&).info.aimask
  winningteam& = -1
  FOR vc& = 0 TO channels(chnr&).info.nvictoryconditions-1
'PRINT channels(chnr&).actions(vc&).actiontype, channels(chnr&).actions(vc&).actionparam, channels(chnr&).actions(vc&).player, channels(chnr&).actions(vc&).shop
    r& = 0
    SELECT CASE channels(chnr&).actions(vc&).actiontype
    CASE %ACTYPE_VC_PLAYERDEFEATED  'Spieler besiegt (keine Einheiten mehr)
      pl& = channels(chnr&).actions(vc&).actionparam
      IF CountUnits&(chnr&, pl&, -1) = 0 THEN r& = 1

    CASE %ACTYPE_VC_ALLENEMIES_DEFEATED:  'alle gegnerischen Spieler besiegt
      plnr& = channels(chnr&).actions(vc&).player
      team& = channels(chnr&).player(plnr&).team
      n& = 0
      FOR pl& = 0 TO %MAXPLAYERS-1
        IF team& <> channels(chnr&).player(pl&).team THEN n& = n&+CountUnits&(chnr&, pl&, -1)
      NEXT pl&
      IF n& = 0 THEN
        winningteam& = team&
        r& = 1
      END IF

    CASE %ACTYPE_VC_SHOPOCCUPIED    'Shop erobert
      pl& = channels(chnr&).actions(vc&).actionparam
      shopnr& = channels(chnr&).actions(vc&).shop
      IF shopnr& >= 0 AND shopnr& < channels(chnr&).info.nshops AND channels(chnr&).shops(shopnr&).shoptype > 0 AND channels(chnr&).shops(shopnr&).owner = pl& THEN r& = 1

    CASE %ACTYPE_VC_UNITDEAD_PLAYER1 TO %ACTYPE_VC_UNITDEAD_PLAYER6  'alle Einheiten eines Typs eines Spielers vernichtet
      pl& = channels(chnr&).actions(vc&).shop
      unittp& = channels(chnr&).actions(vc&).actionparam
      IF CountUnits&(chnr&, pl&, unittp&) = 0 THEN r& = 1

    CASE %ACTYPE_VC_TEAM_CONTROLS_ALL_HQS:  'ein Team kontrolliert alle HQs
      r& = 1
      lastteam& = -1
      FOR shopnr& = 0 TO channels(chnr&).info.nshops-1
        plnr& = channels(chnr&).shops(shopnr&).owner
        IF channels(chnr&).shops(shopnr&).shoptype = %SHOPTYPE_HQ AND plnr& < %MAXPLAYERS THEN
          team& = channels(chnr&).player(plnr&).team
          IF lastteam& = -1 THEN
            lastteam& = team&
          ELSE
            IF lastteam& <> team& THEN
              r& = 0
              EXIT FOR
            END IF
          END IF
        END IF
      NEXT shopnr&
      IF r& = 1 THEN winningteam& = lastteam&

    CASE %ACTYPE_BONUS_SHOPOCCUPIED  'Shop erobert
      pl& = channels(chnr&).actions(vc&).actionparam
      shopnr& = channels(chnr&).actions(vc&).shop
      IF shopnr& >= 0 AND shopnr& < channels(chnr&).info.nshops AND channels(chnr&).shops(shopnr&).shoptype > 0 AND channels(chnr&).shops(shopnr&).owner = pl& THEN
        bonus& = 1
        r& = 1
      END IF

    CASE %ACTYPE_BONUS_TIMELIMIT  'Runde noch nicht erreicht
      IF channels(chnr&).info.turn < channels(chnr&).actions(vc&).actionparam THEN bonus& = 1

    CASE %ACTYPE_BONUS_NOUNITSLOST  'keine Einheit verloren
      pl& = channels(chnr&).actions(vc&).actionparam
      IF channels(chnr&).player(pl&).lostunits = 0 THEN bonus& = 1

    CASE %ACTYPE_BONUS_UNITSURVIVED  'Einheit hat überlebt
      pl& = channels(chnr&).actions(vc&).shop
      unittp& = channels(chnr&).actions(vc&).actionparam
      IF CountUnits&(chnr&, pl&, unittp&) > 0 THEN bonus& = 1

    CASE %ACTYPE_BONUS_SUPERSECRET  'alle zu Spielbeginn vorhandenen Landeinheiten haben überlebt und befinden sich nördlich von Zeile 10
      IF CheckSpecialVictoryCondition&(chnr&, 0) <> 0 THEN bonus& = 1

    END SELECT
    IF r& <> 0 THEN
      IF channels(chnr&).actions(vc&).category = %ACCAT_VICTORY THEN
        nvictory& = nvictory&+1
      ELSE
        ndefeat& = ndefeat&+1
        defeatCondition& = vc&
      END IF
    END IF
  NEXT vc&

  'Zeitlimit prüfen
  IF channels(chnr&).info.turnlimit > 0 AND channels(chnr&).info.turn >= channels(chnr&).info.turnlimit THEN
    IF channels(chnr&).info.turnlimitisvictory = 1 THEN
      nvictory& = nvictory&+1
    ELSE
      ndefeat& = ndefeat&+1
      defeatCondition& = -1
    END IF
  END IF

  'Ergebnis auswerten
  episode& = channels(chnr&).campaign.episode
  missionnr& = channels(chnr&).info.currentmission
  relativemissionnr& = missionnr&-GetEpisodeStartMap&(episode&)
  IF nvictory& >= channels(chnr&).info.requiredvictorycond THEN
    IF CheckNextActionLevel&(chnr&) <> 0 THEN EXIT SUB
    IF gameMode& = %GAMEMODE_SERVER THEN
      IF winningteam& = -1 THEN
        IF GetMultiplayerMode&(chnr&) = %MULTIPLAYERMODE_COOP THEN
          'die menschlichen Spieler gewinnen
          plnr& = INT(LOG2(humanmask&))
          winningteam& = channels(chnr&).player(plnr&).team
        ELSE
          'Spieler 1 (blau) gewinnt
          winningteam& = channels(chnr&).player(0).team
        END IF
      END IF
      CALL ServerGameOver(chnr&, winningteam&)
    ELSE
      channels(chnr&).info.state = IIF&(bonus& = 0, %CHANNELSTATE_VICTORY, %CHANNELSTATE_VICTORYBONUS)
      IF selectedShop& >= 0 THEN CALL ExitButtonPressed(0)
      IF episode& = 5 AND relativemissionnr& >= 0 AND relativemissionnr& <= UBOUND(missionVideoNumbers) THEN
        CALL ShowVideoMessage(chnr&, localPlayerNr&, missionVideoNumbers(relativemissionnr&).victory)
      ELSE
        CALL ShowGameMessage(chnr&, localPlayerNr&, IIF&(bonus& = 0, %MSG_VICTORY, %MSG_SECRET_LEVEL))
      END IF
      menuOpenTime! = 0
      shopSelectionTime! = 0
    END IF
    CALL StopRecordReplay(chnr&, 1)
    EXIT SUB
  END IF
  IF ndefeat& > 0 THEN
    IF gameMode& = %GAMEMODE_SERVER THEN
      IF winningteam& = -1 THEN
        IF GetMultiplayerMode&(chnr&) = %MULTIPLAYERMODE_COOP THEN
          'die menschlichen Spieler verlieren (winningteam& kann auf -1 stehen bleiben, da die KI eh keinen Gameover-Screen sieht)
        ELSE
          'Spieler 1 (blau) verliert (und der andere Spieler gewinnt)
          plnr& = INT(LOG2(humanmask& AND 62))
          winningteam& = channels(chnr&).player(plnr&).team
        END IF
      END IF
      CALL ServerGameOver(chnr&, winningteam&)
    ELSE
      channels(chnr&).info.state = %CHANNELSTATE_DEFEAT
      IF selectedShop& >= 0 THEN CALL ExitButtonPressed(0)
      IF episode& = 5 AND relativemissionnr& >= 0 AND relativemissionnr& <= UBOUND(missionVideoNumbers) THEN
        CALL ShowVideoMessage(chnr&, localPlayerNr&, missionVideoNumbers(relativemissionnr&).defeat)
      ELSE
        CALL ShowGameMessage(chnr&, localPlayerNr&, %MSG_DEFEAT)
      END IF
      menuOpenTime! = 0
      shopSelectionTime! = 0
    END IF
    CALL StopRecordReplay(chnr&, 1)
    EXIT SUB
  END IF
END SUB



'Bedingung für Super-Geheim-Mission prüfen
FUNCTION CheckSpecialVictoryCondition&(chnr&, plnr&)
  LOCAL a$$, i&, n&, nsuccess&, unitnr&

  n& = LEN($SUPERSECRETUNITS)

  'prüfen, ob die Start-Landeinheiten des Spielers alle noch leben (und nicht ersetzt wurden) und sich nördlich von Zeile 10 befinden
  FOR i& = 1 TO n&
    unitnr& = ASC($SUPERSECRETUNITS, i&)
    IF UnitIsAlive&(chnr&, unitnr&) <> 0 AND channels(chnr&).units(unitnr&).owner = plnr& AND channels(chnr&).units(unitnr&).unittype = ASC($SPECIALVICTORYUNITTYPES, i&) AND channels(chnr&).units(unitnr&).ypos < 10 THEN nsuccess& = nsuccess&+1
  NEXT i&

  'falls Bedingung erfüllt ist, Siegmeldung erstellen
  IF nsuccess& = n& THEN
    IF langcode$ = "GER" THEN
      a$$ = "^VOC1Gratulation Stratege! Sie haben nicht nur die Mission erfolgreich abgeschlossen, sondern außerdem jede einzelne Person evakuiert und damit vor dem sicheren Tod gerettet."+CHR$(13,13) _
        + "Das Volk von Chromos wird Ihnen diese Heldentat nie vergessen. Zudem konnte uns einer der Geretteten von einer geheimen Titan-Net Basis auf einer uns bislang unbekannten Insel berichten. Gehen Sie diesem Hinweis umgehend nach."
    ELSE
      a$$ = "^VOC1Congratulations strategist! Not only did you successfully complete the mission, but you also evacuated every single person, saving them from certain death."+CHR$(13,13) _
        + "The people of Chromos will never forget this feat for you. In addition, one of the rescued was able to tell us about a secret Titan-Net base on a previously unknown island. Follow up this notice immediately."
    END IF
    gameMessages$$(%MSG_SECRET_LEVEL) = FormatGameMessage(a$$)
  END IF

  CheckSpecialVictoryCondition& = (nsuccess& = n&)
END FUNCTION



'Prüft, ob die Mission weitere Aktionsstufen hat und aktiviert das nächste Level
FUNCTION CheckNextActionLevel&(chnr&)
  LOCAL i&, category&, actionlevel&, deleted&, nextlevelfound&, tp&, vc&
  LOCAL actions$, differentvictoryconditions$

  FOR i& = channels(chnr&).info.nactions-1 TO 0 STEP -1
    category& = channels(chnr&).actions(i&).category
    actionlevel& = GetActionLevel&(category&)
    IF actionlevel& = 0 THEN
      'alle Aktionen des aktiven Levels löschen
      CALL DeleteAction(chnr&, i&)
      deleted& = deleted&+1
    ELSE
      'Aktionsstufe um 1 reduzieren, so daß Aktionsstufe 1 nun zum neuen aktiven Level wird
      channels(chnr&).actions(i&).category = category&-1
      IF GetActionLevel&(category&-1) = 0 THEN
        nextlevelfound& = 1
        channels(chnr&).actions(i&).turn = channels(chnr&).actions(i&).turn+channels(chnr&).info.turn
        IF channels(chnr&).actions(i&).category = %ACCAT_VICTORY THEN
          tp& = channels(chnr&).actions(i&).actiontype
          SELECT CASE tp&
          CASE %ACTYPE_VC_PLAYERDEFEATED: vc& = 255+tp&*256+channels(chnr&).actions(i&).actionparam*65536
          CASE %ACTYPE_VC_ALLENEMIES_DEFEATED: vc& = 255+tp&*256+channels(chnr&).actions(i&).player*65536
          CASE %ACTYPE_VC_SHOPOCCUPIED: vc& = 255+tp&*256+channels(chnr&).actions(i&).shop*65536
          CASE %ACTYPE_VC_UNITDEAD_PLAYER1 TO %ACTYPE_VC_UNITDEAD_PLAYER6: vc& = 255+tp&*256+channels(chnr&).actions(vc&).shop*65536+channels(chnr&).actions(vc&).actionparam*16777216
          CASE ELSE: vc& = 0
          END SELECT
          IF INSTR(differentvictoryconditions$, MKL$(vc&)) = 0 THEN differentvictoryconditions$ = differentvictoryconditions$+MKL$(vc&)
        END IF
      ELSE
      END IF
    END IF
  NEXT i&
'CALL BILog("CheckNextActionLevel: "+FORMAT$(deleted&)+" actions deleted, remaining "+FORMAT$(channels(chnr&).info.nactions), 0)

  IF nextlevelfound& = 0 THEN EXIT FUNCTION

  'nächstes Level aktivieren
  actions$ = PEEK$(VARPTR(channels(chnr&).actions(0)), channels(chnr&).info.nactions*SIZEOF(TAction))
  actions$ = SortActions$(chnr&, actions$)
  POKE$ VARPTR(channels(chnr&).actions(0)), actions$
  IF channels(chnr&).info.turnlimit >= 0 THEN channels(chnr&).info.turnlimit = channels(chnr&).info.turnlimit+channels(chnr&).info.turn
  channels(chnr&).info.requiredvictorycond = LEN(differentvictoryconditions$)/4
  CALL RefuelAllUnits(chnr&)
  CALL PreRenderMissionText(chnr&)
  IF gameMode& <> %GAMEMODE_SERVER THEN CALL MusicButtonPressed(0)
'CALL BILog(FORMAT$(channels(chnr&).info.nactions)+" actions, "+FORMAT$(channels(chnr&).info.requiredvictorycond)+" victory conds, turn limit "+FORMAT$(channels(chnr&).info.turnlimit)+", start pos: "+FORMAT$(channels(chnr&).info.actionposition), 0)

  CheckNextActionLevel& = 1
END FUNCTION



'Löscht eine Aktion
SUB DeleteAction(chnr&, actionNumber&)
  LOCAL i&

  IF actionNumber& > channels(chnr&).info.nactions-1 OR channels(chnr&).info.nactions = 0 THEN EXIT SUB

  FOR i& = actionNumber& TO channels(chnr&).info.nactions-2
    channels(chnr&).actions(i&) = channels(chnr&).actions(i&+1)
  NEXT i&
  channels(chnr&).info.nactions = channels(chnr&).info.nactions-1
END SUB



'Standard-Siegbedingungen erzeugen
SUB CreateDefaultWinConditions(chnr&)
  LOCAL plnr&, enemy&, team&, vc$, actions$
  LOCAL cond AS TAction

  actions$ = PEEK$(VARPTR(channels(chnr&).actions(0)), channels(chnr&).info.nactions*SIZEOF(TAction))
  cond.category = %ACCAT_VICTORY
  cond.actiontype = %ACTYPE_VC_ALLENEMIES_DEFEATED

  'Bedingungen für Gegner besiegen
  FOR plnr& = 0 TO %MAXPLAYERS
    IF (channels(chnr&).info.aliveplayers AND 2^plnr&) <> 0 THEN
      cond.player = plnr&
      vc$ = vc$+PEEK$(VARPTR(cond), SIZEOF(cond))+actions$
    END IF
  NEXT plnr&

  'Siegbedingungen zu Aktionen hinzufügen
  actions$ = vc$+actions$
  channels(chnr&).info.nactions = LEN(actions$)/SIZEOF(TAction)
  POKE$ VARPTR(channels(chnr&).actions(0)), actions$
  channels(chnr&).info.nvictoryconditions = LEN(vc$)/SIZEOF(TAction)
  channels(chnr&).info.actionposition = LEN(vc$)/SIZEOF(TAction)
  channels(chnr&).info.requiredvictorycond = 1
END SUB



'Prüft ob eine Nachricht eine Video-Nachricht ist
FUNCTION IsVideoMessage&(chnr&, BYVAL msgid&)
  IsVideoMessage& = channels(chnr&).campaign.episode >= 5 AND channels(chnr&).campaign.episode <= 9 AND msgid& >= 100 AND (msgid& < %ANI_BUILDINGLOST0 OR msgid& > %ANI_FEMALE)
END FUNCTION



'Liefert den Text einer Spielnachricht zu einer Nachrichten-ID
FUNCTION GetGameMessageText(textid&, replaceNonPrintableChar&, replacePlaceholders&) AS WSTRING
  LOCAL a$$

  IF textid& > UBOUND(gameMessages$$) THEN EXIT FUNCTION
  a$$ = gameMessages$$(textid&)
  IF replacePlaceholders& <> 0 THEN a$$ = ReplaceMessagePlaceholders(a$$)
  IF replaceNonPrintableChar& <> 0 THEN a$$ = ReplaceNonPrintable(a$$, IIF&(replaceNonPrintableChar& = 2, 1, 0))  'replaceNonPrintableChar: 0 = keine Ersetzung , 1 = normale Ersetzung , 2 = für Sprachausgabe

  GetGameMessageText = a$$
END FUNCTION



'Berechnet den Klemm-Faktor im Kampf
FUNCTION CalculateSurroundingFactor&(chnr&, attacker&, defender&)
  LOCAL i&, x&, y&, plnr&, enemyunit&, attackerpos&, factor&, s$
  LOCAL xoff&(), yoff&()

  'alle an den Verteidiger angrenzenden Felder prüfen
  x& = channels(chnr&).units(defender&).xpos
  y& = channels(chnr&).units(defender&).ypos
  plnr& = channels(chnr&).units(attacker&).owner
  CALL GetAdjacentFields(chnr&, x&, y&, xoff&(), yoff&())
  FOR i& = 0 TO 5
    IF xoff&(i&) >= 0 THEN
      enemyunit& = channels(chnr&).zone3(xoff&(i&), yoff&(i&))
      IF enemyunit& >= 0 AND channels(chnr&).units(enemyunit&).owner = plnr& AND CanAttackField&(chnr&, enemyunit&, x&, y&) <> 0 THEN
        IF enemyunit& = attacker& THEN attackerpos& = i& ELSE s$ = s$+CHR$(i&)
      END IF
    END IF
  NEXT i&

  'Position der gefundenen Einheiten relativ zum Angreifer bewerten
  FOR i& = 1 TO LEN(s$)
    x& = ASC(s$, i&)-attackerpos&
    IF x& < -3 THEN x& = x&+6
    IF x& > 3 THEN x& = x&-6
    x& = ABS(x&)
    factor& = factor&+ASC(CHR$(10,25,45), x&)*8
  NEXT i&

  CalculateSurroundingFactor& = factor&
END FUNCTION



'Ermittelt die Koordinaten des Felds hinter einem Zielfeld von einem angrenzenden Quellfeld aus gesehen
SUB GetFieldBehindField(BYVAL chnr&, BYVAL xtarget&, BYVAL ytarget&, BYVAL xsrc&, BYVAL ysrc&, BYREF xbehind&, BYREF ybehind&)
  LOCAL i&, srcpos&
  LOCAL xoff&(), yoff&()

  'ermitteln, an welcher Stelle sich das Quellfeld neben dem Zielfeld befindet
  xbehind& = -1
  srcpos& = -1
  CALL GetAdjacentFields(chnr&, xtarget&, ytarget&, xoff&(), yoff&())
  FOR i& = 0 TO 5
    IF xoff&(i&) >= xsrc& AND yoff&(i&) = ysrc& THEN srcpos& = i&
  NEXT i&
  IF srcpos& = -1 THEN EXIT SUB

  'gegenüberliegendes Feld zurückliefern
  srcpos& = srcpos&+3
  IF srcpos& > 5 THEN srcpos& = srcpos&-6
  xbehind& = xoff&(srcpos&)
  ybehind& = yoff&(srcpos&)
END SUB



'Prüft, ob eine Einheit ein Gebäude betreten kann
FUNCTION UnitCanEnterBuilding&(chnr&, unitnr&, shopnr&)
  LOCAL unitowner&, unittp&, shopowner&, shoptype&, freeshopslots&, requiredshopslots&

  unitowner& = channels(chnr&).units(unitnr&).owner
  unittp& = channels(chnr&).units(unitnr&).unittype
  shopowner& = channels(chnr&).shops(shopnr&).owner
  shoptype& = channels(chnr&).shops(shopnr&).shoptype
  freeshopslots& = CountFreeShopSlots&(chnr&, shopnr&)
  requiredshopslots& = IIF&((channelsnosave(chnr&).unitclasses(channels(chnr&).units(unitnr&).unittype).flags AND %UCF_TRANSPORTER) <> 0, 9-CountFreeTransportSlots&(chnr&, unitnr&), 1)

  'Einheiten, die Gebäude einnehmen können, können fremde Gebäude immer betreten und eigene sofern Platz ist
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_OCCUPY) <> 0 AND (unitowner& <> shopowner& OR freeshopslots& >= requiredshopslots&) THEN
    UnitCanEnterBuilding& = -1
    EXIT FUNCTION
  END IF

  'alle anderen Einheiten können nur eigene Gebäude betreten in denen Platz ist und die den richtigen Typ haben
  IF unitowner& <> shopowner& OR freeshopslots& < requiredshopslots& THEN EXIT FUNCTION
  IF unittp& = channelsnosave(chnr&).crystalClass THEN
    UnitCanEnterBuilding& = -1
  ELSE
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN
      UnitCanEnterBuilding& = (shoptype& = %SHOPTYPE_HARBOUR OR shoptype& = %SHOPTYPE_HQ)
    ELSE
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN
        UnitCanEnterBuilding& = (shoptype& = %SHOPTYPE_AIRPORT OR shoptype& = %SHOPTYPE_HQ)
        IF (unittp& = 35 OR unittp& = 41) AND shoptype& = %SHOPTYPE_CITY THEN UnitCanEnterBuilding& = -1  'Helicopter können Städte betreten
      ELSE
        UnitCanEnterBuilding& = (shoptype& <> %SHOPTYPE_AIRPORT AND shoptype& <> %SHOPTYPE_HARBOUR)
      END IF
    END IF
  END IF
END FUNCTION



'Erzeugt eine neue Einheit auf dem Spielfeld
FUNCTION CreateUnitFromCode&(BYVAL chnr&, BYVAL x&, BYVAL y&, BYVAL unitcode&)
  LOCAL unitnr&, unittp&, owner&

  unittp& = (unitcode& AND 255)
  owner& = GetOwnerFromCode&(unitcode&)
  CALL BIDebugLog("Creating unit "+FORMAT$(channels(chnr&).info.nunits)+" from code: "+FORMAT$(unittp&))

  'Besitzer von Aldinium ist immer der neutrale Spieler
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_CRYSTAL) <> 0 THEN owner& = 6

  unitnr& = CreateUnit&(chnr&, x&, y&, unittp&, owner&, 0)
'CALL BILog("CreateUnitFromCode: "+FORMAT$(unittp&)+" -> "+FORMAT$(unitnr&), 0)

  CreateUnitFromCode& = unitnr&
END FUNCTION



'Erzeugt eine neue Einheit auf dem Spielfeld
FUNCTION CreateUnit&(BYVAL chnr&, BYVAL x&, BYVAL y&, BYVAL unittp&, BYVAL owner&, BYVAL dummy&)
  LOCAL unitnr&, d&, z&, i&
  LOCAL u AS TUnit

  'freie Einheitennummer suchen
  IF dummy& = 0 THEN
    FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
      IF UnitIsAlive&(chnr&, unitnr&) = 0 THEN EXIT FOR
    NEXT unitnr&
    IF unitnr& = channels(chnr&).info.nunits OR channels(chnr&).info.nunits = 0 THEN
      'prüfen, ob Einheitenlimit erreicht wurde
      IF channels(chnr&).info.nunits = %MAXUNITS THEN
        CreateUnit& = -1
        EXIT FUNCTION
      END IF
      channels(chnr&).info.nunits = unitnr&+1
    END IF
  ELSE
    'Einheit am Ende des Arrays erzeugen (außerhalb der Sichtbarkeit aller Logiken)
    unitnr& = %MAXUNITS
  END IF

  'Einheit so ausrichten, daß sie zur Kartenmitte zeigt
  d& = GetAngle&(x&, y&, channels(chnr&).info.xsize/2, channels(chnr&).info.ysize/2)

  'Einheit erzeugen
  u.unittype = unittp&
  u.owner = owner&
  u.groupsize = channelsnosave(chnr&).unitclasses(unittp&).groupsize
  u.fuel = channelsnosave(chnr&).unitclasses(unittp&).fuel
  u.ammo(0) = channelsnosave(chnr&).unitclasses(unittp&).weapons(0).ammo
  u.ammo(1) = channelsnosave(chnr&).unitclasses(unittp&).weapons(1).ammo
  u.ammo(2) = channelsnosave(chnr&).unitclasses(unittp&).weapons(2).ammo
  u.ammo(3) = channelsnosave(chnr&).unitclasses(unittp&).weapons(3).ammo
  u.transportvolume = 0
  u.experience = IIF&(channels(chnr&).info.difficulty = %DIFFICULTY_EASY AND (channels(chnr&).info.aimask AND 2^owner&) = 0, 2, 1)
  u.flags = 0
  u.xpos = x&
  u.ypos = y&
  u.xposold = x&
  u.yposold = y&
  z& = %UZ_GROUND
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN z& = %UZ_AIR
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN z& = %UZ_WATER
  u.zpos = z&
  u.direction = INT((d&+30)/60) MOD 6
  FOR i& = 0 TO 7
    u.transportcontent(i&) = -1
  NEXT i&

  channels(chnr&).units(unitnr&) = u

  CreateUnit& = unitnr&
END FUNCTION



'Einheit im Shop produzieren
FUNCTION BuildUnit&(chnr&, shopnr&, unittp&)
  LOCAL slotnr&, owner&, unitnr&, costenergy&, costmat&

  gamedataChanged& = 1
  owner& = channels(chnr&).shops(shopnr&).owner
  slotnr& = GetFreeShopSlot&(chnr&, shopnr&)
  IF slotnr& < 0 THEN
    BuildUnit& = -1
    EXIT FUNCTION
  END IF

  'Einheit erzeugen
  unitnr& = CreateUnit&(chnr&, channels(chnr&).shops(shopnr&).position, channels(chnr&).shops(shopnr&).position2, unittp&, owner&, 0)
  channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags OR %US_DONE
  channels(chnr&).shops(shopnr&).content(slotnr&) = unitnr&
  costenergy& = channelsnosave(chnr&).unitclasses(unittp&).costenergy
  costmat& = channelsnosave(chnr&).unitclasses(unittp&).costmaterial
  channels(chnr&).player(owner&).energy = channels(chnr&).player(owner&).energy-costenergy&
  channels(chnr&).shops(shopnr&).material = channels(chnr&).shops(shopnr&).material-costmat&
  channels(chnr&).player(owner&).producedunits = channels(chnr&).player(owner&).producedunits+1

  'Replay aufzeichnen
  CALL AddReplay(chnr&, %REPLAY_SHOPACTION, owner&, MKL$(shopnr&)+MKL$(%SHOPACTION_BUILD)+MKL$(unittp&)+PEEK$(VARPTR(channels(chnr&).shops(shopnr&)), SIZEOF(TShop)))

  IF LocalPlayersTurn& THEN CALL UpdateProgressbar
  CALL UpdateVisionAll(chnr&)

  BuildUnit& = unitnr&
END FUNCTION



'Ermittelt das Produktionsmenü eines Shops
FUNCTION GetShopProductionMenu$(chnr&, shopnr&, ai&, onlyproducible&)
  LOCAL i&, d&&, p&, tp&, owner&, energy&, material&, avail&, prod$

  'prüfen, ob Shop Fabrik, Hafen oder Flughafen ist
  tp& = channels(chnr&).shops(shopnr&).shoptype
  IF tp& <> %SHOPTYPE_AIRPORT AND tp& <> %SHOPTYPE_HARBOUR AND tp& <> %SHOPTYPE_FACTORY THEN EXIT FUNCTION

  'verfügbare Energie/Material ermitteln
  owner& = channels(chnr&).shops(shopnr&).owner
  material& = channels(chnr&).shops(shopnr&).material
  energy& = channels(chnr&).player(owner&).energy

  'im leichten und normalen Schwierigkeitsgrad soll die KI nur Teile ihrer Resourcen nutzen
  IF ai& = 1 THEN
    SELECT CASE channels(chnr&).info.difficulty
    CASE %DIFFICULTY_EASY:
      material& = INT(material&*0.5)
      energy& = INT(energy&*0.5)
    CASE %DIFFICULTY_NORMAL:
      material& = INT(material&*0.75)
      energy& = INT(energy&*0.75)
    END SELECT
  END IF

  'Shop-Menü
  FOR i& = 0 TO 3
    p& = channels(chnr&).shops(shopnr&).production(i&)
    IF p& > 0 AND INSTR(prod$, CHR$(p&)) = 0 THEN
      IF onlyproducible& = 0 OR (channelsnosave(chnr&).unitclasses(p&).costenergy <= energy& AND channelsnosave(chnr&).unitclasses(p&).costmaterial <= material&) THEN prod$ = prod$+CHR$(p&)
    END IF
  NEXT i&

  'allgemeine Produktionspalette
  d&& = 1
  FOR i& = 0 TO 62
    avail& = 0
    IF ai& = 0 AND (channels(chnr&).info.playergpm AND d&&) <> 0 THEN avail& = 1
    IF ai& = 1 AND (channels(chnr&).info.aigpm AND d&&) <> 0 THEN avail& = 1
    IF avail& = 1 AND onlyproducible& = 1 AND (channelsnosave(chnr&).unitclasses(i&).costenergy > energy& OR channelsnosave(chnr&).unitclasses(i&).costmaterial > material&) THEN avail& = 0
    IF (channelsnosave(chnr&).unitclasses(i&).shopmask AND tp&) = 0 THEN avail& = 0
    IF avail& = 1 AND INSTR(prod$, CHR$(i&)) = 0 THEN prod$ = prod$+CHR$(i&)
    IF LEN(prod$) = 16 THEN EXIT FOR
    d&& = d&&+d&&
  NEXT i&

  GetShopProductionMenu$ = prod$
END FUNCTION



'Ermittelt in welcher Richtung ein Feld zu einem andere liegt
FUNCTION GetDirectionToField&(srcx&, srcy&, destx&, desty&)
  LOCAL r&

  IF srcx& = destx& THEN
    'beide Felder befinden sich auf derselben Vertikalen
    r& = IIF&(srcy& > desty&, 0, 3)
  ELSE
    IF srcx& < destx& THEN
      'Quellfeld ist links von Zielfeld
      IF srcy& > desty& THEN
        r& = 1
      ELSE
        IF srcy& < desty& THEN
          r& = 2
        ELSE
          r& = IIF&((srcx& AND 1) = 0, 2, 1)
        END IF
      END IF
    ELSE
      'Quellfeld ist rechts von Zielfeld
      IF srcy& > desty& THEN
        r& = 5
      ELSE
        IF srcy& < desty& THEN
          r& = 4
        ELSE
          r& = IIF&((srcx& AND 1) = 0, 4, 5)
        END IF
      END IF
    END IF
  END IF

  GetDirectionToField& = r&
END FUNCTION



'Berechnet den Abstand zwischen 2 Feldern
FUNCTION GetDistance&(BYVAL srcx&, BYVAL srcy&, BYVAL destx&, BYVAL desty&)
  LOCAL d&

  WHILE srcx& <> destx& OR srcy& <> desty&
    'ein Feld weit Richtung Zielfeld bewegen
    d& = d&+1
    'neue Position berechnen
    IF srcx& = destx& THEN
      'beide Felder befinden sich auf derselben Vertikalen
      IF srcy& < desty& THEN srcy& = srcy&+1 ELSE srcy& = srcy&-1
    ELSE
      IF srcx& < destx& THEN
        'Quellfeld ist links von Zielfeld
        IF (srcx& AND 1) = 0 AND srcy& > desty& THEN srcy& = srcy&-1
        IF (srcx& AND 1) = 1 AND srcy& < desty& THEN srcy& = srcy&+1
        srcx& = srcx&+1
      ELSE
        'Quellfeld ist rechts von Zielfeld
        IF (srcx& AND 1) = 0 AND srcy& > desty& THEN srcy& = srcy&-1
        IF (srcx& AND 1) = 1 AND srcy& < desty& THEN srcy& = srcy&+1
        srcx& = srcx&-1
      END IF
    END IF
  WEND

  GetDistance& = d&
END FUNCTION



'Prüft ob Terrain betretbar ist
FUNCTION CanEnterTerrain&(chnr&, x&, y&, mask&)
  LOCAL terrain&, overlay&, terrainmask&, overlaymask&, m&

  'Terrain/Overlay-Eigenschaften auslesen
  terrain& = channels(chnr&).zone1(x&, y&)
  terrainmask& = channelsnosave(chnr&).terraindef(terrain&).typemask
  overlay& = channels(chnr&).zone2(x&, y&)
  IF overlay& >= 0 THEN overlaymask& = channelsnosave(chnr&).terraindef(overlay&).typemask

  'falls Terrain Wasser ist und Einheit Wasser betreten kann, Overlay ignorieren
  IF overlay& >= 0 AND (terrainmask& AND %TERRAIN_WATER AND mask&) <> 0 AND (overlaymask& AND %TERRAIN_BUILDING) = 0 THEN
    CanEnterTerrain& = -1
    EXIT FUNCTION
  END IF

'  m& = %TERRAIN_ROAD OR %TERRAIN_PATH OR %TERRAIN_TRENCH OR %TERRAIN_RAIL OR %TERRAIN_FOREST
  IF overlay& >= 0 THEN
    CanEnterTerrain& = (overlaymask& AND mask&) <> 0 AND (overlaymask& AND %TERRAIN_BUILDING) = 0
    EXIT FUNCTION
'    IF (overlaymask& AND m&) <> 0 THEN
     'Feld hat Straße/Weg/Graben/Schiene/Wald -> nur Overlay prüfen
'      CanEnterTerrain& = (overlaymask& AND m& AND mask&) <> 0
'      EXIT FUNCTION
'    END IF
    'falls Overlay nicht betreten werden kann, dann kann das Feld nicht betreten werden
'    IF (overlaymask& AND mask&) = 0 THEN EXIT FUNCTION
  END IF

  'Feld kann betreten werden falls Terrain betreten werden kann
  CanEnterTerrain& = (terrainmask& AND mask&) <> 0
END FUNCTION



'Befüllt die "neighbor" Arrays mit den Koordinaten der benachbarten Felder (im Uhrzeigersinn beginnened im Norden)
SUB GetAdjacentFields(BYVAL chnr&, BYVAL x&, BYVAL y&, xoff&(), yoff&())
  LOCAL i&
  REDIM xoff&(6), yoff&(6)

  xoff&(0) = x&
  yoff&(0) = y&-1
  xoff&(3) = x&
  yoff&(3) = y&+1
  IF (x& AND 1) = 0 THEN
    xoff&(1) = x&+1
    yoff&(1) = y&-1
    xoff&(2) = x&+1
    yoff&(2) = y&
    xoff&(4) = x&-1
    yoff&(4) = y&
    xoff&(5) = x&-1
    yoff&(5) = y&-1
  ELSE
    xoff&(1) = x&+1
    yoff&(1) = y&
    xoff&(2) = x&+1
    yoff&(2) = y&+1
    xoff&(4) = x&-1
    yoff&(4) = y&+1
    xoff&(5) = x&-1
    yoff&(5) = y&
  END IF

  'Koordinaten außerhalb der Karte auf -1 setzen
  FOR i& = 0 TO 5
    IF xoff&(i&) < 0 OR xoff&(i&) >= channels(chnr&).info.xsize OR yoff&(i&) < 0 OR yoff&(i&) >= channels(chnr&).info.ysize THEN xoff&(i&) = -1
  NEXT i&
END SUB



'Ermittelt, wieviele unerforschte Felder an ein Feld angrenzen
FUNCTION CountAdjacentUnexploredFields&(BYVAL chnr&, BYVAL plnr&, BYVAL x&, BYVAL y&)
  LOCAL i&, n&, plmask&
  LOCAL xoff&(), yoff&()

  'alle angrenzenden Felder überprüfen
  CALL GetAdjacentFields(chnr&, x&, y&, xoff&(), yoff&())
  plmask& = 2^plnr&
  FOR i& = 0 TO 5
    IF xoff&(i&) >= 0 THEN
      IF (channels(chnr&).explored(xoff&(i&), yoff&(i&)) AND plmask&) = 0 THEN n& = n&+1
    END IF
  NEXT i&

  CountAdjacentUnexploredFields& = n&
END FUNCTION



'Prüft, ob eine Einheit direkt neben einer Einheit eines Spielers steht
FUNCTION IsAdjacentToUnitOfPlayer&(BYVAL chnr&, BYVAL unitnr&, BYVAL plnr&)
  LOCAL i&, owner&, unr&
  LOCAL xoff&(), yoff&()

  'alle angrenzenden Felder überprüfen
  CALL GetAdjacentFields(chnr&, channels(chnr&).units(unitnr&).xpos, channels(chnr&).units(unitnr&).ypos, xoff&(), yoff&())
  FOR i& = 0 TO 5
    IF xoff&(i&) >= 0 THEN
      unr& = channels(chnr&).zone3(xoff&(i&), yoff&(i&))
      IF unr& >= 0 THEN
        owner& = channels(chnr&).units(unr&).owner
        IF (channels(chnr&).player(owner&).allymask AND 2^plnr&) <> 0 THEN
          'angrenzende Einheit gehört dem Spieler (oder einem seiner Verbündeten)
          IsAdjacentToUnitOfPlayer& = -1
          EXIT FUNCTION
        END IF
      END IF
    END IF
  NEXT i&

  IsAdjacentToUnitOfPlayer& = 0
END FUNCTION



'Ermittelt alle Felder in einem bestimmten Abstand um ein Startfeld herum (ohne Berücksichtigung von Feldeigenschaften etc.)
SUB GetHexRange(BYVAL chnr&, BYVAL startx&, BYVAL starty&, BYVAL range&, reachablearea?(), r AS RECT)
  LOCAL x&, y&, n&, t&

  'maximal relevanten Bereich der Karte ermitteln
  r.left = MAX&(0, startx&-range&)
  r.top = MAX&(0, starty&-range&)
  r.right = MIN&(channels(chnr&).info.xsize-1, startx&+range&)
  r.bottom = MIN&(channels(chnr&).info.ysize-1, starty&+range&)
  REDIM reachablearea?(r.right-r.left, r.bottom-r.top)

  'Felder im Schnellverfahren markieren
  n& = range&+1  'Höhe des Hexagons am linken Rand
  FOR x& = startx&-range& TO startx&+range&
    IF x& >= r.left AND x& <= r.right THEN
      t& = starty&-INT(n&/2)
      IF (startx& AND 1) = 1 AND (x& AND 1) = 0 THEN t& = t&+1
      FOR y& = t& TO t&+n&-1
        IF y& >= r.top AND y& <= r.bottom THEN reachablearea?(x&-r.left, y&-r.top) = 1
      NEXT y&
    END IF
    IF x& < startx& THEN n& = n&+1 ELSE n& = n&-1
  NEXT x&
END SUB



'Bewegungskosten für eine Einheit auf einem bestimmten Terrain ermitteln
FUNCTION TerrainMovementCost&(BYVAL chnr&, BYVAL unittp&, BYVAL terraintype&)
  LOCAL nocostsmask&, cost&

  'Kosten gelten nur für Landfahrzeuge
  nocostsmask& = %UCF_SHIP OR %UCF_PLANE OR %UCF_RAIL
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND nocostsmask&) <> 0 THEN EXIT FUNCTION

  'feste Kosten für Straße/Weg/Schiene/Graben
  IF (terraintype& AND %TERRAIN_ROAD) <> 0 THEN TerrainMovementCost& = 0 : EXIT FUNCTION
  IF (terraintype& AND %TERRAIN_PATH) <> 0 THEN TerrainMovementCost& = 2 : EXIT FUNCTION
  IF (terraintype& AND %TERRAIN_TRENCH) <> 0 THEN TerrainMovementCost& = 8 : EXIT FUNCTION
  IF (terraintype& AND %TERRAIN_RAIL) <> 0 THEN TerrainMovementCost& = 4 : EXIT FUNCTION

  'Kosten berechnen
  cost& = 2
  IF (terraintype& AND %TERRAIN_FOREST) <> 0 THEN cost& = cost&+6
  IF (terraintype& AND %TERRAIN_MOUTAIN_UNPASSABLE) <> 0 THEN cost& = cost&+18
  IF (terraintype& AND %TERRAIN_MOUNTAIN) <> 0 THEN cost& = cost&+12
  IF (terraintype& AND %TERRAIN_SLOPE) <> 0 THEN cost& = cost&+10
  IF (terraintype& AND %TERRAIN_OBSTACLE) <> 0 THEN cost& = cost&+4

  TerrainMovementCost& = cost&
END FUNCTION



'Ermittelt alle Felder in Reichweite um ein Startfeld herum
'md& = 0 : Sicht
'md& = 1 : Bewegung
'md& = 2 : Angriff
'range& wird in Achtelfeldern angegeben
SUB GetReachableFields(BYVAL chnr&, BYVAL plnr&, BYVAL unitnr&, BYVAL startx&, BYVAL starty&, BYVAL stopx&, BYVAL stopy&, BYVAL range&, BYVAL mask&, BYVAL md&, reachablearea?(), r AS RECT)
  LOCAL qread&, qwrite&, plmask&, x&, y&, v&, i&, j&, rangenext&, sprnr&, terraintype&, unr&, enemycount&, unittp&, cl&, sbmask&, checkterrain&
  LOCAL xoff&(), yoff&(), xenemies&(), yenemies&()
  LOCAL queue() AS TZoneSearch
  DIM queue(4095)

  plmask& = 2^plnr&
  unittp& = channels(chnr&).units(unitnr&).unittype
  sbmask& = (NOT channels(chnr&).player(plnr&).allymask) AND 255
  range& = MIN&(250, range&)

  'maximal relevanten Bereich der Karte ermitteln
  r.left = MAX&(0, startx&-range&/8)
  r.top = MAX&(0, starty&-range&/8)
  r.right = MIN&(channels(chnr&).info.xsize-1, startx&+range&/8)
  r.bottom = MIN&(channels(chnr&).info.ysize-1, starty&+range&/8)
  REDIM reachablearea?(r.right-r.left, r.bottom-r.top)

  'erreichbare Felder über Breitensuche ermitteln
  queue(0).x = startx&
  queue(0).y = starty&
  queue(0).v = range&+1
  qwrite& = 1
  reachablearea?(startx&-r.left, starty&-r.top) = range&+1
  WHILE qwrite& < 4096 AND qread& < qwrite&
    x& = queue(qread&).x
    y& = queue(qread&).y
    v& = queue(qread&).v
    qread& = qread&+1
    'falls Reichweite >= 1.0 dann alle angrenzenden Felder prüfen
    IF v& >= 8 THEN
      CALL GetAdjacentFields(chnr&, x&, y&, xoff&(), yoff&())
      FOR i& = 0 TO 5
        IF xoff&(i&) >= 0 THEN
          rangenext& = v&-8
          'Modifikation durch Terrain prüfen
          sprnr& = channels(chnr&).zone1(xoff&(i&), yoff&(i&))
          terraintype& = channelsnosave(chnr&).terraindef(sprnr&).typemask
          sprnr& = channels(chnr&).zone2(xoff&(i&), yoff&(i&))
          IF sprnr& >= 0 THEN terraintype& = terraintype& OR channelsnosave(chnr&).terraindef(sprnr&).typemask
          SELECT CASE md&
          CASE 0:  'Sichtbarkeit
            IF (terraintype& AND %TERRAIN_MOUTAIN_UNPASSABLE) <> 0 THEN rangenext& = rangenext&-6
            IF (terraintype& AND %TERRAIN_FOREST) <> 0 THEN rangenext& = rangenext&-1
            IF (terraintype& AND %TERRAIN_BUILDING) <> 0 THEN rangenext& = rangenext&-6
            IF (terraintype& AND %TERRAIN_BUILDING2) <> 0 THEN rangenext& = rangenext&-6
            IF (channels(chnr&).sightblock(xoff&(i&), yoff&(i&)) AND sbmask&) <> 0 THEN rangenext& = rangenext&-2
            IF (channels(chnr&).sightblock(xoff&(i&), yoff&(i&)) AND sbmask&*256) <> 0 THEN rangenext& = rangenext&-4

          CASE 1:  'Bewegung
            unr& = channels(chnr&).zone3(xoff&(i&), yoff&(i&))
            IF (channels(chnr&).explored(xoff&(i&), yoff&(i&)) AND plmask&) = 0 THEN rangenext& = 0  'nur erforschte Felder können betreten werden
            'Terrain nur prüfen, falls sich dort kein betretbarer Shop oder betretbarer Transporter befindet
            checkterrain& = 1
            IF unr& < -1 AND UnitCanEnterBuilding&(chnr&, unitnr&, -2-unr&) <> 0 THEN checkterrain& = 0
            IF unr& >= 0 AND channels(chnr&).units(unr&).owner = plnr& AND (channelsnosave(chnr&).unitclasses(channels(chnr&).units(unr&).unittype).flags AND %UCF_TRANSPORTER) <> 0 AND TransporterCanLoadUnit&(chnr&, unr&, unitnr&) <> 0 THEN
              IF (channels(chnr&).units(unr&).flags AND (%US_ASCEND OR %US_DIVE)) = 0 THEN checkterrain& = 0
            END IF
            IF checkterrain& = 1 AND CanEnterTerrain&(chnr&, xoff&(i&), yoff&(i&), mask&) = 0 THEN
              rangenext& = 0  'Feld ist für diese Einheit nicht betretbar
            ELSE
              rangenext& = rangenext&-TerrainMovementCost&(chnr&, channels(chnr&).units(unitnr&).unittype, terraintype&)
            END IF
            IF unr& > -1 THEN
              IF channels(chnr&).units(unr&).owner = 6 THEN
                'auf dem Feld steht eine neutrale Einheit (sollte nur Aldinium sein)
              ELSE
                IF IsEnemyUnit&(chnr&, plnr&, unr&) THEN
                  'auf dem Feld steht eine feindliche Einheit (Mine und Aldinium kann von Transportern eingesammelt werden, alles andere blockiert den Weg)
                  IF (channelsnosave(chnr&).unitclasses(channels(chnr&).units(unr&).unittype).flags AND (%UCF_BADDEFENDER OR %UCF_CRYSTAL)) = 0 OR TransporterCanLoadUnit&(chnr&, unitnr&, unr&) = 0 THEN rangenext& = 0
                END IF
              END IF
            END IF
            'ab 1 angrenzenden feindlichen Einheit wird die Reichweite reduziert
            IF rangenext& > 0 THEN
              enemycount& = 0
              CALL GetAdjacentFields(chnr&, xoff&(i&), yoff&(i&), xenemies&(), yenemies&())
              FOR j& = 0 TO 5
                IF xenemies&(j&) >= 0 THEN
                  unr& = channels(chnr&).zone3(xenemies&(j&), yenemies&(j&))
                  IF unr& >= 0 AND IsEnemyUnit&(chnr&, plnr&, unr&) THEN
                    enemycount& = enemycount&+1
                    IF enemycount& > 0 THEN
                      rangenext& = rangenext&-8  'geblockt durch eine feindliche Einheit
                      EXIT FOR
                    END IF
                  END IF
                END IF
              NEXT j&
            END IF

          CASE 2:  'Angriff
            IF (channels(chnr&).explored(xoff&(i&), yoff&(i&)) AND plmask&) = 0 OR (channels(chnr&).vision(xoff&(i&), yoff&(i&)) AND plmask&) = 0 THEN rangenext& = 0  'nur aufgedeckte Felder können beschossen werden

          END SELECT
          IF rangenext& > reachablearea?(xoff&(i&)-r.left, yoff&(i&)-r.top) THEN
            'dieses Feld markieren und Suche später hier fortsetzen
            queue(qwrite&).x = xoff&(i&)
            queue(qwrite&).y = yoff&(i&)
            queue(qwrite&).v = rangenext&
            qwrite& = qwrite&+1
            reachablearea?(xoff&(i&)-r.left, yoff&(i&)-r.top) = rangenext&
            'prüfen, ob Zielfeld erreicht wurde
            IF xoff&(i&) = stopx& AND yoff&(i&) = stopy& THEN EXIT SUB
          ELSE
            IF md& = 0 AND qread& = 1 THEN reachablearea?(xoff&(i&)-r.left, yoff&(i&)-r.top) = 1  'alle direkt an die Einheit/Shop angrendenden Felder sind immer sichtbar
          END IF
        END IF
      NEXT i&
    END IF
  WEND
END SUB



'Aktualisiert die überwachten Felder eines Spielers
SUB UpdateVision(chnr&, plnr&)
  LOCAL x&, y&, i&, j&, unitnr&, unittp&, shopnr&, owner&, sight&, plmask&
  LOCAL reachablearea?(), r AS RECT

  'alte Sichtbarkeit entfernen
  plmask& = 255-2^plnr&
  FOR y& = 0 TO channels(chnr&).info.ysize-1
    FOR x& = 0 TO channels(chnr&).info.xsize-1
      channels(chnr&).vision(x&, y&) = channels(chnr&).vision(x&, y&) AND plmask&
    NEXT x&
  NEXT y&

  'alle Einheiten des Spielers und seiner Verbündeten suchen
  plmask& = 2^plnr&
  FOR y& = 0 TO channels(chnr&).info.ysize-1
    FOR x& = 0 TO channels(chnr&).info.xsize-1
      unitnr& = channels(chnr&).zone3(x&, y&)
      IF unitnr& >= 0 THEN
        owner& = channels(chnr&).units(unitnr&).owner
        IF owner& < 7 AND (channels(chnr&).player(plnr&).allymask AND 2^owner&) <> 0 THEN
          'sichtbare Felder für diese Einheit ermitteln
          unittp& = channels(chnr&).units(unitnr&).unittype
          sight& = channelsnosave(chnr&).unitclasses(unittp&).sight
          CALL GetReachableFields(chnr&, plnr&, unitnr&, x&, y&, -1, -1, sight&, 0, 0, reachablearea?(), r)
          'sichtbare Felder markieren
          FOR j& = r.top TO r.bottom
            FOR i& = r.left TO r.right
              IF reachablearea?(i&-r.left, j&-r.top) > 0 THEN
                channels(chnr&).vision(i&, j&) = channels(chnr&).vision(i&, j&) OR plmask&
                channels(chnr&).explored(i&, j&) = channels(chnr&).explored(i&, j&) OR plmask&
              END IF
            NEXT i&
          NEXT j&
        END IF
      END IF
    NEXT x&
  NEXT y&

  'alle Shops des Spielers und seiner Verbündeten suchen
  FOR shopnr& = 0 TO channels(chnr&).info.nshops-1
    owner& = channels(chnr&).shops(shopnr&).owner
    IF channels(chnr&).shops(shopnr&).shoptype > 0 AND owner& < 7 AND (channels(chnr&).player(plnr&).allymask AND 2^owner&) <> 0 THEN
      'Einheit mit der größten Sichtweite im Shop ermitteln
      sight& = 15
      FOR i& = 0 TO 15
        unitnr& = channels(chnr&).shops(shopnr&).content(i&)
        IF unitnr& >= 0 THEN
          unittp& = channels(chnr&).units(unitnr&).unittype
          sight& = MAX&(sight&, channelsnosave(chnr&).unitclasses(unittp&).sight)
        END IF
      NEXT i&
      CALL GetReachableFields(chnr&, plnr&, 0, channels(chnr&).shops(shopnr&).position, channels(chnr&).shops(shopnr&).position2, -1, -1, sight&, 0, 0, reachablearea?(), r)
      'sichtbare Felder markieren
      FOR j& = r.top TO r.bottom
        FOR i& = r.left TO r.right
          IF reachablearea?(i&-r.left, j&-r.top) > 0 THEN
            channels(chnr&).vision(i&, j&) = channels(chnr&).vision(i&, j&) OR plmask&
            channels(chnr&).explored(i&, j&) = channels(chnr&).explored(i&, j&) OR plmask&
          END IF
        NEXT i&
      NEXT j&
    END IF
  NEXT shopnr&
END SUB



'Aktualisiert die überwachten Felder aller Spieler
SUB UpdateVisionAll(chnr&)
  LOCAL plnr&, mask&

  mask& = channels(chnr&).info.aliveplayers
  FOR plnr& = 0 TO %MAXPLAYERS-1
    IF (mask& AND 2^plnr&) <> 0 THEN CALL UpdateVision(chnr&, plnr&)
  NEXT plnr&
END SUB



'Markiert alle Zielfelder für eine Einheit
'md& = 1 : Bewegung
'md& = 2 : Angriffsziele
'md& = 3 : Bewegung + Angriffsziele
FUNCTION GetTargets&(BYVAL chnr&, BYVAL unitnr&, BYVAL md&, BYVAL rangeoverwrite&, targetunits&())
  LOCAL i&, x&, y&, z&, startx&, starty&, plnr&, plmask&, unittp&, range&, weaponnr&, unr&, transporter&, enemy&, friend&, friendunittp&, friendzpos&, mattype&, ground&, overlay&, buildable&, foundtargets&, cl&, canberefueled&
  LOCAL reachablearea?(), notreachablearea?(), r AS RECT, notr AS RECT
  LOCAL xoff&(), yoff&()
  REDIM targetunits&(4096)

  'prüfen, ob Einheit sich in einem Shop oder Transporter befindet
  transporter& = -1
  IF UnitIsInShop&(chnr&, unitnr&) >= 0 THEN
    md& = md& AND 1  'kein Angriff aus Shop heraus möglich
  ELSE
    transporter& = UnitIsInTransporter&(chnr&, unitnr&)
    IF transporter& >= 0 THEN
      md& = md& AND 1  'kein Angriff aus Transporter heraus möglich
      IF (channels(chnr&).units(transporter&).flags AND (%US_ASCEND OR %US_DIVE)) <> 0 THEN md& = 0  'keine Bewegung aus getauchtem oder hoch fliegenden Transporter heraus möglich
    END IF
  END IF

  'Zielfelder suchen
  startx& = channels(chnr&).units(unitnr&).xpos
  starty& = channels(chnr&).units(unitnr&).ypos
  z& = channels(chnr&).units(unitnr&).zpos
  unittp& = channels(chnr&).units(unitnr&).unittype
  plnr& = channels(chnr&).units(unitnr&).owner
  plmask& = 2^plnr&

  'Bewegungsfelder suchen
  IF (md& AND 1) = 1 AND (channels(chnr&).units(unitnr&).flags AND %US_ACTIONUSED) = 0 THEN
    range& = IIF&(rangeoverwrite& = 0, MIN&(channelsnosave(chnr&).unitclasses(unittp&).range-channels(chnr&).units(unitnr&).moved*8, channels(chnr&).units(unitnr&).fuel*8/channelsnosave(chnr&).unitclasses(unittp&).fuelusage), rangeoverwrite&)
    CALL GetReachableFields(chnr&, plnr&, unitnr&, startx&, starty&, -1, -1, range&, channelsnosave(chnr&).unitclasses(unittp&).terrain, 1, reachablearea?(), r)

    'Zielfelder markieren
    FOR y& = r.top TO r.bottom
      FOR x& = r.left TO r.right
        IF reachablearea?(x&-r.left, y&-r.top) > 0 THEN
          unr& = channels(chnr&).zone3(x&, y&)
          IF unr& = -1 THEN
            'Feld ist frei
            channels(chnr&).player(plnr&).targets(x&, y&) = channels(chnr&).player(plnr&).targets(x&, y&) OR %TG_MOVE
            IF foundtargets& < 4096 THEN
              targetunits&(foundtargets&) = x&+y&*256
              foundtargets& = foundtargets&+1
            END IF
          ELSE
            IF unr& < -1 THEN
              'auf dem Feld befindet sich ein Shop
              IF UnitCanEnterBuilding&(chnr&, unitnr&, -2-unr&) AND UnitIsInShop&(chnr&, unitnr&) <> -2-unr& THEN
                channels(chnr&).player(plnr&).targets(x&, y&) = channels(chnr&).player(plnr&).targets(x&, y&) OR %TG_MOVE
                IF foundtargets& < 4096 THEN
                  targetunits&(foundtargets&) = x&+y&*256
                  foundtargets& = foundtargets&+1
                END IF
              END IF
            ELSE
              'auf dem Feld befindet sich eine Einheit
              IF channels(chnr&).units(unr&).unittype = channelsnosave(chnr&).crystalClass AND TransporterCanLoadUnit&(chnr&, unitnr&, unr&) <> 0 THEN
                'Einheit kann eingesammelt werden (Aldinium)
                channels(chnr&).player(plnr&).targets(x&, y&) = channels(chnr&).player(plnr&).targets(x&, y&) OR %TG_MOVE
                IF foundtargets& < 4096 THEN
                  targetunits&(foundtargets&) = x&+y&*256
                  foundtargets& = foundtargets&+1
                END IF
              ELSE
                IF channels(chnr&).units(unr&).owner = plnr& AND (channelsnosave(chnr&).unitclasses(channels(chnr&).units(unr&).unittype).flags AND %UCF_TRANSPORTER) <> 0 AND TransporterCanLoadUnit&(chnr&, unr&, unitnr&) <> 0 AND unr& <> transporter& THEN
                  IF (channels(chnr&).units(unr&).flags AND (%US_ASCEND OR %US_DIVE)) = 0 THEN
                    'in Transporter einsteigen
                    channels(chnr&).player(plnr&).targets(x&, y&) = channels(chnr&).player(plnr&).targets(x&, y&) OR %TG_MOVE
                    IF foundtargets& < 4096 THEN
                      targetunits&(foundtargets&) = x&+y&*256
                      foundtargets& = foundtargets&+1
                    END IF
                  END IF
                ELSE
                  IF (channelsnosave(chnr&).unitclasses(channels(chnr&).units(unr&).unittype).flags AND %UCF_BADDEFENDER) <> 0 AND TransporterCanLoadUnit&(chnr&, unitnr&, unr&) <> 0 THEN
                    'Mine einsammeln (egal von welchem Besitzer)
                    channels(chnr&).player(plnr&).targets(x&, y&) = channels(chnr&).player(plnr&).targets(x&, y&) OR %TG_MOVE
                    IF foundtargets& < 4096 THEN
                      targetunits&(foundtargets&) = x&+y&*256
                      foundtargets& = foundtargets&+1
                    END IF
                  END IF
                END IF
              END IF
            END IF
          END IF
        END IF
      NEXT i&
    NEXT j&
  END IF

  'Angriffsziele suchen
  IF (md& AND 2) = 2 THEN
    FOR weaponnr& = 0 TO 3
      IF channels(chnr&).units(unitnr&).ammo(weaponnr&) > 0 AND channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).damage > 0 THEN
        CALL GetHexRange(chnr&, startx&, starty&, channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).maxrange, reachablearea?(), r)
        CALL GetHexRange(chnr&, startx&, starty&, channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).minrange-1, notreachablearea?(), notr)

        'Zielfelder markieren
        FOR y& = r.top TO r.bottom
          FOR x& = r.left TO r.right
            IF reachablearea?(x&-r.left, y&-r.top) > 0 AND (channels(chnr&).vision(x&, y&) AND plmask&) <> 0 THEN
              IF x& < notr.left OR x& > notr.right OR y& < notr.top OR y& > notr.bottom OR notreachablearea?(x&-notr.left, y&-notr.top) = 0 THEN
                enemy& = channels(chnr&).zone3(x&, y&)
                IF enemy& >= 0 AND IsEnemyUnit&(chnr&, plnr&, enemy&) AND (channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).targets AND (2^channels(chnr&).units(enemy&).zpos)) <> 0 THEN
                  channels(chnr&).player(plnr&).targets(x&, y&) = channels(chnr&).player(plnr&).targets(x&, y&) OR %TG_ATTACK
                  IF foundtargets& < 4096 THEN
                    targetunits&(foundtargets&) = enemy&
                    foundtargets& = foundtargets&+1
                  END IF
                END IF
              END IF
            END IF
          NEXT i&
        NEXT j&
      END IF
    NEXT weaponnr&

    'Vorsorgungsmaterial ermitteln
    mattype& = GetMaterialType&(chnr&, unitnr&)

    'Einheiten zum Befüllen suchen
    CALL GetAdjacentFields(chnr&, startx&, starty&, xoff&(), yoff&())
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REFUEL) <> 0 THEN
      FOR i& = 0 TO 5
        IF xoff&(i&) >= 0 THEN
          friend& = channels(chnr&).zone3(xoff&(i&), yoff&(i&))
          IF friend& >= 0 AND channels(chnr&).units(friend&).owner = plnr& THEN
            friendzpos& = channels(chnr&).units(friend&).zpos
            IF (friendzpos& = %UZ_GROUND OR friendzpos& = %UZ_WATER) AND (z& = %UZ_GROUND OR z& = %UZ_WATER) THEN friendzpos& = z&  'Landeinheiten können Schiffe befüllen und umgekehrt
            IF z& = friendzpos& OR ((channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REFUELAIR) <> 0 AND friendzpos& = 1) THEN
              friendunittp& = channels(chnr&).units(friend&).unittype
              'Treibstoff
              IF (mattype& AND %WP_FUEL) <> 0 THEN
                canberefueled& = IIF&(channels(chnr&).units(friend&).fuel < channelsnosave(chnr&).unitclasses(friendunittp&).fuel, 1, 0)
                FOR weaponnr& = 0 TO 3
                  IF channels(chnr&).units(friend&).ammo(weaponnr&) < channelsnosave(chnr&).unitclasses(friendunittp&).weapons(weaponnr&).ammo AND (channelsnosave(chnr&).unitclasses(friendunittp&).weapons(weaponnr&).targets AND %WP_FUEL) <> 0 THEN
                    canberefueled& = 1
                  END IF
                NEXT weaponnr&
                IF canberefueled& > 0 THEN
                  channels(chnr&).player(plnr&).targets(xoff&(i&), yoff&(i&)) = channels(chnr&).player(plnr&).targets(xoff&(i&), yoff&(i&)) OR %TG_REFUEL
                  IF foundtargets& < 4096 THEN
                    targetunits&(foundtargets&) = friend&
                    foundtargets& = foundtargets&+1
                  END IF
                END IF
              END IF
              'Munition
              IF (mattype& AND %WP_AMMO) <> 0 THEN
                FOR weaponnr& = 0 TO 3
                  IF channels(chnr&).units(friend&).ammo(weaponnr&) < channelsnosave(chnr&).unitclasses(friendunittp&).weapons(weaponnr&).ammo AND (channelsnosave(chnr&).unitclasses(friendunittp&).weapons(weaponnr&).targets AND %WP_FUEL) = 0 THEN
                    channels(chnr&).player(plnr&).targets(xoff&(i&), yoff&(i&)) = channels(chnr&).player(plnr&).targets(xoff&(i&), yoff&(i&)) OR %TG_RECHARGE
                    IF foundtargets& = 0 OR (foundtargets& < 4096 AND targetunits&(foundtargets&-1) <> friend&) THEN
                      targetunits&(foundtargets&) = friend&
                      foundtargets& = foundtargets&+1
                    END IF
                  END IF
                NEXT weaponnr&
              END IF
            END IF
          END IF
        END IF
      NEXT i&
    END IF

    'Land/Wasser-Einheiten zum Reparieren suchen (Flugzeuge können nur in Gebäuden repariert werden)
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_REPAIR) <> 0 AND GetUnitRepairMaterial&(chnr&, unitnr&) > 0 THEN
      FOR i& = 0 TO 5
        IF xoff&(i&) >= 0 THEN
          friend& = channels(chnr&).zone3(xoff&(i&), yoff&(i&))
          IF friend& >= 0 AND channels(chnr&).units(friend&).owner = plnr& AND (channels(chnr&).units(friend&).zpos = %UZ_GROUND OR channels(chnr&).units(friend&).zpos = %UZ_WATER) THEN
            friendunittp& = channels(chnr&).units(friend&).unittype
            IF channels(chnr&).units(friend&).groupsize < channelsnosave(chnr&).unitclasses(friendunittp&).groupsize THEN
              channels(chnr&).player(plnr&).targets(xoff&(i&), yoff&(i&)) = channels(chnr&).player(plnr&).targets(xoff&(i&), yoff&(i&)) OR %TG_REPAIR
              IF foundtargets& < 4096 THEN
                targetunits&(foundtargets&) = friend&
                foundtargets& = foundtargets&+1
              END IF
            END IF
          END IF
        END IF
      NEXT i&
    END IF

    'Straßen/Schienen bauen
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_BUILD) <> 0 AND (mattype& AND %WP_MATERIAL) <> 0 THEN
      CALL GetAdjacentFields(chnr&, startx&, starty&, xoff&(), yoff&())
      FOR i& = 0 TO 5
        IF xoff&(i&) >= 0 THEN
          buildable& = 0
          ground& = channels(chnr&).zone1(xoff&(i&), yoff&(i&))
          overlay& = channels(chnr&).zone2(xoff&(i&), yoff&(i&))
          IF overlay& = -1 AND (channelsnosave(chnr&).terraindef(ground&).typemask AND (%TERRAIN_SHALLOWWATER OR %TERRAIN_DEEPWATER)) = 0 THEN buildable& = 1
          IF overlay& >= 0 AND (channelsnosave(chnr&).terraindef(overlay&).typemask AND %TERRAIN_INFRASTRUCTURE) <> 0 THEN buildable& = 1
          IF overlay& >= 0 AND (channelsnosave(chnr&).terraindef(overlay&).typemask AND %TERRAIN_OBSTACLE) <> 0 THEN buildable& = 1
          IF overlay& = %SPRITE_TITAN_BARRIER THEN buildable& = 1
          IF channels(chnr&).zone3(xoff&(i&), yoff&(i&)) <> -1 THEN buildable& = 0
          IF buildable& = 1 THEN
            channels(chnr&).player(plnr&).targets(xoff&(i&), yoff&(i&)) = channels(chnr&).player(plnr&).targets(xoff&(i&), yoff&(i&)) OR %TG_BUILD
            IF foundtargets& < 4096 THEN
              targetunits&(foundtargets&) = xoff&(i&)+yoff&(i&)*256
              foundtargets& = foundtargets&+1
            END IF
          END IF
        END IF
      NEXT i&
    END IF

    'Befestigungen bauen
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_FORTIFY) <> 0 AND (mattype& AND %WP_MATERIAL) <> 0 THEN
      CALL GetAdjacentFields(chnr&, startx&, starty&, xoff&(), yoff&())
      FOR i& = 0 TO 5
        IF xoff&(i&) >= 0 THEN
          buildable& = 0
          ground& = channels(chnr&).zone1(xoff&(i&), yoff&(i&))
          overlay& = channels(chnr&).zone2(xoff&(i&), yoff&(i&))
          IF overlay& = -1 AND (channelsnosave(chnr&).terraindef(ground&).typemask AND %TERRAIN_WATER) = 0 THEN buildable& = 1
          IF overlay& >= 0 AND (channelsnosave(chnr&).terraindef(overlay&).typemask AND %TERRAIN_INFRASTRUCTURE) <> 0 THEN buildable& = 1
          IF channels(chnr&).zone3(xoff&(i&), yoff&(i&)) <> -1 THEN buildable& = 0
          IF buildable& = 1 THEN
            channels(chnr&).player(plnr&).targets(xoff&(i&), yoff&(i&)) = channels(chnr&).player(plnr&).targets(xoff&(i&), yoff&(i&)) OR %TG_BUILD
            IF foundtargets& < 4096 THEN
              targetunits&(foundtargets&) = xoff&(i&)+yoff&(i&)*256
              foundtargets& = foundtargets&+1
            END IF
          END IF
        END IF
      NEXT i&
    END IF
  END IF

  GetTargets& = foundtargets&
END FUNCTION



'Sichert die Zielmarkierungen eines Spielers
FUNCTION PushTargets$(chnr&, plnr&)
  LOCAL a$

  PushTargets$ = PEEK$(VARPTR(channels(chnr&).player(plnr&).targets(0, 0)), 256*256)
END FUNCTION



'Stellt die Zielmarkierungen eines Spielers wieder her
SUB PopTargets(chnr&, plnr&, a$)
  POKE$ VARPTR(channels(chnr&).player(plnr&).targets(0, 0)), a$
END SUB



'Entfernt alle Zielfeldmarkierungen eines Spielers
SUB ClearTargets(BYVAL chnr&, BYVAL plnr&)
  POKE$ VARPTR(channels(chnr&).player(plnr&).targets(0, 0)), STRING$(256*256, 0)
END SUB



'Bewegungspfad für eine Einheit berechnen
SUB CalculateUnitPath(BYVAL chnr&, BYVAL unitnr&, BYVAL destx&, BYVAL desty&, BYVAL range&)
  LOCAL i&, v&, x&, y&, startx&, starty&, plnr&, plmask&, unittp&
  LOCAL pathx$, pathy$
  LOCAL reachablearea?(), r AS RECT
  LOCAL xoff&(), yoff&()

  'alte Markierungen entfernen
  plnr& = channels(chnr&).units(unitnr&).owner
  CALL ClearTargets(chnr&, plnr&)

  'Pfad zum Zielfeld suchen
  startx& = channels(chnr&).units(unitnr&).xpos
  starty& = channels(chnr&).units(unitnr&).ypos
  unittp& = channels(chnr&).units(unitnr&).unittype
  IF range& = 0 THEN range& = MIN&(channelsnosave(chnr&).unitclasses(unittp&).range, channels(chnr&).units(unitnr&).fuel*8/channelsnosave(chnr&).unitclasses(unittp&).fuelusage)
  CALL GetReachableFields(chnr&, plnr&, unitnr&, startx&, starty&, destx&, desty&, range&, channelsnosave(chnr&).unitclasses(unittp&).terrain, 1, reachablearea?(), r)
  'Zielfelder markieren
'DEBUG ONLY
'  plmask& = NOT plmask&
'  FOR y& = r.top TO r.bottom
'    FOR x& = r.left TO r.right
'      IF reachablearea?(x&-r.left, y&-r.top) > 0 THEN
'        channels(chnr&).targets(x&, y&) = channels(chnr&).targets(x&, y&) OR plmask&
'      END IF
'    NEXT i&
'  NEXT j&

  'Pfad erstellen
  x& = destx&
  y& = desty&
  IF r.left <= x& AND r.right >= x& AND r.top <= y& AND r.bottom >= y& AND reachablearea?(x&-r.left, y&-r.top) > 0 THEN
    pathx$ = CHR$(x&)
    pathy$ = CHR$(y&)
    DO
      CALL GetAdjacentFields(chnr&, x&, y&, xoff&(), yoff&())
      v& = 0
      FOR i& = 0 TO 5
        IF xoff&(i&) >= 0 AND IsInRect&(xoff&(i&), yoff&(i&), r) <> 0 AND reachablearea?(xoff&(i&)-r.left, yoff&(i&)-r.top) > v& THEN
          x& = xoff&(i&)
          y& = yoff&(i&)
          v& = reachablearea?(x&-r.left, y&-r.top)
        END IF
      NEXT i&
      pathx$ = pathx$+CHR$(x&)
      pathy$ = pathy$+CHR$(y&)

      'Endlosschleife prüfen
      IF LEN(pathx$) > %MAXUNITPATH THEN
        IF debugEnabled& = 1 OR gameMode& = %GAMEMODE_SERVER THEN CALL PrintError("CalculateUnitPath failed: "+UnitIDString$(chnr&, unitnr&)+" from "+FORMAT$(startx&)+","+FORMAT$(starty&)+" to "+FORMAT$(destx&)+","+FORMAT$(desty&))
        EXIT LOOP
      END IF
    LOOP UNTIL x& = startx& AND y& = starty&
  END IF

  'Pfad speichern
  IF LEN(pathx$) > %MAXUNITPATH THEN
    pathx$ = LEFT$(pathx$, %MAXUNITPATH)
    pathy$ = LEFT$(pathy$, %MAXUNITPATH)
  END IF
  channels(chnr&).player(plnr&).unitpathlen = LEN(pathx$)
  channels(chnr&).player(plnr&).unitpathpos = 0
  channels(chnr&).player(plnr&).unitanimstep = 0
  POKE$ VARPTR(channels(chnr&).player(plnr&).unitpathx(0)), STRREVERSE$(pathx$)
  POKE$ VARPTR(channels(chnr&).player(plnr&).unitpathy(0)), STRREVERSE$(pathy$)
END SUB



'Aktualisiert die Radarstörung durch jeden Spieler
SUB UpdateSightBlock(chnr&)
  LOCAL x&, y&, i&, j&, unitnr&, unittp&, owner&, sb&, plmask&
  LOCAL reachablearea?()
  LOCAL r AS RECT

  'Status zurücksetzen
  FOR y& = 0 TO channels(chnr&).info.ysize-1
    FOR x& = 0 TO channels(chnr&).info.xsize-1
      channels(chnr&).sightblock(x&, y&) = 0
    NEXT x&
  NEXT y&

  'alle Einheiten mit Störfähigkeit suchen und anwenden
  FOR y& = 0 TO channels(chnr&).info.ysize-1
    FOR x& = 0 TO channels(chnr&).info.xsize-1
      unitnr& = channels(chnr&).zone3(x&, y&)
      IF unitnr& >= 0 THEN
        unittp& = channels(chnr&).units(unitnr&).unittype
        sb& = INT((channelsnosave(chnr&).unitclasses(unittp&).sightblock+7)/8)
        IF sb& > 0 THEN
          'Einheit kann Radar stören
          owner& = channels(chnr&).units(unitnr&).owner
          plmask& = 2^owner&
          CALL GetHexRange(chnr&, x&, y&, sb&, reachablearea?(), r)
          FOR j& = r.top TO r.bottom
            FOR i& = r.left TO r.right
              IF reachablearea?(i&-r.left, j&-r.top) > 0 THEN
                IF (channels(chnr&).sightblock(i&, j&) AND plmask&) = 0 THEN channels(chnr&).sightblock(i&, j&) = channels(chnr&).sightblock(i&, j&) OR plmask& ELSE channels(chnr&).sightblock(i&, j&) = channels(chnr&).sightblock(i&, j&) OR plmask&*256
              END IF
            NEXT i&
          NEXT j&
        END IF
      END IF
    NEXT x&
  NEXT y&
END SUB



'Einheit zum Testen auf ein Feld verschieben
SUB MoveUnitForTesting(BYVAL chnr&, BYVAL unitnr&, BYVAL newx&, BYVAL newy&, BYREF oldzone3&, BYREF oldunitx&, BYREF oldunity&)
  'alte Werte merken
  oldzone3& = channels(chnr&).zone3(newx&, newy&)
  oldunitx& = channels(chnr&).units(unitnr&).xpos
  oldunity& = channels(chnr&).units(unitnr&).ypos

  'Einheit temporär verschieben
  channels(chnr&).zone3(newx&, newy&) = unitnr&
  channels(chnr&).units(unitnr&).xpos = newx&
  channels(chnr&).units(unitnr&).ypos = newy&
END SUB



'Test-Verschiebung rückgängig machen
SUB UndoMoveUnit(chnr&, unitnr&, oldzone3&, oldunitx&, oldunity&)
  channels(chnr&).zone3(channels(chnr&).units(unitnr&).xpos, channels(chnr&).units(unitnr&).ypos) = oldzone3&
  channels(chnr&).units(unitnr&).xpos = oldunitx&
  channels(chnr&).units(unitnr&).ypos = oldunity&
END SUB



'Prüft, ob eine Einheit sich nach dem Kampf zurückziehen kann
FUNCTION UnitCanRetreat&(chnr&, unitnr&)
  LOCAL unittp&

  unittp& = channels(chnr&).units(unitnr&).unittype
  UnitCanRetreat& = (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_RETREAT) <> 0 AND channels(chnr&).units(unitnr&).moved < channelsnosave(chnr&).unitclasses(unittp&).range/8
END FUNCTION



'Prüft, ob eine Einheit ein Feld angreifen könnte (passende Waffe mit Munition vorhanden)
FUNCTION CanAttackField&(chnr&, unitnr&, x&, y&)
  LOCAL unittp&, weaponnr&, dist&, targetunit&, z&
  LOCAL w AS TWeapon

  'Entfernung zum Zielfeld berechnen
  dist& = GetDistance&(channels(chnr&).units(unitnr&).xpos, channels(chnr&).units(unitnr&).ypos, x&, y&)
  unittp& = channels(chnr&).units(unitnr&).unittype

  'Einheit auf Zielfeld auslesen
  targetunit& = channels(chnr&).zone3(x&, y&)
  IF targetunit& >= 0 THEN z& = 2^channels(chnr&).units(targetunit&).zpos ELSE z& = 32

  'alle Waffen der Einheit prüfen
  FOR weaponnr& = 0 TO 3
    w = channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&)
    IF channels(chnr&).units(unitnr&).ammo(weaponnr&) > 0 AND w.damage > 0 THEN
      IF w.minrange <= dist& AND w.maxrange >= dist& AND (w.targets AND z&) <> 0 THEN
        CanAttackField& = -1
        EXIT FUNCTION
      END IF
    END IF
  NEXT weaponnr&

  CanAttackField& = 0
END FUNCTION



'Prüft, ob eine Einheit eine andere angreifen könnte (passende Waffe mit Munition vorhanden)
FUNCTION CanAttackUnitIgnoreDistance&(chnr&, attacker&, defender&)
  LOCAL attackerunittp&, weaponnr&, z&

  'Z-Position des Verteidigers ermitteln
  z& = 2^channels(chnr&).units(defender&).zpos

  'alle Waffen des Angreifers prüfen
  attackerunittp& = channels(chnr&).units(attackerunittp&).unittype
  FOR weaponnr& = 0 TO 3
    IF channels(chnr&).units(attacker&).ammo(weaponnr&) > 0 AND channelsnosave(chnr&).unitclasses(attackerunittp&).weapons(weaponnr&).damage > 0 AND (channelsnosave(chnr&).unitclasses(attackerunittp&).weapons(weaponnr&).targets AND z&) <> 0 THEN
      CanAttackUnitIgnoreDistance& = -1
      EXIT FUNCTION
    END IF
  NEXT weaponnr&

  CanAttackUnitIgnoreDistance& = 0
END FUNCTION



'Prüft, ob eine Einheit eine andere aus einem Hinterhalt angreifen könnte (passende Waffe mit Munition vorhanden und Entfernung 1)
FUNCTION CanAmbushUnit&(chnr&, attacker&, defender&)
  LOCAL attackerunittp&, weaponnr&, z&
  LOCAL w AS TWeapon

  'Z-Position des Verteidigers ermitteln
  z& = 2^channels(chnr&).units(defender&).zpos

  'alle Waffen des Angreifers prüfen
  attackerunittp& = channels(chnr&).units(attacker&).unittype
  FOR weaponnr& = 0 TO 3
    w = channelsnosave(chnr&).unitclasses(attackerunittp&).weapons(weaponnr&)
    IF channels(chnr&).units(attacker&).ammo(weaponnr&) > 0 AND w.damage > 0 AND w.minrange = 1 AND (w.targets AND z&) <> 0 THEN
      CanAmbushUnit& = -1
      EXIT FUNCTION
    END IF
  NEXT weaponnr&

  CanAmbushUnit& = 0
END FUNCTION



'Prüft, ob feindliche Einheiten auf nicht überwachten direkt angrenzenden Feldern einen Überraschungsangriff durchführen können
FUNCTION CheckSurpriseAttack&(chnr&, unitnr&, x&, y&)
  LOCAL i&, plnr&, plmask&, enemyunit&, r&
  LOCAL xoff&(), yoff&()

  'bei Wiedergabe einer Replay ist der Überraschungskampf ein normaler Kampf und wird durch die Replay abgearbeitet
  IF replayMode&(chnr&) >= %REPLAYMODE_PLAY THEN EXIT FUNCTION

  'im Mehrspielermodus verwaltet der Server den Überraschungsangriff
  IF gameMode& = %GAMEMODE_CLIENT THEN EXIT FUNCTION

  'alle angrenzenden Felder prüfen
  channels(chnr&).info.surprisedunit = -1
  plnr& = channels(chnr&).units(unitnr&).owner
  plmask& = 2^plnr&
  CALL GetAdjacentFields(chnr&, x&, y&, xoff&(), yoff&())
  FOR i& = 0 TO 5
    IF xoff&(i&) >= 0 AND (channels(chnr&).vision(xoff&(i&), yoff&(i&)) AND plmask&) = 0 THEN
      enemyunit& = channels(chnr&).zone3(xoff&(i&), yoff&(i&))
      IF enemyunit& >= 0 AND IsEnemyUnit&(chnr&, plnr&, enemyunit&) <> 0 AND CanAmbushUnit&(chnr&, enemyunit&, unitnr&) <> 0 THEN
        channels(chnr&).units(enemyunit&).flags = channels(chnr&).units(enemyunit&).flags OR (%US_SURPRISE_ATTACK1*plmask&)
        channels(chnr&).info.surprisedunit = unitnr&
        channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags OR %US_SURPRISED
        CALL SetPhase(chnr&, plnr&, %PHASE_SURPRISEATTACK)
        r& = -1
      END IF
    END IF
  NEXT i&

  CheckSurpriseAttack& = r&
END FUNCTION



'Einheit abwählen
SUB UnselectUnit(BYVAL chnr&, BYVAL plnr&)
  LOCAL unitnr&

  unitnr& = channels(chnr&).player(plnr&).selectedunit
  channels(chnr&).player(plnr&).selectedunit = -1
  IF unitnr& >= 0 THEN
    IF (channels(chnr&).units(unitnr&).flags AND (%US_MOVED OR %US_ATTACKED)) <> 0 THEN CALL UnitDone(chnr&, unitnr&)
    CALL ClearTargets(chnr&, plnr&)
  END IF
END SUB



'Shop einnehmen
SUB OccupyShop(chnr&, shopnr&, plnr&)
  LOCAL slotnr&, unitnr&, prevowner&, statuskeep&

  IF gameMode& = %GAMEMODE_CLIENT THEN EXIT SUB

  'Energie übernehmen falls Shop neutral war
  IF channels(chnr&).shops(shopnr&).owner = 6 THEN
    channels(chnr&).player(plnr&).energy = channels(chnr&).player(plnr&).energy+channels(chnr&).shops(shopnr&).energy
    IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdatePlayerToClients(chnr&, plnr&)
  END IF

  'Besitzer des Shops wechseln
  prevowner& = channels(chnr&).shops(shopnr&).owner
  channels(chnr&).shops(shopnr&).owner = plnr&
  IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateShopToClients(chnr&, shopnr&)

  'Besitzer aller Einheiten des Shops wechseln
  statuskeep& = %US_ASCEND OR %US_DIVE
  FOR slotnr& = 0 TO 15
    unitnr& = channels(chnr&).shops(shopnr&).content(slotnr&)
    IF unitnr& >= 0 AND channels(chnr&).units(unitnr&).owner <> plnr& THEN
      channels(chnr&).units(unitnr&).owner = plnr&
      channels(chnr&).units(unitnr&).moved = 0
      channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags AND statuskeep&
      channels(chnr&).player(prevowner&).lostunits = channels(chnr&).player(prevowner&).lostunits+1
      channels(chnr&).player(plnr&).capturedunits = channels(chnr&).player(plnr&).capturedunits+1
      IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateUnitToAllClients(chnr&, unitnr&)
    END IF
  NEXT slotnr&
  IF LocalPlayersTurn& THEN CALL UpdateProgressbar

  'Meldung anzeigen
  lastOccupiedShop$ = channels(chnr&).info.shopnames(shopnr&)
  CALL ShowGameMessage(chnr&, plnr&, %MSG_OCCUPIED)
  IF prevowner& < 6 THEN CALL ShowGameMessage(chnr&, prevowner&, IIF&((channels(chnr&).player(prevowner&).allymask AND 2^plnr&) = 0, %ANI_BUILDINGLOST0+plnr&, %ANI_BUILDINGTOALLY))

  'Siegbedingungen prüfen
  CALL CheckVictoryConditions(chnr&)
END SUB



'Verschiebt alle Einheiten in einem Transporter an die aktuelle Position des Transporters
SUB TransportUnits(chnr&, transporter&)
  LOCAL i&, unitnr&

  FOR i& = 0 TO 7
    unitnr& = channels(chnr&).units(transporter&).transportcontent(i&)
    IF unitnr& >= 0 THEN
      channels(chnr&).units(unitnr&).xpos = channels(chnr&).units(transporter&).xpos
      channels(chnr&).units(unitnr&).ypos = channels(chnr&).units(transporter&).ypos
    END IF
  NEXT i&
END SUB



'Verschiebt eine Einheit (mitsamt Inhalt) in einen Shop
SUB EnterShop(chnr&, unitnr&, shopnr&)
  LOCAL i&, slotnr&, unittp&, unr&

  'Aldinium zum Materialvorrat des Shops hinzufügen
  IF channels(chnr&).units(unitnr&).unittype = channelsnosave(chnr&).crystalClass THEN
    channels(chnr&).shops(shopnr&).material = channels(chnr&).shops(shopnr&).material+4*channels(chnr&).units(unitnr&).groupsize
    CALL KillUnit(chnr&, unitnr&, -1, %KILLREASON_CONSUMED, 0)
    IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateShopToClients(chnr&, shopnr&)
    EXIT SUB
  END IF

  'im Client-Modus Einheit nur in den Shop verschieben, falls der Server dies noch nicht getan hat
  IF gameMode& = %GAMEMODE_CLIENT THEN
    FOR slotnr& = 0 TO 15
      IF channels(chnr&).shops(shopnr&).content(slotnr&) = unitnr& THEN EXIT SUB
    NEXT slotnr&
  END IF

  'Einheit in den Shop verschieben
  slotnr& = GetFreeShopSlot&(chnr&, shopnr&)
  IF slotnr& = -1 THEN slotnr& = 15
  channels(chnr&).shops(shopnr&).content(slotnr&) = unitnr&

  'falls die Einheit Ladung hatte, diese ebenfalls in den Shop verschieben
  unittp& = channels(chnr&).units(unitnr&).unittype
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_TRANSPORTER) <> 0 THEN
    FOR i& = 0 TO 7
      unr& = channels(chnr&).units(unitnr&).transportcontent(i&)
      IF unr& >= 0 THEN
        IF channels(chnr&).units(unr&).unittype = channelsnosave(chnr&).crystalClass THEN
          'Aldinium zum Materialvorrat des Shops hinzufügen
          channels(chnr&).shops(shopnr&).material = channels(chnr&).shops(shopnr&).material+4*channels(chnr&).units(unr&).groupsize
          channels(chnr&).units(unr&).groupsize = 0
        ELSE
          'Einheit im Shop platzieren
          slotnr& = GetFreeShopSlot&(chnr&, shopnr&)
          IF slotnr& = -1 THEN slotnr& = 15  'das sollte nur passieren, wenn ein fremdes Gebäude eingenommen wird, in dem kein Platz mehr für die einnehmende Einheit ist
          channels(chnr&).shops(shopnr&).content(slotnr&) = unr&
        END IF
        channels(chnr&).units(unitnr&).transportcontent(i&) = -1
      END IF
    NEXT i&
  END IF
END SUB



'Eine Einheit beendet ihre Bewegung
SUB EndMovement(chnr&, unitnr&, BYVAL x&, BYVAL y&)
  LOCAL plnr&, unittp&, shopnr&, slotnr&, unitatdest&, terrain&, overlay&, foundtargets&, ambushed&
  LOCAL targetunits&()

  'Einheit auf dem Zielfeld platzieren
  gamedataChanged& = 1
  unitMovementStartTime! = 0
  plnr& = channels(chnr&).units(unitnr&).owner
  unittp& = channels(chnr&).units(unitnr&).unittype
  channels(chnr&).units(unitnr&).fuel = channels(chnr&).units(unitnr&).fuel-(channels(chnr&).player(plnr&).unitpathlen-1)*channelsnosave(chnr&).unitclasses(unittp&).fuelusage
  channels(chnr&).units(unitnr&).xpos = x&
  channels(chnr&).units(unitnr&).ypos = y&
  IF channels(chnr&).units(unitnr&).zpos = %UZ_GROUND OR channels(chnr&).units(unitnr&).zpos = %UZ_WATER THEN
    'Landheiten auf Wasser zu Wassereinheiten machen und Schiffe auf Land (unter Brücken) zu Landeinheiten machen
    terrain& = channels(chnr&).zone1(x&, y&)
    overlay& = channels(chnr&).zone2(x&, y&)
    channels(chnr&).units(unitnr&).zpos = IIF&((channelsnosave(chnr&).terraindef(terrain&).typemask AND %TERRAIN_WATER) <> 0 AND overlay& = -1, %UZ_WATER, %UZ_GROUND)
  END IF
  channels(chnr&).units(unitnr&).moved = channels(chnr&).player(plnr&).unitpathlen-1
  channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags OR %US_MOVED
  channels(chnr&).player(plnr&).unitpathlen = 0
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_TRANSPORTER) <> 0 THEN CALL TransportUnits(chnr&, unitnr&)
  unitatdest& = channels(chnr&).zone3(x&, y&)
  IF unitatdest& = unitnr& THEN unitatdest& = -1

  ambushed& = CheckSurpriseAttack&(chnr&, unitnr&, x&, y&)

  IF unitatdest& < -1 THEN
    CALL AddReplay(chnr&, %REPLAY_MOVE, plnr&, MKL$(unitnr&)+MKI$(x&)+MKI$(y&)+PEEK$(VARPTR(channels(chnr&).units(unitnr&)), SIZEOF(TUnit)))
    IF ambushed& = 0 THEN
      'Einheit in den Shop verschieben
      shopnr& = -2-unitatdest&
      CALL EnterShop(chnr&, unitnr&, shopnr&)
      CALL UnitDone(chnr&, unitnr&)
      channels(chnr&).player(plnr&).selectedunit = -1
      CALL SetPhase(chnr&, plnr&, %PHASE_NONE)
      'Shop einnehmen, falls dieser einem anderen Spieler gehört hat
      IF channels(chnr&).shops(shopnr&).owner <> plnr& THEN CALL OccupyShop(chnr&, shopnr&, plnr&)
    END IF
  ELSE
    IF unitatdest& >= 0 THEN
      IF channels(chnr&).units(unitatdest&).unittype = channelsnosave(chnr&).crystalClass THEN
        'Aldinium einsammeln
        CALL TransportLoad(chnr&, unitnr&, unitatdest&)
        channels(chnr&).zone3(x&, y&) = unitnr&
        unitatdest& = -1
      ELSE
        IF (channelsnosave(chnr&).unitclasses(channels(chnr&).units(unitatdest&).unittype).flags AND %UCF_BADDEFENDER) <> 0 THEN
          'Mine einsammeln
          CALL TransportLoad(chnr&, unitnr&, unitatdest&)
          channels(chnr&).zone3(x&, y&) = unitnr&
          unitatdest& = -1
        ELSE
          'in Transporter einsteigen
          CALL TransportLoad(chnr&, unitatdest&, unitnr&)
        END IF
      END IF
    ELSE
      channels(chnr&).zone3(x&, y&) = unitnr&
    END IF
    'Replay aufzeichnen
    CALL AddReplay(chnr&, %REPLAY_MOVE, plnr&, MKL$(unitnr&)+MKI$(x&)+MKI$(y&)+PEEK$(VARPTR(channels(chnr&).units(unitnr&)), SIZEOF(TUnit)))
  END IF

  'Server bescheid geben, daß Animation beendet wurde
  IF gameMode& = %GAMEMODE_CLIENT THEN
    CALL SignalEndAnimation(unitnr&, -1)
  END IF

  'prüfen, ob feindlichen Einheiten einen Überraschungsangriff auf dieses Feld durchführen können
  IF ambushed& <> 0 THEN
    CALL UnitDone(chnr&, unitnr&)
    channels(chnr&).player(plnr&).selectedunit = -1
    EXIT SUB
  END IF

  'Sicht aktualisieren
  IF channelsnosave(chnr&).unitclasses(unittp&).sightblock > 0 THEN CALL UpdateSightBlock(chnr&)
  CALL UpdateVisionAll(chnr&)
  updateMiniMap& = 1
  IF unitatdest& <> -1 THEN
    'falls Einheit nicht mehr auf dem Spielfeld ist (da in Shop oder einem Transporter), dann Ende
    CALL UnitDone(chnr&, unitnr&)
    channels(chnr&).player(plnr&).selectedunit = -1
    CALL SetPhase(chnr&, plnr&, %PHASE_NONE)
    CALL ClearTargets(chnr&, plnr&)
    EXIT SUB
  END IF
  IF (channels(chnr&).units(unitnr&).flags AND %US_MOVEDFROMSHOP) <> 0 THEN
    CALL UnitDone(chnr&, unitnr&)
    CALL SetPhase(chnr&, plnr&, %PHASE_NONE)
    CALL ClearTargets(chnr&, plnr&)
    IF gameMode& <> %GAMEMODE_SERVER AND plnr& = localPlayerNr& AND lastSelectedTransporter& >= 0 THEN CALL ReselectTransporter
    EXIT SUB
  END IF
  CALL SetPhase(chnr&, plnr&, %PHASE_UNITSELECTED)

  'falls Einheit im Mehrspieler-Modus nicht dem lokalen Spieler gehört, dann Ende
  IF gameMode& = %GAMEMODE_CLIENT AND channels(chnr&).units(unitnr&).owner <> localPlayerNr& THEN EXIT SUB

  'prüfen, ob Einheit steigen oder tauchen kann
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_CLIMB) <> 0 THEN
    IF (channels(chnr&).info.aimask AND 2^plnr&) = 0 THEN
      CALL ShowClimbMenu(chnr&, unitnr&)
    ELSE
      CALL AICheckClimb(chnr&, plnr&, unitnr&)
    END IF
  END IF
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_DIVE) <> 0 THEN
    IF (channels(chnr&).info.aimask AND 2^plnr&) = 0 THEN
      CALL ShowDiveMenu(chnr&, unitnr&)
    ELSE
      CALL AICheckDive(chnr&, plnr&, unitnr&)
    END IF
  END IF

  'Angriffsziele suchen
  CALL ClearTargets(chnr&, plnr&)
  IF (channels(chnr&).units(unitnr&).flags AND %US_ATTACKED) = 0 AND (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_COOLDOWN) = 0 THEN foundtargets& = GetTargets&(chnr&, unitnr&, 2, 0, targetunits&())
  IF foundtargets& = 0 THEN CALL UnitDone(chnr&, unitnr&)
END SUB



'Liefert die erste Einheit, die einen Überraschungsangriff durchführt
FUNCTION FindSurpriseAttacker&(chnr&, surprisedunitnr&)
  LOCAL unitnr&, owner&, mask&, isalive&

  IF surprisedunitnr& < 0 THEN
    FindSurpriseAttacker& = -1
    EXIT FUNCTION
  END IF
  isalive& = UnitIsAlive&(chnr&, surprisedunitnr&)
  owner& = channels(chnr&).units(surprisedunitnr&).owner
  mask& = %US_SURPRISE_ATTACK1*2^owner&

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF UnitIsAlive&(chnr&, unitnr&) <> 0 AND (channels(chnr&).units(unitnr&).flags AND mask&) <> 0 THEN
      IF isalive& = 0 THEN
        'Überraschungsangriff abbrechen, da überraschte Einheit tot ist
        channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags AND NOT mask&
      ELSE
        FindSurpriseAttacker& = unitnr&
        EXIT FUNCTION
      END IF
    END IF
  NEXT unitnr&

  FindSurpriseAttacker& = -1
END FUNCTION



'Liefert die beste Waffe (mit Munition) einer Einheit gegen eine andere Einheit
'Rückgabewert: 0 = keine passende Waffe mit Munition vorhanden , 1-4 = beste Waffe
FUNCTION GetBestWeapon&(chnr&, attacker&, defender&)
  LOCAL weaponnr&, bestweapon&, bestdamage&, dist&, z&, attunittp&
  LOCAL w AS TWeapon

  'Entfernung zwischen den Einheiten berechnen
  dist& = GetDistance&(channels(chnr&).units(attacker&).xpos, channels(chnr&).units(attacker&).ypos, channels(chnr&).units(defender&).xpos, channels(chnr&).units(defender&).ypos)

  'Einheitenart des Verteidigers ermitteln (Luft, Land, Wasser, etc.)
  z& = 2^channels(chnr&).units(defender&).zpos

  'alle 4 Waffen vergleichen
  attunittp& = channels(chnr&).units(attacker&).unittype
  FOR weaponnr& = 0 TO 3
    w = channelsnosave(chnr&).unitclasses(attunittp&).weapons(weaponnr&)
    IF channels(chnr&).units(attacker&).ammo(weaponnr&) > 0 AND w.damage > 0 THEN
      IF w.minrange <= dist& AND w.maxrange >= dist& AND (w.targets AND z&) <> 0 THEN
        IF w.damage > bestdamage& THEN
          bestdamage& = w.damage
          bestweapon& = weaponnr&+1
        END IF
      END IF
    END IF
  NEXT weaponnr&

  GetBestWeapon& = bestweapon&
END FUNCTION



'Modifiziert den Angriffwert bestimmter Einheiten um sie dem Battle Isle II Expertenmodus anzupassen
FUNCTION GetAttackModifer!(chnr&, unittp&)
  LOCAL m!

  m! = 1.0
  SELECT CASE unittp&
  CASE 4: m! = 0.5    'Buggy
  CASE 7: m! = 0.33   'Imperator
  CASE 10: m! = 0.5   'Sting
  CASE 12: m! = 0.67  'Artillerie
  CASE 21: m! = 0.5   'Medusa
  CASE 22: m! = 0.33  'Skull
  CASE 27: m! = 0.33  'Anaconda
  CASE 32: m! = 0.5   'Ghost
  CASE 48: m! = 0.5   'Polar
  CASE 50: m! = 0.33  'Hydra
  END SELECT

  GetAttackModifer! = m!
END FUNCTION



'Ermittelt den Verteidigungsbonus durch das Gelände
FUNCTION GetTerrainDefenseBonus&(BYVAL chnr&, BYVAL x&, BYVAL y&)
  LOCAL sprnr1&, sprnr2&, tp&, def&

  sprnr1& = channels(chnr&).zone1(x&, y&)
  sprnr2& = channels(chnr&).zone2(x&, y&)
  def& = INT(channelsnosave(chnr&).terraindef(sprnr1&).defense/2)
  IF sprnr2& >= 0 THEN def& = MAX&(def&, INT(channelsnosave(chnr&).terraindef(sprnr2&).defense/2))
  def& = MIN&(def&, 200)
  tp& = channelsnosave(chnr&).terraindef(sprnr1&).typemask
  IF (tp& AND %TERRAIN_OBSTACLE) <> 0 THEN def& = def&+40
  IF (tp& AND %TERRAIN_MOUNTAIN) <> 0 THEN def& = def&+120
  IF (tp& AND %TERRAIN_FOREST) <> 0 THEN def& = def&+80
  IF sprnr2& >= 0 THEN
    tp& = channelsnosave(chnr&).terraindef(sprnr2&).typemask
    IF (tp& AND %TERRAIN_OBSTACLE) <> 0 THEN def& = def&+40
    IF (tp& AND %TERRAIN_MOUNTAIN) <> 0 THEN def& = def&+120
    IF (tp& AND %TERRAIN_FOREST) <> 0 THEN def& = def&+80
  END IF

  GetTerrainDefenseBonus& = def&
END FUNCTION



'Befüllt die Kampfparameter Struktur
SUB SetCombatData(BYVAL chnr&, BYVAL attacker&, BYVAL defender&, BYVAL attweapon&, BYVAL defweapon&, BYVAL rndmod!)
  LOCAL attunittp&, defunittp&, dist&, xpmod!, hpmod!, srmod!, distmod!, dmg!

  'Waffen bestimmen
  IF attweapon& = -1 THEN attweapon& = GetBestWeapon&(chnr&, attacker&, defender&)
  IF defweapon& = -1 THEN defweapon& = GetBestWeapon&(chnr&, defender&, attacker&)

  'Entfernung zwischen Angreifer und Verteidiger messen
  dist& = GetDistance&(channels(chnr&).units(attacker&).xpos, channels(chnr&).units(attacker&).ypos, channels(chnr&).units(defender&).xpos, channels(chnr&).units(defender&).ypos)

  'Kampfparameter speichern (Angreifer)
  attunittp& = channels(chnr&).units(attacker&).unittype
  channels(chnr&).combat.params(0, 0) = channelsnosave(chnr&).unitclasses(attunittp&).weapons(attweapon&-1).damage
  IF (channelsnosave(chnr&).unitclasses(attunittp&).flags AND %UCF_PLANE) = 0 THEN
    channels(chnr&).combat.params(1, 0) = GetTerrainDefenseBonus&(chnr&, channels(chnr&).units(attacker&).xpos, channels(chnr&).units(attacker&).ypos)
  ELSE
    channels(chnr&).combat.params(1, 0) = 0
  END IF
  IF dist& = 1 THEN
    channels(chnr&).combat.params(2, 0) = CalculateSurroundingFactor&(chnr&, attacker&, defender&)
  ELSE
    channels(chnr&).combat.params(2, 0) = 0
  END IF
  channels(chnr&).combat.params(3, 0) = channelsnosave(chnr&).unitclasses(attunittp&).attack
  xpmod! = 0.8+(0.5*channels(chnr&).units(attacker&).experience)
  hpmod! = channels(chnr&).units(attacker&).groupsize/channelsnosave(chnr&).unitclasses(attunittp&).groupsize
  srmod! = 1+channels(chnr&).combat.params(2, 0)/400
  distmod! = ASC(CHR$(100, 90, 81, 73, 66, 60, 55, 51, 48, 47  ,  46, 45, 44, 43, 42, 41, 40, 39, 38, 37), dist&)/100
  IF rndmod! = 0 THEN rndmod! = RND(800, 1200)/1000
  channels(chnr&).combat.params(4, 0) = channels(chnr&).combat.params(0, 0)*hpmod!*xpmod!*srmod!*distmod!*rndmod!
  IF dist& > 1 THEN channels(chnr&).combat.params(4, 0) = channels(chnr&).combat.params(4, 0)*GetAttackModifer!(chnr&, attunittp&)

  'Kampfparameter speichern (Verteidiger)
  defunittp& = channels(chnr&).units(defender&).unittype
  IF (channelsnosave(chnr&).unitclasses(defunittp&).flags AND %UCF_PLANE) = 0 THEN
    channels(chnr&).combat.params(1, 1) = GetTerrainDefenseBonus&(chnr&, channels(chnr&).units(defender&).xpos, channels(chnr&).units(defender&).ypos)
  ELSE
    channels(chnr&).combat.params(1, 1) = 0
  END IF
  IF defweapon& > 0 THEN
    channels(chnr&).combat.params(0, 1) = channelsnosave(chnr&).unitclasses(defunittp&).weapons(defweapon&-1).damage
    IF (channelsnosave(chnr&).unitclasses(defunittp&).flags AND %UCF_BADDEFENDER) <> 0 THEN channels(chnr&).combat.params(0, 1) = channels(chnr&).combat.params(0, 1)/5
    channels(chnr&).combat.params(3, 1) = channelsnosave(chnr&).unitclasses(defunittp&).attack
  ELSE
    channels(chnr&).combat.params(0, 1) = 0
    channels(chnr&).combat.params(3, 1) = 0
  END IF
  channels(chnr&).combat.params(2, 1) = 0  'Verteidiger kann nicht klemmen
  xpmod! = 0.8+(0.5*channels(chnr&).units(defender&).experience)
  hpmod! = channels(chnr&).units(defender&).groupsize/channelsnosave(chnr&).unitclasses(defunittp&).groupsize
  IF rndmod! = 0 THEN rndmod! = RND(800, 1200)/1000
  channels(chnr&).combat.params(4, 1) = channels(chnr&).combat.params(0, 1)*hpmod!*xpmod!*rndmod!

  'Schaden berechnen: Schadenspunkte = (GesamtwertAngreifer - TerrainVerteidiger) / PanzerungVerteidiger
  dmg! = MAX(0, (channels(chnr&).combat.params(4, 1)-channels(chnr&).combat.params(1, 0))/channelsnosave(chnr&).unitclasses(attunittp&).armor)
  channels(chnr&).combat.params(5, 0) = MIN&(ROUND(dmg!, 0), channels(chnr&).units(attacker&).groupsize)
  dmg! = MAX(0, (channels(chnr&).combat.params(4, 0)-channels(chnr&).combat.params(1, 1))/channelsnosave(chnr&).unitclasses(defunittp&).armor)
  channels(chnr&).combat.params(5, 1) = MIN&(ROUND(dmg!, 0), channels(chnr&).units(defender&).groupsize)

  'Kampfgegner speichern
  channels(chnr&).combat.attacker = attacker&
  channels(chnr&).combat.defender = defender&
  channels(chnr&).combat.weaponatt = attweapon&
  channels(chnr&).combat.weapondef = defweapon&
END SUB



'Kampf beginnen
SUB StartCombat(BYVAL chnr&, BYVAL attacker&, BYVAL defender&, BYVAL attweapon&, BYVAL defweapon&, BYVAL combatDataPreset&)
  LOCAL attackingplayer&, defendingplayer&

  'Kampfparameter festlegen
  IF combatDataPreset& = 0 THEN CALL SetCombatData(chnr&, attacker&, defender&, attweapon&, defweapon&, 0)

  'Kampf starten
  attackingplayer& = channels(chnr&).units(attacker&).owner
  defendingplayer& = channels(chnr&).units(defender&).owner
  CALL SetPhase(chnr&, attackingplayer&, %PHASE_COMBAT)
  CALL SetPhase(chnr&, defendingplayer&, %PHASE_COMBAT)
  IF gameMode& = %GAMEMODE_SINGLE THEN CALL SetPhase(chnr&, localPlayerNr&, %PHASE_COMBAT)
  combatStartTime! = gametime!
  combatSoundEffects& = 0
  IF gameMode& <> %GAMEMODE_SERVER THEN
    CALL CreateMissile(attacker&, defender&, channels(chnr&).combat.weaponatt, channels(chnr&).combat.params(5, 1))
    CALL CreateMissile(defender&, attacker&, channels(chnr&).combat.weapondef, channels(chnr&).combat.params(5, 0))
  END IF

  'Einheit des aktiven Spielers als benutzt markieren
  IF channels(chnr&).units(attacker&).owner = channels(chnr&).info.activeplayer AND UnitCanRetreat&(chnr&, attacker&) = 0 THEN CALL UnitDone(chnr&, attacker&)

  'zum Kampfgeschehen scrollen, falls lokaler Spieler angegriffen wird
  IF gameMode& = %GAMEMODE_SINGLE AND defendingplayer& = localPlayerNr& THEN CALL ScrollToMapPos(channels(0).units(defender&).xpos, channels(0).units(defender&).ypos, 0.5)

  'Replay aufzeichnen
  CALL AddReplay(chnr&, %REPLAY_ATTACK, attackingplayer&, PEEK$(VARPTR(channels(chnr&).combat), SIZEOF(TCombatInfo)))
END SUB



'Kampf beenden
SUB EndCombat(chnr&, combatData AS TCombatInfo)
  LOCAL a$$, b$$, attackingplayer&, attacker&, defender&, defendingplayer&, canretreat&, targetunits&()

  'Protokolleintrag erstellen
  gamedataChanged& = 1
  attacker& = combatData.attacker
  attackingplayer& = channels(chnr&).units(attacker&).owner
  defender& = combatData.defender
  defendingplayer& = channels(chnr&).units(defender&).owner
  a$$ = words$$(%WORD_COMBAT_LOG)
  b$$ = UnitIDString$(chnr&, defender&)+" "+FORMAT$(channels(chnr&).units(defender&).groupsize)+">"+FORMAT$(channels(chnr&).units(defender&).groupsize-combatData.params(5, 1))
  REPLACE "$" WITH "$COLOR"+FORMAT$(channels(chnr&).units(defender&).owner+1)+"$"+b$$+"$COLOR0$" IN a$$
  b$$ = UnitIDString$(chnr&, attacker&)+" "+FORMAT$(channels(chnr&).units(attacker&).groupsize)+">"+FORMAT$(channels(chnr&).units(attacker&).groupsize-combatData.params(5, 0))
  REPLACE "%" WITH "$COLOR"+FORMAT$(channels(chnr&).units(attacker&).owner+1)+"$"+b$$+"$COLOR0$" IN a$$
  CALL AddProtocol(a$$)

  'Schaden verrechnen
  CALL DamageUnit(chnr&, attacker&, defender&, combatData.params(5, 1))
  CALL DamageUnit(chnr&, defender&, attacker&, combatData.params(5, 0))
  channels(chnr&).units(attacker&).ammo(combatData.weaponatt-1) = channels(chnr&).units(attacker&).ammo(combatData.weaponatt-1)-1
  IF combatData.weapondef > 0 THEN channels(chnr&).units(defender&).ammo(combatData.weapondef-1) = channels(chnr&).units(defender&).ammo(combatData.weapondef-1)-1

  'Sicht aktualisieren
  CALL UpdateVisionAll(chnr&)
  IF attackingplayer& = localPlayerNr& THEN CALL ClearTargets(chnr&, attackingplayer&)
  updateMiniMap& = 1

  'Server bescheid geben, daß Animation beendet wurde
  IF gameMode& = %GAMEMODE_CLIENT THEN
    CALL SignalEndAnimation(attacker&, defender&)
  END IF

  'nächsten Überraschungsangriff abarbeiten (oder Kampf beenden falls kein Überraschungsangriff vorliegt)
  IF gameMode& = %GAMEMODE_SINGLE THEN
    CALL SurpriseAttack(chnr&)
  ELSE
    SetPhase(chnr&, attackingplayer&, %PHASE_NONE)
    SetPhase(chnr&, defendingplayer&, %PHASE_NONE)
    IF gameMode& = %GAMEMODE_CLIENT AND attackingplayer& = localPlayerNr& AND channels(chnr&).player(localPlayerNr&).selectedunit = attacker& AND UnitIsAlive&(chnr&, attacker&) = 0 THEN channels(chnr&).player(localPlayerNr&).selectedunit = -1
    IF gameMode& = %GAMEMODE_CLIENT AND defendingplayer& = localPlayerNr& AND channels(chnr&).player(localPlayerNr&).selectedunit = defender& AND UnitIsAlive&(chnr&, defender&) = 0 THEN channels(chnr&).player(localPlayerNr&).selectedunit = -1
  END IF

  'Einheit abwählen, falls diese sich nicht noch nach dem Kampf bewegen kann
  IF IsActivePlayer&(chnr&, attackingplayer&) THEN channels(chnr&).units(attacker&).flags = channels(chnr&).units(attacker&).flags OR %US_ATTACKED
  IF GetPhase&(chnr&, attackingplayer&) = %PHASE_NONE AND UnitIsAlive&(chnr&, attacker&) <> 0 AND attackingplayer& = localPlayerNr& THEN
    IF UnitCanRetreat&(chnr&, attacker&) <> 0 THEN
      SetPhase(chnr&, attackingplayer&, %PHASE_UNITSELECTED)
      CALL ClearTargets(chnr&, channels(chnr&).units(attacker&).owner)
      CALL GetTargets&(chnr&, attacker&, 1, 0, targetunits&())
      canretreat& = 1
    END IF
  END IF

  'Aktionen prüfen, falls Einheit sich nicht noch nach dem Kampf bewegen kann
  IF canretreat& = 0 THEN CALL CheckActions(chnr&)
END SUB



'Überraschungsangriff durchführen (nur Einzelspieler-Modus)
SUB SurpriseAttack(chnr&)
  LOCAL surprisedunit&, unitnr&, owner&, mask&, x&, y&, unitatdest&, shopnr&, unittp&

  'nächsten Angreifer finden
  surprisedunit& = IIF&(gameMode& = %GAMEMODE_CLIENT, -1, channels(chnr&).info.surprisedunit)  'im Mehrspielermodus verwaltet der Server den Überraschungsangriff
  IF surprisedunit& >= 0 THEN
    owner& = channels(chnr&).units(surprisedunit&).owner
    mask& = %US_SURPRISE_ATTACK1*2^owner&
    unitnr& = FindSurpriseAttacker&(chnr&, surprisedunit&)
  ELSE
    unitnr& = -1
  END IF
  IF unitnr& < 0 OR replayMode&(chnr&) >= %REPLAYMODE_PLAY THEN
    'alle Angreifer wurden abgearbeitet bzw. bei Wiedergabe einer Replay ist der Überraschungskampf ein normaler Kampf und wird durch die Replay abgearbeitet
    CALL SetPhaseAll(chnr&, %PHASE_NONE, %PHASE_COMBAT)
    CALL SetPhaseAll(chnr&, %PHASE_NONE, %PHASE_SURPRISEATTACK)
    'falls die überraschte Einheit noch lebt und dabei war, ein Gebäude einzunehmen, dann jetzt das Gebäude einnehmen
    IF surprisedunit& >= 0 AND UnitIsAlive&(chnr&, surprisedunit&) THEN
      x& = channels(chnr&).units(surprisedunit&).xpos
      y& = channels(chnr&).units(surprisedunit&).ypos
      unitatdest& = channels(chnr&).zone3(x&, y&)
      IF unitatdest& < -1 THEN
        shopnr& = -2-unitatdest&
        CALL EnterShop(chnr&, surprisedunit&, shopnr&)
        CALL UnitDone(chnr&, surprisedunit&)
        channels(chnr&).player(owner&).selectedunit = -1
        CALL SetPhase(chnr&, owner&, %PHASE_NONE)
        'Shop einnehmen, falls dieser einem anderen Spieler gehört hat
        IF channels(chnr&).shops(shopnr&).owner <> owner& THEN CALL OccupyShop(chnr&, shopnr&, owner&)
        unittp& = channels(chnr&).units(surprisedunit&).unittype
        IF channelsnosave(chnr&).unitclasses(unittp&).sightblock > 0 THEN CALL UpdateSightBlock(chnr&)
        CALL UpdateVisionAll(chnr&)
        updateMiniMap& = 1
      END IF
    END IF
    EXIT SUB
  END IF
  channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags AND NOT mask&

  SELECT CASE gameMode&
  CASE %GAMEMODE_SINGLE
    'Kampf durchführen
    CALL StartCombat(chnr&, unitnr&, surprisedunit&, -1, -1, 0)
  CASE %GAMEMODE_SERVER
    'Kampf auf Server wird durchführen, wenn der Besitzer der überraschten Einheit die Bewegungsanimation für diese Einheit abgeschlossen hat (ClientFinishedAnimation)
  CASE %GAMEMODE_CLIENT
    'Phase zurücksetzen, da Überraschungsangriff durch Server neu gestartet wird
    CALL SetPhaseAll(chnr&, %PHASE_NONE, %PHASE_COMBAT)
    CALL SetPhaseAll(chnr&, %PHASE_NONE, %PHASE_SURPRISEATTACK)
  END SELECT
END SUB



'Kampfschaden zufügen
SUB DamageUnit(chnr&, attacker&, defender&, damage&)
  LOCAL unittp&, slotnr&, unr&, attplayer&

  'Schaden zufügen
  IF damage& < 1 THEN EXIT SUB
  attplayer& = channels(chnr&).units(attacker&).owner
  channels(chnr&).units(defender&).groupsize = channels(chnr&).units(defender&).groupsize-MIN&(damage&, channels(chnr&).units(defender&).groupsize)
  channels(chnr&).units(attacker&).experience = MIN&(%MAX_EXPERIENCE, channels(chnr&).units(attacker&).experience+1)
  IF UnitIsAlive&(chnr&, defender&) = 0 THEN
    channels(chnr&).units(attacker&).experience = MIN&(%MAX_EXPERIENCE, channels(chnr&).units(attacker&).experience+1)
    'Einheit von der Karte entfernen
    CALL KillUnit(chnr&, defender&, attplayer&, %KILLREASON_DAMAGE, 0)
    'Siegbedingungen prüfen
    CALL CheckVictoryConditions(chnr&)
    EXIT SUB
  END IF

  'bei Transportern geladenen Einheiten denselben Schaden zufügen
  unittp& = channels(chnr&).units(defender&).unittype
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_TRANSPORTER) <> 0 THEN
    FOR slotnr& = 0 TO 7
      unr& = channels(chnr&).units(defender&).transportcontent(slotnr&)
      IF unr& >= 0 AND UnitIsAlive&(chnr&, unr&) <> 0 THEN
        channels(chnr&).units(unr&).groupsize = channels(chnr&).units(unr&).groupsize-MIN&(damage&, channels(chnr&).units(unr&).groupsize)
        IF UnitIsAlive&(chnr&, unr&) = 0 THEN
          channels(chnr&).units(defender&).transportcontent(slotnr&) = -1
          CALL KillUnit(chnr&, unr&, attplayer&, %KILLREASON_DAMAGE, 0)
          CALL CheckVictoryConditions(chnr&)
        END IF
      END IF
    NEXT slotnr&
  END IF
END SUB



'Einheit zerstören
'md=0 : normal zerstören
'md=1 : Einheit ist in einem Transporter (der ebenfalls zerstört wird), weswegen Sicht nicht neu berechnet werden braucht
'md=2 : Einheit ist virtuell (für Vorschau)
SUB KillUnit(chnr&, unitnr&, killingplayer&, killreason&, md&)
  LOCAL x&, y&, owner&, unittp&, slotnr&, unr&
  LOCAL a$$

  'Einheit vom Spielfeld entfernen
  x& = channels(chnr&).units(unitnr&).xpos
  y& = channels(chnr&).units(unitnr&).ypos
  owner& = channels(chnr&).units(unitnr&).owner
  unittp& = channels(chnr&).units(unitnr&).unittype
  channels(chnr&).units(unitnr&).groupsize = 0
  IF gameMode& = %GAMEMODE_SERVER THEN CALL DestroyUnitOnClients(chnr&, unitnr&, killingplayer&, killreason&)
  IF unittp& <> channelsnosave(chnr&).crystalClass THEN
    channels(chnr&).player(owner&).lostunits = channels(chnr&).player(owner&).lostunits+1
    IF killingplayer& >= 0 THEN channels(chnr&).player(killingplayer&).killedunits = channels(chnr&).player(killingplayer&).killedunits+1
  END IF
  IF channels(chnr&).zone3(x&, y&) = unitnr& THEN
    channels(chnr&).zone3(x&, y&) = -1
    IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateZoneToClients(chnr&, x&, y&)
  END IF

  'Protokolleintrag erstellen
  SELECT CASE killreason&
  CASE %KILLREASON_NOFUEL
    a$$ = words$$(%WORD_AIRUNIT_OUT_OF_FUEL)
    REPLACE "%" WITH "$COLOR"+FORMAT$(owner&+1)+"$"+channelsnosave(chnr&).unitclasses(unittp&).uname+"("+FORMAT$(unitnr&)+")$COLOR0$" IN a$$
    CALL AddProtocol(a$$)
  CASE %KILLREASON_SUNKEN:
    a$$ = words$$(%WORD_GROUNDUNIT_SUNKEN)
    REPLACE "%" WITH "$COLOR"+FORMAT$(owner&+1)+"$"+channelsnosave(chnr&).unitclasses(unittp&).uname+"("+FORMAT$(unitnr&)+")$COLOR0$" IN a$$
    CALL AddProtocol(a$$)
  END SELECT

  'bei Transportern alle geladenen Einheiten ebenfalls zerstören
  IF md& = 1 THEN EXIT SUB
  IF md& = 0 THEN
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_TRANSPORTER) <> 0 THEN
      FOR slotnr& = 0 TO 7
        unr& = channels(chnr&).units(unitnr&).transportcontent(slotnr&)
        IF unr& >= 0 AND UnitIsAlive&(chnr&, unr&) <> 0 THEN CALL KillUnit(chnr&, unr&, killingplayer&, killreason&, 1)
      NEXT slotnr&
    END IF
  END IF

  'Einheitenzähler optimieren
  WHILE channels(chnr&).info.nunits > 0 AND UnitIsAlive&(chnr&, channels(chnr&).info.nunits-1) = 0
    channels(chnr&).info.nunits = channels(chnr&).info.nunits-1
  WEND
  IF md& = 2 THEN EXIT SUB

  CALL UpdateSightBlock(chnr&)
  CALL CheckVictoryConditions(chnr&)
  IF LocalPlayersTurn& THEN CALL UpdateProgressbar
END SUB



'Einheit aus Shop entfernen
SUB RemoveUnitFromShop(chnr&, shopnr&, unitnr&)
  LOCAL i&

  FOR i& = 0 TO 15
    IF channels(chnr&).shops(shopnr&).content(i&) = unitnr& THEN
      channels(chnr&).shops(shopnr&).content(i&) = -1
      channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags OR %US_MOVEDFROMSHOP
    END IF
  NEXT i&
END SUB



'Alle Einheiten auf der Karte befüllen
SUB RefuelAllUnits(chnr&)
  LOCAL unitnr&, unittp&, weaponnr&

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF UnitIsAlive&(chnr&, unitnr&) <> 0 THEN
      'Treibstoff auffüllen
      unittp& = channels(chnr&).units(unitnr&).unittype
      channels(chnr&).units(unitnr&).fuel = channelsnosave(chnr&).unitclasses(unittp&).fuel
      'Munition auffüllen
      FOR weaponnr& = 0 TO 3
        channels(chnr&).units(unitnr&).ammo(weaponnr&) = channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).ammo
      NEXT weaponnr&
    END IF
  NEXT unitnr&
END SUB



'Einheit im Shop befüllen
SUB RefuelInShop(chnr&, shopnr&, unitnr&)
  LOCAL shopowner&, unittp&, weaponnr&, refueled&, recharged&

  gamedataChanged& = 1
  shopowner& = channels(chnr&).shops(shopnr&).owner
  unittp& = channels(chnr&).units(unitnr&).unittype

  'Treibstoff auffüllen
  IF channels(chnr&).player(shopowner&).energy > 0 AND channels(chnr&).units(unitnr&).fuel < channelsnosave(chnr&).unitclasses(unittp&).fuel THEN
    channels(chnr&).units(unitnr&).fuel = channelsnosave(chnr&).unitclasses(unittp&).fuel
    channels(chnr&).player(shopowner&).energy = channels(chnr&).player(shopowner&).energy-1
    refueled& = 1
    CALL SetShopAnimation(unitnr&, %MAPOVERLAY_REFUEL)
  END IF

  'Munition auffüllen
  IF channels(chnr&).shops(shopnr&).material > 0 THEN
    FOR weaponnr& = 0 TO 3
      IF channels(chnr&).units(unitnr&).ammo(weaponnr&) < channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).ammo THEN
        channels(chnr&).units(unitnr&).ammo(weaponnr&) = channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).ammo
        recharged& = 1
        CALL SetShopAnimation(unitnr&, %MAPOVERLAY_REFUEL)
      END IF
    NEXT weaponnr&
    IF recharged& = 1 THEN channels(chnr&).shops(shopnr&).material = channels(chnr&).shops(shopnr&).material-1
  END IF

  'Replay aufzeichnen
  IF refueled& = 1 OR recharged& = 1 THEN CALL AddReplay(chnr&, %REPLAY_SHOPACTION, shopowner&, MKL$(shopnr&)+MKL$(%SHOPACTION_REFUEL)+MKL$(unitnr&)+PEEK$(VARPTR(channels(chnr&).shops(shopnr&)), SIZEOF(TShop)))
END SUB



'Einheit durch Versorgungsfahrzeug befüllen
SUB Refuel(chnr&, BYVAL supportunitnr&, BYVAL targetunitnr&)
  LOCAL plnr&, ai&, mattype&, weaponnr&, supportunittp&, targetunittp&, req&, factor&, wpfuel&, wpammo&, refueled&, ammorecharged&, targetunits&()

  'Vorsorgungsmaterial ermitteln
  gamedataChanged& = 1
  plnr& = channels(chnr&).units(supportunitnr&).owner
  IF (channels(chnr&).info.aimask AND 2^plnr&) <> 0 THEN ai& = 1
  factor& = IIF&(ai& = 1, 4, 1)  'bei normaler Schwierigkeit verbraucht die KI nur 1/4 der Resourcen
  supportunittp& = channels(chnr&).units(supportunitnr&).unittype
  FOR weaponnr& = 0 TO 3
    IF channels(chnr&).units(supportunitnr&).ammo(weaponnr&) > 0 THEN
      mattype& = mattype& OR channelsnosave(chnr&).unitclasses(supportunittp&).weapons(weaponnr&).targets
      IF (channelsnosave(chnr&).unitclasses(supportunittp&).weapons(weaponnr&).targets AND %WP_FUEL) <> 0 THEN wpfuel& = weaponnr&
      IF (channelsnosave(chnr&).unitclasses(supportunittp&).weapons(weaponnr&).targets AND %WP_AMMO) <> 0 THEN wpammo& = weaponnr&
    END IF
  NEXT weaponnr&

  'Treibstoff auffüllen
  targetunittp& = channels(chnr&).units(targetunitnr&).unittype
  req& = channelsnosave(chnr&).unitclasses(targetunittp&).fuel-channels(chnr&).units(targetunitnr&).fuel
  IF (mattype& AND %WP_FUEL) <> 0 AND req& > 0 THEN
    IF channels(chnr&).info.difficulty = %DIFFICULTY_EASY OR (ai& = 1 AND channels(chnr&).info.difficulty = %DIFFICULTY_HARD) THEN
      'es wird nur 1 Einheit Treibstoff des Versorgungsfahrzeugs benötigt
      channels(chnr&).units(targetunitnr&).fuel = channelsnosave(chnr&).unitclasses(targetunittp&).fuel
      refueled& = 1
    ELSE
      'es wird soviel Treibstoff des Versorgungsfahrzeugs benötigt wie der Zieleinheit fehlt
      req& = MIN&(req&, channels(chnr&).units(supportunitnr&).ammo(wpfuel&)*factor&)
      channels(chnr&).units(targetunitnr&).fuel = channels(chnr&).units(targetunitnr&).fuel+req&
      channels(chnr&).units(supportunitnr&).ammo(wpfuel&) = channels(chnr&).units(supportunitnr&).ammo(wpfuel&)-req&/factor&
      refueled& = req&
    END IF
  END IF

  'Treibstoffvorrat eines anderen Versorgungsfahrzeugs auffüllen
  IF (mattype& AND %WP_FUEL) <> 0 THEN
    FOR weaponnr& = 0 TO 3
      IF (channelsnosave(chnr&).unitclasses(targetunittp&).weapons(weaponnr&).targets AND %WP_FUEL) <> 0 THEN
        req& = channelsnosave(chnr&).unitclasses(targetunittp&).weapons(weaponnr&).ammo-channels(chnr&).units(targetunitnr&).ammo(weaponnr&)
        IF req& > 0 THEN
          IF channels(chnr&).info.difficulty = %DIFFICULTY_EASY OR (ai& = 1 AND channels(chnr&).info.difficulty = %DIFFICULTY_HARD) THEN
            'es wird nur 1 Einheit Treibstoff des Versorgungsfahrzeugs benötigt
            channels(chnr&).units(targetunitnr&).ammo(weaponnr&) = channelsnosave(chnr&).unitclasses(targetunittp&).weapons(weaponnr&).ammo
            refueled& = 1
          ELSE
            'es wird soviel Treibstoff des Versorgungsfahrzeugs benötigt wie der Zieleinheit fehlt
            req& = MIN&(req&, channels(chnr&).units(supportunitnr&).ammo(wpfuel&)*factor&)
            channels(chnr&).units(targetunitnr&).ammo(weaponnr&) = channels(chnr&).units(targetunitnr&).ammo(weaponnr&)+req&
            channels(chnr&).units(supportunitnr&).ammo(wpfuel&) = channels(chnr&).units(supportunitnr&).ammo(wpfuel&)-req&/factor&
            refueled& = refueled&+req&
          END IF
        END IF
      END IF
    NEXT weaponnr&
  END IF
  IF refueled& = 1 AND (channels(chnr&).info.difficulty = %DIFFICULTY_EASY OR (ai& = 1 AND channels(chnr&).info.difficulty = %DIFFICULTY_HARD)) THEN
    channels(chnr&).units(supportunitnr&).ammo(wpfuel&) = channels(chnr&).units(supportunitnr&).ammo(wpfuel&)-1
  END IF

  'Munition auffüllen
  IF (mattype& AND %WP_AMMO) <> 0 THEN
    FOR weaponnr& = 0 TO 3
      IF channelsnosave(chnr&).unitclasses(targetunittp&).weapons(weaponnr&).damage > 0 OR (channelsnosave(chnr&).unitclasses(targetunittp&).weapons(weaponnr&).targets AND (%WP_AMMO OR %WP_MATERIAL)) <> 0 THEN
        req& = channelsnosave(chnr&).unitclasses(targetunittp&).weapons(weaponnr&).ammo-channels(chnr&).units(targetunitnr&).ammo(weaponnr&)
        IF req& > 0 THEN
          IF channels(chnr&).info.difficulty = %DIFFICULTY_EASY  OR (ai& = 1 AND channels(chnr&).info.difficulty = %DIFFICULTY_HARD) THEN
            'es wird nur 1 Einheit Munition des Versorgungsfahrzeugs benötigt
            channels(chnr&).units(targetunitnr&).ammo(weaponnr&) = channelsnosave(chnr&).unitclasses(targetunittp&).weapons(weaponnr&).ammo
            ammorecharged& = 1
          ELSE
            'es wird soviel Munition des Versorgungsfahrzeugs benötigt wie der Zieleinheit fehlt
            req& = MIN&(req&, channels(chnr&).units(supportunitnr&).ammo(wpammo&)*factor&)
            channels(chnr&).units(targetunitnr&).ammo(weaponnr&) = channels(chnr&).units(targetunitnr&).ammo(weaponnr&)+req&
            channels(chnr&).units(supportunitnr&).ammo(wpammo&) = channels(chnr&).units(supportunitnr&).ammo(wpammo&)-req&/factor&
            ammorecharged& = ammorecharged&+req&
          END IF
        END IF
      END IF
    NEXT weaponnr&
    IF ammorecharged& = 1 AND (channels(chnr&).info.difficulty = %DIFFICULTY_EASY OR (ai& = 1 AND channels(chnr&).info.difficulty = %DIFFICULTY_HARD)) THEN
      channels(chnr&).units(supportunitnr&).ammo(wpammo&) = channels(chnr&).units(supportunitnr&).ammo(wpammo&)-1
    END IF
  END IF

  'Replay aufzeichnen
  CALL AddReplay(chnr&, %REPLAY_UNITACTION, plnr&, _
    MKL$(supportunitnr&)+MKI$(%UNITACTION_REFUEL)+MKI$(channels(chnr&).units(targetunitnr&).xpos)+MKI$(channels(chnr&).units(targetunitnr&).ypos)+PEEK$(VARPTR(channels(chnr&).units(supportunitnr&)), SIZEOF(TUnit)))

  'falls Versorgungsfahrzeug nicht bewegt wurde nach weiteren Zielen suchen
  CALL ClearTargets(chnr&, plnr&)
  IF refueled& > 0 OR ammorecharged& > 0 THEN channels(chnr&).units(supportunitnr&).flags = channels(chnr&).units(supportunitnr&).flags OR %US_ACTIONUSED
  IF (channels(chnr&).units(supportunitnr&).flags AND %US_MOVED) = 0 THEN
    IF ai& = 0 THEN
      CALL SetPhase(chnr&, plnr&, %PHASE_UNITSELECTED)
      IF GetTargets&(chnr&, supportunitnr&, 2, 0, targetunits&()) = 0 THEN CALL UnselectUnit(chnr&, channels(chnr&).units(supportunitnr&).owner)
    END IF
  ELSE
    CALL UnitDone(chnr&, supportunitnr&)
  END IF
END SUB



'Ermittelt welche Art von Material ein Versorgungsfahrzeug verfügbar hat
FUNCTION GetMaterialType&(chnr&, unitnr&)
  LOCAL unittp&, weaponnr&, mattype&

  unittp& = channels(chnr&).units(unitnr&).unittype
  FOR weaponnr& = 0 TO 3
    IF channels(chnr&).units(unitnr&).ammo(weaponnr&) > 0 THEN mattype& = mattype& OR channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).targets
  NEXT weaponnr&

  GetMaterialType& = mattype&
END FUNCTION



'Ermittelt wieviel Reparaturmaterial ein Reparaturfahrzeug verfügbar hat
FUNCTION GetUnitRepairMaterial&(chnr&, unitnr&)
  LOCAL unittp&, weaponnr&, tg&, ammo&, fuel&, material&

  unittp& = channels(chnr&).units(unitnr&).unittype
  FOR weaponnr& = 0 TO 3
    tg& = channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).targets
    IF (tg& AND %WP_AMMO) <> 0 THEN ammo& = ammo&+channels(chnr&).units(unitnr&).ammo(weaponnr&)
    IF (tg& AND %WP_FUEL) <> 0 THEN fuel& = fuel&+channels(chnr&).units(unitnr&).ammo(weaponnr&)
    IF (tg& AND %WP_MATERIAL) <> 0 THEN material& = material&+channels(chnr&).units(unitnr&).ammo(weaponnr&)
  NEXT weaponnr&

  GetUnitRepairMaterial& = MAX&(material&, MIN&(ammo&, fuel&))
END FUNCTION



'Ermittelt wieviel Munition ein Füllfahrzeug verfügbar hat
FUNCTION GetUnitRechargeMaterial&(chnr&, unitnr&)
  LOCAL unittp&, weaponnr&, tg&, ammo&

  unittp& = channels(chnr&).units(unitnr&).unittype
  FOR weaponnr& = 0 TO 3
    tg& = channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).targets
    IF (tg& AND %WP_AMMO) <> 0 THEN ammo& = ammo&+channels(chnr&).units(unitnr&).ammo(weaponnr&)
  NEXT weaponnr&

  GetUnitRechargeMaterial& = ammo&
END FUNCTION



'Bestimmt die Art eines Versorgungsfahrzeugs
FUNCTION GetWeaponTypeByUnit&(chnr&, unitnr&)
  GetWeaponTypeByUnit& = GetWeaponTypeByClass&(chnr&, channels(chnr&).units(unitnr&).unittype)
END FUNCTION



'Bestimmt die Art eines Versorgungsfahrzeugs
FUNCTION GetWeaponTypeByClass&(BYVAL chnr&, BYVAL unittp&)
  LOCAL weaponnr&, tg&, r&

  FOR weaponnr& = 0 TO 3
    IF channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).ammo > 0 THEN r& = r& OR channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).targets
  NEXT weaponnr&

  GetWeaponTypeByClass& = r&
END FUNCTION



'Einheit mit Kampagnenpunkten trainieren
SUB TrainCampaign(chnr&, unitnr&, v&)
  LOCAL a$$, unittp&, shopnr&, shopowner&

  unittp& = channels(chnr&).units(unitnr&).unittype
  channels(chnr&).units(unitnr&).experience = MIN&(%MAX_EXPERIENCE, channels(chnr&).units(unitnr&).experience+v&)
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND (%UCF_PLANE OR %UCF_SHIP)) = 0 THEN channels(chnr&).campaign.groundscore = MAX&(0, channels(chnr&).campaign.groundscore-v&)
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN channels(chnr&).campaign.waterscore = MAX&(0, channels(chnr&).campaign.waterscore-v&)
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN channels(chnr&).campaign.airscore = MAX&(0, channels(chnr&).campaign.airscore-v&)
  CALL UnitDone(chnr&, unitnr&)
  IF v& = 1 THEN
    CALL BILog(words$$(%WORD_CAMPAIGN_TRAINING1), 0)
  ELSE
    a$$ = words$$(%WORD_CAMPAIGN_TRAINING2)
    REPLACE "%" WITH FORMAT$(v&) IN a$$
    CALL BILog(a$$, 0)
  END IF

  'Replay aufzeichnen
  shopnr& = UnitIsInShop&(chnr&, unitnr&)
  IF shopnr& >= 0 THEN
    shopowner& = channels(chnr&).shops(shopnr&).owner
    CALL AddReplay(chnr&, %REPLAY_SHOPACTION, shopowner&, MKL$(shopnr&)+MKL$(%SHOPACTION_TRAINCAMPAIGN_ONE_LEVEL+v&-1)+MKL$(unitnr&)+PEEK$(VARPTR(channels(chnr&).shops(shopnr&)), SIZEOF(TShop)))
  END IF
END SUB



'Einheit im Shop trainieren
SUB TrainInShop(chnr&, shopnr&, unitnr&)
  LOCAL e$$, shoptype&, shopowner&, unittp&, maxtraining&

  shoptype& = channels(chnr&).shops(shopnr&).shoptype
  shopowner& = channels(chnr&).shops(shopnr&).owner

  IF shoptype& = %SHOPTYPE_ACADEMY THEN
    '+2 Erfahrungspunkte für 4 Energie
    gamedataChanged& = 1
    IF channels(chnr&).player(shopowner&).energy < 4 THEN EXIT SUB
    channels(chnr&).units(unitnr&).experience = MIN&(%MAX_EXPERIENCE, channels(chnr&).units(unitnr&).experience+2)
    CALL UnitDone(chnr&, unitnr&)
    channels(chnr&).player(shopowner&).energy = channels(chnr&).player(shopowner&).energy-4
    'Replay aufzeichnen
    CALL AddReplay(chnr&, %REPLAY_SHOPACTION, shopowner&, MKL$(shopnr&)+MKL$(%SHOPACTION_TRAIN)+MKL$(unitnr&)+PEEK$(VARPTR(channels(chnr&).shops(shopnr&)), SIZEOF(TShop)))
  ELSE
    '+1 Erfahrungspunkt für 1 Kampagnenpunkt (nur Einzelspieler-Modus)
    IF (shoptype& = %SHOPTYPE_HQ OR shoptype& = %SHOPTYPE_AIRPORT OR shoptype& = %SHOPTYPE_HARBOUR OR shoptype& = %SHOPTYPE_FACTORY) THEN
      unittp& = channels(chnr&).units(unitnr&).unittype
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND (%UCF_PLANE OR %UCF_SHIP)) = 0 THEN maxtraining& = channels(chnr&).campaign.groundscore
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN maxtraining& = channels(chnr&).campaign.waterscore
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN maxtraining& = channels(chnr&).campaign.airscore
      maxtraining& = MIN&(maxtraining&, %MAX_EXPERIENCE-channels(chnr&).units(unitnr&).experience)
      SELECT CASE maxtraining&
      CASE 1: CALL TrainCampaign(chnr&, unitnr&, 1)
      CASE 2: e$$ = words$$(%WORD_TRAINING_ONE_LEVEL)+CHR$(13)+words$$(%WORD_TRAINING_TWO_LEVELS)+CHR$(13)+words$$(%WORD_CANCEL)
      CASE 3: e$$ = words$$(%WORD_TRAINING_ONE_LEVEL)+CHR$(13)+words$$(%WORD_TRAINING_TWO_LEVELS)+CHR$(13)+words$$(%WORD_TRAINING_THREE_LEVELS)+CHR$(13)+words$$(%WORD_CANCEL)
      CASE 4 TO 11: e$$ = words$$(%WORD_TRAINING_ONE_LEVEL)+CHR$(13)+words$$(%WORD_TRAINING_TWO_LEVELS)+CHR$(13)+words$$(%WORD_TRAINING_THREE_LEVELS)+CHR$(13)+words$$(%WORD_TRAINING_FOUR_LEVELS)+CHR$(13)+words$$(%WORD_CANCEL)
      END SELECT
      IF e$$ <> "" THEN
        buttonShopBuild.Visible = 0
        buttonShopMove.Visible = 0
        buttonShopRefuel.Visible = 0
        buttonShopRepair.Visible = 0
        buttonShopTrain.Visible = 0
        buttonClose.Visible = 0
        CALL OpenMenu(%PHASE_CAMPAIGNTRAINING, 0, words$$(%WORD_TRAINING_WITH_CAMPAIGNPOINTS), e$$)
        menuOpenTime! = gametime!
      END IF
    END IF
  END IF
END SUB



'Einheit im Shop reparieren
SUB RepairInShop(chnr&, shopnr&, unitnr&)
  LOCAL shopowner&, unittp&, missinghitpoints&, maxrepair&, xp&

  gamedataChanged& = 1
  shopowner& = channels(chnr&).shops(shopnr&).owner
  unittp& = channels(chnr&).units(unitnr&).unittype

  'Einheit reparieren
  missinghitpoints& = channelsnosave(chnr&).unitclasses(unittp&).groupsize-channels(chnr&).units(unitnr&).groupsize
  maxrepair& = MIN&(channels(chnr&).player(shopowner&).energy, channels(chnr&).shops(shopnr&).material)
  maxrepair& = MIN&(maxrepair&, missinghitpoints&)
  IF maxrepair& = 0 THEN EXIT SUB
  channels(chnr&).units(unitnr&).groupsize = channels(chnr&).units(unitnr&).groupsize+maxrepair&
  channels(chnr&).shops(shopnr&).material = channels(chnr&).shops(shopnr&).material-maxrepair&
  channels(chnr&).player(shopowner&).energy = channels(chnr&).player(shopowner&).energy-maxrepair&

  'Erfahrungspunkte der Einheit reduzieren
  xp& = channels(chnr&).units(unitnr&).experience-INT(maxrepair&/2)
  IF (maxrepair& AND 1) = 1 AND (channels(chnr&).units(unitnr&).groupsize AND 1) = 1 THEN xp& = xp&-1
  channels(chnr&).units(unitnr&).experience = MAX&(1, xp&)
  CALL SetShopAnimation(unitnr&, %MAPOVERLAY_REPAIR)

  'Replay aufzeichnen
  CALL AddReplay(chnr&, %REPLAY_SHOPACTION, shopowner&, MKL$(shopnr&)+MKL$(%SHOPACTION_REPAIR)+MKL$(unitnr&)+PEEK$(VARPTR(channels(chnr&).shops(shopnr&)), SIZEOF(TShop)))
END SUB



'Einheit durch Versorgungsfahrzeug reparieren
SUB Repair(chnr&, supportunitnr&, targetunitnr&)
  LOCAL plnr&, availmat&, maxrepair&, supportunittp&, targetunittp&, weaponnr&, tg&, n&, xp&, targetunits&()

  'Reparaturmaterial ermitteln
  plnr& = channels(chnr&).units(supportunitnr&).owner
  gamedataChanged& = 1
  availmat& = GetUnitRepairMaterial&(chnr&, supportunitnr&)

  'Schaden ermitteln
  targetunittp& = channels(chnr&).units(targetunitnr&).unittype
  maxrepair& = MIN&(availmat&, channelsnosave(chnr&).unitclasses(targetunittp&).groupsize-channels(chnr&).units(targetunitnr&).groupsize)
  IF maxrepair& = 0 THEN EXIT SUB

  'Einheit reparieren
  channels(chnr&).units(targetunitnr&).groupsize = channels(chnr&).units(targetunitnr&).groupsize+maxrepair&

  'Erfahrungspunkte der Einheit reduzieren
  xp& = channels(chnr&).units(targetunitnr&).experience-INT(maxrepair&/2)
  IF (maxrepair& AND 1) = 1 AND (channels(chnr&).units(targetunitnr&).groupsize AND 1) = 1 THEN xp& = xp&-1
  channels(chnr&).units(targetunitnr&).experience = MAX&(1, xp&)

  'Reparaturmaterial verbrauchen
  supportunittp& = channels(chnr&).units(supportunitnr&).unittype
  FOR weaponnr& = 0 TO 3
    tg& = channelsnosave(chnr&).unitclasses(supportunittp&).weapons(weaponnr&).targets
    IF (tg& AND %WP_MATERIAL) <> 0 AND channels(chnr&).units(supportunitnr&).ammo(weaponnr&) > 0 THEN
      n& = MIN&(maxrepair&, channels(chnr&).units(supportunitnr&).ammo(weaponnr&))
      channels(chnr&).units(supportunitnr&).ammo(weaponnr&) = channels(chnr&).units(supportunitnr&).ammo(weaponnr&)-n&
      maxrepair& = maxrepair&-n&
    END IF
  NEXT weaponnr&

  'falls Reparaturmaterial nicht ausreichend war, dann Munition+Treibstoff verbrauchen
  IF maxrepair& > 0 THEN
    FOR weaponnr& = 0 TO 3
      tg& = channelsnosave(chnr&).unitclasses(supportunittp&).weapons(weaponnr&).targets
      IF (tg& AND %WP_AMMO) <> 0 AND channels(chnr&).units(supportunitnr&).ammo(weaponnr&) >= maxrepair& THEN channels(chnr&).units(supportunitnr&).ammo(weaponnr&) = channels(chnr&).units(supportunitnr&).ammo(weaponnr&)-maxrepair&
      IF (tg& AND %WP_FUEL) <> 0 AND channels(chnr&).units(supportunitnr&).ammo(weaponnr&) >= maxrepair& THEN channels(chnr&).units(supportunitnr&).ammo(weaponnr&) = channels(chnr&).units(supportunitnr&).ammo(weaponnr&)-maxrepair&
    NEXT weaponnr&
  END IF

  'Replay aufzeichnen
  CALL AddReplay(chnr&, %REPLAY_UNITACTION, plnr&, _
    MKL$(supportunitnr&)+MKI$(%UNITACTION_REPAIR)+MKI$(channels(chnr&).units(targetunitnr&).xpos)+MKI$(channels(chnr&).units(targetunitnr&).ypos)+PEEK$(VARPTR(channels(chnr&).units(supportunitnr&)), SIZEOF(TUnit)))

  'falls Versorgungsfahrzeug nicht bewegt wurde nach weiteren Zielen suchen
  CALL ClearTargets(chnr&, plnr&)
  channels(chnr&).units(supportunitnr&).flags = channels(chnr&).units(supportunitnr&).flags OR %US_ACTIONUSED
  IF (channels(chnr&).units(supportunitnr&).flags AND %US_MOVED) = 0 THEN
    CALL SetPhase(chnr&, plnr&, %PHASE_UNITSELECTED)
    IF GetTargets&(chnr&, supportunitnr&, 2, 0, targetunits&()) = 0 THEN CALL UnselectUnit(chnr&, channels(chnr&).units(supportunitnr&).owner)
  ELSE
    CALL UnitDone(chnr&, supportunitnr&)
  END IF
END SUB



'Flughöhe ändern
SUB ChangeFlightHeight(chnr&, unitnr&, action&)
  LOCAL plnr&, foundtargets&, targetunits&()

  'Flughöhe ändern
  IF action& = %UNITACTION_ASCEND THEN channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags OR %US_ASCEND
  IF action& = %UNITACTION_DESCEND THEN channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags AND NOT %US_ASCEND
  channels(chnr&).units(unitnr&).zpos = IIF&((channels(chnr&).units(unitnr&).flags AND %US_ASCEND) = 0, %UZ_AIR, %UZ_HIGHAIR)

  'Replay aufzeichnen
  plnr& = channels(chnr&).units(unitnr&).owner
  CALL AddReplay(chnr&, %REPLAY_UNITACTION, plnr&, MKL$(unitnr&)+MKI$(action&)+MKL$(0)+PEEK$(VARPTR(channels(chnr&).units(unitnr&)), SIZEOF(TUnit)))

  'Angriffsziele suchen
  CALL ClearTargets(chnr&, plnr&)
  IF (channels(chnr&).units(unitnr&).flags AND %US_ATTACKED) = 0 THEN foundtargets& = GetTargets&(chnr&, unitnr&, 2, 0, targetunits&())
  IF foundtargets& = 0 THEN CALL UnitDone(chnr&, unitnr&)
  CALL SetPhase(chnr&, plnr&, %PHASE_UNITSELECTED)
END SUB



'Fahrtiefe ändern
SUB ChangeWaterHeight(chnr&, unitnr&, action&)
  LOCAL plnr&, foundtargets&, targetunits&()

  'Fahrtiefe ändern
  IF action& = %UNITACTION_DESCEND THEN channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags OR %US_DIVE
  IF action& = %UNITACTION_ASCEND THEN channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags AND NOT %US_DIVE
  channels(chnr&).units(unitnr&).zpos = IIF&((channels(chnr&).units(unitnr&).flags AND %US_DIVE) = 0, %UZ_WATER, %UZ_UNDERWATER)

  'Replay aufzeichnen
  plnr& = channels(chnr&).units(unitnr&).owner
  CALL AddReplay(chnr&, %REPLAY_UNITACTION, plnr&, MKL$(unitnr&)+MKI$(action&)+MKL$(0)+PEEK$(VARPTR(channels(chnr&).units(unitnr&)), SIZEOF(TUnit)))

  'Angriffsziele suchen
  CALL ClearTargets(chnr&, plnr&)
  IF (channels(chnr&).units(unitnr&).flags AND %US_ATTACKED) = 0 THEN foundtargets& = GetTargets&(chnr&, unitnr&, 2, 0, targetunits&())
  IF foundtargets& = 0 THEN CALL UnitDone(chnr&, unitnr&)
  CALL SetPhase(chnr&, plnr&, %PHASE_UNITSELECTED)
END SUB



'Wandelt einen Menüeintrag in eine Aktion um
FUNCTION GetActionCode&(menucode&)
  LOCAL actioncode&

  SELECT CASE menucode&
  CASE %WORD_REPAIR: actioncode& = %UNITACTION_REPAIR
  CASE %WORD_REFUEL: actioncode& = %UNITACTION_REFUEL
  CASE %WORD_BUILDROAD: actioncode& = %UNITACTION_BUILDROAD
  CASE %WORD_BUILDRAIL: actioncode& = %UNITACTION_BUILDRAIL
  CASE %WORD_BUILDTRENCH: actioncode& = %UNITACTION_BUILDTRENCH
  CASE %WORD_DESTRUCT: actioncode& = %UNITACTION_DESTRUCT
  CASE %WORD_MOVE: actioncode& = %UNITACTION_MOVE
  CASE ELSE: actioncode& = -1
  END SELECT

  GetActionCode& = actioncode&
END FUNCTION



'Aktion mit Versorgungsfahrzeug ausführen
SUB SupportAction(chnr&, unitnr&, action&, target&)
  LOCAL x&, y&, targetunit&, plnr&, targetunits&()

  plnr& = channels(chnr&).units(unitnr&).owner
  x& = target& AND 255
  y& = INT(target&/256)
  targetunit& = channels(chnr&).zone3(x&, y&)
  IF targetunit& < 0 THEN EXIT SUB

  'Aktion ausführen
  SELECT CASE action&
  CASE %UNITACTION_REFUEL
    CALL Refuel(0, unitnr&, targetunit&)
  CASE %UNITACTION_REPAIR
    CALL Repair(0, unitnr&, targetunit&)
  CASE %UNITACTION_MOVE
    CALL MoveUnit(unitnr&, x&, y&)
  END SELECT

  'Replay aufzeichnen
  CALL AddReplay(chnr&, %REPLAY_UNITACTION, plnr&, MKL$(unitnr&)+MKI$(action&)+MKI$(x&)+MKI$(y&)+PEEK$(VARPTR(channels(chnr&).units(unitnr&)), SIZEOF(TUnit)))

  'falls Versorgungsfahrzeug nicht bewegt wurde nach weiteren Zielen suchen
  CALL ClearTargets(chnr&, plnr&)
  channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags OR %US_ACTIONUSED
  IF (channels(chnr&).units(unitnr&).flags AND %US_MOVED) = 0 THEN
    IF (channels(chnr&).info.aimask AND 2^plnr&) = 0 THEN
      CALL SetPhase(chnr&, plnr&, %PHASE_UNITSELECTED)
      IF GetTargets&(chnr&, unitnr&, 2, 0, targetunits&()) = 0 THEN CALL UnselectUnit(chnr&, channels(chnr&).units(unitnr&).owner)
    END IF
  ELSE
    CALL UnitDone(chnr&, unitnr&)
  END IF
END SUB



'Aktion mit Baufahrzeug ausführen
SUB BuildAction(chnr&, unitnr&, action&, target&)
  LOCAL x&, y&, overlay&, roadspr&, unittp&, weaponnr&, plnr&, targetunits&()

  gamedataChanged& = 1
  plnr& = channels(chnr&).units(unitnr&).owner
  x& = target& AND 255
  y& = INT(target&/256)
  overlay& = channels(chnr&).zone2(x&, y&)

  'Aktion ausführen
  SELECT CASE action&
  CASE %UNITACTION_BUILDROAD
    roadspr& = IIF&(channels(chnr&).info.weather = %WEATHER_HEAVYSNOW, %SPRITE_SNOWCOVERED_ROAD, %SPRITE_ROAD)
    channels(chnr&).zone2(x&, y&) = IIF&(overlay& = %SPRITE_RAIL, %SPRITE_ROAD_RAIL, roadspr&)
  CASE %UNITACTION_BUILDRAIL
    channels(chnr&).zone2(x&, y&) = IIF&(overlay& = %SPRITE_ROAD, %SPRITE_ROAD_RAIL, %SPRITE_RAIL)
  CASE %UNITACTION_BUILDTRENCH
    channels(chnr&).zone2(x&, y&) = %SPRITE_TRENCH
  CASE %UNITACTION_DESTRUCT
    channels(chnr&).zone2(x&, y&) = -1
  CASE %UNITACTION_MOVE
    CALL MoveUnit(unitnr&, x&, y&)
    EXIT SUB
  END SELECT

  'Baumaterial verbrauchen
  unittp& = channels(chnr&).units(unitnr&).unittype
  FOR weaponnr& = 0 TO 3
    IF (channelsnosave(chnr&).unitclasses(unittp&).weapons(weaponnr&).targets AND %WP_MATERIAL) <> 0 AND channels(chnr&).units(unitnr&).ammo(weaponnr&) > 0 THEN
      channels(chnr&).units(unitnr&).ammo(weaponnr&) = channels(chnr&).units(unitnr&).ammo(weaponnr&)-1
      EXIT FOR
    END IF
  NEXT weaponnr&

  'Replay aufzeichnen
  CALL AddReplay(chnr&, %REPLAY_UNITACTION, plnr&, MKL$(unitnr&)+MKI$(action&)+MKI$(x&)+MKI$(y&)+PEEK$(VARPTR(channels(chnr&).units(unitnr&)), SIZEOF(TUnit)))

  'falls Baufahrzeug nicht bewegt wurde nach weiteren Zielen suchen
  CALL ClearTargets(chnr&, plnr&)
  channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags OR %US_ACTIONUSED
  IF (channels(chnr&).units(unitnr&).flags AND %US_MOVED) = 0 THEN
    IF (channels(chnr&).info.aimask AND 2^plnr&) = 0 THEN
      CALL SetPhase(chnr&, plnr&, %PHASE_UNITSELECTED)
      IF GetTargets&(chnr&, unitnr&, 2, 0, targetunits&()) = 0 THEN CALL UnselectUnit(chnr&, channels(chnr&).units(unitnr&).owner)
    END IF
  ELSE
    CALL UnitDone(chnr&, unitnr&)
  END IF
END SUB



'Aktion mit Einheit ausführen
SUB UnitAction(chnr&, plnr&, unitnr&, action&, x&, y&)
  LOCAL targetunitnr&, unittp&
  LOCAL targetunits&()

  'prüfen, ob Ziel gültig ist
  targetunitnr& = -1
  CALL ClearTargets(chnr&, plnr&)
  CALL GetTargets&(chnr&, unitnr&, 2, 0, targetunits&())
  unittp& = channels(chnr&).units(unitnr&).unittype

  SELECT CASE action&
  CASE %UNITACTION_REFUEL
    'Treibstoff/Munition der Zieleinheit auffüllen
    IF (channels(chnr&).player(plnr&).targets(x&, y&) AND %TG_REFUEL) <> %TG_REFUEL AND (channels(chnr&).player(plnr&).targets(x&, y&) AND %TG_RECHARGE) <> %TG_RECHARGE THEN EXIT SUB
    targetunitnr& = channels(chnr&).zone3(x&, y&)
    CALL Refuel(chnr&, unitnr&, targetunitnr&)

  CASE %UNITACTION_REPAIR
    'Zieleinheit repaireren
    IF (channels(chnr&).player(plnr&).targets(x&, y&) AND %TG_REPAIR) <> %TG_REPAIR THEN EXIT SUB
    targetunitnr& = channels(chnr&).zone3(x&, y&)
    CALL Repair(chnr&, unitnr&, targetunitnr&)

  CASE %UNITACTION_BUILDROAD, %UNITACTION_BUILDRAIL, %UNITACTION_BUILDTRENCH, %UNITACTION_DESTRUCT
    'Straße/Schiene/Graben bauen oder einreißen
    IF (channels(chnr&).player(plnr&).targets(x&, y&) AND %TG_BUILD) <> %TG_BUILD THEN EXIT SUB
    CALL BuildAction(chnr&, unitnr&, action&, x&+y&*256)
    IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateZoneToClients(chnr&, x&, y&)

  CASE %UNITACTION_ASCEND
    'Aufsteigen
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_CLIMB) <> 0 THEN CALL ChangeFlightHeight(chnr&, unitnr&, %UNITACTION_ASCEND)
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_DIVE) <> 0 THEN CALL ChangeWaterHeight(chnr&, unitnr&, %UNITACTION_ASCEND)

  CASE %UNITACTION_DESCEND
    'Tauchen
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_CLIMB) <> 0 THEN CALL ChangeFlightHeight(chnr&, unitnr&, %UNITACTION_DESCEND)
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_DIVE) <> 0 THEN CALL ChangeWaterHeight(chnr&, unitnr&, %UNITACTION_DESCEND)
  END SELECT

  'Einheiten auf den Clients aktualiseren
  IF gameMode& = %GAMEMODE_SERVER THEN
    CALL UpdateUnitToAllClients(chnr&, unitnr&)
    IF targetunitnr& >= 0 THEN CALL UpdateUnitToAllClients(chnr&, targetunitnr&)
  END IF
END SUB



'DF-Layer aktivieren
SUB ActivateDFLayer(BYVAL chnr&, BYVAL layernr&)
  LOCAL i&, p&, n&, x&, y&, plnr&

  'prüfen, ob Layer-Nummer gültig ist
  IF layernr& < 0 OR layernr& >= %MAXDFLAYER THEN EXIT SUB

  'einzufügende Sätze ermitteln
  p& = channels(chnr&).dfoffsets(layernr&)
  n& = channels(chnr&).dfcount(layernr&)

  'Sätze einfügen
  FOR i& = 1 TO n&
    x& = channels(chnr&).dflayer(p&).xpos
    y& = channels(chnr&).dflayer(p&).ypos
    IF channels(chnr&).dflayer(p&).terrain >= 0 THEN channels(chnr&).zone1(x&, y&) = channels(chnr&).dflayer(p&).terrain
    channels(chnr&).zone2(x&, y&) = channels(chnr&).dflayer(p&).overlay
    IF channels(chnr&).zone3(x&, y&) = -1 AND channels(chnr&).dflayer(p&).unit >= 0 THEN
      channels(chnr&).zone3(x&, y&) = CreateUnitFromCode&(chnr&, x&, y&, channels(chnr&).dflayer(p&).unit)
      IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateUnitToAllClients(chnr&, channels(chnr&).zone3(x&, y&))
    END IF
    IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateZoneToClients(chnr&, x&, y&)
    p& = p&+1
  NEXT i&

  'alle Einheiten zerstören, die jetzt auf Terrain stehen, welches sie nicht betreten können
  CALL RemoveUnitsOnInvalidTerrain(chnr&, 0, channels(chnr&).info.ysize-1)
  IF LocalPlayersTurn& THEN CALL UpdateProgressbar

  'Sicht aktualisieren
  CALL UpdateSightBlock(chnr&)
  CALL UpdateVisionAll(chnr&)
  updateMiniMap& = 1
END SUB



'Allianz ändern
SUB ChangeAlliance(BYVAL chnr&, BYVAL plnr&, BYVAL newally&)
  LOCAL i&, mask&, plmask&

  'Allianz für den Spieler ändern
  plmask& = 2^plnr&
  channels(chnr&).player(plnr&).allymask = newally& OR plmask&
  IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdatePlayerToClients(chnr&, plnr&)

  'Allianzen der anderen Spieler anpassen
  FOR i& = 0 TO %MAXPLAYERS-1
    IF i& <> plnr& THEN
      mask& = channels(chnr&).player(i&).allymask
      IF (newally& AND 2^i&) = 0 THEN mask& = mask& AND NOT plmask& ELSE mask& = mask& OR plmask&
      channels(chnr&).player(i&).allymask = mask&
      IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdatePlayerToClients(chnr&, i&)
    END IF
  NEXT i&

  'Sicht für alle Spieler aktualisieren
  CALL UpdateSightBlock(chnr&)
  CALL UpdateVisionAll(chnr&)
END SUB



'Einheit hat keine Aktionen mehr in dieser Runde
SUB UnitDone(chnr&, unitnr&)
  LOCAL plnr&

  IF (channels(chnr&).units(unitnr&).flags AND %US_DONE) <> 0 THEN EXIT SUB
  channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags OR %US_DONE

  'bei menschlichen Spielern, Bewegungszähler erhöhen
  plnr& = channels(chnr&).info.activeplayer
  IF (channels(chnr&).info.aimask AND 2^plnr&) = 0 THEN
    channels(chnr&).info.movement = channels(chnr&).info.movement+1
    IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateInfoToClients(chnr&, VARPTR(channels(chnr&).info.movement)-VARPTR(channels(chnr&).info), MKL$(channels(chnr&).info.movement))
    CALL CheckActions(chnr&)
  END IF
  IF LocalPlayersTurn& THEN CALL UpdateProgressbar
END SUB



'Berechnet die Siegpunkte für einen Spieler
SUB CalculateScore(BYVAL chnr&, BYVAL plnr&, BYREF scoreground&, BYREF scoreair&, BYREF scorewater&, BYREF bestunits$)
  LOCAL i&, unitnr&, unittp&, xp&, maxrepair&, missionnr&, turn&, totalscore&, sc$
  LOCAL xpbyclass&(), classids&()
  DIM xpbyclass&(channelsnosave(chnr&).nunitclasses-1), classids&(channelsnosave(chnr&).nunitclasses-1)

  scoreground& = 0
  scoreair& = 0
  scorewater& = 0

  'Gesamtpunkte ermitteln
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = plnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 THEN
      unittp& = channels(chnr&).units(unitnr&).unittype

      'Erfahrungspunkte der Einheit ermitteln (als sei die Einheit vollständig repariert worden)
      maxrepair& = channelsnosave(chnr&).unitclasses(unittp&).groupsize-channels(chnr&).units(unitnr&).groupsize
      xp& = channels(chnr&).units(unitnr&).experience-INT(maxrepair&/2)
      IF (maxrepair& AND 1) = 1 AND (channels(chnr&).units(unitnr&).groupsize AND 1) = 1 THEN xp& = xp&-1
      xp& = MAX&(1, xp&)
      xpbyclass&(unittp&) = MAX&(xpbyclass&(unittp&), xp&)
      classids&(unittp&) = unittp&

      'Einheitenklasse (Boden/Luft/Wasser) ermitteln
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN
        scoreair& = scoreair&+xp&-1
      ELSE
        IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN
          scorewater& = scorewater&+xp&-1
        ELSE
          scoreground& = scoreground&+xp&-1
        END IF
      END IF
    END IF
  NEXT unitnr&

  'beste Einheiten pro Klasse nach Erfahrung sortiert zurückliefern
  ARRAY SORT xpbyclass&(0) FOR channelsnosave(chnr&).nunitclasses, TAGARRAY classids&(), DESCEND
  bestunits$ = ""
  FOR i& = 0 TO channelsnosave(chnr&).nunitclasses-1
    IF xpbyclass&(i&) > 0 THEN bestunits$ = bestunits$+CHR$(classids&(i&))+CHR$(xpbyclass&(i&))
  NEXT i&

  'Highscore aktualisieren falls die neuen Punkte höher sind als die bisherigen
  IF channels(0).info.state >= %CHANNELSTATE_VICTORY AND channels(0).info.state <= %CHANNELSTATE_VICTORYBONUS AND replayMode&(chnr&) < %REPLAYMODE_PLAY AND channels(chnr&).info.turn < 9999 THEN
    'Highscore lokal speichern
    missionnr& = channels(chnr&).info.currentmission
    sc$ = mapscore$(missionnr&)
    turn& = 99999
    IF sc$ <> "" THEN
      turn& = VAL(PARSE$(sc$, ";", 1))
      totalscore& = VAL(PARSE$(sc$, ";", 2))+VAL(PARSE$(sc$, ";", 3))+VAL(PARSE$(sc$, ";", 4))
    END IF
    IF channels(chnr&).info.turn < turn& OR (channels(chnr&).info.turn = turn& AND scoreground&+scorewater&+scoreair& > totalscore&) THEN
      mapscore$(missionnr&) = FORMAT$(channels(chnr&).info.turn)+";"+FORMAT$(scoreground&)+";"+FORMAT$(scorewater&)+";"+FORMAT$(scoreair&)
    END IF

    'Highscore global auf Server speichern
    highscorePacket$ = MKL$(missionnr&)+MKL$(channels(chnr&).info.turn)+MKL$(scoreground&)+MKL$(scorewater&)+MKL$(scoreair&)+CHR$(channels(chnr&).info.difficulty)+MKL$(channels(chnr&).info.checksum)
    IF hClientSocket& < 0 THEN
      CALL ConnectToServer&(StringToIP&($HIGHSCORESERVER))
    ELSE
      CALL SendHighScore
    END IF

  END IF
END SUB



'Treibstoff bei allen in der Luft befindlichen Flugzeugen verbrauchen
SUB CheckAirUnitFuel(chnr&)
  LOCAL unitnr&, unittp&

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF UnitIsAlive&(chnr&, unitnr&) <> 0 AND UnitIsInShop&(chnr&, unitnr&) = -1 THEN
      unittp& = channels(chnr&).units(unitnr&).unittype
      IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN
        IF channels(chnr&).units(unitnr&).fuel = 0 THEN
          'Absturz
          CALL KillUnit(chnr&, unitnr&, -1, %KILLREASON_NOFUEL, 0)
        ELSE
          channels(chnr&).units(unitnr&).fuel = channels(chnr&).units(unitnr&).fuel-1
        END IF
        IF gameMode& = %GAMEMODE_SERVER THEN CALL UpdateUnitToAllClients(chnr&, unitnr&)
      END IF
    END IF
  NEXT unitnr&
END SUB



'Neue Runde initialisieren
SUB InitRound(chnr&)
  LOCAL a$$, unitnr&, shopnr&, owner&, statuskeep&

  'Rundenzähler erhöhen
  channels(chnr&).info.turn = channels(chnr&).info.turn+1
  channels(chnr&).info.movement = 0
  CALL SetPhaseAll(chnr&, %PHASE_NONE, -1)
  a$$ = words$$(%WORD_TURN_NUMBER)
  REPLACE "%" WITH FORMAT$(channels(chnr&).info.turn+1) IN a$$
  CALL AddProtocol("$TITLE$"+a$$)

  'Status aller Einheiten zurücksetzen
  statuskeep& = %US_ASCEND OR %US_DIVE
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF UnitIsAlive&(chnr&, unitnr&) <> 0 THEN
      channels(chnr&).units(unitnr&).flags = channels(chnr&).units(unitnr&).flags AND statuskeep&
      channels(chnr&).units(unitnr&).moved = 0
      channels(chnr&).units(unitnr&).xposold = channels(chnr&).units(unitnr&).xpos
      channels(chnr&).units(unitnr&).yposold = channels(chnr&).units(unitnr&).ypos
    END IF
  NEXT unitnr&

  'Energie und Material in allen Shops produzieren
  FOR shopnr& = 0 TO channels(chnr&).info.nshops-1
    owner& = channels(chnr&).shops(shopnr&).owner
    IF owner& < 6 AND channels(chnr&).shops(shopnr&).shoptype > 0 THEN
      channels(chnr&).player(owner&).energy = channels(chnr&).player(owner&).energy+channels(chnr&).shops(shopnr&).eplus
      channels(chnr&).shops(shopnr&).material = channels(chnr&).shops(shopnr&).material+channels(chnr&).shops(shopnr&).mplus
    END IF
  NEXT shopnr&

  IF gameMode& <> %GAMEMODE_CLIENT THEN
    'Treibstoff bei allen in der Luft befindlichen Flugzeugen verbrauchen
    CALL CheckAirUnitFuel(chnr&)

    'Aktionen und Siegbedingungen prüfen
    CALL CheckActions(chnr&)
    CALL CheckVictoryConditions(chnr&)
  END IF

  'Reifenspuren entfernen
  IF gameMode& <> %GAMEMODE_SERVER THEN POKE$ VARPTR(trails?(0,0)), STRING$(65536, 0)

  IF gameMode& = %GAMEMODE_SERVER THEN CALL ResetPlayerState(chnr&)
END SUB



'Kontrolle an nächsten Spieler weitergeben
SUB EndTurn(chnr&)
  LOCAL oldplnr&, plnr&, team&, p&, i&

  'Zug finalisieren
  oldplnr& = channels(chnr&).info.activeplayer
  channels(chnr&).player(oldplnr&).selectedunit = -1
  CALL ClearTargets(chnr&, oldplnr&)
  plnr& = oldplnr&

  'alle noch verbleibenden Actions für diesen Zug ausführen
  channels(chnr&).info.movement = 9999
  DO
    p& = channels(chnr&).info.actionposition
    CALL CheckActions(chnr&)
  LOOP UNTIL p& = channels(chnr&).info.actionposition

  IF gameMode& = %GAMEMODE_SERVER THEN
    'Kontrolle an nächstes noch lebendes Team weitergeben
    i& = 0
    team& = channels(chnr&).info.activeteam
    DO
      i& = i&+1
      team& = team&+1
      IF team& > 6 THEN
        team& = 1
        CALL InitRound(chnr&)
      END IF
    LOOP UNTIL TeamIsAlive&(chnr&, team&) > 0 OR i& = 7
    IF i& = 7 THEN CALL ServerGameOver(chnr&, -1)
    channels(chnr&).info.activeteam = team&
    FOR plnr& = 0 TO %MAXPLAYERS-1
      IF (channels(chnr&).info.aliveplayers AND 2^plnr&) <> 0 AND channels(chnr&).player(plnr&).team = team& THEN
        channels(chnr&).info.activeplayer = plnr&
        EXIT FOR
      END IF
    NEXT plnr&
  ELSE
    'Kontrolle an nächsten noch lebenden Spieler weitergeben
    DO
      plnr& = plnr&+1
      IF plnr& = 6 THEN
        plnr& = 0
        CALL InitRound(chnr&)
      END IF
    LOOP UNTIL (channels(chnr&).info.aliveplayers AND 2^plnr&) <> 0
    channels(chnr&).info.activeplayer = plnr&
    IF plnr& = localPlayerNr& THEN
      CALL ClearTargets(0, localPlayerNr&)
      CALL EnableAllMenuButtons
    END IF
    CALL UpdateProgressbar
  END IF

  'Replay aufzeichnen
  CALL AddReplay(chnr&, %REPLAY_ENDTURN, oldplnr&, CHR$(plnr&)+CHR$(team&))
END SUB



'Liefert den kompletten Inhalt eines Channels zurück
FUNCTION GetFullChannelData$(chnr&)
  LOCAL i&, y&, p&, q&, n&, bytesperrow&
  LOCAL a$, info$, z$, explored$, unit$, shops$, actions$, players$, campaign$, df$

  'Channelinfo (ohne die Shopnamen der ungenutzten Shops)
  info$ = PEEK$(VARPTR(channels(chnr&).info), SIZEOF(TChannelInfo)-(%MAXSHOPS-channels(chnr&).info.nshops)*32)

  'Karte
  z$ = STRING$(channels(chnr&).info.xsize*channels(chnr&).info.ysize*6, 0)
  q& = 1
  bytesperrow& = channels(chnr&).info.xsize*2
  FOR i& = 1 TO 3
    SELECT CASE i&
    CASE 1: p& = VARPTR(channels(chnr&).zone1(0, 0))
    CASE 2: p& = VARPTR(channels(chnr&).zone2(0, 0))
    CASE 3: p& = VARPTR(channels(chnr&).zone3(0, 0))
    END SELECT
    FOR y& = 0 TO channels(chnr&).info.ysize-1
      MID$(z$, q&, bytesperrow&) = PEEK$(p&, bytesperrow&)
      p& = p&+512
      q& = q&+bytesperrow&
    NEXT y&
  NEXT i&

  'erforschte Karte
  explored$ = STRING$(channels(chnr&).info.xsize*channels(chnr&).info.ysize, 0)
  p& = VARPTR(channels(chnr&).explored(0, 0))
  q& = 1
  bytesperrow& = channels(chnr&).info.xsize
  FOR y& = 0 TO channels(chnr&).info.ysize-1
    MID$(explored$, q&, bytesperrow&) = PEEK$(p&, bytesperrow&)
    p& = p&+256
    q& = q&+bytesperrow&
  NEXT y&

  'Einheiten
  unit$ = PEEK$(VARPTR(channels(chnr&).units(0)), SIZEOF(TUnit)*channels(chnr&).info.nunits)

  'Shops
  shops$ = PEEK$(VARPTR(channels(chnr&).shops(0)), SIZEOF(TShop)*channels(chnr&).info.nshops)

  'Actions
  actions$ = PEEK$(VARPTR(channels(chnr&).actions(0)), SIZEOF(TAction)*channels(chnr&).info.nactions)

  'Spieler (ohne targets Array)
  FOR i& = 0 TO %MAXPLAYERS-1
    players$ = players$+PEEK$(VARPTR(channels(chnr&).player(i&)), SIZEOF(TPlayer)-65536)
  NEXT i&

  'Kampagne
  campaign$ = PEEK$(VARPTR(channels(chnr&).campaign), SIZEOF(TCampaign))

  'DF-Layer
  FOR i& = 0 TO %MAXDFLAYER-1
    p& = channels(chnr&).dfoffsets(i&)
    n& = channels(chnr&).dfcount(i&)
    df$ = df$+MKI$(p&)+MKI$(n&)+PEEK$(VARPTR(channels(chnr&).dflayer(p&)), SIZEOF(TDFData)*n&)
  NEXT i&

  GetFullChannelData$ = MKL$(LEN(info$))+MKL$(LEN(z$))+MKL$(LEN(explored$))+MKL$(LEN(unit$))+MKL$(LEN(shops$))+MKL$(LEN(actions$))+MKL$(LEN(players$))+MKL$(LEN(campaign$))+MKL$(LEN(df$))+info$+z$+explored$+unit$+shops$+actions$+players$+campaign$+df$
END FUNCTION



'Erstellt einen kompletten Channels aus einem String
FUNCTION SetFullChannelData&(chnr&, a$)
  LOCAL i&, y&, p&, q&, n&, bytesperrow&, nshops&
  LOCAL headerlen&, infolen&, zonelen&, exploredlen&, unitlen&, shoplen&, actionlen&, playerlen&, campaignlen&, dflen&
  LOCAL chinfo AS TChannelInfo, testinfo AS TChannelInfo

  'Längen auslesen und validieren
  headerlen& = 36
  infolen& = CVL(a$, 1)
  zonelen& = CVL(a$, 5)
  exploredlen& = CVL(a$, 9)
  unitlen& = CVL(a$, 13)
  shoplen& = CVL(a$, 17)
  actionlen& = CVL(a$, 21)
  playerlen& = CVL(a$, 25)
  campaignlen& = CVL(a$, 29)
  dflen& = CVL(a$, 33)
  POKE$ VARPTR(chinfo), MID$(a$, headerlen&+1, SIZEOF(TChannelInfo))
  nshops& = chinfo.nshops
  IF infolen& <> SIZEOF(TChannelInfo)-(%MAXSHOPS-nshops&)*32 THEN EXIT FUNCTION
  IF zonelen& <> chinfo.xsize*chinfo.ysize*6 THEN EXIT FUNCTION
  IF exploredlen& <> chinfo.xsize*chinfo.ysize THEN EXIT FUNCTION
  IF unitlen& <> SIZEOF(TUnit)*chinfo.nunits THEN EXIT FUNCTION
  IF shoplen& <> SIZEOF(TShop)*chinfo.nshops THEN EXIT FUNCTION
  IF actionlen& <> SIZEOF(TAction)*chinfo.nactions THEN EXIT FUNCTION
  IF playerlen& <> (SIZEOF(TPlayer)-65536)*%MAXPLAYERS THEN EXIT FUNCTION
  IF campaignlen& <> SIZEOF(TCampaign) THEN EXIT FUNCTION
  IF dflen& > SIZEOF(TDFData)*%MAXDFRECORDS+4*%MAXDFLAYER THEN EXIT FUNCTION

  'Channelinfo
  POKE$ VARPTR(channels(chnr&).info), MID$(a$, headerlen&+1, infolen&)

  'Karte
  p& = headerlen&+infolen&+1
  bytesperrow& = channels(chnr&).info.xsize*2
  FOR i& = 1 TO 3
    SELECT CASE i&
    CASE 1: q& = VARPTR(channels(chnr&).zone1(0, 0))
    CASE 2: q& = VARPTR(channels(chnr&).zone2(0, 0))
    CASE 3: q& = VARPTR(channels(chnr&).zone3(0, 0))
    END SELECT
    FOR y& = 0 TO channels(chnr&).info.ysize-1
      POKE$ q&, MID$(a$, p&, bytesperrow&)
      p& = p&+bytesperrow&
      q& = q&+512
    NEXT y&
  NEXT i&

  'erforschte Karte
  q& = VARPTR(channels(chnr&).explored(0, 0))
  bytesperrow& = channels(chnr&).info.xsize
  FOR y& = 0 TO channels(chnr&).info.ysize-1
    POKE$ q&, MID$(a$, p&, bytesperrow&)
    p& = p&+bytesperrow&
    q& = q&+256
  NEXT y&

  'Einheiten
  POKE$ VARPTR(channels(chnr&).units(0)), MID$(a$, headerlen&+infolen&+zonelen&+exploredlen&+1, unitlen&)

  'Shops
  POKE$ VARPTR(channels(chnr&).shops(0)), MID$(a$, headerlen&+infolen&+zonelen&+exploredlen&+unitlen&+1, shoplen&)

  'Actions
  POKE$ VARPTR(channels(chnr&).actions(0)), MID$(a$, headerlen&+infolen&+zonelen&+exploredlen&+unitlen&+shoplen&+1, actionlen&)

  'Spieler
  q& = INT(playerlen&/%MAXPLAYERS)
  p& = headerlen&+infolen&+zonelen&+exploredlen&+unitlen&+shoplen&+actionlen&+1
  FOR i& = 0 TO %MAXPLAYERS-1
    POKE$ VARPTR(channels(chnr&).player(i&)), MID$(a$, p&, q&)+STRING$(65536, 0)
    p& = p&+q&
  NEXT i&

  'Kampagne
  POKE$ VARPTR(channels(chnr&).campaign), MID$(a$, headerlen&+infolen&+zonelen&+exploredlen&+unitlen&+shoplen&+actionlen&+playerlen&+1, campaignlen&)

  'DF-Layer
  p& = headerlen&+infolen&+zonelen&+exploredlen&+unitlen&+shoplen&+actionlen&+playerlen&+campaignlen&+1
  FOR i& = 0 TO %MAXDFLAYER-1
    q& = CVI(a$, p&)
    n& = CVI(a$, p&+2)
    channels(chnr&).dfoffsets(i&) = q&
    channels(chnr&).dfcount(i&) = n&
    POKE$ VARPTR(channels(chnr&).dflayer(q&)), MID$(a$, p&+4, SIZEOF(TDFData)*n&)
    p& = p&+4+SIZEOF(TDFData)*n&
  NEXT i&

  SetFullChannelData& = 1
END FUNCTION



'REPLACE Befehl als Fumktion
FUNCTION ReplaceString$$(BYVAL mainstring$$, BYVAL matchstring$$, BYVAL newstring$$)
  REPLACE matchstring$$ WITH newstring$$ IN mainstring$$
  ReplaceString$$ = mainstring$$
END FUNCTION



'Channel-Daten prüfen und ggf. reparieren
SUB CheckAndFixChannelData(chnr&)
  LOCAL unitnr&, unittp&, x&, y&, unitatmap&, shopnr&, slotnr&, cargo&

  'Einheitenreferenzen entfernen, die auf keine Einheit mehr zeigen
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF UnitIsAlive&(chnr&, unitnr&) <> 0 THEN
      x& = channels(chnr&).units(unitnr&).xpos
      y& = channels(chnr&).units(unitnr&).ypos
      IF x& < 0 OR x& >= channels(chnr&).info.xsize OR y& < 0 OR y& >= channels(chnr&).info.ysize THEN
        'Einheit ist außerhalb der Karte
        channels(chnr&).units(unitnr&).groupsize = 0
        CALL BILog(ReplaceString$(words$$(%WORD_FIXMAP_UNITOUTSIDEMAP), "%", FORMAT$(unitnr&)), 8)
      ELSE
        unitatmap& = channels(chnr&).zone3(x&, y&)
        IF unitatmap& < -1 THEN
          'prüfen, ob sich Einheit wirklich im Shop befindet
          shopnr& = -2-unitatmap&
          FOR slotnr& = 0 TO 15
            IF channels(chnr&).shops(shopnr&).content(slotnr&) = unitnr& THEN EXIT FOR
          NEXT slotnr&
          IF slotnr& > 15 THEN
            channels(chnr&).units(unitnr&).groupsize = 0
            CALL BILog(ReplaceString$(ReplaceString$(words$$(%WORD_FIXMAP_UNITNOTFOUNDINSHOP), "%", FORMAT$(unitnr&)), "&", channels(chnr&).info.shopnames(shopnr&)), 8)
          END IF
        ELSE
          'prüfen, ob sich an dieser Stelle auf der Karte die Einheitenreferenz befindet
          IF unitatmap& <> unitnr& THEN
            IF unitatmap& >= 0 THEN
              FOR slotnr& = 0 TO 7
                IF channels(chnr&).units(unitatmap&).transportcontent(slotnr&) = unitnr& THEN EXIT FOR
              NEXT slotnr&
              IF slotnr& > 7 THEN
                channels(chnr&).units(unitnr&).groupsize = 0
                CALL BILog(ReplaceString$(words$$(%WORD_FIXMAP_UNITNOTFOUNDONMAP), "%", FORMAT$(unitnr&)), 8)
              END IF
            ELSE
              channels(chnr&).units(unitnr&).groupsize = 0
              CALL BILog(ReplaceString$(words$$(%WORD_FIXMAP_UNITNOTFOUNDONMAP), "%", FORMAT$(unitnr&)), 8)
            END IF
          END IF
        END IF
      END IF
    END IF
  NEXT unitnr&

  'Einheitenzähler optimieren
  WHILE channels(chnr&).info.nunits > 0 AND UnitIsAlive&(chnr&, channels(chnr&).info.nunits-1) = 0
    channels(chnr&).info.nunits = channels(chnr&).info.nunits-1
  WEND

  'Einheiten von der Karte entfernen, deren Referenz ungültig ist
  FOR y& = 0 TO channels(chnr&).info.ysize-1
    FOR x& = 0 TO channels(chnr&).info.xsize-1
      unitnr& = channels(chnr&).zone3(x&, y&)
      IF unitnr& >= 0 THEN
        IF unitnr& >= channels(chnr&).info.nunits THEN
          'Referenz ist größer als Einheitenzähler
          channels(chnr&).zone3(x&, y&) = -1
          CALL BILog(ReplaceString$(words$$(%WORD_FIXMAP_INVALIDUNITREF), "%", FORMAT$(unitnr&)), 8)
        ELSE
          'prüfen, ob Einheit noch lebt
          IF UnitIsAlive&(chnr&, unitnr&) = 0 THEN
            channels(chnr&).zone3(x&, y&) = -1
            CALL BILog(ReplaceString$(words$$(%WORD_FIXMAP_DEADUNIT), "%", FORMAT$(unitnr&)), 8)
          END IF
        END IF
      END IF
    NEXT x&
  NEXT y&

  'Einheiten aus Transportern entfernen, deren Referenz ungültig ist
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    unittp& = channels(chnr&).units(unitnr&).unittype
    IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_TRANSPORTER) <> 0 THEN
      FOR slotnr& = 0 TO 7
        cargo& = channels(chnr&).units(unitnr&).transportcontent(slotnr&)
        IF cargo& >= 0 AND UnitIsAlive&(chnr&, cargo&) = 0 THEN channels(chnr&).units(unitnr&).transportcontent(slotnr&) = -1
      NEXT slotnr&
    END IF
  NEXT unitnr&

  'Einheiten aus Shops entfernen, deren Referenz ungültig ist
  FOR shopnr& = 0 TO channels(chnr&).info.nshops-1
    IF channels(chnr&).shops(shopnr&).shoptype > 0 THEN
      FOR slotnr& = 0 TO 15
        unitnr& = channels(chnr&).shops(shopnr&).content(slotnr&)
        IF unitnr& >= 0 AND UnitIsAlive&(chnr&, unitnr&) = 0 THEN channels(chnr&).shops(shopnr&).content(slotnr&) = -1
      NEXT slotnr&
    END IF
  NEXT shopnr&

  CALL UpdateSightBlock(chnr&)
  CALL UpdateVisionAll(chnr&)
END SUB



'Spiel-Ereignisse abarbeiten (nur Einzelspieler-Modus)
SUB ProcessGameEvents(chnr&)
  IF AnyPlayerHasPhase&(chnr&, %PHASE_SURPRISEATTACK) THEN
    CALL SurpriseAttack(chnr&)
  END IF
END SUB
