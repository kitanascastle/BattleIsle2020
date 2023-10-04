'Battle Isle 2020 - Client

'Neuen Channel erstellen
SUB CreateMultiplayerChannel
  LOCAL a$, m$

  IF hClientSocket& < 0 THEN EXIT SUB

  m$ = mapnames$(channels(0).info.currentmission)
  a$ = m$+SPACE$(7-LEN(m$))+editGameName.Value+SPACE$(32-LEN(editGameName.Value))
  CALL BIDebugLog("C: CRTG "+mapnames$(channels(0).info.currentmission)+" "+editGameName.Value)
  CALL SendPacketToServer("CRTG", a$+CHR$(defaultDifficulty&)+MKL$(channels(0).info.checksum))
END SUB



'Spiel beitreten
SUB JoinMultiplayerChannel(chnr&)
  IF hClientSocket& < 0 THEN EXIT SUB
  CALL BIDebugLog("C: JOIN "+FORMAT$(chnr&))
  CALL SendPacketToServer("JOIN", CHR$(chnr&))
END SUB



'Spielerfarbe/Team wechseln
SUB ChangePlayerColor
  CALL BIDebugLog("C: COLR")
  CALL SendPacketToServer("COLR", "")
END SUB



'Einheit bewegen
SUB ClientMoveUnit(unitnr&, x&, y&)
  CALL BIDebugLog("C: MOVE "+UnitIDString$(0, unitnr&)+" -> "+FORMAT$(x&)+","+FORMAT$(y&))
  CALL SendPacketToServer("MOVE", MKL$(unitnr&)+CHR$(x&)+CHR$(y&))
END SUB



'Mit Einheit angreifen
SUB ClientAttackUnit(attacker&, defender&, weapon&)
  CALL BIDebugLog("C: ATCK "+UnitIDString$(0, attacker&)+" with weapon "+FORMAT$(weapon&)+" vs "+UnitIDString$(0, defender&))
  CALL UnitDone(0, attacker&)
  CALL SendPacketToServer("ATCK", MKL$(attacker&)+MKL$(defender&)+CHR$(weapon&))
END SUB



'Aktion mit einer Einheit durchführen
SUB ClientUnitAction(BYVAL unitnr&, BYVAL action&, BYVAL x&, BYVAL y&)
  IF action& = %UNITACTION_MOVE THEN
    CALL ClientMoveUnit(unitnr&, x&, y&)
    EXIT SUB
  END IF

  CALL BIDebugLog("C: UNAC "+UnitIDString$(0, unitnr&)+" action "+FORMAT$(action&)+" at "+FORMAT$(x&)+","+FORMAT$(y&))
  CALL SendPacketToServer("UNAC", MKL$(unitnr&)+CHR$(action&)+CHR$(x&)+CHR$(y&))
END SUB



'Aktion in einem Shop durchführne
SUB ClientShopAction(shopnr&, action&, actionarg&)
  CALL BIDebugLog("C: SHAC "+channels(0).info.shopnames(shopnr&)+"("+FORMAT$(shopnr&)+") action "+FORMAT$(action&))
  CALL SendPacketToServer("SHAC", MKL$(shopnr&)+CHR$(action&)+MKL$(actionarg&))
END SUB



'Sendet eine Chat-Nachricht
SUB SendChatMessage(a$$, teamonly&)
  LOCAL rawdata$

  rawdata$ = PEEK$(STRPTR(a$$), LEN(a$$)*2)
  CALL BIDebugLog("C: CHAT")
  CALL SendPacketToServer("CHAT", CHR$(teamonly&)+rawdata$)
END SUB



'Zug beenden
SUB SignalEndTurn
  CALL BIDebugLog("C: ENDT")
  CALL SendPacketToServer("ENDT", "")
END SUB



'Animation wurde beendet
SUB SignalEndAnimation(unitnr1&, unitnr2&)
  CALL BIDebugLog("C: ENDA "+UnitIDString$(0, unitnr1&)+IIF$(unitnr2& >= 0, " , "+UnitIDString$(0, unitnr2&), ""))
  CALL SendPacketToServer("ENDA", MKL$(unitnr1&)+MKL$(unitnr2&))
END SUB



'Spiel erneut mit dem Server synchronisieren
SUB ResyncWithServer
  'CALL BILog(words$$(%WORD_SYNC_START), 0)
  CALL BIDebugLog("C: SYNC")
  CALL SendPacketToServer("SYNC", "")
END SUB



'Schickt einen Ping an den Server
SUB SendPing
  pingMillisecs& = -999
  pingID& = pingID&+1
  pingSentTime! = TIMER
  CALL BIDebugLog("C: PING #"+FORMAT$(pingID&))
  CALL SendPacketToServer("PING", MKL$(pingID&)+MKS$(pingSentTime!))
END SUB



'Schickt den Punktestand zum Server
SUB SendHighScore
  CALL BIDebugLog("C: HSCO")
  CALL SendPacketToServer("HSCO", highscorePacket$)
  highscorePacket$ = ""
END SUB



'Erfragt die Bestenliste für eine Mission
SUB QueryHighscore(missionnr&)
  CALL BIDebugLog("C: GSCO")
  CALL SendPacketToServer("GSCO", MKL$(missionnr&))
END SUB



'Datei vom Server herunterladen
SUB DownloadFile(f$)
  CALL BIDebugLog("C: GETF "+f$)
  CALL SendPacketToServer("GETF", f$)
END SUB



'Erfragt die Channel-Checksummen
SUB QueryDebugChecksums
  CALL BIDebugLog("C: QCKS")
  CALL SendPacketToServer("QCKS", "")
END SUB



'Prüft, ob ein Server erreichbar ist
'https://www.codeproject.com/Tips/168704/How-to-set-a-socket-connection-timeout
FUNCTION TestConnectionToServer&(serverip&)
  LOCAL md&, r&, hSocket&
  LOCAL address AS sockaddr_in
  LOCAL fdwrite AS fd_setstruc, fderror AS fd_setstruc
  LOCAL tmout AS timeval

  'nicht-blockierenden Socket erzeugen
  hSocket& = socket(%AF_INET, %SOCK_STREAM, %IPPROTO_TCP)
  md& = 1
  r& = ioctlsocket(hSocket&, %FIONBIO, md&)
  IF r& <> %NO_ERROR THEN EXIT FUNCTION

  address.sin_addr.s_addr = serverip&
  address.sin_port = htons(%SERVERPORT)
  address.sin_family = %AF_INET
  r& = cconnect(hSocket&, address, SIZEOF(address))
  IF r& = 0 THEN EXIT FUNCTION

  'Socket auf blockierend setzen
  md& = 0
  r& = ioctlsocket(hSocket&, %FIONBIO, md&)
  IF r& <> %NO_ERROR THEN
    CALL closesocket(hSocket&)
    EXIT FUNCTION
  END IF

  'prüfen, ob Socket bereit ist
  CALL FD_ZERO(fdwrite)
  CALL FD_ZERO(fderror)
  CALL FD_SET(hSocket&, fdwrite)
  CALL FD_SET(hSocket&, fderror)
  tmout.tv_sec = 3
  tmout.tv_usec = 0
  CALL sselect(0, BYVAL %NULL, fdwrite, fderror, tmout)
  IF FD_ISSET(hSocket&, fdwrite) = 0 THEN
    CALL closesocket(hSocket&)
    EXIT FUNCTION
  END IF
  CALL closesocket(hSocket&)

  TestConnectionToServer& = 1
END FUNCTION



'Socket-Verbindung zu einem Server herstellen
FUNCTION ConnectToServer&(serverip&)
  LOCAL a$$, ip$
  LOCAL D AS WSAdata

  ip$ = IpToString$(serverip&)
  CALL WSAStartup(257, D)

  'Serveradresse prüfen
  IF serverip& = 0 THEN
    CALL PrintError(words$$(%WORD_UNKNOWN_SERVER))
    EXIT FUNCTION
  END IF

  'alte Verbindung schließen
  CALL CloseConnectionToServer("Reconnect")

  IF TestConnectionToServer&(serverip&) = 0 THEN
    a$$ = words$$(%WORD_SERVERCONNECTION_FAILED)
    REPLACE "%" WITH ip$ IN a$$
    CALL PrintError(a$$)
    EXIT FUNCTION
  END IF

  'Socket erzeugen
  hClientSocket& = FREEFILE
  TCP OPEN PORT %SERVERPORT AT ip$ AS hClientSocket& TIMEOUT 4000
  IF ERRCLEAR THEN
    a$$ = words$$(%WORD_SERVERCONNECTION_FAILED)
    REPLACE "%" WITH ip$ IN a$$
    CALL PrintError(a$$)
    EXIT FUNCTION
  END IF

  'Window-Message empfangen wenn Server Daten sendet
  clientReceivedData$ = ""
  TCP NOTIFY hClientSocket&, RECV CLOSE TO hWIN& AS %TCP_CLIENTREAD

  ConnectToServer& = -1
END FUNCTION



'Socket-Verbindung zum Server schließen
SUB CloseConnectionToServer(reason$)
'CALL BILog("Close: "+reason$, 0)
  connectedToAuthenticServer& = 0
  IF hClientSocket& < 0 THEN EXIT SUB
  TCP CLOSE hClientSocket&
  hClientSocket& = -1
END SUB



'Schickt ein Datenpaket zu einem Server
SUB SendPacketToServer(tp$, a$)
  TCP SEND hClientSocket&, tp$+MKL$(LEN(a$))+a$
END SUB



'Anmeldung beim Server
SUB LoginToServer(q$)
  LOCAL serverdate&, servertime&, serverversion&, serverquestion&, clientquestion&, s$, a$$

  'Daten extrahieren
  serverdate& = CVL(q$, 1)
  servertime& = CVL(q$, 5)
  serverversion& = CVL(q$, 9)
  serverquestion& = CVL(q$, 13)
  s$ = MID$(q$, 17)
  CALL BIDebugLog("S: HELO Server Version "+FORMAT$(serverversion&/1000))

  'falls Verbindung nur herstellt wurde, um die Server-Version zu erfragen, dann Verbindung nun wieder schließen
  IF updateCheck& = -1 THEN
    updateCheck& = serverversion&
    CALL CloseConnectionToServer("Done checking for updates")
    IF serverversion& > %VERSION THEN CALL PrintError(words$$(%WORD_CLIENTUPDATEAVAILABLE))
    EXIT SUB
  END IF

  'Daten validieren
  IF s$ = "" OR serverversion& < %VERSION-9 THEN  'Client Entwickler-Beta-Version auch zulassen (z.B. Client 1181 und Server 1180)
    CALL CloseConnectionToServer("Server outdated")
    CALL PrintError(words$$(%WORD_SERVEROUTDATED))
    EXIT SUB
  END IF
  IF gameState& = %GAMESTATE_CREATELOBBY OR gameState& = %GAMESTATE_JOINLOBBY THEN
    a$$ = words$$(%WORD_SUCCESSFULLY_CONNECTED)
    REPLACE "%" WITH s$+" "+FORMAT$(serverversion&/1000, "0.000") IN a$$
    CALL BILog(a$$, 0)
  END IF

  'Authentifizierung durchführen
  clientquestion& = RND(-2100300400, -1)
  clientSecurityAnswer& = CalcSecurityAnswer&(clientquestion&, today&, curtime&)
  s$ = MKL$(today&)+MKL$(curtime&)+MKL$(%VERSION)+MKL$(CalcSecurityAnswer&(serverquestion&, serverdate&, servertime&))+MKL$(clientquestion&)+MKL$(localPlayerXP&)+localPlayerName$+SPACE$(16-LEN(localPlayerName$))
  CALL BIDebugLog("C: HELO")
  CALL SendPacketToServer("HELO", s$)
END SUB



'Prüfen, ob Server authentisch ist
SUB AuthentificateServer(q$)
  LOCAL serveranswer&, p&, f$

  CALL BIDebugLog("S: WELC")
  serveranswer& = CVL(q$)
  IF LEN(q$) <> 4 OR serveranswer& <> clientSecurityAnswer& THEN
    CALL CloseConnectionToServer("Bad server")
    CALL PrintError(words$$(%WORD_BADSERVER))
    EXIT SUB
  END IF

  'Higscore zum Server schicken, falls Verbindung nur deswegen hergestellt wurde
  IF highscorePacket$ <> "" THEN
    CALL SendHighScore
    CALL CloseConnectionToServer("Done sending highscore")
    EXIT SUB
  END IF

  'Highscore vom Server erfragen, falls Verbindung nur deswegen hergestellt wurde
  IF highscoreMapData$ = "*" THEN
    CALL QueryHighscore(channels(0).info.currentmission)
    EXIT SUB
  END IF

  'Dateien vom Server herunterladen, falls Verbindung nur deswegen hergestellt wurde
  IF filesToDownload$ <> "" THEN
    p& = INSTR(filesToDownload$, CHR$(0))
    f$ = LEFT$(filesToDownload$, p&-1)
    filesToDownload$ = MID$(filesToDownload$, p&+1)
    CALL DownloadFile(f$)
    EXIT SUB
  END IF

  'Server ist authentisch - diesen als Standardserver merken
  defaultServer$ = editServerIP.Value
  connectedToAuthenticServer& = 1
END SUB



'Lobby aktualisieren
SUB UpdateLobbyData(q$)
  CALL BIDebugLog("S: CHAN")
  lobbyData$ = q$
END SUB



'Channel erfolgreich betreten
SUB ChannelEntered(q$)
  LOCAL missionnr&, episode&, difficulty&, a$, m$

  selectedLobbyChannel& = ASC(q$)
  localPlayerNr& = ASC(q$, 2)
  localPlayerMask& = 2^localPlayerNr&
  localPlayerTeam& = ASC(q$, 3)
  missionnr& = CVL(q$, 4)
  difficulty& = ASC(q$, 8)
  episode& = GetEpisodeForMap&(missionnr&)
  CALL BIDebugLog("S: ETRD Channel "+FORMAT$(selectedLobbyChannel&)+", Player "+FORMAT$(localPlayerNr&)+", Team "+FORMAT$(localPlayerTeam&)+", Mission "+FORMAT$(missionnr&)+", Difficulty "+FORMAT$(difficulty&))
  IF mapPreviewCreated& = 0 THEN
    IF ishost& = 0 THEN CALL LoadMission&("MIS\MISS"+FORMAT$(missionnr&, "000")+".DAT", episode&, difficulty&, 0)  'der Host braucht die Mission nicht zu laden, da bereits zuvor geschehen
    updateMapPreview& = 1
  END IF
  channels(0).info.currentmission = missionnr&
  channels(0).info.difficulty = difficulty&
  channels(0).info.localplayer = localPlayerNr&
  gameState& = %GAMESTATE_CHANNELJOINED

  'Spielstand zum Server übertragen
  IF ishost& <> 0 AND multiplayerGameFromSavegame& = 1 THEN
    multiplayerGameFromSavegame& = 2
    m$ = mapnames$(missionnr&)
    a$ = m$+SPACE$(7-LEN(m$))+editGameName.Value+SPACE$(32-LEN(editGameName.Value))+GetFullChannelData$(0)
    CALL BIDebugLog("C: LOAD "+mapnames$(missionnr&))
    CALL SendPacketToServer("LOAD", a$)
  END IF
END SUB



'Spielstand vom Server übernehmen
SUB LoadMissionFromServer(q$)
  CALL BIDebugLog("S: LOAD")
  IF SetFullChannelData&(0, q$) = 0 THEN
    CALL PrintError(words$$(%WORD_SAVEGAME_INVALID))
    EXIT SUB
  END IF
END SUB



'Spielerinformationen speichern
SUB SetPlayerInfo(q$)
  LOCAL p&, plnr&, teamnr&, isai&, plname$, a$

  p& = 1
  WHILE p& <= LEN(q$)
    plname$ = RTRIM$(MID$(q$, p&, 16))
    plnr& = ASC(q$, p&+16)
    teamnr& = ASC(q$, p&+17)
    isai& = ASC(q$, p&+18)
    playernames$(plnr&) = plname$
    IF plname$ = localPlayerName$ THEN
      localPlayerNr& = plnr&
      localPlayerMask& = 2^localPlayerNr&
      localPlayerTeam& = teamnr&
    END IF
    p& = p&+19
    a$ = a$+"Player "+FORMAT$(plnr&)+" "+plname$+" (Team "+FORMAT$(teamnr&)+", AI="+FORMAT$(isai&)+") "
  WEND
  CALL BIDebugLog("S: PLYR "+RTRIM$(a$))
END SUB



'Fehlermeldung vom Server ausgeben
SUB PrintServerError(q$)
  LOCAL msg&

  msg& = CVL(q$)
  CALL BIDebugLog("S: RJCT "+words$$(msg&))
  CALL PrintError(words$$(msg&))
END SUB



'Multiplayer Spiel starten
SUB ClientStartGame(q$)
  LOCAL difficulty&

  mpCountdown& = ASC(q$)
  mpCountdownTime! = gametime!
  CALL BIDebugLog("S: STGM "+FORMAT$(mpCountdown&))

  IF mpCountdown& = 0 THEN
    CALL CloseLobby(0, 0)
    gameState& = %GAMESTATE_INGAME
    difficulty& = channels(0).info.difficulty
    IF channels(0).info.fromSavegame = 0 THEN
      IF LoadMission&("MIS\MISS"+FORMAT$(channels(0).info.currentmission, "000")+".DAT", 4, difficulty&, 0) = 0 THEN
        CALL CloseConnectionToServer("Failed to load map")
        EXIT SUB
      END IF
    END IF
    CALL InitMap(0, difficulty&)
    CALL BIDebugLog("C: REDY")
    CALL SendPacketToServer("REDY", "")
  END IF
END SUB



'Kontrolle an aktives Team übergeben
SUB SetActiveTeam(q$)
  LOCAL team&

  team& = ASC(q$)
  CALL BIDebugLog("S: ACTT "+FORMAT$(team&))
  IF team& < channels(0).info.activeteam THEN CALL InitRound(0)
  channels(0).info.activeteam = team&
  IF channels(0).info.activeteam = localPlayerTeam& THEN
    autoResync& = 1
    CALL QueryDebugChecksums
    CALL EnableAllMenuButtons
  ELSE
    CALL DisableAllMenuButtons
  END IF
  CALL UpdateProgressbar
END SUB



'Ein Spieler bewegt eine Einheit
SUB UnitMovedByServer(q$)
  LOCAL unitnr&, x&, y&, plnr&

  'Daten extrahieren
  unitnr& = CVL(q$, 1)
  x& = ASC(q$, 5)
  y& = ASC(q$, 6)
  CALL BIDebugLog("S: MOVE "+UnitIDString$(0, unitnr&)+" -> "+FORMAT$(x&)+","+FORMAT$(y&))

  'Einheit bewegen
  plnr& = channels(0).units(unitnr&).owner
  channels(0).player(plnr&).selectedunit = unitnr&
  CALL MoveUnit(unitnr&, x&, y&)
END SUB



'Ein Kampf findet statt
SUB CombatByServer(q$)
  LOCAL attacker&, defender&
  LOCAL combat AS TCombatInfo

  POKE$ VARPTR(combat), q$
  attacker& = combat.attacker
  defender& = combat.defender
  CALL BIDebugLog("S: ATCK "+UnitIDString$(0, attacker&)+" with weapon "+FORMAT$(combat.weaponatt)+" vs "+UnitIDString$(0, defender&)+" with weapon "+FORMAT$(combat.weapondef))
  CALL CreateMissile(attacker&, defender&, combat.weaponatt, combat.params(5, 1))
  CALL CreateMissile(defender&, attacker&, combat.weapondef, combat.params(5, 0))

  IF channels(0).units(attacker).owner = localPlayerNr& OR channels(0).units(defender&).owner = localPlayerNr& THEN
    'Kampf darstellen
    POKE$ VARPTR(channels(0).combat), q$
    CALL SetPhase(0, localPlayerNr&, %PHASE_COMBAT)
    combatStartTime! = gametime!
    IF channels(0).units(defender&).owner = localPlayerNr& THEN CALL ClearTargets(0, localPlayerNr&)  'Bewegungsmarkierungen entfernen (falls aktiver Spieler mit einem Überraschungsangriff angegriffen wurde)
    'zum Kampfgeschehen scrollen, falls lokaler Spieler angegriffen wird
    IF channels(0).units(defender&).owner = localPlayerNr& THEN CALL ScrollToMapPos(channels(0).units(defender&).xpos, channels(0).units(defender&).ypos, 0.5)
  ELSE
    'Kampf nur auswerten
    CALL EndCombat(0, combat)
  END IF
END SUB



'Einheit zerstören
SUB UnitDestroyedByServer(q$)
  LOCAL unitnr&, killingplayer&, killreason&

  unitnr& = CVL(q$)
  killingplayer& = CVL(q$, 5)
  killreason& = CVL(q$, 9)
  CALL BIDebugLog("S: KILL "+UnitIDString$(0, unitnr&)+" by player "+FORMAT$(killingplayer&)+" with "+FORMAT$(killreason&))
  CALL KillUnit(0, unitnr&, killingplayer&, killreason&, 0)
END SUB



'Einheit mit Daten vom Server aktualisieren
SUB UpdateUnitByServer(q$)
  LOCAL unitnr&, d&, orgtype&, flags&, md&, targetunits&()

  unitnr& = CVL(q$)
  CALL BIDebugLog("S: UPDU "+UnitIDString$(0, unitnr&))
  POKE$ VARPTR(channels(0).units(unitnr&)), MID$(q$, 5, SIZEOF(TUnit))
  IF unitnr& >= channels(0).info.nunits AND UnitIsAlive&(0, unitnr&) <> 0 THEN channels(0).info.nunits = unitnr&+1
  IF channels(0).units(unitnr&).owner = localPlayerNr& AND channels(0).player(localPlayerNr&).selectedunit = unitnr& AND LocalPlayersTurn& <> 0 THEN
    CALL ClearTargets(0, localPlayerNr&)
    flags& = channels(0).units(unitnr&).flags
    IF (flags& AND %US_MOVED) = 0 OR ((flags& AND %US_ATTACKED) <> 0 AND UnitCanRetreat&(0, unitnr&) <> 0) THEN md& = md& OR 1
    IF (flags& AND %US_ATTACKED) = 0 THEN md& = md& OR 2
    IF (flags& AND %US_DONE) = 0 AND md& <> 0 THEN
      IF GetPhase&(0, localPlayerNr&) < %PHASE_WEAPONMENU THEN CALL SetPhase(0, localPlayerNr&, %PHASE_UNITSELECTED)
      IF GetTargets&(0, unitnr&, md&, 0, targetunits&()) = 0 THEN CALL UnselectUnit(0, localPlayerNr&)
    END IF
  END IF
END SUB



'Shop mit Daten vom Server aktualisieren
SUB UpdateShopByServer(q$)
  LOCAL shopnr&

  shopnr& = CVL(q$)
  CALL BIDebugLog("S: UPDS "+channels(0).info.shopnames(shopnr&)+"("+FORMAT$(shopnr&)+")")
  POKE$ VARPTR(channels(0).shops(shopnr&)), MID$(q$, 5, SIZEOF(TShop))
END SUB



'Spieler mit Daten vom Server aktualisieren
SUB UpdatePlayerByServer(q$)
  LOCAL plnr&, oldallymask&

  plnr& = ASC(q$)
  oldallymask& = channels(0).player(plnr&).allymask
  channels(0).player(plnr&).allymask = ASC(q$, 2)
  channels(0).player(plnr&).team = ASC(q$, 3)
  channels(0).player(plnr&).energy = CVL(q$, 4)
  CALL BIDebugLog("S: UPDP "+playernames$(plnr&)+"("+FORMAT$(plnr&)+") Team "+FORMAT$(channels(0).player(plnr&).team)+", Energy "+FORMAT$(channels(0).player(plnr&).energy))
  IF plnr& <> localPlayerNr& THEN
    channels(0).player(plnr&).selectedunit = CVL(q$, 8)
  ELSE
    IF channels(0).player(plnr&).allymask <> oldallymask& THEN
      CALL UpdateSightBlock(0)
      CALL UpdateVisionAll(0)
    END IF
  END IF
END SUB



'Kartenfeld mit Daten vom Server aktualisieren
SUB UpdateZoneByServer(q$)
  LOCAL x&, y&

  x& = ASC(q$, 1)
  y& = ASC(q$, 2)
  CALL BIDebugLog("S: UPDZ "+FORMAT$(x&)+","+FORMAT$(y&))
  channels(0).zone1(x&, y&) = CVL(q$, 3)
  channels(0).zone2(x&, y&) = CVL(q$, 5)
  channels(0).zone3(x&, y&) = CVL(q$, 7)
END SUB



'Channelinfo mit Daten vom Server aktualisieren
SUB UpdateInfoByServer(q$)
  LOCAL ofs&, a$

  ofs& = CVI(q$)
  a$ = MID$(q$, 3)
  CALL BIDebugLog("S: UPDI "+FORMAT$(ofs&))
  IF ofs& >= 0 AND ofs&+LEN(a$) <= SIZEOF(TChannelInfo) THEN
    POKE$ VARPTR(channels(0).info)+ofs&, a$
  END IF
END SUB



'Wetter ändern
SUB UpdateWeatherByServer(q$)
  LOCAL newweather&, startrow&, endrow&

  newweather& = ASC(q$)
  startrow& = ASC(q$, 2)
  endrow& = ASC(q$, 3)
  CALL BIDebugLog("S: WEAT "+FORMAT$(newweather&)+" from row "+FORMAT$(startrow&)+" to "+FORMAT$(endrow&))
  CALL ApplyWeather(0, newweather&, startrow&, endrow&)
END SUB



'Gesamten Channel mit Datem vom Server aktualisieren
SUB UpdateChannelByServer(q$)
  LOCAL i&, unitcount&
  LOCAL d&()

  CALL BIDebugLog("S: SYNC")

  'Ausrichtung der Einheiten merken, da die Ausrichtung auf dem Server nicht existiert
  unitcount& = channels(0).info.nunits
  DIM d&(unitcount&-1)
  FOR i& = 0 TO unitcount&-1
    d&(i&) = channels(0).units(i&).direction
  NEXT i&

  'Daten vom Server übernehmen
  CALL SetFullChannelData&(0, q$)

  'Client-Daten wiederherstellen, die nicht auf dem Server existieren
  channels(0).info.localplayer = localPlayerNr&
  unitcount& = MIN&(unitcount&, channels(0).info.nunits)
  FOR i& = 0 TO unitcount&-1
    channels(0).units(i&).direction = d&(i&)
  NEXT i&

  CALL ClearTargets(0, localPlayerNr&)
  CALL UpdateSightBlock(0)
  CALL UpdateVisionAll(0)
  updateMiniMap& = 1

  'CALL BILog(words$$(%WORD_SYNC_END), 0)
END SUB



'AI-Fortschritt aktualisieren
SUB UpdateAIProgressByServer(q$)
  LOCAL plnr&, curprogress&, totalprogress&

  plnr& = ASC(q$)
  curprogress& = CVL(q$, 2)
  totalprogress& = CVL(q$, 6)
  CALL BIDebugLog("S: AIPR "+playernames$(plnr&)+" "+FORMAT$(curprogress&)+"/"+FORMAT$(totalprogress&))
  CALL ShowAIProgress(playernames$(plnr&), plnr&, curprogress&, totalprogress&)
END SUB



'Channel-Zustand aktualisieren
SUB UpdateChannelState(q$)
  channels(0).info.state = ASC(q$)
  CALL BIDebugLog("S: CHST "+FORMAT$(channels(0).info.state))
END SUB



'Chat-Nachricht erhalten (Unicode)
SUB ChatMessageReceived(q$)
  LOCAL plnr&, n&, a$$, u$$

  plnr& = ASC(q$)
  n& = LEN(q$)-1
  u$$ = SPACE$(n&/2)
  POKE$ STRPTR(u$$), MID$(q$, 2)
  CALL BIDebugLog("S: CHAT "+playernames$(plnr&))
  a$$ = playernames$(plnr&)+": "+u$$
  CALL BILog(a$$, plnr&+1)
END SUB



'Spiel-Nachricht erhalten
SUB GameMessageFromServer(q$)
  LOCAL msgid&

  msgid& = CVL(q$)
  CALL BIDebugLog("S: GMSG "+FORMAT$(msgid&))
  CALL ShowGameMessage(0, localPlayerNr&, msgid&)
END SUB



'Highscore Daten erhalten
SUB ReceiveHighscoreFromServer(q$)
  LOCAL i&, n&, sc&(), d$()
  LOCAL r AS THighScore

  CALL BIDebugLog("S: HSCO")

  'Verbindung zum Server wieder schließen, falls diese nur hergestellt wurde, um den Highscore zu erfragen
  IF highscoreMapData$ = "*" THEN CALL CloseConnectionToServer("Done sending highscore2")

  'Einträge sortieren
  n& = (LEN(q$)-4)/SIZEOF(THighScore)
  DIM sc&(n&-1), d$(n&-1)
  FOR i& = 0 TO n&-1
    d$(i&) = MID$(q$, i&*SIZEOF(THighScore)+5, SIZEOF(THighScore))
    POKE$ VARPTR(r), d$(i&)
    sc&(i&) = r.difficulty*100000000-r.turnnumber*100000+r.scoreground+r.scorewater+r.scoreair
  NEXT i&
  ARRAY SORT sc&(0) FOR n&, TAGARRAY d$(), DESCEND

  'sortierte Einträge zurückschreiben
  highscoreMapData$ = LEFT$(q$, 4)
  FOR i& = 0 TO n&-1
    highscoreMapData$ = highscoreMapData$+d$(i&)
  NEXT i&
END SUB



'Datei vom Server erhalten
SUB ReceiveFileFromServer(q$)
  LOCAL f$, a$, nextfile$, namelen&, datalen&, p&

  'Verbindung zum Server wieder schließen, falls diese nur hergestellt wurde, um Dateien herunterzuladen und die letzte Datei wurde soeben empfangen
  IF LEN(filesToDownload$) > 0 THEN
    p& = INSTR(filesToDownload$, CHR$(0))
    nextfile$ = LEFT$(filesToDownload$, p&-1)
    filesToDownload$ = MID$(filesToDownload$, p&+1)
    IF nextfile$ = "" THEN CALL CloseConnectionToServer("Files received")
  END IF

  'Datei lokal speichern
  namelen& = ASC(q$)
  datalen& = CVL(q$, 2)
  f$ = MID$(q$, 6, namelen&)
  a$ = MID$(q$, 6+namelen&, datalen&)
  IF LEFT$(f$, 1) = "." OR LEFT$(f$, 1) = "\" THEN EXIT SUB
  CALL BIDebugLog("S: FILE "+f$)
  CALL WriteFileContent(f$, a$)

  'nächste Datei herunterladen
  IF nextfile$ <> "" THEN CALL DownloadFile(nextfile$)
END SUB



'Ping-Antwort erhalten
SUB PingReceived(q$)
  LOCAL id&, receiveTime!, sentTime!

  receiveTime! = TIMER
  id& = CVL(q$)
  sentTime! = CVS(q$, 5)
  pingMillisecs& = (receiveTime!-sentTime!)*1000
  CALL BIDebugLog("S: PING #"+FORMAT$(id&)+" "+FORMAT$(pingMillisecs&)+" ms")
END SUB



'Channel-Checksummen erhalten
SUB ReceiveServerChecksums(q$)
  CALL BIDebugLog("S: DCKS")
  debugServerChecksums$ = q$
  IF autoResync& = 1 THEN
    autoResync& = 0
    IF debugServerChecksums$ <> DebugCalculateChannelChecksum$(0) THEN CALL ResyncWithServer
  END IF
END SUB



'Verbindung zum Server verloren
SUB ConnectionLost
  CALL PrintError(words$$(%WORD_CONNECTION_LOST))
  CALL CloseConnectionToServer("By server")
END SUB



'Ein Spieler hat das Spiel verlassen
SUB PlayerLeftGame(q$)
  LOCAL plnr&, a$$

  plnr& = ASC(q$)
  CALL BIDebugLog("S: DISC "+playernames$(plnr&))
  a$$ = words$$(%WORD_PLAYER_LEFT)
  REPLACE "%" WITH playernames$(plnr&) IN a$$
  CALL BILog(a$$, 0)
END SUB



'Prüft, ob ein Datenpaket vom Server gültig ist
FUNCTION ValidateServerPacket&(tp$, datalen&)
  LOCAL minlen&, exactlen&

  exactlen& = -1
  SELECT CASE tp$
  CASE "HELO": minlen& = 16
  CASE "WELC": exactlen& = 4
  CASE "RJCT": exactlen& = 4
  CASE "CHAN": minlen& = 0
  CASE "ETRD": exactlen& = 8
  CASE "LOAD": minlen& = 0
  CASE "PLYR": minlen& = 0
  CASE "STGM": exactlen& = 1
  CASE "ACTT": exactlen& = 1
  CASE "MOVE": exactlen& = 6
  CASE "ATCK": exactlen& = SIZEOF(TCombatInfo)
  CASE "KILL": exactlen& = 12
  CASE "UPDU": exactlen& = 4+SIZEOF(TUnit)
  CASE "UPDS": exactlen& = 4+SIZEOF(TShop)
  CASE "UPDP": exactlen& = 11
  CASE "UPDZ": exactlen& = 8
  CASE "UPDI": minlen& = 2
  CASE "WEAT": exactlen& = 3
  CASE "CHAT": minlen& = 1
  CASE "AIPR": exactlen& = 9
  CASE "DISC": exactlen& = 1
  CASE "SYNC": minlen& = 0
  CASE "GMSG": exactlen& = 4
  CASE "CHST": exactlen& = 1
  CASE "HSCO": minlen& = 4
  CASE "DCKS": exactlen& = 20
  CASE "FILE": minlen& = 5
  CASE "PING": exactlen& = 8
  CASE ELSE:
    'unbekannter Pakettyp
    CALL BIDebugLog("Received unknown package from server: "+tp$+"("+FORMAT$(datalen&)+")")
    ValidateServerPacket& = -1
    EXIT FUNCTION
  END SELECT

  IF (exactlen& >= 0 AND datalen& <> exactlen&) OR datalen& < minlen& THEN
    'falsche Paketlänge
    CALL BIDebugLog("Received malformed package from server: "+tp$+"("+FORMAT$(datalen&)+")")
    ValidateServerPacket& = -2
    EXIT FUNCTION
  END IF

  ValidateServerPacket& = 1
END FUNCTION



'Nimmt Daten vom Server entgegen
SUB ProcessServerData
  LOCAL receiveddata$, q$, tp$
  LOCAL datalen&, v&

  'Daten lesen
  TCP RECV hClientSocket&, 64000, receiveddata$
  clientReceivedData$ = clientReceivedData$+receiveddata$
  IF LEN(clientReceivedData$) < 8 THEN EXIT SUB

  DO
    tp$ = LEFT$(clientReceivedData$, 4)
    datalen& = CVL(clientReceivedData$, 5)
    v& = ValidateServerPacket&(tp$, datalen&)
    IF v& < 1 THEN
      'Puffer leeren, um Datenmüll zu löschen
'CALL BILog("Packet error: "+tp$, 0)
      clientReceivedData$ = ""
      EXIT SUB
    END IF
'CALL BILog(tp$+" "+FORMAT$(datalen&)+" "+FORMAT$(LEN(q$)), 0)
    IF LEN(clientReceivedData$) < datalen&+8 THEN EXIT SUB
    q$ = MID$(clientReceivedData$, 9, datalen&)
    clientReceivedData$ = MID$(clientReceivedData$, 9+datalen&)

    'Daten auswerten
    SELECT CASE tp$
    CASE "HELO": CALL LoginToServer(q$)
    CASE "WELC": CALL AuthentificateServer(q$)
    CASE "RJCT": CALL PrintServerError(q$)
    CASE "CHAN": CALL UpdateLobbyData(q$)
    CASE "ETRD": CALL ChannelEntered(q$)
    CASE "LOAD": CALL LoadMissionFromServer(q$)
    CASE "PLYR": CALL SetPlayerInfo(q$)
    CASE "STGM": CALL ClientStartGame(q$)
    CASE "ACTT": CALL SetActiveTeam(q$)
    CASE "MOVE": CALL UnitMovedByServer(q$)
    CASE "ATCK": CALL CombatByServer(q$)
    CASE "KILL": CALL UnitDestroyedByServer(q$)
    CASE "UPDU": CALL UpdateUnitByServer(q$)
    CASE "UPDS": CALL UpdateShopByServer(q$)
    CASE "UPDP": CALL UpdatePlayerByServer(q$)
    CASE "UPDZ": CALL UpdateZoneByServer(q$)
    CASE "UPDI": CALL UpdateInfoByServer(q$)
    CASE "WEAT": CALL UpdateWeatherByServer(q$)
    CASE "CHAT": CALL ChatMessageReceived(q$)
    CASE "AIPR": CALL UpdateAIProgressByServer(q$)
    CASE "DISC": CALL PlayerLeftGame(q$)
    CASE "SYNC": CALL UpdateChannelByServer(q$)
    CASE "GMSG": CALL GameMessageFromServer(q$)
    CASE "CHST": CALL UpdateChannelState(q$)
    CASE "HSCO": CALL ReceiveHighscoreFromServer(q$)
    CASE "DCKS": CALL ReceiveServerChecksums(q$)
    CASE "FILE": CALL ReceiveFileFromServer(q$)
    CASE "PING": CALL PingReceived(q$)
    END SELECT
  LOOP UNTIL clientReceivedData$ = ""
END SUB
