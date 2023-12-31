'Battle Isle 2 - Datentypen


'Terrain
TYPE TTerrain
  u1 AS INTEGER
  u2 AS BYTE
  u3 AS BYTE  '? Animation
  u4 AS INTEGER  '? (nur bei Sprite 233 gesetzt)
  typemask AS LONG  'Terrain-Eigenschaft (B0=Stra�e , B1=Weg , B2=Graben , B3=Schiene , B4=kleines Hindernis , ... , B6=H�gel , B7=Berg
                    '                     B8=Abhang , B9=Grasland/Kornfeld? , B10=Wald , ... , B12=Wasserbecken? , ... , B15=Fels im Wasser
                    '                     B16=flaches Wasser , B17=mittleres Wasser , B18=tiefes Wasser, B19=? , B20=Geb�ude , B21=Geb�ude(betretbar) , B22=Geb�udeeingang)
  animationstart AS INTEGER  'Animations-Startsprite
  defense AS INTEGER
  u14 AS INTEGER
  animationlength AS BYTE  'Animations-L�nge
  u17 AS BYTE
  u18 AS INTEGER
  weathermapping(4) AS INTEGER  '20-21 = Sprite f�r Sch�nwetter , 22-23 = Sprite f�r leichter Regen , 24-25 = Sprite f�r starker Regen , 26-27 = Sprite f�r leichter Schneefall , 28-29 = Sprite f�r starker Schneefall
  u30(15) AS BYTE
END TYPE

'Waffe
TYPE TWeapon
  wname AS ASCIIZ*33  'Waffenname
  ammo AS INTEGER     'max. Ammo
  minrange AS BYTE    'min. Reichweite
  maxrange AS BYTE    'max. Reichweite
  damage AS WORD      'Schaden
  targets AS DWORD    'Zielfelder (B0=Hohe Luft , B1=Luft , B2=Land , B3=Wasser , B4=Getaucht , B5=Strukturen , B8=Ammo , B9=BTreibstoff , B10=?Sinus? , B11=Reparaturmaterial
END TYPE

'Einheitenklasse (speichert alle statischen Einheiten-Daten)
TYPE TUnitClass
  uname AS WSTRINGZ*33        'Einheitenname
  description AS WSTRINGZ*33  'Einheitenbeschreibung
  fuel AS WORD              'max. Treibstoff
  fuelusage AS BYTE         'Treibstoffverbrauch pro Feld
  weight AS BYTE            'Gewicht
  costenergy AS BYTE        'Baukosten Energie
  costmaterial AS BYTE      'Baukosten Material
  groupsize AS BYTE         'Gruppenst�rke
  range AS BYTE             'Reichweite (in Achtelfeldern)
  weapons(3) AS TWeapon     'Waffen
  armor AS WORD             'Panzerung
  attack AS WORD            '???
  accuracy AS WORD          'Zielgenauigkeit bei Fernkampf
  flags AS DWORD            'Flags (R�ckzug, Geb�udeBesetzen, etc.)
  terrain AS DWORD          'betretbares Terrain
  sight AS BYTE             'Sichtweite (in Achtelfeldern)
  sightblock AS BYTE        'Radarst�rung (in Achtelfeldern)
  shopmask AS BYTE          'betretbare Shops
  transportmask AS BYTE     'Transportmaske (welche Art von Einheiten transportiert werden k�nnen)
  transportclass AS BYTE    'Transportklasse (Art als die die Einheit z�hlt wenn sie transportiert wird)
  transportvolume AS BYTE   'Transportvolumen
  rating AS WORD            'Einstufung durch Server
  sfxselection AS WORD      'Sound-Effekt bei Selektion der Einheit
  sfxfire AS WORD           'Sound-Effekt f�r Kampf
  artworkhandle AS LONG     'Handle des Artwork Bitmaps (identisch mit dem Wert aus hArtwork&())
END TYPE

'Einheit (speichert alle dynamischen Einheiten-Daten)
TYPE TUnit
  unittype AS BYTE                'Verweis auf die zugeh�rige Einheitenklasse
  owner AS BYTE                   'Besitzer
  refreshtime AS BYTE             'Reaktivierung (in Minuten relativ zum Rundenanfang)
  groupsize AS BYTE               'aktuelle Gruppenst�rke
  fuel AS WORD                    'verbleibender Treibstoff
  ammo(3) AS WORD                 'verbleibende Munition
  transportvolume AS BYTE         'belegte Transportkapazit�t
  experience AS BYTE              'Erfahrungspunkte
  flags AS LONG                   'Flags (getaucht, aufgestiegen, wurdeBewegt, etc.)
  xpos AS BYTE                    'X-Position
  ypos AS BYTE                    'Y-Position
  zpos AS BYTE                    'Ebene (0=hochfliegend , 1=fliegend , 2=Land , 3=Wasser , 4=getaucht)
  direction AS BYTE               'Richtung
  moved AS BYTE                   'Anzahl Felder die sich die Einheit diese Runde bewegt hat
  xposold AS BYTE                 'X-Position am Anfang der Runde
  yposold AS BYTE                 'Y-Position am Anfang der Runde
  transportcontent(7) AS INTEGER  'Einheiten in diesem Transporter
  'AI-Felder m�ssen am Ende der Struktur stehen
  aimissioncommand AS INTEGER     'durch die Mission vorgegebener AI Befehl
  aicommand AS WORD               'Befehl f�r diese Runde
  aitargetpriority AS WORD        'Zielpriorit�t f�r AI
  aitargetunit AS INTEGER         'Angriffsziel
  aitargetshop AS INTEGER         'aufzusuchender Shop
END TYPE

'Geb�ude
TYPE TShop  '80 Bytes
  unittype AS INTEGER  '1=Shop , 2=AI-Point
  position AS WORD
  shopfunction AS INTEGER  '1=HQ , 2=Produktion , 4=Depot , 8=Akademie , 16=Stadt , 32=Transporter
  sprite AS INTEGER     'unbekannt, wird hier als Sprite-Nummer genutzt
  nameindex AS INTEGER  'Index in Shopfile (nur f�r Geb�ude)
  position2 AS WORD  '0 f�r AI-Points
  energy AS INTEGER
  material AS INTEGER
  eplus AS BYTE
  mplus AS BYTE
  content(15) AS INTEGER
  u5 AS BYTE
  shoptype AS BYTE  '0=Einheit , 1 = HQ , 2=Flughafen , 4=Hafen , 8=Fabrik , 16=Depot , 32=Stadt , 64=Akademie
  production(3) AS INTEGER
  u6 AS INTEGER
  prio AS INTEGER
  owner AS INTEGER
  aidata1 AS INTEGER
  aidata2 AS INTEGER
  aidata3 AS INTEGER
  aicommand AS INTEGER
  aidata5 AS INTEGER
  aidata6xxx AS INTEGER
  aidata7 AS INTEGER
END TYPE

'Kampf-Parameter
TYPE TCombatInfo
  attacker AS LONG       'angreifende Einheit
  defender AS LONG       'angegriffene Einheit
  weaponatt AS LONG      'Waffe des Angreifers (0 oder 1-4)
  weapondef AS LONG      'Waffe des Verteidigers (0 oder 1-4)
  params(5, 1) AS LONG   'Kampfwerte beider Einheiten (Waffenschaden, Terrain, Klemmen, Initiative, Gesamtwert, genommener Schaden)
END TYPE

'Channel-Informationen
TYPE TChannelInfo
  state AS BYTE                      'Channel-Zustand
  connectioncount AS BYTE            'Anzahl aktiver Client-Verbindungen (nur Server) / Anzahl menschlicher Spieler
  countdown AS BYTE                  'Spielstart-Countdown (nur Multiplayer Modus)
  localplayer AS BYTE                'Nummer des lokalen Spielers (nur Multiplayer Modus)
  fromSavegame AS LONG               'wird auf 1 gesetzt, damit im Mehrspielermodus die Clients nicht die Karte laden
  turn AS LONG                       'Rundennummer
  movement AS LONG                   'Bewegungsnummer in dieser Runde (des menschlichen Spielers)
  xsize AS LONG                      'Kartenbreite
  ysize AS LONG                      'Kartenh�he
  nunits AS LONG                     'Anzahl Einheiten auf der Karte
  nshops AS LONG                     'Anzahl Geb�ude auf der Karte
  nactions AS LONG                   'Anzahl Aktionen in der Mission (inklusive Siegbedingungen)
  nvictoryconditions AS LONG         'Anzahl Aktionen die Siegbedingungen sind
  requiredvictorycond AS LONG        'Anzahl ben�tigter erf�llter Siegbedingungen
  turnlimit AS LONG                  'Maximalanzahl Z�ge (Siegbedingung; -1 f�r kein Zuglimit)
  turnlimitisvictory AS LONG         'wenn auf 1 gesetzt, dann z�hlt Erreichen des Zeitlimits als Sieg
  actionposition AS LONG             'Nummer der Aktion, die als n�chstes eintreten wird
  currentmission AS LONG             'Nummer dieser Karte
  nextmission AS LONG                'Nummer der n�chsten Karte
  bonusmission AS LONG               'Nummer der Bonusmission
  missionbriefing AS LONG            'Nachrichten-ID der Missionsbeschreibung
  nteams AS LONG                     'Anzahl Teams auf der Karte
  originalplayercount AS LONG        'Anzahl der urspr�nglich auf der Karte vorhandenen Spieler
  originalplayers AS LONG            'Bitmaske der urspr�nglich auf der Karte vorhandenen Spieler
  aliveplayers AS LONG               'Bitmaske der noch auf der Karte vorhandenen Spieler
  aimask AS LONG                     'Bitmaske welche Spieler von der AI gesteuert werden
  ppt AS LONG                        'Spieler pro Team
  difficulty AS LONG                 'Schwierigkeitsgrad
  playergpm AS QUAD                  'allgemeine Produktionspalette f�r menschliche Spieler
  aigpm AS QUAD                      'allgemeine Produktionspalette f�r AI  Spieler
  startenergy AS LONG                'Anfangsenergie f�r neue Spieler
  weather AS LONG                    'Wetter
  activeplayer AS LONG               'aktiver Spieler
  activeteam AS LONG                 'aktives Team (nur Multiplayer Modus)
  surprisedunit AS LONG              'Einheit die mit einem �berraschungsangriff angegriffen wird
  checksum AS LONG                   'Kontrollsumme �ber die Bytes der Missionsdatei
  netbufferaccess AS LONG            'Semaphore f�r Zugriff auf netbuffers$()
  hAIThread AS LONG                  'AI-Thread
  unused AS LONG                     'Thread zur Verarbeitung von Client-Anfragen (nur Server)
  unused2 AS LONG                     '
  cname AS ASCIIZ*33                 'Channelname
  mapdescription AS ASCIIZ*256       'Kartenschreibung
  mapshortdescr AS ASCIIZ*128        'Kartenkurzbeschreibung
  'letztes Element in der Struktur
  shopnames(%MAXSHOPS) AS ASCIIZ*32  'Namen der Shops
END TYPE

'Spieler
TYPE TPlayer
  state AS LONG                      'Status
  phase AS LONG                      'Phase
  allymask AS LONG                   'verb�ndete Spieler (1 Bit pro Spieler)
  team AS LONG                       'eindeutige Team-ID f�r alle Spieler mit derselben Allianzmaske
  energy AS LONG                     'aktuelle Energie des Spielers
  animationfinished AS LONG          'wird auf 1 gesetzt, wenn der Client die Animation abgeschlossen hat
  selectedunit AS LONG               'ausgew�hlte Einheit
  selectedtarget AS LONG             'Ziel f�r die ausgew�hlte Einheit
  killedunits AS LONG                'Anzahl vernichteter Feindeinheiten
  capturedunits AS LONG              'Anzahl eroberter Einheiten
  producedunits AS LONG              'Anzahl produzierter Einheiten
  lostunits AS LONG                  'Anzahl verlorener Einheiten
  aiglobaltargetx AS LONG            'X-Position des Feldes auf dem die AI Siegbedingung zu erf�llen ist
  aiglobaltargety AS LONG            'Y-Position des Feldes auf dem die AI Siegbedingung zu erf�llen ist
  unitpathlen AS LONG                'L�nge des Pfads den die gew�hlte Einheit bewegt werden soll
  unitpathpos AS LONG                'Position im Pfad den die gew�hlte Einheit bewegt werden soll
  unitanimstep AS LONG               'Animationsschritt (0 bis %FRAMES_PER_UNIT_MOVE_ANIMATION)
  unitpathx(%MAXUNITPATH-1) AS BYTE  'Pfad den die gew�hlte Einheit bewegt werden soll
  unitpathy(%MAXUNITPATH-1) AS BYTE  'Pfad den die gew�hlte Einheit bewegt werden soll
  'letztes Element in der Struktur
  targets(255,255) AS BYTE           'B0-B5 = %TG_MOVE .. %TG_BUILD
END TYPE

'Kartenver�nderungen
TYPE TDFData
  xpos AS BYTE         'X-Position
  ypos AS BYTE         'Y-Position
  terrain AS INTEGER   'neues Terrain
  overlay AS INTEGER   'neues Overlay
  unit AS INTEGER      'zu erzeugende Einheit
END TYPE

'Aktionen
TYPE TAction
  category AS BYTE     'Kategorie (0=normal , 48=Siegbedingung , 80=Niederlagenbedingung) , enth�lt aber auch die Aktionsstufe
  u1 AS BYTE
  turn AS WORD         'Runde
  movement AS WORD     'Bewegung in der Runde
  player AS BYTE       'Spieler
  actiontype AS BYTE   'Typ
  u2 AS LONG
  actionparam AS WORD  'Aktionsparameter
  shop AS WORD         'Shopparameter
  newally AS WORD      'Allianzparameter
  u3(21) AS BYTE
END TYPE

'Kampagne
TYPE TCampaign
  episode AS LONG       'Nummer der Episode
  time AS LONG          'Gesamtspielzeit (in Sekunden)
  groundscore AS LONG   'Gesamtpunkte f�r Bodeneinheiten
  waterscore AS LONG    'Gesamtpunkte f�r Wassereinheiten
  airscore AS LONG      'Gesamtpunkte f�r Lufteinheiten
  secrets AS LONG       'Anzahl gefundener geheimer Missionen
END TYPE

'Channel
TYPE TChannel
  info AS TChannelInfo               'Channel-Infos
  combat AS TCombatInfo              'Kampf-Infos
  zone1(255,255) AS INTEGER          'Terrain-Ebene
  zone2(255,255) AS INTEGER          'Overlay/Geb�ude-Ebene
  zone3(255,255) AS INTEGER          'Einheiten-Ebene
  explored(255,255) AS BYTE          'erforschte Karte (1 Bit pro Spieler)
  vision(255,255) AS BYTE            'aktuelle �berwachte Karte (1 Bit pro Spieler)
  sightblock(255,255) AS WORD        'Sichtblockierung (2 Bits pro Team - Bit 0-7 = einfache Blockade , Bit 8-15 = doppelte Blockade)
  UNITS(%MAXUNITS) AS TUnit          'Einheiten
  shops(%MAXSHOPS) AS TShop          'Geb�ude
  actions(%MAXACTIONS) AS TAction    'Aktionen
  player(%MAXPLAYERS) AS TPlayer     'Spieler
  campaign AS TCampaign              'Kampagne
  dfoffsets(%MAXDFLAYER) AS WORD     'Zeiger auf dflayer()
  dfcount(%MAXDFLAYER) AS WORD       'Anzahl S�tz in dem DF-Layer
  dflayer(%MAXDFRECORDS) AS TDFData  'Kartenver�nderungen
END TYPE

'Channeldaten die nicht im Spielstand gespeichert werden m�ssen
TYPE TChannelNoSave
  nunitclasses AS LONG               'Anzahl Einheitenklassen
  nterrain AS LONG                   'Anzahl Terrain-Definitionen
  crystalClass AS LONG               'Einheitenklasse von Aldinium
  unitclasses(%MAXUNITCLASSES-1) AS TUnitClass
  terraindef(%MAXTERRAINDEF-1) AS TTerrain
  playertargets(255,255) AS BYTE
END TYPE

'Breitensuche
TYPE TZoneSearch
  x AS BYTE      'X-Position
  y AS BYTE      'Y-Position
  v AS INTEGER   'Bewertung diese Felds
END TYPE

'Highscore
TYPE THighScore
  playername AS STRING*16
  missionnumber AS LONG
  turnnumber AS LONG
  scoreground AS LONG
  scorewater AS LONG
  scoreair AS LONG
  difficulty AS BYTE
  u1 AS BYTE
  u2 AS BYTE
  u3 AS BYTE
  scoredate AS LONG
  u4(19) AS BYTE
END TYPE

'Animations-Skript
TYPE TAnimationScript
  animtype AS LONG        'Video/Sound/Text
  sequence AS LONG        'Nummer des Video/Sound/Text
  starttime AS LONG       'Startzeitpunkt (in 1/25 Sekunden)
  duration AS LONG        'L�nge (in 1/25 Sekunden)
  animationspeed AS LONG  'Bilder pro Sekunde
  freezetime AS LONG      'letzten Frame als Standbild zeigen (in 1/25 Sekunden)
  zpos AS LONG            'Bildebene
  xoffset AS LONG         'Offset relativ zum linken Rand in Pixeln
  yoffset AS LONG         'Offset relativ zum oberen Rand in Pixeln
  frame AS LONG           'Nummer des Video-Frames, der zu diesem Zeitpunkt angezeigt werden soll
END TYPE

'Battle Isle 3 Missions-Videos
TYPE TMissionVideo
  cutscene AS LONG
  briefing AS LONG
  victory AS LONG
  defeat AS LONG
END TYPE

'Socket-Verbindung
TYPE TConnection
  fnr AS LONG               'Socket-Handle (Dateinummer)
  ip AS LONG                'IP-Adresse des Spielers
  securityquestion AS LONG  'Autorisierungs-Frage
  securityanswer AS LONG    'Autorisierungs-Antwort (erwartete Antwort)
  authentificated AS LONG   'Client wurde erfolgreich authentifiziert
  chnr AS LONG              'Channel in dem sich der Spieler befindet
  snr AS LONG               'Spielernummer im Channel
  xp AS LONG                'Erfahrung des Spielers
  playername AS ASCIIZ*17   'Spielername
END TYPE

'Struktur f�r Wolken im Intro
TYPE TIntroCloud
  xpos AS SINGLE
  ypos AS SINGLE
  size AS SINGLE
  rotation AS SINGLE
  speed AS SINGLE
  opacity AS SINGLE
  color AS LONG
END TYPE

'Struktur f�r Geschosse/Raketen
TYPE TMissile
  startx AS LONG
  starty AS LONG
  endx AS LONG
  endy AS LONG
  animationlength AS LONG
  position AS LONG
  ownertype AS LONG
  weapontargets AS LONG
  damage AS LONG
END TYPE

'Struktur f�r Zwischensequenz-Objekte
TYPE TCutSceneObject
  xpos AS SINGLE
  ypos AS SINGLE
  width AS SINGLE
  height AS SINGLE
  opacity AS SINGLE
  frame AS SINGLE
  xspeed AS SINGLE
  yspeed AS SINGLE
  widthadd AS SINGLE
  heightadd AS SINGLE
  opacitygrowth AS SINGLE
  frameadd AS SINGLE
END TYPE


'Channel
GLOBAL channels() AS TChannel, nchannels&
GLOBAL channelsnosave() AS TChannelNoSave
GLOBAL netbuffers$()
