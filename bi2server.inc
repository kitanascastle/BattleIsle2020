'Battle Isle 2020 - Server

TYPE hostentStru  ' hostent type in winsock.h - renamed to avoid conflicts
    h_name  AS ASCIIZ PTR   ' official name of host
    h_alias AS DWORD        ' alias list (pointer to ASCIIZ PTR list)
    h_addr  AS INTEGER      ' host address type
    h_len   AS INTEGER      ' length of address
    h_list  AS DWORD        ' list of addresses (pointer to ASCIIZ PTR list)
END TYPE



'Server-Konsole darstellen
SUB UpdateServerConsole
  LOCAL a$, s$, m$, i&, y&, n&, cl&, chnr&, plnr&, cnr&, npl&

  IF serverConsoleMode& <> 1 THEN EXIT SUB

  'Titel
  LOCATE 1, 1
  COLOR 15, 1
  a$ = $WINDOWTITLESERVER+" v"+FORMAT$(%VERSION/1000, "0.00")
  PRINT " "+a$+SPACE$(79-LEN(a$));

  'Channels
  LOCATE 2, 2
  COLOR 14, 0
  PRINT "Channel  State  Players  Map       NetBuf";
  COLOR 15, 0
  FOR chnr& = 0 TO %MAXCHANNELS-1
    LOCATE 3+chnr&, 1
    s$ = GetChannelStateName$(channels(chnr&).info.state)
    npl& = 0
    FOR i& = 0 TO %MAXCONNECTIONS-1
      IF connections(i&).fnr > 0 AND connections(i&).chnr = chnr& THEN npl& = npl&+1
    NEXT i&
    m$ = ""
    IF channels(chnr&).info.state > %CHANNELSTATE_EMPTY AND channels(chnr&).info.currentmission >= 0 AND channels(chnr&).info.currentmission <= UBOUND(mapnames$()) THEN m$ =  mapnames$(channels(chnr&).info.currentmission)
    n& = LEN(netbuffers$(chnr&))
    a$ = FORMAT$(chnr&, "* ##")+"      "+s$+SPACE$(8-LEN(s$))+FORMAT$(npl&)+"      "+m$+SPACE$(9-LEN(m$))+FORMAT$(n&, "* ######")
    a$ = a$+SPACE$(46-LEN(a$))
    PRINT a$;
  NEXT chnr&

  'Connections
  y& = 3
  LOCATE 2, 47
  COLOR 11, 0
  PRINT "Conn  Player     Channel  Phase";
  FOR cnr& = 0 TO %MAXCONNECTIONS-1
    IF connections(cnr&).fnr > 0 THEN
      COLOR (cnr& AND 7)+8, 0
      LOCATE y&, 46
      m$ = LEFT$(connections(cnr&).playername, 8)
      s$ = ""
      chnr& = connections(cnr&).chnr
      plnr& = connections(cnr&).snr
      IF chnr& >= 0 AND plnr& >= 0 THEN s$ = GetPhaseName$(channels(chnr&).player(plnr&).phase)
      a$ = FORMAT$(cnr&, "* ##")+"   "+m$+SPACE$(10-LEN(m$))+FORMAT$(chnr&, "* ##")+"      "+s$
      a$ = a$+SPACE$(32-LEN(a$))
      PRINT a$;
      y& = y&+1
      IF y& = %MAXCHANNELS+3 THEN EXIT FOR
    END IF
  NEXT cnr&
  FOR i& = y& TO %MAXCHANNELS+2
    LOCATE i&, 46
    PRINT SPACE$(32);
  NEXT i&

  'Protokoll
  y& = %MAXCHANNELS+3
  LOCATE y&, 1
  COLOR 15, 1
  PRINT SPACE$(80);
  FOR i& = 0 TO messageCount&-1
    cl& = ASC(messageBuffer$$(i&))
    a$ = LEFT$(MID$(messageBuffer$$(i&), 2), 80)
    a$ = a$+SPACE$(80-LEN(a$))
    y& = y&+1
    LOCATE y&, 1
    COLOR IIF&(cl& = 7, 4, IIF&(cl& = 0, 7, cl&)), 0
    PRINT a$;
  NEXT i&
END SUB



'Liefert den Namen zu einem Channel-Zustand
FUNCTION GetChannelStateName$(BYVAL s&)
  SELECT CASE s&
  CASE %CHANNELSTATE_EMPTY: GetChannelStateName$ = "Empty"
  CASE %CHANNELSTATE_LOBBY: GetChannelStateName$ = "Lobby"
  CASE %CHANNELSTATE_WAITREADY: GetChannelStateName$ = "Ready"
  CASE %CHANNELSTATE_INGAME: GetChannelStateName$ = "InGame"
  CASE %CHANNELSTATE_VICTORY: GetChannelStateName$ = "Victor"
  CASE %CHANNELSTATE_VICTORYBONUS: GetChannelStateName$ = "VBonus"
  CASE %CHANNELSTATE_DEFEAT: GetChannelStateName$ = "Defeat"
  END SELECT
END FUNCTION



'Liefert den Namen zu einer Spiel-Phase
FUNCTION GetPhaseName$(BYVAL p&)
  SELECT CASE p&
  CASE %PHASE_NONE: GetPhaseName$ = "None"
  CASE %PHASE_UNITSELECTED: GetPhaseName$ = "Select"
  CASE %PHASE_UNITMOVING: GetPhaseName$ = "Move"
  CASE %PHASE_SURPRISEATTACK: GetPhaseName$ = "SurAtt"
  CASE %PHASE_COMBAT: GetPhaseName$ = "Combat"
  CASE %PHASE_WEAPONMENU: GetPhaseName$ = "Weapon"
  CASE %PHASE_CLIMBMENU: GetPhaseName$ = "Climb"
  CASE %PHASE_DIVEMENU: GetPhaseName$ = "Dive"
  CASE %PHASE_MAINMENU: GetPhaseName$ = "Menu"
  CASE %PHASE_BUILDMENU: GetPhaseName$ = "Build"
  CASE %PHASE_SUPPORTMENU: GetPhaseName$ = "Supprt"
  CASE %PHASE_CAMPAIGNTRAINING: GetPhaseName$ = "CTrain"
  END SELECT
END FUNCTION



'Gibt den Inhalt aller Net-Buffers aus
SUB DebugPrintNetBuffers(ch&)
  LOCAL startch&, endch&, chnr&, p&, cnr&, bufferlen&, packetlen&, datalen&, a$, tp$

  startch& = IIF&(ch& >= 0, ch&, 0)
  endch& = IIF&(ch& >= 0, ch&, %MAXCHANNELS-1)
  FOR chnr& = startch& TO endch&
    IF channels(chnr&).info.state > %CHANNELSTATE_EMPTY THEN
      CALL SetNetBufferSemaphore(chnr&)
      bufferlen& = LEN(netbuffers$(chnr&))
      IF bufferlen& > 0 THEN
        a$ = ""
        p& = 1

        DO
          IF a$ <> "" THEN a$ = a$+"  "
          cnr& = CVL(netbuffers$(chnr&), p&)
          packetlen& = CVL(netbuffers$(chnr&), p&+4)
          tp$ = MID$(netbuffers$(chnr&), p&+8, 4)
          datalen& = CVL(netbuffers$(chnr&), p&+12)
          a$ = a$+"C"+FORMAT$(cnr&)+"+PL:"+FORMAT$(packetlen&)+"+"+tp$+"(DL:"+FORMAT$(datalen&)+")"
          IF p&+15+datalen& > bufferlen& THEN a$ = a$+"!"
          p& = p&+8+packetlen&
        LOOP UNTIL p& > bufferlen& OR LEN(a$) > 70 OR packetlen& < 1
      END IF
      CALL ResetNetBufferSemaphore(chnr&)
      IF a$ <> "" THEN CALL BILog(a$, 0)
'      CALL WriteFileContent("test\debug-netbuffer"+FORMAT$(chnr&)+".dbg", netbuffers$(chnr&))
    END IF
  NEXT chnr&
END SUB



'Tastatureingaben in der Server-Konsole verarbeiten
SUB ProcessServerKeys
  LOCAL c$, chnr&

  c$ = INKEY$
  IF c$ = "" THEN EXIT SUB

  SELECT CASE c$
  CASE CHR$(1):   'STRG+A
    serverLogPackets& = 1-serverLogPackets&
  CASE CHR$(2):   'STRG+B
    CALL DebugPrintNetBuffers(-1)
  CASE CHR$(11):  'STRG+K
    FOR chnr& = 0 TO %MAXCHANNELS-1
     IF channels(chnr&).info.state > %CHANNELSTATE_EMPTY THEN CALL DebugSaveChannelToFile(chnr&)
    NEXT chnr&
  CASE CHR$(23):  'STRG+W
    CALL BILog("Pausing server for 300 seconds.", 0)
    CALL UpdateServerConsole
    SLEEP 300000
    CALL BILog("Resuming server.", 0)
  END SELECT
END SUB



'Traffic protokollieren
SUB TrafficLog(chnr&, src&, dest&, a$)
  LOCAL d$

  IF enabledTrafficLog& = 0 THEN EXIT SUB

  IF src& = -1 THEN
    'Daten vom Server
    IF dest& = -1 THEN
      d$ = DATE$
      serverTrafficLog$ = serverTrafficLog$+MID$(d$, 7, 4)+"-"+MID$(d$, 1, 2)+"-"+MID$(d$, 4, 2)+" "+TIME$+": "+a$+CHR$(13,10)
    ELSE
      serverTrafficLog$ = serverTrafficLog$+FormatServerTraffic$(chnr&, dest&, a$)+CHR$(13,10)
    END IF
  ELSE
    'Daten von einem Client
    serverTrafficLog$ = serverTrafficLog$+FORMAT$(src&)+": "+a$+CHR$(13,10)
  END IF
END SUB



'Traffic vom Server für Logfile formatieren
FUNCTION FormatServerTraffic$(chnr&, dest&, a$)
  LOCAL b$, tp$

  tp$ = LEFT$(a$, 4)
  b$ = "S>"+FORMAT$(dest&)+": "+tp$
  SELECT CASE tp$
  CASE "RJCT": b$ = b$+" "+FORMAT$(CVL(a$, 5))
  CASE "ACTT": b$ = b$+" "+FORMAT$(ASC(a$, 5))
  CASE "MOVE": b$ = b$+" "+UnitIDString$(chnr&, CVL(a$, 5))+" > "+FORMAT$(ASC(a$, 9))+","+FORMAT$(ASC(a$, 10))
  CASE "ATCK": b$ = b$+" "+UnitIDString$(chnr&, CVL(a$, 5))+" > "+UnitIDString$(chnr&, CVL(a$, 9))
  CASE "KILL": b$ = b$+" "+UnitIDString$(chnr&, CVL(a$, 5))
  CASE "UPDU": b$ = b$+" "+UnitIDString$(chnr&, CVL(a$, 5))
  CASE "UPDS": b$ = b$+" "+FORMAT$(CVL(a$, 5))
  CASE "UPDP": b$ = b$+" Player "+FORMAT$(ASC(a$, 5))+" > AllyMask "+FORMAT$(ASC(a$, 6))+" , Team "+FORMAT$(ASC(a$, 7))+" , Energy "+FORMAT$(ASC(a$, 8))+" , Select "+UnitIDString$(chnr&, CVL(a$, 12))
  CASE "UPDZ": b$ = b$+" "+FORMAT$(ASC(a$, 5))+","+FORMAT$(ASC(a$, 6))+" > "+FORMAT$(CVI(a$, 7))+","+FORMAT$(CVI(a$, 9))+","+FORMAT$(CVI(a$, 11))
  CASE "UPDI": b$ = b$+" "+FORMAT$(CVI(a$, 5))
  CASE "AIPR": b$ = b$+" AI "+FORMAT$(ASC(a$, 5))+" > "+FORMAT$(CVL(a$, 6))+"/"+FORMAT$(CVL(a$, 10))
  CASE "DISC": b$ = b$+" "+FORMAT$(ASC(a$, 5))
  CASE "GMSG": b$ = b$+" "+FORMAT$(CVL(a$, 5))
  CASE "CHST": b$ = b$+" "+FORMAT$(ASC(a$, 5))
  END SELECT

  FormatServerTraffic$ = b$
END FUNCTION



'Traffic in Datei speichern
SUB SaveTrafficLog
  IF serverTrafficLog$ <> "" THEN
    CALL AppendFileContent($TRAFFICLOGFILE, serverTrafficLog$)
    serverTrafficLog$ = ""
  END IF
END SUB



'Zählt die Zeit bis zum Start eines Spiels herunter
SUB GameStartCountDown
  LOCAL chnr&, countdown&

  FOR chnr& = 0 TO %MAXCHANNELS-1
    countdown& = channels(chnr&).info.countdown
    IF channels(chnr&).info.state = %CHANNELSTATE_LOBBY AND countdown& > 0 AND countdown& < 255 THEN
      countdown& = countdown&-1
      channels(chnr&).info.countdown = countdown&
      IF (countdown& MOD 10) = 0 THEN CALL SendPacketToChannel(chnr&, "STGM", CHR$(INT(countdown&/10)))
      IF countdown& = 0 THEN CALL WaitPlayersReady(chnr&)
    END IF
  NEXT chnr&
END SUB



'Prüft, ob alle Spieler bereit sind
SUB CheckPlayersReady
  LOCAL chnr&, plnr&, nready&, humanmask&

  FOR chnr& = 0 TO %MAXCHANNELS-1
    IF channels(chnr&).info.state = %CHANNELSTATE_WAITREADY THEN
      nready& = 0
      FOR plnr& = 0 TO %MAXPLAYERS-1
        IF channels(chnr&).player(plnr&).state = %PLAYERSTATE_READY THEN nready& = nready&+1
      NEXT plnr&
      humanmask& = channels(chnr&).info.originalplayers AND NOT channels(chnr&).info.aimask
      IF nready& = CountBits&(humanmask&) THEN CALL SwitchChannelToIngame(chnr&)
    END IF
  NEXT chnr&
END SUB



'Timer im Server-Modus
SUB ServerTimerEvent
  CALL GameStartCountDown
  CALL CheckPlayersReady
END SUB



'IP-Adresse als String formatieren
FUNCTION IPToString$(ip&)
  LOCAL x&, i&, a$

  x& = ip&
  FOR i& = 1 TO 4
    IF a$ <> "" THEN a$ = a$+"."
    a$ = a$+LTRIM$(STR$(x& AND 255))
    x& = INT(x&/256)
  NEXT i&

  IPToString$ = a$
END FUNCTION



'Wandelt einen String in eine IP-Adresse um
FUNCTION StringToIP&(s$)
  LOCAL a$, i&

  IF PARSECOUNT(s$, ".") <> 4 THEN
    HOST ADDR s$ TO i&
    StringToIP& = i&
    EXIT FUNCTION
  END IF

  FOR i& = 1 TO 4
    a$ = a$+CHR$(VAL(PARSE$(s$, ".", i&)))
  NEXT i&

  StringToIP& = CVL(a$)
END FUNCTION



'Alle IP-Adressen des lokalen Computers ermitteln
SUB GetAllIPs(ip&(), n&)
  LOCAL h AS ASCIIZ*64
  LOCAL i&,p&
  LOCAL PE AS hostentStru PTR, E AS hostentStru
  LOCAL D AS WSAdata

  'Computername auslesen
  HOST NAME TO h

  'WSA initialisieren
  CALL WSAStartup(257, D)
  PE = GetHostByName(h)

  'alle IP-Adressen holen
  n& = 0
  IF PE > 0 THEN
    E = @PE
    p& = E.h_list  'Zeiger auf Liste von Adress-Zeigern
    DO
      i& = CVL(PEEK$(p&, 4))
      IF i& = 0 THEN EXIT LOOP
      ip&(n&) = CVL(PEEK$(i&, 4))
      n& = n&+1
      p& = p&+4
    LOOP
  END IF

  'localhost hinzufügen
  ip&(n&) = &H0100007F
  n& = n&+1

  'WSA beenden
  CALL WSACleanup
END SUB



'Ermittelt die IP-Adresse einer Verbindung (anhand der PB-Dateinummer)
FUNCTION TCPAddr&(nr&)
  LOCAL hSocket&
  LOCAL SA AS sockaddr_in
  LOCAL sz&, ip&

  'PB-Dateinummer in Winsock Handle konvertieren
  hSocket& = FILEATTR(nr&, 2)

  'Informationen über den Socket auslesen
  ip& = 0
  sz& = SIZEOF(SA)
  IF GetPeername(hSocket&, SA, sz&) = 0 THEN
    ip& = SA.sin_addr.S_un.s_addr  'IP Adresse auslesen
  END IF

  TCPAddr& = ip&
END FUNCTION



'Berechnet die Checksummen aller Missionen im "MIS" Verzeichnis des Servers
SUB CalculateChecksumForAllMissions
  LOCAL missionnr&, f$, p$, a$
  DIM mapchecksums&(%MAXMISSIONS-1)

  p$ = EXEPATH$+"MIS\"
  f$ = DIR$(p$+"MISS*.DAT")
  WHILE f$ <> ""
    missionnr& = VAL(MID$(f$, 5, 3))
    IF missionnr& >= 0 AND missionnr& < %MAXMISSIONS AND UCASE$(f$) = "MISS"+FORMAT$(missionnr&, "000")+".DAT" THEN
      a$ = ReadFileContent("MIS\"+f$, 0)
      mapchecksums&(missionnr&) = CalculateChecksum&(a$)
    END IF
    f$ = DIR$
  WEND

  a$ = RESOURCE$(RCDATA, 100)
  mapchecksums&(%SUPERSECRETMISSION) = CalculateChecksum&(a$)
END SUB



'Server initialisieren
SUB InitServer(hWnd&)
  LOCAL ip&(), n&, i&
  DIM ip&(%MAXLOCALIPS-1)
  DIM hServer&(%MAXLOCALIPS-1)
  DIM connections(%MAXCONNECTIONS-1)
  DIM netbuffers$(%MAXCHANNELS-1)

  'globale Fehlerbehandlung aktivieren
'  hExceptionHandler& = AddVectoredExceptionHandler(1 , CODEPTR(GlobalErrorHandler&))

  'Artworks cachen
  n& = CacheArtworks&

  'alle IP-Adressen des Computers ermitteln
  CALL GetAllIPs(ip&(), n&)

  'Server starten
  FOR i& = 0 TO n&-1
    hServer&(i&) = FREEFILE
    TCP OPEN SERVER ADDR ip&(i&) PORT %SERVERPORT AS hServer&(i&) TIMEOUT 5000
    IF ERRCLEAR THEN
      CALL PrintError("Cannot create server socket. Another server already running?")
    ELSE
      TCP NOTIFY hServer&(I&), ACCEPT TO hWnd& AS %TCP_ACCEPT+i&
      CALL BILog("Server started at "+IPToString$(ip&(i&))+":"+FORMAT$(%SERVERPORT)+".", 0)
    END IF
  NEXT i&

  initDone& = 1
END SUB



'Sucht den ersten freien Client-Socket
FUNCTION FindFreeSocket&
  LOCAL i&

  FOR i& = 0 TO %MAXCONNECTIONS-1
    IF connections(i&).fnr = 0 THEN
      FindFreeSocket& = i&
      EXIT FUNCTION
    END IF
  NEXT i&

  FindFreeSocket& = -1
END FUNCTION



'Ermittelt das Socket-Handle für einen Spieler
FUNCTION GetPlayerSocket&(chnr&, plnr&)
  LOCAL cnr&

  FOR cnr& = 0 TO %MAXCONNECTIONS-1
    IF connections(cnr&).fnr > 0 AND connections(cnr&).chnr = chnr& AND connections(cnr&).snr = plnr& THEN
      GetPlayerSocket& = cnr&
      EXIT FUNCTION
    END IF
  NEXT cnr&

  GetPlayerSocket& = -1
END FUNCTION



'Schickt die Willkommens-Nachricht an einen Client
SUB SendHelloMessage(cnr&)
  LOCAL a$

  a$ = MKL$(today&)+MKL$(curtime&)+MKL$(%VERSION)+MKL$(connections(cnr&).securityquestion)+$SERVERINFO
  CALL SendPacketToClient(cnr&, "HELO", a$)
END SUB



'Weist eine Client-Verbindung ab
SUB SendServerOverloadPage(servernr&)
  LOCAL h&, a$

  'Verbindung annehmen
  h& = FREEFILE
  TCP ACCEPT hServer&(servernr&) AS h&

  'Fehlermeldung erstellen
  a$ = "RJCT"+MKL$(4)+MKL$(%WORD_SERVERFULL)

  'Antwort verschicken und Verschindung sofort schließen
  TCP SEND h&, a$
  TCP CLOSE h&
END SUB



'Schickt ein Datenpaket zu einem Client
SUB SendPacketToClient(cnr&, tp$, a$)
  IF cnr& < 0 THEN
    CALL BILog("Tried to send packet "+tp$+" to invalid connection ("+FORMAT$(cnr&)+")", 0)
    EXIT SUB
  END IF

  TCP SEND connections(cnr&).fnr, tp$+MKL$(LEN(a$))+a$
  CALL TrafficLog(connections(cnr&).chnr, -1, cnr&, tp$+a$)
END SUB



'Schickt ein Datenpaket an alle Clients in dem Channel
SUB SendPacketToChannel(chnr&, tp$, a$)
  LOCAL cnr&

  FOR cnr& = 0 TO %MAXCONNECTIONS-1
    IF connections(cnr&).fnr > 0 AND connections(cnr&).chnr = chnr& THEN CALL SendPacketToClient(cnr&, tp$, a$)
  NEXT cnr&
END SUB



'Schickt ein Datenpaket an alle Clients in der Lobby
SUB SendPacketToLobby(tp$, a$)
  LOCAL cnr&, chnr&

  'Paket an alle Spieler in einem Lobby-Channel oder ohne Channel schicken
  FOR cnr& = 0 TO %MAXCONNECTIONS-1
    IF connections(cnr&).fnr > 0 THEN
      chnr& = connections(cnr&).chnr
      IF chnr& < 0 OR channels(chnr&).info.state = %CHANNELSTATE_LOBBY THEN CALL SendPacketToClient(cnr&, tp$, a$)
    END IF
  NEXT cnr&
END SUB



'Client-Verbindung annehmen
SUB AcceptConnection(servernr&)
  LOCAL cnr&

  cnr& = FindFreeSocket&
  IF cnr& = -1 THEN  'maximale Verbindungsanzahl erreicht
    CALL SendServerOverloadPage(servernr&)
    CALL BILog("Rejecting connection (server full)", 0)
  ELSE
    connections(cnr&).fnr = FREEFILE
    TCP ACCEPT hServer&(servernr&) AS connections(cnr&).fnr
    TCP NOTIFY connections(cnr&).fnr, RECV CLOSE TO hWIN& AS %TCP_READ+cnr&
    connections(cnr&).ip = TCPAddr&(connections(cnr&).fnr)
    connections(cnr&).securityquestion = INT(RND*2100300400)
    connections(cnr&).securityanswer = CalcSecurityAnswer&(connections(cnr&).securityquestion, today&, curtime&)
    connections(cnr&).authentificated = 0
    connections(cnr&).chnr = -1
    connections(cnr&).snr = -1
    CALL SendHelloMessage(cnr&)
    CALL BILog("Client connected: "+IPToString(connections(cnr&).ip), 0)
  END IF
END SUB



'Client-Verbindung schließen
SUB CloseConnectionToClient(cnr&)
  LOCAL chnr&, plnr&, plname$

  'serverseitigen Socket schließen
  chnr& = connections(cnr&).chnr
  plnr& = connections(cnr&).snr
  IF chnr& >= 0 AND plnr& >= 0 THEN plname$ = GetPlayerName$(chnr&, plnr&)
  TCP CLOSE connections(cnr&).fnr
  connections(cnr&).fnr = 0
  connections(cnr&).chnr = -1
  connections(cnr&).snr = -1

  'Spieler aus Channel entfernen
  CALL DeleteEmptyChannels
  IF chnr& >= 0 THEN
    IF channels(chnr&).info.state = %CHANNELSTATE_LOBBY THEN
      CALL SendLobbyChannels(-1)
    END IF
  END IF

  CALL BILog("Client disconnected"+IIF$(plname$ = "", "", " ("+plname$+")"), 0)
  IF plnr& >= 0 AND channels(chnr&).info.state > %CHANNELSTATE_LOBBY THEN CALL SendPacketToChannel(chnr&, "DISC", CHR$(plnr&))
END SUB



'Alle Channels ohne Spieler löschen
SUB DeleteEmptyChannels
  LOCAL chnr&, cnr&

  'Verbindungszähler in allen Channels zurücksetzen
  FOR chnr& = 0 TO %MAXCHANNELS-1
    channels(chnr&).info.connectioncount = 0
  NEXT chnr&

  'aktive Verbindungen zählen
  FOR cnr& = 0 TO %MAXCONNECTIONS-1
    IF connections(cnr&).fnr > 0 AND connections(cnr&).chnr >= 0 THEN channels(connections(cnr&).chnr).info.connectioncount = channels(connections(cnr&).chnr).info.connectioncount+1
  NEXT cnr&

  'Channels ohne Verbindung löschen
  FOR chnr& = 0 TO %MAXCHANNELS-1
    IF channels(chnr&).info.connectioncount = 0 THEN
      IF channels(chnr&).info.state >= %CHANNELSTATE_INGAME THEN
        CALL StopRecordReplay(chnr&, 1)
        CALL APPLOG($APPNAME, logFilename$, "Deleting channel "+FORMAT$(chnr&)+" in turn "+FORMAT$(channels(chnr&).info.turn))
      END IF
      channels(chnr&).info.state = %CHANNELSTATE_EMPTY
    END IF
  NEXT chnr&
END SUB



'Ermittelt den nächsten freien Spielerplatz in einem Channel
FUNCTION GetNextFreeHumanSlot&(chnr&)
  LOCAL humanmask&, i&, plnr&

  'Spielerplätze insgesamt ermitteln
  humanmask& = channels(chnr&).info.originalplayers AND NOT channels(chnr&).info.aimask

  'besetzte Plätze entfernen
  FOR i& = 0 TO %MAXCONNECTIONS-1
    IF connections(i&).fnr > 0 AND connections(i&).chnr = chnr& THEN
      plnr& = connections(i&).snr
      humanmask& = humanmask& AND NOT (2^plnr&)
    END IF
  NEXT i&

  'ersten freien Platz suchen
  FOR plnr& = 0 TO %MAXPLAYERS-1
    IF (humanmask& AND (2^plnr&)) <> 0 THEN
      GetNextFreeHumanSlot& = plnr&
      EXIT FUNCTION
    END IF
  NEXT plnr&

  GetNextFreeHumanSlot& = -1
END FUNCTION



'Prüft, ob ein Channel voll ist
FUNCTION ChannelIsFull&(chnr&)
  ChannelIsFull& = channels(chnr&).info.connectioncount = CountBits&(channels(chnr&).info.originalplayers AND NOT channels(chnr&).info.aimask)
END FUNCTION



'Spieler anmelden
SUB ClientLogin(cnr&, q$)
  LOCAL clientdate&, clienttime&, clientversion&, securityanswer&, clientquestion&, playerxp&, playername$

  'Daten extrahieren
  clientdate& = CVL(q$, 1)
  clienttime& = CVL(q$, 5)
  clientversion& = CVL(q$, 9)
  securityanswer& = CVL(q$, 13)
  clientquestion& = CVL(q$, 17)
  playerxp& = CVL(q$, 21)
  playername$ = RTRIM$(MID$(q$, 25))

  'Client-Authentifizierung durchführen
  IF securityanswer& <> connections(cnr&).securityanswer THEN
    CALL SendPacketToClient(cnr&, "RJCT", MKL$(%WORD_INVALIDCLIENT))
    CALL CloseConnectionToClient(cnr&)
    EXIT SUB
  END IF
  IF clientversion& < %VERSION THEN
    CALL SendPacketToClient(cnr&, "RJCT", MKL$(%WORD_CLIENTOUTDATED))
    CALL CloseConnectionToClient(cnr&)
    EXIT SUB
  END IF
  connections(cnr&).authentificated = 1

  'Spieler aufnehmen
  connections(cnr&).xp = playerxp&
  connections(cnr&).playername = playername$
  CALL TrafficLog(-1, -1, -1, playername$+" logged in as "+FORMAT$(cnr&))
  CALL SendPacketToClient(cnr&, "WELC", MKL$(CalcSecurityAnswer&(clientquestion&, clientdate&, clienttime&)))
  CALL SendLobbyChannels(cnr&)
END SUB



'Setzt den Status aller Spieler in einem Channel zurück
SUB ResetPlayerState(chnr&)
  LOCAL plnr&

  FOR plnr& = 0 TO %MAXPLAYERS-1
    channels(chnr&).player(plnr&).state = %PLAYERSTATE_NONE
  NEXT plnr&
END SUB



'Ermittelt den Namen eines Spielers
FUNCTION GetPlayerName$(chnr&, plnr&)
  LOCAL i&, a$

  'Spielernamen anhand der Verbindung ermitteln
  FOR i& = 0 TO %MAXCONNECTIONS-1
    IF connections(i&).fnr > 0 AND connections(i&).chnr = chnr& AND connections(i&).snr = plnr& THEN
      GetPlayerName$ = connections(i&).playername
      EXIT FUNCTION
    END IF
  NEXT i&

  'Standardnamen verwenden
  GetPlayerName$ = defaultPlayernames$(plnr&)
END FUNCTION



'Startet den Countdown für ein Multiplayer Spiel
SUB StartMultiplayerGame(chnr&)
  LOCAL plnr&, ai&, a$, plname$

  'Countdown starten
  CALL ResetPlayerState(chnr&)
  channels(chnr&).info.countdown = 30
  CALL SendPacketToChannel(chnr&, "STGM", CHR$(3))

  'Spielerinformationen senden
  FOR plnr& = 0 TO %MAXPLAYERS-1
    IF (channels(chnr&).info.originalplayers AND 2^plnr&) <> 0 THEN
      ai& = IIF&((channels(chnr&).info.aimask AND 2^plnr&) = 0, 0, 1)
      plname$ = GetPlayerName$(chnr&, plnr&)
      a$ = a$+plname$+SPACE$(16-LEN(plname$))+CHR$(plnr&)+CHR$(channels(chnr&).player(plnr&).team)+CHR$(ai&)
    END IF
  NEXT plnr&
  CALL SendPacketToChannel(chnr&, "PLYR", a$)
END SUB



'Wartet bis alle Spieler bereit sind (die Mission geladen und initialisiert haben)
SUB WaitPlayersReady(chnr&)
  channels(chnr&).info.state = %CHANNELSTATE_WAITREADY
END SUB



'Startet ein Multiplayer Spiel
SUB SwitchChannelToIngame(chnr&)
  LOCAL a$, i&, plnr&

  a$ = "Starting map "+mapnames$(channels(chnr&).info.currentmission)+" , players "
  FOR i& = 0 TO %MAXCONNECTIONS-1
    IF connections(i&).fnr > 0 AND connections(i&).chnr = chnr& THEN
      plnr& = connections(i&).snr
      a$ = a$+connections(i&).playername+","
    END IF
  NEXT i&
  a$ = RTRIM$(a$, ",")+" in channel "+FORMAT$(chnr&)
  CALL APPLOG($APPNAME, logFilename$, a$)

  CALL ResetPlayerState(chnr&)
  channels(chnr&).info.state = %CHANNELSTATE_INGAME
  channels(chnr&).info.activeteam = 1
  CALL SendPacketToChannel(chnr&, "ACTT", CHR$(1))
  CALL CheckActions(chnr&)
END SUB



'Beendet ein Multiplayer Spiel
SUB ServerGameOver(chnr&, winningteam&)
  LOCAL plnr&, cnr&, msgid&, victory&

  'Sieg/Niederlage-Nachricht an alle Spieler schicken
  FOR plnr& = 0 TO %MAXPLAYERS-1
    IF (channels(chnr&).info.originalplayers AND 2^plnr&) <> 0 AND (channels(chnr&).info.aimask AND 2^plnr&) = 0 THEN
      cnr& = GetPlayerSocket&(chnr&, plnr&)
      IF channels(chnr&).player(plnr&).team = winningteam& THEN
        CALL SendGameMessage(chnr&, plnr&, %MSG_VICTORY)
        IF cnr& >= 0 THEN CALL SendPacketToClient(cnr&, "CHST", CHR$(%CHANNELSTATE_VICTORY))
        victory& = 1
      ELSE
        CALL SendGameMessage(chnr&, plnr&, %MSG_DEFEAT)
        IF cnr& >= 0 THEN CALL SendPacketToClient(cnr&, "CHST", CHR$(%CHANNELSTATE_DEFEAT))
      END IF
    END IF
  NEXT plnr&

  'Channel-Zustand setzen
  channels(chnr&).info.state = IIF&(victory& = 0, %CHANNELSTATE_DEFEAT, %CHANNELSTATE_VICTORY)
END SUB



'Prüft, ob alle Spieler des aktiven Teams ihren Zug beendet haben
FUNCTION AllPlayersFinishedTurn&(chnr&)
  LOCAL plnr&, aliveplayers&, activeteam&

  activeteam& = channels(chnr&).info.activeteam
  aliveplayers& = channels(chnr&).info.aliveplayers
  FOR plnr& = 0 TO %MAXPLAYERS-1
    IF (aliveplayers& AND 2^plnr&) <> 0 AND channels(chnr&).player(plnr&).team = activeteam& AND channels(chnr&).player(plnr&).state <> %PLAYERSTATE_READY THEN EXIT FUNCTION
  NEXT plnr&

  AllPlayersFinishedTurn& = -1
END FUNCTION



'Karte starten
FUNCTION ServerInitMap&(chnr&, missionnr&, difficulty&)
  LOCAL episode&

  'Mission laden
  episode& = GetEpisodeForMap&(missionnr&)
  IF missionnr& >= 0 AND LoadMission&("MIS\MISS"+FORMAT$(missionnr&, "000")+".DAT", episode&, difficulty&, chnr&) <= 0 THEN EXIT FUNCTION
  channels(chnr&).info.difficulty = difficulty&

  'Siegbedingungen für Versus-Modus hinzufügen
  IF GetMultiplayerMode&(chnr&) = %MULTIPLAYERMODE_VERSUS AND channels(chnr&).info.nvictoryconditions = 0 THEN CALL CreateDefaultWinConditions(chnr&)

  'Sicht berechnen
  CALL UpdateSightBlock(chnr&)
  CALL UpdateVisionAll(chnr&)

  'Replay Aufzeichnung initalisieren
  CALL StartRecordReplay(chnr&)

  ServerInitMap& = -1
END FUNCTION



'Erstellt einen neuen Channel
SUB ServerCreateChannel(cnr&, q$, fromSavegame&)
  LOCAL chnr&, missionnr&, plnr&, difficulty&, checksum&, createthreads&
  LOCAL mcode$, chname$

  'Daten extrahieren
  mcode$ = RTRIM$(LEFT$(q$, 7))
  chname$ = RTRIM$(MID$(q$, 8, 32))

  'Daten validieren
  IF fromSavegame& = 0 THEN
    difficulty& = ASC(q$, 40)
    checksum& = CVL(q$, 41)
    missionnr& = GetMissionNumber&(mcode$)
    IF missionnr& < 0 THEN
      CALL SendPacketToClient(cnr&, "RJCT", MKL$(%WORD_UNKNOWN_MAP))
      EXIT SUB
    END IF
  END IF

  'Channel erstellen (falls Spieler nicht bereits in einem Channel ist und nun einen Spielstand hinzufügen möchte)
  IF connections(cnr&).chnr = -1 AND fromSavegame& = 0 THEN
    FOR chnr& = 0 TO %MAXCHANNELS-1
      IF channels(chnr&).info.state = %CHANNELSTATE_EMPTY THEN EXIT FOR
    NEXT chnr&
    IF chnr& = %MAXCHANNELS THEN
      CALL SendPacketToClient(cnr&, "RJCT", MKL$(%WORD_SERVERFULL))
      EXIT SUB
    END IF
    createthreads& = 1
  ELSE
    chnr& = connections(cnr&).chnr
  END IF

  IF fromSavegame& = 0 THEN
    'Mission laden
    IF ServerInitMap&(chnr&, missionnr&, difficulty&) = 0 THEN
      CALL SendPacketToClient(cnr&, "RJCT", MKL$(%WORD_INVALID_MISSIONFILE))
      channels(chnr&).info.state = %CHANNELSTATE_EMPTY
      EXIT SUB
    END IF
    IF channels(chnr&).info.checksum <> checksum& THEN
      CALL SendPacketToClient(cnr&, "RJCT", MKL$(%WORD_MAP_MISMATCH))
      channels(chnr&).info.state = %CHANNELSTATE_EMPTY
      EXIT SUB
    END IF
  ELSE
    'Spielstand laden
    IF SetFullChannelData&(chnr&, MID$(q$, 40)) = 0 THEN
      CALL SendPacketToClient(cnr&, "RJCT", MKL$(%WORD_SAVEGAME_INVALID))
      channels(chnr&).info.state = %CHANNELSTATE_EMPTY
      EXIT SUB
    END IF
    CALL ServerInitMap&(chnr&, -1, channels(chnr&).info.difficulty)
  END IF
  channels(chnr&).info.cname = chname$
  channels(chnr&).info.state = %CHANNELSTATE_LOBBY
  channels(chnr&).info.fromSavegame = fromSavegame&
  netbuffers$(chnr&) = ""
  channels(chnr&).info.netbufferaccess = CreateSemaphore(BYVAL %NULL, 1, 1, BYVAL %NULL)
'  channels(chnr&).info.netbufferaccess = 0

  'Host zum Channel hinzufügen
  IF fromSavegame& = 0 THEN
    plnr& = GetNextFreeHumanSlot&(chnr&)
  ELSE
    plnr& = channels(chnr&).info.localplayer
  END IF
  connections(cnr&).chnr = chnr&
  connections(cnr&).snr = plnr&
  channels(chnr&).info.connectioncount = 1
  channels(chnr&).info.countdown = 255

'CALL BILog("Channel created: "+FORMAT$(fromSavegame&), 0)

  'Channel-Threads erzeugen
  IF createthreads& = 1 THEN
'    THREAD CREATE CommunicationThread&(chnr&) TO channels(chnr&).info.hCommunicationThread
    IF channels(chnr&).info.aimask > 0 THEN THREAD CREATE AIThread&(chnr&) TO channels(chnr&).info.hAIThread
  END IF

  CALL SendPacketToClient(cnr&, "ETRD", CHR$(chnr&)+CHR$(plnr&)+CHR$(channels(chnr&).player(plnr&).team)+MKL$(channels(chnr&).info.currentmission)+CHR$(channels(chnr&).info.difficulty))
  CALL SendLobbyChannels(-1)
END SUB



'Spieler tritt Channel bei
SUB JoinChannel(cnr&, q$)
  LOCAL chnr&, plnr&

  'prüfen, ob Channel gültig ist
  chnr& = ASC(q$)
  IF chnr& < 0 OR chnr& >= %MAXCHANNELS OR channels(chnr&).info.state <> %CHANNELSTATE_LOBBY THEN
    CALL SendPacketToClient(cnr&, "RJCT", MKL$(%WORD_INVALIDCHANNEL))
    EXIT SUB
  END IF

  'freien Slot für den Spieler suchen
  plnr& = GetNextFreeHumanSlot&(chnr&)
  IF plnr& < 0 THEN
    CALL SendPacketToClient(cnr&, "RJCT", MKL$(%WORD_CHANNELFULL))
    EXIT SUB
  END IF
  connections(cnr&).chnr = chnr&
  connections(cnr&).snr = plnr&
  channels(chnr&).info.connectioncount = channels(chnr&).info.connectioncount+1
  CALL SendPacketToClient(cnr&, "ETRD", CHR$(chnr&)+CHR$(plnr&)+CHR$(channels(chnr&).player(plnr&).team)+MKL$(channels(chnr&).info.currentmission)+CHR$(channels(chnr&).info.difficulty))
  IF channels(chnr&).info.fromSavegame = 1 THEN
    CALL SendPacketToClient(cnr&, "LOAD", GetFullChannelData$(chnr&))
  END IF
  CALL SendLobbyChannels(-1)

  'prüfen, ob Spiel gestartet werden kann
  IF ChannelIsFull&(chnr&) THEN CALL StartMultiplayerGame(chnr&)
END SUB



'Ermittelt die Art des Mehrspielermodus (0=Coop , 1=Versus)
FUNCTION GetMultiplayerMode&(chnr&)
  LOCAL humanmask&, maxplayers&, team&, plnr&, gamemode&

  humanmask& = channels(chnr&).info.originalplayers AND NOT channels(chnr&).info.aimask
  maxplayers& = CountBits&(humanmask&)
  gamemode& = %MULTIPLAYERMODE_COOP
  team& = 0

  'falls alle menschlichen Spieler dasselbe Team haben, dann Coop sonst Versus
  FOR plnr& = 0 TO %MAXPLAYERS-1
    IF (humanmask& AND (2^plnr&)) <> 0 THEN
      IF team& = 0 THEN
        team& = channels(chnr&).player(plnr&).allymask
      ELSE
        IF team& <> channels(chnr&).player(plnr&).allymask THEN gamemode& = %MULTIPLAYERMODE_VERSUS
      END IF
    END IF
  NEXT plnr&

  GetMultiplayerMode& = gamemode&
END FUNCTION



'Aktualisiert die Informationen über alle Lobby-Channels
SUB SendLobbyChannels(cnr&)
  LOCAL a$, m$, plinfo$, chnr&, plnr&, i&, nplayers&, humanmask&, maxplayers&, gamemode&

  FOR chnr& = 0 TO %MAXCHANNELS-1
    IF channels(chnr&).info.state = %CHANNELSTATE_LOBBY THEN
      'Channel befindet sich im Lobby-Zustand
      plinfo$ = ""
      nplayers& = 0
      FOR i& = 0 TO %MAXCONNECTIONS-1
        IF connections(i&).fnr > 0 AND connections(i&).chnr = chnr& THEN
          nplayers& = nplayers&+1
          plnr& = connections(i&).snr
          plinfo$ = plinfo$+connections(i&).playername+SPACE$(16-LEN(connections(i&).playername))+CHR$(plnr&)+CHR$(channels(chnr&).player(plnr&).team)+CHR$(connections(i&).xp)
        END IF
      NEXT i&
      humanmask& = channels(chnr&).info.originalplayers AND NOT channels(chnr&).info.aimask
      maxplayers& = CountBits&(humanmask&)
      gamemode& = GetMultiplayerMode&(chnr&)
      m$ = mapnames$(channels(chnr&).info.currentmission)
      a$ = a$+CHR$(chnr&)+m$+SPACE$(7-LEN(m$))+channels(chnr&).info.cname+SPACE$(32-LEN(channels(chnr&).info.cname))+CHR$(channels(chnr&).info.difficulty)+CHR$(nplayers&)+CHR$(maxplayers&)+CHR$(gamemode&)+plinfo$
    END IF
  NEXT chnr&

  IF cnr& < 0 THEN
    CALL SendPacketToLobby("CHAN", a$)
  ELSE
    CALL SendPacketToClient(cnr&, "CHAN", a$)
  END IF
END SUB



'Farbe/Team eines Spielers auf die nächste freie Farbe ändern
SUB AssignNextColorToPlayer(cnr&)
  LOCAL chnr&, plnr&

  'prüfen, ob Channel noch im Lobby-Modus ist
  chnr& = connections(cnr&).chnr
  IF channels(chnr&).info.state <> %CHANNELSTATE_LOBBY THEN EXIT SUB

  'nächste freie Farbe suchen
  plnr& = GetNextFreeHumanSlot&(chnr&)
  IF plnr& < 0 THEN EXIT SUB
  connections(cnr&).snr = plnr&

  CALL SendLobbyChannels(-1)
END SUB



'Spieler ist bereit
SUB SetPlayerReady(cnr&)
  LOCAL chnr&, plnr&

  chnr& = connections(cnr&).chnr
  plnr& = connections(cnr&).snr
  IF chnr& < 0 OR plnr& < 0 THEN EXIT SUB
  channels(chnr&).player(plnr&).state = %PLAYERSTATE_READY
END SUB



'Spieler hat seinen Zug beenden
SUB ClientFinishedTurn(cnr&)
  LOCAL chnr&, plnr&

  'Zug-Beendet-Flag setzen
  chnr& = connections(cnr&).chnr
  plnr& = connections(cnr&).snr
  channels(chnr&).player(plnr&).state = %PLAYERSTATE_READY

  'prüfen, ob alle Spieler des aktiven Teams ihren Zug beendet haben
  IF AllPlayersFinishedTurn&(chnr&) THEN
    CALL EndTurn(chnr&)
    CALL SendPacketToChannel(chnr&, "ACTT", CHR$(channels(chnr&).info.activeteam))
  END IF
END SUB



'Client hat Bewegungs/Kampf-Animation beendet
SUB ClientFinishedAnimation(cnr&, q$)
  LOCAL chnr&, plnr&, unitnr1&, unitnr2&

  'Animation-Beendet-Flag setzen
  chnr& = connections(cnr&).chnr
  plnr& = connections(cnr&).snr
  unitnr1& = CVL(q$, 1)
  unitnr2& = CVL(q$, 5)
  channels(chnr&).player(plnr&).animationfinished = 1

  'Einheiten beim Client aktualisieren
  IF unitnr1& >= 0 THEN CALL UpdateUnitToClient(chnr&, cnr&, unitnr1&)
  IF unitnr2& >= 0 THEN CALL UpdateUnitToClient(chnr&, cnr&, unitnr2&)

  'prüfen, ob Überraschungsangriff vorhanden ist
  IF unitnr1& >= 0 AND channels(chnr&).units(unitnr1&).owner = plnr& AND (channels(chnr&).units(unitnr1&).flags AND %US_SURPRISED) <> 0 THEN
    CALL ServerSurpriseAttack(chnr&, unitnr1&)
  END IF
  IF unitnr2& >= 0 AND channels(chnr&).units(unitnr2&).owner = plnr& AND (channels(chnr&).units(unitnr2&).flags AND %US_SURPRISED) <> 0 THEN
    CALL ServerSurpriseAttack(chnr&, unitnr2&)
  END IF
END SUB



'Setzt das Animations-Flag aller Clients zurück
SUB ResetClientAnimationFlag(chnr&)
  LOCAL plnr&

  FOR plnr& = 0 TO %MAXPLAYERS-1
    channels(chnr&).player(plnr&).animationfinished = 0
  NEXT plnr&
END SUB



'Prüft ob alle Clients die Animation beendet haben
FUNCTION CheckAllClientsFinishedAnimation&(chnr&)
  LOCAL plnr&, humanmask&, donecount&

  humanmask& = channels(chnr&).info.originalplayers AND NOT channels(chnr&).info.aimask
  FOR plnr& = 0 TO %MAXPLAYERS-1
    IF channels(chnr&).player(plnr&).animationfinished > 0 THEN donecount& = donecount&+1
  NEXT plnr&

  CheckAllClientsFinishedAnimation& = donecount& >= CountBits&(humanmask&)
END FUNCTION



'Wartet bis alle Clients die Animation beendet haben
SUB WaitUntilClientsFinishedAnimation(chnr&)
  LOCAL plnr&, humanmask&, donecount&, tmout!

  tmout! = TIMER+5
  DO
    humanmask& = channels(chnr&).info.originalplayers AND NOT channels(chnr&).info.aimask
    donecount& = 0
    FOR plnr& = 0 TO %MAXPLAYERS-1
      IF channels(chnr&).player(plnr&).animationfinished > 0 THEN donecount& = donecount&+1
    NEXT plnr&
    SLEEP 10
  LOOP UNTIL donecount& >= CountBits&(humanmask&) OR TIMER > tmout!
END SUB



'Chat-Nachricht an Spieler schicken
SUB ForwardChatMessage(sendercnr&, q$)
  LOCAL chnr&, plnr&, plnr2&, cnr&, receiver&, a$

  'Daten extrahieren
  IF LEN(q$) < 2 OR LEN(q$) > 161 THEN EXIT SUB
  chnr& = connections(sendercnr&).chnr
  plnr& = connections(sendercnr&).snr
  receiver& = ASC(q$)
  a$ = CHR$(plnr&)+MID$(q$, 2)

  'Nachricht verteilen
  FOR cnr& = 0 TO %MAXCONNECTIONS-1
    IF connections(cnr&).fnr > 0 AND connections(cnr&).chnr = chnr& THEN
      plnr2& = connections(cnr&).snr
      IF receiver& = 0 OR channels(chnr&).player(plnr&).team = channels(chnr&).player(plnr2&).team THEN CALL SendPacketToClient(cnr&, "CHAT", a$)
    END IF
  NEXT cnr&
END SUB



'Setzt das Fertig-Flag bei allen bewegten Einheiten eines Spielers (da der Server keine UnselectUnit Benachrichtigung erhält)
SUB FlagAllMovedUnitsDone(chnr&, plnr&, exceptionunitnr&)
  LOCAL unitnr&, flags&

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF unitnr& <> exceptionunitnr& AND UnitIsAlive&(chnr&, unitnr&) <> 0 AND channels(chnr&).units(unitnr&).owner = plnr& THEN
      flags& = channels(chnr&).units(unitnr&).flags
      IF (flags& AND %US_MOVED) <> 0 AND (flags& AND %US_DONE) = 0 THEN
        CALL UnitDone(chnr&, unitnr&)
        CALL UpdateUnitToAllClients(chnr&, unitnr&)
      END IF
    END IF
  NEXT unitnr&
END SUB



'Spieler (oder KI) bewegt eine Einheit
SUB ServerMoveUnit(cnr&, BYVAL chnr&, BYVAL plnr&, q$)
  LOCAL unitnr&, x&, y&, p&, oldx&, oldy&, endx&, endy&, unr&, shopnr&
  LOCAL targetunits&()

  'Daten extrahieren
  IF cnr& >= 0 THEN
    chnr& = connections(cnr&).chnr
    plnr& = connections(cnr&).snr
  END IF
  unitnr& = CVL(q$, 1)
  x& = ASC(q$, 5)
  y& = ASC(q$, 6)

  'Daten validieren
  IF unitnr& < 0 OR unitnr& >= channels(chnr&).info.nunits THEN CALL PrintError("MoveUnitError: Bad Unit "+FORMAT$(unitnr&)) : EXIT SUB
  IF channels(chnr&).units(unitnr&).owner <> plnr& THEN CALL PrintError("MoveUnitError: Unit "+FORMAT$(unitnr&)+" wrong owner "+FORMAT$(channels(chnr&).units(unitnr&).owner)) : EXIT SUB
  IF (channels(chnr&).units(unitnr&).flags AND %US_DONE) <> 0 THEN CALL PrintError("MoveUnitError: Unit done "+FORMAT$(unitnr&)+" (PL:"+FORMAT$(plnr&)+" , Flags:"+FORMAT$(channels(chnr&).units(unitnr&).flags)+")") : EXIT SUB
  IF x& < 0 OR x& >= channels(chnr&).info.xsize OR y& < 0 OR y& >= channels(chnr&).info.ysize THEN CALL PrintError("MoveUnitError: Unit "+FORMAT$(unitnr&)+ "bad field "+FORMAT$(x&)+","+FORMAT$(y&)) : EXIT SUB
  IF channels(chnr&).info.activeteam <> channels(chnr&).player(plnr&).team THEN CALL PrintError("MoveUnitError: Unit "+FORMAT$(unitnr&)+" not your turn") : EXIT SUB

  'prüfen, ob Feld erreichbar ist
  oldx& = channels(chnr&).units(unitnr&).xpos
  oldy& = channels(chnr&).units(unitnr&).ypos
  CALL ClearTargets(chnr&, plnr&)
  CALL GetTargets&(chnr&, unitnr&, 1, 0, targetunits&())
  IF (channels(chnr&).player(plnr&).targets(x&, y&) AND %TG_MOVE) <> %TG_MOVE THEN
    CALL PrintError("MoveUnitError: Unit "+FORMAT$(unitnr&)+" field "+FORMAT$(x&)+","+FORMAT$(y&)+" not reachable from "+FORMAT$(oldx&)+","+FORMAT$(oldy&)+" : "+FORMAT$(channels(chnr&).zone3(x&, y&)))
    EXIT SUB
  END IF

  'alle anderen Einheiten des Spielers als Fertig markieren, falls diese schon bewegt wurden aber nicht angegriffen haben
  CALL FlagAllMovedUnitsDone(chnr&, plnr&, unitnr&)

  'Einheit auf den Clients bewegen
  CALL ResetClientAnimationFlag(chnr&)
  CALL SendPacketToChannel(chnr&, "MOVE", q$)

  'Einheit auf dem Server bewegen
  CALL CalculateUnitPath(chnr&, unitnr&, x&, y&, 0)
  IF channels(chnr&).zone3(oldx&, oldy&) = unitnr& THEN
    channels(chnr&).zone3(oldx&, oldy&) = -1
  ELSE
    unr& = channels(chnr&).zone3(oldx&, oldy&)
    IF unr& >= 0 THEN
      CALL TransportUnload(chnr&, unr&, unitnr&)
      CALL UpdateUnitToAllClients(chnr&, unr&)
    ELSE
      shopnr& = -2-unr&
      IF shopnr& >= 0 THEN
        CALL RemoveUnitFromShop(chnr&, shopnr&, unitnr&)
        CALL UpdateShopToClients(chnr&, shopnr&)
      END IF
    END IF
  END IF

  'neue Bewegungsrichtung ermitteln
  p& = channels(chnr&).player(plnr&).unitpathlen
  IF p& > 1 THEN
    oldx& = channels(chnr&).player(plnr&).unitpathx(p&-2)
    oldy& = channels(chnr&).player(plnr&).unitpathy(p&-2)
    endx& = channels(chnr&).player(plnr&).unitpathx(p&-1)
    endy& = channels(chnr&).player(plnr&).unitpathy(p&-1)
    channels(chnr&).units(unitnr&).direction = GetDirectionToField&(oldx&, oldy&, endx&, endy&)
  END IF

  CALL EndMovement(chnr&, unitnr&, x&, y&)
END SUB



'Spieler greift mit einer Einheit an
SUB ServerAttackUnit(cnr&, q$)
  LOCAL chnr&, plnr&, attacker&, defender&, attweapon&, attunittp&, dist&, z&, owner&
  LOCAL w AS TWeapon

  'Daten extrahieren
  chnr& = connections(cnr&).chnr
  plnr& = connections(cnr&).snr
  attacker& = CVL(q$, 1)
  defender& = CVL(q$, 5)
  attweapon& = ASC(q$, 9)

  'Daten validieren
  IF attweapon& < 1 OR attweapon& > 4 THEN CALL BILog("AttackUnit: Invalid weapon number", 0) : EXIT SUB
  IF attacker& < 0 OR attacker& >= channels(chnr&).info.nunits THEN CALL BILog("AttackUnit: Invalid attack unit", 0) : EXIT SUB
  IF channels(chnr&).units(attacker&).owner <> plnr& THEN CALL BILog("AttackUnit: Invalid attack player", 0) : EXIT SUB
  IF defender& < 0 OR defender& >= channels(chnr&).info.nunits THEN CALL BILog("AttackUnit: Invalid defend unit", 0) : EXIT SUB
  IF channels(chnr&).units(defender&).owner = plnr& THEN CALL BILog("AttackUnit: Invalid depend player", 0) : EXIT SUB
  IF (channels(chnr&).units(attacker&).flags AND %US_DONE) <> 0 THEN CALL BILog("AttackUnit: Unit done", 0) : EXIT SUB
  IF channels(chnr&).info.activeteam <> channels(chnr&).player(plnr&).team THEN CALL BILog("AttackUnit: Invalid team", 0) : EXIT SUB

  'Waffe prüfen
  attunittp& = channels(chnr&).units(attacker&).unittype
  w = channelsnosave(chnr&).unitclasses(attunittp&).weapons(attweapon&-1)
  z& = 2^channels(chnr&).units(defender&).zpos
  dist& = GetDistance&(channels(chnr&).units(attacker&).xpos, channels(chnr&).units(attacker&).ypos, channels(chnr&).units(defender&).xpos, channels(chnr&).units(defender&).ypos)
  IF channels(chnr&).units(attacker&).ammo(attweapon&-1) = 0 OR w.damage = 0 THEN CALL BILog("AttackUnit: W"+FORMAT$(attweapon&)+" Ammo "+FORMAT$(channels(chnr&).units(attacker&).ammo(attweapon&-1))+" , Damage "+FORMAT$(w.damage), 0) : EXIT SUB
  IF w.minrange > dist& OR w.maxrange < dist& OR (w.targets AND z&) = 0 THEN CALL BILog("AttackUnit: Bad attack range or target", 0) : EXIT SUB

  'alle anderen Einheiten des Spielers als Fertig markieren, falls diese schon bewegt wurden aber nicht angegriffen haben
  CALL FlagAllMovedUnitsDone(chnr&, plnr&, attacker&)

  'Kampf durchführen
  CALL SetCombatData(chnr&, attacker&, defender&, attweapon&, IIF&(dist& = 1, -1, 0), 0)
  CALL ResetClientAnimationFlag(chnr&)
  CALL SendPacketToChannel(chnr&, "ATCK", PEEK$(VARPTR(channels(chnr&).combat), SIZEOF(TCombatInfo)))
  owner& = channels(chnr&).units(attacker&).owner
  IF owner& < %MAXPLAYERS AND channels(chnr&).player(owner&).team = channels(chnr&).info.activeteam AND (channelsnosave(chnr&).unitclasses(attunittp&).flags AND %UCF_RETREAT) = 0 THEN CALL UnitDone(chnr&, attacker&)
  CALL EndCombat(chnr&, channels(chnr&).combat)
END SUB



'Überraschungsangriff durchführen
SUB ServerSurpriseAttack(chnr&, surprisedunitnr&)
  LOCAL attacker&, owner&, mask&

  attacker& = FindSurpriseAttacker&(chnr&, surprisedunitnr&)
  IF attacker& < 0 THEN
    channels(chnr&).units(surprisedunitnr&).flags = channels(chnr&).units(surprisedunitnr&).flags AND NOT %US_SURPRISED
    EXIT SUB
  END IF
  owner& = channels(chnr&).units(surprisedunitnr&).owner
  mask& = %US_SURPRISE_ATTACK1*2^owner&
  channels(chnr&).units(attacker&).flags = channels(chnr&).units(attacker&).flags AND NOT mask&

  'Kampf durchführen
  CALL SetCombatData(chnr&, attacker&, surprisedunitnr&, -1, -1, 0)
  CALL ResetClientAnimationFlag(chnr&)
  CALL SendPacketToChannel(chnr&, "ATCK", PEEK$(VARPTR(channels(chnr&).combat), SIZEOF(TCombatInfo)))
  CALL EndCombat(chnr&, channels(chnr&).combat)
END SUB



'Spieler benutzt Aktion einer Einheit
SUB ServerUnitAction(cnr&, q$)
  LOCAL chnr&, plnr&, unitnr&, unittp&, action&, x&, y&, targetunitnr&
  LOCAL targetunits&()

  'Daten extrahieren
  chnr& = connections(cnr&).chnr
  plnr& = connections(cnr&).snr
  unitnr& = CVL(q$, 1)
  action& = ASC(q$, 5)
  x& = ASC(q$, 6)
  y& = ASC(q$, 7)

  'Daten validieren
  IF unitnr& < 0 OR unitnr& >= channels(chnr&).info.nunits THEN EXIT SUB
  IF channels(chnr&).units(unitnr&).owner <> plnr& THEN EXIT SUB
  IF action& <> %UNITACTION_ASCEND AND action& <> %UNITACTION_DESCEND THEN
    IF (channels(chnr&).units(unitnr&).flags AND %US_DONE) <> 0 THEN EXIT SUB
    IF x& < 0 OR x& >= channels(chnr&).info.xsize OR y& < 0 OR y& >= channels(chnr&).info.ysize THEN EXIT SUB
  END IF
  IF channels(chnr&).info.activeteam <> channels(chnr&).player(plnr&).team THEN EXIT SUB

  'alle anderen Einheiten des Spielers als Fertig markieren, falls diese schon bewegt wurden aber nicht angegriffen haben
  CALL FlagAllMovedUnitsDone(chnr&, plnr&, unitnr&)

  'prüfen, ob Ziel gültig ist
  targetunitnr& = -1
  CALL ClearTargets(chnr&, plnr&)
  CALL GetTargets&(chnr&, unitnr&, 2, 0, targetunits&())
  unittp& = channels(chnr&).units(unitnr&).unittype

  'Aktion ausführen
  CALL UnitAction(chnr&, plnr&, unitnr&, action&, x&, y&)
END SUB



'Spieler benutzt Aktion eines Shops
SUB ServerShopAction(cnr&, q$)
  LOCAL chnr&, plnr&, shopnr&, action&, target&, newunit&

  'Daten extrahieren
  chnr& = connections(cnr&).chnr
  plnr& = connections(cnr&).snr
  shopnr& = CVL(q$, 1)
  action& = ASC(q$, 5)
  target& = CVL(q$, 6)

  'Daten validieren
  IF shopnr& < 0 OR shopnr& >= channels(chnr&).info.nshops THEN EXIT SUB
  IF channels(chnr&).shops(shopnr&).owner <> plnr& THEN EXIT SUB
  IF channels(chnr&).info.activeteam <> channels(chnr&).player(plnr&).team THEN EXIT SUB
  IF action& = %SHOPACTION_TRAIN AND channels(chnr&).shops(shopnr&).shoptype <> %SHOPTYPE_ACADEMY THEN EXIT SUB
  IF action& = %SHOPACTION_BUILD THEN
    IF target& < 0 OR target& >= channelsnosave(chnr&).nunitclasses THEN EXIT SUB
    IF INSTR(GetShopProductionMenu$(chnr&, shopnr&, 0, 1), CHR$(target&)) = 0 THEN EXIT SUB
  ELSE
    IF target& < 0 OR target& >= channels(chnr&).info.nunits THEN EXIT SUB
    IF UnitIsInShop&(chnr&, target&) <> shopnr& THEN EXIT SUB
  END IF

  'Aktion ausführen
  SELECT CASE action&
  CASE %SHOPACTION_REFUEL
    'Treibstoff/Munition der Zieleinheit auffüllen
    CALL RefuelInShop(chnr&, shopnr&, target&)
    CALL UpdatePlayerToClients(chnr&, plnr&)
    CALL UpdateShopToClients(chnr&, shopnr&)
    CALL UpdateUnitToAllClients(chnr&, target&)
  CASE %SHOPACTION_REPAIR
    'Zieleinheit reparieren
    CALL RepairInShop(chnr&, shopnr&, target&)
    CALL UpdatePlayerToClients(chnr&, plnr&)
    CALL UpdateShopToClients(chnr&, shopnr&)
    CALL UpdateUnitToAllClients(chnr&, target&)
  CASE %SHOPACTION_BUILD
    'Einheit produzieren
    newunit& = BuildUnit&(chnr&, shopnr&, target&)
    IF newunit& < 0 THEN EXIT SUB
    CALL UpdatePlayerToClients(chnr&, plnr&)
    CALL UpdateShopToClients(chnr&, shopnr&)
    CALL UpdateUnitToAllClients(chnr&, newunit&)
  CASE %SHOPACTION_TRAIN
    'Einheit trainieren
    CALL TrainInShop(chnr&, shopnr&, target&)
    CALL UpdatePlayerToClients(chnr&, plnr&)
    CALL UpdateShopToClients(chnr&, shopnr&)
    CALL UpdateUnitToAllClients(chnr&, target&)
  END SELECT
END SUB



'Einheit auf allen Clients zerstören
SUB DestroyUnitOnClients(chnr&, unitnr&, killingplayer&, killreason&)
  CALL SendPacketToChannel(chnr&, "KILL", MKL$(unitnr&)+MKL$(killingplayer&)+MKL$(killreason&))
END SUB



'Einheit bei einem Client aktualisieren
SUB UpdateUnitToClient(chnr&, cnr&, BYVAL unitnr&)
  CALL SendPacketToClient(cnr&, "UPDU", MKL$(unitnr&)+PEEK$(VARPTR(channels(chnr&).units(unitnr&)), SIZEOF(TUnit)))
END SUB



'Einheit bei allen Clients aktualisieren
SUB UpdateUnitToAllClients(chnr&, BYVAL unitnr&)
  CALL SendPacketToChannel(chnr&, "UPDU", MKL$(unitnr&)+PEEK$(VARPTR(channels(chnr&).units(unitnr&)), SIZEOF(TUnit)))
END SUB



'Shop bei allen Client aktualisieren
SUB UpdateShopToClients(chnr&, shopnr&)
  CALL SendPacketToChannel(chnr&, "UPDS", MKL$(shopnr&)+PEEK$(VARPTR(channels(chnr&).shops(shopnr&)), SIZEOF(TShop)))
END SUB



'Spieler bei allen Clients aktualisieren
SUB UpdatePlayerToClients(chnr&, plnr&)
  LOCAL a$

  a$ = CHR$(plnr&)+CHR$(channels(chnr&).player(plnr&).allymask)+CHR$(channels(chnr&).player(plnr&).team)+MKL$(channels(chnr&).player(plnr&).energy)+MKL$(channels(chnr&).player(plnr&).selectedunit)
  CALL SendPacketToChannel(chnr&, "UPDP", a$)
END SUB



'Kartenfeld bei allen Clients aktualisieren
SUB UpdateZoneToClients(chnr&, x&, y&)
  LOCAL a$

  a$ = CHR$(x&)+CHR$(y&)+MKI$(channels(chnr&).zone1(x&, y&))+MKI$(channels(chnr&).zone2(x&, y&))+MKI$(channels(chnr&).zone3(x&, y&))
  CALL SendPacketToChannel(chnr&, "UPDZ", a$)
END SUB



'Channelinfo bei allen Clients aktualisieren
SUB UpdateInfoToClients(chnr&, ofs&, d$)
  LOCAL a$

  a$ = MKI$(ofs&)+d$
  CALL SendPacketToChannel(chnr&, "UPDI", a$)
END SUB



'Wetter bei allen Clients aktualisieren
SUB UpdateWeatherToClients(chnr&, newweather&, startrow&, endrow&)
  CALL SendPacketToChannel(chnr&, "WEAT", CHR$(newweather&)+CHR$(startrow&)+CHR$(endrow&))
END SUB



'AI-Fortschritt bei allen Clients aktualisieren
SUB SendAIProgress(chnr&, plnr&, curprogress&, totalprogress&)
  LOCAL a$

  a$ = CHR$(plnr&)+MKL$(curprogress&)+MKL$(totalprogress&)
  CALL SendPacketToChannel(chnr&, "AIPR", a$)
END SUB



'Gesamte Channeldaten zum Client schicken
SUB ResyncClient(cnr&)
  LOCAL chnr&

  chnr& = connections(cnr&).chnr
  CALL SendPacketToClient(cnr&, "SYNC", GetFullChannelData$(chnr&))
END SUB



'Client Spielmeldung anzeigen lassen
SUB SendGameMessage(chnr&, plnr&, msgid&)
  LOCAL cnr&

  cnr& = GetPlayerSocket&(chnr&, plnr&)
  IF cnr& >= 0 THEN CALL SendPacketToClient(cnr&, "GMSG", MKL$(msgid&))
END SUB



'Highscore durch Client aktualisieren
SUB UpdateGlobalHighscore(cnr&, q$)
  LOCAL plname$, u$, i&, missionnr&, difficulty&, turn&, scoreground&, scorewater&, scoreair&, totalscore&, checksum&

  CALL EnterSemaphore(semaphore_highscore&)

  'Daten extrahieren
  plname$ = connections(cnr&).playername
  u$ = UCASE$(plname$)+SPACE$(16-LEN(plname$))
  missionnr& = CVL(q$, 1)
  turn& = CVL(q$, 5)
  scoreground& = CVL(q$, 9)
  scorewater& = CVL(q$, 13)
  scoreair& = CVL(q$, 17)
  totalscore& = scoreground&+scorewater&+scoreair&
  difficulty& = ASC(q$, 21)
  checksum& = CVL(q$, 22)

  'prüfen, ob Missionsnummer gültig ist und Mission nicht manipuliert wurde
  IF missionnr& < 0 OR missionnr& >= %MAXMISSIONS OR mapchecksums&(missionnr&) <> checksum& OR turn& >= 9999 THEN
    CALL LeaveSemaphore(semaphore_highscore&)
    CALL BILog("Invalid highscore update request", 0)
    EXIT SUB
  END IF

  'Eintrag für diesen Spieler/Mission/Schwierigkeit suchen
  FOR i& = 0 TO highscoreSize&-1
    IF highscoreTable(i&).missionnumber > missionnr& THEN
      'Eintrag nicht gefunden (Einträge sind nach Missionsnummer sortiert)
      REDIM PRESERVE highscoreTable(highscoreSize&)
      ARRAY INSERT highscoreTable(i&)
      highscoreSize& = highscoreSize&+1
      EXIT FOR
    END IF
    IF highscoreTable(i&).missionnumber = missionnr& AND highscoreTable(i&).difficulty = difficulty& AND UCASE$(highscoreTable(i&).playername) = u$ THEN
      'prüfen, ob neuer Punktestand besser ist als alter
      IF turn& > highscoreTable(i&).turnnumber OR (turn& = highscoreTable(i&).turnnumber AND totalscore& <= highscoreTable(i&).scoreground+highscoreTable(i&).scorewater+highscoreTable(i&).scoreair) THEN
        CALL LeaveSemaphore(semaphore_highscore&)
        EXIT SUB
      END IF
      EXIT FOR
    END IF
  NEXT i&
  IF i& = highscoreSize& THEN
    REDIM PRESERVE highscoreTable(i&)
    ARRAY INSERT highscoreTable(i&)
    highscoreSize& = highscoreSize&+1
  END IF

  'Eintrag aktualisieren
  highscoreTable(i&).playername = plname$
  highscoreTable(i&).missionnumber = missionnr&
  highscoreTable(i&).turnnumber = turn&
  highscoreTable(i&).scoreground = scoreground&
  highscoreTable(i&).scorewater = scorewater&
  highscoreTable(i&).scoreair = scoreair&
  highscoreTable(i&).difficulty = difficulty&
  highscoreTable(i&).scoredate = today&

  'Highscore speichern
  u$ = PEEK$(VARPTR(highscoreTable(0)), SIZEOF(THighScore)*highscoreSize&)
  CALL WriteFileContent($HIGHSCOREFILE, u$)
  CALL APPLOG($APPNAME, logFilename$, "New highscore for "+mapnames$(missionnr&)+": "+plname$+" turn "+FORMAT$(turn&))

  CALL LeaveSemaphore(semaphore_highscore&)
END SUB



'Datei zum Client schicken
SUB SendFileToClient(cnr&, q$)
  LOCAL fnr&, i&, a$, f$

  'Artwork zum Dateinamen suchen
  ARRAY SCAN artworkCacheNames$(0) FOR artworkCacheSize&, COLLATE UCASE, =q$, TO fnr&
  IF fnr& > 0 THEN
    f$ = "ARTWORK\"+artworkCacheNames$(fnr&-1)+".gif"
    a$ = artworkCache$(fnr&-1)
  ELSE
    CALL APPLOG($APPNAME, logFilename$, "Client requested unknown file: "+q$)
  END IF

  IF LEN(a$) > 0 THEN CALL SendPacketToClient(cnr&, "FILE", CHR$(LEN(f$))+MKL$(LEN(a$))+f$+a$)
END SUB



'Highscore für eine Mission an den Client schicken
SUB ServerSendHighscore(cnr&, q$)
  LOCAL a$, i&, p&, missionnr&

  CALL EnterSemaphore(semaphore_highscore&)

  'alle Einträge zu dieser Missionsnummer ermitteln
  missionnr& = CVL(q$)
  p& = -1
  FOR i& = 0 TO highscoreSize&-1
    IF highscoreTable(i&).missionnumber > missionnr& THEN EXIT FOR  'Einträge sind nach Missionsnummer sortiert
    IF highscoreTable(i&).missionnumber = missionnr& AND p& = -1 THEN p& = i&
  NEXT i&
  IF p& >= 0 THEN
    a$ = PEEK$(VARPTR(highscoreTable(p&)), SIZEOF(THighScore)*(i&-p&))
  END IF

  'Einträge an Client schicken
  CALL SendPacketToClient(cnr&, "HSCO", q$+a$)

  CALL LeaveSemaphore(semaphore_highscore&)
END SUB



'Ping beantworten
SUB ReplyPing(cnr&, q$)
  CALL SendPacketToClient(cnr&, "PING", q$)
END SUB



'Checksummen für Channel berechnen
SUB DebugSendChecksums(cnr&)
  LOCAL chnr&

  chnr& = connections(cnr&).chnr
  CALL SendPacketToClient(cnr&, "DCKS", DebugCalculateChannelChecksum$(chnr&))
END SUB



'Spielinformationen für ServerApp erstellen
FUNCTION CreateServerAppStatistics$
  LOCAL cnr&, chnr&, nch&, npl&, maps$

  CALL DeleteEmptyChannels

  'Anzahl Verbindungen (Spieler im Spiel und Lobby) ermitteln
  FOR cnr& = 0 TO %MAXCONNECTIONS-1
    IF connections(cnr&).fnr > 0 THEN npl& = npl&+1
  NEXT cnr&

  'Anzahl Channel und Missionsnamen ermitteln
  FOR chnr& = 0 TO %MAXCHANNELS-1
    IF channels(chnr&).info.state > %CHANNELSTATE_EMPTY THEN
      nch& = nch&+1
      maps$ = maps$+maps$+mapnames$(channels(chnr).info.currentmission)
    END IF
  NEXT chnr&

  CreateServerAppStatistics$ = MKI$(nch&)+MKI$(npl&)+LEFT$(maps$, 56)
END FUNCTION



'Verarbeitet Anfragen, die über das AppLog Interface gestellt werden
SUB ProcessServerApplicationQueries
  LOCAL m$, d$
  LOCAL ch&, pl&

  m$ = APPREADMESSAGE$(-1)
  d$ = MID$(m$, 5)

  SELECT CASE LEFT$(m$, 4)
  CASE "STAT":  'Server-Status
    CALL APPSENDMESSAGE(CreateServerAppStatistics$, -1)

  CASE "QUIT"  'Server beenden
    exitprg& = 1
    PostQuitMessage 0

  END SELECT
END SUB



'Semaphore für Zugriff auf Channel-Puffer setzen
SUB SetNetBufferSemaphore(chnr&)
  LOCAL r&

  'warten bis kein anderer Thread mehr auf den Puffer zugreift
'  DO
'    r& = WaitForSingleObject(channels(chnr&).info.netbufferaccess, 0)
'  LOOP UNTIL r& = %WAIT_OBJECT_0

'  WHILE channels(chnr&).info.netbufferaccess <> 0
'    SLEEP 0
'  WEND
'  channels(chnr&).info.netbufferaccess = 1
END SUB



'Semaphore für Zugriff auf Channel-Puffer wieder zurücksetzen
SUB ResetNetBufferSemaphore(chnr&)
'  channels(chnr&).info.netbufferaccess = 0
'  ReleaseSemaphore(channels(chnr&).info.netbufferaccess, 1 , BYVAL %NULL)
END SUB



'Fügt vom Client gelesene Daten dem Channel-Puffer hinzu
SUB AddClientData(cnr&, q$)
  LOCAL chnr&, l&

  chnr& = connections(cnr&).chnr
  CALL SetNetBufferSemaphore(chnr&)
'  validtp$ = "*HELO*LIST*CRTG*LOAD*JOIN*COLR*REDY*MOVE*ATCK*UNAC*SHAC*ENDT*ENDA*CHAT*SYNC*HSCO*GSCO*QCKS*GETF*PING*"
'l& = LEN(netbuffers$(chnr&))
  netbuffers$(chnr&) = netbuffers$(chnr&)+MKL$(cnr&)+MKL$(LEN(q$))+q$
'CALL BILog("Append "+FORMAT$(LEN(q$))+" bytes to buffer: "+FORMAT$(l&)+" -> "+FORMAT$(LEN(netbuffers$(chnr&))), 0)
  CALL ResetNetBufferSemaphore(chnr&)
END SUB



'Daten aus Client-Socket lesen
SUB ReadClientData(cnr&)
  LOCAL chnr&, q$

  'Daten lesen
  TCP RECV connections(cnr&).fnr, 64000, q$
  IF q$ = "" THEN EXIT SUB

'CALL BILog("Read "+FORMAT$(LEN(q$))+" bytes", 0)
'CALL APPLOG($APPNAME, logFilename$, "Received "+FORMAT$(LEN(q$))+" bytes "+LEFT$(q$, 4))
  CALL TrafficLog(connections(cnr&).chnr, cnr&, -1, LEFT$(q$, 4))

  chnr& = connections(cnr&).chnr
  IF chnr& < 0 THEN
    'solange Channel noch im Lobby Modus ist, Daten direkt (im Haupt-Thread) verarbeiten
    CALL ProcessClientQuery(cnr&, q$)
  ELSE
    'Daten zum Puffer hinzufügen
    CALL AddClientData(cnr&, q$)
    'alle vollständigen Pakete verarbeiten
    DO
      q$ = FetchNextPacket$(chnr&)
      IF LEN(q$) = 0 THEN EXIT LOOP
      CALL ProcessClientQuery(cnr&, MID$(q$, 5))
    LOOP
  END IF
END SUB



'Liefert das nächste vollständige Datenpaket aus der Warteschlange
FUNCTION FetchNextPacket$(chnr&)
  LOCAL p&, cnr&, startcnr&, packetnr&, packetlen&, datalen&, removefrombuffer&, q$, r$, tp$

  'prüfen ob mindestens 1 vollständiges Paket vorhanden ist (mindestens 16 Bytes, da CNR:32,PacketLen:32,PacketTyp:32,DataLen:32,Data:*)
  IF LEN(netbuffers$(chnr&)) < 16 THEN EXIT FUNCTION

  'Paket lesen
  r$ = ""
  startcnr& = -1
  p& = 1
  CALL SetNetBufferSemaphore(chnr&)
  IF LEN(netbuffers$(chnr&)) < 16 THEN
    CALL ResetNetBufferSemaphore(chnr&)
    EXIT FUNCTION
  END IF
  DO
    cnr& = CVL(netbuffers$(chnr&), p&)
    packetlen& = CVL(netbuffers$(chnr&), p&+4)
    q$ = MID$(netbuffers$(chnr&), p&+8, packetlen&)
    removefrombuffer& = 0
    IF packetnr& = 0 THEN
      'beim ersten Paket die Paketart prüfen
      tp$ = LEFT$(q$, 4)
      r$ = q$
      datalen& = CVL(q$, 5)
      packetnr& = packetnr&+1
      removefrombuffer& = 1
      startcnr& = cnr&
    ELSE
      IF cnr& = startcnr& THEN
        r$ = r$+q$
        packetnr& = packetnr&+1
        removefrombuffer& = 1
      END IF
    END IF

    'falls Paket vom ersten Absender stammt, dann Paket aus dem Puffer entfernen - sonst überspringen
    IF removefrombuffer& = 1 THEN
      netbuffers$(chnr&) = LEFT$(netbuffers$(chnr&), p&-1)+MID$(netbuffers$(chnr&), p&+packetlen&+8)
    ELSE
      p& = p&+8+packetlen&
    END IF

    'prüfen, ob Paket vollständig ist
    IF packetnr& > 0 AND LEN(r$) >= datalen&+8 THEN
      'falls Datenstrom mehr als ein Paket enthält, alles ab dem zweiten Paket wieder zum Puffer hinzufügen
      IF LEN(r$) > datalen&+8 THEN
        q$ = MID$(r$, datalen&+9)
        netbuffers$(chnr&) = netbuffers$(chnr&)+MKL$(startcnr&)+MKL$(LEN(q$))+q$
        r$ = LEFT$(r$, datalen&+8)
      END IF
      CALL ResetNetBufferSemaphore(chnr&)
      FetchNextPacket$ = MKL$(cnr&)+r$
      EXIT FUNCTION
    END IF

  LOOP UNTIL p& > LEN(netbuffers$(chnr&))

  'unvollständiges Paket wieder zum Puffer hinzufügen
  IF packetnr& > 0 THEN
    netbuffers$(chnr&) = netbuffers$(chnr&)+MKL$(startcnr&)+MKL$(LEN(r$))+r$
CALL ResetNetBufferSemaphore(chnr&)
'CALL DebugPrintNetBuffers(chnr&)
'CALL BILog("Incomplete packet for con "+FORMAT$(cnr&)+" , PacketCount="+FORMAT$(packetnr&)+" , BufferLen="+FORMAT$(LEN(netbuffers$(chnr&))), 0)
  END IF

  'kein passendes Paket gefunden
  CALL ResetNetBufferSemaphore(chnr&)
  FetchNextPacket$ = ""
END FUNCTION



'Kommunikations-Thread für kurze Anfragen
THREAD FUNCTION CommunicationThread&(BYVAL chnr&)
  LOCAL cnr&, q$

'  DO
    'prüfen, ob neuen Daten eingegangen sind
'    q$ = FetchNextPacket$(chnr&)
'    IF LEN(q$) > 0 THEN
'      cnr&= CVL(q$)
'      CALL ProcessClientQuery(cnr&, MID$(q$, 5))
'    ELSE
'      SLEEP 0
'    END IF
'  LOOP WHILE exitprg& = 0 AND channels(chnr&).info.state > %CHANNELSTATE_EMPTY
END FUNCTION



'Prüft, ob ein Datenpaket von einem Client gültig ist
FUNCTION ValidateClientPacket&(cnr&, tp$, q$)
  LOCAL paketlen&, inchannel&, reqchannelstate&, chnr&, r&

  'Socket-Nummer prüfen
  IF cnr& < 0 OR cnr& >= %MAXCONNECTIONS THEN EXIT FUNCTION

  'Authentifizierung prüfen
  IF tp$ <> "HELO" AND connections(cnr&).authentificated = 0 THEN EXIT FUNCTION

  'erforderliche Paketeigenschaften
  SELECT CASE tp$
  CASE "HELO": paketlen& = 40
  CASE "LIST": paketlen& = 0
  CASE "CRTG": paketlen& = 44
  CASE "LOAD": paketlen& = -2
  CASE "JOIN": paketlen& = 1
  CASE "COLR": paketlen& = 0 : inchannel& = 1
  CASE "REDY": paketlen& = 0 : inchannel& = 1
  CASE "MOVE": paketlen& = 6 : inchannel& = 1 : reqchannelstate& = %CHANNELSTATE_INGAME
  CASE "ATCK": paketlen& = 9 : inchannel& = 1 : reqchannelstate& = %CHANNELSTATE_INGAME
  CASE "UNAC": paketlen& = 7 : inchannel& = 1 : reqchannelstate& = %CHANNELSTATE_INGAME
  CASE "SHAC": paketlen& = 9 : inchannel& = 1 : reqchannelstate& = %CHANNELSTATE_INGAME
  CASE "ENDT": paketlen& = 0 : inchannel& = 1 : reqchannelstate& = %CHANNELSTATE_INGAME
  CASE "ENDA": paketlen& = 8 : inchannel& = 1 : reqchannelstate& = %CHANNELSTATE_INGAME
  CASE "CHAT": paketlen& = -2 : inchannel& = 1 : reqchannelstate& = %CHANNELSTATE_INGAME
  CASE "SYNC": paketlen& = 0 : inchannel& = 1 : reqchannelstate& = %CHANNELSTATE_INGAME
  CASE "HSCO": paketlen& = 25
  CASE "GSCO": paketlen& = 4
  CASE "QCKS": paketlen& = 0 : inchannel& = 1 : reqchannelstate& = %CHANNELSTATE_INGAME
  CASE "GETF": paketlen& = -2
  CASE "PING": paketlen& = 8
  CASE ELSE: paketlen& = -1
  END SELECT

  'Paketlänge und Channel-Zustand prüfen
  r& = -1
  chnr& = connections(cnr&).chnr
  IF LEN(q$) <> paketlen& AND paketlen& <> -2 THEN
    r& = 0
  ELSE
    IF inchannel& = 1 AND chnr& < 0 THEN
      r& = 0
    ELSE
      IF reqchannelstate& <> %CHANNELSTATE_EMPTY AND channels(chnr&).info.state < reqchannelstate& THEN r& = 0
    END IF
  END IF

  ValidateClientPacket& = r&
END FUNCTION



'Beantwortet eine Client-Anfrage
SUB ProcessClientQuery(cnr&, q$)
  LOCAL tp$, datalen&, chnr&, starttime#, endtime#

  'Paket validieren
  IF LEN(q$) < 8 THEN EXIT SUB
  tp$ = LEFT$(q$, 4)
  datalen& = CVL(q$, 5)
  IF LEN(q$) <> datalen&+8 THEN EXIT SUB
  q$ = MID$(q$, 9, datalen&)
  IF ValidateClientPacket&(cnr&, tp$, q$) = 0 THEN
    CALL PrintError("Packet validation failed: "+tp$+" ("+FORMAT$(datalen&)+") , Channel: "+FORMAT$(connections(cnr&).chnr)+" , State: "+FORMAT$(IIF&(connections(cnr&).chnr < 0, -999, channels(connections(cnr&).chnr).info.state)))
CALL APPLOG($APPNAME, logFilename$, "Packet validation failed: "+tp$+" ("+FORMAT$(datalen&)+") , Channel: "+FORMAT$(connections(cnr&).chnr)+" , State: "+FORMAT$(IIF&(connections(cnr&).chnr < 0, -999, channels(connections(cnr&).chnr).info.state)))
    chnr& = connections(cnr&).chnr
    IF chnr& >= 0 AND channels(chnr&).info.state = %CHANNELSTATE_INGAME THEN
      CALL ResyncClient(cnr&)
    ELSE
      CALL CloseConnectionToClient(cnr&)
    END IF
    EXIT SUB
  END IF

'PRINT cnr&; tp$

  'Anfrage auswerten
  starttime# = TIMER
  SELECT CASE tp$
  CASE "HELO": CALL ClientLogin(cnr&, q$)
  CASE "CRTG": CALL ServerCreateChannel(cnr&, q$, 0)
  CASE "LOAD": CALL ServerCreateChannel(cnr&, q$, 1)
  CASE "JOIN": CALL JoinChannel(cnr&, q$)
  CASE "COLR": CALL AssignNextColorToPlayer(cnr&)
  CASE "REDY": CALL SetPlayerReady(cnr&)
  CASE "MOVE": CALL ServerMoveUnit(cnr&, -1, -1, q$)
  CASE "ATCK": CALL ServerAttackUnit(cnr&, q$)
  CASE "UNAC": CALL ServerUnitAction(cnr&, q$)
  CASE "SHAC": CALL ServerShopAction(cnr&, q$)
  CASE "ENDT": CALL ClientFinishedTurn(cnr&)
  CASE "ENDA": CALL ClientFinishedAnimation(cnr&, q$)
  CASE "CHAT": CALL ForwardChatMessage(cnr&, q$)
  CASE "SYNC": CALL ResyncClient(cnr&)
  CASE "HSCO": CALL UpdateGlobalHighscore(cnr&, q$)
  CASE "GSCO": CALL ServerSendHighscore(cnr&, q$)
  CASE "QCKS": CALL DebugSendChecksums(cnr&)
  CASE "GETF": CALL SendFileToClient(cnr&, q$)
  CASE "PING": CALL ReplyPing(cnr&, q$)
  END SELECT
  endtime# = TIMER
  IF serverLogPackets& <> 0 THEN CALL BILog(FORMAT$(starttime#, "* ######.000")+" "+tp$+" ("+FORMAT$(INT((endtime#-starttime#)*1000))+" ms)", (cnr& AND 7)+8)
END SUB
