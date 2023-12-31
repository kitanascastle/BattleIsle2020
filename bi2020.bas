'Battle Isle 2020 - Hauptpogramm / Benutzersteuerung / Darstellung

#DIM ALL
#CONSOLE OFF
#DEBUG ERROR OFF
#DEBUG DISPLAY OFF
#TOOLS OFF
#STACK 4194304

#INCLUDE "..\DXLib\direct2dlib.inc"
#INCLUDE "..\DXLib\directsoundlib.inc"
#INCLUDE "..\DXLib\CAfxMp3.inc"
#INCLUDE "..\DXLib\avilib.inc"
#INCLUDE "WIN32API.INC"
#INCLUDE "IPHLPAPI.INC"
#INCLUDE "..\SPEECH.INC"
%WINAPI = 1  'verhindern, daß APPLOG.INC Typen aus der WinAPI selber definiert
#INCLUDE "APPLOG.INC"
#INCLUDE "BI2CONST.INC"
#INCLUDE "BI2TYPES.INC"

#RESOURCE ICON,    50, "Resources\progamicon.ico"
#RESOURCE RCDATA, 100, "Resources\SSECRET.DAT"
#RESOURCE RCDATA, 101, "Resources\SSECRET.GER"
#RESOURCE RCDATA, 102, "Resources\SSECRET.ENG"


'System
GLOBAL EXEPATH$
GLOBAL today&                  'aktuelles Datum im Format 16:8:8
GLOBAL curtime&                'Sekunden seit Mitternacht
GLOBAL configfilename$
GLOBAL D2D AS IDIRECT2D
GLOBAL DS AS ICDIRECTSOUND
GLOBAL pWindow AS IWindow
GLOBAL hWIN&
GLOBAL hInitThread&            'Handle des Initialisierungs-Threads
GLOBAL hClientUpdateThread&    'Handle des Client-Update-Threads
GLOBAL hExceptionHandler&      'Handle der globalen Fehlerbehandlung
GLOBAL fullscreenMode&
GLOBAL windowWidth&, windowHeight&
GLOBAL uiscale!
GLOBAL mousexpos&, mouseypos&, mousedownx&, mousedowny&, currentArea&
GLOBAL dragStartX&, dragStartY&
GLOBAL exitprg&                'wird auf 1 gesetzt, wenn das Programm beendet wird
GLOBAL languageNr&             'Nummer der gewählte Sprache
GLOBAL langcode$               'Code gewählte Sprache (GER oder ENG)
GLOBAL supportedLanguages$()   'unterstützte Sprachen
GLOBAL words$$()               'im Spiel verwendete Beschriftungen
GLOBAL gametime!               'Zeit in Sekunden (verwendet für animierte Darstellung)
GLOBAL lasttimeupdate!
GLOBAL lastCampaignTimeUpdate!
GLOBAL serverConsoleUpdateTime!
GLOBAL terrainAnimationSpeed!  'Millisekunden pro Animationsschritt
GLOBAL overlayAnimationSpeed!  'Millisekunden pro Animationsschritt
GLOBAL shopAnimationSpeed!     'Millisekunden pro Animationsschritt
GLOBAL orgConfigData$          'Konfigurationsdaten (zum Vergleich, ob der Benutzer etwas geändert hat)
GLOBAL checkInstallation&      'prüft zu Beginn, ob alle Blue Byte Dateien vorhanden sind
GLOBAL dataModified&           'wird auf 1 gesetzt, wenn der Spielstand noch nicht gespeichert ist
GLOBAL startupaction&          'Aktions die beim Starten des Programms ausgeführt werden soll
GLOBAL startupMap$             'Missionscode der durch Kommandozeilenparameter gewählen Karte
GLOBAL logFilename$            'vollständiger Pfad+Dateiname des Server-Logfiles
GLOBAL aviFolder$              'Pfad zu den BI3 Videos
GLOBAL serverConsoleMode&      'Serverkonsole anzeigen: 0 = nein, 1 = Fenster mit Spielerübersicht , 2 = nur Protokoll
GLOBAL screenShoting&          'wird auf 1 gesetzt, wenn gerade ein Screenshot erstellt wird (um Deadlocks zu verhindern)
GLOBAL enableDebugLog&         'Debug-Logging aktivieren wenn auf 1 gesetzt

'Spiel
GLOBAL gameState&              'Spiel Status
GLOBAL gameMode&               'Spielmodus (Einzel/Mehrspieler/Server)
GLOBAL localPlayerName$        'Name des lokalen Spielers
GLOBAL localPlayerXP&          'Erfahrungspunkte des lokalen Spielers
GLOBAL localPlayerNr&          'Nummer des lokalen Spielers
GLOBAL localPlayerTeam&        'Team des lokalen Spielers
GLOBAL localPlayerMask&        'Bitmaske des lokalen Spielers (2^localPlayerNr&)
GLOBAL localPlayerHQ&          'Shop-Nummer des Hauptquartier des lokalen Spielers (oder -1 falls nicht vorhanden)
GLOBAL playerColors$           'Farbindexes der 7 Spieler
GLOBAL dialogueClosing&        'wenn auf 1 gesetzt, wird der aktive Dialog geschlossen
GLOBAL unitSelectionTime!      'Zeitpunkt an dem eine Einheit ausgewählt wurde (nur für Selektions-Animation)
GLOBAL combatStartTime!        'Zeitpunkt an dem der Kampf begann (nur für Kampf-Animation)
GLOBAL combatSoundEffects&     'Flags für Kampf-Sound-Effekte
GLOBAL selectedShop&           'Nummer des ausgewählten (geöffneten) Shops
GLOBAL selectedProduction&     'Ausgewählte Produktion im geöffneten Shop (Einheiten-ID)
GLOBAL productionPreviewUnit&  'Temporäre Einheit zur Vorschau der Prodution
GLOBAL selectedShopProd$       'Produktionsmenü des ausgewählten (geöffneten) Shops
GLOBAL shopSelectionTime!      'Zeitpunkt an dem ein Shop ausgewählt wurde (nur für Selektions-Animation)
GLOBAL shopAnimationUnit&      'Einheit, die im Shop bearbeitet wird (nur für Animation)
GLOBAL shopAnimationType&      'Aktion, die im Shop ausgeführt wird (nur für Animation)
GLOBAL shopAnimationTime!      'Animationsschritt für Einheiten-Aktion im Shop (und Startzeitpunkt der Animation)
GLOBAL updateMiniMap&          'wird auf 1 gesetzt, wenn die Minimap aktualisiert werden muß
GLOBAL defaultDifficulty&      'Schwierigkeitsgrad für neue Spiele
GLOBAL mapnames$()             'Codes für die Missionen der aktuellen Episode
GLOBAL mapchecksums&()         'Checksummen für alle Missionen
GLOBAL mapinfoOpenTime!        'Zeitpunkt an dem die Karteninfo geöffnet wurde
GLOBAL missionTextRows$$()     'Missionstexte in Zeilen passend zur Breite des Karteninfo-Fensters
GLOBAL playernames$()          'Namen der Spieler (nur Mehrspieler)
GLOBAL defaultPlayernames$()   'Standardnamen für Spieler ohne Namen
GLOBAL defeatCondition&        'Nummer der Siegbedingung, die erfüllt wurde und zur Niederlage führte
GLOBAL mapscore$()             'erreichte Punkte in allen Missionen
GLOBAL highscorePacket$        'Datenpaket mit Highscore für den Server
GLOBAL highscoreTable() AS THighScore  'Highscore Tabelle (nur Server)
GLOBAL highscoreSize&          'Anzahl Einträge in der Highscore Tabelle
GLOBAL showProtocol&           'wenn auf 1 gesetzt, dann Protokoll anzeigen statt Spielfeld
GLOBAL gamedataChanged&        'wird auf 1 gesetzt, wenn sich die Spieldaten ändern und das Spiel noch nicht gespeichert wurde
GLOBAL unitInfoOverlay&        'Einheiten-Informationen direkt neben der Einheit auf dem Spielfeld anzeigen
GLOBAL combatMode&             'gewählte Kampfdarstellung
GLOBAL unitAnimFrameWidth&     'Breite eines Einheiten-Animations-Frames in Pixeln
GLOBAL unitAnimFrameHeight&    'Höhe eines Einheiten-Animations-Frames in Pixeln
GLOBAL unitAnimFPS&            'Einheiten-Animations-Bilder pro Sekunde
GLOBAL orgMissionData$         'vollständige Missionsdaten (der MISSxxx.DAT Datei)
GLOBAL orgShopNames$           'vollständige Shopnamen (der MISSxxx.TXT Datei)
GLOBAL mapRandomSeed&          'Initialisierungswert des Zufallsgenerators
GLOBAL lastOccupiedShop$       'Name des zuletzt eingenommenen Shops (egal von welchem Spieler)
GLOBAL autoResync&             'wenn auf 1 gesetzt, dann Client automatisch mit Server synchronisieren falls Checksummenfehler
GLOBAL unitMovementStartTime!  'Zeitpunkt an dem eine Einheit ihre Bewegung begann (wird genutzt, um die Bewegung zu beenden, falls dies während einer Replay-Wiedergabe nicht automatisch passiert)
GLOBAL loadingScreenUnitType&  'am Ladebildschirm angezeigte Einheit
GLOBAL unitListByXP$           'Einheitenliste nach Erfahrungspunkten (nur für Mapinfo Dialog)
GLOBAL unitListByType$         'Einheitenliste nach Type (nur für Mapinfo Dialog)
GLOBAL tabbedUnits$            'Einheitenliste, die mit der Tab-Taste angewählt wurden
GLOBAL lastSelectedTransporter&  'zuletzt angewählter Transporter

'Ladebildschirm
GLOBAL initProgress&           'Lade-Fortschritt
GLOBAL initProgressText$$      'Anzeige, was gerade geladen wird
GLOBAL initDone&               'Bit-Maske für abgeschlossene Lade-Vorgänge

'Zwischensequenzen
GLOBAL cutSceneObjects() AS TCutSceneObject
GLOBAL nCutSceneObjects&       'Anzahl animierter Zwischensequenz-Objekte
GLOBAL cutSceneNumber&         'Nummer der Zwischensequenz
GLOBAL cutSceneStartTime!      'Zeitpunkt an dem die Zwischensequenz gestartet wurde
GLOBAL lastCutSceneObjCreationTime!  'Zeitpunkt an dem zuletzt ein Zwischensequenz-Objekte erzeugt wurde
GLOBAL lastCutSceneUpdateTime! 'Zeitpunkt an dem die Zwischensequenz zuletzt aktualisiert wurde
GLOBAL cutSceneTextStartTime!  'Zeitpunkt an dem die Darstellung der Textzeile begann
GLOBAL cutSceneTextEndTime!    'Zeitpunkt zum Löschen der Zeile und Beginn der nächsten Zeile inklusive Pause am Ende der Darstellung
GLOBAL cutSceneTextSkip&       'Anzahl im Text auszulassender Zeichen
GLOBAL cutSceneScrollPos!      'Pixel-Position des linken Rands der Zwischensequenz-Darstellung
GLOBAL cutsceneCurrentTextLine$$  'Text (inklusive Farb-Steuerzeichen) der aktuell darzustellnenden Zeile
GLOBAL isVideoCutscene&        '0 = BI2 Zwischensequenz (Sidescroller) , 1 = BI3 Video Zwischensequenz

'Semaphoren
GLOBAL semaphore_unitmoving&   'schützt "channels(chnr&).player(plnr&).selectedunit" + "channels(chnr&).player(plnr&).unitpathlen"
GLOBAL semaphore_highscore&    'schützt "highscoreTable()"
GLOBAL semaphore_crttexture&   'verhindert daß neue Texturen erzeugt werden, während die Ausgabe gerendert wird
GLOBAL semaphore_scrollpos&    'schützt "scollX&" und "scrollY&", damit diese nicht durch "CreateScreenshot" verändert werden während "ScrollToMapPos" aufgerufen wird

'Debug
GLOBAL debugEnabled&           'wenn auf 1 gesetzt, dann Debug-Befehle erlauben
GLOBAL debugInfo&              'allgemeine Debug-Informationen anzeigen
GLOBAL debugShowUnits&         'Einheiten-IDs auf dem Spielfeld anzeigen
GLOBAL debugNoFog&             'gesamte Karte anzeigen (inklusive nicht aufgedeckter/überwachter Felder)
GLOBAL debugShowUnitList&      'Einheiten-Liste anzeigen
GLOBAL debugShowChannelInfo&   'Channel-Info anzeigen
GLOBAL debugChecksums&         'Client-Checksummen mit Server-Checksummen vergleichen
GLOBAL lastChecksumUpdate!     'Zeitpunkt der letzten QCKS Anfrage (alle 2 Sekunden)
GLOBAL debugServerChecksums$   'Serverantwort auf die letzte QCKS Anfrage
GLOBAL serverLogPackets&       'Client-Pakete in der Serverkonsole anzeigen

'Cheats
GLOBAL cheatUnlimitedTurns&    'kein Rundenlimit
GLOBAL cheatCombatPreview&     'Kampfvorschau auf jedem Schwierigkeitsgrad anzeigen

'Fonts
GLOBAL hSystemFont&             'Arial 9
GLOBAL hSmallWeaponFont&        'Arial 9 Bold
GLOBAL hWeaponFont&             'Arial 10 Bold
GLOBAL hCaptionFont&            'Arial 16 Bold
GLOBAL hBigCaptionFont&         'Arial 60 Bold
GLOBAL hCreditFont&             'Arial 50 Bold
GLOBAL hShopCaptionFont&        'Arial 20 Bold
GLOBAL hHallOfFameCaptionFont&  'Arial 28 Bold
GLOBAL hMenuFont&               'Arial 15
GLOBAL hGameMessageFont&        'Arial 13
GLOBAL hLobbyCaptionFont&       'Arial 11 Bold

'Pinsel
GLOBAL brushBlack&             '0/0/0
GLOBAL brushWhite&             '255/255/255
GLOBAL brushLightGrey&         '192/192/192
GLOBAL brushRed&               '255/0/0
GLOBAL brushDarkRed&           '128/0/0
GLOBAL brushBronze&            '224/128/32
GLOBAL brushSilver&            '208/208/208
GLOBAL brushGold&              '255/192/64
GLOBAL brushBlue&              '0/0/255
GLOBAL brushBlueTransparent&   '0/0/255 / 128
GLOBAL brushGoldTransparent&   '255/192/64 / 128
GLOBAL brushMenuBackground&    '64/64/64 / 128
GLOBAL brushMenuHighlight&     '255/255/255 / 128
GLOBAL brushMenuBorder&        '32/32/32
GLOBAL brushButtonBackground&  '101/81/69
GLOBAL brushButtonBorder&      '154/130/117
GLOBAL brushButtonShadow&      '77/65/58
GLOBAL brushUnexplored&        '70/19/13
GLOBAL brushBlack50&           '0/0/0 / 128
GLOBAL brushPlayer&()

'Palette
GLOBAL pal???()

'Bitmaps
GLOBAL hSkin&              'Hintergrundbild
GLOBAL hDialog&            'Dialoge
GLOBAL hDialog2&           'zusätzliche Dialog-Elemente
GLOBAL hHudElements&       'Oberflächenelemente
GLOBAL hIntro&             'Intro-Elemente
GLOBAL hPanels&            'Einheiten-Panels
GLOBAL hBuildings&         'Gebäude
GLOBAL hButtons&           'Bitmap-Buttons
GLOBAL hLoadingScreen&     'zufälliges Bild für den Ladebildschirm
GLOBAL hArtwork&()         'Artworks der Einheiten
GLOBAL artworkFilenames$() 'Dateinamen der Artworks
GLOBAL hTerrain&           'Terrain und Overlays
GLOBAL hShops&             'Shop in allen Spielerfarben
GLOBAL hUnits&()           'Einheiten in allen Spielerfarben
GLOBAL unitsLoadedForEpisode&    'Episode passend zu den Sprites in hUnits&()
GLOBAL terrainLoadedForEpisode&  'Episode passend zu den Sprites in hTerrain&
GLOBAL hRoads&()           'Straßen/Wege/Gräben/Schienen/Reifenspuren/verschneite Straßen/verschneite Wege
GLOBAL hAnimations&()      'Charakter/Hintergrund-Animationen
GLOBAL hMinimap&           'Minimap
GLOBAL hMapPreview&        'Kartenvorschau
GLOBAL hCutScene&          'Hintergrund der aktuellen Zwischensequenz
GLOBAL hCutSceneElements&  'Vordergrund-Objekte aller Zwischensequenzen
GLOBAL hVideoFrame&        'Memory-Bitmap für Video-Wiedergabe
GLOBAL hVideoFreezeFrame&  'Memory-Bitmap für Video-Standbild

'Sound/Musik/Sprachausgabe
%MAXVOICES = 7
%MAXINSTALLEDVOICES = 100
GLOBAL soundInitialized&   'wird auf 1 gesetzt, wenn das Sound-System initialisiert wurde
GLOBAL disableOGG&         'OGG Musikdateien ignorieren wenn auf 1 gesetzt
GLOBAL musicVolume&        'Musiklautstärke (0-100)
GLOBAL effectiveMusicVolume&  'tatsächliche Musiklautstärke (modifiziert durch Ingame-Ereignisse)
GLOBAL effectVolume&       'Geräuschlautstärke (0-100)
GLOBAL speechVolume&       'Lautstärke der Sprachausgabe (0-100)
GLOBAL installedVoices$()  'verfügbare Stimmen für die gewählte Sprache
GLOBAL nInstalledVoices&   'Anzahl verfügbarer Stimmen
GLOBAL speechRate&         'Sprechgeschwindigkeit
GLOBAL voices$()           'Stimmen für Sprachausgabe
GLOBAL soundchannels() AS ICDIRECTSOUNDCHANNEL
GLOBAL soundtracks&()
GLOBAL hFirstEffect&       'Sound-Handle des ersten Effekts
GLOBAL hVideoSoundTrack&   'Sound-Handle für den Audio-Stream des Videos
GLOBAL introSoundEffect&   'Zähler für Soundeffekte während des Intros
GLOBAL selectedVoiceNr&    'Stimme (Sprecher), die neu zugewiesen werden soll
GLOBAL pAfxMp3 AS IAfxMp3  'Klasse zum Abspielen von Musik
GLOBAL nSoundTracks&       'Anzahl Soundtracks
GLOBAL currentSoundTrack&  'Nummer des gerade spielenden Soundtracks
GLOBAL musicfiles$()       'Dateinamen (ohne Pfad) der Soundtracks

'Nachrichten
%GAMEMESSAGE_SPEED = 20        'Schreibgeschwindigkeit (in Buchstaben pro Sekunde)
GLOBAL gameMessageKind&        '0 = ungültig , 1 = BI2-Animation , 2 = BI3-Video
GLOBAL messageOpenTime!
GLOBAL currentMessageId&
GLOBAL currentTextId&
GLOBAL messageSender$
GLOBAL msgSenderCard&
GLOBAL gameMessageScrollY&
GLOBAL nGameMessage&
GLOBAL gameMessages$$()
GLOBAL cachedAnimations&       'Bitmaske für bereits geladene Dateien (BI2 / EDT)
GLOBAL nAnimationsScripts&
GLOBAL animationFrameCount&()  'Frames pro Charakter-Animation
GLOBAL animationWidth&()
GLOBAL animationHeight&()
GLOBAL animationsScripts$()

'Video-Nachrichten
GLOBAL videoFrameWidth&        'Breite des Videos in Pixeln
GLOBAL videoFrameHeight&       'Höhe des Videos in Pixeln
GLOBAL videoFrameCount&        'Anzahl Frames im Videos
GLOBAL videoMillisecsPerFrame& 'Anzeigedauer eines Frames in Millisekunden
GLOBAL currentVideoFrame&      'Nummer des gerade angezeigten Video-Frames
GLOBAL audioSamplesPerSecond&  'Abspielgeschwindigkeit des Audio-Streams
GLOBAL audioStreamData$        'Sound-Samples des Audio-Streams
GLOBAL videoSoundTrackUpdateTime!
GLOBAL videoMapping&()         'Abbildung der Videonachrichten aus den Actions auf die Dateinummern
GLOBAL missionVideoNumbers() AS TMissionVideo  'Cutscene/Briefing/Sieg/Niederlage für jede BI3 Kampagnen-Mission
GLOBAL videoMappingCount&      'Anzahl Einträge in videoMapping&()
GLOBAL videoMappingCreated&    'wird auf die Episodennummer gesetzt wenn die Arrays videoMapping&() und missionVideoNumbers&() befüllt worden sind

'Controls
GLOBAL buttonMapInfo AS IDXCONTROL
GLOBAL buttonSaveGame AS IDXCONTROL
GLOBAL buttonLoadGame AS IDXCONTROL
GLOBAL buttonMusic AS IDXCONTROL
GLOBAL buttonProtocol AS IDXCONTROL
GLOBAL buttonHighscore AS IDXCONTROL
GLOBAL buttonEndTurn AS IDXCONTROL
GLOBAL buttonOpenMenu AS IDXCONTROL
'
GLOBAL buttonShopBuild AS IDXCONTROL
GLOBAL buttonShopMove AS IDXCONTROL
GLOBAL buttonShopRefuel AS IDXCONTROL
GLOBAL buttonShopRepair AS IDXCONTROL
GLOBAL buttonShopTrain AS IDXCONTROL
'
GLOBAL buttonClose AS IDXCONTROL
'
GLOBAL progressbar AS IDXCONTROL
GLOBAL editChat AS IDXCONTROL
GLOBAL buttonChatTeam AS IDXCONTROL
GLOBAL buttonChatAll AS IDXCONTROL
'
GLOBAL editMissionCode AS IDXCONTROL
GLOBAL editPlayername AS IDXCONTROL
'
GLOBAL editServerIP AS IDXCONTROL
GLOBAL editGameName AS IDXCONTROL
GLOBAL buttonConnect AS IDXCONTROL
GLOBAL buttonCreateGame AS IDXCONTROL
GLOBAL buttonJoinGame AS IDXCONTROL
GLOBAL buttonChangeColor AS IDXCONTROL
'
GLOBAL protocolScrollbar AS IDXCONTROL

'Kartenbereiche
GLOBAL activedialoguearea AS RECT  'Fläche, die der aktive Dialog belegt
GLOBAL maparea AS RECT             'Kartenbereich (ohne Rahmen)
GLOBAL buttonarea AS RECT          'Buttonbereich (ohne Rahmen)
GLOBAL minimaparea AS RECT         'Minikartenbereich (ohne Rahmen)
GLOBAL unitpicarea AS RECT         'Eineheitenbildbereich (ohne Rahmen)
GLOBAL unitinfoarea AS RECT        'Einheiteninfobereich (ohne Rahmen)
GLOBAL messagearea AS RECT         'Meldungsbereich (ohne Rahmen)

'Texturbereiche
GLOBAL txarea_menu AS RECT
GLOBAL txarea_combat1 AS RECT
GLOBAL txarea_highscore AS RECT
GLOBAL txarea_msg AS RECT
GLOBAL txarea_shop1 AS RECT
GLOBAL txarea_shop2 AS RECT
GLOBAL txarea_shopenergy AS RECT
GLOBAL txarea_playercolors AS RECT
GLOBAL txarea_energyicon AS RECT
GLOBAL txarea_materialicon AS RECT
GLOBAL txarea_prodslot AS RECT
GLOBAL txarea_mapinfo AS RECT
GLOBAL txarea_mapinfoplayer AS RECT
GLOBAL txarea_combat2 AS RECT
GLOBAL txarea_mplobby AS RECT
GLOBAL txarea_blankheader AS RECT
GLOBAL txarea_milopheader AS RECT
GLOBAL txarea_blankfooter AS RECT
GLOBAL txarea_menuitem AS RECT
GLOBAL txarea_menuitempressed AS RECT
GLOBAL txarea_menuitemhighlight AS RECT
GLOBAL txarea_roundbox AS RECT
GLOBAL txarea_stars AS RECT
GLOBAL txarea_starborder AS RECT
GLOBAL txarea_gradiantblueblack AS RECT
'
GLOBAL txarea_wheel_transparent AS RECT
GLOBAL txarea_videomsg AS RECT
'
GLOBAL txarea_introbattleisle AS RECT
GLOBAL txarea_intro2020 AS RECT
GLOBAL txarea_introemblem AS RECT
GLOBAL txarea_introhighlight AS RECT

'Meldungen
%MESSAGEBUFFERSIZE = 32
%PROTOCOLBUFFERSIZE = 1000
GLOBAL messageCount&
GLOBAL messageBuffer$$()
GLOBAL protocolCount&
GLOBAL protocolBuffer$$()

'Menü
GLOBAL menuItemAreas() AS RECT
GLOBAL menuCaption$$
GLOBAL menuCount&
GLOBAL menuEntries$$()
GLOBAL menuOpenTime!
GLOBAL menuSelectedEntry&
GLOBAL mainMenuEntries$
GLOBAL highlightedMenuEntry&
GLOBAL lastHighlightedMenuEntry&

'Multiplayer-Lobby
GLOBAL lobbyOpenTime!
GLOBAL mpCountdownTime!
GLOBAL multiplayerGameFromSavegame&
GLOBAL updateMapPreview&
GLOBAL mapPreviewCreated&

'Karte
GLOBAL zoom#             'Karten Zoomfaktor
GLOBAL scrollX&          'Anzahl Pixel der Karte, die links außerhalb des Darstellungsbereichs liegen
GLOBAL scrollY&          'Anzahl Pixel der Karte, die oberhalb des Darstellungsbereichs liegen
GLOBAL cursorXPos&       'X-Position des Cursors (in Feldern)
GLOBAL cursorYPos&       'Y-Position des Cursors (in Feldern)
GLOBAL mapDrawOptions&   'Darstellungsparameter (0 = normal, 1 = nur Terrain , 2 = nur Terrain+Overlays)
GLOBAL trails?()         'Reifenspuren
GLOBAL gridMode&         'Gitter um Hexfelder zeichnen (0 = aus , 1 = ein)
GLOBAL defenseInfo&      'Verteidigungsbonus anzeigen (0 = aus , 1 = ein)
GLOBAL coordinateInfo&   'Koordinaten unter Maus-Cursor anzeigen (0 = aus , 1 = ein)
GLOBAL missiles() AS TMissile
GLOBAL scrollStartX&     'Usprung des Scrollens
GLOBAL scrollStartY&     'Usprung des Scrollens
GLOBAL scrollStartTime!  'Startzeitpunkt des Scrollens
GLOBAL scrollEndX&       'Ziel des Scrollens
GLOBAL scrollEndY&       'Ziel des Scrollens
GLOBAL scrollEndTime!    'Endzeitpunkt des Scrollens

'Einheiteninfo
GLOBAL lastPreviewUnit&
GLOBAL unitPreviewZoom#

'Shopinfo
GLOBAL lastPreviewShop&
GLOBAL shopPreviewZoom#
GLOBAL shopCursorPos&

'Karteninfo
GLOBAL mapinfoScrollPos&

'Highscore-Bildschirm
GLOBAL highscoreOpenTime!
GLOBAL highscoreMapData$

'Sieg-Bildschirm
GLOBAL gameoverOpenTime!
GLOBAL scoreGroundUnit&
GLOBAL scoreAirUnits&
GLOBAL scoreWaterUnits&
GLOBAL unitclassesByXp$

'Intro
%MAXINTROCLOUDS = 128
%INTROCLOUDTEXTURESIZE = 512
GLOBAL introClouds() AS TIntroCloud
GLOBAL introStartTime!
GLOBAL introLightDirection&
GLOBAL introHighlightX!
GLOBAL introHighlightSize!
GLOBAL introHighlightMaxSize!

'Credits
GLOBAL creditStartTime!

'Replay
GLOBAL replayMode&()                 '0 = aus , 1 = Aufzeichnen , 2 = Abspielen
GLOBAL replayPosition&()             'Zeiger auf aktuelle Aufnahme/Wiedergabe-Position in replayData$
GLOBAL replayData$()                 'Replay Daten
GLOBAL replayUsername$               'Benutzername für Wiedergabe
GLOBAL replayDelay!                  'Wartezeit bis zur Wiedergabe der nächsten Aktion
GLOBAL replayPause&                  'Wiedergabe pausieren, wenn auf 1 gesetzt
GLOBAL startupReplay$                'an der Kommandozeile angegebene Replay
GLOBAL replayUnitUpdate$             'Einheitendaten der Replay für die Einheit der letzten Einheitenbewegung/Aktion
GLOBAL exportingReplay&              'wird auf 1 gesetzt, während die Replay exportiert wird

'Spielstände
%MAXSAVEGAMES = 512
GLOBAL nSaveFiles&
GLOBAL saveFiles$()

'Server
GLOBAL hServer&()                    'Handles für den Serverport
GLOBAL connections() AS TConnection  'Socket-Verbindungen zu den Clients
GLOBAL artworkCache$()               'Artwork-Dateien für Download
GLOBAL artworkCacheNames$()          'Dateinamen der Artworks
GLOBAL artworkCacheSize&             'Anzahl Elemente im Artwork-Cache
GLOBAL enabledTrafficLog&            'wenn auf 1 gesetzt, dann speichert Server allen Traffic in einem Logfile
GLOBAL serverTrafficSaveTime!        'Zeitpunkt wann das Traffic-Logfile zuletzt gespeichert wurde
GLOBAL serverTrafficLog$             'aktueller (noch nicht gespeicherter) Inhalt des Traffic-Logfile

'Client
GLOBAL defaultServer$               'Standard-Server
GLOBAL hClientSocket&               'Socket-Verbindung zum Server
GLOBAL clientSecurityAnswer&        'Antwort auf die Authentifizierungsfrage für den Server
GLOBAL lobbyData$                   'Daten über alle Channels in der Lobby
GLOBAL lobbyChannels$               'IDs aller Lobby-Channel
GLOBAL selectedLobbyChannel&        'ID des angewählten Lobby-Channels
GLOBAL mpCountdown&                 'Countdown für den Start eine Multiplayer Spiels
GLOBAL clientReceivedData$          'Puffer für vom Server empfangene Daten
GLOBAL ishost&                      'wird auf 1 gesetzt, wenn lokaler Spieler der Host ist
GLOBAL filesToDownload$             'Liste mit Datei-IDs, die vom Server heruntergeladen werden sollen
GLOBAL enablePing&                  'wenn auf 1 gesetzt, dann Ping alle 2 Sekunden zum Server schicken
GLOBAL pingID&                      'ID des zuletzt geschickten Pings
GLOBAL pingSentTime!                'Zeitpunkt des zuletzt geschickten Pings
GLOBAL pingMillisecs&               'zuletzt gemessener Ping in Millisekunden
GLOBAL connectedToAuthenticServer&  'wird auf 1 gesetzt, wenn Client und Server sich gegenseitig erfolgreich authentifiziert haben
GLOBAL updateCheck&                 'wenn auf -1 gesetzt, dann wird eine Verbindung zum offiziellen Server hergestellt und dessen Version in dieser Variable gespeichert


#INCLUDE "BI2INIT.INC"
#INCLUDE "BI2GAME.INC"
#INCLUDE "BI2AI.INC"
#INCLUDE "BI2SERVER.INC"
#INCLUDE "BI2CLIENT.INC"
#INCLUDE "BI2SECRET.INC"



'Gibt eine Fehlermeldung aus
SUB PrintError(BYVAL a$$)
  'älteste Nachricht entfernen falls Puffer voll ist
  IF messageCount& > UBOUND(messageBuffer$$()) THEN
    ARRAY DELETE messageBuffer$$(0)
    messageCount& = messageCount&-1
  END IF

  'neue Nachricht am Ende des Puffers einfügen
  messageBuffer$$(messageCount&) = CHR$(7)+a$$
  messageCount& = messageCount&+1

  'im Server Meldung zusätzlich in die Application LOG-Datei schreiben
  IF gameMode& = %GAMEMODE_SERVER THEN
    IF serverConsoleMode& = 2 THEN PRINT a$$
    CALL APPLOG($APPNAME, logFilename$, ACODE$(a$$))
  END IF
END SUB



'Gibt eine Log-Meldung aus
SUB BILog(BYVAL a$$, cl&)
  IF gameMode& = %GAMEMODE_SERVER AND serverConsoleMode& = 2 THEN PRINT a$$

  'Duplikate im Server-Logbuch verhindern
  IF gameMode& = %GAMEMODE_SERVER AND messageCount& > 0 THEN
    IF messageBuffer$$(messageCount&-1) = CHR$(cl&)+a$$ THEN EXIT SUB
  END IF

  'älteste Nachricht entfernen falls Puffer voll ist
  IF messageCount& > UBOUND(messageBuffer$$()) THEN
    ARRAY DELETE messageBuffer$$(0)
    messageCount& = messageCount&-1
  END IF

  'neue Nachricht am Ende des Puffers einfügen
  messageBuffer$$(messageCount&) = CHR$(cl&)+a$$
  messageCount& = messageCount&+1
END SUB



'Schreibt eine Meldung ins Debug-Logfile
SUB BIDebugLog(BYVAL a$$)
  LOCAL nr&

  IF enableDebugLog& = 0 THEN EXIT SUB

  nr& = FREEFILE
  OPEN EXEPATH$+$DEBUGLOGFILE FOR APPEND AS nr&
  PRINT# nr&, a$$
  CLOSE nr&
END SUB



'Fügt dem Protokoll einen Eintrag hinzu
SUB AddProtocol(msg AS WSTRING)
  IF gameMode& = %GAMEMODE_SERVER THEN EXIT SUB

  'neuen Eintrag am Ende des Puffers einfügen
  protocolBuffer$$(protocolCount&) = msg
  protocolCount& = protocolCount&+1
  protocolScrollbar.MaxScroll = MAX&(0, protocolCount&-protocolScrollbar.VisibleRows)
END SUB



'Programm mit Fehlermeldung beenden
SUB CriticalError(errmsg&)
  LOCAL c AS WSTRINGZ*1024, t AS WSTRINGZ*1024

  c = words$$(%WORD_CRITITCAL_ERROR)
  t = words$$(errmsg&)
  MessageBoxW(hWIN&, t, c, %MB_OK OR %MB_ICONERROR)

  END
END SUB



'Wartet darauf, eine kritische Sektion betreten zu dürfen
SUB EnterSemaphore(BYREF semaphore&)
  LOCAL t!

'  t! = TIMER+5
  WHILE semaphore& <> 0
    SLEEP 1
'    IF TIMER > t! THEN
'      SELECT CASE VARPTR(semaphore&)
'      CASE VARPTR(semaphore_unitmoving&): PRINT "semaphore_unitmoving& Timeout!"
'      CASE VARPTR(semaphore_highscore&): PRINT "semaphore_highscore& Timeout!"
'      CASE VARPTR(semaphore_crttexture&): PRINT "semaphore_crttexture& Timeout!"
'      CASE VARPTR(semaphore_scrollpos&): PRINT "semaphore_scrollpos& Timeout!"
'      END SELECT
'      EXIT LOOP
'    END IF
  WEND
  semaphore& = 1
END SUB



'Verläßt eine kritische Sektion
SUB LeaveSemaphore(BYREF semaphore&)
  semaphore& = 0
END SUB



'Wartet darauf, daß das Spiel vollständig initialisiert ist
SUB WaitGameInitialised
  WHILE initDone& <> 1
    SLEEP 50
    IF semaphore_crttexture& = 0 THEN
      CALL EnterSemaphore(semaphore_crttexture&)
      CALL UpdateDateTime
      D2D.OnRender(mousexpos&, mouseypos&)
      CALL LeaveSemaphore(semaphore_crttexture&)
    END IF
  WEND
END SUB



'Liest eine Datei ein
FUNCTION ReadFileContent$(f$, istext&)
  LOCAL fullname$, fullnameotherlang$, a$, errmsg$$, filefound&, fnr&, n&, p&, lng$

  fullname$ = f$
  IF LEFT$(fullname$, 2) <> "\\" AND INSTR(fullname$, ":") = 0 THEN fullname$ = EXEPATH$+fullname$
  filefound& = ISFILE(fullname$)
  IF filefound& = 0 THEN
    'prüfen, ob nicht gefundene Datei in anderer Sprache vorhanden ist
    p& = INSTR(-1, fullname$, "\"+langcode$+"\")
    IF p& >= LEN(EXEPATH$) THEN
      lng$ = IIF$(langcode$ = "GER", "ENG", "GER")
      fullnameotherlang$ = LEFT$(fullname$, p&)+lng$+MID$(fullname$, p&+4)
      IF ISFILE(fullnameotherlang$) THEN
        CALL BILog(words$$(%WORD_SWITCHING_LANGUAGE), 0)
        filefound& = 1
        fullname$ = fullnameotherlang$
        langcode$ = lng$
        languageNr& = IIF&(langcode$ = "GER", 0, 1)
        CALL ReadLangFile&(langcode$+"\BI2020.TXT")
      END IF
    END IF
  END IF

  IF filefound& = 0 THEN
    errmsg$$ = words$$(%WORD_FILE_NOT_FOUND)
    REPLACE "%" WITH f$ IN errmsg$$
    CALL PrintError(errmsg$$)
    CALL BIDebugLog(errmsg$$)
    EXIT FUNCTION
  END IF

  fnr& = FREEFILE
  OPEN fullname$ FOR BINARY LOCK SHARED AS fnr&
  n& = LOF(fnr&)
  GET$ fnr&, n&, a$
  CLOSE fnr&
  IF istext& <> 0 THEN
    REPLACE CHR$(13,10) WITH CHR$(13) IN a$
    REPLACE CHR$(10) WITH CHR$(13) IN a$
  END IF
  CALL BIDebugLog("Loading file "+fullname$+" ("+FORMAT$(n&)+" bytes).")

  ReadFileContent$ = a$
END FUNCTION



'Schreibt eine Datei
SUB WriteFileContent(f$, a$)
  LOCAL fnr&

  fnr& = FREEFILE
  OPEN EXEPATH$+f$ FOR OUTPUT AS fnr&
  PRINT# fnr&, a$;
  CLOSE fnr&
END SUB



'Fügt Daten zu einer Datei hinzu
SUB AppendFileContent(f$, a$)
  LOCAL fnr&

  fnr& = FREEFILE
  OPEN EXEPATH$+f$ FOR APPEND AS fnr&
  PRINT# fnr&, a$;
  CLOSE fnr&
END SUB



'Passt den Dateinamen an eine bestimmte Episode an
FUNCTION AdjustFilenameForEpisode$(f$, episode&, canFallback&)
  LOCAL p&, g$

  p& = INSTR(-1, f$, "000")
  IF p& = 0 THEN
    AdjustFilenameForEpisode$ = f$
    EXIT FUNCTION
  END IF

  g$ = LEFT$(f$, p&-1)+FORMAT$(episode&, "000")+MID$(f$, p&+3)
  IF ISFILE(g$) = 0 AND canFallback& <> 0 THEN g$ = f$
  AdjustFilenameForEpisode$ = g$
END FUNCTION



'Unterteilt einen Text in Zeilen
FUNCTION TextToRows&(BYVAL a$$, BYVAL maxwidth&, BYVAL textfont&, textrows$$())
  LOCAL b$$, n&, rownr&, textwd&, texthg&
  REDIM textrows$$(19)

  WHILE a$$ <> ""
    n& = LEN(a$$)
    DO
      b$$ = LEFT$(a$$, n&)
      D2D.GraphicTextSizeW(b$$, textfont&, textwd&, texthg&)
      IF textwd& <= maxwidth& THEN EXIT LOOP
      n& = INSTR(-1, b$$, " ")-1
    LOOP
    IF rownr& > UBOUND(textrows$$()) THEN REDIM PRESERVE textrows$$(UBOUND(textrows$$())+20)
    textrows$$(rownr&) = LEFT$(a$$, n&)
    a$$ = LTRIM$(MID$(a$$, n&+1))
    rownr& = rownr&+1
  WEND

  TextToRows& = rownr&
END FUNCTION



'Ersetzt alle nicht-druckbaren Zeichen in einem String durch Leerzeichen
FUNCTION ReplaceNonPrintable(BYVAL a$$, forSpeech&) AS WSTRING
  LOCAL i&

  IF forSpeech& <> 0 THEN REPLACE CHR$(0) WITH SAPISilence(1000) IN a$$

  FOR i& = 1 TO LEN(a$$)
    IF ASC(a$$, i&) < 32 THEN ASC(a$$, i&) = 32
  NEXT i&

  ReplaceNonPrintable = a$$
END FUNCTION



'Ersetzt alle Platzhalter in einer benutzerdefinierten Spielnachricht
FUNCTION ReplaceMessagePlaceholders(BYVAL a$$) AS WSTRING
  REPLACE "^OCCUPIEDSHOP" WITH lastOccupiedShop$ IN a$$
  REPLACE "^ALLYPLAYERNAME" WITH GetTeamMateName$(0, localPlayerNr&) IN a$$
  REPLACE "^PLAYERNAME" WITH localPlayerName$ IN a$$

  ReplaceMessagePlaceholders = a$$
END FUNCTION



'Prüft, ob das Programm aus einer Konsole heraus gestartet wurde
FUNCTION StartedFromConsole&
 LOCAL lpStartupInfo AS STARTUPINFO

 lpStartupInfo.cb = SIZEOF(STARTUPINFO)
 GetStartupInfo lpStartupInfo

 StartedFromConsole& = lpStartupInfo.dwXSize <> 0 OR lpStartupInfo.dwYSize <> 0
END FUNCTION



'Konsole verstecken
SUB HideConsole
  LOCAL hCons&

  IF StartedFromConsole& <> 0 THEN EXIT SUB
  hCons& = FindWindow(BYVAL 0, $WINDOWTITLE)
  ShowWindow hCons&, %SW_HIDE
END SUB



'Ermittelt die Anzahl der gesetzten Bits in einer Zahl
FUNCTION CountBits&(x&)
  LOCAL i&, d&, n&

  d& = 1
  FOR i& = 0 TO 30
    IF (x& AND d&) <> 0 THEN n& = n&+1
    d& = d&+d&
  NEXT i&
  IF x& < 0 THEN n& = n&+1

  CountBits& = n&
END FUNCTION



'Liefert einen Text, der eine Einheit identifiziert
FUNCTION UnitIDString$(chnr&, unitnr&)
  LOCAL a$, unittp&

  IF chnr& < 0 OR unitnr& < 0 OR unitnr& >= channels(chnr&).info.nunits THEN
    a$ = "Invalid("+FORMAT$(unitnr&)+")"
  ELSE
    unittp& = channels(chnr&).units(unitnr&).unittype
    a$ = channelsnosave(chnr&).unitclasses(unittp&).uname+"("+FORMAT$(unitnr&)+")
  END IF

  UnitIDString$ = a$
END FUNCTION



'Replay Aktion hinzufügen
SUB AddReplay(chnr&, action&, BYVAL plnr&, BYVAL args$)
  IF replayMode&(chnr&) >= %REPLAYMODE_PLAY THEN EXIT SUB

  'zusätzlichen Speicher allokieren
  IF replayPosition&(chnr&) > LEN(replayData$(chnr&)) THEN replayData$(chnr&) = replayData$(chnr&)+STRING$(80*128, 0)

  'Aktion speichern
  args$ = LEFT$(args$, 72)
  MID$(replayData$(chnr&), replayPosition&(chnr&), 80) = MKL$(gametime!)+CHR$(plnr&,action&,0,0)+args$+STRING$(72-LEN(args$), 0)
  replayPosition&(chnr&) = replayPosition&(chnr&)+80
END SUB



'Replay Aktion abspielen
SUB NextReplayAction
  LOCAL activeplayer&, plnr&, action&, args$
  LOCAL unitnr&, attacker&, defender&, dist&, x&, y&, unitac&, shopnr&, shopac&, shopparam&

  'prüfen, ob letzte Aktion abgeschlossen wurde
  activeplayer& = channels(0).info.activeplayer
  IF unitMovementStartTime! > 0 AND gametime!-unitMovementStartTime! > 2.5 AND AnyPlayerHasPhase&(0, %PHASE_UNITMOVING) <> 0 AND replayUnitUpdate$ <> "" THEN
    unitnr& = CVL(replayUnitUpdate$)
    POKE$ VARPTR(channels(0).units(unitnr&)), MID$(replayUnitUpdate$, 5, SIZEOF(TUnit))
    CALL EndMovement(0, unitnr&, channels(0).units(unitnr&).xpos, channels(0).units(unitnr&).ypos)
  END IF
  IF replayPause& <> 0 THEN EXIT SUB
  IF replayMode&(0) = %REPLAYMODE_PLAY THEN
    IF GetPhase&(0, activeplayer&) > %PHASE_UNITSELECTED THEN EXIT SUB
    IF messageOpenTime! > 0 THEN EXIT SUB
    IF replayDelay! > gametime! THEN EXIT SUB
  END IF

  'Wartezeit zwischen 2 Aktionen
  IF replayDelay! < 0 THEN
    replayDelay! = gametime!-replayDelay!
    EXIT SUB
  END IF

  'prüfen, ob Ende der Wiedergabe erreicht wurde
  IF replayPosition&(0) > LEN(replayData$(0)) THEN
    IF replayMode&(0) = %REPLAYMODE_FASTPLAY THEN EXIT SUB
    replayMode&(0) = %REPLAYMODE_OFF
    gamedataChanged& = 0
    CALL BILog(words$$(%WORD_REPLAY_END), 0)
    CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_RING2, %SOUNDBUFFER_EFFECT1, %PLAYFLAGS_NONE)
    CALL ShowMainMenu(%SUBMENU_MAIN, %PHASE_NONE, 0)
    EXIT SUB
  END IF

  'prüfen, ob Shop-Dialog wieder geschlossen werden kann
  action& = ASC(replayData$(0), replayPosition&(0)+5)
  IF selectedShop& >= 0 AND action& <> %REPLAY_SHOPACTION AND replayMode&(0) = %REPLAYMODE_PLAY THEN
    IF gametime! > shopSelectionTime!+1 THEN CALL ExitButtonPressed(0)
    EXIT SUB
  END IF

  'Einheit der letzten Aktion aktualisieren
  IF replayUnitUpdate$ <> "" THEN
    unitnr& = CVL(replayUnitUpdate$)
    POKE$ VARPTR(channels(0).units(unitnr&)), MID$(replayUnitUpdate$, 5, SIZEOF(TUnit))
    replayUnitUpdate$ = ""
  END IF

  'nächste Aktion laden
  plnr& = ASC(replayData$(0), replayPosition&(0)+4)
  args$ = MID$(replayData$(0), replayPosition&(0)+8, 72)
  replayPosition&(0) = replayPosition&(0)+80

  'Aktion ausführen
  SELECT CASE action&
  CASE %REPLAY_MOVE:
    unitnr& = CVL(args$)
    x& = CVI(args$, 5)
    y& = CVI(args$, 7)
    replayUnitUpdate$ = MKL$(unitnr&)+MID$(args$, 9, SIZEOF(TUnit))
    IF channels(0).player(plnr&).selectedunit >= 0 AND channels(0).player(plnr&).selectedunit <> unitnr& THEN CALL UnselectUnit(0, plnr&)
    channels(0).player(plnr&).selectedunit = unitnr&
    IF replayMode&(0) = %REPLAYMODE_PLAY THEN
      IF (channels(0).vision(x&, y&) AND localPlayerMask&) <> 0 OR (channels(0).vision(channels(0).units(unitnr&).xpos, channels(0).units(unitnr&).ypos) AND localPlayerMask&) <> 0 THEN
        CALL ScrollToMapPos(channels(0).units(unitnr&).xpos, channels(0).units(unitnr&).ypos, 0.5)
      END IF
      CALL MoveUnit(unitnr&, x&, y&)
      replayDelay! = -0.5
    ELSE
      CALL MoveUnit(unitnr&, x&, y&)
    END IF

  CASE %REPLAY_ATTACK:
    POKE$ VARPTR(channels(0).combat), LEFT$(args$, SIZEOF(TCombatInfo))
    attacker& = channels(0).combat.attacker
    defender& = channels(0).combat.defender
    IF channels(0).player(plnr&).selectedunit >= 0 AND channels(0).player(plnr&).selectedunit <> attacker& THEN CALL UnselectUnit(0, plnr&)
    channels(0).player(plnr&).selectedunit = attacker&
    channels(0).player(plnr&).selectedtarget = defender&
    dist& = GetDistance&(channels(0).units(attacker&).xpos, channels(0).units(attacker&).ypos, channels(0).units(defender&).xpos, channels(0).units(defender&).ypos)
    IF replayMode&(0) = %REPLAYMODE_PLAY THEN
      CALL ScrollToMapPos(channels(0).units(attacker&).xpos, channels(0).units(attacker&).ypos, 0.5)
      CALL StartCombat(0, attacker&, defender&, channels(0).combat.weaponatt, IIF&(dist& = 1, -1, 0), 1)
      replayDelay! = -0.5
    ELSE
      CALL EndCombat(0, channels(0).combat)
    END IF

  CASE %REPLAY_UNITACTION:
    unitnr& = CVL(args$)
    unitac& = CVI(args$, 5)
    x& = CVI(args$, 7)
    y& = CVI(args$, 9)
    replayUnitUpdate$ = MKL$(unitnr&)+MID$(args$, 11, SIZEOF(TUnit))
    IF channels(0).player(plnr&).selectedunit >= 0 AND channels(0).player(plnr&).selectedunit <> unitnr& THEN CALL UnselectUnit(0, plnr&)
    IF replayMode&(0) = %REPLAYMODE_PLAY AND (channels(0).vision(channels(0).units(unitnr&).xpos, channels(0).units(unitnr&).ypos) AND localPlayerMask&) <> 0 THEN
      CALL ScrollToMapPos(channels(0).units(unitnr&).xpos, channels(0).units(unitnr&).ypos, 0.5)
    END IF
    CALL UnitAction(0, plnr&, unitnr&, unitac&, x&, y&)
    IF replayMode&(0) = %REPLAYMODE_PLAY THEN replayDelay! = -0.5

  CASE %REPLAY_SHOPACTION:
    shopnr& = CVL(args$)
    shopac& = CVL(args$, 5)
    shopparam& = CVL(args$, 9)
    IF replayMode&(0) = %REPLAYMODE_PLAY THEN
      CALL ScrollToMapPos(channels(0).shops(shopnr&).position, channels(0).shops(shopnr&).position2, 0.5)
      IF (channels(0).player(localPlayerNr&).allymask AND 2^plnr&) <> 0 THEN
        CALL SelectShop(shopnr&)
        shopSelectionTime! = gametime!-0.5  'Shop-Öffnen-Animation überspringen
      END IF
    END IF
    channels(0).player(plnr&).selectedunit = shopparam&
    SELECT CASE shopac&
    CASE %SHOPACTION_BUILD: CALL BuildUnit&(0, shopnr&, shopparam&)
    CASE %SHOPACTION_REFUEL: CALL RefuelInShop(0, shopnr&, shopparam&)
    CASE %SHOPACTION_REPAIR: CALL RepairInShop(0, shopnr&, shopparam&)
    CASE %SHOPACTION_TRAIN: CALL TrainInShop(0, shopnr&, shopparam&)
    CASE %SHOPACTION_TRAINCAMPAIGN_ONE_LEVEL: CALL TrainCampaign(0, shopparam&, 1)
    CASE %SHOPACTION_TRAINCAMPAIGN_TWO_LEVELS: CALL TrainCampaign(0, shopparam&, 2)
    CASE %SHOPACTION_TRAINCAMPAIGN_THREE_LEVELS: CALL TrainCampaign(0, shopparam&, 3)
    CASE %SHOPACTION_TRAINCAMPAIGN_FOUR_LEVELS: CALL TrainCampaign(0, shopparam&, 4)
    END SELECT
    IF replayMode&(0) = %REPLAYMODE_PLAY THEN replayDelay! = -2.0

  CASE %REPLAY_ENDTURN:
    CALL EndTurn(0)

  END SELECT

  CALL UpdateProgressbar
  gamedataChanged& = 0
END SUB



'Prüft, ob der lokale Spieler am Zug ist
FUNCTION LocalPlayersTurn&
  LocalPlayersTurn& = IsActivePlayer&(0, localPlayerNr&)
END FUNCTION



'Prüft ob sich ein Punkt in einem Rechteck befindet
FUNCTION IsInRect&(x&, y&, r AS RECT)
  IsInRect& = IIF&(x& >= r.left AND x& <= r.right AND y& >= r.top AND y& <= r.bottom, -1, 0)
END FUNCTION



'Ermittelt über welchem Bereich sich der Mauscursor befindet
FUNCTION GetAreaAtMousePos&
  LOCAL r&

  IF IsInRect&(mousexpos&, mouseypos&, maparea) THEN
    r& = %AREA_MAP
  ELSE
    IF IsInRect&(mousexpos&, mouseypos&, minimaparea) THEN
      r& = %AREA_MINIMAP
    ELSE
      IF IsInRect&(mousexpos&, mouseypos&, unitpicarea) THEN
        r& = %AREA_UNITPIC
      ELSE
        IF IsInRect&(mousexpos&, mouseypos&, unitinfoarea) THEN
          r& = %AREA_UNITINFO
        ELSE
          IF IsInRect&(mousexpos&, mouseypos&, messagearea) THEN
            r& = %AREA_MESSAGE
          END IF
        END IF
      END IF
    END IF
  END IF

  GetAreaAtMousePos& = r&
END FUNCTION



'Ermittelt die Einheit in dem Inhalts-Slot an der Mausposition
FUNCTION GetShopUnitAtMousePos&(movecursor&)
  LOCAL x&, y&, v&

  'prüfen, ob Shop-Overlay angezeigt wird
  IF selectedShop& < 0 THEN
    GetShopUnitAtMousePos& = -1
    EXIT FUNCTION
  END IF

  'Slot an der Mausposition bestimmen
  v& = -1
  x& = (mousexpos&-IIF&(selectedShopProd$ = "", 238, 430)*uiscale!-activedialoguearea.left)/uiscale!
  y& = (mouseypos&-47*uiscale!-activedialoguearea.top)/uiscale!

  IF x& >= 0 AND y& >= 0 AND (x& MOD 82) < 72 AND (y& MOD 82) < 72 THEN
    x& = INT(x&/82)
    y& = INT(y&/82)
    IF x& < 4 AND y& < 4 THEN
      v& = channels(0).shops(selectedShop&).content(x&+y&*4)
      IF movecursor& <> 0 THEN shopCursorPos& = x&+y&*4
    END IF
  END IF

  GetShopUnitAtMousePos& = v&
END FUNCTION



'Ermittelt die Einheit in dem Produktions-Slot an der Mausposition
FUNCTION GetShopProductionAtMousePos&
  LOCAL x&, y&, v&, unittp&, costenergy&, costmat&

  'prüfen, ob Shop-Overlay angezeigt wird
  IF selectedShop& < 0 OR selectedShopProd$ = "" THEN
    GetShopProductionAtMousePos& = -1
    EXIT FUNCTION
  END IF

  'Slot an der Mausposition bestimmen
  v& = -1
  x& = (mousexpos&-47*uiscale!-activedialoguearea.left)/uiscale!
  y& = (mouseypos&-47*uiscale!-activedialoguearea.top)/uiscale!
  IF x& >= 0 AND y& >= 0 AND (x& MOD 82) < 72 AND (y& MOD 82) < 72 THEN
    x& = INT(x&/82)
    y& = INT(y&/82)
    IF x& < 4 AND y& < 4 AND x&+y&*4 < LEN(selectedShopProd$) THEN
      unittp& = ASC(selectedShopProd$, x&+y&*4+1)
      'prüfen, ob ausreichend Energie und Material vorhanden sind
      costenergy& = channelsnosave(0).unitclasses(unittp&).costenergy
      costmat& = channelsnosave(0).unitclasses(unittp&).costmaterial
      IF costenergy& <= channels(0).player(localPlayerNr&).energy AND costmat& <= channels(0).shops(selectedShop&).material THEN v& = unittp&
    END IF
  END IF

  GetShopProductionAtMousePos& = v&
END FUNCTION



'Pixelposition eines Felds errechnen (Zentrum des Felds)
SUB GetPixelPos(BYVAL mapx&, BYVAL mapy&, x&, y&)

  x& = 12+mapx&*16
  y& = 12+mapy&*24+IIF&((mapx& AND 1) = 1, 12, 0)

  'Zoom und Scrollposition berücksichtigen
  x& = x&*zoom#+maparea.left-scrollX&
  y& = y&*zoom#+maparea.top-scrollY&
END SUB



'Kartenfeld zu Pixelposition errechnen
SUB GetMapPos(BYVAL x&, BYVAL y&, mapx&, mapy&)
  LOCAL v&, w&

  'Position normieren
  x& = (x&-maparea.left+scrollX&)/zoom#
  y& = (y&-maparea.top+scrollY&)/zoom#

  mapx& = INT(x&/16)
  mapy& = INT((y&-IIF&((mapx& AND 1) = 1, 12, 0))/24)

  v& = x& AND 15
  IF v& < 8 THEN
    IF (mapx& AND 1) = 0 THEN
      w& = 12-ASC($spx, (y& MOD 24)+1)/2
      IF v& < w& THEN
        mapx& = mapx&-1
        mapy& = INT((y&-12)/24)
      END IF
    ELSE
      w& = ASC($spx, (y& MOD 24)+1)/2-4
      IF v& < w& THEN
        mapx& = mapx&-1
        mapy& = INT(y&/24)
      END IF
    END IF
  END IF

  'prüfen ob Position gültig ist
  IF mapx& < 0 OR mapx& >= channels(0).info.xsize OR mapy& < 0 OR mapy& >= channels(0).info.ysize THEN mapx& = -1
END SUB



'Kartenfeld zentrieren
SUB ScrollToMapPos(BYVAL mapx&, BYVAL mapy&, duration!)
  IF screenShoting& <> 0 THEN EXIT SUB
  CALL EnterSemaphore(semaphore_scrollpos&)
  scrollStartX& = scrollX&
  scrollStartY& = scrollY&
  scrollEndX& = mapx&*16*zoom#-(maparea.right-maparea.left)/2
  scrollEndY& = mapy&*24*zoom#-(maparea.bottom-maparea.top)/2
  scrollEndX& = MAX&(0, MIN&(channels(0).info.xsize*16*zoom#-maparea.right+maparea.left+8*zoom#, scrollEndX&))
  scrollEndY& = MAX&(0, MIN&(channels(0).info.ysize*24*zoom#-maparea.bottom+maparea.top+12*zoom#, scrollEndY&))
  scrollStartTime! = gametime!
  scrollEndTime! = scrollStartTime!+duration!
  CALL LeaveSemaphore(semaphore_scrollpos&)
END SUB



'Kartenfeld zentrieren, falls Cursorposition nahe dem Darstellungsrand ist
SUB ScrollToMapPosWhenNearEdge(BYVAL mapx&, BYVAL mapy&, duration!)
  LOCAL cx&, cy&, mapareaWidth&, mapareaHeight&

  mapareaWidth& = (maparea.right-maparea.left)/6
  mapareaHeight& = (maparea.bottom-maparea.top)/6
  cx& = maparea.left+(cursorXPos&*16+12)*zoom#-scrollX&
  cy& = maparea.top+(cursorYPos&*24+(cursorXPos& AND 1)*12+12)*zoom#-scrollY&
  IF cx& < maparea.left+mapareaWidth& OR cx& > maparea.right-mapareaWidth& OR cy& < maparea.top+mapareaHeight& OR cy& > maparea.bottom-mapareaHeight& THEN CALL ScrollToMapPos(mapx&, mapy&, duration!)
END SUB



'Automatisches Scrollen der Karte
SUB AutoScroll
  LOCAL t!, duration!

  duration! = scrollEndTime!-scrollStartTime!
  IF duration! <= 0 THEN EXIT SUB
  IF screenShoting& <> 0 THEN EXIT SUB

  'aktuelle Position interpolieren
  t! = gametime!-scrollStartTime!
  IF t! >= duration! THEN
    t! = duration!
    scrollStartTime! = 0
    scrollEndTime! = 0
  END IF
  CALL EnterSemaphore(semaphore_scrollpos&)
  scrollX& = scrollStartX&+(scrollEndX&-scrollStartX&)*t!/duration!
  scrollY& = scrollStartY&+(scrollEndY&-scrollStartY&)*t!/duration!
  CALL LeaveSemaphore(semaphore_scrollpos&)
END SUB



'Ermittelt den Winkel der Gerade zwischen zwei Punkten in Grad (0-359)
FUNCTION GetAngle&(BYVAL startx&, BYVAL starty&, BYVAL endx&, BYVAL endy&)
  LOCAL dx&, dy&, a&

  dx& = endx&-startx&
  dy& = endy&-starty&

  IF dx& <> 0 THEN
    a& = 90+180*ATN(dy&/dx&)/3.141592653589793
    IF dx& < 0 THEN a& = a&+180
    IF a& < 0 THEN a& = a&+360
    IF a& >= 360 THEN a& = a&-360
  ELSE
    IF dy& > 0 THEN a& = 180
  END IF

  GetAngle& = a&
END FUNCTION



'Prüft, ob eine Mission zu "Erbe des Titan" gehört
FUNCTION IsEDTMission&(missionnr&)
  SELECT CASE missionnr&
  CASE 17, 19, 33, 42 TO 99: IsEDTMission& = 1
  CASE ELSE: IsEDTMission& = 0
  END SELECT
END FUNCTION



'Ermittelt die Missions-Nummer zu einem Missions-Code
FUNCTION GetMissionNumber&(missioncode$)
  LOCAL missionnr&

  'prüfen, ob Code die richtige Länge hat
  IF LEN(missioncode$) < 5 OR LEN(missioncode$) > 7 THEN
    GetMissionNumber& = -1
    EXIT FUNCTION
  END IF

  'Code validieren
  ARRAY SCAN mapnames$(0) FOR UBOUND(mapnames$())+1, COLLATE UCASE, =missioncode$, TO missionnr&

  GetMissionNumber& = missionnr&-1
END FUNCTION



'Ermittelt die Episode zu einer Missions-Nummer
FUNCTION GetEpisodeForMap&(missionnr&)
  LOCAL episode&

  SELECT CASE missionnr&
  CASE %EPISODE1_START TO %EPISODE2_START-1  'Battle Isle II
    episode& = 1
  CASE %EPISODE2_START TO %EPISODE3_START-1  'Erbe des Titan
    episode& = 2
  CASE %EPISODE3_START TO %EPISODE4_START-1  'Kitanas Schloß
    episode& = 3
  CASE %EPISODE4_START TO %EPISODE5_START-1  'Multiplayer Kampagne
    episode& = 4
  CASE %EPISODE5_START TO %EPISODE6_START-1:  'Battle Isle 3
    episode& = 5
  CASE %EPISODE6_START TO %EPISODE7_START-1:  'Battle Isle 1
    episode& = 6
  CASE %EPISODE7_START TO %EPISODE8_START-1:  'Battle Isle 1 Wüste
    episode& = 7
  CASE %EPISODE8_START TO %EPISODE9_START-1:  'Battle Isle 1 Mond von Chromos
    episode& = 8
  CASE %EPISODE9_START TO %EPISODE10_START-1:  'Battle Isle 3 Drulls
    episode& = 9
  CASE %EPISODE10_START TO %EPISODE11_START-1:  'Battle Isle 3 Titan-Net
    episode& = 10
  CASE %EPISODE11_START TO %EPISODE12_START-1:  'Battle Isle 3 Imperium
    episode& = 11
  CASE %EPISODE12_START TO 999:  'Battle Isle 3 Epilog
    episode& = 12
  END SELECT

  GetEpisodeForMap& = episode&
END FUNCTION



'Liefert die erste Mission einer Episode
FUNCTION GetEpisodeStartMap&(episode&)
  SELECT CASE episode&
  CASE 1: GetEpisodeStartMap& = %EPISODE1_START
  CASE 2: GetEpisodeStartMap& = %EPISODE2_START
  CASE 3: GetEpisodeStartMap& = %EPISODE3_START
  CASE 4: GetEpisodeStartMap& = %EPISODE4_START
  CASE 5: GetEpisodeStartMap& = %EPISODE5_START
  CASE 6: GetEpisodeStartMap& = %EPISODE6_START
  CASE 7: GetEpisodeStartMap& = %EPISODE7_START
  CASE 8: GetEpisodeStartMap& = %EPISODE8_START
  CASE 9: GetEpisodeStartMap& = %EPISODE9_START
  CASE 10: GetEpisodeStartMap& = %EPISODE10_START
  CASE 11: GetEpisodeStartMap& = %EPISODE11_START
  CASE 12: GetEpisodeStartMap& = %EPISODE12_START
  END SELECT
END FUNCTION



'Spielererfahrungspunkte in (Einheiten)-Erfahrungs-Icon umwandeln
FUNCTION PlayerXPToIcon&(plxp&)
  LOCAL unitxp&

  IF plxp& < 3 THEN
    unitxp& = 0
  ELSE
    'jede erfolgreich abgeschlossene Mission bringt 1 Punkt -> Max 255
    unitxp& = MIN&(11, plxp&/10+1)
  END IF

  PlayerXPToIcon& = unitxp&
END FUNCTION



'Nächste unterstützte Sprache auswählen
SUB NextLanguage
  LOCAL i&, n&

  'aktuelle Sprache suchen
  n& = UBOUND(supportedLanguages$)+1
  FOR i& = 0 TO n&-1
    IF LEFT$(supportedLanguages$(i&), 3) = langcode$ THEN EXIT FOR
  NEXT i&

  'nächste Sprache anwählen
  i& = i&+1
  IF i& >= n& THEN i& = 0
  langcode$ = LEFT$(supportedLanguages$(i&), 3)
  languageNr& = IIF&(langcode$ = "GER", 0, 1)
END SUB



'Ermittelt alle Stimmen für eine bestimmte Sprache
FUNCTION GetVoicesForLangcode$(lang$)
  LOCAL i&, a$, langid$
  LOCAL vname$(), vattr$()
  REDIM installedVoices$(%MAXINSTALLEDVOICES-1)

  'SAPI Sprachcode aus BI Sprachcode ermitteln
  FOR i& = 0 TO UBOUND(supportedLanguages$)
    IF LEFT$(supportedLanguages$(i&), 3) = lang$ THEN langid$ = MID$(supportedLanguages$(i&), 4)
  NEXT i&

  'alle installierten Stimme zu diesem Sprachcode suchen
  nInstalledVoices& = 0
  CALL GETVOICES(vname$(), vattr$())
  FOR i& = 0 TO UBOUND(vname$())
    IF MID$(vattr$(i&), 3) = langid$ THEN
      installedVoices$(nInstalledVoices&) = vname$(i&)
      a$ = a$+MKI$(%MENUENTRY_VOICES+nInstalledVoices&)
      nInstalledVoices& = nInstalledVoices&+1
      IF nInstalledVoices& = %MAXINSTALLEDVOICES THEN EXIT FOR
    END IF
  NEXT I&

  GetVoicesForLangcode$ = a$
END FUNCTION



'Stimme einem Sprecher zuweisen
SUB AssignVoice(menuentry&, voicenr&)
  LOCAL a$$, slotnr&, msgid&, textid&

  'Sprecher ermitteln
  SELECT CASE menuentry&
  CASE %WORD_VOICE_STANDARD: slotnr& = 0
  CASE %WORD_VOICE_MISSIONBRIEFING: slotnr& = 1
  CASE %WORD_VOICE_FORECAST: slotnr& = 2
  CASE %WORD_VOICE_AISASCIA: slotnr& = 3
  CASE %WORD_VOICE_MOL_DURAG: slotnr& = 4
  CASE %WORD_VOICE_MALE: slotnr& = 5
  CASE %WORD_VOICE_FEMALE: slotnr& = 6
  END SELECT

  'Stimme zuweisen
  voices$(slotnr&, languageNr&) = installedVoices$(voicenr&)

  'erste Nachricht ermitteln, die von diesem Sprecher gesprochen wird
  FOR msgid& = 0 TO nGameMessage&-1
    IF GetVoiceForMessage&(msgid&) = slotnr& THEN
      textid& = GetTextIdForMessage&(msgid&)
      a$$ = GetGameMessageText(textid&, 1, 0)
      EXIT FOR
    END IF
  NEXT msgid&

  'Nachricht vorlesen
  CALL SAPISpeak(SAPISetVoice(SAPISetVolume(100, SAPISetRate(2, a$$)), 0, 0, voices$(slotnr&, languageNr&)))
END SUB



'Multiplayer-Lobby öffnen
SUB OpenLobby(missionnr&)
  LOCAL a$, r&, episode&

  SELECT CASE missionnr&
  CASE -1  'Spiel beitreten
    gameState& = %GAMESTATE_JOINLOBBY
    buttonJoinGame.Enabled = 0
    multiplayerGameFromSavegame& = 0
    ishost& = 0
  CASE -2  'Spiel mit Spielstand erstellen
    gameState& = %GAMESTATE_CREATELOBBY
    editGameName.Value = LEFT$(channels(0).info.mapshortdescr, 32)
    multiplayerGameFromSavegame& = 1
    updateMapPreview& = 1
    ishost& = 1
  CASE ELSE  'neues Spiel erstellen
    gameState& = %GAMESTATE_CREATELOBBY
    episode& = GetEpisodeForMap&(missionnr&)
    r& = LoadMission&("MIS\MISS"+FORMAT$(missionnr&, "000")+".DAT", 4, defaultDifficulty&, 0)
    IF r& <= 0 THEN
      IF r& = 0 THEN CALL PrintError(words$$(%WORD_INVALID_MISSIONFILE))
      EXIT SUB
    END IF
    editGameName.Value = LEFT$(channels(0).info.mapshortdescr, 32)
    updateMapPreview& = 1
    ishost& = 1
  END SELECT

  editServerIP.Value = defaultServer$
  lobbyData$ = ""
  lobbyOpenTime! = gametime!
  lobbyChannels$ = ""
  selectedLobbyChannel& = -1
  mpCountdown& = 0
  mapPreviewCreated& = 0
  messageOpenTime! = 0
  CALL ConnectToServer&(StringToIP&(defaultServer$))
END SUB



'Multiplayer-Lobby schließen
SUB CloseLobby(disconnect&, closeimmediately&)
  IF disconnect& <> 0 THEN CALL CloseConnectionToServer("Lobby closed")
  CALL SetMultiplayerLobbyControls(0)
  buttonClose.Visible = 0
  IF closeimmediately& = 0 THEN
    lobbyOpenTime! = gametime!
    dialogueClosing& = 1
  ELSE
    lobbyOpenTime! = 0
    dialogueClosing& = 0
  END IF
END SUB



'Schließt alle Dialoge
SUB CloseAllDialogues
  dialogueClosing& = 0
  CALL InitArea(activedialoguearea, 0, 0, 0, 0)
  IF menuOpenTime! > 0 THEN
    menuOpenTime! = 0
    EXIT SUB
  END IF
  messageOpenTime! = 0
  combatStartTime! = 0
  shopSelectionTime! = 0
  selectedShop& = -1
  mapinfoOpenTime! = 0
  highscoreOpenTime! = 0
  lobbyOpenTime! = 0
  buttonShopBuild.Visible = 0
  buttonShopMove.Visible = 0
  buttonShopRefuel.Visible = 0
  buttonShopRepair.Visible = 0
  buttonShopTrain.Visible = 0
  buttonClose.Visible = 0
  IF GetPhase&(0, localPlayerNr&) = %PHASE_COMBAT THEN CALL EndCombat(0, channels(0).combat)
END SUB



'Menü öffnen
SUB OpenMenu(menutype&, defaultEntry&, c$$, e$$)
  LOCAL i&

  IF gameMode& = %GAMEMODE_SERVER THEN EXIT SUB

  CALL SetPhase(0, localPlayerNr&, menutype&)
  highlightedMenuEntry& = defaultEntry&
  lastHighlightedMenuEntry& = -1
  menuOpenTime! = gametime!
  menuSelectedEntry& = -1

  'Überschrift merken
  menuCaption$$ = c$$

  'Einträge extrahieren
  menuCount& = PARSECOUNT(e$$, CHR$(13))
  REDIM menuEntries$$(menuCount&-1), menuItemAreas(menuCount&-1)
  FOR i& = 1 TO menuCount&
    menuEntries$$(i&-1) = PARSE$(e$$, CHR$(13), i&)
  NEXT i&
END SUB



'Menü schließen
SUB CloseMenu(x&, y&)
  LOCAL i&, phase&

  'prüfen, ob Eintrag gewählt wurde
  FOR i& = 0 TO menuCount&-1
    IF IsInRect&(x&, y&, menuItemAreas(i&)) <> 0 THEN EXIT FOR
  NEXT i&
  IF i& = menuCount& THEN EXIT SUB

  'Menü schließen und gewählten Eintrag merken
  menuSelectedEntry& = i&
  phase& = GetPhase&(0, localPlayerNr&)
  IF phase& < %PHASE_WEAPONMENU OR phase& > %PHASE_CAMPAIGNTRAINING OR CVI(mainMenuEntries$, menuSelectedEntry&*2+1) = %WORD_BACKTOGAME THEN
    dialogueClosing& = 1
    menuOpenTime! = gametime!
    CALL SetPhase(0, localPlayerNr&, %PHASE_NONE)
  ELSE
    'Untermenü öffnen
    CALL MenuEntrySelected
  END IF
END SUB



'Menü ohne Auswahl schließen
SUB AbortMenu
  LOCAL phase&

  dialogueClosing& = 1
  CALL UpdateDateTime
  menuOpenTime! = gametime!
  menuSelectedEntry& = -2
  phase& = GetPhase&(0, localPlayerNr&)
  IF phase& = %PHASE_WEAPONMENU OR phase& = %PHASE_CLIMBMENU OR phase& = %PHASE_DIVEMENU OR phase& = %PHASE_BUILDMENU OR phase& = %PHASE_SUPPORTMENU THEN
    CALL UnselectUnit(0, localPlayerNr&)
    CALL ClearTargets(0, localPlayerNr&)
  END IF
  CALL SetPhase(0, localPlayerNr&, %PHASE_NONE)
END SUB



'Gewählten Menüeintrag ausführen
SUB MenuEntrySelected
  LOCAL nr&, selectedunit&, selectedtarget&, weaponnr&, attacker&, defender&, dist&, menutype&, action&

  'Sound-Effekt spielen
  CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_CLICK, %SOUNDBUFFER_EFFECT1, %PLAYFLAGS_NONE)

  'Menü verstecken
  menuOpenTime! = 0
  menutype& = GetPhase&(0, localPlayerNr&)
  CALL SetPhase(0, localPlayerNr&, %PHASE_NONE)
  selectedunit& = channels(0).player(localPlayerNr&).selectedunit
  selectedtarget& = channels(0).player(localPlayerNr&).selectedtarget
  IF ISOBJECT(editMissionCode) THEN
    editMissionCode.Visible = 0
    editPlayername.Visible = 0
  END IF

  'gewählten Eintrag ausführen
  SELECT CASE menutype&
  CASE %PHASE_MAINMENU
    nr& = CVI(mainMenuEntries$, menuSelectedEntry&*2+1)
    SELECT CASE nr&
    CASE %WORD_STARTGAME
      'neue Kampagne oder Karte starten
      CALL ShowMainMenu(%SUBMENU_START, menutype&, 0)

    CASE %WORD_LOADGAME
      'gespeicherte Spielstände zeigen
      CALL ShowMainMenu(%SUBMENU_LOADGAME, menutype&, 0)

    CASE %MENUENTRY_LOADGAME TO %MENUENTRY_LOADGAME+%MAXSAVEGAMES-1
      'Spielstand laden
      CALL LoadGame(saveFiles$(0, nr&-%MENUENTRY_LOADGAME))
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      CALL UpdateDateTime

    CASE %WORD_SHOW_ALL_SAVEGAMES
      'alle Spielstände zeigen
      menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü offen halten
      CALL SelectSaveGame
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      CALL UpdateDateTime

    CASE %WORD_DELETE_OLD_SAVEGAMES
      'alte Spielstände löschen
      CALL DeleteOldSavegames
      CALL ShowMainMenu(%SUBMENU_LOADGAME, menutype&, 0)

    CASE %WORD_SAVEGAME
      'Spielstand speichern
      CALL SaveGame(0)
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      CALL UpdateDateTime

    CASE %WORD_VIEWREPLAY
      'Replay ansehen
      CALL SelectReplay

    CASE %WORD_CREDITS
      'Credit anzeigen
      CALL ShowControls(0)
      creditStartTime! = gametime!
      gameState& = %GAMESTATE_CREDITS
      CALL StartMusic(6)
      CALL ReadUnitDefs&(0)

    CASE %WORD_OPTIONS
      'Einstellungen
      CALL ShowMainMenu(%SUBMENU_SETTINGS, menutype&, 0)

    CASE %WORD_LANGUAGE
      'Sprache
      IF gameState& < %GAMESTATE_INGAME THEN
        CALL NextLanguage
        CALL ReadLangFile&(langcode$+"\BI2020.TXT")
        CALL ReadUnitDefs&(0)
      END IF
      CALL ShowMainMenu(%SUBMENU_SETTINGS, menutype&, 0)
      menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü-Öffnen-Sequenz überspringen

    CASE %WORD_DIFFICULTY
      'Schwierigkeitsgrad
      defaultDifficulty& = (defaultDifficulty&+1) MOD 3
      CALL ShowMainMenu(%SUBMENU_SETTINGS, menutype&, 1)
      menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü-Öffnen-Sequenz überspringen

    CASE %WORD_COMBAT_LARGE, %WORD_COMBAT_SMALL
      'Kampfdarstellung
      combatMode& = 1-combatMode&
      CALL ShowMainMenu(%SUBMENU_SETTINGS, menutype&, 2)
      menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü-Öffnen-Sequenz überspringen

    CASE %WORD_MUSICVOLUME
      'Musiklautstärke
      musicVolume& = musicVolume&+10
      IF musicVolume& > 100 THEN musicVolume& = 0
      CALL SetMusicVolume
      CALL ShowMainMenu(%SUBMENU_SETTINGS, menutype&, 3)
      menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü-Öffnen-Sequenz überspringen

    CASE %WORD_EFFECTVOLUME
      'Effektlautstärke
      effectVolume& = effectVolume&+10
      IF effectVolume& > 100 THEN effectVolume& = 0
      CALL ShowMainMenu(%SUBMENU_SETTINGS, menutype&, 4)
      menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü-Öffnen-Sequenz überspringen

    CASE %WORD_SPEECHVOLUME
      'Sprachausgabelautstärke
      speechVolume& = speechVolume&+10
      IF speechVolume& > 100 THEN speechVolume& = 0
      CALL ShowMainMenu(%SUBMENU_SETTINGS, menutype&, 5)
      menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü-Öffnen-Sequenz überspringen

    CASE %WORD_VOICE
      'Stimmen
      CALL ShowMainMenu(%SUBMENU_VOICES, menutype&, 0)
      menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü-Öffnen-Sequenz überspringen

    CASE %WORD_VOICE_STANDARD, %WORD_VOICE_MISSIONBRIEFING, %WORD_VOICE_FORECAST, %WORD_VOICE_AISASCIA, %WORD_VOICE_MOL_DURAG, %WORD_VOICE_MALE, %WORD_VOICE_FEMALE
      'Stimmauswahl
      selectedVoiceNr& = nr&
      CALL ShowMainMenu(%SUBMENU_ASSIGNVOICE, menutype&, 0)
      menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü-Öffnen-Sequenz überspringen

    CASE %MENUENTRY_VOICES TO %MENUENTRY_VOICES+%MAXINSTALLEDVOICES-1
      'Stimme zuweisen
      CALL AssignVoice(selectedVoiceNr&, nr&-%MENUENTRY_VOICES)
      CALL ShowMainMenu(%SUBMENU_VOICES, menutype&, 0)
      menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü-Öffnen-Sequenz überspringen

    CASE %WORD_PLAYERNAME
      'Spielername
      editPlayername.XPos = menuItemAreas(7).left+9
      editPlayername.YPos =  menuItemAreas(7).top+9
      editPlayername.Width =  menuItemAreas(7).right-menuItemAreas(7).left-18
      editPlayername.Height =  menuItemAreas(7).bottom-menuItemAreas(7).top-18
      editPlayername.Value = localPlayerName$
      editPlayername.Visible = 1
      D2D.OnClick(editPlayername.XPos+1, editPlayername.YPos+1, 0)  'Fokus auf das Feld setzen
      CALL ShowMainMenu(%SUBMENU_SETTINGS, menutype&, 7)
      menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü-Öffnen-Sequenz überspringen

    CASE %WORD_QUIT
      'Spiel beenden
      IF gamedataChanged& = 0 OR replayMode&(0) >= %REPLAYMODE_PLAY THEN
        CALL StopRecordReplay(0, 1)
        PostQuitMessage 0
      ELSE
        CALL ShowMainMenu(%SUBMENU_EXIT, menutype&, 0)
        menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Menü-Öffnen-Sequenz überspringen
      END IF

    CASE %WORD_SAVE_AND_QUIT
      'Spiel speichern und dann beenden
      CALL StopRecordReplay(0, 1)
      CALL SaveGame(0)
      PostQuitMessage 0

    CASE %WORD_EXIT_WITHOUT_SAVING
      'Spiel beenden
      CALL StopRecordReplay(0, 1)
      PostQuitMessage 0

    CASE %WORD_BACKTOGAME
      'zurück zum Spiel

    CASE %WORD_BACK
      'zurück zur obersten Ebene des Haupmenüs
      CALL SaveConfig
      CALL ShowMainMenu(%SUBMENU_BACKTOMAIN, menutype&, 0)

    CASE %WORD_GAME_BI1
      'Battle Isle 1
      CALL ShowMainMenu(%SUBMENU_GAMEBI1, menutype&, 0)

    CASE %WORD_GAME_BI2
      'Battle Isle 2
      CALL ShowMainMenu(%SUBMENU_GAMEBI2, menutype&, 0)

    CASE %WORD_GAME_BI3
      'Battle Isle 3
      CALL ShowMainMenu(%SUBMENU_GAMEBI3, menutype&, 0)

    CASE %WORD_CAMPAIGN_BI2
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      CALL StartEpisode(1)
      CALL UpdateDateTime

    CASE %WORD_CAMPAIGN_EDT
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      CALL StartEpisode(2)
      CALL UpdateDateTime

    CASE %WORD_CAMPAIGN_KC
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      CALL StartEpisode(3)
      CALL UpdateDateTime

    CASE %WORD_CAMPAIGN_MULTIPLAYER
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      CALL StartEpisode(4)
      CALL UpdateDateTime

    CASE %WORD_CAMPAIGN_BI3
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      CALL StartEpisode(5)
      CALL UpdateDateTime

    CASE %WORD_SELECTMAP
      'Code für Karte eingeben
      editMissionCode.XPos = menuItemAreas(3).left+9
'      editMissionCode.YPos =  menuItemAreas(3).top+9
      editMissionCode.YPos =  menuItemAreas(5).top+9
      editMissionCode.Width =  menuItemAreas(3).right-menuItemAreas(3).left-18
      editMissionCode.Height =  menuItemAreas(3).bottom-menuItemAreas(3).top-18
      editMissionCode.Visible = 1
      D2D.OnClick(editMissionCode.XPos+1, editMissionCode.YPos+1, 0)  'Fokus auf das Feld setzen
      action& = highlightedMenuEntry&
      CALL ShowMainMenu(%SUBMENU_START, menutype&, 0)
      highlightedMenuEntry& = action&

    CASE %WORD_JOIN_MULIPLAYER
      'Mehrspieler-Spiel beitreten
      CALL OpenLobby(-1)
    END SELECT

  CASE %PHASE_WEAPONMENU
    IF menuSelectedEntry& >= 0 THEN
      'Angriffswaffe wurde gewählt -> Kampf starten
      weaponnr& = ASC(menuEntries$$(menuSelectedEntry&))-64
      attacker& = selectedunit&
      defender& = channels(0).player(localPlayerNr&).selectedtarget
      dist& = GetDistance&(channels(0).units(attacker&).xpos, channels(0).units(attacker&).ypos, channels(0).units(defender&).xpos, channels(0).units(defender&).ypos)
      CALL InitArea(activedialoguearea, 0, 0, 0, 0)
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      IF gameMode& = %GAMEMODE_SINGLE THEN
        CALL StartCombat(0, attacker&, defender&, weaponnr&, IIF&(dist& = 1, -1, 0), 0)
      ELSE
        CALL ClientAttackUnit(attacker&, defender&, weaponnr&)
      END IF
    ELSE
      'neues Angriffsziel wählen
      CALL SetPhase(0, localPlayerNr&, %PHASE_UNITSELECTED)
    END IF

  CASE %PHASE_CLIMBMENU
    IF menuSelectedEntry& >= 0 THEN  'der zweite Eintrag ist immer "Flughöhe beibehalten"
      IF (channels(0).units(selectedunit&).flags AND %US_ASCEND) = 0 THEN
        action& = IIF&(menuSelectedEntry& = 0, %UNITACTION_ASCEND, %UNITACTION_DESCEND)
      ELSE
        action& = IIF&(menuSelectedEntry& = 0, %UNITACTION_DESCEND, %UNITACTION_ASCEND)
      END IF
      CALL InitArea(activedialoguearea, 0, 0, 0, 0)
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      IF gameMode& = %GAMEMODE_SINGLE THEN
        CALL ChangeFlightHeight(0, selectedunit&, action&)
      ELSE
        CALL ClientUnitAction(selectedunit&, action&, 0, 0)
      END IF
    END IF

  CASE %PHASE_DIVEMENU
    IF menuSelectedEntry& >= 0 THEN  'der zweite Eintrag ist immer "Fahrtiefe beibehalten"
      IF (channels(0).units(selectedunit&).flags AND %US_DIVE) = 0 THEN
        action& = IIF&(menuSelectedEntry& = 0, %UNITACTION_DESCEND, %UNITACTION_ASCEND)
      ELSE
        action& = IIF&(menuSelectedEntry& = 0, %UNITACTION_ASCEND, %UNITACTION_DESCEND)
      END IF
      CALL InitArea(activedialoguearea, 0, 0, 0, 0)
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      IF gameMode& = %GAMEMODE_SINGLE THEN
        CALL ChangeWaterHeight(0, selectedunit&, action&)
      ELSE
        CALL ClientUnitAction(selectedunit&, action&, 0, 0)
      END IF
    END IF

  CASE %PHASE_BUILDMENU
    action& = GetActionCode&(CVI(mainMenuEntries$, menuSelectedEntry&*2+1))
    IF menuSelectedEntry& >= 0 THEN
      CALL InitArea(activedialoguearea, 0, 0, 0, 0)
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      IF gameMode& = %GAMEMODE_SINGLE THEN
        CALL BuildAction(0, selectedunit&, action&, selectedtarget&)
      ELSE
        CALL ClientUnitAction(selectedunit&, action&, selectedtarget& AND 255, INT(selectedtarget&/256))
      END IF
    END IF

  CASE %PHASE_SUPPORTMENU
    action& = GetActionCode&(CVI(mainMenuEntries$, menuSelectedEntry&*2+1))
    IF menuSelectedEntry& >= 0 THEN
      CALL InitArea(activedialoguearea, 0, 0, 0, 0)
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      IF gameMode& = %GAMEMODE_SINGLE THEN
        CALL SupportAction(0, selectedunit&, action&, selectedtarget&)
      ELSE
        CALL ClientUnitAction(selectedunit&, action&, selectedtarget& AND 255, INT(selectedtarget&/256))
      END IF
    END IF

  CASE %PHASE_CAMPAIGNTRAINING
    IF menuSelectedEntry& >= 0 AND menuSelectedEntry& < menuCount&-1 THEN
      CALL InitArea(activedialoguearea, 0, 0, 0, 0)
      dialogueClosing& = 1
      menuOpenTime! = gametime!
      CALL TrainCampaign(0, selectedunit&, menuSelectedEntry&+1)
    END IF
    CALL CheckShopRefuel(selectedunit&)

  END SELECT
END SUB



'Ermittelt die Beschriftungen des Hauptmenüs
FUNCTION GetMenuCaptions AS WSTRING
  LOCAL i&, n&, nr&, e$$

  n& = LEN(mainMenuEntries$)/2
  FOR i& = 1 TO n&
    nr& = CVI(mainMenuEntries$, i&*2-1)
    SELECT CASE nr&
    CASE 0 TO %MAXWORDS-1
      e$$ = e$$+words$$(nr&)
      SELECT CASE nr&
      CASE %WORD_LANGUAGE: e$$ = e$$+": "+langcode$
      CASE %WORD_DIFFICULTY: e$$ = e$$+": "+words$$(%WORD_EASY+defaultDifficulty&)
      CASE %WORD_MUSICVOLUME: e$$ = e$$+" "+FORMAT$(musicVolume&)+" %"
      CASE %WORD_EFFECTVOLUME: e$$ = e$$+" "+FORMAT$(effectVolume&)+" %"
      CASE %WORD_SPEECHVOLUME: e$$ = e$$+" "+FORMAT$(speechVolume&)+" %"
      CASE %WORD_VOICE_STANDARD: e$$ = e$$+": "+voices(0, languageNr&)
      CASE %WORD_VOICE_MISSIONBRIEFING: e$$ = e$$+": "+voices(1, languageNr&)
      CASE %WORD_VOICE_FORECAST: e$$ = e$$+": "+voices(2, languageNr&)
      CASE %WORD_VOICE_AISASCIA: e$$ = e$$+": "+voices(3, languageNr&)
      CASE %WORD_VOICE_MOL_DURAG: e$$ = e$$+": "+voices(4, languageNr&)
      CASE %WORD_VOICE_MALE: e$$ = e$$+": "+voices(5, languageNr&)
      CASE %WORD_VOICE_FEMALE: e$$ = e$$+": "+voices(6, languageNr&)
      CASE %WORD_PLAYERNAME: e$$ = e$$+": "+localPlayerName$
      END SELECT

    CASE %MENUENTRY_LOADGAME TO %MENUENTRY_LOADGAME+%MAXSAVEGAMES-1
      e$$ = e$$+saveFiles$(1, nr&-%MENUENTRY_LOADGAME)

    CASE %MENUENTRY_VOICES TO %MENUENTRY_VOICES+%MAXINSTALLEDVOICES-1
      e$$ = e$$+installedVoices$(nr&-%MENUENTRY_VOICES)

    END SELECT
    IF i& < n& THEN e$$ = e$$+CHR$(13)
  NEXT i&

  GetMenuCaptions = e$$
END FUNCTION



'Hauptmenü anzeigen
SUB ShowMainMenu(submenu&, oldphase&, defaultEntry&)
  LOCAL i&, n&

  'Einträge erstellen
  SELECT CASE submenu&
  CASE %SUBMENU_MAIN, %SUBMENU_BACKTOMAIN
    IF startupaction& = %STARTACTION_TESTMAP THEN
      mainMenuEntries$ = MKI$(%WORD_BACKTOGAME)+MKI$(%WORD_QUIT)
    ELSE
      IF buttonLoadGame.Enabled <> 0 THEN
        mainMenuEntries$ = MKI$(%WORD_STARTGAME)+IIF$(gameState& = %GAMESTATE_INGAME, MKI$(%WORD_BACKTOGAME), "")+MKI$(%WORD_LOADGAME)+MKI$(%WORD_SAVEGAME)+MKI$(%WORD_VIEWREPLAY)+MKI$(%WORD_OPTIONS)+MKI$(%WORD_CREDITS)+MKI$(%WORD_QUIT)
      ELSE
        mainMenuEntries$ = IIF$(gameState& = %GAMESTATE_INGAME, MKI$(%WORD_BACKTOGAME), "")+MKI$(%WORD_OPTIONS)+MKI$(%WORD_QUIT)
      END IF
    END IF

  CASE %SUBMENU_START
    mainMenuEntries$ = MKI$(%WORD_CAMPAIGN_BI2)+MKI$(%WORD_CAMPAIGN_EDT)+MKI$(%WORD_CAMPAIGN_KC)+MKI$(%WORD_CAMPAIGN_MULTIPLAYER)+MKI$(%WORD_CAMPAIGN_BI3)+MKI$(%WORD_SELECTMAP)+MKI$(%WORD_JOIN_MULIPLAYER)+MKI$(%WORD_BACK)
    'mainMenuEntries$ = MKI$(%WORD_GAME_BI1)+MKI$(%WORD_GAME_BI2)+MKI$(%WORD_GAME_BI3)+MKI$(%WORD_SELECTMAP)+MKI$(%WORD_JOIN_MULIPLAYER)+MKI$(%WORD_BACK)

  CASE %SUBMENU_GAMEBI1
    mainMenuEntries$ = MKI$(%WORD_CAMPAIGN_BI1)+MKI$(%WORD_CAMPAIGN_DESERT)+MKI$(%WORD_CAMPAIGN_MOON)+MKI$(%WORD_BACK)

  CASE %SUBMENU_GAMEBI2
    mainMenuEntries$ = MKI$(%WORD_CAMPAIGN_BI2)+MKI$(%WORD_CAMPAIGN_EDT)+MKI$(%WORD_CAMPAIGN_KC)+MKI$(%WORD_CAMPAIGN_MULTIPLAYER)+MKI$(%WORD_BACK)

  CASE %SUBMENU_GAMEBI3
    mainMenuEntries$ = MKI$(%WORD_CAMPAIGN_BI3)+MKI$(%WORD_CAMPAIGN_DRULLIAN)+MKI$(%WORD_CAMPAIGN_TTANNET)+MKI$(%WORD_CAMPAIGN_EMPIRE)+MKI$(%WORD_CAMPAIGN_EPILOGUE)+MKI$(%WORD_BACK)

  CASE %SUBMENU_LOADGAME
    CALL ScanSavegames
    IF nSaveFiles& = 0 THEN
      CALL PrintError(words$$(%WORD_NO_SAVEGAMES))
      CALL ShowMainMenu(%SUBMENU_BACKTOMAIN, oldphase&, 0)
      EXIT SUB
    END IF
    mainMenuEntries$ = ""
    n& = MIN&(6, nSaveFiles&)
    FOR i& = 0 TO n&-1
      mainMenuEntries$ = mainMenuEntries$+MKI$(%MENUENTRY_LOADGAME+i&)
    NEXT i&
    IF nSaveFiles& > n& THEN mainMenuEntries$ = mainMenuEntries$+MKI$(%WORD_SHOW_ALL_SAVEGAMES)+MKI$(%WORD_DELETE_OLD_SAVEGAMES)
    mainMenuEntries$ = mainMenuEntries$+MKI$(%WORD_BACK)

  CASE %SUBMENU_SETTINGS
    mainMenuEntries$ = MKI$(%WORD_LANGUAGE)+MKI$(%WORD_DIFFICULTY)+MKI$(IIF&(combatMode& = 0, %WORD_COMBAT_LARGE, %WORD_COMBAT_SMALL)) _
                      +MKI$(%WORD_MUSICVOLUME)+MKI$(%WORD_EFFECTVOLUME)+MKI$(%WORD_SPEECHVOLUME)+MKI$(%WORD_VOICE)+MKI$(%WORD_PLAYERNAME)+MKI$(%WORD_BACK)

  CASE %SUBMENU_VOICES
    mainMenuEntries$ = MKI$(%WORD_VOICE_STANDARD)+MKI$(%WORD_VOICE_MISSIONBRIEFING)+MKI$(%WORD_VOICE_FORECAST)+MKI$(%WORD_VOICE_AISASCIA)+MKI$(%WORD_VOICE_MOL_DURAG)+MKI$(%WORD_VOICE_MALE)+MKI$(%WORD_VOICE_FEMALE)+MKI$(%WORD_BACK)

  CASE %SUBMENU_ASSIGNVOICE
    mainMenuEntries$ = GetVoicesForLangcode$(langcode$)+MKI$(%WORD_BACK)

  CASE %SUBMENU_EXIT
    mainMenuEntries$ = MKI$(%WORD_SAVE_AND_QUIT)+MKI$(%WORD_EXIT_WITHOUT_SAVING)+MKI$(%WORD_BACK)

  CASE %SUBMENU_ERROR
    mainMenuEntries$ = MKI$(%WORD_EXIT_WITHOUT_SAVING)

  END SELECT

  'Menü anzeigen
  CALL OpenMenu(%PHASE_MAINMENU, defaultEntry&, "", GetMenuCaptions)
  IF oldphase& >= %PHASE_WEAPONMENU AND oldphase& <= %PHASE_SUPPORTMENU THEN menuOpenTime! = gametime!-%DIALOGUE_OPEN_MS/1000  'Öffnen des Menüs überspringen, da Menü bereit offen ist
END SUB



'Aufsteigen-Menü anzeigen
SUB ShowClimbMenu(chnr&, unitnr&)
  LOCAL e$$, px&, py&

  IF replayMode&(chnr&) >= %REPLAYMODE_PLAY THEN EXIT SUB

  IF (channels(chnr&).units(unitnr&).flags AND %US_ASCEND) = 0 THEN
    e$$ = words$$(%WORD_ASCEND)+CHR$(13)+words$$(%WORD_STAYAIR)
  ELSE
    e$$ = words$$(%WORD_DESCENT)+CHR$(13)+words$$(%WORD_STAYAIR)
  END IF
  CALL GetPixelPos(channels(chnr&).units(unitnr&).xpos, channels(chnr&).units(unitnr&).ypos, px&, py&)
  CALL OpenMenu(%PHASE_CLIMBMENU, 0, words$$(%WORD_CHANGEAIRHEIGHT), e$$)
END SUB



'Tauchen-Menü anzeigen
SUB ShowDiveMenu(chnr&, unitnr&)
  LOCAL e$$, px&, py&

  IF replayMode&(chnr&) >= %REPLAYMODE_PLAY THEN EXIT SUB

  IF (channels(chnr&).units(unitnr&).flags AND %US_DIVE) = 0 THEN
    e$$ = words$$(%WORD_DIVE)+CHR$(13)+words$$(%WORD_STAYWATER)
  ELSE
    e$$ = words$$(%WORD_RISE)+CHR$(13)+words$$(%WORD_STAYWATER)
  END IF
  CALL GetPixelPos(channels(chnr&).units(unitnr&).xpos, channels(chnr&).units(unitnr&).ypos, px&, py&)
  CALL OpenMenu(%PHASE_DIVEMENU, 0, words$$(%WORD_CHANGEWATERHEIGHT), e$$)
END SUB



'Baufahrzeug-Menü anzeigen
SUB ShowBuildMenu(chnr&, unitnr&, targetx&, targety&)
  LOCAL px&, py&, unittp&, tg&, canbuild&, canfortify&, overlay&

  IF replayMode&(chnr&) >= %REPLAYMODE_PLAY THEN EXIT SUB

  'Optionen ermitteln
  mainMenuEntries$ = ""
  unittp& = channels(chnr&).units(unitnr&).unittype
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_BUILD) <> 0 THEN canbuild& = 1
  IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_FORTIFY) <> 0 THEN canfortify& = 1
  overlay& = channels(chnr&).zone2(targetx&, targety&)

  'Baufahrzeug kann Schiene auf Straße setzen und Straße auf Schiene, ansonsten nur auf leeres Feld
  IF canbuild& AND overlay& = %SPRITE_ROAD THEN
    mainMenuEntries$ = MKI$(%WORD_BUILDRAIL)+MKI$(%WORD_DESTRUCT)
  ELSE
    IF canbuild& AND overlay& = %SPRITE_RAIL THEN
      mainMenuEntries$ = MKI$(%WORD_BUILDROAD)+MKI$(%WORD_DESTRUCT)
    ELSE
      IF overlay& = -1 THEN
        IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_BUILD) <> 0 THEN mainMenuEntries$ = MKI$(%WORD_BUILDROAD)+MKI$(%WORD_BUILDRAIL)
        IF (channelsnosave(chnr&).unitclasses(unittp&).flags AND %UCF_FORTIFY) <> 0 THEN mainMenuEntries$ = MKI$(%WORD_BUILDTRENCH)
      ELSE
        mainMenuEntries$ = MKI$(%WORD_DESTRUCT)
      END IF
    END IF
  END IF
  'Baufahrzeug kann kleine Hinternisse mit Straße oder Schiene überbauen
  IF canbuild& AND overlay& >= 0 AND (channelsnosave(chnr&).terraindef(overlay&).typemask AND %TERRAIN_OBSTACLE) <> 0 THEN mainMenuEntries$ = MKI$(%WORD_BUILDROAD)+MKI$(%WORD_BUILDRAIL)

  'Einheit bewegen
  IF (channels(chnr&).player(localPlayerNr&).targets(targetx&, targety&) AND %TG_MOVE) <> 0 THEN mainMenuEntries$ = mainMenuEntries$+MKI$(%WORD_MOVE)

  'Menü öffnen
  IF LEN(mainMenuEntries$) = 2 THEN
    IF gameMode& = %GAMEMODE_SINGLE THEN
      CALL BuildAction(chnr&, unitnr&, GetActionCode&(CVI(mainMenuEntries$)), targetx&+targety&*256)
    ELSE
      CALL ClientUnitAction(unitnr&, GetActionCode&(CVI(mainMenuEntries$)), targetx&, targety&)
    END IF
  ELSE
    channels(0).player(localPlayerNr&).selectedtarget = targetx&+targety&*256
    CALL GetPixelPos(channels(chnr&).units(unitnr&).xpos, channels(chnr&).units(unitnr&).ypos, px&, py&)
    CALL OpenMenu(%PHASE_BUILDMENU, 0, words$$(%WORD_SELECTACTION), GetMenuCaptions)
  END IF
END SUB



'Reparatur-Menü anzeigen
SUB ShowSupportMenu(chnr&, supportunitnr&, options&, targetx&, targety&)
  LOCAL px&, py&

  IF replayMode&(chnr&) >= %REPLAYMODE_PLAY THEN EXIT SUB

  'Optionen ermitteln
  mainMenuEntries$ = ""
  IF (options& AND %TG_REPAIR) <> 0 THEN mainMenuEntries$ = mainMenuEntries$+MKI$(%WORD_REPAIR)
  IF (options& AND (%TG_REFUEL OR %TG_RECHARGE)) <> 0 THEN mainMenuEntries$ = mainMenuEntries$+MKI$(%WORD_REFUEL)
  IF (options& AND %TG_MOVE) <> 0 THEN mainMenuEntries$ = mainMenuEntries$+MKI$(%WORD_MOVE)

  'Menü öffnen
  channels(0).player(localPlayerNr&).selectedtarget = targetx&+targety&*256
  CALL GetPixelPos(channels(chnr&).units(supportunitnr&).xpos, channels(chnr&).units(supportunitnr&).ypos, px&, py&)
  CALL OpenMenu(%PHASE_SUPPORTMENU, 0, words$$(%WORD_SELECTACTION), GetMenuCaptions)
END SUB



'Karteninformationen anzeigen
SUB ShowMapInfo
  mapinfoOpenTime! = gametime!
END SUB



'Karteninformationen wieder schließen
SUB CloseMapInfo
  mapinfoOpenTime! = gametime!
  mapinfoScrollPos& = 0
  buttonClose.Visible = 0
  dialogueClosing& = 1
END SUB



'Ermittelt die Gesamtanzahl sowie die Anzahl der bereits benutzten Einheiten eines Spielers
SUB GetUnitCount(BYVAL chnr&, BYVAL plnr&, totalunits&, usedunits&)
  LOCAL unitnr&

  'Einheiten zählen
  totalunits& = 0
  usedunits& = 0
  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF UnitIsAlive&(chnr&, unitnr&) AND channels(chnr&).units(unitnr&).owner = plnr& THEN
      totalunits& = totalunits&+1
      IF (channels(chnr&).units(unitnr&).flags AND %US_DONE) <> 0 THEN usedunits& = usedunits&+1
    END IF
  NEXT i&
END SUB



'Ermittelt die Karten-Informationen
SUB GetMapInfo(info&())
  LOCAL unitnr&, unittp&, owner&, shopnr&, pl&, x&, y&
  REDIM info&(6, 6)

  'Einheiten zählen
  FOR unitnr& = 0 TO channels(0).info.nunits-1
    IF UnitIsAlive&(0, unitnr&) THEN
      owner& = channels(0).units(unitnr&).owner
      x& = channels(0).units(unitnr&).xpos
      y& = channels(0).units(unitnr&).ypos
      IF channels(0).info.difficulty = %DIFFICULTY_EASY OR (channels(0).player(localPlayerNr&).allymask AND 2^owner&) <> 0 _
        OR ((channels(0).vision(x&, y&) AND 2^localPlayerNr&) <> 0 AND UnitIsInShop&(0, unitnr&) = -1 AND UnitIsInTransporter&(0, unitnr&) = -1) THEN
        unittp& = channels(0).units(unitnr&).unittype
        IF (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN
          info&(1, owner&) = info&(1, owner&)+1
        ELSE
          IF (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN
            info&(2, owner&) = info&(2, owner&)+1
          ELSE
            info&(0, owner&) = info&(0, owner&)+1
          END IF
        END IF
      END IF
    END IF
  NEXT i&

  'Gesamtenergie auslesen
  FOR pl& = 0 TO 6
    info&(3, pl&) = channels(0).player(pl&).energy
  NEXT pl&

  'Gesamtmaterial sowie Energie- und Materialzuwachs zählen
  FOR shopnr& = 0 TO channels(0).info.nshops-1
    owner& = channels(0).shops(shopnr&).owner
    IF channels(0).shops(shopnr&).shoptype > 0 THEN
      info&(4, owner&) = info&(4, owner&)+channels(0).shops(shopnr&).material
      info&(5, owner&) = info&(5, owner&)+channels(0).shops(shopnr&).eplus
      info&(6, owner&) = info&(6, owner&)+channels(0).shops(shopnr&).mplus
    END IF
  NEXT shopnr&
END SUB



'Ermittelt die Sprachausgabe-Stimme für eine Nachricht
FUNCTION GetVoiceForMessage&(BYVAL msgid&)
  LOCAL voicenr&

  '0 = VOICE_STANDARD
  '1 = VOICE_MISSIONBRIEFING
  '2 = VOICE_FORECAST
  '3 = VOICE_AISASCIA
  '4 = VOICE_MOL_DURAG
  '5 = VOICE_MALE
  '6 = VOICE_FEMALE
  IF msgid& < DATACOUNT THEN
    voicenr& = VAL(READ$(msgid&+1))
  ELSE
    voicenr& = 0
  END IF
  GetVoiceForMessage& = voicenr&

  '0
  DATA 0, 2, 2, 2, 2
  '5
  DATA 2, 0, 0, 1, 1
  '10
  DATA 5, 5, 5, 5, 0
  '15
  DATA 0, 0, 5, 5, 1
  '20
  DATA 1, 1, 1, 1, 1
  '25
  DATA 5, 3, 5, 5, 0
  '30
  DATA 6, 4, 5, 5, 5
  '35
  DATA 5, 5, 5, 5, 5
  '40
  DATA 5, 5, 5, 5, 5
  '45
  DATA 5, 3, 1, 4, 5
  '50
  DATA 5, 4, 5, 1, 5
  '55
  DATA 5, 1, 5, 5, 5
  '60
  DATA 3, 5, 0, 1, 5
  '65
  DATA 1, 1, 5, 5, 5
  '70
  DATA 5, 1, 1, 6, 5
  '75
  DATA 0, 0, 0, 0, 0
  '80
  DATA 0, 0, 1, 5, 5
  '85
  DATA 5, 1, 5, 5, 4
  '90
  DATA 1, 5, 5, 3, 1
  '95
  DATA 6, 0, 0, 6
END FUNCTION



'Text-Nachricht zu Spiel-Nachricht ermitteln
FUNCTION GetTextIdForMessage&(msgid&)
  LOCAL nsteps&
  LOCAL animsteps() AS TAnimationScript

  nsteps& = GetAnimationSequence&(animsteps(), msgid&, 0, %MSGANI_TXT)
  IF nsteps& > 0 THEN
    GetTextIdForMessage& = animsteps(0).sequence
  ELSE
    GetTextIdForMessage& = msgid&
  END IF
END FUNCTION



'Video-Nachricht anzeigen
SUB ShowVideoMessage(BYVAL chnr&, BYVAL plnr&, BYVAL msgid&)
  LOCAL a$$, f$, pixeldata$, fileid&, freezeFrameNr&, hFile&, hVideoStream&, hAudioStream&
  LOCAL audioFormat&, channelCount&, bitsPerSample&, streamLengthInBytes&

  'Videodatei öffnen
  gameMessageKind& = 0
  fileid& = GetFileForVideoMessageId&(msgid&)
  IF fileid& < 0 THEN EXIT SUB
  f$ = aviFolder$+"API"+FORMAT$(fileid&, "000")+".avi"
  CALL BIDebugLog("Initializing video message "+FORMAT$(msgid&)+" from file "+f$)
  hFile& = OpenAviFile&(f$)
  IF hFile& = 0 THEN
    a$$ = words$$(%ERRMSG_AVI_NOTFOUND)
    REPLACE "%" WITH f$ IN a$$
    CALL PrintError(a$$)
    EXIT SUB
  END IF
  hVideoStream& = GetAviVideoStream&(hFile&)
  IF hVideoStream& = 0 THEN
    CALL BIDebugLog("GetAviVideoStream failed because no matching video decompressor was found")
    CALL PrintError(words$$(%ERRMSG_AVI_NO_DECOMPRESSOR_FOUND))
    EXIT SUB
  END IF

  'Video-Metadaten auslesen
  CALL GetAVIVideoStreamInfo(hVideoStream&, videoFrameWidth&, videoFrameHeight&, videoFrameCount&, videoMillisecsPerFrame&)
  CALL BIDebugLog("Video size: "+FORMAT$(videoFrameWidth&)+"x"+FORMAT$(videoFrameHeight&)+" , "+FORMAT$(videoFrameCount&)+" frames , total length "+FORMAT$(videoMillisecsPerFrame&*videoFrameCount&/1000, "0.0")+" seconds")

  'Standbild aus Sekunde 10 erzeugen
  freezeFrameNr& = GetAVIVideoFrameNumberForMillisecond&(10000)
  IF freezeFrameNr& < 0 OR freezeFrameNr& >= videoFrameCount& THEN freezeFrameNr& = videoFrameCount&/2
  pixeldata$ = GetAVIFramePixelData$(freezeFrameNr&)
  IF LEN(pixeldata$) = 0 THEN pixeldata$ = STRING$(videoFrameWidth&*videoFrameHeight&*4, 0)
  IF hVideoFreezeFrame& = 0 THEN
    hVideoFreezeFrame& = D2D.CreateMemoryBitmap(videoFrameWidth&, videoFrameHeight&, pixeldata$)
  ELSE
    D2D.ReuseMemoryBitmap(hVideoFreezeFrame&, videoFrameWidth&, videoFrameHeight&, pixeldata$)
  END IF

  'Audio-Stream vorbereiten
  soundchannels(%SOUNDBUFFER_VIDEO).Stop
  audioStreamData$ = ""
  hAudioStream& = GetAviAudioStream&(hFile&, 0)
  IF hAudioStream& <> 0 THEN
    CALL GetAVIAudioStreamInfo(hAudioStream&, audioFormat&, channelCount&, audioSamplesPerSecond&, bitsPerSample&, streamLengthInBytes&)
    IF audioFormat& = %WAVE_FORMAT_PCM AND channelCount& = 1 THEN
      audioStreamData$ = ReadAVIAudioStream$(hAudioStream&, 0, -1)
      IF bitsPerSample& = 8 THEN audioStreamData$ = DS.Convert8BitWaveTo16Bit(audioStreamData$)
    END IF
  END IF

  'Video-Fenster öffnen
  messageOpenTime! = gametime!
  currentMessageId& = msgid&
  currentVideoFrame& = -1
  gameMessageKind& = 2
END SUB



'Spiel-Nachricht anzeigen
SUB ShowGameMessage(BYVAL chnr&, BYVAL plnr&, BYVAL msgid&)
  LOCAL a$$, voicenr&, custvoice&, n&, i&
  LOCAL textrows$$()

  gameMessageKind& = 0
  IF msgid& < 0 OR msgid& >= %MAXANIMATIONS THEN EXIT SUB
  IF replayMode&(chnr&) = %REPLAYMODE_FASTPLAY THEN EXIT SUB
  IF msgid& < 256 AND IsEDTMission&(channels(chnr&).info.currentmission) <> 0 THEN msgid& = msgid&+256

  IF gameMode& = %GAMEMODE_SERVER THEN
    CALL SendGameMessage(chnr&, plnr&, msgid&)
    EXIT SUB
  END IF
  gameMessageKind& = 1
  IF plnr& <> localPlayerNr& THEN EXIT SUB

  'Nachrichten-Fenster öffnen
  messageOpenTime! = gametime!
  currentMessageId& = msgid&
  currentTextId& = GetTextIdForMessage&(msgid&)
  messageSender$ = ""
  msgSenderCard& = -1
  gameMessageScrollY& = 0

  'Nachricht sprechen
  a$$ = TRIM$(GetGameMessageText(currentTextId&, 2, 1))
  voicenr& = GetVoiceForMessage&(msgid&)
  IF voices$(voicenr&, languageNr&) <> "" AND speechVolume& > 0 THEN
    IF UCASE$(LEFT$(a$$, 4)) = "^VOC" THEN
      custvoice& = VAL(MID$(a$$, 5, 1))
      a$$ = MID$(a$$, 6)
      IF custvoice& < %MAXVOICES AND voices$(custvoice&, languageNr&) <> "" THEN
        voicenr& = custvoice&
        currentMessageId& = voicenr&+%ANI_COMPUTER  'Video-Animation aus Stimme ermitteln
      END IF
    END IF
    CALL SAPISpeak(SAPISetVoice(SAPISetVolume(speechVolume&, SAPISetRate(speechRate&, SAPISilence(1000)+a$$)), 0, 0, voices$(voicenr&, languageNr&)))
  ELSE
    'Video-Animation aus Stimme ermitteln
    IF UCASE$(LEFT$(a$$, 4)) = "^VOC" THEN
      custvoice& = VAL(MID$(a$$, 5, 1))
      IF custvoice& < %MAXVOICES THEN currentMessageId& = custvoice&+%ANI_COMPUTER
    END IF
  END IF

  'Nachricht ins Protokoll schreiben
  a$$ = TRIM$(GetGameMessageText(currentTextId&, 1, 1))
  n& = TextToRows&(a$$, maparea.right-maparea.left-20, hGameMessageFont&, textrows$$())
  CALL AddProtocol("")
  FOR i& = 0 TO n&-1
    CALL AddProtocol(textrows$$(i&)+"")
  NEXT i&
  CALL AddProtocol("")
END SUB



'Spiel-Nachricht schließen
SUB CloseGameMessage
  buttonClose.Visible = 0

  IF currentMessageId& >= 0 THEN
    IF IsVideoMessage&(0, currentMessageId&) = 0 THEN
      messageOpenTime! = MIN(messageOpenTime!, gametime!-LEN(GetGameMessageText(currentTextId&, 0, 0))/%GAMEMESSAGE_SPEED-3-%DIALOGUE_OPEN_MS/1000)
    ELSE
      messageOpenTime! = gametime!
      dialogueClosing& = 1
      CALL CloseAVIFile
      CALL BIDebugLog("AVI video message file closed.")
    END IF
  END IF
  IF channels(0).info.state >= %CHANNELSTATE_VICTORY AND channels(0).info.state <= %CHANNELSTATE_DEFEAT AND gameState& <> %GAMESTATE_GAMEOVER THEN
    CALL CalculateScore(0, localPlayerNr&, scoreGroundUnit&, scoreAirUnits&, scoreWaterUnits&, unitclassesByXp$)
    gameoverOpenTime! = gametime!
    gameState& = %GAMESTATE_GAMEOVER
    channels(0).campaign.groundscore = channels(0).campaign.groundscore+scoreGroundUnit&
    channels(0).campaign.waterscore = channels(0).campaign.waterscore+scoreWaterUnits&
    channels(0).campaign.airscore = channels(0).campaign.airscore+scoreAirUnits&
    IF channels(0).info.state = %CHANNELSTATE_VICTORYBONUS THEN channels(0).campaign.secrets = channels(0).campaign.secrets+1
    CALL EnableAllMenuButtons
  END IF

  'Sprachausgabe abbrechen
  CALL SAPISPEAK("")
  soundchannels(%SOUNDBUFFER_EFFECT2).Stop
  soundchannels(%SOUNDBUFFER_EFFECT3).Stop
  soundchannels(%SOUNDBUFFER_EFFECT4).Stop
END SUB



'Nächste Einheit auf der Karte auswählen
SUB SelectNextUnit
  LOCAL i&, k&, n&, unitnr&, dist&, bestunitnr&, bestdist&

  FOR k& = 0 TO 1
    'dichteste Einheit ermitteln, die noch nicht angetabbt wurde und noch Aktionen hat
    bestunitnr& = -1
    bestdist& = 999
    n& = LEN(tabbedUnits$)/2
    FOR unitnr& = 0 TO channels(0).info.nunits-1
      IF UnitIsAlive&(0, unitnr&) <> 0 AND channels(0).units(unitnr&).owner = localPlayerNr& AND (channels(0).units(unitnr&).flags AND %US_DONE) = 0 THEN
        FOR i& = 1 TO n&
          IF CVI(tabbedUnits$, i&*2-1) = unitnr& THEN EXIT FOR
        NEXT i&
        IF i& > n& THEN
          dist& = GetDistance&(cursorXPos&, cursorYPos&, channels(0).units(unitnr&).xpos, channels(0).units(unitnr&).ypos)
          IF dist& < bestdist& THEN
            bestdist& = dist&
            bestunitnr& = unitnr&
          END IF
        END IF
      END IF
    NEXT unitnr&

    'gefundene Einheit anwählen
    IF bestunitnr& >= 0 THEN
      cursorXPos& = channels(0).units(bestunitnr&).xpos
      cursorYPos& = channels(0).units(bestunitnr&).ypos
      CALL ScrollToMapPosWhenNearEdge(cursorXPos&, cursorYPos&, 0.5)
      CALL SelectUnit(bestunitnr&, 1)
      EXIT SUB
    END IF

    IF tabbedUnits$ = "" THEN EXIT SUB
    tabbedUnits$ = ""
  NEXT k&
END SUB



'Einheit auf der Karte auswählen
SUB SelectUnit(BYVAL unitnr&, BYVAL selectedByTab&)
  LOCAL unittp&, md&, shopnr&, i&, targetunits&()

  IF NOT LocalPlayersTurn& THEN EXIT SUB

  'Liste mit angetabbten Einheiten aktualisieren
  IF selectedByTab& = 0 THEN
    tabbedUnits$ = ""
  ELSE
    shopnr& = UnitIsInShop&(0, unitnr&)
    IF shopnr& < 0 THEN
      tabbedUnits$ = tabbedUnits$+MKI$(unitnr&)
    ELSE
      FOR i& = 0 TO 15
        IF channels(0).shops(shopnr&).content(i&) >= 0 THEN tabbedUnits$ = tabbedUnits$+MKI$(channels(0).shops(shopnr&).content(i&))
      NEXT i&
    END IF
  END IF

  IF unitnr& < 0 THEN
    IF channels(0).player(localPlayerNr&).selectedunit >= 0 THEN
      CALL UnselectUnit(0, localPlayerNr&)
      CALL ClearTargets(0, localPlayerNr&)
    END IF
    buttonShopMove.Enabled = 0
    channels(0).player(localPlayerNr&).selectedunit = -1
    EXIT SUB
  END IF

  'falls eine andere Einheit bereits angewählt ist, diese abwählen
  IF channels(0).player(localPlayerNr&).selectedunit >= 0 AND channels(0).player(localPlayerNr&).selectedunit <> unitnr& THEN
    CALL UnselectUnit(0, localPlayerNr&)
  END IF

  'Einheit/Shop auswählen
  IF channels(0).units(unitnr&).owner = localPlayerNr& THEN
    channels(0).player(localPlayerNr&).selectedunit = unitnr&
    CALL SetPhase(0, localPlayerNr&, %PHASE_UNITSELECTED)
    unitSelectionTime! = gametime!
    buttonShopMove.Enabled = 0
    CALL ClearTargets(0, localPlayerNr&)
    IF (channels(0).units(unitnr&).flags AND %US_DONE) = 0 THEN
      md& = 1
      IF (channels(0).units(unitnr&).flags AND %US_ATTACKED) = 0 THEN md& = md& OR 2
      CALL GetTargets&(0, unitnr&, md&, 0, targetunits&())
      IF selectedShop& >= 0 THEN buttonShopMove.Enabled = 1
    END IF

    'Sound-Effekt abspielen
    unittp& = channels(0).units(unitnr&).unittype
    CALL PlaySoundEffect(hFirstEffect&+channelsnosave(0).unitclasses(unittp&).sfxselection, %SOUNDBUFFER_EFFECT1, %PLAYFLAGS_NONE)
    soundchannels(%SOUNDBUFFER_EFFECT1).SetSoundEffect(%DX_SOUNDEFFECT_FADEOUT, 1.5)
  END IF
END SUB



'Zuletzt angewählten Transporter wieder anwählen
SUB ReselectTransporter
  IF GetTransportWeight&(0, lastSelectedTransporter&) = 0 AND (channels(0).units(lastSelectedTransporter&).flags AND %US_DONE) <> 0 THEN EXIT SUB
  cursorXPos& = channels(0).units(lastSelectedTransporter&).xpos
  cursorYPos& = channels(0).units(lastSelectedTransporter&).ypos
  CALL ScrollToMapPosWhenNearEdge(cursorXPos&, cursorYPos&, 0.5)
  SelectUnit(lastSelectedTransporter&, 0)
END SUB



'Produktions-Slot im Shop auswählen
SUB SelectProduction(BYVAL unittp&)
  LOCAL unitnr&, costenergy&, costmat&, shopenabled&

  IF unittp& >= 0 THEN shopCursorPos& = INSTR(selectedShopProd$, CHR$(unittp&))+15
  selectedProduction& = unittp&
  channels(0).player(localPlayerNr&).selectedunit = -1
  CALL ClearTargets(0, localPlayerNr&)

  'Produktions-Button freischalten falls ausreichend Energie/Material vorhanden ist
  costenergy& = channelsnosave(0).unitclasses(unittp&).costenergy
  costmat& = channelsnosave(0).unitclasses(unittp&).costmaterial
  IF channels(0).shops(selectedShop&).owner = localPlayerNr& THEN shopenabled& = 1
  buttonShopBuild.Enabled = IIF&(costenergy& <= channels(0).player(localPlayerNr&).energy AND costmat& <= channels(0).shops(selectedShop&).material AND GetFreeShopSlot&(0, selectedShop&) >= 0, shopenabled&, 0)
  buttonShopMove.Enabled = 0
  buttonShopRefuel.Enabled = 0
  buttonShopRepair.Enabled = 0
  buttonShopTrain.Enabled = 0

  'Einheiten-Vorschau erzeugen
  productionPreviewUnit& = CreateUnit&(0, channels(0).shops(selectedShop&).position, channels(0).shops(selectedShop&).position2, unittp&, localPlayerNr&, 1)
  lastPreviewUnit& = -2
  unitSelectionTime! = gametime!
END SUB



'Prüft, ob die im Shop gewählte Einheit befüllt oder repariert werden muß (oder trainiert werden kann)
SUB CheckShopRefuel(unitnr&)
  LOCAL unittp&, shoptype&, availenergy&, availmat&, weaponnr&, shopenabled&

  buttonShopRefuel.Enabled = 0
  buttonShopRepair.Enabled = 0
  buttonShopTrain.Enabled = 0
  buttonShopBuild.Enabled = 0
  IF unitnr& < 0 THEN EXIT SUB

  unittp& = channels(0).units(unitnr&).unittype
  availenergy& = channels(0).player(localPlayerNr&).energy
  availmat& = channels(0).shops(selectedShop&).material
  shoptype& = channels(0).shops(selectedShop&).shoptype
  IF channels(0).shops(selectedShop&).owner = localPlayerNr& THEN shopenabled& = 1

  'prüfen, ob Einheit befüllt werden muß
  IF channels(0).units(unitnr&).fuel < channelsnosave(0).unitclasses(unittp&).fuel AND availenergy& > 0 THEN buttonShopRefuel.Enabled = shopenabled&
  FOR weaponnr& = 0 TO 3
    IF channels(0).units(unitnr&).ammo(weaponnr&) < channelsnosave(0).unitclasses(unittp&).weapons(weaponnr&).ammo AND availmat& > 0 THEN buttonShopRefuel.Enabled = shopenabled&
  NEXT i&

  'prüfen, ob Einheit repariert werden muß
  IF channels(0).units(unitnr&).groupsize < channelsnosave(0).unitclasses(unittp&).groupsize AND availenergy& > 0 AND availmat& > 0 THEN buttonShopRepair.Enabled = shopenabled&

  'prüfen, ob Einheit mit Kampagnenpunkten trainiert werden kann
  IF (shoptype& = %SHOPTYPE_HQ OR shoptype& = %SHOPTYPE_AIRPORT OR shoptype& = %SHOPTYPE_HARBOUR OR shoptype& = %SHOPTYPE_FACTORY) AND channels(0).units(unitnr&).experience < %MAX_EXPERIENCE AND (channels(0).units(unitnr&).flags AND %US_DONE) = 0 THEN
    IF gameMode& = %GAMEMODE_SINGLE THEN
      IF channels(0).campaign.groundscore > 0 AND (channelsnosave(0).unitclasses(unittp&).flags AND (%UCF_PLANE OR %UCF_SHIP)) = 0 THEN buttonShopTrain.Enabled = shopenabled&
      IF channels(0).campaign.waterscore > 0 AND (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 THEN buttonShopTrain.Enabled = shopenabled&
      IF channels(0).campaign.airscore > 0 AND (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN buttonShopTrain.Enabled = shopenabled&
    END IF
  END IF

  IF shoptype& = %SHOPTYPE_ACADEMY THEN
    'prüfen, ob Einheit trainiert werden kann
    IF channels(0).units(unitnr&).experience < %MAX_EXPERIENCE AND availenergy& >= 4 AND (channels(0).units(unitnr&).flags AND %US_DONE) = 0 THEN buttonShopTrain.Enabled = shopenabled&
  END IF
END SUB



'Einheiten-Animation für Shop festlegen
SUB SetShopAnimation(unitnr&, animation&)
  shopAnimationUnit& = unitnr&
  shopAnimationType& = animation&
  shopAnimationTime! = gametime!
END SUB



'Shop auf der Karte auswählen
SUB SelectShop(BYVAL shopnr&)
  LOCAL unitnr&

  selectedShop& = shopnr&
  selectedShopProd$ = GetShopProductionMenu$(0, shopnr&, 0, 0)
  selectedProduction& = -1
  productionPreviewUnit& = -1
  buttonShopBuild.Enabled = 0
  buttonShopMove.Enabled = 0
  buttonShopRefuel.Enabled = 0
  buttonShopRepair.Enabled = 0
  buttonShopTrain.Enabled = 0
  shopSelectionTime! = gametime!
  shopCursorPos& = 0
  lastSelectedTransporter& = -1
  CALL ClearTargets(0, localPlayerNr&)

  'falls sich eine Einheit im ersten Slot befindet, diese auswählen
  unitnr& = channels(0).shops(selectedShop&).content(shopCursorPos&)
  IF unitnr& >= 0 THEN
    CALL CheckShopRefuel(unitnr&)
    CALL SelectUnit(unitnr&, 0)
  END IF
END SUB



'Einheit bewegen
SUB MoveUnit(BYVAL unitnr&, BYVAL newx&, BYVAL newy&)
  LOCAL x&, y&, unr&, shopnr&, owner&

  CALL CalculateUnitPath(0, unitnr&, newx&, newy&, 0)
  owner& = channels(0).units(unitnr&).owner
  x& = channels(0).units(unitnr&).xpos
  y& = channels(0).units(unitnr&).ypos
  CALL SetPhase(0, owner&, %PHASE_UNITMOVING)
  IF channels(0).zone3(x&, y&) = unitnr& THEN
    channels(0).zone3(x&, y&) = -1  'Einheit temporär von der Karte entfernen (wird am Ende der Animation wieder auf dem Zielfeld platziert)
  ELSE
    unr& = channels(0).zone3(x&, y&)
    IF unr& >= 0 THEN
      CALL TransportUnload(0, unr&, unitnr&)
    ELSE
      shopnr& = -2-unr&
      IF shopnr& >= 0 THEN CALL RemoveUnitFromShop(0, shopnr&, unitnr&)
    END IF
  END IF
  unitMovementStartTime! = gametime!
  IF replayMode&(0) = %REPLAYMODE_FASTPLAY THEN CALL EndMovement(0, unitnr&, newx&, newy&)
END SUB



'Gegner/Feld angreifen
SUB Attack(attacker&, x&, y&)
  LOCAL dist&, defender&, attunittp&, weaponnr&, i&, z&, iconx&, icony&, weaponlist$, e$$
  LOCAL w AS TWeapon

  'Entfernung zwischen den Einheiten berechnen
  dist& = GetDistance&(channels(0).units(attacker&).xpos, channels(0).units(attacker&).ypos, x&, y&)

  'Einheitenart des Verteidigers ermitteln (Luft, Land, Wasser, etc.)
  defender& = channels(0).zone3(x&, y&)
  z& = 2^channels(0).units(defender&).zpos
  channels(0).player(localPlayerNr&).selectedtarget = defender&

  'passende Waffen ermitteln
  attunittp& = channels(0).units(attacker&).unittype
  FOR weaponnr& = 0 TO 3
    w = channelsnosave(0).unitclasses(attunittp&).weapons(weaponnr&)
    IF channels(0).units(attacker&).ammo(weaponnr&) > 0 AND w.damage > 0 THEN
      IF w.minrange <= dist& AND w.maxrange >= dist& AND (w.targets AND z&) <> 0 THEN weaponlist$ = weaponlist$+CHR$(weaponnr&+1)
    END IF
  NEXT weaponnr&

  'falls nur 1 Waffe gefunden wurde, mit dieser angreifen
  IF LEN(weaponlist$) = 1 THEN
    IF gameMode& = %GAMEMODE_SINGLE THEN
      CALL StartCombat(0, attacker&, defender&, ASC(weaponlist$), IIF&(dist& = 1, -1, 0), 0)
    ELSE
      CALL ClientAttackUnit(attacker&, defender&, ASC(weaponlist$))
    END IF
    EXIT SUB
  END IF

  'Waffen-Menü anzeigen
  FOR i& = 1 TO LEN(weaponlist$)
    weaponnr& = ASC(weaponlist$, i&)-1
    CALL GetWeaponIcon(attacker&, weaponnr&, iconx&, icony&)
    e$$ = e$$+CHR$(weaponnr&+65)+"   "+CHR$(1)+FORMAT$(iconx&, "0000")+FORMAT$(icony&, "0000")+"  "+FORMAT$(channels(0).units(attacker&).ammo(weaponnr&))+"  "+FORMAT$(channelsnosave(0).unitclasses(attunittp&).weapons(weaponnr&).damage)
    IF i& < LEN(weaponlist$) THEN e$$ = e$$+CHR$(13)
  NEXT i&
  weaponnr& = GetBestWeapon&(0, attacker&, defender&)
  CALL OpenMenu(%PHASE_WEAPONMENU, MAX&(0, weaponnr&-1), words$$(%WORD_SELECTWEAPON), e$$)
END SUB



'Rakete erzeugen
SUB CreateMissile(unitnr&, target&, weaponnr&, damage&)
  LOCAL i&, unittp&, tg&, dist&

  IF weaponnr& <= 0 THEN EXIT SUB

  'freien Raketen-Slot suchen
  FOR i& = 0 TO %MAXMISSILES-1
    IF missiles(i&).position = 0 THEN EXIT FOR
  NEXT i&
  IF i& = %MAXMISSILES THEN EXIT SUB

  'Parameter berechnen
  unittp& = channels(0).units(unitnr&).unittype
  tg& = channelsnosave(0).unitclasses(unittp&).weapons(weaponnr&-1).targets
  missiles(i&).startx = channels(0).units(unitnr&).xpos
  missiles(i&).starty = channels(0).units(unitnr&).ypos
  missiles(i&).endx = channels(0).units(target&).xpos
  missiles(i&).endy = channels(0).units(target&).ypos
  dist& = GetDistance&(missiles(i&).startx, missiles(i&).starty,  missiles(i&).endx,  missiles(i&).endy)
  missiles(i&).animationlength = 20+dist&*10
  missiles(i&).position = missiles(i&).animationlength
  missiles(i&).ownertype& = unittp&
  missiles(i&).weapontargets = tg&
  missiles(i&).damage = damage&
END SUB



'Musik-Lautstärke ändern
SUB ChangeMusicVolume(v&)
  musicVolume& = musicVolume&+v&
  IF musicVolume& < 0 THEN musicVolume& = 0
  IF musicVolume& > 100 THEN musicVolume& = 100
  CALL SetMusicVolume
END SUB



'Musik-Lautstärke setzen
SUB SetMusicVolume
  LOCAL v$, lv&

  IF ISOBJECT(pAfxMp3) THEN
    IF messageOpenTime! > 0 THEN
      IF effectiveMusicVolume& > 0 THEN effectiveMusicVolume& = effectiveMusicVolume&-1
    ELSE
      IF effectiveMusicVolume& < musicVolume& THEN effectiveMusicVolume& = MIN&(effectiveMusicVolume&+1, musicVolume&)
      IF effectiveMusicVolume& > musicVolume& THEN effectiveMusicVolume& = MAX&(effectiveMusicVolume&-1, musicVolume&)
    END IF

    v$ = MKI$(-10000)+MKI$(-8000)+MKI$(-6300)+MKI$(-4800)+MKI$(-3500)+MKI$(-2400)+MKI$(-1500)+MKI$(-1000)+MKI$(-600)+MKI$(-300)+MKI$(0)
    lv& = INT(effectiveMusicVolume&/10)
    pAfxMp3.Volume = CVI(v$, lv&*2+1)
  END IF
END SUB



'Musik von vorne abspielen, falls Ende des Soundtracks erreicht wurde
SUB LoopMusic
  LOCAL musiclen&&, currentpos&&

  IF ISNOTHING(pAfxMp3) OR musicVolume& = 0 THEN EXIT SUB
  musiclen&& = pAfxMp3.Duration
  currentpos&& = pAfxMp3.CurrentPosition
  IF musiclen&& > 0 AND currentpos&& >= musiclen&& THEN
    pAfxMp3.SetPositions(0, musiclen&&, %TRUE)
  END IF
END SUB



'Soundtrack laden und abspielen
SUB StartMusic(tracknr&)
  LOCAL a$$, f$, soundnumber&, r&

  IF gameMode& = %GAMEMODE_SERVER OR musicVolume& = 0 THEN EXIT SUB

  IF tracknr& < 1 OR tracknr& > nSoundTracks& THEN
    a$$ = words$$(%WORD_SOUNDTRACK_NOTFOUND)
    REPLACE "%" WITH FORMAT$(tracknr&) IN a$$
    CALL PrintError(a$$)
    EXIT SUB
  END IF

  IF ISOBJECT(pAfxMp3) THEN
    f$ = musicfiles$(tracknr&-1)
    r& = pAfxMp3.Load(EXE.PATH$+f$)
    IF r& <> %TRUE THEN
      a$$ = words$$(%WORD_SOUNDTRACK_INVALID)
      REPLACE "%" WITH f$ IN a$$
      CALL PrintError(a$$)
      IF UCASE$(RIGHT$(f$, 4)) = ".OGG" THEN CALL CheckOGGFilter
    ELSE
      pAfxMp3.Run
      CALL SetMusicVolume
      currentSoundTrack& = tracknr&
    END IF
  END IF
END SUB



'Sound effekt abspielen
SUB PlaySoundEffect(soundnumber&, soundchannel&, flags&)
  IF soundInitialized& = 0 OR gameMode& = %GAMEMODE_SERVER THEN EXIT SUB

  'Channel erzeugen und 100 ms des Soundtracks cachen
  soundchannels(soundchannel&).InitChannel(DS, soundnumber&)
  soundchannels(soundchannel&).SetVolume(effectVolume&/100, effectVolume&/100)
  soundchannels(soundchannel&).FillBuffer(100)
  soundchannels(soundchannel&).Play(flags&)
END SUB



'Alle Sound-Channels anhalten
SUB StopAllSoundEffects
  LOCAL i&

  FOR i& = 0 TO %MAXSOUNDCHANNELS-1
    IF ISOBJECT(soundchannels(i&)) THEN soundchannels(i&).Stop
  NEXT i&
END SUB



'Straße/Weg/Graben/Schiene weich zeichnen
FUNCTION SmoothRoad&(BYVAL sprnr&, BYVAL x&, BYVAL y&, spritehandle&)
  LOCAL i&, k&, m&, d&, spr&, buildings$
  LOCAL xFieldsToSmooth&(), yFieldsToSmooth&()

  'alle angrenzenden Felder ermitteln
  CALL GetAdjacentFields(0, x&, y&, xFieldsToSmooth&(), yFieldsToSmooth&())

  'angrenzende identische Sprites oder Gebäude suchen
  buildings$ = MKL$(54)+MKL$(55)+MKL$(76)+MKL$(71)+MKL$(80)+MKL$(81)+MKL$(44)+MKL$(45)+MKL$(66)+MKL$(63)+MKL$(69)+MKL$(86)+MKL$(87)+MKL$(88)+MKL$(393)+MKL$(395)+MKL$(563)
  SELECT CASE sprnr&
  CASE %SPRITE_ROAD:  'Straße
    buildings$ = buildings$+MKL$(1264)+MKL$(1265)+MKL$(1266)+MKL$(1270)+MKL$(1271)+MKL$(1272)+MKL$(1274)+MKL$(%SPRITE_SNOWCOVERED_ROAD)
    spritehandle& = hRoads&(0)
  CASE %SPRITE_RAIL:  'Schiene
    buildings$ = buildings$+MKL$(1265)+MKL$(1267)+MKL$(1269)+MKL$(1270)+MKL$(1272)+MKL$(1273)+MKL$(1274)
    spritehandle& = hRoads&(3)
  CASE %SPRITE_PATH:  'Weg
    buildings$ = buildings$+MKL$(1264)+MKL$(1267)+MKL$(1268)+MKL$(1270)+MKL$(1271)+MKL$(1273)+MKL$(1274)
    spritehandle& = hRoads&(1)
  CASE %SPRITE_TRENCH:  'Graben
    buildings$ = MKL$(1266)+MKL$(1268)+MKL$(1269)+MKL$(1271)+MKL$(1272)+MKL$(1273)+MKL$(1274)
    spritehandle& = hRoads&(2)
  CASE %SPRITE_SNOWCOVERED_ROAD:  'verschneite Straße
    buildings$ = buildings$+MKL$(1264)+MKL$(1265)+MKL$(1266)+MKL$(1270)+MKL$(1271)+MKL$(1272)+MKL$(1274)+MKL$(90)
    spritehandle& = hRoads&(5)
  END SELECT
  d& = 1
  FOR i& = 0 TO 5
    IF xFieldsToSmooth&(i&) >= 0 THEN
      spr& = channels(0).zone2(xFieldsToSmooth&(i&), yFieldsToSmooth&(i&))
      IF spr& = sprnr& OR INSTR(buildings$, MKL$(spr&)) > 0 THEN m& = m& OR d&
    ELSE
      'am Kartenrand Straßen geradeaus weiterzeichnen
      k& = (i&+3) MOD 6
      IF xFieldsToSmooth&(k&) >= 0 THEN
        spr& = channels(0).zone2(xFieldsToSmooth&(k&), yFieldsToSmooth&(k&))
        IF spr& = sprnr& OR INSTR(buildings$, MKL$(spr&)) > 0 THEN m& = m& OR d&
      END IF
    END IF
    d& = d&+d&
  NEXT i&

  SmoothRoad& = m&
END FUNCTION



'Darstellungsbereich eines Sprites ermitteln
SUB GetSpriteDestRect(BYVAL mapx&, BYVAL mapy&, BYREF x0&, BYREF y0&, BYREF x1&, BYREF y1&)
  'Zoomfaktor und Kartenscroll-Position anwenden
  x0& = maparea.left+(mapx&*16)*zoom#-scrollX&
  y0& = maparea.top+(mapy&*24+(mapx& AND 1)*12)*zoom#-scrollY&
  x1& = maparea.left+(mapx&*16+24)*zoom#-scrollX&
  y1& = maparea.top+(mapy&*24+(mapx& AND 1)*12+24)*zoom#-scrollY&

  'prüfen, ob Zielbereich komplett außerhalb des Darstellungsbereichs liegt
  IF x1& < maparea.left OR x0& > maparea.right OR y1& < maparea.top OR y0& > maparea.bottom THEN x0& = -1
END SUB



'Sprite darstellen
SUB DrawSprite(spritehandle&, BYVAL sprnr&, mapx&, mapy&, opacity!)
  LOCAL x0&, y0&, x1&, y1&, srcx0&, srcy0&, srcx1&, srcy1&, colnr&, rownr&, animstep&, expand!

  'Zielbereich berechnen
  CALL GetSpriteDestRect(mapx&, mapy&, x0&, y0&, x1&, y1&)
  IF x0& = -1 THEN EXIT SUB

  'sichtbaren Quellbereich berechnen
  srcx0& = 0
  srcy0& = 0
  srcx1& = 24
  srcy1& = 24
  IF x0& < maparea.left THEN
    srcx0& = INT((maparea.left-x0&)/zoom#)
    x0& = maparea.left
  END IF
  IF x1& > maparea.right THEN
    srcx1& = 24-INT((x1&-maparea.right)/zoom#)
    x1& = maparea.right
  END IF
  IF y0& < maparea.top THEN
    srcy0& = INT((maparea.top-y0&)/zoom#)
    y0& = maparea.top
  END IF
  IF y1& > maparea.bottom THEN
    srcy1& = 24-INT((y1&-maparea.bottom)/zoom#)
    y1& = maparea.bottom
  END IF

  'Sprite in der Textur lokalisieren (Textur hat 40 Sprites pro Zeile)
  IF spritehandle& = hHudElements& THEN
    'Markierungen
    IF sprnr& < %MAPOVERLAY_REFUEL THEN
      srcx0& = srcx0&+sprnr&*24
      srcx1& = srcx1&+sprnr&*24
      srcy0& = srcy0&+718
      srcy1& = srcy1&+718
    ELSE
      animstep& = INT(gametime!*1000/overlayAnimationSpeed!) AND 3
      IF sprnr& = %MAPOVERLAY_BUILD THEN
        sprnr& = animstep&+8
        srcx0& = srcx0&+sprnr&*24
        srcx1& = srcx1&+sprnr&*24
        srcy0& = srcy0&+652
        srcy1& = srcy1&+652
      ELSE
        sprnr& = sprnr&-%MAPOVERLAY_REFUEL+animstep&
        srcx0& = srcx0&+sprnr&*24
        srcx1& = srcx1&+sprnr&*24
        srcy0& = srcy0&+676
        srcy1& = srcy1&+676
      END IF
    END IF
  ELSE
    rownr& = INT(sprnr&/40)
    colnr& = sprnr&-rownr&*40
    srcx0& = srcx0&+colnr&*24
    srcy0& = srcy0&+rownr&*24
    srcx1& = srcx1&+colnr&*24
    srcy1& = srcy1&+rownr&*24
  END IF

  expand! = IIF(gridMode& = 1, 0, 0.5+zoom#/4)
  IF sprnr& = %MAPOVERLAY_FOG THEN expand! = 0
  D2D.GraphicStretch(spritehandle&, srcx0&, srcy0&, srcx1&, srcy1&, x0&-expand!, y0&-expand!, x1&+expand!, y1&+expand!, opacity!)
END SUB



'Liefert das Quellrechteck für ein Map-Overlay
SUB GetMapOverlaySrcRect(BYVAL overlay&, BYVAL animstep&, BYREF srcx0&, BYREF srcy0&, BYREF srcx1&, BYREF srcy1&)
  IF overlay& = %MAPOVERLAY_BUILD THEN
    srcx0& = (animstep&+8)*24
    srcy0& = 652
  ELSE
    srcx0& = (overlay&-%MAPOVERLAY_REFUEL+animstep&)*24
    srcy0& = 676
  END IF
  srcx1& = srcx0&+24
  srcy1& = srcy0&+24
END SUB



'Karten-Cursor darstellen
SUB RenderCursor
  LOCAL unitnr&, x0!, y0!, x1!, y1!, cx!, cy!, angle!

  'prüfen, ob Cursor sichtbar ist
  IF cursorXPos& < 0 THEN EXIT SUB
  unitnr& = channels(0).player(localPlayerNr&).selectedunit
  IF unitnr& >= 0 AND channels(0).units(unitnr&).xpos = cursorXPos& AND channels(0).units(unitnr&).ypos = cursorYPos& THEN EXIT SUB

  'Zielbereich berechnen
  x0! = maparea.left+(cursorXPos&*16)*zoom#-scrollX&
  y0! = maparea.top+(cursorYPos&*24+(cursorXPos& AND 1)*12)*zoom#-scrollY&
  x1! = maparea.left+(cursorXPos&*16+24)*zoom#-scrollX&
  y1! = maparea.top+(cursorYPos&*24+(cursorXPos& AND 1)*12+24)*zoom#-scrollY&
  cx! = (x0!+x1!)/2
  cy! = (y0!+y1!)/2

  'sich drehender Kreis
  angle! = gametime!*90
  D2D.RotateOutput(angle!, cx!, cy!)
  D2D.GraphicStretch(hHudElements&, 384, localPlayerNr&*100, 484, localPlayerNr&*100+100, x0!, y0!, x1!, y1!)
  D2D.ResetMatrix
END SUB



'Ausgewählte Einheit markieren
SUB HighlightUnit(unitnr&)
  LOCAL i&, x0!, y0!, x1!, y1!, cx!, cy!, mapx&, mapy&, t!, t2!, angle!

  'Zielbereich berechnen
  mapx& = channels(0).units(unitnr&).xpos
  mapy& = channels(0).units(unitnr&).ypos
  x0! = maparea.left+(mapx&*16)*zoom#-scrollX&
  y0! = maparea.top+(mapy&*24+(mapx& AND 1)*12)*zoom#-scrollY&
  x1! = maparea.left+(mapx&*16+24)*zoom#-scrollX&
  y1! = maparea.top+(mapy&*24+(mapx& AND 1)*12+24)*zoom#-scrollY&
  cx! = (x0!+x1!)/2
  cy! = (y0!+y1!)/2

  t! = gametime!-unitSelectionTime!
  IF t! < 0.5 THEN
    'auf die Einheit zuschnellende Dreiecke
    t! = 0.5-t!
    x0! = cx!-3*zoom#
    x1! = cx!+3*zoom#
    y0! = cy!-(8-t!*400)*zoom#
    y1! = cy!-(2-t!*400)*zoom#
    FOR i& = 0 TO 3
      angle! = i&*90
      D2D.RotateOutput(angle!, cx!, cy!)
      D2D.GraphicStretch(hHudElements&, 484, localPlayerNr&*28, 512, localPlayerNr&*28+28, x0!, y0!, x1!, y1!)
    NEXT i&
    D2D.ResetMatrix
  ELSE
    'sich drehender Kreis
    angle! = t!*90
    D2D.RotateOutput(angle!, cx!, cy!)
    D2D.GraphicStretch(hHudElements&, 384, localPlayerNr&*100, 484, localPlayerNr&*100+100, x0!, y0!, x1!, y1!)
    'Dreiecke
    x0! = cx!-3*zoom#
    x1! = cx!+3*zoom#
    t2! = t! MOD 2
    IF t2! <= 1 THEN
      y0! = cy!-8*zoom#
      y1! = cy!-2*zoom#
    ELSE
      IF t2! <= 1.5 THEN t2! = t2!-1 ELSE t2! = 2.0-t2!
      y0! = cy!-(8-t2!*4)*zoom#
      y1! = cy!-(2-t2!*4)*zoom#
    END IF
    FOR i& = 0 TO 3
      angle! = i&*90-(t!+0.5)*3
      D2D.RotateOutput(angle!, cx!, cy!)
      D2D.GraphicStretch(hHudElements&, 484, localPlayerNr&*28, 512, localPlayerNr&*28+28, x0!, y0!, x1!, y1!)
    NEXT i&
    D2D.ResetMatrix
  END IF
END SUB



'Ausgewählte Einheit im Shop markieren
SUB HighlightShopUnit(BYVAL x0!, BYVAL y0!, BYVAL x1!, BYVAL y1!)
  LOCAL i&, angle!, cx!, cy!, t2!

  'sich drehender Kreis
  angle! = gametime!*90
  cx! = (x0!+x1!)/2
  cy! = (y0!+y1!)/2
  D2D.RotateOutput(angle!, cx!, cy!)
  D2D.GraphicStretch(hHudElements&, 384, localPlayerNr&*100, 484, localPlayerNr&*100+100, x0!, y0!, x1!, y1!)

  'Dreiecke
  x0! = cx!-9
  x1! = cx!+9
  t2! = gametime! MOD 2
  IF t2! <= 1 THEN
    y0! = cy!-24
    y1! = cy!-6
  ELSE
    IF t2! <= 1.5 THEN t2! = t2!-1 ELSE t2! = 2.0-t2!
    y0! = cy!-(8-t2!*4)*3
    y1! = cy!-(2-t2!*4)*3
  END IF
  FOR i& = 0 TO 3
    angle! = i&*90-(gametime!+0.5)*3
    D2D.RotateOutput(angle!, cx!, cy!)
    D2D.GraphicStretch(hHudElements&, 484, localPlayerNr&*28, 512, localPlayerNr&*28+28, x0!, y0!, x1!, y1!)
  NEXT i&
  D2D.ResetMatrix
END SUB



'Setzt den Standardwert für den Fortschrittsbalken
SUB UpdateProgressbar
  LOCAL a$$, plname$, plnr&, totalunits&, usedunits&

  IF gameMode& = %GAMEMODE_SERVER THEN EXIT SUB

  'Replay Wiedergabe
  IF replayMode&(0) >= %REPLAYMODE_PLAY AND exportingReplay& = 0 THEN
    plnr& = channels(0).info.activeplayer
    CALL GetUnitCount(0, plnr&, totalunits&, usedunits&)
    progressbar.HighlightColor = brushPlayer&(plnr&)
    progressbar.TextColor = IIF&(plnr& >= 4, brushBlack&, brushWhite&)
    progressbar.MaxProgress = totalunits&
    progressbar.CurrentProgress = usedunits&
    a$$ = words$$(IIF&(replayPause& = 0, %WORD_REPLAY_USERS_TURN, %WORD_REPLAY_PAUSE))
    REPLACE "%" WITH FORMAT$(channels(0).info.turn+1) IN a$$
    IF LEN(replayData$(0)) = 0 THEN
      REPLACE "&" WITH "0" IN a$$
    ELSE
      REPLACE "&" WITH FORMAT$((replayPosition&(0)+79)/80)+"/"+FORMAT$(LEN(replayData$(0))/80) IN a$$
    END IF
    REPLACE "$" WITH replayUsername$ IN a$$
    progressbar.Caption = a$$
    EXIT SUB
  END IF

  'normales Spiel
  IF LocalPlayersTurn& THEN
    CALL GetUnitCount(0, localPlayerNr&, totalunits&, usedunits&)
    progressbar.HighlightColor = brushPlayer&(localPlayerNr&)
    progressbar.TextColor = brushWhite&
    progressbar.MaxProgress = totalunits&
    progressbar.CurrentProgress = usedunits&
    a$$ = words$$(%WORD_YOUR_TURN)
    REPLACE "%" WITH FORMAT$(usedunits&)+"/"+FORMAT$(totalunits&) IN a$$
    progressbar.Caption = a$$
  ELSE
    progressbar.MaxProgress = 100
    progressbar.CurrentProgress = 100
    a$$ = words$$(%WORD_OTHER_PLAYERS_TURN)
    IF gameMode& = %GAMEMODE_SINGLE THEN
      plnr& = channels(0).info.activeplayer
    ELSE
      FOR plnr& = 0 TO %MAXPLAYERS-1
        IF (channels(0).info.aliveplayers AND 2^plnr&) <> 0 AND channels(0).player(plnr&).team = channels(0).info.activeteam THEN EXIT FOR
      NEXT plnr&
    END IF
    IF plnr& < %MAXPLAYERS THEN
      plname$ = playernames$(plnr&)
      IF plname$ = "" THEN plname$ = defaultPlayernames$(plnr&)
      REPLACE "%" WITH plname$ IN a$$
      progressbar.HighlightColor = brushPlayer&(plnr&)
      progressbar.TextColor = IIF&(plnr& >= 4, brushBlack&, brushWhite&)
    END IF
    progressbar.Caption = a$$
  END IF
END SUB



'Alle Steuerelemente ein/ausblenden
SUB ShowControls(vis&)
  buttonMapInfo.Visible = vis&
  buttonSaveGame.Visible = vis&
  buttonLoadGame.Visible = vis&
  buttonMusic.Visible = vis&
  buttonProtocol.Visible = vis&
  buttonHighscore.Visible = vis&
  buttonEndTurn.Visible = vis&
  buttonOpenMenu.Visible = vis&
  progressbar.Visible = vis&

  buttonSaveGame.Enabled = (startupaction& <> %STARTACTION_TESTMAP)
  buttonLoadGame.Enabled = (startupaction& <> %STARTACTION_TESTMAP)
  buttonHighscore.Enabled = (startupaction& <> %STARTACTION_TESTMAP)
END SUB



'Credits beenden
SUB EndCredits
  gameState& = %GAMESTATE_NONE
  CALL ShowControls(1)
  CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_RING2, %SOUNDBUFFER_EFFECT1, %PLAYFLAGS_NONE)
  CALL ShowMainMenu(%SUBMENU_MAIN, %PHASE_NONE, 0)
END SUB



'Zwischensequenz beenden
SUB EndCutScene
  'Sprachausgabe abbrechen
  CALL SAPISPEAK("")
  soundchannels(%SOUNDBUFFER_EFFECT2).Stop
  soundchannels(%SOUNDBUFFER_EFFECT3).Stop
  soundchannels(%SOUNDBUFFER_EFFECT4).Stop

  'Video-Datei schließen
  IF isVideoCutscene& = 1 THEN CALL CloseAVIFile
  CALL BIDebugLog("AVI cutscene file closed.")

  'Mission starten
  gameState& = %GAMESTATE_INGAME
  CALL ShowControls(1)
  CALL InitMap2(0, 0)
END SUB



'Zeigt den Fortschritt des KI Zugs
SUB ShowAIProgress(BYVAL a$$, plnr&, curprogress&, maxprogress&)
  IF gameMode& = %GAMEMODE_SERVER THEN EXIT SUB

  progressbar.HighlightColor = brushPlayer&(plnr&)
  progressbar.TextColor = IIF&(plnr& >= 4, brushBlack&, brushWhite&)
  progressbar.CAPTION = a$$+" (%)"
  progressbar.CurrentProgress = curprogress&
  progressbar.MaxProgress = maxprogress&
END SUB



'Liefert das Waffen-Icon für eine Einheit und Waffe
SUB GetWeaponIcon(BYVAL unitnr&, BYVAL weaponnr&, BYREF iconx&, BYREF icony&)
  LOCAL unittp&, tg&

  unittp& = channels(0).units(unitnr&).unittype
  tg& = channelsnosave(0).unitclasses(unittp&).weapons(weaponnr&).targets
  CALL GetWeaponIconByUnitclass(unittp&, tg&, iconx&, icony&)
END SUB



'Liefert das Waffen-Icon für eine Einheitenklasse und Waffenziel
SUB GetWeaponIconByUnitclass(BYVAL unittp&, BYVAL tg&, BYREF iconx&, BYREF icony&)
  CALL GetWeaponIconPos(unittp&, tg&, iconx&, icony&)
  iconx& = iconx&*17+1
  icony& = icony&*17+358
END SUB



'Zeigt den Initialisierungs-Fortschritt
SUB ShowInitProgress
  IF ISNOTHING(progressbar) THEN EXIT SUB

  progressbar.Caption = initProgressText$$
  progressbar.CurrentProgress = initProgress&
END SUB



'Statisches Zwischensequenz-Objekt erzeugen
SUB CreateStaticCutSceneObject(x!, y!, wd!, hg!, sprite!)
  CALL CreateCutSceneObject(x!, y!, wd!, hg!, 1.0, sprite!, 0, 0, 0, 0, 0, 0)
END SUB



'Bewegliches Zwischensequenz-Objekt erzeugen
SUB CreateCutSceneObject(x!, y!, wd!, hg!, opacity!, frame!, xspd!, yspd!, wdadd!, hgadd!, opacitygrowth!, animationspeed!)
  IF nCutSceneObjects& = %MAXCUTSCENEOBJECTS THEN EXIT SUB

  cutSceneObjects(nCutSceneObjects&).xpos = x!
  cutSceneObjects(nCutSceneObjects&).ypos = y!
  cutSceneObjects(nCutSceneObjects&).width = wd!
  cutSceneObjects(nCutSceneObjects&).height = hg!
  cutSceneObjects(nCutSceneObjects&).opacity = opacity!
  cutSceneObjects(nCutSceneObjects&).frame = frame!
  cutSceneObjects(nCutSceneObjects&).xspeed = xspd!
  cutSceneObjects(nCutSceneObjects&).yspeed = yspd!
  cutSceneObjects(nCutSceneObjects&).widthadd = wdadd!
  cutSceneObjects(nCutSceneObjects&).heightadd = IIF(hgadd! = -999, wdadd!, hgadd!)
  cutSceneObjects(nCutSceneObjects&).opacitygrowth = opacitygrowth!
  cutSceneObjects(nCutSceneObjects&).frameadd = animationspeed!
  nCutSceneObjects& = nCutSceneObjects&+1
END SUB



'Nicht mehr benötigte Zwischensequenz-Objekt entfernen
SUB CleanUpCutSceneObjects
  LOCAL i&

  FOR i& = nCutSceneObjects&-1 TO 0 STEP -1
    IF cutSceneObjects(i&).opacity <= 0 OR cutSceneObjects(i&).width <= 0 OR cutSceneObjects(i&).height <= 0 THEN
      ARRAY DELETE cutSceneObjects(i&) FOR nCutSceneObjects&-i&
      nCutSceneObjects& = nCutSceneObjects&-1
    END IF
  NEXT i&
END SUB



'Zwischensequenz animieren
SUB UpdateCutScene
  LOCAL t!, delta!, i&, x!, centerx!, helix!, jetx!

  'Objekte bewegen
  delta! = gametime!-lastCutSceneUpdateTime!
  centerx! = cutSceneScrollPos!+windowWidth&/2
  helix! = 999999
  jetx! = 999999
  FOR i& = 0 TO nCutSceneObjects&-1
    'Objekte aktualisieren
    cutSceneObjects(i&).xpos = cutSceneObjects(i&).xpos+cutSceneObjects(i&).xspeed*delta!
    cutSceneObjects(i&).ypos = cutSceneObjects(i&).ypos+cutSceneObjects(i&).yspeed*delta!
    cutSceneObjects(i&).width = cutSceneObjects(i&).width+cutSceneObjects(i&).widthadd*delta!
    cutSceneObjects(i&).height = cutSceneObjects(i&).height+cutSceneObjects(i&).heightadd*delta!
    cutSceneObjects(i&).opacity = MIN(1.0, cutSceneObjects(i&).opacity+cutSceneObjects(i&).opacitygrowth*delta!)
    IF cutSceneObjects(i&).frameadd > 0 AND cutSceneObjects(i&).frame >= 64 AND cutSceneObjects(i&).frame < 75 THEN
      cutSceneObjects(i&).frame = 64+((cutSceneObjects(i&).frame-64+cutSceneObjects(i&).frameadd*delta!) MOD 11)
    END IF
    IF cutSceneObjects(i&).frameadd > 0 AND cutSceneObjects(i&).frame >= 96 AND cutSceneObjects(i&).frame < 99 THEN
      cutSceneObjects(i&).frame = 96+((cutSceneObjects(i&).frame-96+cutSceneObjects(i&).frameadd*delta!) MOD 3)
    END IF
    IF cutSceneObjects(i&).frameadd > 0 AND cutSceneObjects(i&).frame >= 100 AND cutSceneObjects(i&).frame < 103 THEN
      cutSceneObjects(i&).frame = 100+((cutSceneObjects(i&).frame-100+cutSceneObjects(i&).frameadd*delta!) MOD 3)
    END IF
    IF cutSceneObjects(i&).frameadd > 0 AND cutSceneObjects(i&).frame >= 104 AND cutSceneObjects(i&).frame < 107 THEN
      cutSceneObjects(i&).frame = 104+((cutSceneObjects(i&).frame-104+cutSceneObjects(i&).frameadd*delta!) MOD 3)
    END IF
    'Abstand aller Jets und Helikopter zum Zentrum der Szene berechnen
    IF (cutSceneObjects(i&).frame = 10 OR cutSceneObjects(i&).frame = 11) AND (cutSceneObjects(i&).xspeed <> 0 OR cutSceneObjects(i&).yspeed <> 0) THEN jetx! = MIN(jetx!, ABS(cutSceneObjects(i&).xpos-centerx!))
    IF cutSceneObjects(i&).frame >= 96 AND cutSceneObjects(i&).frame < 108 AND cutSceneObjects(i&).frameadd > 0 THEN helix! = MIN(helix!, ABS(cutSceneObjects(i&).xpos-centerx!))
  NEXT i&
  lastCutSceneUpdateTime! = gametime!

  'Soundeffekte erzeugen oder anpassen
  IF jetx! < 1500 THEN
    IF soundchannels(%SOUNDBUFFER_EFFECT2).IsPlaying = 0 THEN CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_JET_FLYING_LOUD, %SOUNDBUFFER_EFFECT2, %PLAYFLAGS_LOOPING)
    soundchannels(%SOUNDBUFFER_EFFECT2).SetVolume((1-jetx!/1500)*effectVolume&/100, (1-jetx!/1500)*effectVolume&/100)
  ELSE
    IF soundchannels(%SOUNDBUFFER_EFFECT2).IsPlaying <> 0 THEN soundchannels(%SOUNDBUFFER_EFFECT2).Stop
  END IF
  IF helix! < 1500 THEN
    IF soundchannels(%SOUNDBUFFER_EFFECT3).IsPlaying = 0 THEN CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_HELICOPTER, %SOUNDBUFFER_EFFECT3, %PLAYFLAGS_LOOPING)
    soundchannels(%SOUNDBUFFER_EFFECT3).SetVolume((1-helix!/1500)*effectVolume&/100, (1-helix!/1500)*effectVolume&/100)
  ELSE
    IF soundchannels(%SOUNDBUFFER_EFFECT3).IsPlaying <> 0 THEN soundchannels(%SOUNDBUFFER_EFFECT3).Stop
  END IF

  'neue Objekte erzeugen
  t! = gametime!-cutSceneStartTime!
  SELECT CASE cutSceneNumber&
  CASE 1:  'belagerte Stadt
    IF nCutSceneObjects& = 0 THEN
      'Vordergrund-Objekte erzeugen
      CALL CreateStaticCutSceneObject(2583, 102, 130, 172, 4)
      CALL CreateStaticCutSceneObject(2816, 225, 65, 108, 5)
      CALL CreateStaticCutSceneObject(3280.5, 449, 43, 88, 6)
    END IF
    IF t! > lastCutSceneObjCreationTime!+0.3 THEN
      'Rauch erzeugen
      'CreateCutSceneObject(x!, y!, wd!, hg!, opacity!, frame!, xspd!, yspd!, wdadd!, hgadd!, opacitygrowth!, animationspeed!)
      CALL CreateCutSceneObject(2542, 132, 20, 20, 0.5, RND(0, 3), RND(-3, -7), RND(-6, -7), RND(1, 2), -999, -0.025, 0)
      CALL CreateCutSceneObject(2794, 272, 18, 18, 0.5, RND(0, 3), RND(-1, -3), RND(-6, -7), RND(1, 2), -999, -0.015, 0)
      CALL CreateCutSceneObject(3287, 450, 16, 16, 0.5, RND(0, 3), RND(3, 10), RND(-6, -7), RND(1, 2), -999, -0.01, 0)
      lastCutSceneObjCreationTime! = t!
    END IF

  CASE 2:  'Gebirgsbahn
    IF nCutSceneObjects& = 0 THEN
      'Vordergrund-Objekte erzeugen
      CALL CreateStaticCutSceneObject(1200, 587, 2402, 192, 7)
      CALL CreateStaticCutSceneObject(3643, 589, 717, 189, 8)
      'Zug erzeugen
      CALL CreateCutSceneObject(4900, 593, 1083, 178, 1.0, 9, -40, 0, 0, 0, 0, 0)
    END IF

  CASE 3:  'Flugzeugträger im Eis
    IF nCutSceneObjects& = 0 THEN
      'Radarschüssel
      CALL CreateCutSceneObject(3149, 183, 50, 50, 1.0, 64, 0, 0, 0, 0, 0, 6)
      'Flugzeug auf Träger
      CALL CreateStaticCutSceneObject(2940, 256, 53, 25, 11)
    END IF
    IF nCutSceneObjects& = 2 AND t! > 32 THEN
      'Flugzeug
      CALL CreateCutSceneObject(1700, 150, 2.7, 1, 1.0, 10, 550, 20, 54, 20, 0, 0)
      CALL CreateCutSceneObject(1800, 150, 2.7, 1, 1.0, 10, 650, 40, 54, 20, 0, 0)
    END IF
    IF t! > 35 THEN
      cutSceneObjects(1).xspeed = 450
      cutSceneObjects(1).yspeed= 20
      cutSceneObjects(1).widthadd = 53
      cutSceneObjects(1).heightadd = 25
    END IF

  CASE 4:  'Dschungel
    IF nCutSceneObjects& = 0 AND t! > 20 THEN
      'Helikopter
      CALL CreateCutSceneObject(3600, 136, 68.9, 36.7, 1.0, 96, -10, 0.5, 11.483333, 6.116666, 0, 24)
      CALL CreateCutSceneObject(3800, 136, 24.1, 13.5, 1.0, 100, -8, 1.0, 6.025, 3.375, 0, 24)
      CALL CreateCutSceneObject(4500, 300, 178, 87.5, 1.0, 104, -11, 0.0, 3.56, 1.75, 0, 24)
    END IF

  END SELECT

  'alte Objekte entfernen, um Array zu optimieren
  CALL CleanUpCutSceneObjects
END SUB



'Highlight erzeugen
SUB CreateIntroHighlight
  introHighlightX! = IIF(RND(0, 1) = 0, -295, 310)
  introHighlightSize! = 21
  introHighlightMaxSize! = RND(60, 80)
  introLightDirection& = 1
END SUB



'Intro-Objekte bewegen
SUB UpdateIntroObjects()
  LOCAL i&, t!

  t! = gametime!-lasttimeupdate!

  FOR i& = 0 TO %MAXINTROCLOUDS-1
    introClouds(i&).opacity = MIN(1, introClouds(i&).opacity+t/5)
    IF introClouds(i&).opacity > 0.25 THEN
      introClouds(i&).size = introClouds(i&).size+t!*10
      introClouds(i&).xpos = introClouds(i&).xpos+COS(introClouds(i&).rotation!)*t!*introClouds(i&).speed*uiscale!
      introClouds(i&).ypos = introClouds(i&).ypos+SIN(introClouds(i&).rotation!)*t!*introClouds(i&).speed*uiscale!
    END IF
  NEXT i&
END SUB



'Intro abspielen
SUB RenderIntro
  LOCAL i&, srcx&, srcy&, visiblenumbers&
  LOCAL t!, cloudsize!, centerX!, centerY!, biWidth!, biHeight!, emWidth!, emHeight!, zoomTime!, zoom2020!, startTime2020!, destx!, desty!

  t! = gametime!-introStartTime!
  zoomTime! = MIN(t!, 60)
  CALL UpdateIntroObjects

  D2D.CreateClippingRegion(maparea.left, maparea.top, maparea.right, maparea.bottom)
  D2D.GraphicBox(maparea.left, maparea.top, maparea.right, maparea.bottom, brushWhite&, brushWhite&)

  FOR i& = %MAXINTROCLOUDS-1 TO 0 STEP -1
    'Wolken
    IF introClouds(i&).opacity > 0 THEN
      srcx& = introClouds(i&).color AND 3
      srcy& = INT(introClouds(i&).color/4)
      cloudsize! = introClouds(i&).size
      D2D.GraphicStretch(hIntro&, srcx&*%INTROCLOUDTEXTURESIZE, srcy&*%INTROCLOUDTEXTURESIZE, srcx&*%INTROCLOUDTEXTURESIZE+%INTROCLOUDTEXTURESIZE, srcy&*%INTROCLOUDTEXTURESIZE+%INTROCLOUDTEXTURESIZE, _
                        introClouds(i&).xpos-cloudsize!/2, introClouds(i&).ypos-cloudsize!/2, introClouds(i&).xpos+cloudsize!/2, introClouds(i&).ypos+cloudsize!/2, introClouds(i&).opacity)
    END IF

    'Battle Isle
    IF i& = 32 THEN
      biWidth! = (txarea_introbattleisle.right-txarea_introbattleisle.left-140+zoomTime!*7)*uiscale!
      biHeight! = (txarea_introbattleisle.bottom-txarea_introbattleisle.top-20+zoomTime!)*uiscale!
      centerX! = (maparea.left+maparea.right)/2
      centerY! = (maparea.top+maparea.bottom)/2-biHeight!/2-30*uiscale!
      D2D.GraphicStretch(hIntro&, txarea_introbattleisle.left, txarea_introbattleisle.top, txarea_introbattleisle.right, txarea_introbattleisle.bottom, _
                        centerX!-biWidth!/2, centerY!-biHeight!/2, centerX!+biWidth!/2, centerY!+biHeight!/2, MIN(1.0, t!/20))

      'Highlight auf Schrift
      IF t! >= 12.0 THEN
        IF introHighlightSize! < 20.0 AND RND(0, 39) = 1 THEN CALL CreateIntroHighlight
        IF introHighlightSize! >= 20.0 THEN
          introHighlightSize! = introHighlightSize!+introLightDirection&
          IF introHighlightSize! >= introHighlightMaxSize! THEN introLightDirection& = -1
          centerX! = (maparea.left+maparea.right)/2+introHighlightX!*biWidth!/(txarea_introbattleisle.right-txarea_introbattleisle.left)
          centerY! = (maparea.top+maparea.bottom)/2-biHeight!-15*uiscale!
          D2D.RotateOutput(t!*60, centerx!, centerY!)
          D2D.GraphicStretch(hIntro&, txarea_introhighlight.left, txarea_introhighlight.top, txarea_introhighlight.right, txarea_introhighlight.bottom, _
                           centerX!-introHighlightSize!/2*uiscale!, centerY!-introHighlightSize!/2*uiscale!, centerX!+introHighlightSize!/2*uiscale!, centerY!+introHighlightSize!/2*uiscale!, 1.0)
          D2D.ResetMatrix
        END IF
      END IF
    END IF

    'Emblem
    IF i& = 38 THEN
      emWidth! = (txarea_introemblem.right-txarea_introemblem.left-140+zoomTime!*7)*uiscale!
      emHeight! = (txarea_introemblem.bottom-txarea_introemblem.top-50+zoomTime!*3)*uiscale!
      centerX! = (maparea.left+maparea.right)/2
      centerY! = (maparea.top+maparea.bottom)/2+emHeight!/2-30*uiscale!
      D2D.GraphicStretch(hIntro&, txarea_introemblem.left, txarea_introemblem.top, txarea_introemblem.right, txarea_introemblem.bottom, _
                        centerX!-emWidth!/2, centerY!-emHeight!/2, centerX!+emWidth!/2, centerY!+emHeight!/2, MIN(1.0, (t!-3)/20))
    END IF
  NEXT i&

  '2020
  startTime2020! = 12.0
  centerX! = (maparea.left+maparea.right)/2
  desty! = (maparea.top+maparea.bottom)/2+70*uiscale!  'MIN((maparea.bottom*7+maparea.top*3)/10, MAX(centery!, centery!+(t!-9.0)*40))
  visiblenumbers& = MIN&(4, INT(t!-startTime2020!+1))
  FOR i& = 0 TO visiblenumbers&-1
    zoom2020! = 0.0
    IF t! >= i&+startTime2020! AND t! < i&+startTime2020!+1 THEN zoom2020! = startTime2020!*8+8-(t!-i&)*8
    destx! = centerx!-visiblenumbers&*55+i&*110+10
    D2D.GraphicStretch(hIntro&, txarea_intro2020.left+i&*102, txarea_intro2020.top, txarea_intro2020.right+i&*102, txarea_intro2020.bottom, _
                      destx!-zoom2020!*400, desty!+10-zoom2020!*400, destx!+98+zoom2020!*400, desty!+140+zoom2020!*400)
  NEXT i&

  D2D.ReleaseClippingRegion

  'Sound-Effekte und Musik
  IF t! > 1.0 AND introSoundEffect& = 0 THEN
    CALL StartMusic(2)
    introSoundEffect& = 1
  END IF
  IF startTime2020!-t! < 0.4 AND introSoundEffect& = 1 THEN
    CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_INTRO_MUSIC, %SOUNDBUFFER_EFFECT4, %PLAYFLAGS_NONE)
    introSoundEffect& = 2
  END IF
  IF startTime2020!-t! < -1.0 AND introSoundEffect& = 2 THEN
    CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_INTRO_BEAT, %SOUNDBUFFER_EFFECT4, %PLAYFLAGS_NONE)
    introSoundEffect& = 3
  END IF
  IF startTime2020!-t! < -2.1 AND introSoundEffect& = 3 THEN
    CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_INTRO_BEAT, %SOUNDBUFFER_EFFECT4, %PLAYFLAGS_NONE)
    introSoundEffect& = 4
  END IF
  IF startTime2020!-t! < -3.0 AND introSoundEffect& = 4 THEN
    CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_INTRO_BEAT, %SOUNDBUFFER_EFFECT4, %PLAYFLAGS_NONE)
    introSoundEffect& = 5
  END IF
  IF startTime2020!-t! < -4.2 AND introSoundEffect& = 5 THEN
    CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_INTRO_BEAT, %SOUNDBUFFER_EFFECT4, %PLAYFLAGS_NONE)
    introSoundEffect& = 6
  END IF

'D2D.GraphicPrint("Time: "+FORMAT$(t!, "0.000"), 10, 0, brushWhite&, hLobbyCaptionFont&)

  'Hauptmenü öffnen
  IF t! >= 45.0 AND menuOpenTime! = 0 THEN
    CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_RING2, %SOUNDBUFFER_EFFECT1, %PLAYFLAGS_NONE)
    CALL ShowMainMenu(%SUBMENU_MAIN, %PHASE_NONE, 0)
  END IF
END SUB



'Animiniert die Bewegung der aktiven Einheit
SUB RenderUnitMoving
  LOCAL startx&, starty&, endx&, endy&, p&, n&, d&, x&, y&, animstep&, opacity!
  LOCAL plnr&, unitnr&, unittp&, sprnr&, spritehandle&, oldscrollX&, oldscrollY&

  CALL EnterSemaphore(semaphore_unitmoving&)

  'sich bewegende Einheiten aller Spieler suchen
  FOR plnr& = 0 TO %MAXPLAYERS-1
    n& = channels(0).player(plnr&).unitpathlen
    unitnr& = channels(0).player(plnr&).selectedunit
    IF n& > 0 AND unitnr& >= 0 THEN
      'ausgewählte Einheit dieses Spielers ermitteln
      p& = channels(0).player(plnr&).unitpathpos
      sprnr& = channels(0).units(unitnr&).unittype*6+channels(0).units(unitnr&).direction
      spritehandle& = hUnits&(plnr&)
      unittp& = channels(0).units(unitnr&).unittype

      'Bewegungsrichtung ermitteln
      startx& = channels(0).player(plnr&).unitpathx(p&)
      starty& = channels(0).player(plnr&).unitpathy(p&)
      endx& = channels(0).player(plnr&).unitpathx(p&+1)
      endy& = channels(0).player(plnr&).unitpathy(p&+1)
      d& = GetDirectionToField&(startx&, starty&, endx&, endy&)
      IF (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_NOTRAILS) = 0 THEN
        trails?(startx&, starty&) = trails?(startx&, starty&) OR 2^d&
        trails?(endx&, endy&) = trails?(endx&, endy&) OR 2^((d&+3) MOD 6)
      END IF
      channels(0).units(unitnr&).direction = d&

      'prüfen, ob sich Einheit auf einem aufgedeckten Feld befindet
      IF (channels(0).vision(startx&, starty&) AND localPlayerMask&) = 0 THEN
        channels(0).player(plnr&).unitanimstep = %FRAMES_PER_UNIT_MOVE_ANIMATION-1
      ELSE
        'Einheit darstellen
        opacity! = IIF&((channels(0).units(unitnr&).flags AND %US_DIVE) <> 0, 0.5, 1.0)
        animstep& = channels(0).player(plnr&).unitanimstep
        x& = ASC($fielddistance, d&*2+1)-24
        y& = ASC($fielddistance, d&*2+2)-24
        oldscrollX& = scrollX&
        oldscrollY& = scrollY&
        scrollX& = scrollX&-zoom#*x&*animstep&/%FRAMES_PER_UNIT_MOVE_ANIMATION
        scrollY& = scrollY&-zoom#*y&*animstep&/%FRAMES_PER_UNIT_MOVE_ANIMATION
        CALL DrawSprite(spritehandle&, sprnr&, startx&, starty&, opacity!)
        scrollX& = oldscrollX&
        scrollY& = oldscrollY&
        'zur Einheit scrollen, falls lokaler Spieler nicht am Zug ist
        IF NOT LocalPlayersTurn& THEN CALL ScrollToMapPos(channels(0).units(unitnr&).xpos, channels(0).units(unitnr&).ypos, 0.5)
      END IF

      'nächster Animationsschritt
      channels(0).player(plnr&).unitanimstep = channels(0).player(plnr&).unitanimstep+1
      IF channels(0).player(plnr&).unitanimstep = %FRAMES_PER_UNIT_MOVE_ANIMATION THEN
        channels(0).player(plnr&).unitanimstep = 0
        channels(0).player(plnr&).unitpathpos = p&+1
        IF p&+2 = n& THEN
          CALL EndMovement(0, unitnr&, endx&, endy&)
        END IF
      END IF
    END IF
  NEXT plnr&

  CALL LeaveSemaphore(semaphore_unitmoving&)
END SUB



'Ermittelt die Spalte und Zeile eines Waffen-Icon im HUD-Elemente-Bitmap
SUB GetWeaponIconPos(BYVAL unittp&, BYVAL tg&, BYREF iconcol&, BYREF iconrow&)
  'Zeile anhand des Einheitentyps bestimmen
  IF (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_PLANE) <> 0 THEN
    iconrow& = 0  'Luft
  ELSE
    IF (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_SHIP) <> 0 AND (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_DIVE) <> 0 THEN
      iconrow& = 2  'U-Boot
    ELSE
      iconrow& = 1  'Landeinheiten und Schiffe
    END IF
  END IF

  'Spalte anhand der Waffenziele bestimmen
  IF (tg& AND %WP_AMMO) <> 0 THEN
    iconcol& = 10  'Munition
  ELSE
    IF (tg& AND %WP_FUEL) <> 0 THEN
      iconcol& = 11  'Treibstoff
    ELSE
      IF (tg& AND %WP_MATERIAL) <> 0 THEN
        iconcol& = 12  'Baumaterial
      ELSE
        IF (tg& AND %WP_HIGHAIR) <> 0 THEN
          iconcol& = 0  'hochfliegende Flugzeuge
        ELSE
          IF (tg& AND %WP_UNDERWATER) <> 0 THEN
            iconcol& = 8  'getauchte U-Boote
          ELSE
            IF tg& = %WP_STRUCTURE THEN
              iconcol& = 9  'nur Strukturen
            ELSE
              iconcol& = (INT(tg&/2) AND 7)
            END IF
          END IF
        END IF
      END IF
    END IF
  END IF
END SUB



'Ermittelt, wie ein Waffen-Icon im HUD-Elements-Bitmap ausgerichtet ist
FUNCTION GetWeaponIconRotation&(unittp&, tg&)
  LOCAL a&, iconcol&, iconrow&

  CALL GetWeaponIconPos(BYVAL unittp&, BYVAL tg&, BYREF iconcol&, BYREF iconrow&)
  SELECT CASE iconcol&+iconrow&*10
  CASE 0,1,3,5,7,8 , 12,13,14,15,16,17,18 , 22,23,25,26,27,28 : a& = -90
  CASE 2,4,6 : a& = -135
  CASE 9 : a& = -180
  CASE 10,11 , 19 , 29 : a& = -45
  CASE 20,21 : a& = 0
  CASE 24: a& = -67
  END SELECT

  GetWeaponIconRotation& = a&
END FUNCTION



'Geschosse und Raketen darstellen
SUB RenderMissiles
  LOCAL i&, p&, n&, x0&, x1&, y0&, y1&, iconx&, icony&, mapx&, mapy&
  LOCAL startx!, starty!, endx!, endy!, currentx!, currenty!, angle!, explosionsize!

  FOR i& = 0 TO %MAXMISSILES-1
    p& = missiles(i&).position
    IF p& > 0 THEN
      'Position anhand des Animationsfortschritts zwischen Start- und Zielposition interpolieren
      CALL GetSpriteDestRect(missiles(i&).startx, missiles(i&).starty, x0&, y0&, x1&, y1&)
      startx! = (x0&+x1&)/2
      starty! = (y0&+y1&)/2
      CALL GetSpriteDestRect(missiles(i&).endx, missiles(i&).endy, x0&, y0&, x1&, y1&)
      endx! = (x0&+x1&)/2
      endy! = (y0&+y1&)/2

      IF p& > 16 THEN
        'Rakete auf Ziel ausgerichtet darstellen
        n& = missiles(i&).animationlength-16
        currentx! = endx!+(startx!-endx!)*(p&-16)/n&
        currenty! = endy!+(starty!-endy!)*(p&-16)/n&
        CALL GetMapPos(currentx!, currenty!, mapx&, mapy&)
        IF mapx& >= 0 AND (channels(0).vision(mapx&, mapy&) AND localPlayerMask&) <> 0 THEN
          angle! = GetAngle&(startx!, starty!, endx!, endy!)+GetWeaponIconRotation&(missiles(i&).ownertype, missiles(i&).weapontargets)
          CALL GetWeaponIconByUnitclass(missiles(i&).ownertype, missiles(i&).weapontargets, iconx&, icony&)
          D2D.RotateOutput(angle!, currentx!, currenty!)
          D2D.GraphicStretch(hHudElements&, iconx&, icony&, iconx&+16, icony&+16, currentx!-6*zoom#, currenty!-6*zoom#, currentx!+6*zoom#, currenty!+6*zoom#)
          D2D.ResetMatrix
        END IF
      ELSE
        'Explosion darstellen
        CALL GetMapPos(endx!, endy!, mapx&, mapy&)
        IF missiles(i&).damage > 0 AND mapx& >= 0 AND (channels(0).vision(mapx&, mapy&) AND localPlayerMask&) <> 0 THEN
          n& = 16-p&
          iconx& = n& AND 3
          icony& = INT(n&/4)
          explosionsize! = zoom#*(missiles(i&).damage+2)
          D2D.GraphicStretch(hHudElements&, iconx&*32, icony&*32+409, iconx&*32+32, icony&*32+441, endx!-explosionsize!, endy!-explosionsize!, endx!+explosionsize!, endy!+explosionsize!)
        END IF
      END IF

      missiles(i&).position = p&-1
    END IF
  NEXT i&
END SUB



'Karte darstellen
SUB RenderMap
  LOCAL i&, j&, k&, owner&, unitnr&, selectedunit&, sprnr&, roadspr&, spritehandle&, weather&, animstep&, tg&, x0&, y0&, x1&, y1&
  LOCAL xstart&, xend&, ystart&, yend&, explored&, opacity!
  LOCAL terraintype&
  LOCAL drawnUnits&(), nDrawnUnits&
  DIM drawnUnits&(%MAXUNITS-1)

  IF channels(0).info.xsize = 0 THEN EXIT SUB
  D2D.CreateClippingRegion(maparea.left, maparea.top, maparea.right, maparea.bottom)
  D2D.GraphicBox(maparea.left, maparea.top, maparea.right, maparea.bottom, brushUnexplored&, brushUnexplored&)
'localPlayerMask& = 2

  'darzustellenden Kartenausschnitt ermitteln
  xstart& = MAX&(0, INT(scrollX&/16/zoom#)-1)
  xend& = MIN&(channels(0).info.xsize-1, xstart&+(maparea.right-maparea.left)/16/zoom#+1)
  ystart& = MAX&(0, INT(scrollY&/24/zoom#)-1)
  yend& = MIN&(channels(0).info.ysize-1, ystart&+(maparea.bottom-maparea.top)/24/zoom#+1)

  'Kartenelemente rendern
  unitnr& = -1
  weather& = channels(0).info.weather
  FOR k& = 0 TO 2
    IF mapDrawOptions& = 1 AND k& > 0 THEN EXIT FOR
    IF mapDrawOptions& = 2 AND k& > 1 THEN EXIT FOR

    IF k& = 2 AND GetPhase&(0, localPlayerNr&) <> %PHASE_UNITMOVING AND updateMiniMap& = 0 THEN
      'ausgewählte Einheit markieren
      IF channels(0).player(localPlayerNr&).selectedunit >= 0 THEN CALL HighlightUnit(channels(0).player(localPlayerNr&).selectedunit)
      'Cursor darstellen
      CALL RenderCursor
    END IF

    'Kartenelemente darstellen
    FOR j& = ystart& TO yend&
      FOR i& = xstart& TO xend&
        explored& = k& = 0 OR (channels(0).explored(i&, j&) AND localPlayerMask&) <> 0 OR mapDrawOptions& = 2 OR debugNoFog& <> 0
        opacity! = 1

        'Sprite an dieser Stelle auslesen
        SELECT CASE k&
        CASE 0:
          sprnr& = channels(0).zone1(i&, j&)
          spritehandle& = hTerrain&
        CASE 1:
          sprnr& = channels(0).zone2(i&, j&)
          spritehandle& = hTerrain&
        CASE 2:
          unitnr& = -1
          sprnr& = channels(0).zone3(i&, j&)
          IF sprnr& >= 0 AND sprnr& < channels(0).info.nunits THEN
            'Einheit
            unitnr& = sprnr&
            owner& = channels(0).units(sprnr&).owner
            sprnr& = channels(0).units(sprnr&).unittype*6+channels(0).units(sprnr&).direction
            spritehandle& = hUnits&(owner&)
            IF (channels(0).units(unitnr&).flags AND %US_DIVE) <> 0 THEN
              opacity! = 0.5
              IF IsEnemyUnit&(0, localPlayerNr&, unitnr&) <> 0 AND IsAdjacentToUnitOfPlayer&(0, unitnr&, localPlayerNr&) = 0 THEN
                'getauchte feindliche Einheit ist nur sichtbar, wenn sich diese direkt neben einer eigenen Einheit befindet
                unitnr& = -1
                sprnr& = -1
              END IF
            END IF
          ELSE
            IF sprnr& >= -1-channels(0).info.nshops AND sprnr& < -1 THEN
              'Shop
              sprnr& = -2-sprnr&
              owner& = channels(0).shops(sprnr&).owner
              sprnr& = channels(0).shops(sprnr&).sprite
              IF sprnr& < 0 THEN
                'versteckter Shop
                spritehandle& = 0
              ELSE
                sprnr& = sprnr&+owner&*LEN($BUILDINGSPRITES)/2
                spritehandle& = hShops&
              END IF
            ELSE
              sprnr& = -1
              spritehandle& = 0
            END IF
          END IF
        END SELECT

        'Animationen
        IF k& < 2 AND sprnr& >= 0 AND sprnr& < channelsnosave(0).nterrain AND channelsnosave(0).terraindef(sprnr&).animationlength > 0 THEN
          animstep& = INT(gametime!*1000/terrainAnimationSpeed!)
          animstep& = animstep& MOD channelsnosave(0).terraindef(sprnr&).animationlength
          sprnr& = channelsnosave(0).terraindef(sprnr&).animationstart+animstep&
        END IF

        'Straßen/Wege/Gräben/Schienen weichzeichnen
        IF explored& <> 0 AND k& = 1 THEN
          IF (sprnr& >= %SPRITE_ROAD AND sprnr& <= %SPRITE_TRENCH) OR sprnr& = %SPRITE_SNOWCOVERED_ROAD THEN sprnr& = SmoothRoad&(sprnr&, i&, j&, spritehandle&)
          IF sprnr& >= 1264 AND sprnr& <= 1274 THEN
            SELECT CASE sprnr&
            CASE %SPRITE_ROAD_TRENCH, 1268, 1269, 1271, 1272, 1273, 1274:
              roadspr& = SmoothRoad&(%SPRITE_TRENCH, i&, j&, spritehandle&)
              CALL DrawSprite(spritehandle&, roadspr&, i&, j&, 1)  'Graben ganz nach unten
            END SELECT
            SELECT CASE sprnr&
            CASE 1264, 1267, 1268, 1270, 1271, 1273, 1274:
              roadspr& = SmoothRoad&(92, i&, j&, spritehandle&)
              CALL DrawSprite(spritehandle&, roadspr&, i&, j&, 1)  'Weg über Graben zeichnen
            END SELECT
            SELECT CASE sprnr&
            CASE 1264, 1265, %SPRITE_ROAD_TRENCH, 1270, 1271, 1272, 1274:
              roadspr& = SmoothRoad&(IIF&(channels(0).info.weather = %WEATHER_HEAVYSNOW, %SPRITE_SNOWCOVERED_ROAD, %SPRITE_ROAD), i&, j&, spritehandle&)
              CALL DrawSprite(spritehandle&, roadspr&, i&, j&, 1)  'Straße über Weg zeichnen
            END SELECT
            SELECT CASE sprnr&
            CASE 1265, 1267, 1269, 1270, 1272, 1273, 1274:
              roadspr& = SmoothRoad&(91, i&, j&, spritehandle&)
              CALL DrawSprite(spritehandle&, roadspr&, i&, j&, 1)  'Straße über Weg zeichnen
            END SELECT
            sprnr& = -1
          END IF
        END IF

        'Sichtbarkeit prüfen
        IF explored& = 0 THEN
          'Feld ist noch nicht erforscht
          IF k& = 2 THEN CALL DrawSprite(hHudElements&, %MAPOVERLAY_NOTEXPLORED, i&, j&, 1)
        ELSE
          IF k& > 1 AND (channels(0).vision(i&, j&) AND localPlayerMask&) = 0 AND mapDrawOptions& <> 2 AND debugNoFog& = 0 THEN
            'Feld ist zur Zeit nicht überwacht
            IF sprnr& >= 0 AND spritehandle& = hShops& THEN CALL DrawSprite(spritehandle&, sprnr&, i&, j&, 1)  'Shop in Besitzerfarbe darstellen
            CALL DrawSprite(hHudElements&, %MAPOVERLAY_FOG, i&, j&, 1)
          ELSE
            'Feld ist einsehbar
            IF sprnr& >= 0 OR k& = 0 OR spritehandle& = hShops& THEN CALL DrawSprite(spritehandle&, sprnr&, i&, j&, opacity!)
            IF k& = 1 AND trails?(i&, j&) > 0 THEN
              'Reifenspuren
              CALL DrawSprite(hRoads(4), trails?(i&, j&), i&, j&, 1)
            END IF
            IF k& = 2 AND unitnr& >= 0 THEN
              'bereits bewegte Einheiten ausgrauen
              IF (channels(0).units(unitnr&).flags AND %US_DONE) <> 0 THEN CALL DrawSprite(hUnits&(LEN(playerColors$)), sprnr&, i&, j&, opacity!)
              'merken, welche Einheiten sichtbar sind
              CALL GetSpriteDestRect(i&, j&, x0&, y0&, x1&, y1&)
              IF x0& >= 0 THEN
                drawnUnits&(nDrawnUnits&) = unitnr&
                nDrawnUnits& = nDrawnUnits&+1
              END IF
            END IF
          END IF
        END IF

        'Zielfeld-Markierung zeichnen
        tg& = channels(0).player(localPlayerNr&).targets(i&, j&)
        IF k& = 2 AND tg& <> 0 THEN
          sprnr& = %MAPOVERLAY_MOVE
          IF (tg& AND %TG_ATTACK) <> 0 THEN sprnr& = %MAPOVERLAY_ATTACK
          IF (tg& AND %TG_REFUEL) <> 0 THEN sprnr& = %MAPOVERLAY_REFUEL
          IF (tg& AND %TG_RECHARGE) <> 0 THEN sprnr& = %MAPOVERLAY_RECHARGE
          IF (tg& AND %TG_REPAIR) <> 0 THEN sprnr& = %MAPOVERLAY_REPAIR
          IF (tg& AND %TG_BUILD) <> 0 THEN sprnr& = %MAPOVERLAY_BUILD
          CALL DrawSprite(hHudElements&, sprnr&, i&, j&, 1)
        END IF

        'Verteidigungsbonus durch Gelände
        IF k& = 2 AND defenseInfo& = 1 AND (channels(0).explored(i&, j&) AND localPlayerMask&) <> 0 AND (channels(0).vision(i&, j&) AND localPlayerMask&) <> 0 THEN
          CALL RenderTerrainInfo(i&, j&)
        END IF

        'Debug Informationen
        IF k& = 2 AND debugShowUnits& <> 0 AND updateMiniMap& = 0 THEN
          sprnr& = channels(0).zone3(i&, j&)
          IF sprnr& >= 0 THEN D2D.GraphicPrint(FORMAT$(sprnr&), maparea.left+(i&*16)*zoom#-scrollX&+12, maparea.top+(j&*24+(i& AND 1)*12)*zoom#-scrollY&+12, brushWhite&, hWeaponFont&)
        END IF
      NEXT i&
    NEXT j&
  NEXT k&

  'sich bewegende Einheiten darstellen
  CALL RenderUnitMoving

  'Geschosse und Raketen darstellen
  CALL RenderMissiles

  'Einheiteninfo als Kurzform darstellen
  IF unitInfoOverlay& <> 0 THEN
    FOR k& = 0 TO nDrawnUnits&-1
      CALL RenderUnitInfoOverlay(drawnUnits&(k&))
    NEXT k&
  END IF

  'Kampfvorschau darstellen
  IF channels(0).info.difficulty = %DIFFICULTY_EASY OR cheatCombatPreview& <> 0 THEN
    selectedunit& = channels(0).player(localPlayerNr&).selectedunit
    CALL GetMapPos(mousexpos&, mouseypos&, i&, j&)
    IF i& >= 0 AND (channels(0).player(localPlayerNr&).targets(i&, j&) AND %TG_ATTACK) <> 0 AND selectedunit& >= 0 THEN
      unitnr& = channels(0).zone3(i&, j&)
      CALL RenderCombatPreview(selectedunit&, unitnr&, 0)
      CALL RenderCombatPreview(selectedunit&, unitnr&, 1)
    END IF
  END IF
'localPlayerMask& = 1

'D2D.GraphicLine(maparea.left, (maparea.top+maparea.bottom)/2, maparea.right, (maparea.top+maparea.bottom)/2, brushWhite&)
'D2D.GraphicLine((maparea.left+maparea.right)/2, maparea.top, (maparea.left+maparea.right)/2, maparea.bottom, brushWhite&)

'D2D.GraphicGetSize(hTerrain&, i&, j&)
'D2D.GraphicStretch(hTerrain&, 0, 0, i&, j&, maparea.left, maparea.top, i&/j&*(maparea.bottom-maparea.top), maparea.bottom, 1.0)

  D2D.ReleaseClippingRegion
END SUB



'Kampfvorschau anzeigen
'md&: 0 = Angreiferwerte , 1 = Verteidigerwerte darstellen
SUB RenderCombatPreview(attacker&, defender&, md&)
  LOCAL i&, srcpos&, t&, unitnr&, unittp&, px&, py&, dist&, mindamage&, maxdamage&, maxhp&, hp&, owner&, opacity!
  LOCAL orgcombatdata AS TCombatInfo

  'minimalen und maximalen Schaden ermitteln
  unitnr& = IIF&(md& = 0, attacker&, defender&)
  dist& = GetDistance&(channels(0).units(attacker&).xpos, channels(0).units(attacker&).ypos, channels(0).units(defender&).xpos, channels(0).units(defender&).ypos)
  orgcombatdata = channels(0).combat
  CALL SetCombatData(0, attacker&, defender&, -1, IIF&(dist& = 1, -1, 0), 0.8)
  mindamage& = IIF&(md& = 0, channels(0).combat.params(5, 0), channels(0).combat.params(5, 1))
  CALL SetCombatData(0, attacker&, defender&, -1, IIF&(dist& = 1, -1, 0), 1.2)
  maxdamage& = IIF&(md& = 0, channels(0).combat.params(5, 0), channels(0).combat.params(5, 1))
  channels(0).combat = orgcombatdata
  owner& = channels(0).units(unitnr&).owner
  unittp& = channels(0).units(unitnr&).unittype
  maxhp& = channelsnosave(0).unitclasses(unittp&).groupsize
  hp& = channels(0).units(unitnr&).groupsize

  'Darstellungsposition ermitteln
  CALL GetPixelPos(channels(0).units(unitnr&).xpos, channels(0).units(unitnr&).ypos, px&, py&)
  IF channels(0).units(attacker&).ypos = channels(0).units(defender&).ypos AND ABS(channels(0).units(attacker&).xpos-channels(0).units(defender&).xpos) = 2 AND md& = 1 THEN py& = py&+16*zoom#

  'Vorschau darstellen
  D2D.GraphicBox(px&-22*zoom#, py&-16*zoom#, px&+22*zoom#, py&-8*zoom#, brushLightGrey&, brushBlack50&)
  px& = px&-maxhp&*2*zoom#
  py& = py&-14*zoom#
  FOR i& = 1 TO maxhp&
    srcpos& = owner&*18
    IF i& > hp&-mindamage& THEN srcpos& = 18*7
    opacity! = 1
    IF i& > hp&-maxdamage& AND i& <= hp&-mindamage& THEN
      t& = INT(gametime!*2000) MOD 2000
      IF t& > 1000 THEN t& = 2000-t&
      opacity! = t&/1000
    END IF
    D2D.GraphicStretch(hHudElements&, srcpos&, 743, srcpos&+18, 761, px&+(4*zoom#)*(i&-1), py&, px&+(4*zoom#)*i&, py&+(4*zoom#), opacity!)
    IF i& > hp&-mindamage& AND i& <= hp& THEN D2D.GraphicStretch(hHudElements&, 18*8, 743, 18*9, 761, px&+(4*zoom#)*(i&-1), py&, px&+(4*zoom#)*i&, py&+(4*zoom#))
  NEXT i&
END SUB



'Tooltip anzeigen
SUB RenderToolTip(c AS IDXCONTROL, mx&, my&)
  LOCAL t$$, id&, x&, y&, textwd&, texthg&, p&

  IF c.ControlType <> %CTYPE_BUTTON AND c.ControlType <> %CTYPE_BITMAPBUTTON THEN EXIT SUB

  'Text für diesen Button ermitteln
  SELECT CASE c.ID
  CASE buttonMapInfo.ID TO buttonOpenMenu.ID: id& = c.ID-buttonMapInfo.ID
  CASE buttonShopMove.ID: id& = %WORD_TOOLTIP_MOVEUNIT-%WORD_TOOLTIP_MAPINFO
  CASE buttonShopRefuel.ID: id& = %WORD_TOOLTIP_REFUELUNIT-%WORD_TOOLTIP_MAPINFO
  CASE buttonShopRepair.ID: id& = %WORD_TOOLTIP_REPAIRUNIT-%WORD_TOOLTIP_MAPINFO
  CASE buttonShopBuild.ID: id& = %WORD_TOOLTIP_PRODUCEUNIT-%WORD_TOOLTIP_MAPINFO
  CASE buttonShopTrain.ID: id& = %WORD_TOOLTIP_TRAINUNIT-%WORD_TOOLTIP_MAPINFO
  CASE buttonChatTeam.ID, buttonChatAll.ID: id& = c.ID-buttonChatTeam.ID+14
  CASE buttonConnect.ID TO buttonJoinGame.ID: id& = c.ID-buttonConnect.ID+16
  CASE buttonChangeColor.ID: id& = %WORD_TOOLTIP_CHANGECOLOR-%WORD_TOOLTIP_MAPINFO
  CASE buttonClose.ID: id& = %WORD_TOOLTIP_CLOSESHOP-%WORD_TOOLTIP_MAPINFO
  END SELECT
  t$$ = words$$(%WORD_TOOLTIP_MAPINFO+id&)

  'Größe des Tooltips berechnen
  D2D.GraphicTextSizeW(t$$, hGameMessageFont&, textwd&, texthg&)
  textwd& = textwd&+32
  texthg& = texthg&+6
  x& = MIN&(mx&, windowWidth&-textwd&-2)
  y& = c.YPos+c.Height+2
  IF y&+texthg& > windowHeight& THEN y& = c.YPos-texthg&-1

  'Hintergrund
  D2D.GraphicBox(x&, y&, x&+textwd&, y&+texthg&, brushMenuBorder&, brushMenuBackground&)

  'Text
  p& = INSTR(t$$, "   ")
  IF p& = 0 THEN
    D2D.GraphicPrintW(t$$, x&+16, y&+3, brushLightGrey&, hGameMessageFont&)
  ELSE
    D2D.GraphicPrintW(LEFT$(t$$, p&-1), x&+16, y&+3, brushRed&, hGameMessageFont&)
    D2D.GraphicTextSizeW(LEFT$(t$$, p&-1), hGameMessageFont&, textwd&, texthg&)
    D2D.GraphicPrintW(MID$(t$$, p&), x&+16+textwd&, y&+3, brushLightGrey&, hGameMessageFont&)
  END IF
END SUB



'Protokoll anzeigen
SUB RenderProtocol
  LOCAL i&, p&, x&, y&, startpos&, cl&, textwd&, texthg&
  LOCAL a$$, t$$, fmt$$

  D2D.CreateClippingRegion(maparea.left, maparea.top, maparea.right-16, maparea.bottom)

  y& = maparea.top+2
  startpos& = protocolScrollbar.ScrollPosition
  FOR i& = startpos& TO protocolCount&-1
    'Einträge zeilenweise darstellen
    x& = maparea.left+2
    cl& = brushWhite&
    a$$ = protocolBuffer$$(i&)
    WHILE a$$ <> ""
      'Formatanweisungen suchen
      p& = INSTR(a$$, "$")
      IF p& = 1 THEN
        p& = INSTR(2, a$$, "$")
        fmt$$ = MID$(a$$, 2, p&-2)
        a$$ = MID$(a$$, p&+1)
        SELECT CASE fmt$$
        CASE "TITLE":  'Überschrift
          D2D.GraphicBox(maparea.left, y&+1, maparea.right-16, y&+20, brushGoldTransparent&, brushGoldTransparent&)
          D2D.GraphicTextSizeW(a$$, hGameMessageFont&, textwd&, texthg&)
          x& = (maparea.left+maparea.right-textwd&)/2
        CASE "COLOR0":  'Standardfarbe (weiß)
          cl& = brushWhite&
        CASE "COLOR1" TO "COLOR7":  'Spielerfarben
          cl& = brushPlayer&(ASC(fmt$$, 6)-49)
        END SELECT
        ITERATE LOOP
      END IF

      'Text darstellen
      IF p& = 0 THEN
        t$$ = a$$
        a$$ = ""
      ELSE
        t$$ = LEFT$(a$$, p&-1)
        a$$ = MID$(a$$, p&)
      END IF
      D2D.GraphicTextSizeW(t$$, hGameMessageFont&, textwd&, texthg&)
      D2D.GraphicPrintW(t$$, x&, y&, cl&, hGameMessageFont&)
      x& = x&+textwd&
    WEND
    y& = y&+20
  NEXT i&

  D2D.ReleaseClippingRegion
END SUB



'Terraininformationen anzeigen
SUB RenderTerrainInfo(x&, y&)
  LOCAL a$, textwd&, texthg&, x0&, y0&, x1&, y1&
  LOCAL def&

  CALL GetSpriteDestRect(x&, y&, x0&, y0&, x1&, y1&)
  IF x0& < 0 THEN EXIT SUB

  'Verteidigungsbonus
  def& = GetTerrainDefenseBonus&(0, x&, y&)
  IF def& > 0 THEN
    a$ = FORMAT$(def&)
    D2D.GraphicTextSize(a$, hWeaponFont&, textwd&, texthg&)
    D2D.GraphicPrint(a$, (x0&+x1&-textwd&)/2, y0&+2*zoom#, brushLightGrey&, hWeaponFont&)
  END IF
END SUB



'Einheiteninfo in Kurzform unter der Einheit darstellen
SUB RenderUnitInfoOverlay(unitnr&)
  LOCAL i&, p&, x&, y&, x0&, y0&, x1&, y1&, cl&, destx!, desty!, wd!, hg!, xoff!
  LOCAL unittp&, owner&, hp&, maxhp&, xp&, ammo&, fuel&, maxfuel&, fuellevel#

  'Darstellungsposition der Einheit ermitteln
  x& = channels(0).units(unitnr&).xpos
  y& = channels(0).units(unitnr&).ypos
  CALL GetSpriteDestRect(x&, y&, x0&, y0&, x1&, y1&)
  destx! = x0&+2*zoom#
  desty! = y1&-8*zoom#
  wd! = 20*zoom#
  hg! = 8*zoom#

  'Einheitenparameter auslesen
  unittp& = channels(0).units(unitnr&).unittype
  owner& = channels(0).units(unitnr&).owner
  hp& = channels(0).units(unitnr&).groupsize
  maxhp& = channelsnosave(0).unitclasses(unittp&).groupsize
  xp& = channels(0).units(unitnr&).experience-1
  fuel& = channels(0).units(unitnr&).fuel
  maxfuel& = channelsnosave(0).unitclasses(unittp&).fuel

  'Hintergrund
  D2D.GraphicBox(destx!, desty!, destx!+wd!, desty!+hg!, brushPlayer&(owner&), brushBlack50&)

  'Lebenspunkte
  xoff! = wd!/2-(maxhp&*1.8/2*zoom#)
  FOR i& = 0 TO maxhp&-1
    p& = owner&*18
    IF i& >= hp& THEN p& = 18*7
    D2D.GraphicStretch(hHudElements&, p&, 743, p&+18, 761, destx!+i&*1.8*zoom#+0.5*zoom#+xoff!, desty!+1.5*zoom#, destx!+i&*1.8*zoom#+2.0*zoom#+xoff!, desty!+3.0*zoom#)
  NEXT i&

  'Munition
  FOR i& = 0 TO 1
    IF channelsnosave(0).unitclasses(unittp&).weapons(i&).ammo > 0 THEN
      xoff! = i&*8*zoom#
      cl& = IIF&(i& = 0, brushGold&, brushSilver&)
      ammo& = channels(0).units(unitnr&).ammo(i&)
      SELECT CASE ammo&
      CASE 0:
        D2D.GraphicBox(destx!+2+xoff!, desty!+4*zoom#, destx!+2+5*zoom#+xoff!, desty!+4.5*zoom#, brushRed&, brushRed&)
      CASE 1 TO 4:
        FOR p& = 0 TO ammo&-1
          D2D.GraphicBox(destx!+2+p&*2*zoom#+xoff!, desty!+4*zoom#, destx!+2+p&*2*zoom#+1*zoom#+xoff!, desty!+5.5*zoom#, cl&, cl&)
        NEXT p&
      CASE ELSE:
        D2D.GraphicBox(destx!+2+xoff!, desty!+4*zoom#, destx!+2+7*zoom#+xoff!, desty!+5.5*zoom#, cl&, cl&)
      END SELECT
    END IF
  NEXT i&

  'Treibstoff
  SELECT CASE fuel&
  CASE 0:
    'kein Treibstoff
  CASE 1 TO 10:
    D2D.GraphicStretch(hHudElements&, 0, 768, fuel&*10, 776, destx!+1, desty!+hg!-2*zoom#, destx!+1+fuel&*2*zoom#, desty!+hg!-1)
  CASE ELSE:
    fuellevel# = fuel&/maxfuel&
    p& = MAX&(100, 408*fuellevel#)
    i& = MAX&(102, 308*fuellevel#)
    D2D.GraphicStretch(hHudElements&, p&, 768, p&+i&, 776, destx!+1, desty!+hg!-2*zoom#, destx!+1+fuellevel#*(wd!-2-4*zoom#), desty!+hg!-1)
  END SELECT

  'Erfahrungspunkte
  D2D.GraphicStretch(hHudElements&, 320, xp&*64, 384, xp&*64+64, destx!+wd!-5*zoom#, desty!+hg!-5*zoom#, destx!+wd!+1, desty!+hg!+1)
END SUB



'Einheiten-Artwork animiert darstellen
SUB DrawUnitAnimation(unittp&, BYVAL x0!, BYVAL y0!, BYVAL x1!, BYVAL y1!, OPTIONAL ocapity!)
  LOCAL wd&, hg&, cols&, rows&, totalframes&, framenr&, framex&, framey&, hnd&

  IF unitAnimFrameWidth& < 8 OR unitAnimFrameHeight& < 8 OR unitAnimFPS& < 1 THEN EXIT SUB

  'Gesamtanzahl Frames berechnen
  hnd& = channelsnosave(0).unitclasses(unittp&).artworkhandle
  D2D.GraphicGetSize(hnd&, wd&, hg&)
  cols& = INT(wd&/unitAnimFrameWidth&)
  rows& = INT(hg&/unitAnimFrameHeight&)
  totalframes& = cols&*rows&
  IF totalframes& = 0 THEN EXIT SUB

  'darzustellenden Frame ermitteln
  framenr& = INT(gametime!*unitAnimFPS&) MOD totalframes&
  framey& = INT(framenr&/cols&)
  framex& = framenr&-framey&*cols&

  'Frame darstellen
  IF ISMISSING(ocapity!) THEN
    D2D.GraphicStretch(hnd&, framex&*unitAnimFrameWidth&, framey&*unitAnimFrameHeight&, framex&*unitAnimFrameWidth&+unitAnimFrameWidth&, framey&*unitAnimFrameHeight&+unitAnimFrameHeight&, x0!, y0!, x1!, y1!)
  ELSE
    D2D.GraphicStretch(hnd&, framex&*unitAnimFrameWidth&, framey&*unitAnimFrameHeight&, framex&*unitAnimFrameWidth&+unitAnimFrameWidth&, framey&*unitAnimFrameHeight&+unitAnimFrameHeight&, x0!, y0!, x1!, y1!, ocapity!)
  END IF
END SUB



'Einheiteninfo darstellen
FUNCTION RenderUnitInfo&(BYVAL unitnr&)
  LOCAL unittp&, wd&, hg&, i&, x!, y!, p&, contentunitnr&, contentunittp&, colnr&, rownr&, iconx&, icony&, a$$
  LOCAL owner&, maxweight&, loadedweight&, xp&, hp&, maxhp&, ammo&, damage&, rangemin&, rangemax&, fuel&, maxfuel&, fuellevel#
  LOCAL unitclassname AS WSTRING, unitdescription AS WSTRING

  'Sichtbarkeit prüfen
  IF debugNoFog& = 0 THEN
    IF (channels(0).vision(channels(0).units(unitnr&).xpos, channels(0).units(unitnr&).ypos) AND localPlayerMask&) = 0 THEN EXIT FUNCTION
    'getauchte feindliche Einheit ist nur sichtbar, wenn sich diese direkt neben einer eigenen Einheit befindet
    IF (channels(0).units(unitnr&).flags AND %US_DIVE) <> 0 AND IsEnemyUnit&(0, localPlayerNr&, unitnr&) <> 0 AND IsAdjacentToUnitOfPlayer&(0, unitnr&, localPlayerNr&) = 0 THEN EXIT FUNCTION
  END IF

  'prüfen, ob andere Einheit gewählt wurde
  IF lastPreviewUnit& <> unitnr& THEN
    lastPreviewUnit& = unitnr&
    unitPreviewZoom# = 1.0
  ELSE
    unitPreviewZoom# = MAX(0.7, unitPreviewZoom#-0.01)
  END IF

  'Bild
  unittp& = channels(0).units(unitnr&).unittype
  IF unitAnimFrameWidth& = 0 THEN
    D2D.GraphicStretch(channelsnosave(0).unitclasses(unittp&).artworkhandle, 400-400*unitPreviewZoom#, 300-300*unitPreviewZoom#, 400+400*unitPreviewZoom#, 300+300*unitPreviewZoom#, unitpicarea.left, unitpicarea.top, unitpicarea.right, unitpicarea.bottom)
  ELSE
    CALL DrawUnitAnimation(unittp&, unitpicarea.left, unitpicarea.top, unitpicarea.right, unitpicarea.bottom)
  END IF

  'Name und Beschreibung
  unitclassname = channelsnosave(0).unitclasses(unittp&).uname
  D2D.GraphicTextSizeW(unitclassname, hCaptionFont&, wd&, hg&)
  D2D.GraphicPrintW(unitclassname, (unitpicarea.right+unitpicarea.left-wd&)/2, unitpicarea.top+16, brushWhite&, hCaptionFont&)
  unitdescription = channelsnosave(0).unitclasses(unittp&).description
  D2D.GraphicTextSizeW(unitdescription, hWeaponFont&, wd&, hg&)
  D2D.GraphicPrintW(unitdescription, (unitpicarea.right+unitpicarea.left-wd&)/2, unitpicarea.top+40, brushWhite&, hWeaponFont&)

  'Inhalt anzeigen (wird über dem Bild angezeigt)
  owner& = channels(0).units(unitnr&).owner
  IF (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_TRANSPORTER) <> 0 AND ((channels(0).player(localPlayerNr&).allymask AND 2^owner&) <> 0 OR debugNoFog& <> 0) THEN
    'geladenes Gewicht
    maxweight& = channelsnosave(0).unitclasses(unittp&).transportvolume
    loadedweight& = GetTransportWeight&(0, unitnr&)
    IF maxweight& > 0 THEN D2D.GraphicBox(unitpicarea.left+1, unitpicarea.top+1, unitpicarea.left+1+(unitpicarea.right-unitpicarea.left-2)*loadedweight&/maxweight&, unitpicarea.top+16*uiscale!, brushPlayer&(owner&), brushPlayer&(owner&))
    D2D.GraphicBox(unitpicarea.left+1, unitpicarea.top+1, unitpicarea.right-1, unitpicarea.top+16*uiscale!, brushWhite&, -1)
    a$$ = words$$(%WORD_LOADED_WEIGHT)+" "+FORMAT$(loadedweight&)+"/"+FORMAT$(maxweight&)
    D2D.GraphicTextSizeW(a$$, hSmallWeaponFont&, wd&, hg&)
    D2D.GraphicPrintW(a$$, (unitpicarea.right+unitpicarea.left-wd&)/2, unitpicarea.top+2, brushWhite&, hSmallWeaponFont&)
    'geladene Einheiten
    FOR i& = 0 TO 7
      D2D.GraphicStretch(hHudElements&, 0, 321, 36, 357, unitpicarea.left+(2+i&*40)*uiscale!, unitpicarea.bottom-38*uiscale!, unitpicarea.left+(38+i&*40)*uiscale!, unitpicarea.bottom-2*uiscale!)
      contentunitnr& = channels(0).units(unitnr&).transportcontent(i&)
      IF contentunitnr& >= 0 THEN
        contentunittp& = channels(0).units(contentunitnr&).unittype*6
        rownr& = INT(contentunittp&/40)
        colnr& = contentunittp&-rownr&*40
        D2D.GraphicStretch(hUnits&(owner&), colnr&*24, rownr&*24, colnr&*24+24, rownr&*24+24, unitpicarea.left+(2+i&*40)*uiscale!, unitpicarea.bottom-38*uiscale!, unitpicarea.left+(38+i&*40)*uiscale!, unitpicarea.bottom-2*uiscale!)
      END IF
    NEXT i&
  END IF

  'falls gewählte Einheit ein Transporter ist und sich der Mauscursor über dem Inhalt befindet, dann Kampfwerte des Inhalts anzeigen
  IF currentArea& = %AREA_UNITPIC AND (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_TRANSPORTER) <> 0 AND mouseypos& >= unitpicarea.bottom-38*uiscale! AND mouseypos& < unitpicarea.bottom-2*uiscale! THEN
    IF (channels(0).player(localPlayerNr&).allymask AND 2^owner&) <> 0 THEN
      FOR i& = 0 TO 7
        IF mousexpos& >= unitpicarea.left+(2+i&*40)*uiscale! AND mousexpos& < unitpicarea.left+(38+i&*40)*uiscale! THEN
          contentunitnr& = channels(0).units(unitnr&).transportcontent(i&)
          IF contentunitnr& >= 0 THEN
            unitnr& = contentunitnr&
            unittp& = channels(0).units(unitnr&).unittype
            EXIT FOR
          END IF
        END IF
      NEXT i&
    END IF
  END IF

  'Kampfwerte (Erfahrung)
  wd& = unitinfoarea.right-unitinfoarea.left
  hg& = unitinfoarea.bottom-unitinfoarea.top
  owner& = channels(0).units(unitnr&).owner
  D2D.GraphicStretch(hPanels&, 0, owner&*100, 320, owner&*100+100, unitinfoarea.left, unitinfoarea.top, unitinfoarea.right, unitinfoarea.bottom)
  xp& = channels(0).units(unitnr&).experience-1
  D2D.GraphicStretch(hHudElements&, 320, xp&*64, 384, xp&*64+64, unitinfoarea.left+wd&*2/5, unitinfoarea.top+hg&*18/100, unitinfoarea.right-wd&*2/5, unitinfoarea.bottom-hg&*18/100)

  'Kampfwerte (Lebenspunkte)
  maxhp& = channelsnosave(0).unitclasses(unittp&).groupsize
  hp& = channels(0).units(unitnr&).groupsize
  x! = unitinfoarea.left+wd&/2-maxhp&*9*uiscale!
  y! = unitinfoarea.top+1
  FOR i& = 0 TO maxhp&-1
    p& = owner&*18
    IF i& >= hp& THEN p& = 18*7
    D2D.GraphicStretch(hHudElements&, p&, 743, p&+18, 761, x!, y!, x!+18*uiscale!, y!+18*uiscale!)
    x! = x!+18*uiscale!
  NEXT i&

  'Kampfwerte (Waffen)
  FOR i& = 0 TO 3
    x! = IIF&(i& < 2, unitinfoarea.left+wd&*26/320, unitinfoarea.left+wd&*196/320)
    y! = IIF&(i& = 0 OR i& = 2, unitinfoarea.top+hg&*28/100, unitinfoarea.top+hg&*58/100)
    IF channelsnosave(0).unitclasses(unittp&).weapons(i&).ammo > 0 THEN
      CALL GetWeaponIcon(unitnr&, i&, iconx&, icony&)
      D2D.GraphicStretch(hHudElements&, iconx&, icony&, iconx&+16, icony&+16, x!, y!, x!+16*uiscale!, y!+16*uiscale!)
      ammo& = channels(0).units(unitnr&).ammo(i&)
      D2D.GraphicPrint(FORMAT$(ammo&), x!+24*uiscale!, y!, brushWhite&, hWeaponFont&)
      damage& = channelsnosave(0).unitclasses(unittp&).weapons(i&).damage
      IF damage& > 0 THEN D2D.GraphicPrint(FORMAT$(damage&), x!+48*uiscale!, y!, brushWhite&, hWeaponFont&)
      rangemin& = channelsnosave(0).unitclasses(unittp&).weapons(i&).minrange
      rangemax& = channelsnosave(0).unitclasses(unittp&).weapons(i&).maxrange
      IF rangemin& = rangemax& THEN a$$ = FORMAT$(rangemax&) ELSE a$$ = FORMAT$(rangemin&)+"-"+FORMAT$(rangemax&)
      IF a$$ <> "0" THEN D2D.GraphicPrintW(a$$, x!+80*uiscale!, y!, brushWhite&, hWeaponFont&)
    END IF
  NEXT i&

  'Kampfwerte (Treibstoff)
  fuel& = channels(0).units(unitnr&).fuel
  maxfuel& = channelsnosave(0).unitclasses(unittp&).fuel
  x! = unitinfoarea.left+7*uiscale!
  y! = unitinfoarea.top+87*uiscale!
  SELECT CASE fuel&
  CASE 0:
    'kein Treibstoff
  CASE 1 TO 10:
    D2D.GraphicStretch(hHudElements&, 0, 768, fuel&*10, 776, x!, y!, x!+fuel&*10*uiscale!, y!+8*uiscale!)
  CASE ELSE:
    fuellevel# = fuel&/maxfuel&
    p& = MAX&(100, 408*fuellevel#)
    i& = MAX&(102, 308*fuellevel#)
    D2D.GraphicStretch(hHudElements&, p&, 768, p&+i&, 776, x!, y!, x!+i&*uiscale!, y!+8*uiscale!)
  END SELECT
  IF debugInfo& <> 0 THEN
    a$$ = FORMAT$(fuel&)+"/"+FORMAT$(maxfuel&)
    D2D.GraphicTextSizeW(a$$, hSystemFont&, wd&, hg&)
    D2D.GraphicPrintW(a$$, (unitinfoarea.left+unitinfoarea.right-wd&)/2, unitinfoarea.top+85*uiscale!, IIF&(fuel& < maxfuel&/2 OR fuel& <= 10, brushWhite&, brushBlue&), hSystemFont&)
  END IF

  'Kampfwerte (Bewegungsreichweite)
  D2D.GraphicPrint(FORMAT$(INT(channelsnosave(0).unitclasses(unittp&).range/8)), unitinfoarea.left+wd&*20/320, unitinfoarea.top+hg&*3/100, brushWhite&, hWeaponFont&)

  'Kampfwerte (Sicht)
  D2D.GraphicPrint(FORMAT$(INT(channelsnosave(0).unitclasses(unittp&).sight/8)), unitinfoarea.left+wd&*52/320, unitinfoarea.top+hg&*3/100, brushWhite&, hWeaponFont&)

  'Kampfwerte (Panzerung)
  D2D.GraphicPrint(FORMAT$(channelsnosave(0).unitclasses(unittp&).armor), unitinfoarea.left+wd&*274/320, unitinfoarea.top+hg&*3/100, brushWhite&, hWeaponFont&)

  'Kampfwerte (Auf/Absteigen)
  IF (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_CLIMB) <> 0 THEN
    p& = IIF&((channels(0).units(unitnr&).flags AND %US_ASCEND) = 0, 186, 126+owner&*10)
    D2D.GraphicStretch(hHudElements&, p&, 727, p&+10, 735, unitinfoarea.right-14*uiscale!, unitinfoarea.top+3*uiscale!, unitinfoarea.right-4*uiscale!, unitinfoarea.top+11*uiscale!)
    D2D.GraphicStretch(hHudElements&, 186, 734, 196, 742, unitinfoarea.right-14*uiscale!, unitinfoarea.top+12*uiscale!, unitinfoarea.right-4*uiscale!, unitinfoarea.top+20*uiscale!)
  END IF
  IF (channelsnosave(0).unitclasses(unittp&).flags AND %UCF_DIVE) <> 0 THEN
    p& = IIF&((channels(0).units(unitnr&).flags AND %US_DIVE) = 0, 186, 126+owner&*10)
    D2D.GraphicStretch(hHudElements&, 186, 727, 196, 735, unitinfoarea.right-14*uiscale!, unitinfoarea.top+3*uiscale!, unitinfoarea.right-4*uiscale!, unitinfoarea.top+11*uiscale!)
    D2D.GraphicStretch(hHudElements&, p&, 734, p&+10, 742, unitinfoarea.right-14*uiscale!, unitinfoarea.top+12*uiscale!, unitinfoarea.right-4*uiscale!, unitinfoarea.top+20*uiscale!)
  END IF

  RenderUnitInfo& = 1
END FUNCTION



'Shopinfo darstellen
FUNCTION RenderShopInfo&(BYVAL shopnr&)
  LOCAL shoptype&, owner&, i&, x!, y!, wd&, hg&, textwd&, texthg&, animstep&, unitnr&, unittp&, rownr&, colnr&, contentcount&
  LOCAL a$

  'Sichtbarkeit des Feldes prüfen
  IF debugNoFog& = 0 THEN
    IF (channels(0).vision(channels(0).shops(shopnr&).position, channels(0).shops(shopnr&).position2) AND localPlayerMask&) = 0 THEN EXIT FUNCTION
  END IF

  'prüfen, ob anderer Shop gewählt wurde
  IF lastPreviewShop& <> shopnr& THEN
    lastPreviewShop& = shopnr&
    shopPreviewZoom# = 1.0
  ELSE
    shopPreviewZoom# = MAX(0.0, shopPreviewZoom#-0.01)
  END IF

  'Bild
  shoptype& = LOG2(channels(0).shops(shopnr&).shoptype)
  IF shoptype& > 6 THEN shoptype& = 7
  x! = (shoptype& MOD 3)*%BUILDINGS_WIDTH
  y! = INT(shoptype&/3)*%BUILDINGS_HEIGHT
  D2D.GraphicStretch(hBuildings&, _
                    x!+%BUILDINGS_WIDTH/10*shopPreviewZoom#, y!+%BUILDINGS_HEIGHT/10*shopPreviewZoom#, x!+%BUILDINGS_WIDTH-%BUILDINGS_WIDTH/10*shopPreviewZoom#, y!+%BUILDINGS_HEIGHT-%BUILDINGS_HEIGHT/10*shopPreviewZoom#, _
                    unitpicarea.left, unitpicarea.top, unitpicarea.right, unitpicarea.bottom)

  'Panel
  wd& = unitinfoarea.right-unitinfoarea.left
  hg& = unitinfoarea.bottom-unitinfoarea.top
  owner& = channels(0).shops(shopnr&).owner
  D2D.GraphicStretch(hPanels&, 960, owner&*100, 1280, owner&*100+100, unitinfoarea.left, unitinfoarea.top, unitinfoarea.right, unitinfoarea.bottom)

  'Shopname
  a$ = channels(0).info.shopnames(shopnr&)
  D2D.GraphicTextSize(a$, hWeaponFont&, textwd&, texthg&)
  D2D.GraphicPrint(a$, (unitinfoarea.left+unitinfoarea.right-textwd&)/2, unitinfoarea.top+13*uiscale!-texthg&/2, brushWhite&, hWeaponFont&)

  'Sichtbarkeit des Inhalts prüfen
  IF (owner& <> 6) AND (channels(0).player(localPlayerNr&).allymask AND 2^owner&) = 0 AND debugNoFog& = 0 THEN
    RenderShopInfo& = 2
    EXIT FUNCTION
  END IF

  'Energieproduktion anzeigen
  a$ = "+"+FORMAT$(channels(0).shops(shopnr&).eplus)
  D2D.GraphicTextSize(a$, hWeaponFont&, textwd&, texthg&)
  D2D.GraphicPrint(a$, unitinfoarea.left+wd&*30/320-textwd&/2, unitinfoarea.top+hg&*26/100-texthg&/2, brushWhite&, hWeaponFont&)
  animstep& = INT(gametime!*1000/shopAnimationSpeed!) AND 3
  D2D.GraphicStretch(hHudElements&, animstep&*24, 652, animstep&*24+24, 676, unitinfoarea.left-3*wd&/320*uiscale!, unitinfoarea.top+hg&*35/100*uiscale!, unitinfoarea.left+21*wd&/320*uiscale!, unitinfoarea.top+hg&*59/100*uiscale!)

  'Materialproduktion anzeigen
  a$ = "+"+FORMAT$(channels(0).shops(shopnr&).mplus)
  D2D.GraphicTextSize(a$, hWeaponFont&, textwd&, texthg&)
  D2D.GraphicPrint(a$, unitinfoarea.left+wd&*289/320-textwd&/2, unitinfoarea.top+hg&*26/100-texthg&/2, brushWhite&, hWeaponFont&)
  animstep& = (INT(gametime!*1000/shopAnimationSpeed!)+2) AND 3
  D2D.GraphicStretch(hHudElements&, animstep&*24+96, 652, animstep&*24+120, 676, unitinfoarea.right-23*wd&/320*uiscale!, unitinfoarea.top+hg&*35/100*uiscale!, unitinfoarea.right+1*wd&/320*uiscale!, unitinfoarea.top+hg&*59/100*uiscale!)

  'vorhandenes Material anzeigen
  a$ = FORMAT$(channels(0).shops(shopnr&).material)
  D2D.GraphicTextSize(a$, hSmallWeaponFont&, textwd&, texthg&)
  D2D.GraphicPrint(a$, unitinfoarea.left+wd&*305/320-textwd&/2, unitinfoarea.top+hg&*86/100-texthg&/2, brushWhite&, hSmallWeaponFont&)

  'Inhalt anzeigen (Slots)
  FOR i& = 0 TO 15
    SELECT CASE i&
    CASE 0 TO 4:
      x! = 42+i&*50
      y! = 25
    CASE 5 TO 10:
      x! = 17+i&*50-250
      y! = 44
    CASE 11 TO 15:
      x! = 42+i&*50-550
      y! = 63
    END SELECT
    D2D.GraphicStretch(hHudElements&, 37, 321, 73, 357, unitinfoarea.left+x!*uiscale!, unitinfoarea.top+y!*uiscale!, unitinfoarea.left+(x!+36)*uiscale!, unitinfoarea.top+(y!+36)*uiscale!)
  NEXT i&

  'Inhalt anzeigen (Einheiten)
  FOR i& = 0 TO 15
    unitnr& = channels(0).shops(shopnr&).content(i&)
    IF unitnr& >= 0 THEN
      unittp& = channels(0).units(unitnr&).unittype*6
      rownr& = INT(unittp&/40)
      colnr& = unittp&-rownr&*40
      SELECT CASE contentcount&
      CASE 0 TO 4:
        x! = 42+contentcount&*50
        y! = 25
      CASE 5 TO 10:
        x! = 17+contentcount&*50-250
        y! = 44
      CASE 11 TO 15:
        x! = 42+contentcount&*50-550
        y! = 63
      END SELECT
      D2D.GraphicStretch(hUnits&(owner&), colnr&*24, rownr&*24, colnr&*24+24, rownr&*24+24, unitinfoarea.left+x!*uiscale!, unitinfoarea.top+y!*uiscale!, unitinfoarea.left+(x!+36)*uiscale!, unitinfoarea.top+(y!+36)*uiscale!)
      IF (channels(0).units(unitnr&).flags AND %US_DONE) <> 0 THEN
        D2D.GraphicStretch(hUnits&(LEN(playerColors$)), colnr&*24, rownr&*24, colnr&*24+24, rownr&*24+24, unitinfoarea.left+x!*uiscale!, unitinfoarea.top+y!*uiscale!, unitinfoarea.left+(x!+36)*uiscale!, unitinfoarea.top+(y!+36)*uiscale!)
      END IF
      IF debugShowUnits& <> 0 THEN
        D2D.GraphicPrint(FORMAT$(unitnr&), unitinfoarea.left+x!*uiscale!, unitinfoarea.top+y!*uiscale!, brushWhite&, hWeaponFont&)
      END IF
      contentcount& = contentcount&+1
    END IF
  NEXT i&

  RenderShopInfo& = 1
END FUNCTION



'Meldungen darstellen
SUB RenderMessages
  LOCAL a$$, b$$, i&, n&, y&, cl&, brush&, textwd&, texthg&, maxwd&

  D2D.CreateClippingRegion(messagearea.left, messagearea.top, messagearea.right, messagearea.bottom)
  maxwd& = messagearea.right-messagearea.left-4

  y& = messagearea.top+2
  FOR i& = 0 TO messageCount&-1
    'Nachricht zeilenweise darstellen
    cl& = ASC(messageBuffer$$(i&))
    a$$ = MID$(messageBuffer$$(i&), 2)
    SELECT CASE cl&
    CASE 1 TO 6: brush& = brushPlayer&(cl&-1)
    CASE 7: brush& = brushRed&
    CASE 8: brush& = brushBronze&
    CASE ELSE: brush& = brushWhite&
    END SELECT
    DO
      n& = LEN(a$$)
      DO
        b$$ = LEFT$(a$$, n&)
        D2D.GraphicTextSizeW(b$$, hSystemFont&, textwd&, texthg&)
        IF textwd& <= maxwd& THEN EXIT LOOP
        n& = INSTR(-1, b$$, " ")
        IF n& = 0 THEN n& = LEN(b$$)
        n& = n&-1
      LOOP
      D2D.GraphicPrintW(b$$, messagearea.left+2, y&, brush&, hSystemFont&)
      y& = y&+13
      a$$ = LTRIM$(MID$(a$$, LEN(b$$)+1))
    LOOP UNTIL a$$ = ""
    y& = y&+3
  NEXT i&

  'älteste Nachricht entfernen, falls mehr Nachrichten vorhanden sind, als dargestellt werden können
  IF y& >= messagearea.bottom THEN
    ARRAY DELETE messageBuffer$$(0)
    messageCount& = messageCount&-1
  END IF

  D2D.ReleaseClippingRegion
END SUB



'Dialog darstellen
SUB RenderDialogues
  LOCAL centerx!, basey!, destx!, desty!, headerWidthScaled!, headerHeightScaled!, dialogueWidthScaled!, dialogueHeightScaled!, t!, xoffset!, yoffset!
  LOCAL srcTexture&
  LOCAL srcrect AS RECT, destrect AS RECT

  IF hDialog& < 0 THEN EXIT SUB
  srcTexture& = hDialog&

  'prüfen, welcher Dialog angezeigt werden soll
  xoffset! = 0
  yoffset! = 5*uiscale!
  IF menuOpenTime! > 0 THEN
    'Menü
    t! = gametime!-menuOpenTime!
    srcrect = txarea_menu
  ELSE
    IF gameState& = %GAMESTATE_INGAME AND messageOpenTime! > 0 THEN
      'Spiel-Nachricht
      IF GetPhase&(0, localPlayerNr&) = %PHASE_COMBAT THEN
        'Öffnen des Nachrichtenfensters verzögern bis Kampffenster wieder geschlossen wurde
        messageOpenTime! = gametime!
        GOTO RenderDialogues_Combat
      END IF
      t! = gametime!-messageOpenTime!
      IF IsVideoMessage&(0, currentMessageId&) = 0 THEN
        srcrect = txarea_msg
      ELSE
        srcrect = txarea_videomsg
        srcTexture& = hDialog2&
        xoffset! = -100*uiscale!
      END IF
    END IF
    IF gameState& = %GAMESTATE_INGAME AND GetPhase&(0, localPlayerNr&) = %PHASE_COMBAT THEN
      'Kampf
      RenderDialogues_Combat:
      t! = gametime!-combatStartTime!
      IF combatMode& = 1 THEN srcrect = txarea_combat1 ELSE srcrect = txarea_combat2
    END IF
    IF gameState& = %GAMESTATE_INGAME AND selectedShop& >= 0 THEN
      'Shop
      t! = gametime!-shopSelectionTime!
      IF selectedShopProd$ = "" THEN srcrect = txarea_shop1 ELSE srcrect = txarea_shop2
    END IF
    IF gameState& = %GAMESTATE_INGAME AND mapinfoOpenTime! > 0 THEN
      t! = gametime!-mapinfoOpenTime!
      srcrect = txarea_mapinfo
    END IF
    IF gameState& = %GAMESTATE_INGAME AND highscoreOpenTime! > 0 THEN
      t! = gametime!-highscoreOpenTime!
      srcrect = txarea_highscore
    END IF
    IF lobbyOpenTime! > 0 THEN
      IF menuOpenTime! > 0 THEN
        'Öffnen der Lobby verzögern bis Hautpmenü wieder geschlossen wurde
        lobbyOpenTime! = gametime!
      ELSE
        t! = gametime!-lobbyOpenTime!
        srcrect = txarea_mplobby
      END IF
    END IF
  END IF

  'Position des Dialogs berechnen und Dialog anzeigen
  dialogueWidthScaled! = (srcrect.right-srcrect.left)*uiscale!
  dialogueHeightScaled! = (srcrect.bottom-srcrect.top)*uiscale!
  centerx! = (maparea.left+maparea.right)/2
  basey! = maparea.bottom
  IF t! > 0 AND t! <= %DIALOGUE_OPEN_MS/1000 THEN
    destrect.left = centerx!-dialogueWidthScaled!/2+xoffset!
    destrect.right = centerx!+dialogueWidthScaled!/2+xoffset!
    IF dialogueClosing& = 0 THEN
      destrect.top = basey!-(dialogueHeightScaled!-yoffset!)*t!*1000/%DIALOGUE_OPEN_MS
      srcrect.bottom = srcrect.top+(srcrect.bottom-srcrect.top-yoffset!)*t!*1000/%DIALOGUE_OPEN_MS
    ELSE
      destrect.top = basey!-(dialogueHeightScaled!-yoffset!)*(1-t!*1000/%DIALOGUE_OPEN_MS)
      srcrect.bottom = srcrect.top+(srcrect.bottom-srcrect.top-yoffset!)*(1-t!*1000/%DIALOGUE_OPEN_MS)
    END IF
    destrect.bottom = basey!
    D2D.GraphicStretch(srcTexture&, srcrect.left, srcrect.top, srcrect.right, srcrect.bottom, destrect.left, destrect.top, destrect.right, destrect.bottom)
  END IF

  'sobald Dialog beim Schließen seine Zielposition erreicht hat, Dialog ausblenden
  IF dialogueClosing& <> 0 AND t! > %DIALOGUE_OPEN_MS/1000 THEN
    CALL CloseAllDialogues
    EXIT SUB
  END IF

  'sobald Dialog seine Zielposition erreicht hat, Details anzeigen
  IF t! > %DIALOGUE_OPEN_MS/1000 THEN
    activedialoguearea.left = centerx!-dialogueWidthScaled!/2+xoffset!
    activedialoguearea.right = centerx!+dialogueWidthScaled!/2+xoffset!
    activedialoguearea.top = basey!-dialogueHeightScaled!+yoffset!
    activedialoguearea.bottom = basey!+yoffset!
    IF menuOpenTime! = 0 THEN
      buttonClose.XPos = activedialoguearea.right-61*uiscale!
      buttonClose.YPos = activedialoguearea.top-10*uiscale!
      IF highscoreOpenTime! > 0 THEN buttonClose.YPos = activedialoguearea.top+62*uiscale!
      IF lobbyOpenTime! > 0 THEN buttonClose.YPos = activedialoguearea.top+20*uiscale!
      buttonClose.Visible = 1
    END IF
    IF menuOpenTime! > 0 THEN
      CALL RenderMenu
    ELSE
      IF messageOpenTime! > 0 AND GetPhase&(0, localPlayerNr&) <> %PHASE_COMBAT THEN
        CALL RenderGameMessage
      ELSE
        IF GetPhase&(0, localPlayerNr&) = %PHASE_COMBAT THEN
          CALL RenderCombat
        ELSE
          IF selectedShop& >= 0 THEN
            CALL RenderShop(selectedShop&)
          ELSE
            IF mapinfoOpenTime! > 0 THEN
              CALL RenderMapInfo
            ELSE
              IF highscoreOpenTime! > 0 THEN
                CALL RenderHallOfFame
              ELSE
                IF lobbyOpenTime! > 0 THEN
                  CALL SetMultiplayerLobbyControls(1)
                  CALL RenderLobby
                END IF
              END IF
            END IF
          END IF
        END IF
      END IF
    END IF
  ELSE
    'MILOP Logo (nur anzeigen, solange Dialog noch nicht seine Zielposition erreicht hat - anschließend stellt die Detail-Funktion den Header dar)
    headerWidthScaled! = (txarea_milopheader.right-txarea_milopheader.left)*uiscale!
    headerHeightScaled! = (txarea_milopheader.bottom-txarea_milopheader.top)*uiscale!
    destx! = centerx!-headerWidthScaled!/2
    desty! = maparea.bottom-37*uiscale!
    IF t! > 0 AND t! <= %DIALOGUE_OPEN_MS/1000 THEN
      IF dialogueClosing& = 0 THEN
        desty! = desty!-(dialogueHeightScaled!-yoffset!)*t!*1000/%DIALOGUE_OPEN_MS
      ELSE
        desty! = desty!-(dialogueHeightScaled!-yoffset!)*(1-t!*1000/%DIALOGUE_OPEN_MS)
      END IF
    END IF
    D2D.GraphicStretch(hDialog&, txarea_milopheader.left, txarea_milopheader.top, txarea_milopheader.right, txarea_milopheader.bottom, destx!, desty!, destx!+headerWidthScaled!, desty!+headerHeightScaled!)
  END IF
END SUB



'Kampf darstellen
SUB RenderCombat
  LOCAL t!, destx!, desty!, x&, i&, k&, srcimg&, combatval!, srcx!, srcy!
  LOCAL att&, def&, attunittp&, defunittp&, attowner&, defowner&, sprnr1&, sprnr2&, textwd&, texthg&, xp&, hp&, maxhp&, headerWidth&, headerHeight&
  LOCAL a$

  t! = gametime!-combatStartTime!-%DIALOGUE_OPEN_MS/1000
  IF t! < 0 THEN
    combatStartTime! = 0
    EXIT SUB
  END IF

  IF combatMode& = 1 THEN
    CALL RenderCombatMinimal
    EXIT SUB
  END IF

  'beteiligte Einheiten ermitteln
  att& = channels(0).combat.attacker
  def& = channels(0).combat.defender
  attunittp& = channels(0).units(att&).unittype
  defunittp& = channels(0).units(def&).unittype
  attowner& = channels(0).units(att&).owner
  defowner& = channels(0).units(def&).owner

  'Fenster zeichnen (Hintergrund)
  D2D.GraphicStretch(hDialog&, txarea_combat2.left, txarea_combat2.top, txarea_combat2.right, txarea_combat2.bottom, activedialoguearea.left, activedialoguearea.top, activedialoguearea.right, activedialoguearea.bottom)
  D2D.GraphicStretch(hPanels&, 320, attowner&*100, 640, attowner&*100+100, activedialoguearea.left+108*uiscale!, activedialoguearea.top+348*uiscale!, activedialoguearea.left+428*uiscale!, activedialoguearea.top+448*uiscale!)
  D2D.GraphicStretch(hPanels&, 640, defowner&*100, 960, defowner&*100+100, activedialoguearea.left+448*uiscale!, activedialoguearea.top+348*uiscale!, activedialoguearea.left+768*uiscale!, activedialoguearea.top+448*uiscale!)

  'Fenster zeichnen (Einheiten)
  destx! = activedialoguearea.left+28*uiscale!  'linke obere Ecke des Angreiferbilds
  desty! = activedialoguearea.top+28*uiscale!
  srcx! = IIF&(t! < 1.0, 800*t!, 800)
  IF unitAnimFrameWidth& = 0 THEN
    D2D.GraphicStretch(channelsnosave(0).unitclasses(attunittp&).artworkhandle, 800-srcx!, 0, 800, 600, destx!, desty!, destx!+(srcx!*400/800)*uiscale!, desty!+300*uiscale!)
    D2D.GraphicStretch(channelsnosave(0).unitclasses(defunittp&).artworkhandle, 0, 0, srcx!, 600, destx!+(820-srcx!*400/800)*uiscale!, desty!, destx!+820*uiscale!, desty!+300*uiscale!)
  ELSE
    CALL DrawUnitAnimation(attunittp&, destx!, desty!, destx!+400*uiscale!, desty!+300*uiscale!)
    CALL DrawUnitAnimation(defunittp&, destx!+420*uiscale!, desty!, destx!+820*uiscale!, desty!+300*uiscale!)
  END IF

  'Fenster zeichnen (Kampfwerte)
  destx! = activedialoguearea.left+108*uiscale!  'linke obere Ecke der Angreiferwerte
  desty! = activedialoguearea.top+348*uiscale!
  FOR k& = 0 TO 1
    FOR i& = 0 TO 2
      combatval! = channels(0).combat.params(i&, k&)*0.12
      srcx! = i&*30+t!*10
      srcy! = i&*20+t!*30
      IF t! < 1.0 THEN combatval! = combatval!*t!
      IF k& = 0 THEN
        D2D.GraphicStretch(hHudElements&, srcx!, srcy!, srcx!+combatval!, srcy!+9, destx!+36*uiscale!, desty!+(11+i&*24)*uiscale!, destx!+(36+combatval!)*uiscale!, desty!+(25+i&*24)*uiscale!)
      ELSE
        D2D.GraphicStretch(hHudElements&, srcx!, srcy!, srcx!+combatval!, srcy!+9, destx!+(624-combatval!)*uiscale!, desty!+(11+i&*24)*uiscale!, destx!+624*uiscale!, desty!+(25+i&*24)*uiscale!)
      END IF
    NEXT i&
    'Gesamtwert
    x& = channels(0).combat.params(4, k&)
    IF t! < 1.0 THEN x& = x&*t!
    a$ = FORMAT$(x&)
    D2D.GraphicTextSize(a$, hCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrint(a$, destx!+IIF&(k& = 0, 270, 390)*uiscale!-textwd&/2, desty!+50*uiscale!-texthg&/2, brushWhite&, hCaptionFont&)
  NEXT k&

  'Fenster zeichnen (Erfahrungspunkte, Panzerung)
  xp& = channels(0).units(att&).experience-1
  D2D.GraphicStretch(hHudElements&, 320, xp&*64, 384, xp&*64+64, destx!+158*uiscale!, desty!+18*uiscale!, destx!+222*uiscale!, desty!+82*uiscale!)
  xp& = channels(0).units(def&).experience-1
  D2D.GraphicStretch(hHudElements&, 320, xp&*64, 384, xp&*64+64, destx!+438*uiscale!, desty!+18*uiscale!, destx!+502*uiscale!, desty!+82*uiscale!)

  a$ = FORMAT$(channelsnosave(0).unitclasses(attunittp&).armor)
  D2D.GraphicTextSize(a$, hSmallWeaponFont&, textwd&, texthg&)
  D2D.GraphicPrint(a$, destx!+223*uiscale!-textwd&/2, desty!+90*uiscale!-texthg&/2, brushWhite&, hSmallWeaponFont&)
  a$ = FORMAT$(channelsnosave(0).unitclasses(defunittp&).armor)
  D2D.GraphicTextSize(a$, hSmallWeaponFont&, textwd&, texthg&)
  D2D.GraphicPrint(a$, destx!+451*uiscale!-textwd&/2, desty!+90*uiscale!-texthg&/2, brushWhite&, hSmallWeaponFont&)

  'Fenster zeichnen (Lebenspunkte)
  FOR k& = 0 TO 1
    maxhp& = channelsnosave(0).unitclasses(IIF&(k& = 0, attunittp&, defunittp&)).groupsize
    hp& = channels(0).units(IIF&(k& = 0, att&, def&)).groupsize
    IF t! > 1.0 AND t! < 2.5 THEN hp& = INT(hp&*(2.5-t!)/1.5+(hp&-channels(0).combat.params(5, k&))*(t!-1.0)/1.5)
    IF t! >= 2.5 THEN hp& = hp&-channels(0).combat.params(5, k&)
    FOR i& = 0 TO maxhp&-1
      IF i& < hp& THEN srcimg& = IIF&(k& = 0, attowner&, defowner&) ELSE srcimg& = 7
      D2D.GraphicStretch(hHudElements&, srcimg&*18, 743, srcimg&*18+18, 761, destx!+(IIF&(k& = 0, 2, 478)+i&*18)*uiscale!, desty!+80*uiscale!, destx!+(IIF&(k& = 0, 20, 496)+i&*18)*uiscale!, desty!+98*uiscale!)
    NEXT i&
  NEXT k&

  'Header mit Einheitennamen zeichnen
  destx! = activedialoguearea.left+55*uiscale!  'linke obere Ecke des Angreifer-Headers
  desty! = activedialoguearea.top-17*uiscale!
  headerWidth& = (txarea_blankheader.right-txarea_blankheader.left)*uiscale!*0.6
  headerHeight& = (txarea_blankheader.bottom-txarea_blankheader.top)*uiscale!*0.6
  D2D.GraphicStretch(hDialog&, txarea_blankheader.left, txarea_blankheader.top, txarea_blankheader.right, txarea_blankheader.bottom, destx!, desty!, destx!+headerWidth&, desty!+headerHeight&)
  D2D.GraphicStretch(hDialog&, txarea_blankheader.left, txarea_blankheader.top, txarea_blankheader.right, txarea_blankheader.bottom, destx!+420*uiscale!, desty!, destx!+headerWidth&+420*uiscale!, desty!+headerHeight&)
  a$ = channelsnosave(0).unitclasses(attunittp&).uname
  IF debugShowUnits& <> 0 THEN a$ = FORMAT$(att&)+"/"+a$
  D2D.GraphicTextSize(a$, hCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrint(a$, destx!+(headerWidth&-textwd&)/2, desty!+(headerHeight&-texthg&)/2, brushWhite&, hCaptionFont&)
  a$ = channelsnosave(0).unitclasses(defunittp&).uname
  IF debugShowUnits& <> 0 THEN a$ = FORMAT$(def&)+"/"+a$
  D2D.GraphicTextSize(a$, hCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrint(a$, destx!+(headerWidth&-textwd&)/2+420*uiscale!, desty!+(headerHeight&-texthg&)/2, brushWhite&, hCaptionFont&)

  'Sound-Effekte
  IF (combatSoundEffects& AND 1) = 0 AND t! > 0.5 THEN
    combatSoundEffects& = combatSoundEffects& OR 1
    CALL PlaySoundEffect(hFirstEffect&+channelsnosave(0).unitclasses(attunittp&).sfxfire, %SOUNDBUFFER_EFFECT2, %PLAYFLAGS_LOOPING)
  END IF
  IF (combatSoundEffects& AND 2) = 0 AND t! > 1.0 AND channels(0).combat.params(4, 1) > 0 THEN
    combatSoundEffects& = combatSoundEffects& OR 2
    CALL PlaySoundEffect(hFirstEffect&+channelsnosave(0).unitclasses(defunittp&).sfxfire, %SOUNDBUFFER_EFFECT3, %PLAYFLAGS_LOOPING)
  END IF

  'Kampf beenden sobald Sequenz vorbei ist
  IF t! >= 3.5 THEN
    soundchannels(%SOUNDBUFFER_EFFECT2).Stop
    soundchannels(%SOUNDBUFFER_EFFECT3).Stop
    combatStartTime! = gametime!
    dialogueClosing& = 1
    buttonClose.Visible = 0
  END IF
END SUB



'Kampf minimal darstellen
SUB RenderCombatMinimal
  LOCAL t!, destx!, desty!, x&, i&, k&, srcimg&, combatval!, srcx!, srcy!, headerWidth!
  LOCAL att&, def&, attunittp&, defunittp&, attowner&, defowner&, sprnr1&, sprnr2&, textwd&, texthg&, xp&, hp&, maxhp&
  LOCAL a$

  IF replayMode&(0) = %REPLAYMODE_FASTPLAY THEN EXIT SUB
  t! = gametime!-combatStartTime!-%DIALOGUE_OPEN_MS/1000

  'beteiligte Einheiten
  att& = channels(0).combat.attacker
  def& = channels(0).combat.defender
  attunittp& = channels(0).units(att&).unittype
  defunittp& = channels(0).units(def&).unittype
  attowner& = channels(0).units(att&).owner
  defowner& = channels(0).units(def&).owner

  'Fenster zeichnen (Hintergrund)
  D2D.GraphicStretch(hDialog&, txarea_combat1.left, txarea_combat1.top, txarea_combat1.right, txarea_combat1.bottom, activedialoguearea.left, activedialoguearea.top, activedialoguearea.right, activedialoguearea.bottom)
  D2D.GraphicStretch(hPanels&, 320, attowner&*100, 640, attowner&*100+100, activedialoguearea.left+181*uiscale!, activedialoguearea.top+28*uiscale!, activedialoguearea.left+501*uiscale!, activedialoguearea.top+128*uiscale!)
  D2D.GraphicStretch(hPanels&, 640, defowner&*100, 960, defowner&*100+100, activedialoguearea.left+521*uiscale!, activedialoguearea.top+28*uiscale!, activedialoguearea.left+841*uiscale!, activedialoguearea.top+128*uiscale!)

  'Fenster zeichnen (Einheiten)
  destx! = activedialoguearea.left+28*uiscale!  'linke obere Ecke des Angreiferbilds
  desty! = activedialoguearea.top+28*uiscale!
  IF unitAnimFrameWidth& = 0 THEN
    D2D.GraphicStretch(channelsnosave(0).unitclasses(attunittp&).artworkhandle, 0, 0, 800, 600, destx!, desty!, destx!+133*uiscale!, desty!+100*uiscale!)
    D2D.GraphicStretch(channelsnosave(0).unitclasses(defunittp&).artworkhandle, 0, 0, 800, 600, destx!+833*uiscale!, desty!, destx!+966*uiscale!, desty!+100*uiscale!)
  ELSE
    CALL DrawUnitAnimation(attunittp&, destx!, desty!, destx!+133*uiscale!, desty!+100*uiscale!)
    CALL DrawUnitAnimation(defunittp&, destx!+833*uiscale!, desty!, destx!+966*uiscale!, desty!+100*uiscale!)
  END IF

  'Fenster zeichnen (Kampfwerte)
  destx! = activedialoguearea.left+181*uiscale!  'linke obere Ecke der Angreiferwerte
  desty! = activedialoguearea.top+28*uiscale!
  FOR k& = 0 TO 1
    FOR i& = 0 TO 2
      combatval! = channels(0).combat.params(i&, k&)*0.12
      srcx! = i&*30+t!*10
      srcy! = i&*20+t!*30
      IF k& = 0 THEN
        D2D.GraphicStretch(hHudElements&, srcx!, srcy!, srcx!+combatval!, srcy!+9, destx!+36*uiscale!, desty!+(11+i&*24)*uiscale!, destx!+(36+combatval!)*uiscale!, desty!+(25+i&*24)*uiscale!)
      ELSE
        D2D.GraphicStretch(hHudElements&, srcx!, srcy!, srcx!+combatval!, srcy!+9, destx!+(624-combatval!)*uiscale!, desty!+(11+i&*24)*uiscale!, destx!+624*uiscale!, desty!+(25+i&*24)*uiscale!)
      END IF
    NEXT i&
    'Gesamtwert
    x& = channels(0).combat.params(4, k&)
    a$ = FORMAT$(x&)
    D2D.GraphicTextSize(a$, hCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrint(a$, destx!+IIF&(k& = 0, 270, 390)*uiscale!-textwd&/2, desty!+50*uiscale!-texthg&/2, brushWhite&, hCaptionFont&)
  NEXT k&

  'Fenster zeichnen (Erfahrungspunkte, Panzerung)
  xp& = channels(0).units(att&).experience-1
  D2D.GraphicStretch(hHudElements&, 320, xp&*64, 384, xp&*64+64, destx!+158*uiscale!, desty!+18*uiscale!, destx!+222*uiscale!, desty!+82*uiscale!)
  xp& = channels(0).units(def&).experience-1
  D2D.GraphicStretch(hHudElements&, 320, xp&*64, 384, xp&*64+64, destx!+438*uiscale!, desty!+18*uiscale!, destx!+502*uiscale!, desty!+82*uiscale!)

  a$ = FORMAT$(channelsnosave(0).unitclasses(attunittp&).armor)
  D2D.GraphicTextSize(a$, hSmallWeaponFont&, textwd&, texthg&)
  D2D.GraphicPrint(a$, destx!+223*uiscale!-textwd&/2, desty!+90*uiscale!-texthg&/2, brushWhite&, hSmallWeaponFont&)
  a$ = FORMAT$(channelsnosave(0).unitclasses(defunittp&).armor)
  D2D.GraphicTextSize(a$, hSmallWeaponFont&, textwd&, texthg&)
  D2D.GraphicPrint(a$, destx!+451*uiscale!-textwd&/2, desty!+90*uiscale!-texthg&/2, brushWhite&, hSmallWeaponFont&)

  'Fenster zeichnen (Lebenspunkte)
  FOR k& = 0 TO 1
    maxhp& = channelsnosave(0).unitclasses(IIF&(k& = 0, attunittp&, defunittp&)).groupsize
    hp& = channels(0).units(IIF&(k& = 0, att&, def&)).groupsize
    IF t! < 0.5 THEN hp& = INT(hp&*(1.0-t!*2)+(hp&-channels(0).combat.params(5, k&))*t!*2)
    IF t! >= 0.5 THEN hp& = hp&-channels(0).combat.params(5, k&)
    FOR i& = 0 TO maxhp&-1
      IF i& < hp& THEN srcimg& = IIF&(k& = 0, attowner&, defowner&) ELSE srcimg& = 7
      D2D.GraphicStretch(hHudElements&, srcimg&*18, 743, srcimg&*18+18, 761, destx!+(IIF&(k& = 0, 2, 478)+i&*18)*uiscale!, desty!+80*uiscale!, destx!+(IIF&(k& = 0, 20, 496)+i&*18)*uiscale!, desty!+98*uiscale!)
    NEXT i&
  NEXT k&

  'MILOP Header
  headerWidth! = (txarea_milopheader.right-txarea_milopheader.left)*uiscale!
  destx! = activedialoguearea.left+(activedialoguearea.right-activedialoguearea.left-headerWidth!)/2
  desty! = activedialoguearea.top-32*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_milopheader.left, txarea_milopheader.top, txarea_milopheader.right, txarea_milopheader.bottom, destx!, desty!, destx!+headerWidth!, desty!+(txarea_milopheader.bottom-txarea_milopheader.top)*uiscale!)

  'Sound-Effekte
  IF (combatSoundEffects& AND 1) = 0 AND t! > 0.1 THEN
    combatSoundEffects& = combatSoundEffects& OR 1
    CALL PlaySoundEffect(hFirstEffect&+channelsnosave(0).unitclasses(attunittp&).sfxfire, %SOUNDBUFFER_EFFECT2, %PLAYFLAGS_LOOPING)
  END IF
  IF (combatSoundEffects& AND 2) = 0 AND t! > 0.6 AND channels(0).combat.params(4, 1) > 0 THEN
    combatSoundEffects& = combatSoundEffects& OR 2
    CALL PlaySoundEffect(hFirstEffect&+channelsnosave(0).unitclasses(defunittp&).sfxfire, %SOUNDBUFFER_EFFECT3, %PLAYFLAGS_LOOPING)
  END IF

  'Kampf beenden sobald Sequenz vorbei ist
  IF t! >= 1.5 THEN
    soundchannels(%SOUNDBUFFER_EFFECT2).Stop
    soundchannels(%SOUNDBUFFER_EFFECT3).Stop
    combatStartTime! = gametime!
    dialogueClosing& = 1
    buttonClose.Visible = 0
  END IF
END SUB



'Shop darstellen
SUB RenderShop(shopnr&)
  LOCAL t!, wd!, hg!, destx!, desty!, headerWidth!
  LOCAL i&, x&, y&, shoptype&, shopowner&, animstep&, unittp&, unitnr&, rownr&, colnr&, textwd&, texthg&
  LOCAL totalenergy&, totalmat&, costenergy&, costmat&
  LOCAL srcx0&, srcy0&, srcx1&, srcy1&
  LOCAL a$

  t! = gametime!-shopSelectionTime!-%DIALOGUE_OPEN_MS/1000
  IF t! <= 0 THEN EXIT SUB

  'Fenster zeichnen (Hintergrund)
  IF selectedShopProd$ = "" THEN
    D2D.GraphicStretch(hDialog&, txarea_shop1.left, txarea_shop1.top, txarea_shop1.right, txarea_shop1.bottom, activedialoguearea.left, activedialoguearea.top, activedialoguearea.right, activedialoguearea.bottom)
  ELSE
    D2D.GraphicStretch(hDialog&, txarea_shop2.left, txarea_shop2.top, txarea_shop2.right, txarea_shop2.bottom, activedialoguearea.left, activedialoguearea.top, activedialoguearea.right, activedialoguearea.bottom)
  END IF

  'Hintergrund in Farbe des Besitzers einfärben
  shopowner& = channels(0).shops(shopnr&).owner
  destx! = activedialoguearea.left+28*uiscale!  'linke obere Ecke des Hintergrunds
  desty! = activedialoguearea.top+28*uiscale!
  IF shopowner& >= 0 AND shopowner& < 6 THEN
    IF selectedShopProd$ = "" THEN
      D2D.GraphicStretch(hDialog&, txarea_playercolors.left, txarea_playercolors.top+shopowner&*50, txarea_playercolors.left+50, txarea_playercolors.top+shopowner&*50+50, destx!, desty!, destx!+747*uiscale!, desty!+364*uiscale!)
    ELSE
      D2D.GraphicStretch(hDialog&, txarea_playercolors.left, txarea_playercolors.top+shopowner&*50, txarea_playercolors.left+50, txarea_playercolors.top+shopowner&*50+50, destx!, desty!, destx!+363*uiscale!, desty!+364*uiscale!)
      D2D.GraphicStretch(hDialog&, txarea_playercolors.left, txarea_playercolors.top+shopowner&*50, txarea_playercolors.left+50, txarea_playercolors.top+shopowner&*50+50, destx!+383*uiscale!, desty!, destx!+747*uiscale!, desty!+364*uiscale!)
    END IF
  END IF

  'Gebäudebild anzeigen (unteren Teil)
  shoptype& = LOG2(channels(0).shops(shopnr&).shoptype)
  IF shoptype& > 6 THEN shoptype& = 7
  x& = (shoptype& MOD 3)*%BUILDINGS_WIDTH
  y& = INT(shoptype&/3)*%BUILDINGS_HEIGHT
  destx! = activedialoguearea.left+68*uiscale!  'linke obere Ecke des Gebäudebilds
  desty! = activedialoguearea.top+412*uiscale!
  D2D.GraphicStretch(hBuildings&, x&, y&+%BUILDINGS_WIDTH/6, x&+%BUILDINGS_WIDTH, y&+%BUILDINGS_HEIGHT, destx!, desty!, destx!+320*uiscale!, desty!+200*uiscale!)

  'Energie und Material anzeigen
  destx! = activedialoguearea.right-51*uiscale!  'linke obere Ecke des Energie Seitenbereichs
  desty! = activedialoguearea.top+56*uiscale!
  totalenergy& = channels(0).player(shopowner&).energy
  totalmat& = channels(0).shops(shopnr&).material
  D2D.GraphicStretch(hDialog&, txarea_shopenergy.left, txarea_shopenergy.top, txarea_shopenergy.right, txarea_shopenergy.bottom, _
    destx!, desty!, destx!+(txarea_shopenergy.right-txarea_shopenergy.left)*uiscale, desty!+(txarea_shopenergy.bottom-txarea_shopenergy.top)*uiscale)
  D2D.GraphicPrint(FORMAT$(totalenergy&), destx!+70*uiscale!, desty!+68*uiscale!, brushWhite&, hCaptionFont&)
  D2D.GraphicPrint(FORMAT$(totalmat&), destx!+70*uiscale!, desty!+118*uiscale!, brushWhite&, hCaptionFont&)
  D2D.GraphicPrint("+"+FORMAT$(channels(0).shops(shopnr&).eplus), destx!+70*uiscale!, desty!+168*uiscale!, brushGold&, hCaptionFont&)
  D2D.GraphicPrint("+"+FORMAT$(channels(0).shops(shopnr&).mplus), destx!+70*uiscale!, desty!+218*uiscale!, brushGold&, hCaptionFont&)

  'Produktionsmenü anzeigen
  IF selectedShopProd$ <> "" THEN
    destx! = activedialoguearea.left+51*uiscale!  'linke obere Ecke des ersten Produktionsslots
    desty! = activedialoguearea.top+51*uiscale!
    x& = 0
    y& = 0
    FOR i& = 1 TO 16
      'Slot hervorheben, falls dieser ausgewählt wurde
      IF i& = shopCursorPos&-15 THEN
        D2D.GraphicStretch(hDialog&, txarea_prodslot.left, txarea_prodslot.top, txarea_prodslot.right, txarea_prodslot.bottom, destx!+(x&*82-3)*uiscale!, desty!+(y&*82-3)*uiscale!, destx!+(x&*82+75)*uiscale!, desty!+(y&*82+75)*uiscale!)
      END IF
      IF i& <= LEN(selectedShopProd$) THEN
        unittp& = ASC(selectedShopProd$, i&)
        costenergy& = channelsnosave(0).unitclasses(unittp&).costenergy
        costmat& = channelsnosave(0).unitclasses(unittp&).costmaterial
        'Einheiten-Icon
        rownr& = INT(unittp&*6/40)
        colnr& = unittp&*6-rownr&*40
        D2D.GraphicStretch(hUnits&(shopowner&), colnr&*24, rownr&*24, colnr&*24+24, rownr&*24+24, destx!+(12+x&*82)*uiscale!, desty!+(12+y&*82)*uiscale!, destx!+(60+x&*82)*uiscale!, desty!+(60+y&*82)*uiscale!)
        IF costenergy& > totalenergy& OR costmat& > totalmat& THEN
          D2D.GraphicStretch(hUnits&(LEN(playerColors$)), colnr&*24, rownr&*24, colnr&*24+24, rownr&*24+24, destx!+(12+x&*82)*uiscale!, desty!+(12+y&*82)*uiscale!, destx!+(60+x&*82)*uiscale!, desty!+(60+y&*82)*uiscale!)
        END IF
        'Energiekosten
        D2D.GraphicStretch(hDialog&, txarea_energyicon.left, txarea_energyicon.top, txarea_energyicon.right, txarea_energyicon.bottom, destx!+(4+x&*82)*uiscale!, desty!+(22+y&*82)*uiscale!, destx!+(20+x&*82)*uiscale!, desty!+(38+y&*82)*uiscale!)
        a$ = FORMAT$(costenergy&)
        D2D.GraphicTextSize(a$, hSmallWeaponFont&, textwd&, texthg&)
        D2D.GraphicPrint(a$, destx!+(12+x&*82)*uiscale!-textwd&/2, desty!+(38+y&*82)*uiscale!, brushWhite&, hSmallWeaponFont&)
        'Materialkosten
        D2D.GraphicStretch(hDialog&, txarea_materialicon.left, txarea_materialicon.top, txarea_materialicon.right, txarea_materialicon.bottom, destx!+(52+x&*82)*uiscale!, desty!+(22+y&*82)*uiscale!, destx!+(68+x&*82)*uiscale!, desty!+(38+y&*82)*uiscale!)
        a$ = FORMAT$(costmat&)
        D2D.GraphicTextSize(a$, hSmallWeaponFont&, textwd&, texthg&)
        D2D.GraphicPrint(a$, destx!+(60+x&*82)*uiscale!-textwd&/2, desty!+(38+y&*82)*uiscale!, brushWhite&, hSmallWeaponFont&)
      END IF
      'nächster Slot
      x& = x&+1
      IF x& = 4 THEN
        x& = 0
        y& = y&+1
      END IF
    NEXT i&
  END IF

  'Inhalt anzeigen
  destx! = activedialoguearea.left+IIF&(selectedShopProd$ = "", 242, 434)*uiscale!  'linke obere Ecke des ersten Inhaltsslots
  desty! = activedialoguearea.top+51*uiscale!
  x& = 0
  y& = 0
  FOR i& = 0 TO 15
    unitnr& = channels(0).shops(shopnr&).content(i&)
    IF i& = shopCursorPos& THEN CALL HighlightShopUnit(destx!+x&*82*uiscale!, desty!+y&*82*uiscale!, destx!+(x&*82+72)*uiscale!, desty!+(y&*82+72)*uiscale!)
    IF unitnr& >= 0 THEN
      unittp& = channels(0).units(unitnr&).unittype
      'Einheiten-Icon
      rownr& = INT(unittp&*6/40)
      colnr& = unittp&*6-rownr&*40
      D2D.GraphicStretch(hUnits&(shopowner&), colnr&*24, rownr&*24, colnr&*24+24, rownr&*24+24, destx!+(12+x&*82)*uiscale!, desty!+(12+y&*82)*uiscale!, destx!+(60+x&*82)*uiscale!, desty!+(60+y&*82)*uiscale!)
      IF (channels(0).units(unitnr&).flags AND %US_DONE) <> 0 THEN
        D2D.GraphicStretch(hUnits&(LEN(playerColors$)), colnr&*24, rownr&*24, colnr&*24+24, rownr&*24+24, destx!+(12+x&*82)*uiscale!, desty!+(12+y&*82)*uiscale!, destx!+(60+x&*82)*uiscale!, desty!+(60+y&*82)*uiscale!)
      END IF
      'Shop-Aktion-Animation (Reparieren, Befüllen, ...)
      IF unitnr& = shopAnimationUnit& AND gametime! >= shopAnimationTime! AND gametime! < shopAnimationTime!+0.5 THEN
        animstep& = INT((gametime!-shopAnimationTime!)*8)
        CALL GetMapOverlaySrcRect(shopAnimationType&, animstep&, srcx0&, srcy0&, srcx1&, srcy1&)
        D2D.GraphicStretch(hHudElements&, srcx0&, srcy0&, srcx1&, srcy1&, destx!+(4+x&*82)*uiscale!, desty!+(4+y&*82)*uiscale!, destx!+(68+x&*82)*uiscale!, desty!+(68+y&*82)*uiscale!)
      END IF
    END IF
    'nächster Slot
    x& = x&+1
    IF x& = 4 THEN
      x& = 0
      y& = y&+1
    END IF
  NEXT i&

  'Shopnamen im Header anzeigen
  headerWidth! = (txarea_blankheader.right-txarea_blankheader.left)*uiscale!
  destx! = activedialoguearea.left+(activedialoguearea.right-activedialoguearea.left-headerWidth!)/2
  desty! = activedialoguearea.top-32*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_blankheader.left, txarea_blankheader.top, txarea_blankheader.right, txarea_blankheader.bottom, destx!, desty!, destx!+headerWidth!, desty!+(txarea_blankheader.bottom-txarea_blankheader.top)*uiscale!)
  a$ = channels(0).info.shopnames(shopnr&)
  D2D.GraphicTextSize(a$, hShopCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrint(a$, destx!+(headerWidth!-textwd&)/2, desty!+45*uiscale!-texthg&/2, brushWhite&, hShopCaptionFont&)

  'Buttons
  destx! = activedialoguearea.left+483*uiscale!  'linke obere Ecke des ersten Buttons
  desty! = activedialoguearea.top+415*uiscale!
  buttonShopMove.XPos = destx!
  buttonShopMove.YPos = desty!
  buttonShopMove.Visible = 1
  buttonShopRefuel.XPos = destx!+56*uiscale!
  buttonShopRefuel.YPos = desty!
  buttonShopRefuel.Visible = 1
  buttonShopRepair.XPos = destx!+112*uiscale!
  buttonShopRepair.YPos = desty!
  buttonShopRepair.Visible = 1
  buttonShopBuild.XPos = destx!+168*uiscale!
  buttonShopBuild.YPos = desty!
  buttonShopBuild.Visible = 1
  buttonShopTrain.XPos = destx!+224*uiscale!
  buttonShopTrain.YPos = desty!
  buttonShopTrain.Visible = 1
END SUB



'Menü darstellen
SUB RenderMenu
  LOCAL a$$, textbeforeicon$$, t!, y!, starwidth!, starheight!, xp&
  LOCAL textwdbeforeicon&, textwd&, texthg&, i&, b&, n&, p&, iconx&, icony&, fullheight&, highlightedentry&, unittp&
  LOCAL menuwidth&, itemwidth&, itemheight&
  LOCAL srcarea AS RECT, headerarea AS RECT

  t! = gametime!-menuOpenTime!

  IF t! <= 0 THEN EXIT SUB

'  D2D.GraphicBox(0, 0, 400, 200, brushWhite&, brushWhite&)

  highlightedentry& = highlightedMenuEntry&
  menuwidth& = activedialoguearea.right-activedialoguearea.left
  itemwidth& = (txarea_menuitem.right-txarea_menuitem.left)*uiscale!
  itemheight& = (txarea_menuitem.bottom-txarea_menuitem.top)*uiscale!

  'Fenster zeichnen (Hintergrund)
  D2D.GraphicStretch(hDialog&, txarea_menu.left, txarea_menu.top, txarea_menu.right, txarea_menu.bottom, activedialoguearea.left, activedialoguearea.top, activedialoguearea.right, activedialoguearea.bottom)

  'bei Einheitenmenü Bild der Einheit anzeigen
  IF GetPhase&(0, localPlayerNr&) <> %PHASE_MAINMENU AND channels(0).player(localPlayerNr&).selectedunit >= 0 THEN
    headerarea.left = activedialoguearea.left+87*uiscale!
    headerarea.top = activedialoguearea.bottom-50*uiscale!-(menuwidth&-174*uiscale!)*0.75
    headerarea.right = activedialoguearea.right-87*uiscale!
    headerarea.bottom = activedialoguearea.bottom-50*uiscale!
    unittp& = channels(0).units(channels(0).player(localPlayerNr&).selectedunit).unittype
    IF unitAnimFrameWidth& = 0 THEN
      D2D.GraphicStretch(channelsnosave(0).unitclasses(unittp&).artworkhandle, 0, 0, 800, 600, headerarea.left, headerarea.top, headerarea.right, headerarea.bottom)
    ELSE
      CALL DrawUnitAnimation(unittp&, headerarea.left, headerarea.top, headerarea.right, headerarea.bottom)
    END IF
    D2D.GraphicStretch(hHudElements&, 513, 0, 1059, 410, headerarea.left, headerarea.top, headerarea.right, headerarea.bottom)
  END IF

  'Footer
  headerarea.left = activedialoguearea.left+(menuwidth&-(txarea_blankfooter.right-txarea_blankfooter.left)*uiscale!)/2
  headerarea.right = activedialoguearea.right-(menuwidth&-(txarea_blankfooter.right-txarea_blankfooter.left)*uiscale!)/2
  headerarea.top = activedialoguearea.bottom-70*uiscale!
  headerarea.bottom = headerarea.top+(txarea_blankfooter.bottom-txarea_blankfooter.top)*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_blankfooter.left, txarea_blankfooter.top, txarea_blankfooter.right, txarea_blankfooter.bottom, headerarea.left, headerarea.top, headerarea.right, headerarea.bottom)
  IF menuCaption$$ <> "" THEN
    'Überschrift
    D2D.GraphicTextSizeW(menuCaption$$, hCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(menuCaption$$, (headerarea.left+headerarea.right-textwd&)/2, headerarea.top+52*uiscale!-texthg&/2, brushWhite&, hCaptionFont&)
  ELSE
    'Spielername und Erfahrung
    starwidth! = (txarea_starborder.right-txarea_starborder.left)*uiscale!
    starheight! = (txarea_starborder.bottom-txarea_starborder.top)*uiscale!
    xp& = MIN&(120, localPlayerXP&)
    IF xp& >= 96 THEN xp& = xp&+26
    IF xp& >= 72 THEN xp& = xp&+26
    IF xp& >= 48 THEN xp& = xp&+26
    IF xp& >= 24 THEN xp& = xp&+26
    D2D.GraphicStretch(hDialog&, txarea_stars.left, txarea_stars.top, txarea_stars.left+xp&+4, txarea_stars.bottom, _
      activedialoguearea.left+(menuwidth&-starwidth!)/2, headerarea.top+16*uiscale!, activedialoguearea.left+(menuwidth&-starwidth!)/2+(xp&+4)*uiscale!, headerarea.top+16*uiscale!+starheight!)
    D2D.GraphicStretch(hDialog&, txarea_starborder.left, txarea_starborder.top, txarea_starborder.right, txarea_starborder.bottom, _
      activedialoguearea.left+(menuwidth&-starwidth!)/2, headerarea.top+16*uiscale!, activedialoguearea.right-(menuwidth&-starwidth!)/2, headerarea.top+16*uiscale!+starheight!)
    D2D.GraphicTextSizeW(localPlayerName$, hCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(localPlayerName$, (headerarea.left+headerarea.right-textwd&)/2, headerarea.top+60*uiscale!-texthg&/2, brushGold&, hCaptionFont&)
  END IF

  'Einträge anzeigen
  FOR i& = 0 TO menuCount&-1
    'Rahmen
    menuItemAreas(i&).left = activedialoguearea.left+(menuwidth&-itemwidth&)/2
    menuItemAreas(i&).right = activedialoguearea.right-(menuwidth&-itemwidth&)/2
    menuItemAreas(i&).top = activedialoguearea.top+(100+i&*70)*uiscale!
    menuItemAreas(i&).bottom = menuItemAreas(i&).top+itemheight&
    IF mousexpos& > menuItemAreas(i&).left AND mousexpos& < menuItemAreas(i&).right AND mouseypos& > menuItemAreas(i&).top AND mouseypos& < menuItemAreas(i&).bottom THEN highlightedentry& = i&
    IF i& = highlightedentry& THEN srcarea = txarea_menuitemhighlight ELSE srcarea = txarea_menuitem
    D2D.GraphicStretch(hDialog&, srcarea.left, srcarea.top, srcarea.right, srcarea.bottom, menuItemAreas(i&).left, menuItemAreas(i&).top, menuItemAreas(i&).right, menuItemAreas(i&).bottom)

    'Beschriftung
    a$$ = menuEntries$$(i&)
    n& = LEN(a$$)
    DO
      D2D.GraphicTextSizeW(LEFT$(a$$, n&), hMenuFont&, textwd&, texthg&)
      IF textwd& > itemwidth&-40*uiscale! THEN
        n& = n&-1
      ELSE
        EXIT LOOP
      END IF
    LOOP
    IF n& < LEN(menuEntries$$(i&)) THEN
      a$$ = LEFT$(a$$, n&-1)+"..."
      menuEntries$$(i&) = a$$
    END IF
    a$$ = menuEntries$$(i&)
    IF MID$(a$$, 2, 3) = "   " THEN
      'Hotkey
      D2D.GraphicPrintW(LEFT$(a$$, 1), menuItemAreas(i&).left+20*uiscale!, menuItemAreas(i&).top+(itemheight&-texthg&)/2, brushRed&, hMenuFont&)
      a$$ = MID$(a$$, 5)
    END IF
    p& = INSTR(a$$, CHR$(1))
    IF p& > 0 THEN
      'Text und Icon darstellen
      iconx& = VAL(MID$(a$$, p&+1, 4))
      icony& = VAL(MID$(a$$, p&+5, 4))
      textbeforeicon$$ = LEFT$(a$$, p&-1)
      a$$ = MID$(a$$, p&+9)
      D2D.GraphicTextSizeW(textbeforeicon$$, hMenuFont&, textwdbeforeicon&, texthg&)
      D2D.GraphicTextSizeW(textbeforeicon$$+a$$, hMenuFont&, textwd&, texthg&)
      textwd& = textwd&+32  'Darstellungsbreite des Icons
      y! = menuItemAreas(i&).top+(itemheight&-texthg&)/2
      D2D.GraphicPrintW(textbeforeicon$$, menuItemAreas(i&).left+(itemwidth&-textwd&)/2, y!, brushWhite&, hMenuFont&)
      D2D.GraphicStretch(hHudElements&, iconx&, icony&, iconx&+16, icony&+16, _
        menuItemAreas(i&).left+(itemwidth&-textwd&)/2+textwdbeforeicon&, menuItemAreas(i&).top+itemheight&/2-12, menuItemAreas(i&).left+(itemwidth&-textwd&)/2+textwdbeforeicon&+24, menuItemAreas(i&).top+itemheight&/2+12)
      D2D.GraphicPrintW(a$$, menuItemAreas(i&).left+(itemwidth&-textwd&)/2+textwdbeforeicon&+32, y!, brushWhite&, hMenuFont&)
    ELSE
      'nur Text darstellen
      D2D.GraphicTextSizeW(a$$, hMenuFont&, textwd&, texthg&)
      D2D.GraphicPrintW(a$$, menuItemAreas(i&).left+(itemwidth&-textwd&)/2, menuItemAreas(i&).top+(itemheight&-texthg&)/2, brushWhite&, hMenuFont&)
    END IF
  NEXT i&

  'MILOP Header
  headerarea.left = activedialoguearea.left+(menuwidth&-(txarea_milopheader.right-txarea_milopheader.left)*uiscale!)/2
  headerarea.right = activedialoguearea.right-(menuwidth&-(txarea_milopheader.right-txarea_milopheader.left)*uiscale!)/2
  headerarea.top = activedialoguearea.top-32*uiscale!
  headerarea.bottom = headerarea.top+(txarea_milopheader.bottom-txarea_milopheader.top)*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_milopheader.left, txarea_milopheader.top, txarea_milopheader.right, txarea_milopheader.bottom, headerarea.left, headerarea.top, headerarea.right, headerarea.bottom)

  'Sound-Effekt spielen
  IF highlightedentry& >= 0 AND highlightedentry& <> lastHighlightedMenuEntry& THEN
    lastHighlightedMenuEntry& = highlightedentry&
    CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_RING3, %SOUNDBUFFER_EFFECT1, %PLAYFLAGS_NONE)
  END IF
END SUB



'Ermittelt den Absender einer Spielnachricht aus dessen Video
FUNCTION GetGameMessageSender$(videoseq&)
  LOCAL s$

  SELECT CASE videoseq&
  CASE 26: s$ = "Mirk Shano"
  CASE 27: s$ = "Tassi ad Ano"
  CASE 28: s$ = "Kastaia be Con"
  CASE 29: s$ = "Mul Tendo"
  CASE 30: s$ = "Beg Beb"
  CASE 31: s$ = "Tara be Destaan"
  CASE 32: s$ = "Toval ad Sirlon"
  CASE 33: s$ = "Moro ad Odan"
  END SELECT

  GetGameMessageSender$ = s$
END FUNCTION



'Ermittelt den Spielernamen des ersten Verbündeten
FUNCTION GetTeamMateName$(chnr&, plnr&)
  LOCAL i&, allymask&, allyname$

  allymask& = channels(chnr&).player(plnr&).allymask
  FOR i& = 0 TO %MAXPLAYERS-1
    IF i& <> plnr& AND (allymask& AND 2^i&) <> 0 THEN
      allyname$ = playernames$(i&)
      IF allyname$ = "" THEN allyname$ = defaultPlayernames$(i&)
      GetTeamMateName$ = allyname$
      EXIT FUNCTION
    END IF
  NEXT i&

  GetTeamMateName$ = ""
END FUNCTION



'Ermittelt die Animations-Sequenz zu einem Zeitpunkt
FUNCTION GetAnimationSequence&(BYREF steps() AS TAnimationScript, BYVAL msgid&, BYVAL t!, BYVAL tp&)
  LOCAL p&, i&, timestamp&, nsteps&, script$
  LOCAL animstep AS TAnimationScript

  'Animations-Skript laden
  IF msgid& > UBOUND(animationsScripts$) THEN EXIT FUNCTION
  script$ = animationsScripts$(msgid&)

  'alle Einträge mit dem richtigen Typ suchen, die zum jetzigen Zeitpunkt gültig sind
  REDIM steps(9)
  timestamp& = t!*%BI2_ANIMATION_FPS  'Umrechnung von BI2 Zeiteinheiten auf Systemzeit
  p& = 1
  WHILE p& <= LEN(script$)
    POKE$ VARPTR(animstep), MID$(script$, p&, SIZEOF(TAnimationScript))
    IF animstep.animtype = tp& AND timestamp& >= animstep.starttime AND timestamp& < animstep.starttime+animstep.duration+animstep.freezetime THEN
      'relevanten Eintrag gefunden
      IF tp& = %MSGANI_VID THEN
        IF timestamp& < animstep.starttime+animstep.duration THEN
          IF animstep.sequence = 325 THEN
            'Gebäude-Einnehmen Sequenz nicht wiederholen sondern letzten Frame einfrieren
            animstep.frame = MIN&(INT(t!*animstep.animationspeed), animationFrameCount&(animstep.sequence)-1)
          ELSE
            animstep.frame = INT(t!*animstep.animationspeed) MOD animationFrameCount&(animstep.sequence)
          END IF
        ELSE
          animstep.frame = 0
        END IF
      END IF
      'Animationsschritt sortiert nach Z-Position in Ergebnis-Liste einfügen
      FOR i& = 0 TO nsteps&-1
        IF steps(i&).zpos > animstep.zpos THEN EXIT FOR
      NEXT i&
      ARRAY INSERT steps(i&), animstep
      nsteps& = nsteps&+1
      IF nsteps& = 10 THEN EXIT LOOP
    END IF
    p& = p&+SIZEOF(TAnimationScript)
  WEND

  GetAnimationSequence& = nsteps&
END FUNCTION



'Video-Nachricht darstellen
SUB RenderVideoMessage
  LOCAL tmillisecs&, currentFrame&, pixeldata$
  LOCAL audioStartPosition&, audiodata$
  LOCAL destx!, desty!, headerWidth!, a$$

  'darzustellenden Frame ermitteln
  tmillisecs& = (gametime!-messageOpenTime!) * 1000 - %DIALOGUE_OPEN_MS
  IF tmillisecs& <= 0 THEN EXIT SUB
  currentFrame& = GetAVIVideoFrameNumberForMillisecond&(tmillisecs&)
  IF currentFrame& < 0 OR currentFrame& >= videoFrameCount& THEN
    dialogueClosing& = 1
    messageOpenTime! = gametime!
    CALL CloseGameMessage
    EXIT SUB
  END IF

  'Frame ggf. laden und Textur dafür erstellen
  IF currentFrame& <> currentVideoFrame& THEN
    pixeldata$ = GetAVIFramePixelData$(currentFrame&)
    IF LEN(pixeldata$) = 0 THEN pixeldata$ = STRING$(videoFrameWidth&*videoFrameHeight&*4, 0)
    IF hVideoFrame& = 0 THEN
      hVideoFrame& = D2D.CreateMemoryBitmap(videoFrameWidth&, videoFrameHeight&, pixeldata$)
    ELSE
      D2D.ReuseMemoryBitmap(hVideoFrame&, videoFrameWidth&, videoFrameHeight&, pixeldata$)
    END IF
    currentVideoFrame& = currentFrame&
  END IF

  'nächste Audio-Sample abspielen
  IF soundchannels(%SOUNDBUFFER_VIDEO).IsPlaying = 0 OR gametime! >= videoSoundTrackUpdateTime!+1 THEN
    audioStartPosition& = tmillisecs&*audioSamplesPerSecond&/1000
    audiodata$ = MID$(audioStreamData$, audioStartPosition&*2+1, audioSamplesPerSecond&*2)  '1 Sekunde Audio-Daten
    IF hVideoSoundTrack& = 0 THEN
      hVideoSoundTrack& = DS.AddWaveData(audiodata$, audioSamplesPerSecond&, 1)
    ELSE
      DS.SetWaveData(hVideoSoundTrack&, audiodata$, audioSamplesPerSecond&, 1)
    END IF
    CALL PlaySoundEffect(hVideoSoundTrack&, %SOUNDBUFFER_VIDEO, %PLAYFLAGS_NONE)
    videoSoundTrackUpdateTime! = gametime!
  END IF

  'Fenster zeichnen (Hintergrund)
  D2D.GraphicStretch(hDialog2&, txarea_videomsg.left, txarea_videomsg.top, txarea_videomsg.right, txarea_videomsg.bottom, activedialoguearea.left, activedialoguearea.top, activedialoguearea.right, activedialoguearea.bottom)

  'Frame darstellen
  destx! = activedialoguearea.left+230*uiscale!
  desty! = activedialoguearea.top+30*uiscale!
  D2D.GraphicStretch(hVideoFrame&, 2, 2, videoFrameWidth&-2, videoFrameHeight&-2, destx!, desty!, destx!+videoFrameWidth&*2*uiscale!-4*uiscale!, desty!+videoFrameHeight&*2*uiscale!-4*uiscale!)

  'Meta-Informationen darstellen
  destx! = activedialoguearea.left+38*uiscale!
  desty! = activedialoguearea.top+54*uiscale!
  D2D.GraphicPrint("MSGID "+FORMAT$(currentMessageId&), destx!, desty!, brushLightGrey&, hCaptionFont&)
  D2D.GraphicPrint("TIME "+LEFT$(TIME$, 5), destx!, desty!+30, brushLightGrey&, hCaptionFont&)
  a$$ = FORMAT$(INT(tmillisecs&/60000), "00")+":"+FORMAT$(INT(tmillisecs&/1000) MOD 60, "00")+"."+FORMAT$(INT(tmillisecs&/10) MOD 100, "00")
  D2D.GraphicPrint(a$$, destx!, desty!+190*uiscale!, brushLightGrey&, hCaptionFont&)

  'Standbild darstellen
  destx! = activedialoguearea.left+27*uiscale!
  desty! = activedialoguearea.top+376*uiscale!
  D2D.GraphicStretch(hVideoFreezeFrame&, videoFrameWidth&*0.25, videoFrameHeight&*0.25, videoFrameWidth&*0.75, videoFrameHeight&*0.25+videoFrameWidth&/2*108/177, destx!, desty!, destx!+177*uiscale!, desty!+108*uiscale!)

  'MILOP Header
  headerWidth! = (txarea_milopheader.right-txarea_milopheader.left)*uiscale!
  destx! = activedialoguearea.left+(activedialoguearea.right-activedialoguearea.left-headerWidth!)/2+100*uiscale!
  desty! = activedialoguearea.top-32*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_milopheader.left, txarea_milopheader.top, txarea_milopheader.right, txarea_milopheader.bottom, destx!, desty!, destx!+headerWidth!, desty!+(txarea_milopheader.bottom-txarea_milopheader.top)*uiscale!)
END SUB



'Spiel-Nachrichtenfenster darstellen
SUB RenderGameMessage
  LOCAL t!, wd!, hg!, destx!, desty!, closesecondstext! , videozoom!, headerWidth!
  LOCAL textwd&, texthg&, i&, n&, y&, c&, xoffset&, yoffset&, sfx&, videoseq&, videoframe&, videowd&, videohg&, srcx&, srcy&, nsteps&, soundchnr&
  LOCAL a$$, b$$
  LOCAL animsteps() AS TAnimationScript

  IF gameMessageKind& = 0 THEN EXIT SUB
  IF gameMessageKind& = 2 THEN
    CALL RenderVideoMessage
    EXIT SUB
  END IF

  t! = gametime!-messageOpenTime!-%DIALOGUE_OPEN_MS/1000
  IF t! <= 0 THEN EXIT SUB
  IF speechVolume& = 0 OR SAPIWAITUNTILDONE&(1) THEN
    'wenn Sprachausgabe abgeschlossen ist und Text vollständig angezeigt wurde, dann Dialog wieder schließen
    closesecondstext! = 3+LEN(GetGameMessageText(currentTextId&, 0, 0))/%GAMEMESSAGE_SPEED
    IF t! >= closesecondstext! THEN
      dialogueClosing& = 1
      messageOpenTime! = gametime!
      CALL CloseGameMessage
      EXIT SUB
    END IF
  END IF

  'Fenster zeichnen (Hintergrund)
  D2D.GraphicStretch(hDialog&, txarea_msg.left, txarea_msg.top, txarea_msg.right, txarea_msg.bottom, activedialoguearea.left, activedialoguearea.top, activedialoguearea.right, activedialoguearea.bottom)

  'Video darstellen
  videozoom! = 4*uiscale!
  nsteps& = GetAnimationSequence&(animsteps(), currentMessageId&, t!-0.6, %MSGANI_VID)
  FOR i& = 0 TO nsteps&-1
    videoseq& = animsteps(i&).sequence
    videoframe& = animsteps(i&).frame
    videowd& = animationWidth&(videoseq&)
    videohg& = animationHeight&(videoseq&)
    xoffset& = animsteps(i&).xoffset
    yoffset& = animsteps(i&).yoffset
    'Video mit 400% Vergrößerung darstellen
    srcx& = (videoframe& AND 7)*videowd&
    srcy& = INT(videoframe&/8)*videohg&
    destx! = activedialoguearea.left+296*uiscale!
    desty! = activedialoguearea.top+80*uiscale!
    D2D.CreateClippingRegion(destx!, desty!, destx!+480*uiscale!, desty!+256*uiscale!)
    D2D.GraphicStretch(hAnimations&(videoseq&), srcx&, srcy&, srcx&+videowd&, srcy&+videohg&, destx!+xoffset&*videozoom!, desty!+yoffset&*videozoom!, destx!+xoffset&*videozoom!+videowd&*videozoom!, desty!+yoffset&*videozoom!+videohg&*videozoom!)
    D2D.ReleaseClippingRegion

    'Absender der Nachricht aus Video ermitteln
    IF messageSender$ = "" THEN
      messageSender$ = GetGameMessageSender$(videoseq&)
      IF messageSender$ <> "" THEN msgSenderCard& = videoseq&
    END IF
  NEXT i&

  'Sound-Effekte abspielen
  nsteps& = MIN&(3, GetAnimationSequence&(animsteps(), currentMessageId&, t!-0.6, %MSGANI_SFX))
  FOR i& = 0 TO nsteps&-1
    sfx& = animsteps(i&).sequence
    'prüfen, ob dieser Sound-Effekt bereits gespielt wird
    FOR soundchnr& = %SOUNDBUFFER_EFFECT2 TO %SOUNDBUFFER_EFFECT4
      IF soundchannels(soundchnr&).IsPlaying <> 0 AND soundchannels(soundchnr&).SoundNumber = sfx& THEN EXIT FOR
    NEXT soundchnr&
    IF soundchnr& > %SOUNDBUFFER_EFFECT4 THEN
      'freien Sound-Channel suchen
      FOR soundchnr& = %SOUNDBUFFER_EFFECT2 TO %SOUNDBUFFER_EFFECT4
        IF soundchannels(soundchnr&).IsPlaying = 0 THEN EXIT FOR
      NEXT soundchnr&
      IF soundchnr& > %SOUNDBUFFER_EFFECT4 THEN soundchnr& = %SOUNDBUFFER_EFFECT2+i&
      CALL PlaySoundEffect(hFirstEffect&+sfx&, soundchnr&, %PLAYFLAGS_LOOPING)
    END IF
  NEXT i&
  'alle Sound-Channel stoppen, die nicht mehr abgespielt werden sollen
  FOR soundchnr& = %SOUNDBUFFER_EFFECT2 TO %SOUNDBUFFER_EFFECT4
    IF soundchannels(soundchnr&).IsPlaying <> 0 THEN
      FOR i& = 0 TO nsteps&-1
        IF soundchannels(soundchnr&).SoundNumber = animsteps(i&).sequence THEN EXIT FOR
      NEXT i&
      IF i& >= nsteps& THEN soundchannels(soundchnr&).Stop
    END IF
  NEXT soundchnr&

  'Meta-Informationen darstellen
  destx! = activedialoguearea.left+38*uiscale!
  desty! = activedialoguearea.top+72*uiscale!
  D2D.GraphicPrint("MSGID "+FORMAT$(currentMessageId&), destx!, desty!, brushLightGrey&, hCaptionFont&)
  D2D.GraphicPrint("TIME "+LEFT$(TIME$, 5), destx!, desty!+30, brushLightGrey&, hCaptionFont&)
  D2D.GraphicPrint(messageSender$, destx!, desty!+60, brushLightGrey&, hCaptionFont&)
  a$$ = FORMAT$(INT(t!/60), "00")+":"+FORMAT$(INT(t!) MOD 60, "00")+"."+FORMAT$(INT(t!*100) MOD 100, "00")
  D2D.GraphicPrint(a$$, activedialoguearea.left+645*uiscale!, activedialoguearea.top+32*uiscale!, brushLightGrey&, hCaptionFont&)

  'Visitenkarte darstellen
  IF msgSenderCard& >= 0 THEN
    destx! = activedialoguearea.left+28*uiscale!
    desty! = activedialoguearea.top+203*uiscale!
    videowd& = animationWidth&(msgSenderCard&)
    videohg& = animationHeight&(msgSenderCard&)
    D2D.GraphicStretch(hAnimations&(msgSenderCard&), 0, 0, videowd&, videohg&, destx!, desty!, destx!+247*uiscale!, desty!+133*uiscale!)
  END IF

  'Text darstellen
  destx! = activedialoguearea.left+78*uiscale!
  desty! = activedialoguearea.top+360*uiscale!
  D2D.CreateClippingRegion(destx!, desty!, destx!+647*uiscale!, desty!+300*uiscale!)
  xoffset& = 0
  c& = brushWhite&
  a$$ = GetGameMessageText(currentTextId&, 0, 1)
  IF UCASE$(LEFT$(a$$, 4)) = "^VOC" THEN a$$ = MID$(a$$, 6)
  a$$ = LEFT$(a$$, INT(t!*%GAMEMESSAGE_SPEED))
  WHILE a$$ <> ""
    SELECT CASE ASC(a$$)
    CASE 0:  'Steuerzeichen für einen Absatz
      n& = 1
      y& = y&+30
      xoffset& = 0
    CASE 1:  'Textfarbe: weiß
      n& = 1
      c& = brushWhite&
      a$$ = MID$(a$$, 2)
      ITERATE LOOP
    CASE 2:  'Textfarbe: rot
      n& = 1
      c& = brushRed&
      a$$ = MID$(a$$, 2)
      ITERATE LOOP
    CASE 3:  'Name des eroberten Shops
      'wurde bereits vor der Schleife ersetzt
    CASE ELSE  'Text
      n& = INSTR(a$$, ANY CHR$(0,1,2))-1
      IF n& = -1 THEN n& = LEN(a$$)
      DO
        b$$ = LEFT$(a$$, n&)
        D2D.GraphicTextSizeW(b$$, hGameMessageFont&, textwd&, texthg&)
        IF textwd&+xoffset& <= 647*uiscale! THEN EXIT LOOP
        n& = INSTR(-1, b$$, " ")-1
      LOOP UNTIL n& < 1
      IF n& < 1 THEN
        y& = y&+20
        xoffset& = 0
        n& = 0
      ELSE
        IF ASC(a$$, n&+1) = 32 THEN
          n& = n&+1
          b$$ = LEFT$(a$$, n&)
          D2D.GraphicTextSizeW(b$$, hGameMessageFont&, textwd&, texthg&)
        END IF
        D2D.GraphicPrintW(b$$, destx!+xoffset&, desty!+y&-gameMessageScrollY&, c&, hGameMessageFont&)
        xoffset& = xoffset&+textwd&
      END IF
    END SELECT
    a$$ = LTRIM$(MID$(a$$, n&+1))
  WEND
  IF y&-gameMessageScrollY& > 300*uiscale! THEN gameMessageScrollY& = gameMessageScrollY&+1
  D2D.ReleaseClippingRegion

  'MILOP Header
  headerWidth! = (txarea_milopheader.right-txarea_milopheader.left)*uiscale!
  destx! = activedialoguearea.left+(activedialoguearea.right-activedialoguearea.left-headerWidth!)/2
  desty! = activedialoguearea.top-32*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_milopheader.left, txarea_milopheader.top, txarea_milopheader.right, txarea_milopheader.bottom, destx!, desty!, destx!+headerWidth!, desty!+(txarea_milopheader.bottom-txarea_milopheader.top)*uiscale!)
END SUB



'Sieg-Bildschirm darstellen
SUB RenderVictory
  LOCAL a$$, nunits&, i&, unittp&, xp&, totalhours&, totalminutes&, textwd&, texthg&, episode&, nextmission&
  LOCAL destx!, desty!, t!, opacity!, textscale!, headerWidthScaled!, headerHeightScaled!

  D2D.CreateClippingRegion(maparea.left, maparea.top, maparea.right, maparea.bottom)
  D2D.GraphicBox(maparea.left, maparea.top, maparea.right, maparea.bottom, brushBlack&, brushBlack&)
  t! = gametime!-gameoverOpenTime!

  'Sieg
  a$$ = words$$(IIF&(channels(0).info.state = %CHANNELSTATE_VICTORY, %WORD_VICTORY, %WORD_BONUSMISSION))
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, (maparea.left+maparea.right-textwd&)/2+2, maparea.top+80*uiscale!-texthg&/2+2, brushPlayer&(localPlayerNr&), hBigCaptionFont&)
  D2D.GraphicPrintW(a$$, (maparea.left+maparea.right-textwd&)/2, maparea.top+80*uiscale!-texthg&/2, brushWhite&, hBigCaptionFont&)
  desty! = maparea.top+150*uiscale!

  'beste Einheiten ein- und ausblenden
  nunits& = LEN(unitclassesByXp$)/2
  IF nunits& > 0 THEN
    t! = t! MOD (nunits&*2)
    i& = INT(t!/2)
    unittp& = ASC(unitclassesByXp$, i&*2+1)
    xp& = ASC(unitclassesByXp$, i&*2+2)-1
    opacity! = t!-i&*2
    IF opacity! > 1.0 THEN opacity! = 2.0-opacity!
    opacity! = MAX(0.01, opacity!)
    'Einheiten-Bild
    IF unitAnimFrameWidth& = 0 THEN
      D2D.GraphicStretch(channelsnosave(0).unitclasses(unittp&).artworkhandle, 0, 0, 800, 600, (maparea.left+maparea.right)/2-360*uiscale!, desty!+300*uiscale!, (maparea.left+maparea.right)/2+360*uiscale!, desty!+840*uiscale!, opacity!)
    ELSE
      CALL DrawUnitAnimation(unittp&, (maparea.left+maparea.right)/2-360*uiscale!, desty!+300*uiscale!, (maparea.left+maparea.right)/2+360*uiscale!, desty!+840*uiscale!)
    END IF
    'Erfahrungspunkte
    D2D.GraphicStretch(hHudElements&, 321, xp&*64+1, 383, xp&*64+63, (maparea.left+maparea.right)/2-64*uiscale!, desty!+640*uiscale!, (maparea.left+maparea.right)/2+64*uiscale!, desty!+768*uiscale!, opacity!)
  END IF

  'Punkte für Landeinheiten
  a$$ = FORMAT$(scoreGroundUnit&)
  destx! = (maparea.left+maparea.right)/4
  IF unitAnimFrameWidth& = 0 THEN
    D2D.GraphicStretch(channelsnosave(0).unitclasses(11).artworkhandle, 0, 0, 800, 600, destx!-200*uiscale!, desty!, destx!+200*uiscale!, desty!+300*uiscale!)
  ELSE
    CALL DrawUnitAnimation(11, destx!-200*uiscale!, desty!, destx!+200*uiscale!, desty!+300*uiscale!)
  END IF
  D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!+20*uiscale!-texthg&/2, brushWhite&, hShopCaptionFont&)

  'Punkte für Lufteinheiten
  a$$ = FORMAT$(scoreAirUnits&)
  destx! = (maparea.left+maparea.right)/2
  IF unitAnimFrameWidth& = 0 THEN
    D2D.GraphicStretch(channelsnosave(0).unitclasses(36).artworkhandle, 0, 0, 800, 600, destx!-200*uiscale!, desty!, destx!+200*uiscale!, desty!+300*uiscale!)
  ELSE
    CALL DrawUnitAnimation(36, destx!-200*uiscale!, desty!, destx!+200*uiscale!, desty!+300*uiscale!)
  END IF
  D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!+20*uiscale!-texthg&/2, brushWhite&, hShopCaptionFont&)

  'Punkte für Schiffe
  a$$ = FORMAT$(scoreWaterUnits&)
  destx! = (maparea.left+maparea.right)*3/4
  IF unitAnimFrameWidth& = 0 THEN
    D2D.GraphicStretch(channelsnosave(0).unitclasses(49).artworkhandle, 0, 0, 800, 600, destx!-200*uiscale!, desty!, destx!+200*uiscale!, desty!+300*uiscale!)
  ELSE
    CALL DrawUnitAnimation(49, destx!-200*uiscale!, desty!, destx!+200*uiscale!, desty!+300*uiscale!)
  END IF
  D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!+20*uiscale!-texthg&/2, brushWhite&, hShopCaptionFont&)

  IF channels(0).info.nextmission < 0 THEN
    'Gesamtpunkte
    destx! = (maparea.left+maparea.right)/2
    desty! = maparea.top+390*uiscale!
    a$$ = words$$(%WORD_TOTALSCORE)
    D2D.GraphicTextSizeW(a$$, hHallOfFameCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!-texthg&/2, brushGold&, hHallOfFameCaptionFont&)
    a$$ = FORMAT$(channels(0).campaign.airscore)
    D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!+50*uiscale!-texthg&/2, brushGold&, hShopCaptionFont&)
    destx! = (maparea.left+maparea.right)/4
    a$$ = FORMAT$(channels(0).campaign.groundscore)
    D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!+50*uiscale!-texthg&/2, brushGold&, hShopCaptionFont&)
    destx! = (maparea.left+maparea.right)*3/4
    a$$ = FORMAT$(channels(0).campaign.waterscore)
    D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!+50*uiscale!-texthg&/2, brushGold&, hShopCaptionFont&)

    'Gesamtzeit
    totalminutes& = INT(channels(0).campaign.time/60)
    totalhours& = INT(totalminutes&/60)
    totalminutes& = totalminutes&-totalhours&*60
    a$$ = words$$(%WORD_TOTALTIME)
    REPLACE "%" WITH FORMAT$(totalhours&) IN a$$
    REPLACE "$" WITH FORMAT$(totalminutes&) IN a$$
    destx! = (maparea.left+maparea.right)/2
    desty! = maparea.top+500*uiscale!
    D2D.GraphicTextSizeW(a$$, hHallOfFameCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!-texthg&/2, brushGold&, hHallOfFameCaptionFont&)

    'gefundene Geheimmissionen
    a$$ = words$$(%WORD_FOUNDSECRETS)
    REPLACE "%" WITH FORMAT$(channels(0).campaign.secrets) IN a$$
    desty! = maparea.top+550*uiscale!
    D2D.GraphicTextSizeW(a$$, hHallOfFameCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!-texthg&/2, brushGold&, hHallOfFameCaptionFont&)
  END IF

  'nächste Mission
  t! = gametime!-gameoverOpenTime!
  IF t! >= 2.0 AND channels(0).info.nextmission >= 0 THEN
    destx! = (maparea.left+maparea.right)/2
    desty! = maparea.top+450*uiscale!
    a$$ = words$$(%WORD_NEXTMISSION)
    D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!-texthg&/2, brushWhite&, hShopCaptionFont&)
    '
    episode& = GetEpisodeForMap&(channels(0).info.currentmission)
    nextmission& = IIF&(channels(0).info.state = %CHANNELSTATE_VICTORY, channels(0).info.nextmission, channels(0).info.bonusmission)
    IF episode& > 4 THEN nextmission& = nextmission&+GetEpisodeStartMap&(episode&)
    a$$ = mapnames$(nextmission&)
    D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!-texthg&/2+40*uiscale!, brushWhite&, hShopCaptionFont&)
  END IF

  'ESC drücken
  IF t! >= 2.0 AND (INT(t!) AND 1) = 1 THEN
    a$$ = words$$(%WORD_PRESS_ESC)
    D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
    desty! = maparea.bottom-texthg&-40*uiscale!
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!, brushLightGrey&, hShopCaptionFont&)
  END IF

  D2D.ReleaseClippingRegion

  'MILOP Header
  headerWidthScaled! = (txarea_milopheader.right-txarea_milopheader.left)*uiscale!
  headerHeightScaled! = (txarea_milopheader.bottom-txarea_milopheader.top)*uiscale!
  destx! = (maparea.left+maparea.right)/2-headerWidthScaled!/2
  desty! = maparea.bottom-37*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_milopheader.left, txarea_milopheader.top, txarea_milopheader.right, txarea_milopheader.bottom, destx!, desty!, destx!+headerWidthScaled!, desty!+headerHeightScaled!)
END SUB



'Niederlagen-Bildschirm darstellen
SUB RenderDefeat
  LOCAL a$$, nunits&, i&, unittp&, xp&, textwd&, texthg&, shopnr&, destx!, desty!, t!, opacity!, textscale!, headerWidthScaled!, headerHeightScaled!

  D2D.CreateClippingRegion(maparea.left, maparea.top, maparea.right, maparea.bottom)
  D2D.GraphicBox(maparea.left, maparea.top, maparea.right, maparea.bottom, brushBlack&, brushBlack&)
  t! = gametime!-gameoverOpenTime!

  'Niederlage
  a$$ = words$$(%WORD_DEFEAT)
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, (maparea.left+maparea.right-textwd&)/2+2, maparea.top+80*uiscale!-texthg&/2+2, brushPlayer&(localPlayerNr&), hBigCaptionFont&)
  D2D.GraphicPrintW(a$$, (maparea.left+maparea.right-textwd&)/2, maparea.top+80*uiscale!-texthg&/2, brushWhite&, hBigCaptionFont&)
  desty! = maparea.top+150*uiscale!

  'Bedingung zeigen, die zur Niederlage führte
  a$$ = ""
  IF defeatCondition& = -1 THEN
    a$$ = words$$(%WORD_DEFEAT_TIMELIMIT)
  ELSE
    SELECT CASE channels(0).actions(defeatCondition&).actiontype
    CASE %ACTYPE_VC_PLAYERDEFEATED
      a$$ = words$$(%WORD_DEFEAT_ALLUNITS)
    CASE %ACTYPE_VC_SHOPOCCUPIED
      shopnr& = channels(0).actions(defeatCondition&).shop
      IF shopnr& = localPlayerHQ& THEN
        a$$ = words$$(%WORD_DEFEAT_HEADQUARTER)
      ELSE
        a$$ = words$$(%WORD_DEFEAT_SHOP)
        REPLACE "%" WITH channels(0).info.shopnames(shopnr&) IN a$$
      END IF
    CASE %ACTYPE_VC_TURNREACHED
      a$$ = words$$(%WORD_DEFEAT_TIMELIMIT)
    CASE %ACTYPE_VC_UNITDEAD_PLAYER1 TO %ACTYPE_VC_UNITDEAD_PLAYER6
      unittp& = channels(0).actions(defeatCondition&).actionparam
      a$$ = words$$(%WORD_DEFEAT_UNITKILLED)
      REPLACE "%" WITH channelsnosave(0).unitclasses(unittp&).uname IN a$$
    END SELECT
  END IF
  D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, (maparea.left+maparea.right-textwd&)/2, maparea.top+220*uiscale!-texthg&/2, brushWhite&, hShopCaptionFont&)

  'beste Einheiten ein- und ausblenden
  nunits& = LEN(unitclassesByXp$)/2
  IF nunits& > 0 THEN
    t! = t! MOD (nunits&*2)
    i& = INT(t!/2)
    unittp& = ASC(unitclassesByXp$, i&*2+1)
    xp& = ASC(unitclassesByXp$, i&*2+2)-1
    opacity! = t!-i&*2
    IF opacity! > 1.0 THEN opacity! = 2.0-opacity!
    opacity! = MAX(0.01, opacity!)
    'Einheiten-Bild
    IF unitAnimFrameWidth& = 0 THEN
      D2D.GraphicStretch(channelsnosave(0).unitclasses(unittp&).artworkhandle, 0, 0, 800, 600, (maparea.left+maparea.right)/2-360*uiscale!, desty!+300*uiscale!, (maparea.left+maparea.right)/2+360*uiscale!, desty!+840*uiscale!, opacity!)
    ELSE
      CALL DrawUnitAnimation(unittp&, (maparea.left+maparea.right)/2-360*uiscale!, desty!+300*uiscale!, (maparea.left+maparea.right)/2+360*uiscale!, desty!+840*uiscale!)
    END IF
    'Erfahrungspunkte
    D2D.GraphicStretch(hHudElements&, 321, xp&*64+1, 383, xp&*64+63, (maparea.left+maparea.right)/2-64*uiscale!, desty!+640*uiscale!, (maparea.left+maparea.right)/2+64*uiscale!, desty!+768*uiscale!, opacity!)
  END IF

  'nächste Mission
  t! = gametime!-gameoverOpenTime!
  IF t! >= 2.0 THEN
    destx! = (maparea.left+maparea.right)/2
    desty! = maparea.top+350*uiscale!
    a$$ = words$$(%WORD_NEXTMISSION)
    D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!-texthg&/2, brushWhite&, hShopCaptionFont&)
    '
    a$$ = mapnames$(channels(0).info.currentmission)
    D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!-texthg&/2+40*uiscale!, brushWhite&, hShopCaptionFont&)
  END IF

  'ESC drücken
  IF t! >= 2.0 AND (INT(t!) AND 1) = 1 THEN
    a$$ = words$$(%WORD_PRESS_ESC)
    D2D.GraphicTextSizeW(a$$, hShopCaptionFont&, textwd&, texthg&)
    desty! = maparea.bottom-texthg&-40*uiscale!
    D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!, brushLightGrey&, hShopCaptionFont&)
  END IF

  D2D.ReleaseClippingRegion

  'MILOP Header
  headerWidthScaled! = (txarea_milopheader.right-txarea_milopheader.left)*uiscale!
  headerHeightScaled! = (txarea_milopheader.bottom-txarea_milopheader.top)*uiscale!
  destx! = (maparea.left+maparea.right)/2-headerWidthScaled!/2
  desty! = maparea.bottom-37*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_milopheader.left, txarea_milopheader.top, txarea_milopheader.right, txarea_milopheader.bottom, destx!, desty!, destx!+headerWidthScaled!, desty!+headerHeightScaled!)
END SUB



'Multiplayer-Lobby darstellen
SUB RenderLobby
  LOCAL t!, wd!, hg!, destx!, desty!, cdflow!, previewx!, previewy!, angle!, headerWidth!
  LOCAL fullwidth&, fullheight&, nch&, p&, npl&, maxpl&, md&, difficulty&, chnr&, plnr&, team&, xp&
  LOCAL mcode$, chname$, pldetails$, plname$

  t! = gametime!-lobbyOpenTime!-%DIALOGUE_OPEN_MS/1000
  IF t! <= 0 THEN EXIT SUB

  'Fenster zeichnen (Hintergrund)
  D2D.GraphicStretch(hDialog&, txarea_mplobby.left, txarea_mplobby.top, txarea_mplobby.right, txarea_mplobby.bottom, activedialoguearea.left, activedialoguearea.top, activedialoguearea.right, activedialoguearea.bottom)

  'Beschriftung "Offene Spiele"
  destx! = activedialoguearea.left+28*uiscale!  'linke obere Ecke der Channelliste
  desty! = activedialoguearea.top+62*uiscale!
  D2D.GraphicPrintW(words$$(%WORD_GAMENAME), destx!+10*uiscale!, desty!, brushWhite&, hLobbyCaptionFont&)
  D2D.GraphicPrintW(words$$(%WORD_MAP), destx!+320*uiscale!, desty!, brushWhite&, hLobbyCaptionFont&)
  D2D.GraphicPrintW(words$$(%WORD_DIFFICULTY_SHORT), destx!+420*uiscale!, desty!, brushWhite&, hLobbyCaptionFont&)
  D2D.GraphicPrintW(words$$(%WORD_PLAYER), destx!+480*uiscale!, desty!, brushWhite&, hLobbyCaptionFont&)
  D2D.GraphicPrintW(words$$(%WORD_GAMEMODE), destx!+550*uiscale!, desty!, brushWhite&, hLobbyCaptionFont&)

  'Beschriftung "Spieler"
  desty! = activedialoguearea.top+222*uiscale!
  D2D.GraphicPrintW(words$$(%WORD_PLAYERNAME), destx!+180*uiscale!, desty!, brushWhite&, hLobbyCaptionFont&)
  D2D.GraphicPrintW(words$$(%WORD_COLOR), destx!+380*uiscale!, desty!, brushWhite&, hLobbyCaptionFont&)
  D2D.GraphicPrintW(words$$(%WORD_TEAM), destx!+440*uiscale!, desty!, brushWhite&, hLobbyCaptionFont&)
  D2D.GraphicPrintW(words$$(%WORD_XP), destx!+500*uiscale!, desty!, brushWhite&, hLobbyCaptionFont&)

  'Channel-Informationen
  desty! = activedialoguearea.top+90*uiscale!
  lobbyChannels$ = ""
  p& = 1
  WHILE p& < LEN(lobbyData$) AND nch& < 5
    chnr& = ASC(lobbyData$, p&)
    mcode$ = MID$(lobbyData$, p&+1, 7)
    chname$ = RTRIM$(MID$(lobbyData$, p&+8, 32))
    difficulty& = ASC(lobbyData$, p&+40)
    npl& = ASC(lobbyData$, p&+41)
    maxpl& = ASC(lobbyData$, p&+42)
    md& = ASC(lobbyData$, p&+43)
    IF chnr& = selectedLobbyChannel& THEN
      channels(0).info.cname = chname$
      pldetails$ = MID$(lobbyData$, p&+44, npl&*19)
      D2D.GraphicBox(destx!, desty!+(nch&*20-2)*uiscale!, destx!+605*uiscale!, desty!+(nch&*20+18)*uiscale!, brushBlueTransparent&, brushBlueTransparent&)
    END IF
    D2D.GraphicPrint(chname$, destx!+10*uiscale!, desty!+nch&*20*uiscale!, brushWhite&, hLobbyCaptionFont&)
    D2D.GraphicPrint(mcode$, destx!+320*uiscale!, desty!+nch&*20*uiscale!, brushWhite&, hLobbyCaptionFont&)
    D2D.GraphicPrint(FORMAT$(difficulty&+1), destx!+420*uiscale!, desty!+nch&*20*uiscale!, brushWhite&, hLobbyCaptionFont&)
    D2D.GraphicPrint(FORMAT$(npl&)+"/"+FORMAT$(maxpl&), destx!+480*uiscale!, desty!+nch&*20*uiscale!, brushWhite&, hLobbyCaptionFont&)
    D2D.GraphicPrintW(words$$(IIF&(md& = 0, %WORD_COOP, %WORD_VERSUS)), destx!+550*uiscale!, desty!+nch&*20*uiscale!, brushWhite&, hLobbyCaptionFont&)
    lobbyChannels$ = lobbyChannels$+CHR$(chnr&)
    p& = p&+44+npl&*19
    nch& = nch&+1
  WEND

  'Spieler-Details
  desty! = activedialoguearea.top+250*uiscale!
  npl& = 0
  p& = 1
  WHILE p& < LEN(pldetails$)
    plname$ = RTRIM$(MID$(pldetails$, p&, 16))
    plnr& = ASC(pldetails$, p&+16)
    team& = ASC(pldetails$, p&+17)
    xp& = PlayerXPToIcon&(ASC(pldetails$, p&+18))
    D2D.GraphicPrint(plname$, destx!+180*uiscale!, desty!+npl&*20*uiscale!, brushWhite&, hLobbyCaptionFont&)
    D2D.GraphicStretch(hHudElements&, plnr&*18, 743, plnr&*18+18, 761, destx!+390*uiscale!, desty!+npl&*20*uiscale!, destx!+408*uiscale!, desty!+(npl&*20+18)*uiscale!)
    D2D.GraphicPrint(FORMAT$(team&), destx!+450*uiscale!, desty!+npl&*20*uiscale!, brushWhite&, hLobbyCaptionFont&)
    D2D.GraphicStretch(hHudElements&, 320, xp&*64, 384, xp&*64+64, destx!+500*uiscale!, desty!+npl&*20*uiscale!, destx!+516*uiscale!, desty!+(npl&*20+16)*uiscale!)
    p& = p&+19
    npl& = npl&+1
  WEND

  'Channelname
  IF gameState& <> %GAMESTATE_CHANNELJOINED AND ishost& = 1 THEN D2D.GraphicPrintW(words$$(%WORD_GAMENAME), destx!+180*uiscale!, desty!+72*uiscale!, brushGold&, hLobbyCaptionFont&)

  'Server
  D2D.GraphicPrintW(words$$(%WORD_SERVER), destx!+180*uiscale!, desty!+160*uiscale!, brushWhite&, hLobbyCaptionFont&)

  'Kartenvorschau
  destx! = activedialoguearea.left+658*uiscale!  'linke obere Ecke der Kartenvorschau
  desty! = activedialoguearea.top+58*uiscale!
  angle! = gameTime!/4.0  'ca. 25 Sekunden für eine komplette Umdrehung
  previewx! = 30*COS(angle!)
  previewy! = 30*SIN(angle!)
  D2D.GraphicStretch(hMapPreview&, 40+previewx!, 40+previewy!, 200+previewx!, 200+previewy!, destx!, desty!, destx!+320*uiscale!, desty!+320*uiscale!)

  'Countdown
  IF mpCountdown& > 0 THEN
    cdflow! = 5*(gametime!-mpCountdownTime!)
    D2D.GraphicStretch(hHudElements&, mpCountdown&*160+354, 455, mpCountdown&*160+512, 613, destx!+(80+cdflow!)*uiscale!, desty!+(80+cdflow!)*uiscale!, destx!+(240-cdflow!)*uiscale!, desty!+(240-cdflow!)*uiscale!)
  END IF

  'MILOP Header
  headerWidth! = (txarea_milopheader.right-txarea_milopheader.left)*uiscale!
  destx! = activedialoguearea.left+(activedialoguearea.right-activedialoguearea.left-headerWidth!)/2
  desty! = activedialoguearea.top-32*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_milopheader.left, txarea_milopheader.top, txarea_milopheader.right, txarea_milopheader.bottom, destx!, desty!, destx!+headerWidth!, desty!+(txarea_milopheader.bottom-txarea_milopheader.top)*uiscale!)
END SUB



'Steuerelemente der Multiplayer-Lobby ein/ausschalten
SUB SetMultiplayerLobbyControls(visible&)
  editServerIP.XPos = activedialoguearea.left+260*uiscale!
  editServerIP.YPos = activedialoguearea.top+408*uiscale!
  editServerIP.Visible = visible&

  editGameName.XPos = activedialoguearea.left+209*uiscale!
  editGameName.YPos = activedialoguearea.top+344*uiscale!
  editGameName.Visible = IIF&(gameState& = %GAMESTATE_CHANNELJOINED, 0, 1)*ishost&*visible&

  buttonConnect.Visible = visible&
  buttonConnect.XPos = activedialoguearea.left+370*uiscale!
  buttonConnect.YPos = activedialoguearea.top+400*uiscale!

  buttonCreateGame.Visible = ishost&*visible&
  buttonCreateGame.Enabled = IIF&(gameState& = %GAMESTATE_CHANNELJOINED, 0, 1)
  buttonCreateGame.XPos = activedialoguearea.left+505*uiscale!
  buttonCreateGame.YPos = activedialoguearea.top+400*uiscale!

  buttonJoinGame.Visible = (1-ishost&)*visible&
  buttonJoinGame.XPos = activedialoguearea.left+505*uiscale!
  buttonJoinGame.YPos = activedialoguearea.top+400*uiscale!

  buttonChangeColor.Visible = visible&
  buttonChangeColor.Enabled = IIF&(gameState& = %GAMESTATE_CHANNELJOINED, 1, 0)
  buttonChangeColor.XPos = activedialoguearea.left+30*uiscale!
  buttonChangeColor.YPos = activedialoguearea.top+195*uiscale!

  buttonEndTurn.Enabled = IIF&(visible& = 0, 1, 0)
  buttonSaveGame.Enabled = IIF&(visible& = 0, 1, 0)
  buttonMapInfo.Enabled = IIF&(visible& = 0, 1, 0)
  buttonProtocol.Enabled = IIF&(visible& = 0, 1, 0)
  buttonHighscore.Enabled = IIF&(visible& = 0, 1, 0)
END SUB



'Karteninfo-Overlay darstellen
SUB RenderMapInfo
  LOCAL t!, destx!, desty!, headerWidth!, headerHeight!, wheelWidth!, wheelHeight!, blackRadius!, angle!
  LOCAL i&, n&, p&, q&, pl&, textwd&, texthg&, allymask&, hidedata&, listlen&, unitindex&, unitnr&, unittp&, xp&
  LOCAL a$$
  LOCAL info&()

  t! = gametime!-mapinfoOpenTime!-%DIALOGUE_OPEN_MS/1000
  IF t! <= 0 THEN EXIT SUB

  'linkes Rad zeichnen
  D2D.CreateClippingRegion(maparea.left, maparea.top, maparea.right, maparea.bottom)
  destx! = activedialoguearea.left+309*uiscale!  'Mittelpunkt des Rads
  desty! = activedialoguearea.top+383*uiscale!
  wheelWidth! = (txarea_wheel_transparent.right-txarea_wheel_transparent.left)*uiscale!
  wheelHeight! = (txarea_wheel_transparent.bottom-txarea_wheel_transparent.top)*uiscale!
  blackRadius! = wheelHeight!/2-30
  angle! = t!*10
  listlen& = LEN(unitListByXP$)/2
  IF listlen& > 0 THEN
    D2D.RotateOutput(angle!, destx!, desty!)
    D2D.GraphicEllipse(destx!-blackRadius!, desty!-blackRadius!, destx!+blackRadius!, desty!+blackRadius!, brushBlack&, brushBlack&)
    FOR i& = 0 TO 4
      unitindex& = (INT(angle!/22.5)+i&-2) MOD listlen&
      unittp& = ASC(unitListByXP$, unitindex&*2+1)
      xp& = ASC(unitListByXP$, unitindex&*2+2)-1
      D2D.RotateOutput((angle! MOD 22.5)+33.75-i&*22.5, destx!, desty!)
      D2D.GraphicStretch(channelsnosave(0).unitclasses(unittp&).artworkhandle, 0, 0, 800, 600, destx!-373*uiscale!, desty!-51*uiscale!, destx!-237*uiscale!, desty!+51*uiscale!)
      D2D.GraphicStretch(hHudElements&, 321, xp&*64+1, 383, xp&*64+63, destx!-289*uiscale!, desty!+15*uiscale!, destx!-321*uiscale!, desty!+47*uiscale!)
    NEXT i&
    D2D.RotateOutput(angle!, destx!, desty!)
    D2D.GraphicStretch(hDialog2&, txarea_wheel_transparent.left, txarea_wheel_transparent.top, txarea_wheel_transparent.right, txarea_wheel_transparent.bottom, destx!-wheelWidth!/2, desty!-wheelHeight!/2, destx!+wheelWidth!/2, desty!+wheelHeight!/2)
    D2D.ResetMatrix
  END IF

  'rechtes Rad zeichnen
  destx! = activedialoguearea.right-309*uiscale!  'Mittelpunkt des Rads
  angle! = -t!*10
  listlen& = LEN(unitListByType$)/2
  IF listlen& > 0 THEN
    D2D.RotateOutput(angle!, destx!, desty!)
    D2D.GraphicEllipse(destx!-blackRadius!, desty!-blackRadius!, destx!+blackRadius!, desty!+blackRadius!, brushBlack&, brushBlack&)
    FOR i& = 0 TO 4
      unitindex& = (INT(angle!/22.5)+i&-2) MOD listlen&
      IF unitindex& < 0 THEN unitindex& = unitindex&+listlen&
      unittp& = ASC(unitListByType$, unitindex&*2+1)
      n& = ASC(unitListByType$, unitindex&*2+2)
      D2D.RotateOutput((angle! MOD 22.5)+33.75-i&*22.5, destx!, desty!)
      D2D.GraphicStretch(channelsnosave(0).unitclasses(unittp&).artworkhandle, 0, 0, 800, 600, destx!+237*uiscale!, desty!-51*uiscale!, destx!+373*uiscale!, desty!+51*uiscale!)
      a$$ = FORMAT$(n&)+"x"
      D2D.GraphicTextSizeW(a$$, hCaptionFont&, textwd&, texthg&)
      D2D.GraphicPrintW(a$$, destx!+305*uiscale!-textwd&/2, desty!+40*uiscale!-texthg&/2, brushWhite&, hCaptionFont&)
    NEXT i&
    D2D.RotateOutput(angle!, destx!, desty!)
    D2D.GraphicStretch(hDialog2&, txarea_wheel_transparent.left, txarea_wheel_transparent.top, txarea_wheel_transparent.right, txarea_wheel_transparent.bottom, destx!-wheelWidth!/2, desty!-wheelHeight!/2, destx!+wheelWidth!/2, desty!+wheelHeight!/2)
    D2D.ResetMatrix
  END IF
  D2D.ReleaseClippingRegion

  'Fenster zeichnen (Hintergrund)
  D2D.GraphicStretch(hDialog&, txarea_mapinfo.left, txarea_mapinfo.top, txarea_mapinfo.right, txarea_mapinfo.bottom, activedialoguearea.left, activedialoguearea.top, activedialoguearea.right, activedialoguearea.bottom)

  'Karteninformationen darstellen
  CALL GetMapInfo(info&())
  destx! = activedialoguearea.left+28*uiscale!  'linke obere Ecke des ersten Spielers
  desty! = activedialoguearea.top+112*uiscale!
  a$$ = words$$(%WORD_PLAYERNAME)
  D2D.GraphicTextSizeW(a$$, hCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!+10*uiscale!, desty!-30*uiscale!-texthg&/2, brushWhite&, hCaptionFont&)
  a$$ = words$$(%WORD_ALLIANCE)
  D2D.GraphicTextSizeW(a$$, hCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!+580*uiscale!, desty!-30*uiscale!-texthg&/2, brushWhite&, hCaptionFont&)
  FOR pl& = 0 TO %MAXPLAYERS-1
    IF (channels(0).info.originalplayers AND 2^pl&) = 0 THEN ITERATE FOR

    'Spielerfarbe
    D2D.GraphicStretch(hDialog&, txarea_mapinfoplayer.left, txarea_mapinfoplayer.top, txarea_mapinfoplayer.right, txarea_mapinfoplayer.bottom, destx!+2, desty!, destx!+745*uiscale!, desty!+31*uiscale!)
    D2D.GraphicStretch(hDialog&, txarea_playercolors.left, txarea_playercolors.top+pl&*50, txarea_playercolors.left+50, txarea_playercolors.top+pl&*50+50, destx!+2, desty!, destx!+745*uiscale!, desty!+30*uiscale!)

    'Spielername
    a$$ = IIF$(pl& = localPlayerNr&, localPlayerName$, playernames$(pl&))
    IF a$$ = "" THEN a$$ = defaultPlayernames$(pl&)
    D2D.GraphicTextSizeW(a$$, hCaptionFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!+10*uiscale!, desty!+15*uiscale!-texthg&/2, brushPlayer&(pl&), hCaptionFont&)

    'Anzahl Einheiten und Energie/Materialzuwachs
    FOR i& = 0 TO 6
      hidedata& = IIF&(channels(0).info.difficulty > %DIFFICULTY_EASY AND (channels(0).player(localPlayerNr&).allymask AND 2^pl&) = 0, 1, 0)
      a$$ = IIF$(i& > 4, "+", "")+FORMAT$(info&(i&, pl&))
      IF i& < 3 AND hidedata& = 1 THEN a$$ = a$$+" (?)"
      IF i& > 2 AND hidedata& = 1 THEN a$$ = "??"
      D2D.GraphicTextSizeW(a$$, hGameMessageFont&, textwd&, texthg&)
      D2D.GraphicPrintW(a$$, destx!+(204+i&*50)*uiscale!-textwd&/2, desty!+15*uiscale!-texthg&/2, brushWhite&, hGameMessageFont&)
    NEXT i&

    'Allianzen
    allymask& = channels(0).player(pl&).allymask
    q& = 0
    FOR i& = 0 TO %MAXPLAYERS-1
      IF i& <> pl& AND (allymask& AND 2^i&) <> 0 THEN
        D2D.GraphicStretch(hHudElements&, i&*18, 743, i&*18+18, 761, destx!+(580+q&*20)*uiscale!, desty!+6*uiscale!, destx!+(598+q&*20)*uiscale!, desty!+24*uiscale!)
        q& = q&+1
      END IF
    NEXT i&

    desty! = desty!+33*uiscale!
  NEXT pl&

  'Missionstitel (Kurzbeschreibung)
  destx! = activedialoguearea.left+98*uiscale!  'linke obere Ecke des Textbereichs
  desty! = activedialoguearea.top+334*uiscale!
  a$$ = channels(0).info.mapshortdescr
  D2D.GraphicTextSizeW(a$$, hCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!+303*uiscale!-textwd&/2, desty!+20*uiscale!-texthg&/2, brushLightGrey&, hCaptionFont&)

  'Missionsbeschreibung (Langbeschreibung)
  q& = 50
  FOR i& = 0 TO 19
    IF missionTextRows$$(i&, 0) = "" THEN EXIT FOR
    D2D.GraphicTextSizeW(missionTextRows$$(i&, 0), hGameMessageFont&, textwd&, texthg&)
    D2D.GraphicPrintW(missionTextRows$$(i&, 0), destx!+303*uiscale!-textwd&/2, desty!+q&*uiscale!-texthg&/2, brushLightGrey&, hGameMessageFont&)
    q& = q&+20
  NEXT i&

  'Missionsziel (Missionbriefing Nachricht)
  a$$ = words$$(%WORD_MISSIONOBJECTIVE)
  D2D.GraphicTextSizeW(a$$, hCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!+303*uiscale!-textwd&/2, desty!+107*uiscale!-texthg&/2, brushDarkRed&, hCaptionFont&)
  destx! = activedialoguearea.left+38*uiscale!  'linke obere Ecke des Textbereichs (inklusive 10 bzw. 12 Pixel Einrückung)
  desty! = activedialoguearea.top+470*uiscale!
  q& = -mapinfoScrollPos&
  D2D.CreateClippingRegion(destx!, desty!, destx!+727*uiscale!, desty!+176*uiscale!)
  FOR i& = 0 TO 19
    IF missionTextRows$$(i&, 1) = "" THEN EXIT FOR
    D2D.GraphicTextSizeW(missionTextRows$$(i&, 1), hGameMessageFont&, textwd&, texthg&)
    D2D.GraphicPrintW(missionTextRows$$(i&, 1), destx!+363*uiscale!-textwd&/2, desty!+q&, brushLightGrey&, hGameMessageFont&)
    q& = q&+20*uiscale!
    IF q& > 175 AND i& < 19 AND missionTextRows$$(i&+1, 1) <> "" AND t! > 2.0 THEN
      mapinfoScrollPos& = mapinfoScrollPos&+1
      EXIT FOR
    END IF
  NEXT i&
  D2D.ReleaseClippingRegion

  'Header mit Missionscode
  headerWidth! = (txarea_blankheader.right-txarea_blankheader.left)*uiscale!
  destx! = activedialoguearea.left+(activedialoguearea.right-activedialoguearea.left-headerWidth!)/2
  desty! = activedialoguearea.top-32*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_blankheader.left, txarea_blankheader.top, txarea_blankheader.right, txarea_blankheader.bottom, destx!, desty!, destx!+headerWidth!, desty!+(txarea_blankheader.bottom-txarea_blankheader.top)*uiscale!)
  IF CompatibilityMode&(0) = 3 THEN
    a$$ = words$$(%WORD_BI3MISSION)+" "+FORMAT$(channels(0).info.currentmission-255)+"  -  "+mapnames$(channels(0).info.currentmission)
  ELSE
    a$$ = words$$(%WORD_MISSION)+" "+FORMAT$(channels(0).info.currentmission)+"  -  "+mapnames$(channels(0).info.currentmission)
  END IF
  D2D.GraphicTextSizeW(a$$, hCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, (activedialoguearea.right+activedialoguearea.left)/2-textwd&/2, desty!+50*uiscale!-texthg&/2, brushPlayer&(localPlayerNr&), hCaptionFont&)

  'Rundenzähler
  headerWidth! = (txarea_roundbox.right-txarea_roundbox.left)*uiscale!
  headerHeight! = (txarea_roundbox.bottom-txarea_roundbox.top)*uiscale!
  destx! = (activedialoguearea.left+activedialoguearea.right)/2-headerWidth!/2  'linke obere Ecke des Kastens
  desty! = activedialoguearea.bottom-headerHeight!/2-12*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_roundbox.left, txarea_roundbox.top, txarea_roundbox.right, txarea_roundbox.bottom, destx!, desty!, destx!+headerWidth!, desty!+headerHeight!)
  a$$ = words$$(%WORD_TURN)+" "+FORMAT$(channels(0).info.turn+1)
  IF channels(0).info.turnlimit > 0 THEN a$$ = a$$+" / "+FORMAT$(channels(0).info.turnlimit)
  a$$ = a$$+"   "+words$$(%WORD_EASY+channels(0).info.difficulty)
  D2D.GraphicTextSizeW(a$$, hCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!+headerWidth!/2-textwd&/2, desty!+headerHeight!/2-texthg&/2, brushPlayer&(localPlayerNr&), hCaptionFont&)
END SUB



'Bestenliste für die aktuelle Mission darstellen
SUB RenderHallOfFame
  LOCAL destx!, desty!, headerWidth!
  LOCAL i&, n&, x&, y&, b&, textwd&, texthg&
  LOCAL a$$, plname$
  LOCAL r AS THighScore

  'Fenster zeichnen (Hintergrund)
  D2D.GraphicStretch(hDialog&, txarea_highscore.left, txarea_highscore.top, txarea_highscore.right, txarea_highscore.bottom, activedialoguearea.left, activedialoguearea.top, activedialoguearea.right, activedialoguearea.bottom)

  'Beschriftungen (Schwierigkeitsgrade)
  destx! = activedialoguearea.left+230*uiscale!  'Mittelpunkt des ersten Kastens
  desty! = activedialoguearea.top+87*uiscale!
  a$$ = words$$(%WORD_EASY)
  D2D.GraphicTextSizeW(a$$, hCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!-textwd&/2, desty!-texthg&/2, brushBronze&, hCaptionFont&)
  a$$ = words$$(%WORD_NORMAL)
  D2D.GraphicTextSizeW(a$$, hCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!+336*uiscale!-textwd&/2, desty!-texthg&/2, brushSilver&, hCaptionFont&)
  a$$ = words$$(%WORD_HARD)
  D2D.GraphicTextSizeW(a$$, hCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!+672*uiscale!-textwd&/2, desty!-texthg&/2, brushGold&, hCaptionFont&)
  a$$ = words$$(%WORD_PLAYERNAME)
  D2D.GraphicTextSizeW(a$$, hGameMessageFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!-145*uiscale!, desty!+61*uiscale!-texthg&/2, brushBronze&, hGameMessageFont&)
  D2D.GraphicPrintW(a$$, destx!+191*uiscale!, desty!+61*uiscale!-texthg&/2, brushSilver&, hGameMessageFont&)
  D2D.GraphicPrintW(a$$, destx!+527*uiscale!, desty!+61*uiscale!-texthg&/2, brushGold&, hGameMessageFont&)

  'Bestenliste
  destx! = activedialoguearea.left+85*uiscale!  'Start der ersten Textzeile
  desty! = activedialoguearea.top+177*uiscale!
  n& = (LEN(highscoreMapData$)-4)/SIZEOF(THighScore)
  x& = -1
  FOR i& = 0 TO n&-1
    POKE$ VARPTR(r), MID$(highscoreMapData$, i&*SIZEOF(THighScore)+5, SIZEOF(THighScore))
    plname$ = TRIM$(r.playername)
    IF x& <> r.difficulty THEN
      x& = r.difficulty
      SELECT CASE x&
      CASE 0: b& = brushBronze&
      CASE 1: b& = brushSilver&
      CASE 2: b& = brushGold&
      END SELECT
      y& = 0
    END IF
    D2D.GraphicPrint(plname$, destx!+x&*336*uiscale!, desty!+y&*uiscale!, b&, hGameMessageFont&)
    a$$ = FORMAT$(r.turnnumber+1)
    D2D.GraphicTextSizeW(a$$, hGameMessageFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!+(x&*336+127)*uiscale!-textwd&/2, desty!+y&*uiscale!, b&, hGameMessageFont&)
    a$$ = FORMAT$(r.scoreground)
    D2D.GraphicTextSizeW(a$$, hGameMessageFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!+(x&*336+177)*uiscale!-textwd&/2, desty!+y&*uiscale!, b&, hGameMessageFont&)
    a$$ = FORMAT$(r.scoreair)
    D2D.GraphicTextSizeW(a$$, hGameMessageFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!+(x&*336+227)*uiscale!-textwd&/2, desty!+y&*uiscale!, b&, hGameMessageFont&)
    a$$ = FORMAT$(r.scorewater)
    D2D.GraphicTextSizeW(a$$, hGameMessageFont&, textwd&, texthg&)
    D2D.GraphicPrintW(a$$, destx!+(x&*336+277)*uiscale!-textwd&/2, desty!+y&*uiscale!, b&, hGameMessageFont&)
    y& = y&+20
  NEXT i&

  'Beschriftung oben: Ruhmeshalle
  headerWidth! = (txarea_blankheader.right-txarea_blankheader.left)*uiscale!
  destx! = activedialoguearea.left+(activedialoguearea.right-activedialoguearea.left-headerWidth!)/2
  desty! = activedialoguearea.top-32*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_blankheader.left, txarea_blankheader.top, txarea_blankheader.right, txarea_blankheader.bottom, destx!, desty!, destx!+headerWidth!, desty!+(txarea_blankheader.bottom-txarea_blankheader.top)*uiscale!)
  a$$ = words$$(%WORD_HALLOFFAME)
  D2D.GraphicTextSizeW(a$$, hHallOfFameCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, (activedialoguearea.right+activedialoguearea.left)/2-textwd&/2, desty!+49*uiscale!-texthg&/2, brushLightGrey&, hHallOfFameCaptionFont&)

  'Beschriftung unten: Missionscode
  headerWidth! = (txarea_blankfooter.right-txarea_blankfooter.left)*uiscale!
  destx! = activedialoguearea.left+(activedialoguearea.right-activedialoguearea.left-headerWidth!)/2
  desty! = activedialoguearea.bottom-70*uiscale!
  D2D.GraphicStretch(hDialog&, txarea_blankfooter.left, txarea_blankfooter.top, txarea_blankfooter.right, txarea_blankfooter.bottom, destx!, desty!, destx!+headerWidth!, desty!+(txarea_blankfooter.bottom-txarea_blankfooter.top)*uiscale!)
  a$$ = mapnames$(channels(0).info.currentmission)
  D2D.GraphicTextSizeW(a$$, hHallOfFameCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, destx!+headerWidth!/2-textwd&/2, desty!+54*uiscale!-texthg&/2, brushLightGrey&, hHallOfFameCaptionFont&)
END SUB



'Einheitenliste darstellen
SUB RenderUnitList
  LOCAL unitnr&, unittp&, owner&, x0&, x1&, x2&, y0&, y1&, y2&, selunit&, mapx&, mapy&, textwd&, texthg&
  LOCAL wd!, hg!, destx!, desty!
  LOCAL a$

  destx! = maparea.right+10
  desty! = minimaparea.top-10
  wd! = messagearea.right+10-destx!
  hg! = MIN(channels(0).info.nunits*11*uiscale!, messagearea.bottom-desty!)+4

  'prüfen, ob Mauscursor über einem Listeneintrag oder einer Einheit auf dem Spielfeld liegt
  selunit& = -1
  IF mousexpos& > destx! AND mousexpos& < destx!+wd! AND mouseypos& > desty! AND mouseypos& < desty!+hg! THEN
    selunit& = INT((mouseypos&-desty!)/11/uiscale!)
  END IF
  CALL GetMapPos(mousexpos&, mouseypos&, mapx&, mapy&)
  IF mapx& >= 0 AND channels(0).zone3(mapx&, mapy&) <> -1 THEN selunit& = channels(0).zone3(mapx&, mapy&)

  'Hintergrund abdunkeln
  D2D.GraphicBox(destx!, desty!, destx!+wd!, desty!+hg!, brushMenuBorder&, brushMenuBackground&)

  'Einheiteninformationen
  FOR unitnr& = 0 TO channels(0).info.nunits-1
    unittp& = channels(0).units(unitnr&).unittype
    owner& = channels(0).units(unitnr&).owner
    a$ = FORMAT$(unitnr&)+"/"+channelsnosave(0).unitclasses(unittp&).uname+" "+FORMAT$(channels(0).units(unitnr&).groupsize)+"/"+FORMAT$(channelsnosave(0).unitclasses(unittp&).groupsize)+" ST:"+FORMAT$(channels(0).units(unitnr&).flags)
    IF (channels(0).info.aimask AND 2^owner&) <> 0 THEN a$ = a$+" AI:"+FORMAT$(channels(0).units(unitnr&).aicommand)+" EN:"+FORMAT$(channels(0).units(unitnr&).aitargetunit)+" SH:"+FORMAT$(channels(0).units(unitnr&).aitargetshop)
    IF UnitIsAlive&(0, unitnr&) = 0 THEN a$ = a$+" DEAD"
    D2D.GraphicPrint(a$, destx!+4, desty!+unitnr&*11*uiscale!, brushPlayer&(owner&), hSystemFont&)

    'Zeiger auf Spielfeld
    IF selunit& = -1 OR selunit& = unitnr& THEN
      CALL GetPixelPos(channels(0).units(unitnr&).xpos, channels(0).units(unitnr&).ypos, x2&, y2&)
      IF IsInRect&(x2&, y2&, maparea) THEN
        x0& = destx!+1
        y0& = desty!+unitnr&*11*uiscale!+5*uiscale!
        x1& = x2&+ABS(y2&-y0&)
        y1& = y0&
        D2D.GraphicLine(x0&, y0&, x1&, y1&, brushPlayer&(owner&), 3*uiscale!)
        D2D.GraphicLine(x1&, y1&, x2&, y2&, brushPlayer&(owner&), 3*uiscale!)
      END IF
    END IF

    'Information über angewählte Einheit direkt über dieser anzeigen
    IF selunit& = unitnr& THEN
      D2D.GraphicTextSize(a$, hGameMessageFont&, textwd&, texthg&)
      D2D.GraphicBox(x2&-textwd&/2-2, y2&-texthg&-6*zoom#-2, x2&+textwd&/2+2, y2&-6*zoom#+2, brushMenuBorder&, brushMenuBackground&)
      D2D.GraphicPrint(a$, x2&-textwd&/2, y2&-texthg&-6*zoom#, brushPlayer&(owner&), hGameMessageFont&)
    END IF
  NEXT unitnr&
END SUB



'Channel-Informationen anzeigen
SUB RenderChannelInfo
  LOCAL x!

  D2D.GraphicBox(minimaparea.left, minimaparea.top, minimaparea.right, minimaparea.bottom, brushBlack50&, brushBlack50&)

  x! = minimaparea.left+10
  D2D.GraphicPrint("State: "+FORMAT$(channels(0).info.state), x!, minimaparea.top+10, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Turn: "+FORMAT$(channels(0).info.turn), x!, minimaparea.top+26, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Move: "+FORMAT$(channels(0).info.movement), x!, minimaparea.top+42, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Map Size: "+FORMAT$(channels(0).info.xsize)+" x "+FORMAT$(channels(0).info.ysize), x!, minimaparea.top+58, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Unit Count: "+FORMAT$(channels(0).info.nunits), x!, minimaparea.top+74, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Shop Count: "+FORMAT$(channels(0).info.nshops), x!, minimaparea.top+90, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Action Count: "+FORMAT$(channels(0).info.nactions), x!, minimaparea.top+106, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Total Victory Cond: "+FORMAT$(channels(0).info.nvictoryconditions), x!, minimaparea.top+122, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Req Victory Cond: "+FORMAT$(channels(0).info.requiredvictorycond), x!, minimaparea.top+138, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Turn Limit: "+FORMAT$(channels(0).info.turnlimit), x!, minimaparea.top+154, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Action Position: "+FORMAT$(channels(0).info.actionposition), x!, minimaparea.top+170, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Current Mission: "+FORMAT$(channels(0).info.currentmission), x!, minimaparea.top+186, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Next Mission: "+FORMAT$(channels(0).info.nextmission), x!, minimaparea.top+202, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Player Mask: "+FORMAT$(channels(0).info.originalplayers), x!, minimaparea.top+218, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("AI Mask: "+FORMAT$(channels(0).info.aimask), x!, minimaparea.top+234, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Weather: "+FORMAT$(channels(0).info.weather), x!, minimaparea.top+250, brushWhite&, hGameMessageFont&)
  D2D.GraphicPrint("Active Player: "+FORMAT$(channels(0).info.activeplayer), x!, minimaparea.top+266, brushWhite&, hGameMessageFont&)
END SUB



'allgemeine Debug-Informationen anzeigen
SUB RenderDebugInfo
  LOCAL a$, c$, r&, w&, x&, y&, textwd&, texthg&, deltatime!, fps&
  LOCAL musiclen&&, currentpos&&

  'Spielzustand/Phase/FPS
  IF debugInfo& <> 0 THEN
    deltatime! = gametime!-lasttimeupdate!
    IF deltatime! > 0 THEN fps& = 1/deltatime!
    a$ = "GameState: "+FORMAT$(gameState&)+" / Phase: "+FORMAT$(GetPhase&(0, localPlayerNr&))+" / "+FORMAT$(fps&)+" FPS / Mouse: "+FORMAT$(mousexpos&)+","+FORMAT$(mouseypos&)
    CALL GetMapPos(mousexpos&, mouseypos&, x&, y&)
    IF x& >= 0 THEN a$ = a$+" / Field("+FORMAT$(x&)+","+FORMAT$(y&)+") = "+FORMAT$(channels(0).zone1(x&, y&))+"/"+FORMAT$(channels(0).zone2(x&, y&))+"/"+FORMAT$(channels(0).zone3(x&, y&))
    D2D.GraphicPrint(a$, 10, 0, brushWhite&, hLobbyCaptionFont&)
  END IF

  'Client- und Server-Checksummen
  IF debugChecksums& <> 0 THEN
    c$ = DebugCalculateChannelChecksum$(0)
    a$ = ""
    IF CVL(debugServerChecksums$, 1) <> CVL(c$, 1) THEN a$ = a$+" INFO"
    IF CVL(debugServerChecksums$, 5) <> CVL(c$, 5) THEN a$ = a$+" UNIT:"+FORMAT$(CVL(debugServerChecksums$, 17))+"|"+FORMAT$(CVL(c$, 17))
    IF CVL(debugServerChecksums$, 9) <> CVL(c$, 9) THEN a$ = a$+" SHOP"
    IF CVL(debugServerChecksums$, 13) <> CVL(c$, 13) THEN a$ = a$+" PLYR"
    a$ = TRIM$(a$)
    D2D.GraphicTextSize(a$, hGameMessageFont&, textwd&, texthg&)
    D2D.GraphicPrint(a$, buttonarea.right-textwd&, 2, brushRed&, hGameMessageFont&)
  END IF
END SUB



'Liefert das Quellrechteck für ein Zwischensequenz-Objekt
SUB GetCutSceneObjectSrcRect(BYVAL frame&, BYREF xsrc0&, BYREF ysrc0&, BYREF xsrc1&, BYREF ysrc1&)
  SELECT CASE frame&
  CASE 0 TO 3:  'Rauch
    xsrc0& = 1+frame&*256
    ysrc0& = 1
    xsrc1& = 254+frame&*256
    ysrc1& = 254
  CASE 4:  'Stadtgebäude 1
    xsrc0& = 1024
    ysrc0& = 0
    xsrc1& = 1154
    ysrc1& = 172
  CASE 5:  'Stadtgebäude 2
    xsrc0& = 1154
    ysrc0& = 0
    xsrc1& = 1220
    ysrc1& = 108
  CASE 6:  'Stadtgebäude 3
    xsrc0& = 1220
    ysrc0& = 0
    xsrc1& = 1263
    ysrc1& = 88
  CASE 7:  'Berge
    xsrc0& = 0
    ysrc0& = 434
    xsrc1& = 2402
    ysrc1& = 626
  CASE 8:  'Tunneleinfahrt
    xsrc0& = 1083
    ysrc0& = 245
    xsrc1& = 1800
    ysrc1& = 434
  CASE 9:  'Zug
    xsrc0& = 0
    ysrc0& = 256
    xsrc1& = 1083
    ysrc1& = 434
  CASE 10:  'nach rechts zeigendes Flugzeug
    xsrc0& = 1484
    ysrc0& = 100
    xsrc1& = 1592
    ysrc1& = 140
  CASE 11:  'nach vorne zeigendes Flugzeug
    xsrc0& = 1484
    ysrc0& = 50
    xsrc1& = 1590
    ysrc1& = 100
  CASE 64 TO 74:  'Radarschüssel
    xsrc0& = 1484+(frame&-64)*50
    ysrc0& = 0
    xsrc1& = 1534+(frame&-64)*50
    ysrc1& = 50
  CASE 96 TO 98:  'Helikopter 1
    xsrc0& = (frame&-96)*689
    ysrc0& = 626
    xsrc1& = 689+(frame&-96)*689
    ysrc1& = 993
  CASE 100 TO 102:  'Helikopter 2
    xsrc0& = (frame&-100)*482
    ysrc0& = 993
    xsrc1& = 482+(frame&-100)*482
    ysrc1& = 1263
  CASE 104 TO 106:  'Helikopter 3
    xsrc0& = (frame&-104)*356
    ysrc0& = 1263
    xsrc1& = 356+(frame&-104)*356
    ysrc1& = 1438
  END SELECT
END SUB



'Zwischensequenz anzeigen
SUB RenderCutScene
  LOCAL a$$, b$$, title$$, i&, c&, n&, xoffset&, y&, textwd&, texthg&
  LOCAL t!, totaltexttime!, currentLineAvailableTime!, aspectratio!, opacity!, xpos!, ypos!, objwidth!, objheight!, xsrc0&, ysrc0&, xsrc1&, ysrc1&

  IF isVideoCutscene& = 1 THEN
    CALL RenderVideoCutScene
    EXIT SUB
  END IF

  t! = gametime!-cutSceneStartTime!
  aspectratio! = windowWidth&/windowHeight&

  'Scrollposition berechnen
  cutSceneScrollPos! = t!*50-200
  cutSceneScrollPos! = MAX(0, MIN(4000-1080*aspectratio!, cutSceneScrollPos!))

  'Hintergrund (Landschaft)
  D2D.GraphicStretch(hCutScene&, cutSceneScrollPos!, 0, cutSceneScrollPos!+1080*aspectratio!, 1080, 0, 0, windowWidth&, windowHeight&)

  'Objekte (in umgekehrter Reihenfolge darstellen, damit statische Objekte am Anfang des Array über die Szene gelegt werden)
  FOR i& = nCutSceneObjects&-1 TO 0 STEP -1
    objwidth! = cutSceneObjects(i&).width/2*uiscale!
    objheight! = cutSceneObjects(i&).height/2*uiscale!
    xpos! = (cutSceneObjects(i&).xpos-cutSceneScrollPos!)*uiscale!
    ypos! = cutSceneObjects(i&).ypos*uiscale!
    CALL GetCutSceneObjectSrcRect(INT(cutSceneObjects(i&).frame), xsrc0&, ysrc0&, xsrc1&, ysrc1&)
    D2D.GraphicStretch(hCutSceneElements&, xsrc0&, ysrc0&, xsrc1&, ysrc1&, xpos!-objwidth!, ypos!-objheight!, xpos!+objwidth!, ypos!+objheight!, cutSceneObjects(i&).opacity)
  NEXT i&

  'Titel darstellen
  IF t! > 1.0 THEN
    a$$ = GetGameMessageText(currentTextId&, 0, 1)
    IF UCASE$(LEFT$(a$$, 4)) = "^VOC" THEN a$$ = MID$(a$$, 6)
    i& = INSTR(a$$, CHR$(0))
    title$$ = UCASE$(LEFT$(a$$, i&-1))
    IF INSTR(title$$, "CHAPTER") > 0 OR INSTR(title$$, "KAPITEL") > 0 THEN
      'Kapitelnummer steht in der ersten Zeile
      title$$ = LEFT$(a$$, MIN&(i&-1, INT((t!-1)*%GAMEMESSAGE_SPEED)))
      totaltexttime! = (LEN(a$$)-i&)/%GAMEMESSAGE_SPEED+6
      a$$ = REMOVE$(MID$(a$$, i&+1), CHR$(0))  'aus dem restlichen Text die Absätze entfernen
    ELSE
      i& = INSTR(-1, a$$, CHR$(0))
      title$$ = MID$(a$$, i&+1)
      IF INSTR(UCASE$(title$$), "CHAPTER") > 0 OR INSTR(UCASE$(title$$), "KAPITEL") > 0 THEN
        'Kapitelnummer steht in der letzten Zeile
        totaltexttime! = (i&-1)/%GAMEMESSAGE_SPEED+6
        title$$ = LEFT$(title$$, INT((t!-1)*%GAMEMESSAGE_SPEED))
        a$$ = REMOVE$(LEFT$(a$$, i&-1), CHR$(0))  'aus dem restlichen Text die Absätze entfernen
      ELSE
        'kein Titel vorhanden
        title$$ = ""
        totaltexttime! = LEN(a$$)/%GAMEMESSAGE_SPEED+6
        a$$ = REMOVE$(a$$, CHR$(0))  'aus dem Text die Absätze entfernen
      END IF
    END IF
    D2D.GraphicTextSizeW(title$$, hHallOfFameCaptionFont&, textwd&, texthg&)
    xoffset& = (windowWidth&-textwd&)/2
    y& = 80
    D2D.GraphicPrintW(title$$, xoffset&+2, (y&+2)*uiscale!, brushBlack&, hHallOfFameCaptionFont&)
    D2D.GraphicPrintW(title$$, xoffset&, y&*uiscale!, brushGold&, hHallOfFameCaptionFont&)
  END IF

  'nächste Textzeile darstellen, wenn Ende der aktuellen erreicht wurde
  IF t! > 3.0 AND gametime! > cutSceneTextEndTime! THEN
    cutSceneTextSkip& = cutSceneTextSkip&+LEN(cutsceneCurrentTextLine$$)
    cutsceneCurrentTextLine$$ = ""
  END IF

  'Text darstellen (nur 1 Zeile)
  IF t! > 3.0 AND t! < totaltexttime! THEN
    IF cutsceneCurrentTextLine$$ = "" THEN
      cutsceneCurrentTextLine$$ = GetCutSceneTextLine(a$$, cutSceneTextSkip&, hCaptionFont&, windowWidth&-200)
      cutSceneTextStartTime! = gametime!
      cutSceneTextEndTime! = gametime!+LEN(cutsceneCurrentTextLine$$)/%GAMEMESSAGE_SPEED  'Zeitpunkt zum Löschen der Zeile und Beginn der nächsten Zeile
    END IF

    xoffset& = 100
    y& = 140
    c& = brushWhite&
    currentLineAvailableTime! = cutSceneTextEndTime!-cutSceneTextStartTime!-%CUTSCENEENDOFLINEPAUSESECONDS
    a$$ = LEFT$(cutsceneCurrentTextLine$$, LEN(cutsceneCurrentTextLine$$)*(gametime!-cutSceneTextStartTime!)/currentLineAvailableTime!)
    WHILE a$$ <> ""
      SELECT CASE ASC(a$$)
      CASE 1:  'Textfarbe: weiß
        n& = 1
        c& = brushWhite&
        a$$ = MID$(a$$, 2)
        ITERATE LOOP
      CASE 2:  'Textfarbe: rot
        n& = 1
        c& = brushRed&
        a$$ = MID$(a$$, 2)
        ITERATE LOOP
      CASE ELSE  'Text
        n& = INSTR(a$$, ANY CHR$(1,2))-1  'prüfen, ob Text Farbwechsel-Steuerzeichen enthält
        IF n& = -1 THEN n& = LEN(a$$)
        b$$ = LEFT$(a$$, n&)
        D2D.GraphicTextSizeW(b$$, hCaptionFont&, textwd&, texthg&)
        D2D.GraphicPrintW(b$$, xoffset&+2, (y&+2)*uiscale!, brushBlack&, hCaptionFont&)
        D2D.GraphicPrintW(b$$, xoffset&, y&*uiscale!, c&, hCaptionFont&)
        xoffset& = xoffset&+textwd&
      END SELECT
      a$$ = LTRIM$(MID$(a$$, n&+1))
    WEND
  END IF

  'Fade-In
  IF t! < 5.0 THEN
    opacity! = 0.004*t!^3-0.05*t!^2-0.054*t!+1  'https://www.mathepower.com/funktionen.php
    D2D.GraphicStretch(hCutSceneElements&, 1156, 110, 1254, 208, 0, 0, windowWidth&, windowHeight&, MAX(0.01, MIN(1.0, opacity!)))
  END IF

  'Debug-Informationen
  IF debugInfo& <> 0 THEN
    D2D.GraphicPrint("ObjCount: "+FORMAT$(nCutSceneObjects&), 300, 0, brushWhite&, hLobbyCaptionFont&)
  END IF

  'Zwischensequenz beenden (falls Text vollständig angezeigt und vorgelesen wurde und Hintergrund-Scrolling abgeschlossen ist)
  IF t! > totaltexttime!+2 AND cutSceneScrollPos! >= 3999-1080*aspectratio! AND (speechVolume& = 0 OR SAPIWAITUNTILDONE&(1)) THEN CALL EndCutScene
END SUB



'Aktuelle Textzeile der Zwischensequenz ermitteln
FUNCTION GetCutSceneTextLine(allText$$, skipcount&, textfont&, maxWidth&) AS WSTRING
  LOCAL a$$, p&, textwd&, texthg&

  a$$ = MID$(allText$$, skipcount&+1)

  DO
    D2D.GraphicTextSizeW(a$$, textfont&, textwd&, texthg&)
    IF textwd& <= maxWidth& THEN EXIT LOOP
    p& = INSTR(-1, a$$, " ")
    IF p& = 0 THEN EXIT LOOP
    a$$ = LEFT$(a$$, p&-1)
  LOOP

  p& = LEN(a$$)
  IF ASC(allText$$, p&+skipcount&+1) = 32 THEN p& = p&+1

  GetCutSceneTextLine$$ = MID$(allText$$, skipcount&+1, p&)
END FUNCTION



'Video Zwischensequenz anzeigen
SUB RenderVideoCutScene
  LOCAL tmillisecs&, currentFrame&, pixeldata$, aspectratio!
  LOCAL audioStartPosition&, audiodata$
  LOCAL destx!, desty!, destwidth!, destheight!

  'darzustellenden Frame ermitteln
  tmillisecs& = (gametime!-cutSceneStartTime!) * 1000
  IF tmillisecs& <= 0 THEN EXIT SUB
  currentFrame& = GetAVIVideoFrameNumberForMillisecond&(tmillisecs&)
  IF currentFrame& < 0 OR currentFrame& >= videoFrameCount& THEN
    CALL EndCutScene
    EXIT SUB
  END IF

  'Frame ggf. laden und Textur dafür erstellen
  IF currentFrame& <> currentVideoFrame& THEN
    pixeldata$ = GetAVIFramePixelData$(currentFrame&)
    IF hVideoFrame& = 0 THEN
      hVideoFrame& = D2D.CreateMemoryBitmap(videoFrameWidth&, videoFrameHeight&, pixeldata$)
    ELSE
      D2D.ReuseMemoryBitmap(hVideoFrame&, videoFrameWidth&, videoFrameHeight&, pixeldata$)
    END IF
    currentVideoFrame& = currentFrame&
  END IF

  'nächstes Audio-Sample abspielen
  IF soundchannels(%SOUNDBUFFER_VIDEO).IsPlaying = 0 OR gametime! >= videoSoundTrackUpdateTime!+1 THEN
    audioStartPosition& = tmillisecs&*audioSamplesPerSecond&/1000
    audiodata$ = MID$(audioStreamData$, audioStartPosition&*2+1, audioSamplesPerSecond&*2)  '1 Sekunde Audio-Daten
    IF hVideoSoundTrack& = 0 THEN
      hVideoSoundTrack& = DS.AddWaveData(audiodata$, audioSamplesPerSecond&, 1)
    ELSE
      DS.SetWaveData(hVideoSoundTrack&, audiodata$, audioSamplesPerSecond&, 1)
    END IF
    CALL PlaySoundEffect(hVideoSoundTrack&, %SOUNDBUFFER_VIDEO, %PLAYFLAGS_NONE)
    videoSoundTrackUpdateTime! = gametime!
  END IF

  'Hintergrund löschen
  D2D.GraphicBox(0, 0, windowWidth&, windowHeight&, brushBlack&, brushBlack&)

  'Frame darstellen
  aspectratio! = videoFrameWidth&/videoFrameHeight&
  destheight! = windowHeight&
  destwidth! = destheight!*aspectratio!
  IF destwidth! > windowWidth& THEN
    destwidth! = windowWidth&
    destheight! = destwidth!/aspectratio!
  END IF
  destx! = (windowWidth&-destwidth!)/2
  desty! = (windowHeight&-destheight!)/2
  D2D.GraphicStretch(hVideoFrame&, 0, 0, videoFrameWidth&, videoFrameHeight&, destx!, desty!, destx!+destwidth!, desty!+destheight!)
END SUB



'Credits anzeigen
SUB RenderCredits
  LOCAL a$$, u$, upos$, t!, bgpos!, scrollpos!, opacity!, i&, x&, y&, unittp&, textwd&, texthg&

  t! = gametime!-creditStartTime!
  scrollpos! = t!*100
  IF scrollpos! > 9000 THEN
    CALL EndCredits
    EXIT SUB
  END IF

  'Hintergrund
  bgpos! = t!*5.3
  D2D.GraphicStretch(hDialog&, txarea_gradiantblueblack.left, txarea_gradiantblueblack.top+bgpos!, txarea_gradiantblueblack.right, txarea_gradiantblueblack.top+bgpos!+32, 0, 0, windowWidth&, windowHeight&)

  'Einheiten ein/ausblenden
  u$ = CHR$(8,11,13 , 3,4,6 , 10,17,12 , 27,28,29 , 35,37,38 , 34,32,33 , 48,49,45 , 47,42,46 , 18,2,19)
  upos$ = MKI$(2000)+MKI$(2900)+MKI$(3500)+MKI$(4300)+MKI$(5000)+MKI$(5600)+MKI$(6300)+MKI$(6900)+MKI$(7500)
  FOR i& = 1 TO LEN(u$)
    unittp& = ASC(u$, i&)
    x& = (i&-1) MOD 3
    y& = CVI(upos$, INT((i&-1)/3)*2+1)
    opacity! = y&-scrollpos!+x&*20-300
    IF opacity! > 0 AND opacity! < 400 THEN
      opacity! = opacity!/200
      IF opacity! > 1 THEN opacity! = 2-opacity!
      IF unitAnimFrameWidth& = 0 THEN
        D2D.GraphicStretch(channelsnosave(0).unitclasses(unittp&).artworkhandle, 0, 0, 800, 600, (160+x&*600)*uiscale!, (y&-scrollpos!)*uiscale!, (560+x&*600)*uiscale!, (y&+300-scrollpos!)*uiscale!, opacity!)
      ELSE
        CALL DrawUnitAnimation(unittp&, (160+x&*600)*uiscale!, (y&-scrollpos!)*uiscale!, (560+x&*600)*uiscale!, (y&+300-scrollpos!)*uiscale!, opacity!)
      END IF
      D2D.GraphicStretch(hHudElements&, 513, 412, 1059, 452, (160+x&*600)*uiscale!, (y&-scrollpos!-10)*uiscale!, (560+x&*600)*uiscale!, (y&+30-scrollpos!)*uiscale!, opacity!)
      D2D.GraphicStretch(hHudElements&, 513, 30, 1059, 410, (160+x&*600)*uiscale!, (y&-scrollpos!+30)*uiscale!, (560+x&*600)*uiscale!, (y&+300-scrollpos!)*uiscale!, opacity!)
      IF opacity! > 0.25 THEN
        a$$ = channelsnosave(0).unitclasses(unittp&).uname
        D2D.GraphicTextSizeW(a$$, hGameMessageFont&, textwd&, texthg&)
        D2D.GraphicPrintW(a$$, (360+x&*600)*uiscale!-textwd&/2, (y&-scrollpos!-texthg&/2+2)*uiscale!, brushWhite&, hGameMessageFont&)
      END IF
    END IF
  NEXT i&

  'Battle Isle 2020
  D2D.GraphicStretch(hIntro&, txarea_introbattleisle.left, txarea_introbattleisle.top, txarea_introbattleisle.right, txarea_introbattleisle.bottom, 474*uiscale!, (1200-scrollpos!)*uiscale!, 1446*uiscale!, (1330-scrollpos!)*uiscale!)
  D2D.GraphicStretch(hIntro&, txarea_intro2020.left, txarea_intro2020.top, txarea_intro2020.right+360, txarea_intro2020.bottom, 764*uiscale!, (1360-scrollpos!)*uiscale!, 1156*uiscale!, (1490-scrollpos!)*uiscale!)
  '
  D2D.GraphicStretch(hIntro&, txarea_introbattleisle.left, txarea_introbattleisle.top, txarea_introbattleisle.right, txarea_introbattleisle.bottom, 474*uiscale!, (8400-scrollpos!)*uiscale!, 1446*uiscale!, (8530-scrollpos!)*uiscale!)
  D2D.GraphicStretch(hIntro&, txarea_intro2020.left, txarea_intro2020.top, txarea_intro2020.right+360, txarea_intro2020.bottom, 764*uiscale!, (8560-scrollpos!)*uiscale!, 1156*uiscale!, (8690-scrollpos!)*uiscale!)

  'Produzent
  a$$ = words$$(%WORD_CREDIT_PRODUCER)
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (1800-scrollpos!)*uiscale!, brushWhite&, hBigCaptionFont&)
  a$$ = "Thomas Hertzler"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (1900-scrollpos!)*uiscale!, brushGold&, hCreditFont&)

  'Programm und Design
  a$$ = words$$(%WORD_CREDIT_CODING)
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (2400-scrollpos!)*uiscale!, brushWhite&, hBigCaptionFont&)
  a$$ = "Bernhard Ewers"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (2500-scrollpos!)*uiscale!, brushGold&, hCreditFont&)
  a$$ = "Patric Lagny"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (2600-scrollpos!)*uiscale!, brushGold&, hCreditFont&)
  a$$ = "Thomas Häuser"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (2700-scrollpos!)*uiscale!, brushGold&, hCreditFont&)
  a$$ = "Rolf Neumann"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (2800-scrollpos!)*uiscale!, brushGold&, hCreditFont&)

  'Programmierung BI2020
  a$$ = words$$(%WORD_CREDIT_CODING2020)
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (3300-scrollpos!)*uiscale!, brushWhite&, hBigCaptionFont&)
  a$$ = "Daniel Bekowies"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (3400-scrollpos!)*uiscale!, brushGold&, hCreditFont&)

  'Grafische Gestaltung
  a$$ = words$$(%WORD_CREDIT_GRAPHICS)
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (3900-scrollpos!)*uiscale!, brushWhite&, hBigCaptionFont&)
  a$$ = "Thorsten Knop"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (4000-scrollpos!)*uiscale!, brushGold&, hCreditFont&)
  a$$ = "Janos Toth"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (4100-scrollpos!)*uiscale!, brushGold&, hCreditFont&)
  a$$ = "Christoph Werner"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (4200-scrollpos!)*uiscale!, brushGold&, hCreditFont&)

  'Zusätzliche Grafiken BI2020
  a$$ = words$$(%WORD_CREDIT_GRAPHICS2020)
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (4700-scrollpos!)*uiscale!, brushWhite&, hBigCaptionFont&)
  a$$ = "waheedaslam"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (4800-scrollpos!)*uiscale!, brushGold&, hCreditFont&)

  'Musik und Soundeffekte
  a$$ = words$$(%WORD_CREDIT_MUSIC)
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (5400-scrollpos!)*uiscale!, brushWhite&, hBigCaptionFont&)
  a$$ = "Haiko Ruttmann"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (5500-scrollpos!)*uiscale!, brushGold&, hCreditFont&)

  'Ingame-Texte
  a$$ = words$$(%WORD_CREDIT_TEXTS)
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (6000-scrollpos!)*uiscale!, brushWhite&, hBigCaptionFont&)
  a$$ = "Janos Toth"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (6100-scrollpos!)*uiscale!, brushGold&, hCreditFont&)
  a$$ = "Stefan Piasecki"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (6200-scrollpos!)*uiscale!, brushGold&, hCreditFont&)

  'Ingame-Texte BI2020
  a$$ = words$$(%WORD_CREDIT_TEXTS2020)
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (6700-scrollpos!)*uiscale!, brushWhite&, hBigCaptionFont&)
  a$$ = "Daniel Bekowies"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (6800-scrollpos!)*uiscale!, brushGold&, hCreditFont&)

  'Kitanas Schloß Kampagne
  a$$ = words$$(%WORD_CREDIT_KITANACAMPAIGN)
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (7300-scrollpos!)*uiscale!, brushWhite&, hBigCaptionFont&)
  a$$ = "Daniel Bekowies"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (7400-scrollpos!)*uiscale!, brushGold&, hCreditFont&)

  'Danke fürs Mitspielen
  a$$ = words$$(%WORD_CREDIT_THANKS)
  D2D.GraphicTextSizeW(a$$, hBigCaptionFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (7900-scrollpos!)*uiscale!, brushGold&, hBigCaptionFont&)

  'Webseite
  a$$ = "http://www.kitana.org/bi2020"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (8800-scrollpos!)*uiscale!, brushBlue&, hCreditFont&)
  '
  a$$ = "https://www.fiverr.com/waheedaslam"
  D2D.GraphicTextSizeW(a$$, hCreditFont&, textwd&, texthg&)
  D2D.GraphicPrintW(a$$, 960*uiscale!-textwd&/2, (4900-scrollpos!)*uiscale!, brushBlue&, hCreditFont&)
END SUB



'Minimap darstellen
SUB RenderMiniMap
  D2D.CreateClippingRegion(minimaparea.left, minimaparea.top, minimaparea.right, minimaparea.bottom)
  D2D.GraphicStretch(hMinimap&, 0, 0, 344, 344, minimaparea.left-12*uiscale!, minimaparea.top-12*uiscale!, minimaparea.right+12*uiscale!, minimaparea.bottom+12*uiscale!)
  D2D.ReleaseClippingRegion
END SUB



'Ping anzeigen
SUB RenderPing
  LOCAL a$, elapsedMillisecs&

  elapsedMillisecs& = pingMillisecs&
  IF elapsedMillisecs& = -999 THEN elapsedMillisecs& = (TIMER-pingSentTime!)*1000
  a$ = "Ping "+FORMAT$(elapsedMillisecs&)+" ms"
  D2D.GraphicPrint(a$, 130*uiscale!, -1*uiscale!, brushWhite&, hGameMessageFont&)
END SUB



'Karten-Koordinaten anzeigen
SUB RenderCoordinateInfo
  LOCAL a$, mapx&, mapy&, textwd&, texthg&

  IF IsInRect&(mousexpos&, mouseypos&, maparea) <> 0 THEN
    CALL GetMapPos(mousexpos&, mouseypos&, mapx&, mapy&)
    IF mapx& >= 0 THEN
      a$ = FORMAT$(mapx&)+","+FORMAT$(mapy&)
      D2D.GraphicTextSize(a$, hWeaponFont&, textwd&, texthg&)
      D2D.GraphicPrint(a$, mousexpos&-textwd&/2, mouseypos&-texthg&+1, brushWhite&, hWeaponFont&)
    END IF
  END IF
END SUB



'HUD darstellen
SUB RenderSkin
  LOCAL leftcutpos&, rightcutpos&, vleftwidth&, vrightwidth&

  IF windowWidth&*9 = windowHeight&*16 THEN
    'Fenster hat dasselbe Seitenverhältnis wie die Skin-Textur
    D2D.GraphicStretch(hSkin&, 0, 0, 1920, 1080, 0, 0, windowWidth&, windowHeight&)
  ELSE
    'Fenster hat anderes Seitenverhältnis
    leftcutpos& = 200
    rightcutpos& = 1500
    vleftwidth& = leftcutpos&*uiscale!
    vrightwidth& = (1920-rightcutpos&)*uiscale!
    D2D.GraphicStretch(hSkin&, 0, 0, leftcutpos&, 1080, 0, 0, vleftwidth&, windowHeight&)
    D2D.GraphicStretch(hSkin&, rightcutpos&, 0, 1920, 1080, windowWidth&-vrightwidth&, 0, windowWidth&, windowHeight&)
    D2D.GraphicStretch(hSkin&, leftcutpos&, 0, rightcutpos&, 1080, vleftwidth&-1, 0, windowWidth&-vrightwidth&+1, windowHeight&)
  END IF
END SUB



'Minimap erzeugen
SUB CreateMiniMap
  LOCAL mapwd&, maphg&, zoomfactor#

  'Karte zeichnen (auf 344x344 Pixel skaliert)
  mapwd& = channels(0).info.xsize*16
  maphg& = channels(0).info.ysize*24
  zoomfactor# = 344/MAX(mapwd&, maphg&)
  CALL CreateScreenshot(hMinimap&, 344, 344, zoomfactor#, 0)
  updateMiniMap& = 0
END SUB



'Kartenvorschau für Lobby erzeugen
SUB CreateMapPreview
  LOCAL mapwd&, maphg&, zoomfactor#

  'Karte zeichnen (auf 240x240 Pixel skaliert)
  mapwd& = channels(0).info.xsize*16
  maphg& = channels(0).info.ysize*24
  zoomfactor# = 240/MAX(mapwd&, maphg&)
  CALL CreateScreenshot(hMapPreview&, 240, 240, zoomfactor#, 2)
  updateMapPreview& = 0
  mapPreviewCreated& = 1
END SUB



'Screenshot der Karte erzeugen
SUB CreateScreenshot(hTargetBitmap&, destwidth&, destheight&, zoomfactor#, md&)
  LOCAL orgzoom#, orgscrollx&, orgscrolly&, mapwd&, maphg&

  CALL EnterSemaphore(semaphore_scrollpos&)
  screenShoting& = 1

  'Hintergrundfarbe (wie nicht erforschtes Gebiet)
  D2D.GraphicBox(maparea.left, maparea.top, maparea.left+destwidth&, maparea.top+destheight&, brushUnexplored&, brushUnexplored&)

  'Originalparameter merken
  orgzoom# = zoom#
  orgscrollx& = scrollX&
  orgscrolly& = scrollY&

  'Karte zeichnen
  mapwd& = channels(0).info.xsize*16
  maphg& = channels(0).info.ysize*24
  zoom# = zoomfactor#
  scrollX& = mapwd&*zoom#/2-destwidth&/2
  scrollY& = maphg&*zoom#/2-destheight&/2
  mapDrawOptions& = md&
  CALL RenderMap
  D2D.GraphicCopyFromRenderTarget(hTargetBitmap&, maparea.left, maparea.top, maparea.left+destwidth&, maparea.top+destheight&)

  'Originalparameter wiederherstellen
  zoom# = orgzoom#
  scrollX& = orgscrollx&
  scrollY& = orgscrolly&
  mapDrawOptions& = 0
  screenShoting& = 0
  CALL LeaveSemaphore(semaphore_scrollpos&)
END SUB



'Bildschirm rendern
SUB RenderScene(RenderTarget AS ID2D1HwndRenderTarget)
  LOCAL i&, mapx&, mapy&, v&, infoshown&, fps&, deltatime!, headerWidth!, destx!, desty!
  LOCAL c AS IDXCONTROL

  'Programm-Initialisierung
  IF gameState& <> %GAMESTATE_ERROR AND gameState& < %GAMESTATE_INGAME THEN CALL ShowInitProgress

  'Credits
  IF gameState& = %GAMESTATE_CREDITS THEN
    CALL RenderCredits
    EXIT SUB
  END IF

  'Zwischensequenzen
  IF gameState& = %GAMESTATE_CUTSCENE THEN
    CALL RenderCutScene
    CALL RenderDebugInfo
    EXIT SUB
  END IF

  'Minimap zeichnen
  IF gameState& = %GAMESTATE_INGAME THEN
    IF updateMiniMap& = 1 THEN
      CALL CreateMiniMap
    END IF
  END IF

  'Kartenvorschau erzeugen (für Multiplayer-Lobby)
  IF updateMapPreview& = 1 THEN
    CALL CreateMapPreview
  END IF

  'Skin darstellen
  IF hSkin& > -1 THEN CALL RenderSkin

  'Debug-Informationen anzeigen
  CALL RenderDebugInfo

  'Fehler-Zustand
  IF gameState& = %GAMESTATE_ERROR THEN
    CALL RenderMessages
    IF menuOpenTime! > 0 THEN CALL RenderDialogues
    EXIT SUB
  END IF

  'Ladebildschirm
  IF (gameState& = %GAMESTATE_INIT OR gameState& = %GAMESTATE_NONE) AND hLoadingScreen& > 0 THEN
    D2D.GraphicBox(maparea.left, maparea.top, maparea.right, maparea.bottom, brushWhite&, brushBlack&)
    IF unitAnimFrameWidth& = 0 THEN
      D2D.GraphicStretch(hLoadingScreen&, 0, 0, 800, 600, (maparea.right+maparea.left)/2-400, (maparea.bottom+maparea.top)/2-300, (maparea.right+maparea.left)/2+400, (maparea.bottom+maparea.top)/2+300)
    ELSE
      CALL DrawUnitAnimation(loadingScreenUnitType&, (maparea.right+maparea.left-unitAnimFrameWidth&)/2, (maparea.bottom+maparea.top-unitAnimFrameHeight&)/2, _
                           (maparea.right+maparea.left+unitAnimFrameWidth&)/2, (maparea.bottom+maparea.top+unitAnimFrameHeight&)/2)
    END IF
    IF hDialog& >= 0 THEN
      headerWidth! = (txarea_milopheader.right-txarea_milopheader.left)*uiscale!
      destx! = (maparea.left+maparea.right)/2-headerWidth!/2
      desty! = maparea.bottom-37*uiscale!
      D2D.GraphicStretch(hDialog&, txarea_milopheader.left, txarea_milopheader.top, txarea_milopheader.right, txarea_milopheader.bottom, destx!, desty!, destx!+headerWidth!, desty!+(txarea_milopheader.bottom-txarea_milopheader.top)*uiscale!)
    END IF
  END IF

  'Intro
  IF gameState& = %GAMESTATE_INTRO THEN
    CALL RenderIntro
    CALL RenderDialogues  'MILOP Logo
    CALL RenderMessages
    c = D2D.GetControlAtPos(mousexpos&, mouseypos&)
    IF ISOBJECT(c) THEN CALL RenderToolTip(c, mousexpos&, mouseypos&)
    EXIT SUB
  END IF

  'Kartenbereich
  IF gameState& = %GAMESTATE_INGAME AND showProtocol& = 0 THEN CALL RenderMap
  IF gameState& = %GAMESTATE_INGAME AND showProtocol& <> 0 THEN CALL RenderProtocol

  'Minimap
  IF gameState& = %GAMESTATE_INGAME THEN CALL RenderMiniMap

  'Meldungen
  CALL RenderMessages

  'Einheiten/Shop-Info
  IF GetPhase&(0, localPlayerNr&) <> %PHASE_MAINMENU AND gameState& = %GAMESTATE_INGAME THEN
    infoshown& = 0
    IF selectedShop& >= 0 THEN
      v& = GetShopUnitAtMousePos&(0)
      IF v& = -1 THEN v& = channels(0).player(localPlayerNr&).selectedunit
      IF v& >= 0 THEN
        infoshown& = RenderUnitInfo&(v&)
      ELSE
        IF productionPreviewUnit& >= 0 THEN infoshown& = RenderUnitInfo&(productionPreviewUnit&)
      END IF
      IF infoshown& = 0 THEN infoshown& = RenderShopInfo&(selectedShop&)
    ELSE
      CALL GetMapPos(mousexpos&, mouseypos&, mapx&, mapy&)
      IF mapx& >= 0 AND channels(0).zone3(mapx&, mapy&) <> -1 AND showProtocol& = 0 THEN
        v& = channels(0).zone3(mapx&, mapy&)
        IF v& >= 0 THEN
          infoshown& = RenderUnitInfo&(v&)
        ELSE
          infoshown& = RenderShopInfo&(-2-v&)
        END IF
      END IF
      IF infoshown& = 0 THEN
        v& = -1
        IF cursorXPos& >= 0 AND replayMode&(0) < %REPLAYMODE_PLAY THEN v& = channels(0).zone3(cursorXPos&, cursorYPos&)
        SELECT CASE v&
        CASE -1
          IF channels(0).player(localPlayerNr&).selectedunit >= 0 THEN infoshown& = RenderUnitInfo&(channels(0).player(localPlayerNr&).selectedunit)
        CASE >= 0
          infoshown& = RenderUnitInfo&(v&)
        CASE ELSE
          infoshown& = RenderShopInfo&(-2-v&)
        END SELECT
      END IF
    END IF
  END IF

  IF debugShowUnitList& <> 0 THEN CALL RenderUnitList
  IF debugShowChannelInfo& <> 0 THEN CALL RenderChannelInfo

  'aktiven Dialog oder MILOP Logo darstellen
  CALL RenderDialogues

  'Missionsende-Bildschirm
  IF gameState& = %GAMESTATE_GAMEOVER THEN
    IF channels(0).info.state = %CHANNELSTATE_VICTORY OR channels(0).info.state = %CHANNELSTATE_VICTORYBONUS THEN CALL RenderVictory
    IF channels(0).info.state = %CHANNELSTATE_DEFEAT THEN CALL RenderDefeat
  END IF

  'Ping
  IF enablePing& = 1 AND connectedToAuthenticServer& = 1 THEN CALL RenderPing

  'Karten-Koordinaten
  IF coordinateInfo& <> 0 AND gameState& = %GAMESTATE_INGAME AND showProtocol& = 0 THEN CALL RenderCoordinateInfo

  'Tooltips
  c = D2D.GetControlAtPos(mousexpos&, mouseypos&)
  IF ISOBJECT(c) THEN CALL RenderToolTip(c, mousexpos&, mouseypos&)

  'DEBUG DEBUG DEBUG
'  IF gameState& = %GAMESTATE_INGAME THEN D2D.GraphicStretch(hAnimations&(325), 0, 0, 160*8, 64*7, maparea.left, maparea.top, maparea.left+160*8, maparea.top+64*7)
'  IF gameState& = %GAMESTATE_INGAME THEN D2D.GraphicStretch(hRoads&(3), 0, 0, 40*24, 3*24, maparea.left, maparea.top, maparea.left+40*24, maparea.top+3*24)
END SUB



'Maus-Click auf Karte
SUB MapClick(x&, y&)
  LOCAL mapx&, mapy&, unitnr&, targetunit&, tg&, phase&, shopnr&, mask&, mask2&

  IF replayMode&(0) >= %REPLAYMODE_PLAY THEN EXIT SUB
  phase& = GetPhase&(0, localPlayerNr&)

  'Pixel-Koordinaten in Felder umrechnen
  CALL GetMapPos(x&, y&, mapx&, mapy&)
  IF mapx& >= 0 THEN
    cursorXPos& = mapx&
    cursorYPos& = mapy&
  END IF

  IF phase& = %PHASE_UNITSELECTED THEN
    'Aktionsziel auswählen
    tg& = channels(0).player(localPlayerNr&).targets(mapx&, mapy&)
    IF mapx& >= 0 AND tg& <> 0 THEN
      targetunit& = channels(0).zone3(mapx&, mapy&)
      mask& = %TG_MOVE OR %TG_REPAIR OR %TG_REFUEL
      mask2& = %TG_MOVE OR %TG_REPAIR OR %TG_RECHARGE
      IF CountBits&(tg& AND mask&) > 1 OR CountBits&(tg& AND mask2&) > 1 THEN
        'Einheit hat mehrere Optionen für dieses Feld (z.B. REX -> SUPER-VIRUS oder Zieleinheit kann sowohl repariert als auch befüllt werden)
        CALL ShowSupportMenu(0, channels(0).player(localPlayerNr&).selectedunit, tg&, mapx&, mapy&)
        EXIT SUB
      END IF
      IF (tg& AND %TG_MOVE) <> 0 THEN
        'Bewegen
        IF tg& = %TG_MOVE THEN
          IF gameMode& = %GAMEMODE_SINGLE THEN
            CALL MoveUnit(channels(0).player(localPlayerNr&).selectedunit, mapx&, mapy&)
          ELSE
            CALL ClientMoveUnit(channels(0).player(localPlayerNr&).selectedunit, mapx&, mapy&)
          END IF
        ELSE
          CALL ShowBuildMenu(0, channels(0).player(localPlayerNr&).selectedunit, mapx&, mapy&)
        END IF
        EXIT SUB
      ELSE
        IF (tg& AND %TG_ATTACK) <> 0 THEN
          'Angreifen
          CALL Attack(channels(0).player(localPlayerNr&).selectedunit, mapx&, mapy&)
          EXIT SUB
        END IF
        IF (tg& AND %TG_REPAIR) <> 0 THEN
          'Reparieren
          unitnr& = channels(0).player(localPlayerNr&).selectedunit
          CALL Repair(0, unitnr&, targetunit&)  'wird im Multiplayer Modus anschließend durch Serverdaten überschrieben - berechnet/löscht aber die Ziele
          IF gameMode& = %GAMEMODE_CLIENT THEN
            CALL ClientUnitAction(unitnr&, %UNITACTION_REPAIR, channels(0).units(targetunit&).xpos, channels(0).units(targetunit&).ypos)
          END IF
          EXIT SUB
        END IF
        IF (tg& AND %TG_REFUEL) <> 0 OR (tg& AND %TG_RECHARGE) <> 0 THEN
          'Befüllen
          unitnr& = channels(0).player(localPlayerNr&).selectedunit
          CALL Refuel(0, unitnr&, targetunit&)  'wird im Multiplayer Modus anschließend durch Serverdaten überschrieben - berechnet/löscht aber die Ziele
          IF gameMode& = %GAMEMODE_CLIENT THEN
            CALL ClientUnitAction(unitnr&, %UNITACTION_REFUEL, channels(0).units(targetunit&).xpos, channels(0).units(targetunit&).ypos)
          END IF
          EXIT SUB
        END IF
        IF (tg& AND %TG_BUILD) <> 0 THEN
          'Bauen
          CALL ShowBuildMenu(0, channels(0).player(localPlayerNr&).selectedunit, mapx&, mapy&)
          EXIT SUB
        END IF
      END IF
    END IF
  END IF

  IF phase& = %PHASE_NONE OR phase& = %PHASE_UNITSELECTED THEN
    'Einheit auswählen
    IF mapx& >= 0 AND (channels(0).vision(mapx&, mapy&) AND localPlayerMask&) <> 0 AND channels(0).zone3(mapx&, mapy&) >= 0 AND channels(0).units(channels(0).zone3(mapx&, mapy&)).owner = localPlayerNr& THEN
      IF selectedShop& >= 0 THEN CALL ExitButtonPressed(0)  'Shop schließen
      CALL SelectUnit(channels(0).zone3(mapx&, mapy&), 0)
      EXIT SUB
    END IF
    'Shop wählen
    IF mapx& >= 0 AND (channels(0).vision(mapx&, mapy&) AND localPlayerMask&) <> 0 AND channels(0).zone3(mapx&, mapy&) < -1 THEN
      shopnr& = -2-channels(0).zone3(mapx&, mapy&)
      IF (channels(0).player(localPlayerNr&).allymask AND 2^channels(0).shops(shopnr&).owner) <> 0 OR channels(0).shops(shopnr&).owner = 6 THEN
        CALL SelectShop(shopnr&)
        EXIT SUB
      END IF
    END IF
  END IF
END SUB



'Maus-Click auf die Minikarte
SUB MinimapClick(BYVAL x&, BYVAL y&)
  LOCAL mapwd&, maphg&, d&

  'Seitenverhältnis der Karte berechnen
  mapwd& = channels(0).info.xsize*16
  maphg& = channels(0).info.ysize*24
  IF mapwd& > maphg& THEN
    maphg& = maphg&*320/mapwd&
    mapwd& = 320
    d& = (mapwd&-maphg&)/2
    IF y& < d& OR y& > 320-d& THEN EXIT SUB
    y& = y&-d&
  ELSE
    mapwd& = mapwd&*320/maphg&
    maphg& = 320
    d& = (maphg&-mapwd&)/2
    IF x& < d& OR x& > 320-d& THEN EXIT SUB
    x& = x&-d&
  END IF

  'Karte um das angeklickte Feld zentrieren
  CALL ScrollToMapPos(channels(0).info.xsize*x&/mapwd&, channels(0).info.ysize*y&/maphg&, 0.5)
END SUB



'Maus-Click auf Einheitenbild
SUB UnitPicClick(x&, y&)
  LOCAL i&, transporter&, unitnr&

  'falls gewählte Einheit ein Transporter ist, dann geladene Einheit auswählen
  transporter& = channels(0).player(localPlayerNr&).selectedunit
  IF transporter& >= 0 AND (channelsnosave(0).unitclasses(channels(0).units(transporter&).unittype).flags AND %UCF_TRANSPORTER) <> 0 AND y& >= unitpicarea.bottom-38*uiscale! AND y& < unitpicarea.bottom-2*uiscale! THEN
    FOR i& = 0 TO 7
      IF x& >= unitpicarea.left+2*uiscale!+i&*40*uiscale! AND x& < unitpicarea.left+38*uiscale!+i&*40*uiscale! THEN
        unitnr& = channels(0).units(transporter&).transportcontent(i&)
        lastSelectedTransporter& = transporter&
        IF unitnr& >= 0 THEN CALL SelectUnit(unitnr&, 0)
      END IF
    NEXT i&
  END IF
END SUB



'Maus-Click auf Multiplayer-Lobby
SUB LobbyClick(x&, y&)
  LOCAL lobbychnr&

  IF x& < activedialoguearea.left+30*uiscale! OR x& > activedialoguearea.left+634*uiscale! THEN EXIT SUB

  'Channel wählen
  lobbychnr& = INT((y&-activedialoguearea.top-88*uiscale!)/(20*uiscale!))
  IF lobbychnr& < 0 OR lobbychnr& >= LEN(lobbyChannels$) THEN EXIT SUB
  selectedLobbyChannel& = ASC(lobbyChannels$, lobbychnr&+1)
  buttonJoinGame.Enabled = 1
END SUB



'Maus-Click auf Shop-Dialog
SUB ShopClick(x&, y&)
  LOCAL unitnr&, cursorpos&

  IF GetPhase&(0, localPlayerNr&) > %PHASE_UNITSELECTED THEN EXIT SUB
  cursorpos& = shopCursorPos&
  unitnr& = GetShopUnitAtMousePos&(1)
  IF unitnr& >= 0 THEN
    CALL CheckShopRefuel(unitnr&)
    CALL SelectUnit(unitnr&, 0)
  ELSE
    unitnr& = GetShopProductionAtMousePos&
    IF unitnr& >= 0 THEN
      CALL SelectProduction(unitnr&)
    ELSE
      IF cursorpos& <> shopCursorPos& THEN
        'es wurde ein Slot ohne Einheit angewählt
        buttonShopBuild.Enabled = 0
        buttonShopMove.Enabled = 0
        buttonShopRefuel.Enabled = 0
        buttonShopRepair.Enabled = 0
        buttonShopTrain.Enabled = 0
      END IF
    END IF
  END IF
END SUB



'Spielfeld zoomen
SUB ZoomMap(newZoom#)
  LOCAL wd&, hg&, oldZoom#

  oldZoom# = zoom#
  wd& = (maparea.right-maparea.left)/2
  hg& = (maparea.bottom-maparea.top)/2
  zoom# = MAX(1.0, MIN(6.0, newZoom#))
  scrollX& = MAX&(0, MIN&(channels(0).info.xsize*16*zoom#-maparea.right+maparea.left+8*zoom#, (scrollX&+wd&)/oldZoom#*zoom#-wd&))
  scrollY& = MAX&(0, MIN&(channels(0).info.ysize*24*zoom#-maparea.bottom+maparea.top+12*zoom#, (scrollY&+hg&)/oldZoom#*zoom#-hg&))
END SUB



'Maus-Bewegung verarbeiten
SUB MouseMove(x&, y&)
  LOCAL i&, j&, btn&

  mousexpos& = x&
  mouseypos& = y&
  currentArea& = GetAreaAtMousePos&

  'Karte scrollen
  IF dragStartX& >= 0 THEN
    scrollEndTime! = -1  'automatisches Scrollen abbrechen
    scrollX& = MAX&(0, MIN&(channels(0).info.xsize*16*zoom#-maparea.right+maparea.left+8*zoom#, scrollX&+dragStartX&-x&))
    scrollY& = MAX&(0, MIN&(channels(0).info.ysize*24*zoom#-maparea.bottom+maparea.top+12*zoom#, scrollY&+dragStartY&-y&))
    dragStartX& = x&
    dragStartY& = y&
  END IF
END SUB



'Maus-Klicks verarbeiten
'md: 1 = links gedrückt , 2 = links losgelassen , 3 = rechts gedrückt , 4 = rechts losgelassen
SUB MouseClick(x&, y&, md&)
  LOCAL mapx&, mapy&, phase&, tg&, targetunit&, shopnr&

  IF md& = 1 OR md& = 3 THEN
    mousedownx& = x&
    mousedowny& = y&
  ELSE
    dragStartX& = -1
    IF ABS(mousedownx&-x&) > 10 OR ABS(mousedowny&-y&) > 10 THEN EXIT SUB
  END IF
  D2D.OnClick(x&, y&, md&)
  phase& = GetPhase&(0, localPlayerNr&)

  SELECT CASE md&
  CASE 1:  'links gedrückt
    IF x& >= maparea.left AND x& <= maparea.right AND y& >= maparea.top AND y& <= maparea.bottom THEN
      dragStartX& = x&
      dragStartY& = y&
    END IF

  CASE 2:  'links losgelassen
    IF menuOpenTime! > 0 AND IsInRect&(x&, y&, activedialoguearea) <> 0 THEN
      'Menüeintrag auswählen, wenn Menü angeklickt wurde
      CALL CloseMenu(x&, y&)
      EXIT SUB
    END IF

    IF messageOpenTime! > 0 AND IsInRect&(x&, y&, activedialoguearea) <> 0 AND dialogueClosing& = 0 THEN
      'Nachricht schließen, wenn Nachricht angeklickt wurde
      CALL CloseGameMessage
      EXIT SUB
    END IF

    IF lobbyOpenTime! > 0 AND IsInRect&(x&, y&, activedialoguearea) <> 0 THEN
      'Lobby-Aktion ausführen, wenn Lobby angeklickt wurde
      CALL LobbyClick(x&, y&)
      EXIT SUB
    END IF

    IF selectedShop& >= 0 AND IsInRect&(x&, y&, activedialoguearea) <> 0 THEN
      'Einheit oder Produktionspalette im Shop angeklickt
      CALL ShopClick(x&, y&)
      EXIT SUB
    END IF

    IF IsInRect&(x&, y&, maparea) <> 0 AND IsInRect&(x&, y&, activedialoguearea) = 0 AND gameState& = %GAMESTATE_INGAME AND showProtocol& = 0 THEN
      'Klick auf Spielfeld
      CALL MapClick(x&, y&)
    END IF

    IF IsInRect&(x&, y&, unitpicarea) <> 0 AND gameState& = %GAMESTATE_INGAME THEN
      'Klick auf Einheitenbild
      CALL UnitPicClick(x&, y&)
    END IF

    IF IsInRect&(x&, y&, minimaparea) <> 0 AND gameState& = %GAMESTATE_INGAME THEN
      'Klick auf Minimap
      CALL MinimapClick(x&-minimaparea.left, y&-minimaparea.top)
    END IF

  CASE 3:  'rechts gedrückt
    IF x& >= maparea.left AND x& <= maparea.right AND y& >= maparea.top AND y& <= maparea.bottom THEN
      dragStartX& = x&
      dragStartY& = y&
    END IF

  CASE 4:  'rechts losgelassen

  END SELECT
END SUB



'Mausrad verarbeiten
SUB MouseWheel(v#)
  CALL ZoomMap(zoom#+v#*0.25)
END SUB



'Cursor im Shop bewegen
SUB MoveShopCursor(d&)
  LOCAL phase&, unitnr&, unittp&

  phase& = GetPhase&(0, localPlayerNr&)
  IF phase& > %PHASE_UNITSELECTED THEN EXIT SUB

  IF shopCursorPos& < 16 THEN
    'Inhalt
    IF selectedShopProd$ <> "" AND (shopCursorPos& AND 3) = 0 AND d& = -1 THEN
      shopCursorPos& = shopCursorPos&+19
    ELSE
      IF shopCursorPos&+d& >= 0 AND shopCursorPos&+d& < 16 THEN shopCursorPos& = shopCursorPos&+d&
    END IF
  ELSE
    'Produktionsmenü
    IF (shopCursorPos& AND 3) = 3 AND d& = 1 THEN
      shopCursorPos& = shopCursorPos&-19
    ELSE
      IF shopCursorPos&+d& >= 16 AND shopCursorPos&+d& < 32 THEN shopCursorPos& = shopCursorPos&+d&
    END IF
  END IF

  'gewählten Eintrag aktivieren
  IF shopCursorPos& < 16 THEN
    unitnr& = channels(0).shops(selectedShop&).content(shopCursorPos&)
    CALL CheckShopRefuel(unitnr&)
    CALL SelectUnit(unitnr&, 0)
  ELSE
    unittp& = ASC(selectedShopProd$, shopCursorPos&-15)
    IF unittp& >= 0 THEN
      CALL SelectProduction(unittp&)
    ELSE
      productionPreviewUnit& = -1
    END IF
  END IF
      'prüfen, ob ausreichend Energie und Material vorhanden sind
'      costenergy& = unitclasses(unittp&).costenergy
'      costmat& = unitclasses(unittp&).costmaterial
'      IF costenergy& <= channels(0).player(localPlayerNr&).energy AND costmat& <= channels(0).shops(selectedShop&).material THEN v& = unittp&

END SUB



'Eingabe im Eingabefeld bestätigen
SUB ConfirmEditfield(c AS IDXCONTROL)
  LOCAL episode&, missionnr&, a$, r&
  LOCAL emptycampaign AS TCampaign

  IF c.ID = editMissionCode.ID THEN
    c.Visible = 0
    'Mission starten
    missionnr& = GetMissionNumber&(c.Value)
    IF missionnr& >= 0 THEN
      replayMode&(0) = %REPLAYMODE_OFF
      episode& = GetEpisodeForMap&(missionnr&)
      channels(0).campaign = emptycampaign
      channels(0).campaign.episode = episode&
      r& = LoadMission&("MIS\MISS"+FORMAT$(missionnr&, "000")+".DAT", episode&, defaultDifficulty&, 0)
      IF r& <= 0 THEN
        IF r& = 0 THEN CALL PrintError(words$$(%WORD_INVALID_MISSIONFILE))
        EXIT SUB
      END IF
      gameState& = %GAMESTATE_NONE  'notwendig, damit Cutscene nicht in der Multiplayer-Lobby abgespielt wird
      CALL InitMap(0, defaultDifficulty&)
      CALL AbortMenu
      IF gameMode& = %GAMEMODE_CLIENT THEN CALL OpenLobby(missionnr&)
    ELSE
      CALL BILog(words$$(%WORD_UNKNOWN_MAP), 0)
    END IF
  END IF

  IF c.ID = editPlayername.ID THEN
    c.Visible = 0
    'Spielernamen übernehmen
    a$ = TRIM$(c.Value)
    IF a$ <> "" THEN
      localPlayerName$ = a$
      CALL ShowMainMenu(%SUBMENU_SETTINGS, %PHASE_MAINMENU, 7)
      menuOpenTime! = gametime!-5  'Menü-Öffnen-Sequenz überspringen
    END IF
  END IF
END SUB



'ESC Taste wurde gedrückt
SUB EscapePressed
  LOCAL unitnr&, episode&, missionnr&, difficulty&, r&

  SELECT CASE gameState&
  CASE %GAMESTATE_INTRO
    'falls das Hauptmenü nicht offen ist, dies öffnen
    IF menuOpenTime! = 0 THEN
      CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_RING2, %SOUNDBUFFER_EFFECT1, %PLAYFLAGS_NONE)
      CALL ShowMainMenu(%SUBMENU_MAIN, %PHASE_NONE, 0)
    END IF

  CASE %GAMESTATE_INGAME
    'falls eine Spielnachricht angezeigt wird, diese schließen
    IF messageOpenTime! > 0 THEN
      CALL CloseGameMessage
      EXIT SUB
    END IF

    'falls die Karten-Info offen ist, diese schließen
    IF mapinfoOpenTime! > 0 THEN
      CALL CloseMapInfo
      EXIT SUB
    END IF

    'falls die Bestenliste offen ist, diese schließen
    IF highscoreOpenTime! > 0 THEN
      CALL CloseHighscore
      EXIT SUB
    END IF

    'falls ein Shop offen ist, diesen schließen
    IF selectedShop& >= 0 THEN
      CALL ExitButtonPressed(0)
      EXIT SUB
    END IF

    'falls das Kampffenster offen ist, Kampf vorzeitig beenden
    IF combatStartTime! > 0 THEN
      combatStartTime! = gametime!-4.0
      EXIT SUB
    END IF

    'falls das Protokollfenster offen ist, dieses schließen
    IF showProtocol& = 1 THEN
      CALL ProtocolButtonPressed(0)
      EXIT SUB
    END IF

    'falls ein Menü offen ist, dies schließen
    IF menuOpenTime! > 0 AND menuSelectedEntry& = -1 THEN
      CALL AbortMenu
      EXIT SUB
    END IF

    'falls eine Einheit gewählt ist, diese abwählen
    unitnr& = channels(0).player(localPlayerNr&).selectedunit
    IF GetPhase&(0, localPlayerNr&) = %PHASE_UNITSELECTED AND unitnr& >= 0 AND replayMode&(0) < %REPLAYMODE_PLAY THEN
      CALL UnselectUnit(0, localPlayerNr&)
      EXIT SUB
    END IF

  CASE %GAMESTATE_GAMEOVER
    'Missionsende-Bildschirm schließen
    IF replayMode&(0) = %REPLAYMODE_PLAY THEN
      replayPosition&(0) = LEN(replayData$(0))+1
      gameState& = %GAMESTATE_INTRO
      CALL NextReplayAction
    ELSE
      SELECT CASE channels(0).info.state
      CASE %CHANNELSTATE_VICTORY: missionnr& = channels(0).info.nextmission
      CASE %CHANNELSTATE_VICTORYBONUS: missionnr& = channels(0).info.bonusmission
      CASE ELSE: missionnr& = channels(0).info.currentmission
      END SELECT
      IF missionnr& >= 0 THEN
        episode& = GetEpisodeForMap&(channels(0).info.currentmission)
        IF episode& > 4 THEN missionnr& = missionnr&+GetEpisodeStartMap&(episode&)
        difficulty& = channels(0).info.difficulty
        IF gameMode& = %GAMEMODE_SINGLE THEN
          r& = LoadMission&("MIS\MISS"+FORMAT$(missionnr&, "000")+".DAT", channels(0).campaign.episode, difficulty&, 0)
          IF r& <= 0 THEN
            IF r& = 0 THEN CALL PrintError(words$$(%WORD_INVALID_MISSIONFILE))
            EXIT SUB
          END IF
          CALL InitMap(0, difficulty&)
        ELSE
          dialogueClosing& = 0
          IF ishost& = 0 THEN
            CALL OpenLobby(-1)
          ELSE
            CALL OpenLobby(missionnr&)
          END IF
          EXIT SUB
        END IF
      ELSE
        CALL ShowControls(0)
        creditStartTime! = gametime!
        gameState& = %GAMESTATE_CREDITS
        CALL StartMusic(6)
      END IF
    END IF

  CASE %GAMESTATE_CREDITS
    'Credits schließen
    CALL EndCredits

  CASE %GAMESTATE_CUTSCENE
    'Zwischensequenz beenden
    CALL EndCutScene

  END SELECT

  IF lobbyOpenTime! > 0 THEN
    'Multiplayer-Lobby schließen
    CALL CloseLobby(IIF&(gameState& = %GAMESTATE_INGAME, 0, 1), 0)
    EXIT SUB
  END IF
END SUB



'Tastendruck verarbeiten
SUB KeyPress(BYVAL k$)
  LOCAL ctrl&, i&, x&, y&, unitnr&, transporter&, phase&
  LOCAL c AS IDXCONTROL

  phase& = GetPhase&(0, localPlayerNr&)
  ctrl& = IIF&(GetKeyState(%VK_CONTROL) = 0, 0, 1)

  c = D2D.GetFocusedControl()
  IF ISOBJECT(c) THEN
    D2D.OnKeyPress(k$)
    IF c.ControlType = %CTYPE_EDIT AND k$ = CHR$(13) THEN ConfirmEditfield(c)
    IF c.ControlType = %CTYPE_EDIT THEN EXIT SUB  'Edit-Control erhält Key-Event exklusiv
  END IF

  'Menü-Hotkeys
  k$ = UCASE$(k$)
  IF menuOpenTime! > 0 AND menuSelectedEntry& = -1 AND LEN(k$) = 1 THEN
    FOR i& = 0 TO menuCount&-1
      IF LEFT$(menuEntries$$(i&), 4) = k$+"   " THEN
        CALL CloseMenu(menuItemAreas(i&).left+1, menuItemAreas(i&).top+1)
        EXIT SUB
      END IF
    NEXT i&
  END IF

'CALL BILog(FORMAT$(ASC(k$, 1))+","+FORMAT$(ASC(k$, 2))+","+FORMAT$(ctrl&), 0)

  SELECT CASE k$
  CASE CHR$(27):  'ESC
    CALL EscapePressed

  CASE CHR$(4):  'STRG+D
    IF debugEnabled& THEN debugInfo& = 1-debugInfo&

  CASE CHR$(5):  'STRG+E
    IF debugEnabled& THEN debugNoFog& = 1-debugNoFog&

  CASE CHR$(6):  'STRG+F
    IF gameMode& = %GAMEMODE_CLIENT AND gameState& = %GAMESTATE_INGAME THEN CALL ResyncWithServer

  CASE CHR$(8):  'STRG+H
    IF debugEnabled& THEN
      debugChecksums& = 1-debugChecksums&
      lastChecksumUpdate! = gametime!-2
    END IF

  CASE CHR$(9):  'Tab
    IF gameState& = %GAMESTATE_INGAME THEN
      IF replayMode&(0) >= %REPLAYMODE_PLAY THEN
        CALL SwitchReplayView
      ELSE
        IF GetPhase&(0, localPlayerNr&) <= %PHASE_UNITSELECTED AND LocalPlayersTurn& <> 0 AND activedialoguearea.left = 0 THEN CALL SelectNextUnit
      END IF
    END IF

  CASE CHR$(10):  'STRG+J
    IF debugEnabled& THEN debugShowChannelInfo& = 1-debugShowChannelInfo&

  CASE CHR$(11):  'STRG+K
    IF debugEnabled& AND gameState& = %GAMESTATE_INGAME THEN CALL DebugSaveChannelToFile(0)

  CASE CHR$(12):  'STRG+L
    IF debugEnabled& THEN debugShowUnitList& = 1-debugShowUnitList&

  CASE CHR$(16):  'STRG+P
    enablePing& = 1-enablePing&

  CASE CHR$(21):  'STRG+U
    IF debugEnabled& THEN debugShowUnits& = 1-debugShowUnits&

  CASE CHR$(23):  'STRG+W
    IF debugEnabled& AND gameState& = %GAMESTATE_INGAME AND channels(0).info.state = %CHANNELSTATE_INGAME THEN
      channels(0).info.turn = 9999
      channels(0).info.state = %CHANNELSTATE_VICTORY
      CALL ShowGameMessage(0, localPlayerNr&, %MSG_VICTORY)
      menuOpenTime! = 0
      shopSelectionTime! = 0
      CALL StopRecordReplay(0, 1)
    END IF

  CASE CHR$(13), CHR$(32):  'Enter / Space
    IF menuOpenTime! > 0 AND highlightedMenuEntry& >= 0 AND highlightedMenuEntry& < menuCount& THEN
      CALL CloseMenu(menuItemAreas(highlightedMenuEntry&).left+1, menuItemAreas(highlightedMenuEntry&).top+1)
    ELSE
      IF selectedShop& >= 0 THEN
        IF shopCursorPos& >= 0 AND shopCursorPos& < 16 AND channels(0).shops(selectedShop&).content(shopCursorPos&) >= 0 AND buttonShopMove.Enabled <> 0 THEN
          CALL MoveButtonPressed(0)
        END IF
        IF shopCursorPos& >= 16 AND shopCursorPos& < LEN(selectedShopProd$)+16 AND buttonShopBuild.Enabled <> 0 THEN
          CALL BuildButtonPressed(0)
        END IF
      ELSE
        IF cursorXPos& >= 0 THEN
          CALL GetPixelPos(cursorXPos&, cursorYPos&, x&, y&)
          CALL MapClick(x&, y&)
        END IF
      END IF
    END IF

  CASE CHR$(0, 69):  'Pause
    replayPause& = 1-replayPause&
    CALL UpdateProgressbar

  CASE CHR$(0, 72):  'Oben
    IF menuOpenTime! > 0 THEN
      highlightedMenuEntry& = highlightedMenuEntry&-1
      IF highlightedMenuEntry& < 0 THEN highlightedMenuEntry& = menuCount&-1
    ELSE
      IF selectedShop& >= 0 THEN
        CALL MoveShopCursor(-4)
      ELSE
        IF cursorXPos& >= 0 AND cursorYPos& > 0 THEN cursorYPos& = cursorYPos&-1
        CALL ScrollToMapPosWhenNearEdge(cursorXPos&, cursorYPos&, 0.5)
      END IF
    END IF

  CASE CHR$(0, 74):  'Minus (Ziffernblock)
    IF ctrl& = 1 THEN CALL ChangeMusicVolume(-10)

  CASE CHR$(0, 75):  'Links
    IF selectedShop& >= 0 THEN
      CALL MoveShopCursor(-1)
    ELSE
      IF cursorXPos& > 0 THEN cursorXPos& = cursorXPos&-1
      CALL ScrollToMapPosWhenNearEdge(cursorXPos&, cursorYPos&, 0.5)
    END IF

  CASE CHR$(0, 77):  'Rechts
    IF selectedShop& >= 0 THEN
      CALL MoveShopCursor(1)
    ELSE
      IF cursorXPos& >= 0 AND cursorXPos& < channels(0).info.xsize-1 THEN cursorXPos& = cursorXPos&+1
      CALL ScrollToMapPosWhenNearEdge(cursorXPos&, cursorYPos&, 0.5)
    END IF

  CASE CHR$(0, 78):  'Plus (Ziffernblock)
    IF ctrl& = 1 THEN CALL ChangeMusicVolume(10)

  CASE CHR$(0, 80):  'Unten
    IF menuOpenTime! > 0 THEN
      highlightedMenuEntry& = highlightedMenuEntry&+1
      IF highlightedMenuEntry& >= menuCount& THEN highlightedMenuEntry& = 0
    ELSE
      IF selectedShop& >= 0 THEN
        CALL MoveShopCursor(4)
      ELSE
        IF cursorXPos& >= 0 AND cursorYPos& < channels(0).info.ysize-1 THEN cursorYPos& = cursorYPos&+1
        CALL ScrollToMapPosWhenNearEdge(cursorXPos&, cursorYPos&, 0.5)
      END IF
    END IF

  CASE CHR$(0, 59):  'F1
    CALL MenuButtonPressed(0)

  CASE CHR$(0, 60):  'F2
    IF buttonSaveGame.Enabled THEN CALL SaveButtonPressed(0)

  CASE CHR$(0, 61):  'F3
    IF buttonLoadGame.Enabled THEN CALL LoadButtonPressed(0)

  CASE CHR$(0, 62):  'F4
    CALL MenuButtonPressed(-1 * %SUBMENU_SETTINGS)

  CASE CHR$(0, 63):  'F5
    IF buttonHighscore.Enabled THEN CALL ScoreButtonPressed(0)

  CASE CHR$(0, 64):  'F6
    CALL MusicButtonPressed(0)

  CASE CHR$(0, 65):  'F7
    combatMode& = IIF&(combatMode& = 0, 1, 0)

  CASE CHR$(0, 87):  'F11
    CALL EndTurnButtonPressed(0)

  CASE "1" TO "8":
    'falls gewählte Einheit ein Transporter ist, dann geladene Einheit auswählen
    transporter& = channels(0).player(localPlayerNr&).selectedunit
    IF transporter& >= 0 AND (channelsnosave(0).unitclasses(channels(0).units(transporter&).unittype).flags AND %UCF_TRANSPORTER) <> 0 THEN
      i& = ASC(k$)-49
      unitnr& = channels(0).units(transporter&).transportcontent(i&)
      lastSelectedTransporter& = transporter&
      IF unitnr& >= 0 THEN CALL SelectUnit(unitnr&, 0)
    END IF

  CASE "R":
    IF selectedShop& >= 0 AND buttonShopRepair.Enabled <> 0 THEN CALL RepairButtonPressed(0)

  CASE "T":
    IF selectedShop& >= 0 AND buttonShopTrain.Enabled <> 0 THEN CALL TrainButtonPressed(0)

  CASE "F":
    IF selectedShop& >= 0 AND buttonShopRefuel.Enabled <> 0 THEN CALL RefuelButtonPressed(0)

  CASE "I":
    CALL MapInfoButtonPressed(0)

  CASE "P":
    CALL ProtocolButtonPressed(0)

  CASE "Z":
    IF replayMode&(0) = %REPLAYMODE_PLAY THEN
      replayMode&(0) = %REPLAYMODE_FASTPLAY
      combatStartTime! = 0
      progressbar.Visible = 1
    ELSE
      IF replayMode&(0) = %REPLAYMODE_FASTPLAY THEN replayMode&(0) = %REPLAYMODE_PLAY
    END IF

  CASE "G":
    gridMode& = 1-gridMode&

  CASE "U":
    unitInfoOverlay& = 1-unitInfoOverlay&

  CASE "C":
    coordinateInfo& = 1-coordinateInfo&

  CASE "D":
    defenseInfo& = 1-defenseInfo&

  CASE "+":
    CALL ZoomMap(zoom#+0.25)

  CASE "-":
    CALL ZoomMap(zoom#-0.25)

  END SELECT
END SUB



'Karten-Info anzeigen
SUB MapInfoButtonPressed(btn&)
  IF buttonMapInfo.Enabled = 0 THEN EXIT SUB
  IF gameState& = %GAMESTATE_INGAME THEN
    IF mapinfoOpenTime! = 0 THEN
      CALL CloseAllDialogues
      CALL CreateUnitListsForPlayer(0, localPlayerNr&)
      CALL ShowMapInfo
    ELSE
      CALL CloseMapInfo
    END IF
  END IF
END SUB



'Spiel speichern
SUB SaveButtonPressed(btn&)
  'Spielstand speichern
  IF gameState& = %GAMESTATE_INGAME AND GetPhase&(0, localPlayerNr&) >= %PHASE_SURPRISEATTACK THEN EXIT SUB
  CALL SaveGame(0)
END SUB



'Spiel laden
SUB LoadButtonPressed(btn&)
  'Spielstand laden
  IF gameState& = %GAMESTATE_INGAME AND GetPhase&(0, localPlayerNr&) >= %PHASE_SURPRISEATTACK THEN EXIT SUB
  IF lobbyOpenTime! > 0 THEN CALL CloseLobby(1, 1)

  CALL ShowMainMenu(%SUBMENU_LOADGAME, GetPhase&(0, localPlayerNr&), 0)
END SUB



'Soundtrack wechseln
SUB MusicButtonPressed(btn&)
  LOCAL tracknr&

  tracknr& = currentSoundTrack&+1
  IF tracknr& > nSoundTracks& THEN tracknr& = 1
  CALL StartMusic(tracknr&)
END SUB



'Protokoll anzeigen
SUB ProtocolButtonPressed(btn&)
  IF buttonProtocol.Enabled = 0 THEN EXIT SUB
  IF gameState& = %GAMESTATE_INGAME AND GetPhase&(0, localPlayerNr&) >= %PHASE_SURPRISEATTACK THEN EXIT SUB

  IF showProtocol& = 0 THEN
    CALL CloseAllDialogues
    showProtocol& = 1
    protocolScrollbar.XPos = maparea.right-16
    protocolScrollbar.YPos = maparea.top
    protocolScrollbar.Width = 16
    protocolScrollbar.Height = maparea.bottom-maparea.top
    protocolScrollbar.HighlightColor = brushPlayer&(localPlayerNr&)
    protocolScrollbar.VisibleRows = protocolScrollbar.Height/20
    protocolScrollbar.MaxScroll = MAX&(0, protocolCount&-protocolScrollbar.VisibleRows)
    protocolScrollbar.ScrollPosition = protocolScrollbar.MaxScroll
    protocolScrollbar.Visible = 1
    protocolScrollbar.Enabled = 1
  ELSE
    showProtocol& = 0
    protocolScrollbar.Visible = 0
  END IF
END SUB



'Highscore anzeigen
SUB ScoreButtonPressed(btn&)
  IF buttonHighscore.Enabled = 0 THEN EXIT SUB
  IF gameState& = %GAMESTATE_INGAME AND GetPhase&(0, localPlayerNr&) >= %PHASE_UNITMOVING THEN EXIT SUB
  IF lobbyOpenTime! > 0 THEN CALL CloseLobby(1, 1)

  IF highscoreMapData$ = "" OR CVL(highscoreMapData$, 1) <> channels(0).info.currentmission THEN
    IF hClientSocket& < 0 THEN
      highscoreMapData$ = "*"
      CALL ConnectToServer&(StringToIP&($HIGHSCORESERVER))
    ELSE
      CALL QueryHighscore(channels(0).info.currentmission)
    END IF
  END IF

  IF highscoreOpenTime! = 0 THEN
    CALL CloseAllDialogues
    highscoreOpenTime! = gametime!
  ELSE
    CALL CloseHighscore
  END IF
END SUB



'Highscore wieder schließen
SUB CloseHighscore
  buttonClose.Visible = 0
  highscoreOpenTime! = gametime!
  dialogueClosing& = 1
END SUB



'Zug beenden
SUB EndTurnButtonPressed(btn&)
  IF buttonEndTurn.Enabled = 0 THEN EXIT SUB
  IF gameState& = %GAMESTATE_INGAME AND GetPhase&(0, localPlayerNr&) >= %PHASE_UNITMOVING THEN EXIT SUB
  IF mapinfoOpenTime! > 0 OR highscoreOpenTime! > 0 OR selectedShop& >= 0 OR combatStartTime! > 0 OR showProtocol& = 1 THEN EXIT SUB

  CALL ClearTargets(0, localPlayerNr&)
  buttonEndTurn.Enabled = 0
  buttonLoadGame.Enabled = 0
  buttonSaveGame.Enabled = 0
  buttonProtocol.Enabled = 0
  buttonHighscore.Enabled = 0

  IF gameMode& = %GAMEMODE_SINGLE THEN
    buttonMapInfo.Enabled = 0
    CALL EndTurn(0)
  ELSE
    CALL SignalEndTurn
  END IF
END SUB



'Alle Buttons freischalten, die beim Zugende ausgeschaltet wurden
SUB EnableAllMenuButtons
  buttonEndTurn.Enabled = 1
  buttonLoadGame.Enabled = 1
  buttonSaveGame.Enabled = 1
  buttonMapInfo.Enabled = 1
  buttonProtocol.Enabled = 1
  buttonHighscore.Enabled = 1
END SUB



'Alle Buttons ausschalten
SUB DisableAllMenuButtons
  buttonEndTurn.Enabled = 0
  buttonLoadGame.Enabled = 0
  buttonSaveGame.Enabled = 0
  buttonMapInfo.Enabled = 0
  buttonProtocol.Enabled = 0
  buttonHighscore.Enabled = 0
END SUB



'Hauptmenü öffnen
SUB MenuButtonPressed(btn&)
  LOCAL submenu&

  IF gameState& = %GAMESTATE_INGAME AND GetPhase&(0, localPlayerNr&) >= %PHASE_UNITMOVING THEN EXIT SUB
  IF lobbyOpenTime! > 0 THEN CALL CloseLobby(1, 1)

  'Sound-Effekt spielen
  CALL PlaySoundEffect(hFirstEffect&+%SOUNDEFFECT_RING2, %SOUNDBUFFER_EFFECT1, %PLAYFLAGS_NONE)
  CALL CloseAllDialogues
  submenu& = IIF&(btn& < 0, -1 * btn&, %SUBMENU_MAIN)
  CALL ShowMainMenu(submenu&, GetPhase&(0, localPlayerNr&), 0)
END SUB



'Dialog schließen
SUB CloseButtonPressed(btn&)
  CALL EscapePressed
END SUB



'Einheit in Produktionsgebäude bauen
SUB BuildButtonPressed(btn&)
  LOCAL costenergy&, costmat&

  IF gameMode& = %GAMEMODE_SINGLE THEN
    CALL BuildUnit&(0, selectedShop&, selectedProduction&)
  ELSE
    CALL ClientShopAction(selectedShop&, %SHOPACTION_BUILD, selectedProduction&)
  END IF

  'prüfen, ob noch weitere Einheiten produziert werden können
  costenergy& = channelsnosave(0).unitclasses(selectedProduction&).costenergy
  costmat& = channelsnosave(0).unitclasses(selectedProduction&).costmaterial
  buttonShopBuild.Enabled = IIF&(costenergy& <= channels(0).player(localPlayerNr&).energy AND costmat& <= channels(0).shops(selectedShop&).material AND GetFreeShopSlot&(0, selectedShop&) >= 0, 1, 0)
END SUB



'Einheit aus Shop herausbewegen
SUB MoveButtonPressed(bnt&)
  'Shop schließen, ohne die Einheit abzuwählen
  selectedProduction& = -1
  productionPreviewUnit& = -1
  buttonShopBuild.Visible = 0
  buttonShopMove.Visible = 0
  buttonShopRefuel.Visible = 0
  buttonShopRepair.Visible = 0
  buttonShopTrain.Visible = 0
  dialogueClosing& = 1
  shopSelectionTime! = gametime!
  buttonClose.Visible = 0
END SUB



'Einheit in Gebäude betanken
SUB RefuelButtonPressed(btn&)
  LOCAL unitnr&, targetunits&()

  unitnr& = channels(0).player(localPlayerNr&).selectedunit

  IF gameMode& = %GAMEMODE_SINGLE THEN
    CALL RefuelInShop(0, selectedShop&, unitnr&)
    'erreichbare Felder neu berechnen
    CALL ClearTargets(0, localPlayerNr&)
    IF (channels(0).units(unitnr&).flags AND %US_DONE) = 0 THEN CALL GetTargets&(0, unitnr&, 1, 0, targetunits&())
  ELSE
    CALL ClientShopAction(selectedShop&, %SHOPACTION_REFUEL, channels(0).player(localPlayerNr&).selectedunit)
  END IF
  buttonShopRefuel.Enabled = 0
END SUB



'Einheit in Gebäude reparieren
SUB RepairButtonPressed(btn&)
  IF gameMode& = %GAMEMODE_SINGLE THEN
    CALL RepairInShop(0, selectedShop&, channels(0).player(localPlayerNr&).selectedunit)
  ELSE
    CALL ClientShopAction(selectedShop&, %SHOPACTION_REPAIR, channels(0).player(localPlayerNr&).selectedunit)
  END IF
  buttonShopRepair.Enabled = 0
END SUB



'Einheit in Gebäude trainieren
SUB TrainButtonPressed(btn&)
  IF gameMode& = %GAMEMODE_SINGLE THEN
    CALL TrainInShop(0, selectedShop&, channels(0).player(localPlayerNr&).selectedunit)
    buttonShopMove.Enabled = 0
  ELSE
    CALL ClientShopAction(selectedShop&, %SHOPACTION_TRAIN, channels(0).player(localPlayerNr&).selectedunit)
  END IF
  buttonShopTrain.Enabled = 0
END SUB



'Shop wieder schließen
SUB ExitButtonPressed(btn&)
  selectedProduction& = -1
  productionPreviewUnit& = -1
  channels(0).player(localPlayerNr&).selectedunit = -1
  CALL ClearTargets(0, localPlayerNr&)
  buttonShopBuild.Visible = 0
  buttonShopMove.Visible = 0
  buttonShopRefuel.Visible = 0
  buttonShopRepair.Visible = 0
  buttonShopTrain.Visible = 0
  IF channels(0).info.state = %CHANNELSTATE_INGAME THEN
    dialogueClosing& = 1
    shopSelectionTime! = gametime!
  END IF
  buttonClose.Visible = 0
END SUB



'Chat-Nachricht an das Team schicken
SUB ChatToTeam(bnt&)
  LOCAL a$$

  a$$ = editChat.Value
  IF a$$ <> "" THEN
    SendChatMessage(a$$, 1)
    editChat.Value = ""
  END IF
END SUB



'Chat-Nachricht an alle Spieler schicken
SUB ChatToAll(bnt&)
  LOCAL a$$

  a$$ = editChat.Value
  IF a$$ <> "" THEN
    SendChatMessage(a$$, 0)
    editChat.Value = ""
  END IF
END SUB



'Verbindung zum angegebenen Server herstellen
SUB ConnectButtonPressed(btn&)
  LOCAL a$

  a$ = editServerIP.Value
  CALL ConnectToServer&(StringToIP&(a$))
END SUB



'Multiplayer Channel erstellen
SUB CreateGameButtonPressed(btn&)
  CALL CreateMultiplayerChannel
END SUB



'Channel betreten
SUB JoinGameButtonPressed(btn&)
  CALL JoinMultiplayerChannel(selectedLobbyChannel&)
END SUB



'Farbe/Team in der Lobby wechseln
SUB ChangeColorButtonPressed(btn&)
  CALL ChangePlayerColor
END SUB



'Ermittelt alle gespeicherten Spielstände
SUB ScanSavegames
  LOCAL f$, fname$, nm$, episode&, mission&, turn&, dt$, i&
  REDIM saveFiles$(1, %MAXSAVEGAMES)

  'Spielstände suchen
  nSaveFiles& = 0
  f$ = DIR$(EXEPATH$+"SAV\*.sav")
  WHILE f$ <> "" AND nSaveFiles& < %MAXSAVEGAMES
    'Dateiname
    fname$ = "SAV\"+f$

    'Episode/Mission/Runde/Zeit extrahieren
    f$ = PATHNAME$(NAME, f$)
    IF TALLY(f$, "-") <> 3 THEN
      nm$ = "???"
    ELSE
      episode& = VAL(PARSE$(f$, "-", 1))
      mission& = VAL(PARSE$(f$, "-", 2))
      turn& = VAL(PARSE$(f$, "-", 3))
      dt$ = PARSE$(f$, "-", 4)
      nm$ = "E"+FORMAT$(episode&)+" M"+FORMAT$(mission&)+" "+words$$(%WORD_TURN)+" "+FORMAT$(turn&)+" ("
      nm$ = nm$+MID$(dt$, 7, 2)+"."+MID$(dt$, 5, 2)+"."+MID$(dt$, 1, 4)+" "+MID$(dt$, 9, 2)+":"+MID$(dt$, 11, 2)
      nm$ = nm$+")"
    END IF

    'Einfügeposition suchen (neuste Spielstände oben einfügen)
    FOR i& = 0 TO nSaveFiles&-1
      IF dt$ >= MID$(saveFiles$(0, i&), LEN(saveFiles$(0, i&))-15, 12) THEN EXIT FOR
    NEXT i&
    ARRAY INSERT saveFiles$(0, i&) FOR %MAXSAVEGAMES-i&, fname$
    ARRAY INSERT saveFiles$(1, i&) FOR %MAXSAVEGAMES-i&, nm$
    nSaveFiles& = nSaveFiles&+1

    f$ = DIR$
  WEND
END SUB



'Löscht alle Spielstände bis auf den neusten jeder Episode
SUB DeleteOldSavegames
  LOCAL a$$, i&, e&, newestsavegames&

  FOR i& = 0 TO nSaveFiles&-1
    IF LEFT$(saveFiles$(1, i&), 1) = "E" THEN
      e& = VAL(MID$(saveFiles$(1, i&), 2, 1))
      IF (newestsavegames& AND 2^e&) = 0 THEN
        'diesen Spielstand beibehalten
        newestsavegames& = newestsavegames& OR 2^e&
      ELSE
        'Spielstand löschen
        a$$ = words$$(%WORD_DELETING_SAVEGAME)
        REPLACE "%" WITH saveFiles$(0, i&) IN a$$
        CALL BILog(a$$, 0)
        KILL EXEPATH$+saveFiles$(0, i&)
      END IF
    ELSE
      'Spielstand löschen
      a$$ = words$$(%WORD_DELETING_SAVEGAME)
      REPLACE "%" WITH saveFiles$(0, i&) IN a$$
      CALL BILog(a$$, 0)
      KILL EXEPATH$+saveFiles$(0, i&)
    END IF
  NEXT i&
END SUB



'Spielstand über Open-Dialog auswählen
SUB SelectSaveGame
  LOCAL ofn AS OPENFILENAME
  LOCAL szFile AS ASCIIZ*1024
  LOCAL szFilter AS STRING*1024
  LOCAL szDir AS STRING*1024
  LOCAL f$

  szFile = ""
  szDir = EXEPATH$+"SAV"+CHR$(0)
  szFilter = words$$(%WORD_FILETYPE_SAVEGAME)+CHR$(0)+"*.sav"+CHR$(0)+CHR$(0,0)

  ofn.lStructSize = SIZEOF(ofn)
  ofn.hwndOwner = hWIN&
  ofn.lpstrFile = VARPTR(szFile)
  ofn.nMaxFile = 1023
  ofn.lpstrFilter = VARPTR(szFilter)
  ofn.nFilterIndex = 1
  ofn.lpstrFileTitle = %NULL
  ofn.nMaxFileTitle = 0
  ofn.lpstrInitialDir = VARPTR(szDir)
  ofn.Flags = %OFN_PATHMUSTEXIST OR %OFN_FILEMUSTEXIST

  IF GetOpenFileName(ofn) <> 0 THEN
    f$ = szFile
    f$ = MID$(f$, LEN(EXEPATH$)+1)
    CALL LoadGame(f$)
  ELSE
    CALL ShowMainMenu(%SUBMENU_LOADGAME, GetPhase&(0, localPlayerNr&), 0)
  END IF
END SUB



'Lädt einen Spielstand
SUB LoadGame(f$)
  LOCAL a$, headerlen&, channeldatalen&, custommsglen&, humanmask&, i&, p&, msglen&, episode&

  'Spielstand einlesen
  unitsLoadedForEpisode& = 0
  a$ = ReadFileContent$(f$, 0)

  'Spielstand validieren
  IF LEFT$(a$, 4) <> "BISV" THEN
    CALL PrintError(words$$(%WORD_SAVEGAME_INVALID))
    EXIT SUB
  END IF
  headerlen& = 24
  channeldatalen& = CVL(a$, 9)
  custommsglen& = CVL(a$, 13)

  'Replay Aufnahme speichern und beenden
  CALL StopRecordReplay(0, 1)
  replayMode&(0) = %REPLAYMODE_OFF

  'Spielstand aktivieren
  IF SetFullChannelData&(0, MID$(a$, headerlen&+1, channeldatalen&)) <> 0 THEN
    'Einheiten/Terrain-Sprites laden
    episode& = channels(0).campaign.episode
    CALL SetColorSchema(episode&)
    CALL LoadTerrainSpritesAndDef&
    CALL LoadUnitSpritesAndDef&
    IF episode& = 5 THEN CALL ReadVideoMapping(episode&)

    'benutzerdefinierte Nachrichten
    p& = headerlen&+channeldatalen&+1
    FOR i& = 0 TO %MAXCUSTOMMSG-1
      msglen& = CVL(a$, p&)
      gameMessages$$(512+i&) = UTF8TOCHR$(MID$(a$, p&+4, msglen&))
      p& = p&+4+msglen&
    NEXT i&

    'Spielstand prüfen und ggf. reparieren
    CALL CheckAndFixChannelData(0)

    'falls Spielstand Einzelspieler ist, dann Spiel direkt starten, sonst Mehrspieler-Lobby öffnen
    channels(0).info.fromSavegame = 1
    CALL BILog(words$$(%WORD_SAVEGAME_LOADED), 0)
    humanmask& = channels(0).info.originalplayers AND NOT channels(0).info.aimask
    IF CountBits&(humanmask&) = 1 THEN
      CALL InitMap(0, channels(0).info.difficulty)
    ELSE
      CALL OpenLobby(-2)
    END IF
  ELSE
    CALL PrintError(words$$(%WORD_SAVEGAME_INVALID))
  END IF
END SUB



'Speichert das Spiel
SUB SaveGame(chnr&)
  LOCAL i&, a$, cmsg$, h$, f$, dt$, tm$, msgutf8$

  IF gameState& <> %GAMESTATE_INGAME AND gameMode& <> %GAMEMODE_SERVER THEN EXIT SUB

  'alle relevanten Spieldaten ermitteln
  a$ = GetFullChannelData$(chnr&)

  'benutzerdefinierte Nachrichten
  FOR i& = 0 TO %MAXCUSTOMMSG-1
    msgutf8$ = CHRTOUTF8$(gameMessages$$(512+i&))
    cmsg$ = cmsg$+MKL$(LEN(msgutf8$))+msgutf8$
  NEXT i&

  'Header erstellen
  h$ = "BISV"+MKL$(%VERSION)+MKL$(LEN(a$))+MKL$(LEN(cmsg$))+MKL$(0)+MKL$(0)

  'Dateinamen erstellen
  dt$ = DATE$
  tm$ = TIME$
  f$ = FORMAT$(channels(chnr&).campaign.episode)+"-"+FORMAT$(channels(chnr&).info.currentmission)+"-"+FORMAT$(channels(chnr&).info.turn+1)+"-"+MID$(dt$, 7, 4)+MID$(dt$, 1, 2)+MID$(dt$, 4, 2)+MID$(tm$, 1, 2)+MID$(tm$, 4, 2)+".sav"

  'Spielstand speichern
  CALL WriteFileContent("SAV\"+f$, h$+a$+cmsg$)
  CALL BILog(words$$(%WORD_SAVEGAME_CREATED), 0)
  gamedataChanged& = 0
END SUB



'Replay über Open-Dialog auswählen
SUB SelectReplay
  LOCAL ofn AS OPENFILENAME
  LOCAL szFile AS ASCIIZ*1024
  LOCAL szFilter AS STRING*1024
  LOCAL szDir AS STRING*1024
  LOCAL f$

  szFile = ""
  szDir = EXEPATH$+"RPL"+CHR$(0)
  szFilter = words$$(%WORD_FILETYPE_REPLAY)+CHR$(0)+"*.rpl"+CHR$(0)+CHR$(0,0)

  ofn.lStructSize = SIZEOF(ofn)
  ofn.hwndOwner = hWIN&
  ofn.lpstrFile = VARPTR(szFile)
  ofn.nMaxFile = 1023
  ofn.lpstrFilter = VARPTR(szFilter)
  ofn.nFilterIndex = 1
  ofn.lpstrFileTitle = %NULL
  ofn.nMaxFileTitle = 0
  ofn.lpstrInitialDir = VARPTR(szDir)
  ofn.Flags = %OFN_PATHMUSTEXIST OR %OFN_FILEMUSTEXIST

  IF GetOpenFileName(ofn) <> 0 THEN
    f$ = szFile
    f$ = MID$(f$, LEN(EXEPATH$)+1)
    CALL LoadReplay(f$)
    menuOpenTime! = 0
    CALL SetPhase(0, localPlayerNr&, %PHASE_NONE)
  ELSE
    CALL ShowMainMenu(%SUBMENU_MAIN, GetPhase&(0, localPlayerNr&), 0)
  END IF
END SUB



'Lädt eine Replay
SUB LoadReplay(f$)
  LOCAL a$, i&, p&, version&, chdatalen&, cmsglen&, replaylen&, msglen&, episode&, missionnr&, difficulty&

  'Replay einlesen
  a$ = ReadFileContent$(f$, 0)

  'Replay validieren
  IF LEFT$(a$, 8) <> "BI2020RP" THEN
    CALL PrintError(words$$(%WORD_REPLAY_INVALID))
    EXIT SUB
  END IF
  version& = CVL(a$, 9)
  IF version& < 1110 THEN
    CALL PrintError(words$$(%WORD_REPLAY_VERSION_ERROR))
    EXIT SUB
  END IF
  chdatalen& = CVL(a$, 21)
  cmsglen& = CVL(a$, 25)
  replaylen& = CVL(a$, 29)
  episode& = ASC(a$, 33)
  missionnr& = ASC(a$, 34)
  difficulty& = ASC(a$, 35)
  mapRandomSeed& = CVL(a$, 37)
  replayUsername$ = RTRIM$(MID$(a$, 41, 16))

  'Mission laden
  IF SetFullChannelData&(0, MID$(a$, 65, chdatalen&)) = 0 THEN
    CALL PrintError(words$$(%WORD_REPLAY_VERSION_ERROR))
    EXIT SUB
  END IF

  'benutzerdefinierte Nachrichten
  IF cmsglen& > 0 THEN
    p& = chdatalen&+65
    FOR i& = 0 TO %MAXCUSTOMMSG-1
      msglen& = CVL(a$, p&)
      gameMessages$$(512+i&) = UTF8TOCHR$(MID$(a$, p&+4, msglen&))
      p& = p&+4+msglen&
    NEXT i&
  END IF

  unitsLoadedForEpisode& = 0
  CALL SetColorSchema(episode&)
  CALL LoadTerrainSpritesAndDef&
  CALL LoadUnitSpritesAndDef&
  IF episode& = 5 THEN CALL ReadVideoMapping(episode&)

  'Replay starten
  replayData$(0) = MID$(a$, 65+chdatalen&+cmsglen&, replaylen&)
  IF debugEnabled& = 1 THEN
'    CALL ExportReplay(f$)
'    CALL SetFullChannelData&(0, MID$(a$, 65, chdatalen&))
  END IF
  exportingReplay& = 0
  replayUnitUpdate$ = ""
  replayPosition&(0) = 1
  CALL UpdateProgressbar
  replayMode&(0) = %REPLAYMODE_PLAY

  'Mission initialisieren
  IF channels(0).info.turn = 0 AND channels(0).info.movement = 0 THEN channels(0).info.actionposition = channels(0).info.nvictoryconditions
  CALL InitMap(0, difficulty&)
  localPlayerNr& = ASC(a$, 36)
  localPlayerMask& = 2^localPlayerNr&
END SUB



'Speichert eine Replay
SUB SaveReplay(chnr&)
  LOCAL h$, f$, dt$, tm$
  LOCAL p&, replaylen&, chdatalen&, cmsglen&

  'Replay nur speichern, wenn mindestens 1 Zug gespielt wurde
  replayData$(chnr&) = LEFT$(replayData$(chnr&), replayPosition&(chnr&)-1)
  replaylen& = LEN(replayData$(chnr&))
  chdatalen& = CVL(replayData$(chnr&), 21)
  cmsglen& = CVL(replayData$(chnr&), 25)
  p& = 65+chdatalen&+cmsglen&
  WHILE p& < replaylen&
    IF ASC(replayData$(chnr&), p&+5) = %REPLAY_ENDTURN THEN EXIT LOOP
    p& = p&+80
  WEND
  IF p& > replaylen& THEN EXIT SUB

  'Länge der Replay-Datensätze einfügen
  MID$(replayData$(chnr&), 29, 4) = MKL$(replaylen&-64-chdatalen&-cmsglen&)

  'Dateinamen erstellen
  dt$ = DATE$
  tm$ = TIME$
  f$ = mapnames$(channels(chnr&).info.currentmission)+"_"+MID$(dt$, 7, 4)+"-"+MID$(dt$, 1, 2)+"-"+MID$(dt$, 4, 2)+"_"+MID$(tm$, 1, 2)+MID$(tm$, 4, 2)+".rpl"
  IF gameMode& = %GAMEMODE_SERVER THEN f$ = "ch"+FORMAT$(chnr&)+"_"+f$

  'Replay speichern
  CALL WriteFileContent("RPL\"+f$, replayData$(chnr&))
  CALL BILog(words$$(%WORD_REPLAY_CREATED), 0)
END SUB



'Exportiert eine Replay als Textdatei
SUB ExportReplay(f$)
  LOCAL a$, b$, i&, n&, p&, plnr&, ac&
  LOCAL unitnr&, x&, y&, attacker&, defender&, unitac&, shopnr&, shopac&, shopparam&, newplnr&, turnnr&
  LOCAL unitactions$(), shopactions$()
  DIM unitactions$(9), shopactions$(4)

  'Einheitenaktionen
  unitactions$(0) = "???"
  unitactions$(%UNITACTION_REFUEL) = "refuels"
  unitactions$(%UNITACTION_REPAIR) = "repairs"
  unitactions$(%UNITACTION_BUILDROAD) = "builds road"
  unitactions$(%UNITACTION_BUILDRAIL) = "builds rails"
  unitactions$(%UNITACTION_BUILDTRENCH) = "builds trench"
  unitactions$(%UNITACTION_DESTRUCT) = "destructs"
  unitactions$(%UNITACTION_ASCEND) = "ascends"
  unitactions$(%UNITACTION_DESCEND) = "dives"
  unitactions$(%UNITACTION_MOVE) = "moves

  'Shopaktionen
  shopactions$(0) = "???"
  shopactions$(%SHOPACTION_REFUEL) = "refuels"
  shopactions$(%SHOPACTION_REPAIR) = "repairs"
  shopactions$(%SHOPACTION_BUILD) = "produces"
  shopactions$(%SHOPACTION_TRAIN) = "trains"

  'Einträge in Text umwandeln
  exportingReplay& = 1
  n& = LEN(replayData$(0))/80
  p& = 1
  replayPosition&(0) = 1
  replayMode&(0) = %REPLAYMODE_FASTPLAY
  FOR i& = 1 TO n&-1
    'Aktion exportieren
    plnr& = ASC(replayData$(0), p&+4)
    ac& = ASC(replayData$(0), p&+5)
    b$ = FORMAT$(i&, "0000")+"   PL"+FORMAT$(plnr&+1)+" "
    SELECT CASE ac&
    CASE %REPLAY_MOVE:
      unitnr& = CVL(replayData$(0), p&+8)
      x& = CVI(replayData$(0), p&+12)
      y& = CVI(replayData$(0), p&+14)
      b$ = b$+"Move "+UnitIDString$(0, unitnr&)+" -> "+FORMAT$(x&)+","+FORMAT$(y&)+CHR$(13,10)

    CASE %REPLAY_ATTACK:
      POKE$ VARPTR(channels(0).combat), MID$(replayData$(0), p&+8, SIZEOF(TCombatInfo))
      attacker& = channels(0).combat.attacker
      defender& = channels(0).combat.defender
      b$ = b$+"Attack "+UnitIDString$(0, attacker&)+" (dealing "+FORMAT$(channels(0).combat.params(5, 1))+" damage) vs "+UnitIDString$(0, defender&)+" (dealing "+FORMAT$(channels(0).combat.params(5, 0))+" damage)"+CHR$(13,10)

    CASE %REPLAY_UNITACTION:
      unitnr& = CVL(replayData$(0), p&+8)
      unitac& = CVI(replayData$(0), p&+12)
      IF unitac& < 1 OR unitac& > 9 THEN unitac& = 0
      x& = CVI(replayData$(0), p&+14)
      y& = CVI(replayData$(0), p&+16)
      b$ = b$+UnitIDString$(0, unitnr&)+" "+unitactions$(unitac&)+" at "+FORMAT$(x&)+","+FORMAT$(y&)+CHR$(13,10)

    CASE %REPLAY_SHOPACTION:
      shopnr& = CVL(replayData$(0), p&+8)
      shopac& = CVL(replayData$(0), p&+12)
      IF shopac& < 1 OR shopac& > 4 THEN shopac& = 0
      shopparam& = CVL(replayData$(0), p&+16)
      b$ = b$+"Shop "+FORMAT$(shopnr&)+" ("+channels(0).info.shopnames(shopnr&)+") "+shopactions$(shopac&)+" "
      IF shopac& = %SHOPACTION_BUILD THEN b$ = b$+channelsnosave(0).unitclasses(shopparam&).uname+CHR$(13,10) ELSE b$ = b$+UnitIDString$(0, shopparam&)+CHR$(13,10)

    CASE %REPLAY_ENDTURN:
      newplnr& = ASC(replayData$(0), p&+8)
      b$ = b$+"End turn (new player is player "+FORMAT$(newplnr&+1)+")"+CHR$(13,10)+CHR$(13,10)

    END SELECT
    a$ = a$+b$
    p& = p&+80

    'Aktion abspielen, damit Referenzen zu neu produzierten Einheiten passen
    turnnr& = channels(0).info.turn
    CALL NextReplayAction

    'prüfen, ob neue Runde gestartet wurde
    IF turnnr& <> channels(0).info.turn THEN
      a$ = a$+"-------------"+CHR$(13,10) _
         + "   Turn "+FORMAT$(channels(0).info.turn)+CHR$(13,10) _
         + "-------------"+CHR$(13,10)+CHR$(13,10)
    END IF
  NEXT i&

  'Textdatei speichern
  CALL WriteFileContent(LEFT$(f$, LEN(f$)-3)+"txt", a$)
END SUB



'Replay Aufnahme starten
SUB StartRecordReplay(chnr&)
  LOCAL i&, a$, cmsg$, msgutf8$

  'Abbild des Channels erstellen
  a$ = GetFullChannelData$(chnr&)

  'benutzerdefinierte Nachrichten
  FOR i& = 0 TO %MAXCUSTOMMSG-1
    msgutf8$ = CHRTOUTF8$(gameMessages$$(512+i&))
    cmsg$ = cmsg$+MKL$(LEN(msgutf8$))+msgutf8$
  NEXT i&

  'Replay-Header + Channel-Daten + benutzerdefinierte Nachrichten erstellen
  replayData$(chnr&) = "BI2020RP"+MKL$(%VERSION)+MKL$(today&)+MKL$(curtime&)+MKL$(LEN(a$))+MKL$(LEN(cmsg$))+MKL$(0) _
     + CHR$(channels(chnr&).campaign.episode)+CHR$(channels(chnr&).info.currentmission)+CHR$(channels(chnr&).info.difficulty)+CHR$(localPlayerNr&)+MKL$(mapRandomSeed&) _
     + localPlayerName$+SPACE$(16-LEN(localPlayerName$))+STRING$(8, 0)+a$+cmsg$
  replayPosition&(chnr&) = LEN(replayData$(chnr&))+1
  replayMode&(chnr&) = %REPLAYMODE_RECORD
END SUB



'Replay Aufnahme beenden
SUB StopRecordReplay(chnr&, savenow&)
  IF replayMode&(chnr&) <> %REPLAYMODE_RECORD THEN EXIT SUB
  IF savenow& <> 0 THEN CALL SaveReplay(chnr&)
  replayMode&(chnr&) = %REPLAYMODE_OFF
END SUB



'Ansicht in Replay auf nächsten Spieler wechseln
SUB SwitchReplayView
  DO
    localPlayerNr& = localPlayerNr&+1
    IF localPlayerNr& > 5 THEN localPlayerNr& = 0
  LOOP UNTIL (channels(0).info.originalplayers AND 2^localPlayerNr&) <> 0
  localPlayerMask& = 2^localPlayerNr&
END SUB



'Uhrzeit und Datum aktualisieren
SUB UpdateDateTime
  LOCAL d$

  'Zeit aktualisieren
  gametime! = TIMER

  'Datum aktualisieren
  IF INT(gametime!) <> curtime& THEN
    curtime& = INT(gametime!)
    d$ = DATE$
    today& = VAL(MID$(d$, 7, 4))*65536+VAL(MID$(d$, 1, 2))*256+VAL(MID$(d$, 4, 2))
  END IF
END SUB



'Fenstergröße ändern
SUB ResizeWin
  LOCAL newWidth&, newHeight&, xdir&, ydir&, framewidth&, frameheight&, bleft&, btop&, bwd&, bhg&
  LOCAL clientarea AS RECT, windowarea AS RECT

  IF NOT ISOBJECT(buttonMapInfo) THEN EXIT SUB

  'neue Fenstergröße auslesen
  GetClientRect hWIN&, clientarea
  GetWindowRect hWIN&, windowarea
  newWidth& = clientarea.right-clientarea.left
  newHeight& = clientarea.bottom-clientarea.top
  framewidth& = windowarea.right-windowarea.left-newWidth&
  frameheight& = windowarea.bottom-windowarea.top-newHeight&
  xdir& = SGN(windowWidth&-newWidth&)
  ydir& = SGN(windowHeight&-newHeight&)
  IF xdir& = 0 AND ydir& = 0 THEN EXIT SUB
  windowWidth& = newWidth&
  windowHeight& = newHeight&

  IF newWidth& <> windowWidth& OR newHeight& <> windowHeight& THEN
    SetWindowPos hWin&, %HWND_TOP, 0, 0, windowWidth&+framewidth&, windowHeight&+frameheight&, %SWP_NOMOVE
  END IF

  uiscale! = windowHeight&/1080
  CALL InitAreas

  'Controls an neue Größe anpassen
  bleft& = buttonarea.left
  btop& = INT(buttonarea.top+4*uiscale!)
  bwd& = 40*uiscale!
  bhg& = 46*uiscale!
  buttonMapInfo.MoveControl(INT(bleft&+0*uiscale!), btop&, bwd&, bhg&)
  buttonLoadGame.MoveControl(INT(bleft&+40*uiscale!), btop&, bwd&, bhg&)
  buttonSaveGame.MoveControl(INT(bleft&+80*uiscale!), btop&, bwd&, bhg&)
  buttonMusic.MoveControl(INT(bleft&+120*uiscale!), btop&, bwd&, bhg&)
  buttonProtocol.MoveControl(INT(bleft&+160*uiscale!), btop&, bwd&, bhg&)
  buttonHighscore.MoveControl(INT(bleft&+200*uiscale!), btop&, bwd&, bhg&)
  buttonEndTurn.MoveControl(INT(bleft&+240*uiscale!), btop&, bwd&, bhg&)
  buttonOpenMenu.MoveControl(INT(bleft&+280*uiscale!), btop&, bwd&, bhg&)
  buttonShopBuild.MoveControl(0, 0, bwd&, bhg&)
  buttonShopMove.MoveControl(0, 0, bwd&, bhg&)
  buttonShopRefuel.MoveControl(0, 0, bwd&, bhg&)
  buttonShopRepair.MoveControl(0, 0, bwd&, bhg&)
  buttonShopTrain.MoveControl(0, 0, bwd&, bhg&)
  buttonChatTeam.MoveControl(0, 0, bwd&, bhg&)
  buttonChatAll.MoveControl(0, 0, bwd&, bhg&)
  buttonClose.MoveControl(0, 0, 71*uiscale!, 67*uiscale!)
  CALL InitHUD
  'TODO
'  progressbar.MoveControl(40*uiscale!, 1020*uiscale!, 1480*uiscale!, 40*uiscale!)
END SUB



'Globale Fehlerbehandlung
FUNCTION GlobalErrorHandler&(BYVAL pException_Pointers AS Exception_Pointers PTR)
  LOCAL ThisExceptionPointer AS Exception_Pointers
  LOCAL ThisExceptionRecordPointer AS Exception_Record PTR
  LOCAL ThisExceptionRecord AS Exception_Record
  LOCAL i&, n&, nr&, chnr&, a$, dt$, f$

  'Ausnahme-Daten lesen
  ThisExceptionPointer = @pException_Pointers
  ThisExceptionRecordPointer = ThisExceptionPointer.ExceptionRecord
  ThisExceptionRecord = @ThisExceptionRecordPointer

  'Application Log-Datei schreiben
  CALL APPLOG($APPNAME, logFilename$, "Server terminated with exception: "+HEX$(ThisExceptionRecord.ExceptionCode))

  'Log-Eintrag erzeugen
  dt$ = DATE$
  a$ = "Exception code: "+HEX$(ThisExceptionRecord.ExceptionCode)+CHR$(13,10) _
     + "Error address : "+FORMAT$(ThisExceptionRecord.ExceptionAddress)+CHR$(13,10) _
     + "Date/Time     : "+MID$(dt$, 4, 2)+"."+MID$(dt$, 1, 2)+"."+MID$(dt$, 7, 4)+" "+TIME$+CHR$(13,10)

'ACHTUNG: Aktivierung des CALLSTACKs führt sofort zum Programmabsturz!
'  n& = CALLSTKCOUNT
'  FOR i& = 2 TO n&
'    a$ = a$+CALLSTK$(i&)+CHR$(13,10)
'  NEXT i&

  'Log-Datei schreiben
  f$ = LEFT$(logFilename$, INSTR(-1, logFilename$, "\"))+"bi2020_crashlog.log"
  nr& = FREEFILE
  OPEN f$ FOR OUTPUT AS nr&
  PRINT# nr&, a$;
  CLOSE nr&

  'Server kontrolliert beenden
  exitprg& = 1
  PostQuitMessage 0

  GlobalErrorHandler& = %EXCEPTION_CONTINUE_SEARCH
END FUNCTION



'Window Ereignis Verarbeitung
FUNCTION WindowProc (BYVAL hwnd AS DWORD, BYVAL wMsg AS DWORD, BYVAL wParam AS DWORD, BYVAL lParam AS LONG) AS LONG
  LOCAL X&, Y&, EXTKEY&, K&, servernr&, cnr&

  SELECT CASE wMsg
  CASE %WM_LBUTTONDOWN
    CALL MouseClick(lParam AND 65535, INT(lParam/65536), 1)
    EXIT FUNCTION

  CASE %WM_LBUTTONUP
    CALL MouseClick(lParam AND 65535, INT(lParam/65536), 2)
    EXIT FUNCTION

  CASE %WM_RBUTTONDOWN
    CALL MouseClick(lParam AND 65535, INT(lParam/65536), 3)
    EXIT FUNCTION

  CASE %WM_RBUTTONUP
    CALL MouseClick(lParam AND 65535, INT(lParam/65536), 4)
    EXIT FUNCTION

  CASE %WM_MOUSEMOVE
    X& = lParam AND 65535
    Y& = INT(lParam/65536)
    CALL MouseMove(X&, Y&)
    EXIT FUNCTION

  CASE %WM_MOUSEWHEEL
    CALL MouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)/%WHEEL_DELTA)
    EXIT FUNCTION

  CASE %WM_KEYDOWN
    EXTKEY& = INT(lParam/16777216) AND 1
    K& = INT(lParam/65536) AND 255
    IF EXTKEY& = 1 THEN
      IF K& >= 72 THEN CALL KeyPress(CHR$(0, K&))
    ELSE
      IF (K& >= 59 AND K& <= 65) OR K& = 74 OR K& = 78 OR K& = 87 THEN
        CALL KeyPress(CHR$(0, K&))
      END IF
    END IF
'    CALL KEYEVENT(k&, 0)
    EXIT FUNCTION

  CASE %WM_KEYUP
    K& = INT(lParam/65536) AND 255
'    CALL KEYEVENT(k&, 1)
    EXIT FUNCTION

  CASE %WM_CHAR
    CALL KeyPress(CHR$(wParam))

  CASE %WM_TIMER  'Client: alle 20ms (50fps) / Server: alle 100ms
    CALL UpdateDateTime
    IF gameMode& <> %GAMEMODE_SERVER THEN
      CALL AutoScroll
      IF semaphore_crttexture& = 0 THEN
        CALL EnterSemaphore(semaphore_crttexture&)
        D2D.OnRender(mousexpos&, mouseypos&)
        CALL LeaveSemaphore(semaphore_crttexture&)
      END IF
      DS.PlayAllChannels((gametime!-lasttimeupdate!)*1000)
      CALL SetMusicVolume
      CALL LoopMusic
      lasttimeupdate! = gametime!
    ELSE
      CALL APPHEARTBEAT
      CALL ProcessServerApplicationQueries
      CALL ServerTimerEvent
      IF gametime! >= serverConsoleUpdateTime!+1 THEN
        serverConsoleUpdateTime! = gametime!
        CALL UpdateServerConsole
      END IF
      IF gametime! >= serverTrafficSaveTime!+60 THEN
        serverTrafficSaveTime! = gametime!
        CALL SaveTrafficLog
      END IF
    END IF
    IF gameMode& = %GAMEMODE_SINGLE AND gameState& = %GAMESTATE_INGAME AND gametime!-lastCampaignTimeUpdate! >= 1 THEN
      channels(0).campaign.time = channels(0).campaign.time+1
      lastCampaignTimeUpdate! = lastCampaignTimeUpdate!+1
    END IF
    IF debugChecksums& = 1 AND gameMode& = %GAMEMODE_CLIENT AND gameState& = %GAMESTATE_INGAME AND gametime!-lastChecksumUpdate! >= 2 THEN
      lastChecksumUpdate! = gametime!
      CALL QueryDebugChecksums
    END IF
    IF enablePing& = 1 AND gameMode& = %GAMEMODE_CLIENT AND connectedToAuthenticServer& = 1 THEN
      IF (pingMillisecs& >= 0 AND gametime!-pingSentTime! >= 2) OR gametime!-pingSentTime! >= 30 THEN CALL SendPing
    END IF
    IF gameMode& = %GAMEMODE_SINGLE AND gameState& = %GAMESTATE_INGAME AND replayMode&(0) >= %REPLAYMODE_PLAY AND exportingReplay& = 0 THEN CALL NextReplayAction
    IF gameMode& <> %GAMEMODE_SERVER AND gameState& = %GAMESTATE_CUTSCENE AND isVideoCutscene& = 0 THEN CALL UpdateCutScene
    EXIT FUNCTION

  CASE %WM_EXITSIZEMOVE
    CALL ResizeWin
    IF ISOBJECT(D2D) THEN D2D.OnResize(windowWidth&, windowHeight&)

  CASE %WM_GETMINMAXINFO
    POKE LONG, lParam+24, 800  'ptMinTrackSize.x
    POKE LONG, lParam+28, 450  'ptMinTrackSize.y

  CASE %TCP_ACCEPT TO %TCP_ACCEPT+%MAXLOCALIPS-1  'Server nimmt Verbindung entgegen
    servernr& = wMsg-%TCP_ACCEPT
    IF LOWRD(lParam) = %FD_ACCEPT THEN CALL AcceptConnection(servernr&)
    FUNCTION = 1
    EXIT FUNCTION

  CASE %TCP_READ TO %TCP_READ+%MAXCONNECTIONS-1  'im Server eingehende Socket-Daten
    cnr& = wMsg-%TCP_READ
    SELECT CASE LOWRD(lParam)
    CASE %FD_READ:
      CALL ReadClientData(cnr&)
    CASE %FD_CLOSE:
      CloseConnectionToClient(cnr&)
    END SELECT
    FUNCTION = 1
    EXIT FUNCTION

  CASE %TCP_CLIENTREAD  'im Client eingehende Socket-Daten
    SELECT CASE LOWRD(lParam)
    CASE %FD_READ:
      CALL ProcessServerData
    CASE %FD_CLOSE:
      CALL ConnectionLost
    END SELECT
    FUNCTION = 1
    EXIT FUNCTION

  CASE %WM_CLOSE
'    IF SAVEQUERY& = 2 THEN EXIT FUNCTION

  CASE %WM_DESTROY
    CALL StopRecordReplay(0, 1)
    D2D = NOTHING
    'close the application by sending a WM_QUIT message
    exitprg& = 1
    PostQuitMessage 0
    EXIT FUNCTION

  CASE %WM_SYSCOMMAND
    X& = wParam AND &HFFF0
    IF X& = %SC_CLOSE THEN
      SendMessage hwnd, %WM_CLOSE, 0, 0
      EXIT FUNCTION
    END IF
  END SELECT

  'pass unprocessed messages to Windows
  FUNCTION = DefWindowProc(hWnd, wMsg, wParam, lParam)
END FUNCTION



'Hauptprogramm
FUNCTION PBMAIN&
  LOCAL c$, i&, n&, missionnr&
  DIM channels(%MAXCHANNELS-1), channelsnosave(%MAXCHANNELS-1)
  DIM messageBuffer$$(%MESSAGEBUFFERSIZE-1)
  DIM protocolBuffer$$(%PROTOCOLBUFFERSIZE-1)
  DIM soundchannels(%MAXSOUNDCHANNELS-1)
  DIM soundtracks&(%MAXSOUNDTRACKS-1)
  DIM voices$(%MAXVOICES-1, 1)
  DIM playernames$(%MAXPLAYERS-1), defaultPlayernames$(%MAXPLAYERS-1)
  DIM mapscore$(999)
  DIM replayMode&(%MAXCHANNELS-1), replayPosition&(%MAXCHANNELS-1), replayData$(%MAXCHANNELS-1)

  RANDOMIZE TIMER
  EXEPATH$ = EXE.PATH$
  configfilename$ = $CONFIGFILE
  langcode$ = "GER"
  gameState& = %GAMESTATE_NONE

  'Kommandozeilenparameter auswerten
  c$ = TRIM$(COMMAND$)
  CALL ProcessCommandArgs(c$)

  'Konfigurationsdatei einlesen
  CALL ReadConfig&
  CALL CleanUpOldVersionFiles

  'Sprachdatei einlesen
  IF ReadLangFile&(langcode$+"\BI2020.TXT") = 0 THEN
    EXIT FUNCTION
  END IF

  'Standardwerte setzen
  CALL InitDefaults

  IF gameMode& <> %GAMEMODE_SERVER THEN
'    hExceptionHandler& = AddVectoredExceptionHandler(1 , CODEPTR(GlobalErrorHandler&))
    CALL BILog("Battle Isle 2020 v"+FORMAT$(%VERSION/1000, "0.00"), 0)

    'Direct2D initialisieren
    D2D = CLASS "CDIRECT2D"
    IF ISNOTHING(D2D) THEN EXIT FUNCTION

    'Grafikfenster erzeugen
    CALL CREATEWIN
    CALL InitAreas
    IF ISFALSE D2D.InitD2D(hWIN&, 2000, CODEPTR(RenderScene)) THEN CALL CriticalError(%ERRMSG_DIRECTX_INIT)
    IF InitFonts& = 0 THEN CALL CriticalError(%ERRMSG_DIRECTX_FONTS)
    IF InitBrushes& = 0 THEN CALL CriticalError(%ERRMSG_DIRECTX_BRUSHES)
    ShowWindow hWIN&, IIF&(fullscreenMode& = 1, %SW_MAXIMIZE, %SW_SHOW)
    UpdateWindow hWIN&
    SetTimer(hWIN&, 1, 20, %NULL)

    'DirectSound initialisieren
    DS = CLASS "CDIRECTSOUND"
    IF ISNOTHING(DS) THEN CALL CriticalError(%ERRMSG_DIRECTX_SOUND)
    IF ISFALSE(DS.InitDirectSound(hWIN&, %MAXSOUNDCHANNELS)) THEN CALL CriticalError(%ERRMSG_DIRECTX_SOUND)
    pAfxMp3 = CLASS "CAfxMp3"
    IF ISNOTHING(pAfxMp3) THEN CALL PrintError(words$$(%ERRMSG_MP3_INIT))

    'Speicherplatz für Reifenspuren, Raketen und Zwischensequenzen allokieren
    DIM trails?(255,255), missiles(%MAXMISSILES-1), cutSceneObjects(%MAXCUTSCENEOBJECTS-1)
  ELSE
    REDIM messageBuffer$$(23)
    CON.NEW
    CONSOLE NAME $WINDOWTITLESERVER
    CON.VIEW = 43, 80
    CON.VIRTUAL = 43, 80
    CON.SCREEN = 43, 80
    CON.CURSOR.OFF
    CALL CREATEWIN
    SetTimer(hWIN&, 1, 100, %NULL)
    CALL APPLOG($APPNAME, logFilename$, "Server version "+FORMAT$(%VERSION/1000, "0.00")+" started")
    CALL REGISTERSERVERAPP($APPNAME, "v"+FORMAT$(%VERSION/1000, "0.00"))
  END IF

  'Einheitendefinitionen einlesen
  IF checkInstallation& = 0 AND ReadUnitDefs&(0) = 0 THEN checkInstallation& = 1

  'Terraindefinitionen einlesen
  IF checkInstallation& = 0 AND ReadTerrainDefs&(0) = 0 THEN checkInstallation& = 1

  'Blue Byte Dateien prüfen und ggf. installieren
  IF checkInstallation& = 1 THEN
    IF CheckGameFiles& <> 0 THEN
      checkInstallation& = 0
      IF channelsnosave(0).nunitclasses = 0 AND ReadUnitDefs&(0) = 0 THEN checkInstallation& = 1
      IF channelsnosave(0).nterrain = 0 AND ReadTerrainDefs&(0) = 0 THEN checkInstallation& = 1
    END IF
  END IF

  'Darstellungs-Parameter
  hSkin& = -1
  hDialog& = -1
  hCutSceneElements& = -1
  terrainAnimationSpeed! = 250
  overlayAnimationSpeed! = 250
  shopAnimationSpeed! = 150
  speechRate& = 1
  zoom# = 2.0
  dragStartX& = -1
  lastPreviewUnit& = -1
  lastPreviewShop& = -1
  selectedShop& = -1
  hClientSocket& = -1
  playerColors$ = $playercolorsbi2
  CALL UpdateDateTime
  lasttimeupdate! = gametime!

  'lokalen Spieler festlegen
  localPlayerNr& = 0

  IF checkInstallation& = 0 THEN
    'Nachrichtentexte laden
    IF LoadMessages&(0) = 0 THEN checkInstallation& = 1

    'Missionscodes laden
    IF LoadMissionnames& = 0 THEN checkInstallation& = 1

    IF gameMode& <> %GAMEMODE_SERVER THEN
      'Bitmaps laden
      IF LoadBitmaps& = 0 THEN checkInstallation& = 1

      'Geräusche laden
      IF LoadSounds& = 0 THEN checkInstallation& = 1

      'Musik-Dateien ermitteln
      nSoundTracks& = LoadMusic&

      'Buttons etc erzeugen
      CALL InitControls

      'Artworks aller Einheiten
      CALL LoadArtworks&

      'Sprachausgabe initialisieren
      CALL SAPIOPTIONS(1)  'asynchrone Ausgabe aktivieren
      IF speechVolume& > 0 THEN
        i& = SAPIINIT&(0)
        IF i& <> 1 THEN
          CALL BIDebugLog("SAPI error: "+SAPIGETERRORMSG$(i&))
          speechVolume& = 0
        END IF
      END IF

      'Intro initialisieren
      CALL InitIntro

      'Artworks und Animationen  asynchron laden
      gameState& = %GAMESTATE_INIT
      THREAD CREATE GameInitThread&(0) TO hInitThread&

      'asynchron nach Client-Update suchen
      IF startupaction& = %STARTACTION_NONE THEN
        THREAD CREATE ClientUpdateThread&(0) TO hClientUpdateThread&
      END IF

      'Thread für AI erzeugen
      THREAD CREATE AIThread&(0) TO channels(0).info.hAIThread
    END IF
  ELSE
    'falls Programm nicht korrekt installiert wurde, wenigstens versuchen, die Bitmaps zu laden, damit der HUD angezeigt wird
    i& = LoadBitmaps&
  END IF
  CALL UpdateDateTime

  'Server starten
  IF gameMode& = %GAMEMODE_SERVER THEN
    CALL CalculateChecksumForAllMissions
    CALL LoadHighscore
    CALL InitServer(hWIN&)
    serverConsoleUpdateTime! = gametime!
  END IF

  'Startaktion ausführen
  IF gameMode& <> %GAMEMODE_SERVER THEN
    IF checkInstallation& = 1 THEN
      gameState& = %GAMESTATE_ERROR
      CALL ShowMainMenu(%SUBMENU_ERROR, %PHASE_NONE, 0)
    ELSE
      SELECT CASE startupaction&
      CASE -1
        'DEBUG
    '      i& = LoadMission&("MIS\MISS152.DAT", 0)
    '      i& = LoadMission&("Test\edtunits.dat", 0)
        LoadMission&("Test\MISS903", 0, defaultDifficulty&, 0)
        CALL InitMap(0, defaultDifficulty&)
        lasttimeupdate! = gametime!
      CASE %STARTACTION_NONE
        'Intro
        introStartTime! = gametime!
        lasttimeupdate! = gametime!
        gameState& = %GAMESTATE_INTRO
      CASE %STARTACTION_HOSTGAME
        'Multiplayer Spiel erstellen
        CALL StartEpisode(4)
      CASE %STARTACTION_JOINGAME
        'Multiplayer Spiel beitreten
        CALL OpenLobby(-1)
      CASE %STARTACTION_STARTMAP
        'Karte wählen und starten
        missionnr& = GetMissionNumber&(startupMap$)
        IF missionnr& >= 0 THEN
          editMissionCode.Value = startupMap$
          CALL ConfirmEditfield(editMissionCode)
        ELSE
          CALL BILog(words$$(%WORD_UNKNOWN_MAP), 0)
        END IF
      CASE %STARTACTION_REPLAY
        'Replay abspielen
        CALL LoadReplay(startupReplay$)
      CASE %STARTACTION_TESTMAP
        'Karte aus Editor heraus testen
        LoadMission&(startupMap$, 0, defaultDifficulty&, 0)
        CALL InitMap(0, defaultDifficulty&)
        CALL ShowControls(1)
        lasttimeupdate! = gametime!
      END SELECT
    END IF
  END IF

  'Window Nachrichten verarbeiten
  LOCAL uMsg AS tagMsg
  WHILE GetMessage(uMsg, %NULL, 0, 0)
    TranslateMessage uMsg
    DispatchMessage uMsg
    IF gameMode& = %GAMEMODE_SINGLE THEN CALL ProcessGameEvents(0)
    IF gameMode& = %GAMEMODE_SERVER THEN CALL ProcessServerKeys
  WEND

  KillTimer(hWIN&, 1)
  IF gameMode& <> %GAMEMODE_SERVER THEN
    CALL SaveConfig
  ELSE
    CALL UNREGISTERSERVERAPP
    CALL APPLOG($APPNAME, logFilename$, "Finished")
  END IF
  CALL BIDebugLog("BI2020 exited normally.")
END FUNCTION
