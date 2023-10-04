'Battle Isle 2020 - Initialisierungsroutinen


'Konfigurationsdatei einlesen
FUNCTION ReadConfig&
  LOCAL a$, b$, v$, languages$, length&, p&, q&

  'Standardwerte festlegen
  langcode$ = "ENG"
  languages$ = "GER407,ENG409"
  fullscreenMode& = 1
  defaultServer$ = "www.kitana.org"
  localPlayerName$ = "Val Haris"
  musicVolume& = 50
  speechVolume& = 100
  effectVolume& = 80
  checkInstallation& = 1

  'Datei einlesen
  IF ISFILE(EXEPATH$+configfilename$) <> 0 THEN a$ = ReadFileContent$(configfilename$, 0)
  orgConfigData$ = a$
  REPLACE CHR$(13,10) WITH CHR$(13) IN a$
  REPLACE CHR$(10) WITH CHR$(13) IN a$
  length& = LEN(a$)

  'Datei auswerten
  localPlayerXP& = 0
  p& = 1
  WHILE p& <= length&
    q& = INSTR(p&, a$, CHR$(13))
    IF q& = 0 THEN q& = length&+1
    b$ = TRIM$(MID$(a$, p&, q&-p&))   'eine Zeile
    p& = q&+1
    IF LEFT$(b$, 1) = "#" THEN b$ = ""  'Kommentare entfernen
    IF b$ <> "" THEN
      q& = INSTR(b$, "=")
      IF q& > 0 THEN
        v$ = LTRIM$(MID$(b$, q&+1))            'Wert
        b$ = UCASE$(RTRIM$(LEFT$(b$, q&-1)))   'Attribut
        SELECT CASE b$
        CASE "LANGUAGE": langcode$ = v$
        CASE "AVAILABLELANGUAGES": languages$ = v$
        CASE "FULLSCREEN": fullscreenMode& = IIF&(UCASE$(TRIM$(v$)) = "YES", 1, 0)
        CASE "WINDOWWIDTH": windowWidth& = VAL(v$)
        CASE "WINDOWHEIGHT": windowHeight& = VAL(v$)
        CASE "DIFFICULTY": defaultDifficulty& = VAL(v$)
        CASE "DEFAULTSERVER": defaultServer$ = v$
        CASE "PLAYERNAME": localPlayerName$ = LEFT$(v$, 16)
        CASE "MUSICVOLUME": musicVolume& = VAL(v$)
        CASE "SPEECHVOLUME": speechVolume& = VAL(v$)
        CASE "EFFECTVOLUME": effectVolume& = VAL(v$)
        CASE "DISABLEOGG": disableOGG& = IIF&(UCASE$(TRIM$(v$)) = "YES", 1, 0)
        CASE "COMBATMODE": combatMode& = VAL(v$)
        CASE "UNITANIMATIONFRAMEWIDTH": unitAnimFrameWidth& = VAL(v$)
        CASE "UNITANIMATIONFRAMEHEIGHT": unitAnimFrameHeight& = VAL(v$)
        CASE "UNITANIMATIONFRAMESPERSECOND": unitAnimFPS& = VAL(v$)
        CASE "VOICEGER0": voices$(0, 0) = TRIM$(v$)
        CASE "VOICEGER1": voices$(1, 0) = TRIM$(v$)
        CASE "VOICEGER2": voices$(2, 0) = TRIM$(v$)
        CASE "VOICEGER3": voices$(3, 0) = TRIM$(v$)
        CASE "VOICEGER4": voices$(4, 0) = TRIM$(v$)
        CASE "VOICEGER5": voices$(5, 0) = TRIM$(v$)
        CASE "VOICEGER6": voices$(6, 0) = TRIM$(v$)
        CASE "VOICEENG0": voices$(0, 1) = TRIM$(v$)
        CASE "VOICEENG1": voices$(1, 1) = TRIM$(v$)
        CASE "VOICEENG2": voices$(2, 1) = TRIM$(v$)
        CASE "VOICEENG3": voices$(3, 1) = TRIM$(v$)
        CASE "VOICEENG4": voices$(4, 1) = TRIM$(v$)
        CASE "VOICEENG5": voices$(5, 1) = TRIM$(v$)
        CASE "VOICEENG6": voices$(6, 1) = TRIM$(v$)
        CASE "INSTALLED": checkInstallation& = IIF&(VAL(TRIM$(v$))*1000 < %VERSION, 1, 0)
        CASE "DEBUGENABLED": IF UCASE$(TRIM$(v$)) = "YES" THEN debugEnabled& = 1
        CASE "UNLIMITEDTURNS": IF UCASE$(TRIM$(v$)) = "YES" THEN cheatUnlimitedTurns& = 1
        CASE "COMBATPREVIEW": IF UCASE$(TRIM$(v$)) = "YES" THEN cheatCombatPreview& = 1
        CASE "LOGFILE": logFilename$ = v$
        CASE "AVIFOLDER": aviFolder$ = v$
        END SELECT
        IF LEFT$(b$, 8) = "MAPSCORE" THEN
          q& = VAL(MID$(b$, 9))
          IF q& >= 0 AND q& < 1000 THEN
            mapscore$(q&) = v$
            localPlayerXP& = localPlayerXP&+1
          END IF
        END IF
      END IF
    END IF
  WEND

  'unterstützte Sprachen extrahieren
  q& = PARSECOUNT(languages$, ",")
  REDIM supportedLanguages$(q&-1)
  FOR p& = 1 TO q&
    supportedLanguages$(p&-1) = PARSE$(languages$, ",", p&)
    CALL BIDebugLog("Supported language: "+supportedLanguages$(p&-1))
  NEXT p&
  languageNr& = IIF&(langcode$ = "GER", 0, 1)
  CALL BIDebugLog("Selected language is "+langcode$+".")

  IF aviFolder$ = "" OR ISFOLDER(aviFolder$) = 0 THEN aviFolder$ = EXEPATH$+"AVI"
  IF RIGHT$(aviFolder$, 1) <> "\" THEN aviFolder$ = aviFolder$+"\"
  effectiveMusicVolume& = musicVolume&

  ReadConfig& = 1
END FUNCTION



'Konfigurationsdatei speichern, falls Einstellungen geändert wurden
SUB SaveConfig
  LOCAL a$, b$, i&, k&

  'Konfiguration erstellen
  a$ = "Language="+langcode$+CHR$(13,10) _
    + "FullScreen="+IIF$(fullscreenMode& = 1, "Yes", "No")+CHR$(13,10) _
    + "WindowWidth="+FORMAT$(windowWidth&)+CHR$(13,10) _
    + "WindowHeight="+FORMAT$(windowHeight&)+CHR$(13,10) _
    + "Difficulty="+FORMAT$(defaultDifficulty&)+CHR$(13,10) _
    + "DefaultServer="+defaultServer$+CHR$(13,10) _
    + "PlayerName="+localPlayerName$+CHR$(13,10) _
    + "Experience="+FORMAT$(localPlayerXP&)+CHR$(13,10) _
    + "MusicVolume="+FORMAT$(musicVolume&)+CHR$(13,10) _
    + "SpeechVolume="+FORMAT$(speechVolume&)+CHR$(13,10) _
    + "EffectVolume="+FORMAT$(effectVolume&)+CHR$(13,10) _
    + "DisableOGG="+IIF$(disableOGG& = 1, "Yes", "No")+CHR$(13,10) _
    + "CombatMode="+FORMAT$(combatMode&)+CHR$(13,10) _
    + "UnitAnimationFrameWidth="+FORMAT$(unitAnimFrameWidth&)+CHR$(13,10) _
    + "UnitAnimationFrameHeight="+FORMAT$(unitAnimFrameHeight&)+CHR$(13,10) _
    + "UnitAnimationFramesPerSecond="+FORMAT$(unitAnimFPS&)+CHR$(13,10) _
    + "Installed="+IIF$(checkInstallation& = 1, "0", FORMAT$(%VERSION/1000, 3))+CHR$(13,10) _
    + "Logfile="+logFilename$+CHR$(13,10) _
    + "AVIFolder="+aviFolder$+CHR$(13,10) _
    + "AvailableLanguages="+JOIN$(supportedLanguages$(), ",")+CHR$(13,10)
  FOR k& = 0 TO 1
    FOR i& = 0 TO 6
      a$ = a$+"Voice"+IIF$(k& = 0, "GER", "ENG")+FORMAT$(i&)+"="+voices$(i&, k&)+CHR$(13,10)
    NEXT i&
  NEXT k&
  FOR i& = 0 TO 999
    IF mapscore$(i&) <> "" THEN a$ = a$+"MapScore"+FORMAT$(i&)+"="+mapscore$(i&)+CHR$(13,10)
  NEXT i&
  IF debugEnabled& <> 0 THEN a$ = a$+"DebugEnabled=Yes"+CHR$(13,10)
  IF cheatUnlimitedTurns& <> 0 THEN a$ = a$+"UnlimitedTurns=Yes"+CHR$(13,10)
  IF cheatCombatPreview& <> 0 THEN a$ = a$+"CombatPreview=Yes"+CHR$(13,10)

  'prüfen, ob Änderungen vorhanden sind
  IF a$ = orgConfigData$ THEN EXIT SUB

  'Konfiguration speichern
  CALL WriteFileContent(configfilename$, a$)
  orgConfigData$ = a$
END SUB



'Kommandozeilenparameter auswerten
SUB ProcessCommandArgs(c$)
  LOCAL i&, n&, missionnr&, episode&, a$

  serverConsoleMode& = 1
  n& = PARSECOUNT(c$, " ")
  i& = 1
  WHILE i& <= n&
    a$ = UCASE$(PARSE$(c$, " ", i&))
    SELECT CASE a$
    CASE "/SERVER"
      gameMode& = %GAMEMODE_SERVER
    CASE "/NOCONSOLE"
      serverConsoleMode& = 0
    CASE "/SIMPLECONSOLE"
      serverConsoleMode& = 2
    CASE "/LOGTRAFFIC"
      enabledTrafficLog& = 1
    CASE "/HOSTGAME"
      startupaction& = %STARTACTION_HOSTGAME
    CASE "/JOIN"
      startupaction& = %STARTACTION_JOINGAME
    CASE "/MAP"
      IF i& < n& THEN
        i& = i&+1
        startupMap$ = PARSE$(c$, " ", i&)
        startupaction& = %STARTACTION_STARTMAP
      END IF
    CASE "/TESTMAP"
      IF i& < n& THEN
        i& = i&+1
        startupMap$ = PARSE$(c$, " ", i&)
        REPLACE "+" WITH " " IN startupMap$
        startupaction& = %STARTACTION_TESTMAP
      END IF
    CASE "/INI"
      IF i& < n& THEN i& = i&+1 : configfilename$ = PARSE$(c$, " ", i&)
    CASE "/REPLAY"
      IF i& < n& THEN
        i& = i&+1
        startupReplay$ = PARSE$(c$, " ", i&)
        IF INSTR(startupReplay$, "\") = 0 THEN startupReplay$ = "RPL\"+startupReplay$
        startupaction& = %STARTACTION_REPLAY
      END IF
    CASE "/DEBUGLOG"
      enableDebugLog& = 1
    END SELECT
    i& = i&+1
  WEND

  a$ = DATE$
  CALL BIDebugLog("BI2020 started on "+RIGHT$(a$, 4)+"-"+LEFT$(a$, 2)+"-"+MID$(a$, 4, 2)+" "+TIME$)
  CALL BIDebugLog("Command line arguments: "+c$)
END SUB



'Terrain-Definitionen einlesen
FUNCTION ReadTerrainDefs&(chnr&)
  LOCAL f$, d$, length&, i&, sprnr&, episode&

  'Datei einlesen
  episode& = GetEpisodeForMap&(channels(chnr&).info.currentmission)
  f$ = AdjustFilenameForEpisode$(EXEPATH$+$TERRAINDEFFILE, episode&, 1)
  d$ = ReadFileContent$(f$, 0)
  length& = LEN(d$)
  channelsnosave(chnr&).nterrain = length&/46  '46 Bytes pro Terrain
  IF channelsnosave(chnr&).nterrain = 0 THEN EXIT FUNCTION

  'Datenstruktur befüllen
  POKE$ VARPTR(channelsnosave(chnr&).terraindef(0)), d$

  'Shops
  FOR i& = 1 TO LEN($BUILDINGSPRITES)/2
    sprnr& = CVI($BUILDINGSPRITES, i&*2-1)
    channelsnosave(chnr&).terraindef(sprnr&).animationstart = i&-1
  NEXT i&

  ReadTerrainDefs& = 1
END FUNCTION



'Sprachdatei einlesen
FUNCTION ReadLangFile&(f$)
  LOCAL a$, b$, s$, l&, p&, q&, nr&
  DIM words$$(%MAXWORDS-1)

  'Datei einlesen
  a$ = ReadFileContent$(f$, 1)
  l& = LEN(a$)
  IF l& = 0 THEN EXIT FUNCTION

  'Zeilen verarbeiten
  p& = 1
  WHILE p& <= l&
    q& = INSTR(p&, a$, CHR$(13))
    IF q& = 0 THEN q& = l&+1
    b$ = RTRIM$(MID$(a$, p&, q&-p&))
    IF LEFT$(b$, 1) = "#" AND RIGHT$(b$, 1) = "}" THEN
      'gültige Zeile gefunden
      nr& = VAL(MID$(b$, 2, 4))
      p& = INSTR(b$, "{")
      IF p& > 0 THEN
        s$ = MID$(b$, p&+1, LEN(b$)-p&-1)
        IF nr& >= 0 AND nr& < %MAXWORDS THEN words$$(nr&) = UTF8TOCHR$(s$)
      END IF
    END IF
    p& = q&+1
  WEND

  ReadLangFile& = 1
END FUNCTION



'XOR verschlüsselten String entschlüsseln
FUNCTION DecodeXOR$(a$)
  LOCAL k$, r$, i&, keypos&, keylen&

  k$ = "FUCKBAUEREWALD"
  r$ = a$
  keypos& = 1
  keylen& = LEN(k$)
  FOR i& = 1 TO LEN(a$)
    ASC(r$, i&) = ASC(r$, i&) XOR ASC(k$, keypos&)
    keypos& = keypos&+1
    IF keypos& > keylen& THEN keypos& = 1
  NEXT i&

  DecodeXOR$ = r$
END FUNCTION



'Komprimierte TXT Datei entpacken
FUNCTION DecodeTPWM$(a$)
  LOCAL id$, rep$, decoded$
  LOCAL orglen&, p&, inputlen&, c&, n&, q&, plaintextmask&, b&

  'Header auslesen
  id$ = LEFT$(a$, 4)
  IF id$ <> "TPWM" THEN
    DecodeTPWM$ = a$
    EXIT FUNCTION
  END IF
  orglen& = CVL(a$, 5)
  inputlen& = LEN(a$)
  IF orglen& > inputlen&*10 THEN
    DecodeTPWM$ = DecodeXOR$(MID$(a$, 5))
    EXIT FUNCTION
  END IF

  'Zeichen dekodieren/entpacken
  p& = 9
  WHILE p& <= inputlen&
    plaintextmask& = ASC(a$, p&)
    b& = 256
    p& = p&+1
    WHILE p& <= inputlen& AND b& > 1
      b& = b&/2
      IF (plaintextmask& AND b&) = 0 THEN
        'Klartext
        c& = ASC(a$, p&)
        decoded$ = decoded$+CHR$(c&)
        p& = p&+1
      ELSE
        'wiederholte Daten
        c& = ASC(a$, p&)
        n& = c&  'die untersten 4 Bit geben die Anzahl der zu kopierender Zeichen (+3) an
        p& = p&+1
        q& = LEN(decoded$)+1-ASC(a$, p&)
        IF n& < 16 THEN
          n& = n&+3
        ELSE
          q& = q&-256*INT(n&/16)
          n& = (n& AND 15)+3
        END IF
        IF q& > 0 THEN
          rep$ = MID$(decoded$, q&, n&)
          IF LEN(rep$) < n& THEN rep$ = LEFT$(REPEAT$(n&, rep$), n&)
          decoded$ = decoded$+rep$
          p& = p&+1
        ELSE
'          PRINT "Warning: Invalid repeat position "+FORMAT$(q&)
        END IF
      END IF
    WEND
  WEND

  DecodeTPWM$ = LEFT$(decoded$, orglen&)
END FUNCTION



'Ermittelt den Pfad zur GOG.com Installation von Battle Isle
FUNCTION GetGOGBIPath$(biVersion&)
  LOCAL i&, basekey$, gamename$, gamepath$
  LOCAL subkeys$()

  basekey$ = "SOFTWARE\GOG.com\Games"
  GetRegistrySubkeys(%HKEY_LOCAL_MACHINE, basekey$, subkeys$())
  FOR i& = 0 TO UBOUND(subkeys$)
    gamename$ = GetRegistryValue$(%HKEY_LOCAL_MACHINE, basekey$+"\"+subkeys$(i&), "gameName")
    IF UCASE$(gamename$) = "BATTLE ISLE "+FORMAT$(biVersion&) THEN
      gamepath$ = GetRegistryValue$(%HKEY_LOCAL_MACHINE, basekey$+"\"+subkeys$(i&), "path")
      IF gamepath$ <> "" AND RIGHT$(gamepath$, 1) <> "\" THEN gamepath$ = gamepath$+"\"
      GetGOGBIPath$ = gamepath$
      EXIT FUNCTION
    END IF
  NEXT i&
END FUNCTION



'Ermittelt den Pfad zur Battte Isle Installations-CD
FUNCTION GetInstallCDPath$(biVersion&)
  LOCAL n&, p&, q&
  LOCAL driveStrings AS STRING*256, driveToCheck AS ASCIIZ*256

  n& = GetLogicalDriveStrings(255, VARPTR(driveStrings))  'liefert z.B. "C:\<0>D:\<0><0>"
  p& = 1
  WHILE p& < n&
    q& = INSTR(p&, driveStrings, CHR$(0))
    IF q& = 0 THEN EXIT LOOP
    driveToCheck = MID$(driveStrings, p&, q&-p&)
    IF GetDriveType(driveToCheck) = %DRIVE_CDROM THEN
      'CD-ROM Laufwerk gefunden
      IF biVersion& = 2 AND ISFILE(driveToCheck+"LBM\GAME.PAL") THEN
        GetInstallCDPath$ = driveToCheck
        EXIT FUNCTION
      END IF
      IF biVersion& = 3 AND ISFILE(driveToCheck+"SDISTART.EXE") THEN
        GetInstallCDPath$ = driveToCheck
        EXIT FUNCTION
      END IF
    END IF
    p& = q&+1
  WEND
END FUNCTION



'Zählt die Dateien in einem Ordner
FUNCTION CountFilesInFolder&(p$)
  LOCAL n&, f$

  'prüfen, ob Ordner existiert
  IF NOT ISFOLDER(p$) THEN
    CountFilesInFolder& = -1
    EXIT FUNCTION
  END IF

  'Dateien zählen
  f$ = DIR$(p$+"\*")
  WHILE f$ <> ""
    n& = n&+1
    f$ = DIR$
  WEND

  CountFilesInFolder& = n&
END FUNCTION



'Kopiert alle Dateien aus einem Verzeichnis in ein anderes
'Info: Parameter sind ohne Backslash am Ende
SUB CopyFolderContent(srcpath$, destpath$)
  LOCAL f$

  IF ISFOLDER(srcpath$) = 0 THEN EXIT SUB
  IF ISFOLDER(destpath$) = 0 THEN MKDIR destpath$
  CALL BILog("Copying files from "+srcpath$, 0)
  CALL BIDebugLog("Copying files from "+srcpath$+" to "+destpath$)

  f$ = DIR$(srcpath$+"\*")
  WHILE f$ <> ""
    FILECOPY srcpath$+"\"+f$, destpath$+"\"+f$
    f$ = DIR$
  WEND
END SUB



'Kopiert alle Dateien, die auf die Maske passen und im angegebenen Nummernbereich liegen, aus einem Verzeichnis in ein anderes und verändert die Nummer dabei
SUB CopyFolderContentAndRename(srcpath$, destpath$, filemask$, startnum&, endnum&, deststartnum&)
  LOCAL f$, num$, numposition&, srcnum&, destnum&

  IF ISFOLDER(srcpath$) = 0 THEN EXIT SUB
  IF ISFOLDER(destpath$) = 0 THEN MKDIR destpath$
  CALL BILog("Copying files from "+srcpath$, 0)
  CALL BIDebugLog("Copying files "+FORMAT$(startnum&)+" to "+FORMAT$(endnum&)+" matching "+filemask$+" from "+srcpath$+" to "+destpath$+" with new numbers starting at "+FORMAT$(deststartnum&))
  numposition& = INSTR(filemask$, "*")

  f$ = DIR$(srcpath$+"\"+filemask$)
  WHILE f$ <> ""
    num$ = MID$(f$, numposition&)
    srcnum& = VAL(num$)
    IF srcnum& >= startnum& AND srcnum& <= endnum& THEN
      destnum& = srcnum&-startnum&+deststartnum&
      FILECOPY srcpath$+"\"+f$, destpath$+"\"+LEFT$(f$, numposition&-1)+FORMAT$(destnum&, "000")+MID$(f$, numposition&+3)
    END IF
    f$ = DIR$
  WEND
END SUB



'Prüft, welche Artworks fehlen
SUB CheckMissingArtworks
  filesToDownload$ = ""
  CALL CheckMissingArtworksForEpisode(0)  'BI2
  CALL CheckMissingArtworksForEpisode(5)  'BI3
  IF filesToDownload$ <> "" THEN filesToDownload$ = filesToDownload$+CHR$(0)
END SUB



'Prüft, welche Artworks fehlen
SUB CheckMissingArtworksForEpisode(episode&)
  LOCAL f$, g$, i&, n&

  channels(0).info.currentmission = GetEpisodeStartMap&(episode&)
  IF ReadUnitDefs&(0) = 0 THEN EXIT SUB

  'Artworks suchen
  FOR i& = 0 TO channelsnosave(0).nunitclasses-1
    f$ = channelsnosave(0).unitclasses(i&).uname
    IF f$ <> "" THEN
      CALL FixArtworkFilename(f$)
      IF ISFILE(EXEPATH$+"ARTWORK\"+f$+".bmp") = 0 AND ISFILE(EXEPATH$+"ARTWORK\"+f$+".gif") = 0 AND INSTR(filesToDownload$, f$+CHR$(0)) = 0 THEN filesToDownload$ = filesToDownload$+f$+CHR$(0)
    END IF
  NEXT i&
END SUB



'Prüft, ob alle Blue Byte Dateien/Ordner vorhanden sind
FUNCTION CheckGameFiles&
  LOCAL r&, i&, nger&, neng&
  LOCAL t!
  LOCAL a$, bi2020folder$, gogbasefolder$, gogfolderbi2$, gogfolderedt$, dosboxfolder$, dosboxconf$, installcdfolder$, bi3sourcefolder$
  LOCAL uMsg AS tagMsg

  IF gameMode& = %GAMEMODE_SERVER THEN
    CheckGameFiles& = -1
    EXIT FUNCTION
  END IF

  r& = -1
  D2DGraphicBitmapLoad&("GFX\BI2020Console.png", hSkin&)
  hDialog& = -1
  D2D.OnRender(mousexpos&, mouseypos&)

  'Ordnername der BI2020 Installation ermitteln
  bi2020folder$ = EXEPATH$

  'Ordner erstellen
  IF NOT ISFOLDER(bi2020folder$+"GER") THEN MKDIR bi2020folder$+"GER"
  IF NOT ISFOLDER(bi2020folder$+"ENG") THEN MKDIR bi2020folder$+"ENG"
  IF NOT ISFOLDER(bi2020folder$+"MIS") THEN MKDIR bi2020folder$+"MIS"
  IF NOT ISFOLDER(bi2020folder$+"MUS") THEN MKDIR bi2020folder$+"MUS"
  IF NOT ISFOLDER(bi2020folder$+"ANI") THEN MKDIR bi2020folder$+"ANI"
  IF NOT ISFOLDER(bi2020folder$+"LIB") THEN MKDIR bi2020folder$+"LIB"
  IF NOT ISFOLDER(bi2020folder$+"LBM") THEN MKDIR bi2020folder$+"LBM"
  IF NOT ISFOLDER(bi2020folder$+"SAV") THEN MKDIR bi2020folder$+"SAV"
  IF NOT ISFOLDER(bi2020folder$+"RPL") THEN MKDIR bi2020folder$+"RPL"
  IF NOT ISFOLDER(bi2020folder$+"AVI") THEN MKDIR bi2020folder$+"AVI"
  IF NOT ISFOLDER(bi2020folder$+"ARTWORK") THEN MKDIR bi2020folder$+"ARTWORK"

  'Ordnername der GOG.com Battle Isle 2 Installation ermitteln oder alternativ die Battle Isle 2 Installations-CD
  gogbasefolder$ = GetGOGBIPath$(2)
  CALL BIDebugLog("GOG Battle Isle 2 base folder: "+gogbasefolder$)
  IF gogbasefolder$ = "" THEN
    installcdfolder$ = GetInstallCDPath$(2)
    CALL BIDebugLog("Battle Isle 2 CD installation folder: "+installcdfolder$)
    IF installcdfolder$ = "" THEN
      CALL PrintError("Please insert your BI2 CD or install the GOG version of BI2.")
      EXIT FUNCTION
    END IF

    'Dateien von der CD kopieren
    CALL CopyFolderContent(installcdfolder$+"GER", bi2020folder$+"GER")
    CALL CopyFolderContent(installcdfolder$+"ENG", bi2020folder$+"ENG")
    CALL CopyFolderContent(installcdfolder$+"MIS", bi2020folder$+"MIS")
    CALL CopyFolderContent(installcdfolder$+"ANI", bi2020folder$+"ANI")
    CALL CopyFolderContent(installcdfolder$+"LIB", bi2020folder$+"LIB")
    FILECOPY installcdfolder$+"LBM\GAME.PAL", bi2020folder$+"LBM\GAME.PAL"

  ELSE
    'Dateien aus der GOG Installation extrahieren
    gogfolderbi2$ = gogbasefolder$+"ISLE2\"
    gogfolderedt$ = gogbasefolder$+"SCENERY\"
    dosboxfolder$ = gogbasefolder$+"DOSBOX\"

    'DOSBox prüfen
    IF ISFILE(dosboxfolder$+"DOSBox.exe") = 0 THEN
      CALL PrintError("DOSBox not found: "+dosboxfolder$+"DOSBox.exe")
      CALL BIDebugLog("DOSBox not found: "+dosboxfolder$+"DOSBox.exe")
      EXIT FUNCTION
    END IF
    dosboxconf$ = "dosbox_bi2020.conf"

    'DOSBox-Config erstellen, um CD-Image zu mounten und Daten zu kopieren
    IF ISFILE(gogfolderbi2$+"BI2.inst") THEN
      a$ = "[autoexec]"+CHR$(13,10) _
        + "mount C "+CHR$(34)+bi2020folder$+CHR$(34)+CHR$(13,10) _
        + "imgmount D "+CHR$(34)+gogfolderbi2$+"BI2.inst"+CHR$(34)+" -t iso -fs iso"+CHR$(13,10) _
        + "copy D:\BI2\GER\*.* C:\GER"+CHR$(13,10) _
        + "copy D:\BI2\ENG\*.* C:\ENG"+CHR$(13,10) _
        + "copy D:\BI2\MIS\*.* C:\MIS"+CHR$(13,10) _
        + "copy D:\BI2\ANI\*.* C:\ANI"+CHR$(13,10) _
        + "copy D:\BI2\LIB\*.* C:\LIB"+CHR$(13,10) _
        + "copy D:\BI2\LBM\GAME.PAL C:\LBM"+CHR$(13,10) _
        + "exit"+CHR$(13,10)
      CALL WriteFileContent(dosboxconf$, a$)

      'DOSBox starten
      CALL BILog("Extracting original Battle Isle II files from: "+gogfolderbi2$, 0)
      CALL BIDebugLog("Executing: "+dosboxfolder$+"DOSBox.exe -conf "+CHR$(34)+EXEPATH$+dosboxconf$+CHR$(34)+" -noconsole -exit")
      SHELL dosboxfolder$+"DOSBox.exe -conf "+CHR$(34)+EXEPATH$+dosboxconf$+CHR$(34)+" -noconsole -exit"
    ELSE
      CALL PrintError("File not found: "+gogfolderbi2$+"BI2.inst")
      CALL BIDebugLog("File not found: "+gogfolderbi2$+"BI2.inst")
    END IF
  END IF

  'installierte Sprache prüfen
  nger& = CountFilesInFolder&(bi2020folder$+"GER")
  neng& = CountFilesInFolder&(bi2020folder$+"ENG")
  IF nger& < 40 AND neng& >= 40 THEN langcode$ = "ENG"
  IF nger& >= 40 AND neng& < 40 THEN langcode$ = "GER"
  languageNr& = IIF&(langcode$ = "GER", 0, 1)

  'Erfolg prüfen
  IF (nger& < 40 AND neng& < 40) OR CountFilesInFolder&(bi2020folder$+"MIS") < 40 THEN
    CALL PrintError("Failed to extract files. Please re-install your GOG version of Battle Isle II.")
    CALL BIDebugLog("Failed to extract files. Please re-install your GOG version of Battle Isle II.")
    D2D.OnRender(mousexpos&, mouseypos&)
    r& = 0
  END IF

  'DOSBox-Config erstellen, um CD-Image zu mounten und Daten zu kopieren (Szenario CD)
  IF gogfolderedt$ <> "" THEN
    IF ISFILE(gogfolderedt$+"BI2scen.inst") THEN
      CALL BILog("Extracting Battle Isle II scenery files from: "+gogfolderedt$, 0)
      a$ = "[autoexec]"+CHR$(13,10) _
        + "mount C "+CHR$(34)+bi2020folder$+CHR$(34)+CHR$(13,10) _
        + "imgmount D "+CHR$(34)+gogfolderedt$+"BI2scen.inst"+CHR$(34)+" -t iso -fs iso"+CHR$(13,10) _
        + "copy D:\BI2_SCEN\GER\*.* C:\GER"+CHR$(13,10) _
        + "copy D:\BI2_SCEN\ENG\*.* C:\ENG"+CHR$(13,10) _
        + "copy D:\BI2_SCEN\MIS\*.* C:\MIS"+CHR$(13,10) _
        + "copy D:\BI2_SCEN\ANI\ABCK001.LIB C:\ANI"+CHR$(13,10) _
        + "copy D:\BI2_SCEN\ANI\APER001.LIB C:\ANI"+CHR$(13,10) _
        + "copy D:\BI2_SCEN\ANI\ASCR001.LIB C:\ANI"+CHR$(13,10) _
        + "copy D:\BI2_SCEN\LIB\UNIT000.* C:\LIB"+CHR$(13,10) _
        + "exit"+CHR$(13,10)
      CALL WriteFileContent(dosboxconf$, a$)

      'DOSBox starten
      CALL BIDebugLog("Executing: "+dosboxfolder$+"DOSBox.exe -conf "+CHR$(34)+EXEPATH$+dosboxconf$+CHR$(34)+" -noconsole -exit")
      SHELL dosboxfolder$+"DOSBox.exe -conf "+CHR$(34)+EXEPATH$+dosboxconf$+CHR$(34)+" -noconsole -exit"
    ELSE
      CALL BILog("Battle Isle II scenery files not found. Skipped.", 0)
      CALL BIDebugLog("Battle Isle II scenery files not found. Skipped.")
    END IF

    'Musik kopieren
    FOR i& = 202 TO 207
      FILECOPY gogfolderbi2$+"BI"+FORMAT$(i&)+".ogg", bi2020folder$+"MUS\"+"BI"+FORMAT$(i&)+".ogg"
    NEXT i&
  END IF

  'Battle Isle 3 installieren
  gogbasefolder$ = GetGOGBIPath$(3)
  CALL BIDebugLog("GOG Battle Isle 3 base folder: "+gogbasefolder$)
  IF gogbasefolder$ = "" THEN
    installcdfolder$ = GetInstallCDPath$(3)
    CALL BIDebugLog("Battle Isle 3 CD installation folder: "+installcdfolder$)
    IF installcdfolder$ = "" THEN
      CALL PrintError("Please insert your BI3 CD or install the GOG version of BI3.")
      EXIT FUNCTION
    END IF
    bi3sourcefolder$ = installcdfolder$
  ELSE
    bi3sourcefolder$ = gogbasefolder$+"SDI\DATA\"
  END IF
  'Dateien kopieren und umbenennen
  CALL CopyFolderContentAndRename(bi3sourcefolder$+"GER", bi2020folder$+"GER", "MISS*.TXT", 100, 119, 256)  'MISS100 wird MISS256
  CALL CopyFolderContentAndRename(bi3sourcefolder$+"ENG", bi2020folder$+"ENG", "MISS*.TXT", 100, 119, 256)
  CALL CopyFolderContentAndRename(bi3sourcefolder$+"GER", bi2020folder$+"GER", "MCP*.TXT", 0, 0, 5)
  CALL CopyFolderContentAndRename(bi3sourcefolder$+"ENG", bi2020folder$+"ENG", "MCP*.TXT", 0, 0, 5)
  CALL CopyFolderContentAndRename(bi3sourcefolder$+"MIS", bi2020folder$+"MIS", "MISS*.DAT", 100, 119, 256)
  CALL CopyFolderContentAndRename(bi3sourcefolder$+"MIS", bi2020folder$+"MIS", "MCP*.DAT", 0, 0, 5)
  FILECOPY bi3sourcefolder$+"GER\UNIT000.TXT", bi2020folder$+"GER\UNIT005.TXT"  'BI3 = Episode 5
  FILECOPY bi3sourcefolder$+"ENG\UNIT000.TXT", bi2020folder$+"ENG\UNIT005.TXT"
  FILECOPY bi3sourcefolder$+"LIB\PART000.DAT", bi2020folder$+"LIB\PART005.DAT"
  FILECOPY bi3sourcefolder$+"LIB\PART000.LIB", bi2020folder$+"LIB\PART005.LIB"
  FILECOPY bi3sourcefolder$+"LIB\UNIT000.DAT", bi2020folder$+"LIB\UNIT005.DAT"
  FILECOPY bi3sourcefolder$+"LIB\UNIT000.LIB", bi2020folder$+"LIB\UNIT005.LIB"
  CALL CopyFolderContent(bi3sourcefolder$+"AVI", bi2020folder$+"AVI")
  CALL CheckAndFixBI3VideoCodec

  'Artwork prüfen
  CALL CheckMissingArtworks
  IF filesToDownload$ <> "" AND r& <> 0 THEN
    CALL ConnectToServer&(StringToIP&($HIGHSCORESERVER))
    t! = TIMER
    i& = LEN(filesToDownload$)
    'Window Nachrichten verarbeiten, bis alle fehlenden Dateien vom Server heruntergeladen wurden
    WHILE GetMessage(uMsg, %NULL, 0, 0) <> 0 AND filesToDownload$ <> "" AND t! > TIMER-5.0
      TranslateMessage uMsg
      DispatchMessage uMsg
      IF LEN(filesToDownload$) < i& THEN
        i& = LEN(filesToDownload$)
        t! = t!+2.0
      END IF
    WEND
    CALL CloseConnectionToServer("Downloading files done")
    D2D.OnRender(mousexpos&, mouseypos&)
    'IF filesToDownload$ <> "" THEN r& = 0  'ignore missing artworks
  END IF

  'temporäre Dateien löschen
  IF ISFILE(bi2020folder$+dosboxconf$) THEN KILL bi2020folder$+dosboxconf$
  IF ISFILE(bi2020folder$+"stderr.txt") THEN KILL bi2020folder$+"stderr.txt"
  IF ISFILE(bi2020folder$+"stdout.txt") THEN KILL bi2020folder$+"stdout.txt"

  CheckGameFiles& = r&
END FUNCTION



'Löscht alle Dateien von alten BI2020 Versionen
SUB CleanUpOldVersionFiles
  LOCAL p$, f$, i&

  IF gameMode& = %GAMEMODE_SERVER THEN EXIT SUB

  p$ = EXEPATH$+"GFX\"
  FOR i& = 1 TO DATACOUNT
    f$ = READ$(i&)
    IF ISFILE(p$+f$) THEN KILL p$+f$
  NEXT i&

  DATA "combat.png"
  DATA "combat2.png"
  DATA "highscore.png"
  DATA "konsole-bi2020.png"
  DATA "mainmenu.png"
  DATA "mapinfo.png"
  DATA "mplobby.png"
  DATA "shop.png"
END SUB



'Legt den Registry-Key für den BI3 Video-Codec an, falls der Key nicht vorhanden ist, aber der Codec
SUB CheckAndFixBI3VideoCodec
  LOCAL regvalue$, h???
  LOCAL systemfolder AS ASCIIZ*512
  LOCAL driverskey AS ASCIIZ*512
  LOCAL valuename AS ASCIIZ*512
  LOCAL dllname AS ASCIIZ*512

  dllname = "ir32_32.dll"
  IF GetSystemDirectory(systemfolder, 511) = 0 THEN EXIT SUB
  IF DIR$(systemfolder+"\"+dllname) = "" THEN EXIT SUB

  driverskey = "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Drivers32"
  valuename = "vidc.iv32"
  regvalue$ = GetRegistryValue$(%HKEY_LOCAL_MACHINE, driverskey, valuename)
  IF regvalue$ = "" THEN
    IF RegOpenKeyEx(%HKEY_LOCAL_MACHINE, driverskey, 0, %KEY_WRITE, h???) <> 0 THEN
      RegSetValueEx %HKEY_LOCAL_MACHINE, valuename, 0, %RRF_RT_REG_SZ, dllname, LEN(dllname)+1  'wird fehlschlagen, wenn Benutzer kein Administrator ist
      RegCloseKey h???
    END IF
  END IF
END SUB



'Konvertiert den Inhalt einer BI2 Textdatei in ein Array
FUNCTION TextToArray&(BYVAL a$, textlines$())
  LOCAL l&, p&, q&, nr&, mx&
  LOCAL b$

  'Umlaute ersetzen
  IF langcode$ = "GER" THEN
    REPLACE CHR$(142) WITH "Ä" IN a$
    REPLACE CHR$(153) WITH "Ö" IN a$
    REPLACE CHR$(154) WITH "Ü" IN a$
  END IF

  'Zeilen verarbeiten
  REDIM textlines$(99)
  l& = LEN(a$)
  p& = 1
  WHILE p& <= l&
    q& = INSTR(p&, a$, CHR$(13,10))
    IF q& = 0 THEN q& = l&+1
    b$ = RTRIM$(MID$(a$, p&, q&-p&))
    IF LEFT$(b$, 1) = "#" AND RIGHT$(b$, 1) = "}" THEN
      'gültige Zeile gefunden
      nr& = VAL(MID$(b$, 2, 4))
      IF nr& >= 0 AND nr& < 4096 THEN
        IF nr& > UBOUND(textlines$()) THEN REDIM PRESERVE textlines$(nr&+100)
        p& = INSTR(b$, "{")
        IF p& > 0 THEN textlines$(nr&) = MID$(b$, p&+1, LEN(b$)-p&-1)
        mx& = MAX&(mx&, nr&)
      END IF
    END IF
    p& = q&+2
  WEND

  'Anzahl Element zurückliefern
  REDIM PRESERVE textlines$(mx&)

  TextToArray& = mx&+1
END FUNCTION



'Liefert einen bestimmten Wert aus der Registry
FUNCTION GetRegistryValue$(rootkey&, BYVAL ky$, BYVAL attr$)
  LOCAL r&, vlsize&
  LOCAL k AS ASCIIZ*256, atr AS ASCIIZ*16384, vl AS ASCIIZ*16384

  'Wert auslesen
  k = ky$
  atr = attr$
  vlsize& = 16383
  r& = RegGetValue(rootkey&, k, atr, %RRF_RT_REG_SZ, BYVAL %NULL, BYVAL VARPTR(vl), vlsize&)
  vl = LEFT$(vl, vlsize&)

  GetRegistryValue$ = vl
END FUNCTION



'Liefert alle Unterschlüssel eines Schlüssel
SUB GetRegistrySubkeys(rootkey&, ky$, subkeys$())
  LOCAL h???, r&, i&, nSubkeys&, maxSubkeySize&, maxClassSize&, nValues&, maxValueName&, maxValueData&, secDescr&, subkeySize&
  LOCAL k AS ASCIIZ*256, cl AS ASCIIZ*256, ATR AS ASCIIZ*16384, subkeyName AS ASCIIZ*16384
  LOCAL ft AS FILETIME

  'Schlüssel öffnen
  k = ky$
  IF RegOpenKeyEx(rootkey&, k, 0, %KEY_READ, h???) <> 0 THEN
    REDIM subkeys$(0)
    EXIT SUB
  END IF

  'Schlüssel-Informationen auslesen
  r& = RegQueryInfoKey(h???, cl, 255, BYVAL %NULL, nSubkeys&, maxSubkeySize&, maxClassSize&, nValues&, maxValueName&, maxValueData&, secDescr&, ft)
  IF nSubkeys& < 1 THEN
    REDIM subkeys$(0)
    EXIT SUB
  END IF
  REDIM subkeys$(nSubkeys&-1)

  'Unterschlüssel auslesen
  FOR i& = 0 TO nSubkeys&-1
    subkeySize& = 16383
    r& = RegEnumKeyExA(h???, i&, subkeyName, subkeySize&, BYVAL %NULL, BYVAL %NULL, BYVAL %NULL, BYVAL %NULL)
    IF r& = %ERROR_SUCCESS THEN
      subkeys$(i&) = subkeyName
    END IF
  NEXT i&

  'Schlüssel schließen
  r& = RegCloseKey(h???)
END SUB



'Prüft, ob das OGG Plugin installiert ist
SUB CheckOGGFilter
  LOCAL a$, r&
  LOCAL c AS WSTRINGZ*1024, t AS WSTRINGZ*1024

  'Registry prüfen
  IF disableOGG& = 1 THEN EXIT SUB
  a$ = GetRegistryValue$(%HKEY_CLASSES_ROOT, "CLSID\{083863F1-70DE-11D0-BD40-00A0C911CE86}\INSTANCE\{1F3EFFE4-0E70-47C7-9C48-05EB99E20011}", "CLSID")
  IF a$ <> "" THEN EXIT SUB

  'Meldung, wo das Plugin heruntergeladen werden kann
  c = words$$(%WORD_MISSING_PLUGIN)
  t = words$$(%WORD_DOWNLOAD_PLUGIN)
  r& = MessageBoxW(hWIN&, t, c, %MB_OKCANCEL OR %MB_ICONQUESTION)
  IF r& = 2 THEN
    disableOGG& = 1
  ELSE
    r& = ShellExecute(BYVAL %NULL, "open", "https://xiph.org/dshow/", BYVAL %NULL, BYVAL %NULL, %SW_SHOWNORMAL)
    exitprg& = 1
    PostQuitMessage 0
  END IF
END SUB



'Stuft eine Einheit ein
FUNCTION RateUnit(u AS TUnitClass) AS WORD
  LOCAL totalrating&
  LOCAL rfire&, rarmor&, rrange&, rtype&, rsight&, rflags&, rterrain&
  LOCAL i&, r&, scale#
  LOCAL w AS TWeapon

  'Feuerkraft 600-20000
  rfire& = 0
  FOR i& = 0 TO 3
    w = u.weapons(i&)
    r& = w.damage*2
    scale# = 1
    IF w.maxrange > 1 THEN scale# = (w.maxrange-2)/2+3
    IF w.minrange > 1 THEN scale# = scale#-w.minrange*0.2
    IF w.ammo < 4 THEN scale# = scale#*(w.ammo/10+0.6)
    r& = r&*scale#
    rfire& = MAX&(rfire&, r&)
  NEXT i&

  'Panzerung 1000-6000
  rarmor& = u.armor*u.groupsize

  'Bewegungsreichweite 300-2000
  r& = u.range
  scale# = u.fuel/u.fuelusage
  rrange& = r&/4*(scale#+50)

  'Sicht 300-1000
  rsight& = u.sight/4*100+u.sightblock/4*300

  'Flags -300-1000
  rflags& = 0
  IF (u.flags AND %UCF_OCCUPY) > 0 THEN rflags& = rflags&+400
  IF (u.flags AND %UCF_RETREAT) > 0 THEN rflags& = rflags&+300
  IF (u.flags AND %UCF_COOLDOWN) > 0 THEN rflags& = rflags&-300
'  IF (u.flags AND %UF_BUILD) > 0 THEN rflags& = rflags&+250
'  IF (u.flags AND %UF_FORTIFY) > 0 THEN rflags& = rflags&+150
  IF (u.flags AND %UCF_DIVE) > 0 THEN rflags& = rflags&+400
  IF (u.flags AND %UCF_CLIMB) > 0 THEN rflags& = rflags&+500
  IF (u.flags AND %UCF_BADDEFENDER) > 0 THEN rflags& = rflags&-500
'  IF (u.flags AND %UF_TRANSPORTFOOTMEN) > 0 THEN rflags& = rflags&+100
'  IF (u.flags AND %UF_TRANSPORTCRYSTALS) > 0 THEN rflags& = rflags&+100
'  IF (u.flags AND %UF_TRANSPORTMINES) > 0 THEN rflags& = rflags&+50
'  IF (u.flags AND %UF_TRANSPORTLIGHT) > 0 THEN rflags& = rflags&+150
'  IF (u.flags AND %UF_TRANSPORTHEAVY) > 0 THEN rflags& = rflags&+200
'  IF (u.flags AND %UF_TRANSPORTAIR) > 0 THEN rflags& = rflags&+50
'  IF (u.flags AND %UF_TRANSPORTFOOTMEN+%UF_TRANSPORTCRYSTALS+%UF_TRANSPORTLIGHT+%UF_TRANSPORTHEAVY+%UF_TRANSPORTAIR) > 0 THEN rflags& = rflags&+u.armor/2+u.transportvolume*7
  IF (u.flags AND %UCF_REFUEL) > 0 THEN rflags& = rflags&+400
'  IF (u.flags AND %UF_RECHARGE) > 0 THEN rflags& = rflags&+400
  IF (u.flags AND %UCF_REPAIR) > 0 THEN rflags& = rflags&+1200

  'Terrain 100-1700
  rterrain& = 0
  FOR i& = 0 TO 30
    IF (u.terrain AND 2^i&) > 0 THEN rterrain& = rterrain&+100
  NEXT i&

'  PRINT u.uname;":";rfire&;rarmor&;rrange&;rtype&;rsight&;rflags&;rterrain&

  'Gesamtstärke berechnen
  totalrating& = rfire&*1.5 + rarmor&*1.0 + rrange&*1.0 + rtype&*1.0 + rsight&*2.0 + rflags&*1.0 + rterrain&*1.0
'  PRINT u.uname;":",totalrating&

  RateUnit = totalrating&
END FUNCTION



'Waffen-Definition erstellen
SUB MakeWeapon(d$, w AS TWeapon)
  LOCAL r$, p&, i&

  'Parameteranzahl prüfen
  IF PARSECOUNT(d$, ",") <> 9 THEN
    w.ammo = -1
    EXIT SUB
  END IF

  'Parameter extrahieren
  w.wname = TRIM$(PARSE$(d$, ",", 1))
  w.ammo = VAL(TRIM$(PARSE$(d$, ",", 2)))
  r$ = TRIM$(PARSE$(d$, ",", 3))
  p& = INSTR(r$, "-")
  IF p& = 0 THEN
    w.minrange = VAL(r$)
    w.maxrange = w.minrange
  ELSE
    w.minrange = VAL(LEFT$(r$, p&-1))
    w.maxrange = VAL(LTRIM$(MID$(r$, p&+1)))
  END IF
  FOR i& = 0 TO 0
    w.damage = VAL(TRIM$(PARSE$(d$, ",", 4+i&)))
  NEXT i&
END SUB



'Einheiten-Definition einlesen
FUNCTION ReadUnitDefs&(chnr&)
  LOCAL a$$, f$, d$, l&, i&, w&, p&, v&, totaldamage&, sfxsel$, sfxfire$, textcount&, episode&, indexname&, indexdescr&
  LOCAL unit AS TUnitClass, emptyunit AS TUnitClass
  LOCAL unitfilecontent AS STRING, unittexts() AS STRING

  'Einheitennamen einlesen
  episode& = GetEpisodeForMap&(channels(chnr&).info.currentmission)
  textcount& = -1
  f$ = AdjustFilenameForEpisode$(EXEPATH$+langcode$+"\"+$UNITNAMEFILE, episode&, 1)
  unitfilecontent = ReadFileContent(f$, 0)
  IF unitfilecontent <> "" THEN
    unitfilecontent = DecodeTPWM$(unitfilecontent)
    textcount& = TextToArray&(unitfilecontent, unittexts())
  END IF

  'Einheiten-Definition einlesen
  f$ = AdjustFilenameForEpisode$(EXEPATH$+$UNITDEFFILE, episode&, 1)
  d$ = ReadFileContent$(f$, 0)
  l& = LEN(d$)
  channelsnosave(chnr&).nunitclasses = l&/88  '88 Bytes pro Einheit
  CALL BIDebugLog("Found "+FORMAT$(channelsnosave(chnr&).nunitclasses)+" unit classes.")
  IF channelsnosave(chnr&).nunitclasses = 0 THEN EXIT FUNCTION

  'Sound-Effekte
  sfxsel$ = CHR$(%SOUNDEFFECT_TANK,%SOUNDEFFECT_TANK,%SOUNDEFFECT_TANK,%SOUNDEFFECT_MOTORCYCLE,%SOUNDEFFECT_MOTORCYCLE) _
      + CHR$(%SOUNDEFFECT_TANK,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_TANK,%SOUNDEFFECT_TANK,%SOUNDEFFECT_TANK) _
      + CHR$(%SOUNDEFFECT_TANK,%SOUNDEFFECT_TANK,%SOUNDEFFECT_TANK,%SOUNDEFFECT_TANK,%SOUNDEFFECT_TANK) _
      + CHR$(%SOUNDEFFECT_TANK,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_TANK,%SOUNDEFFECT_MOTORCYCLE,%SOUNDEFFECT_TANK) _
      + CHR$(%SOUNDEFFECT_BEEP,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_SILENCE) _
      + CHR$(%SOUNDEFFECT_MOTORCYCLE,%SOUNDEFFECT_TRAIN,%SOUNDEFFECT_TRAIN,%SOUNDEFFECT_TRAIN,%SOUNDEFFECT_TRAIN) _
      + CHR$(%SOUNDEFFECT_TRAIN,%SOUNDEFFECT_JET_FLYING,%SOUNDEFFECT_JET_FLYING_FAST,%SOUNDEFFECT_JET_FLYING_FAST,%SOUNDEFFECT_JET_FLYING_LOUD) _
      + CHR$(%SOUNDEFFECT_HELICOPTER,%SOUNDEFFECT_JET_FLYING_FAST,%SOUNDEFFECT_JET_FLYING_LOUD,%SOUNDEFFECT_JET_FLYING_LOUD,%SOUNDEFFECT_JET_FLYING) _
      + CHR$(%SOUNDEFFECT_JET_FLYING,%SOUNDEFFECT_HELICOPTER,%SOUNDEFFECT_SHIP,%SOUNDEFFECT_SHIP,%SOUNDEFFECT_SHIP) _
      + CHR$(%SOUNDEFFECT_SHIP,%SOUNDEFFECT_SUBMARINE,%SOUNDEFFECT_SUBMARINE,%SOUNDEFFECT_SHIP,%SOUNDEFFECT_SHIP) _
      + CHR$(%SOUNDEFFECT_SHIP,%SOUNDEFFECT_POWER,%SOUNDEFFECT_TANK,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_BEEP) _
      + CHR$(%SOUNDEFFECT_TANK,%SOUNDEFFECT_TANK,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_SILENCE)
  sfxfire$ = CHR$(%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_MACHINEGUN,%SOUNDEFFECT_MISSILE) _
      + CHR$(%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_PISTOL,%SOUNDEFFECT_CANNON4,%SOUNDEFFECT_CANNON,%SOUNDEFFECT_MACHINEGUN) _
      + CHR$(%SOUNDEFFECT_MISSILE,%SOUNDEFFECT_CANNON,%SOUNDEFFECT_CANNON2,%SOUNDEFFECT_CANNON,%SOUNDEFFECT_MACHINEGUN) _
      + CHR$(%SOUNDEFFECT_MACHINEGUN,%SOUNDEFFECT_PISTOL2,%SOUNDEFFECT_MISSILE2,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_MACHINEGUN) _
      + CHR$(%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_FLAK2,%SOUNDEFFECT_CANNON2,%SOUNDEFFECT_IONFIRE,%SOUNDEFFECT_EXPLOSION) _
      + CHR$(%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_CANNON,%SOUNDEFFECT_CANNON2,%SOUNDEFFECT_CANNON3) _
      + CHR$(%SOUNDEFFECT_MACHINEGUN,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_MISSILE2,%SOUNDEFFECT_MISSILE2,%SOUNDEFFECT_MACHINEGUN) _
      + CHR$(%SOUNDEFFECT_MISSILE,%SOUNDEFFECT_MACHINEGUN,%SOUNDEFFECT_MISSILE,%SOUNDEFFECT_MISSILE,%SOUNDEFFECT_MACHINEGUN) _
      + CHR$(%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_MACHINEGUN,%SOUNDEFFECT_MACHINEGUN,%SOUNDEFFECT_MACHINEGUN) _
      + CHR$(%SOUNDEFFECT_MISSILE2,%SOUNDEFFECT_MISSILE2,%SOUNDEFFECT_MISSILE2,%SOUNDEFFECT_CANNON4,%SOUNDEFFECT_CANNON4) _
      + CHR$(%SOUNDEFFECT_CANNON,%SOUNDEFFECT_MACHINEGUN,%SOUNDEFFECT_MACHINEGUN,%SOUNDEFFECT_SILENCE,%SOUNDEFFECT_MISSILE2) _
      + CHR$(%SOUNDEFFECT_CANNON2,%SOUNDEFFECT_CANNON,%SOUNDEFFECT_CANNON3,%SOUNDEFFECT_FLAK2,%SOUNDEFFECT_MISSILE2)

  'Datenstruktur befüllen
  p& = 1
  FOR i& = 0 TO channelsnosave(chnr&).nunitclasses-1
    unit = emptyunit
    indexdescr& = CVI(d$, p&+16)
    indexname& = CVI(d$, p&+18)
    IF indexdescr& < textcount& AND indexname& < textcount& THEN
      IF langcode$ = "GER" THEN
        unit.uname = unittexts(indexname&)
        unit.description = unittexts(indexdescr&)
      ELSE
        unit.uname = UTF8TOCHR$(unittexts(indexname&))
        unit.description = UTF8TOCHR$(unittexts(indexdescr&))
      END IF
    ELSE
      unit.uname = READ$(i&*2+1)
      unit.description = READ$(i&*2+2)
      IF episode& = 5 AND i& = 14 THEN unit.uname = "Golem" : unit.description = "Schwerer Kampfroboter"
      IF episode& = 5 AND i& = 29 THEN unit.uname = "Python" : unit.description = "Raketenpanzer"
    END IF
    unit.flags = CVL(d$, p&+4)
    IF i& = 0 OR i& = 1 OR i& = 2 THEN unit.flags = unit.flags AND NOT %UCF_COOLDOWN  'Korrektur für PLANUM / REGIO / SINUS
    unit.terrain = CVL(d$, p&+8)
    unit.fuel = MIN&(%UA_MAXFUEL, CVI(d$, p&+22))
    unit.range = MIN&(%UA_MAXMOVERANGE, ASC(d$, p&+24))
    IF unit.terrain = 2097152 OR unit.terrain = 6291456 THEN unit.range = 0  'Einheit kann keine Felder betreten (SKULL + RUNE)
    unit.fuelusage = MIN&(%UA_MAXFUELUSAGE, ASC(d$, p&+25))
    unit.costenergy = MIN&(%UA_MAXCOSTENERGY, ASC(d$, p&+26))
    unit.costmaterial = MIN&(%UA_MAXCOSTMATERIAL, ASC(d$, p&+27))
    totaldamage& = 0
    FOR w& = 0 TO 3
      v& = CVI(d$, p&+w&*2+28)
      IF v& > %UA_MAXDAMAGE THEN v& = MIN&(%UA_MAXDAMAGE, INT(v&/10))  'Korrektur für UX-2 + VISION
      totaldamage& = totaldamage&+v&
      unit.weapons(w&).damage = v&
      unit.weapons(w&).maxrange = MAX&(%UA_MINWEAPONRANGE, MIN&(%UA_MAXWEAPONRANGE, ASC(d$, p&+w&+40)))
      unit.weapons(w&).minrange = MAX&(%UA_MINWEAPONRANGE, MIN&(unit.weapons(w&).maxrange, ASC(d$, p&+w&+36)))
      unit.weapons(w&).ammo = MIN&(%UA_MAXAMMO, CVI(d$, p&+w&*2+44))
      v& = CVL(d$, p&+w&*4+52)
      unit.weapons(w&).targets = (INT(v&/16) AND 63)+256*(v& AND 15)
      IF i& = 2 AND unit.weapons(w&).targets = 1024 THEN unit.weapons(w&).targets = %WP_AMMO  'Korrektur für SINUS
      IF i& = 2 AND unit.weapons(w&).targets = 2048 THEN unit.weapons(w&).targets = %WP_FUEL  'Korrektur für SINUS
      IF channels(chnr&).info.difficulty > %DIFFICULTY_EASY AND unit.weapons(w&).targets = %WP_FUEL THEN unit.weapons(w&).ammo = unit.weapons(w&).ammo*3/2
    NEXT w&
    unit.weight = MIN&(%UA_MAXWEIGHT, ASC(d$, p&+68))
    unit.transportvolume = MIN&(%UA_MAXTRANSPORTVOL, ASC(d$, p&+69))
    unit.groupsize = MIN&(%UA_MAXGROUPSIZE, ASC(d$, p&+70))
    IF unit.groupsize = 1 THEN unit.groupsize = MIN&(%UA_MAXGROUPSIZE, ASC(d$, p&+71))
    unit.armor = MIN&(%UA_MAXARMOR, CVI(d$, p&+72))
    unit.transportclass = ASC(d$, p&+74)
    unit.shopmask = ASC(d$, p&+75)
    unit.sightblock = MIN&(%UA_MAXSIGHTBLOCK, ASC(d$, p&+76))
    unit.sight = MIN&(%UA_MAXSIGHT, MAX&(8, ASC(d$, p&+77)))
    unit.transportmask = ASC(d$, p&+80)
    unit.attack = MIN&(%UA_MAXATTACK, CVI(d$, p&+82))
    IF totaldamage& = 0 AND unit.sight >= 40 THEN unit.flags = unit.flags OR %UCF_RECON ELSE unit.flags = unit.flags AND NOT %UCF_RECON
    unit.sfxselection = ASC(sfxsel$, i&+1)
    unit.sfxfire = ASC(sfxfire$, i&+1)
    unit.artworkhandle = GetArtworkHandleByUnitclassName&(unit.uname)
    unit.rating = RateUnit(unit)

    channelsnosave(chnr&).unitclasses(i&) = unit
    p& = p&+88

    'Einheitenklasse von Aldinium ermitteln
    IF (channelsnosave(chnr&).unitclasses(i&).flags AND %UCF_CRYSTAL) <> 0 THEN channelsnosave(chnr&).crystalClass = i&

    'Einheit validieren
    IF unit.armor < 1 THEN
      a$$ = words$$(%ERRMSG_UNITDEF_NO_ARMOR)
      REPLACE "%" WITH unit.uname IN a$$
      CALL PrintError(a$$)
    END IF
    IF unit.groupsize < 1 THEN
      a$$ = words$$(%ERRMSG_UNITDEF_NO_HITPOINTS)
      REPLACE "%" WITH unit.uname IN a$$
      CALL PrintError(a$$)
    END IF
  NEXT i&

  'Status ausgeben
'CALL BILog(FORMAT$(nunitclasses&)+" unit classes found.", 0)
  ReadUnitDefs& = 1

  DATA "Planum 5", "Baufahrzeug Straße/Schienen"
  DATA "Regio", "Baufahrzeug Befestigung"
  DATA "Sinus", "Instandsetzungsfahrzeug"
  DATA "Ranger", "Spähtrupp"
  DATA "Buggy", "Spähfahrzeug"
  '
  DATA "Orion OR-3", "Radarfahrzeug"
  DATA "Demon I31", "Leichter Kampfroboter"
  DATA "Imperator SP", "Fahrende Festung"
  DATA "Technotrax", "Leichter Panzer"
  DATA "Snake", "Schützenpanzer"
  '
  DATA "Sting", "Raketenpanzer"
  DATA "Samurai-2", "Schwerer Panzer"
  DATA "Pulsar A3", "Artillerie"
  DATA "Nashorn", "Sturmgeschütz"
  DATA "Spring 1", "Transportpanzer"
  '
  DATA "Elixir", "Störfahrzeug"
  DATA "Troll I42", "Elite Kampfroboter"
  DATA "Archimedes", "Raketenwerfer"
  DATA "Algol", "Tankfahrzeug"
  DATA "Alcor", "Munitionstransporter"
  '
  DATA "Rune", "Radarstellung"
  DATA "Medusa", "FlaRak-Geschütz"
  DATA "Skull", "Bunker"
  DATA "Ionstar", "Geschützturm"
  DATA "Super Virus", "Mine"
  '
  DATA "Atlas", "Transportfahrzeug"
  DATA "Dolmen Z1", "Transportzug"
  DATA "Anaconda Z2", "Kampfpanzerzug"
  DATA "Excalibur Z3", "Eisenbahngeschütz"
  DATA "Menhir Z4", "Schnellzug"
  '
  DATA "Monolith Z6", "Instandsetzungszug"
  DATA "U.H.U. R-51", "Aufklärer"
  DATA "Ghost FB-3", "Tarnkappenjäger"
  DATA "Drohne", ""
  DATA "Stormbringer", "Höhenbomber"
  '
  DATA "Dragon H1", "Kampfhubschrauber"
  DATA "Genom J1", "Jagdflugzeug"
  DATA "Exterminator", "Jagdbomber"
  DATA "Sperber TB-4", "Wasserflugzeug"
  DATA "Spectrum", "Tankflugflugzeug"
  '
  DATA "Crux", "Transportflugzeug"
  DATA "Guppi H2", "Transporthubschrauber"
  DATA "Rex", "Reparatur & Transportschiff"
  DATA "Möwe SX-1", "Schnellboot"
  DATA "Patrix", "Patrouillenboot"
  '
  DATA "Titan N2", "Flugzeugträger"
  DATA "Shell S3", "Transport U-Boot"
  DATA "Orca U7", "Jagd U-Boot"
  DATA "Polar C-6", "Kreuzer"
  DATA "Zenit MBS-19", "Schlachtschiff"
  '
  DATA "Hydra", "Torpedoboot"
  DATA "Vader D-1", "Landungsboot"
  DATA "Comet FP-42", "FlakPanzer"
  DATA "Aldinium", "Kristall"
  DATA "Vision", "Aufklärungssatellit"
  '
  DATA "Quasar", "Leichte Artillerie"
  DATA "Samurai-3", "Schwerer Panzer"
  DATA "Vader-DF", "Landungsboot"
  DATA "Medusa-2", "FlaRak-Panzer"
  DATA "UX-2", "Mittelstreckenrakete"

  DATA "Axis", "Schlachtpanzer"
  DATA "Citadelle", "Mobiler Bunker"
  DATA "Pluto", "Schlacht U-Boot"
  DATA "Pterano", "Fliegende Festung"
END FUNCTION



'Lädt die Spielernamen für eine BI3 Mission
SUB ReadPlayerNamesBI3(episode&, relativemissionnr&)
  LOCAL f$, filecontent$, a$, plname$
  LOCAL i&, n&, p&, missionnr&, plnr&

  f$ = AdjustFilenameForEpisode$(EXEPATH$+langcode$+"\"+$BI3PLAYERNAMESFILE, episode&, 0)
  filecontent$ = DecodeTPWM$(ReadFileContent(f$, 0))
  IF filecontent$ = "" THEN EXIT SUB

  'Dateiinhalt parsen (TextToArray kann nicht genutzt werden, da viele Zeilen keine Zeilennummer haben)
  n& = PARSECOUNT(filecontent$, CHR$(13,10))
  FOR i& = 1 TO n&
    a$ = TRIM$(PARSE$(filecontent$, CHR$(13,10), i&))
    IF LEFT$(a$, 1) = "#" THEN
      p& = INSTR(a$, "{NUMMER:")
      IF p& > 0 THEN
        missionnr& = VAL(MID$(a$, p&+8))
      ELSE
        p& = INSTR(a$, "{PLAYER:")
        IF p& > 0 AND missionnr& = relativemissionnr& THEN
          'Spielernamen für diese Mission gefunden
          a$ = MID$(a$, p&+8, LEN(a$)-p&-8)
          FOR plnr& = 1 TO %MAXPLAYERS
            plname$ = TRIM$(PARSE$(a$, "|", plnr&))
            IF plname$ <> "" THEN playernames$(plnr&-1) = plname$
          NEXT plnr&
          EXIT SUB
        END IF
      END IF
    END IF
  NEXT i&
END SUB



'Lädt das Video-Mapping für die BI3 Videos
SUB ReadVideoMapping(episode&)
  LOCAL a$, f$, filecontent$
  LOCAL i&, n&, p&, missionnr&, mappingnr&, videoid&, filenr&, maxMissions&

  IF videoMappingCreated& = episode& THEN EXIT SUB

  'Datei einlesen
  f$ = AdjustFilenameForEpisode$(EXEPATH$+$BI3VIDEOMAPPING, episode&, 0)
  filecontent$ = DecodeTPWM$(ReadFileContent(f$, 0))
  IF filecontent$ = "" THEN EXIT SUB

  videoMappingCount& = 0
  missionnr& = -1
  maxMissions& = GetEpisodeStartMap&(episode&+1)-GetEpisodeStartMap&(episode&)
  REDIM missionVideoNumbers(maxMissions&-1), videoMapping&(1, %MAXVIDEOMAPPINGS-1)

  'Dateiinhalt parsen (TextToArray kann nicht genutzt werden, da viele Zeilen keine Zeilennummer haben)
  n& = PARSECOUNT(filecontent$, CHR$(13,10))
  FOR i& = 1 TO n&
    a$ = TRIM$(PARSE$(filecontent$, CHR$(13,10), i&))
    IF LEFT$(a$, 1) = "#" THEN
      p& = INSTR(a$, "{VIDEOSEQUENCE:")
      IF p& > 0 AND videoMappingCount& < %MAXVIDEOMAPPINGS THEN
        'Video-Mapping
        a$ = TRIM$(MID$(a$, p&+15))
        videoid& = VAL(a$)
        filenr& = VAL(MID$(a$, INSTR(a$, "^")+1))
        videoMapping&(0, videoMappingCount&) = videoid&
        videoMapping&(1, videoMappingCount&) = filenr&
        videoMappingCount& = videoMappingCount&+1
      ELSE
        p& = INSTR(a$, "{NUMMER:")
        IF p& > 0 THEN
          'Missions-Videos
          missionnr& = VAL(MID$(a$, p&+8))
        ELSE
          p& = INSTR(a$, "{TERMINATIONSEQS:")
          IF p& > 0 AND missionnr& >=0 AND missionnr& < maxMissions& THEN
            a$ = TRIM$(MID$(a$, p&+17))
            missionVideoNumbers(missionnr&).cutscene = VAL(PARSE$(a$, " ", 1))
            missionVideoNumbers(missionnr&).briefing = VAL(PARSE$(a$, " ", 2))
            missionVideoNumbers(missionnr&).victory = VAL(PARSE$(a$, " ", 3))
            missionVideoNumbers(missionnr&).defeat = VAL(PARSE$(a$, " ", 4))
          END IF
        END IF
      END IF
    END IF
  NEXT i&

  videoMappingCreated& = episode&
END SUB



'Datei-ID zu Video-ID ermitteln
FUNCTION GetFileForVideoMessageId&(videoid&)
  LOCAL i&

  IF videoid& >= 10000 AND videoid& < 11000 THEN
    GetFileForVideoMessageId& = videoid&-10000
    EXIT FUNCTION
  END IF

  FOR i& = 0 TO videoMappingCount&-1
    IF videoMapping&(0, i&) = videoid& THEN
      GetFileForVideoMessageId& = videoMapping&(1, i&)
      EXIT FUNCTION
    END IF
  NEXT i&

  GetFileForVideoMessageId& = -1
END FUNCTION



'Lädt und initialisiert eine Zwischensequenz
FUNCTION InitCutScene&(scenenr&, msgid&, musictrack&)
  LOCAL a$$, voicenr&, custvoice&, p&

  CALL BIDebugLog("Initializing cutscene "+FORMAT$(scenenr&)+".")
  IF D2DGraphicBitmapLoad&("GFX\cutscene"+FORMAT$(scenenr&)+".png", hCutScene&) = 0 THEN EXIT FUNCTION
  IF hCutSceneElements& = -1 AND D2DGraphicBitmapLoad&("GFX\cutscene0.png", hCutSceneElements&) = 0 THEN EXIT FUNCTION

  CALL ShowControls(0)
  gameState& = %GAMESTATE_CUTSCENE
  cutSceneStartTime! = gametime!
  cutSceneTextEndTime! = 0
  lastCutSceneObjCreationTime! = 0
  currentTextId& = msgid&
  cutSceneNumber& = scenenr&
  cutSceneTextSkip& = 0
  cutsceneCurrentTextLine$$ = ""
  nCutSceneObjects& = 0
  isVideoCutscene& = 0

  'Musik starten
  CALL StartMusic(musictrack&)

  'Nachricht sprechen
  a$$ = GetGameMessageText(currentTextId&, 0, 0)
  p& = INSTR(-1, a$$, CHR$(0))
  IF p& > 0 AND (INSTR(UCASE$(MID$(a$$, p&+1)), "CHAPTER") > 0 OR INSTR(UCASE$(MID$(a$$, p&+1)), "KAPITEL") > 0) THEN
    'Titel aus der letzten Zeile an den Anfang stellen
    a$$ = MID$(a$$, p&+1)+CHR$(0)+LEFT$(a$$, p&-1)
  END IF
  a$$ = TRIM$(ReplaceNonPrintable(ReplaceMessagePlaceholders(a$$), 1))
  voicenr& = 1
  'römische Kapitelnummer ersetzen
  REPLACE " IV." WITH " 4" IN a$$
  REPLACE " III." WITH " 3" IN a$$
  REPLACE " II." WITH " 2" IN a$$
  REPLACE " I." WITH " 1" IN a$$
  IF voices$(voicenr&, languageNr&) <> "" AND speechVolume& > 0 THEN
    IF UCASE$(LEFT$(a$$, 4)) = "^VOC" THEN
      custvoice& = VAL(MID$(a$$, 5, 1))
      a$$ = MID$(a$$, 6)
      IF custvoice& < %MAXVOICES AND voices$(custvoice&, languageNr&) <> "" THEN voicenr& = custvoice&
    END IF
    CALL SAPISpeak(SAPISetVoice(SAPISetVolume(speechVolume&, SAPISetRate(speechRate&, SAPISilence(1000)+a$$)), 0, 0, voices$(voicenr&, languageNr&)))
  END IF

  InitCutScene& = 1
END FUNCTION



'Battle Isle 3 Zwischensequenz initialisieren
SUB InitVideoCutScene(msgid&)
  LOCAL fileid&, hFile&, hVideoStream&, hAudioStream&
  LOCAL audioFormat&, channelCount&, bitsPerSample&, streamLengthInBytes&
  LOCAL a$$, f$

  fileid& = GetFileForVideoMessageId&(msgid&)
  IF fileid& < 0 THEN EXIT SUB
  f$ = aviFolder$+"API"+FORMAT$(fileid&, "000")+".avi"
  CALL BIDebugLog("Initializing video cutscene "+FORMAT$(msgid&)+" from "+f$)
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

  CALL ShowControls(0)
  gameState& = %GAMESTATE_CUTSCENE
  cutSceneStartTime! = gametime!
  isVideoCutscene& = 1
  currentVideoFrame& = -1
  pAfxMp3.Stop  'Musik abschalten
END SUB



'Intro initialisieren
SUB InitIntro
  LOCAL i&, centerX!, centerY!, rotation!
  DIM introClouds(%MAXINTROCLOUDS-1)

  'Wolken erzeugen
  centerX! = (maparea.left+maparea.right)/2
  centerY! = (maparea.top+maparea.bottom)/2
  FOR i& = 0 TO %MAXINTROCLOUDS-1
    rotation! = RND*2*3.1415926535
    introClouds(i&).xpos = centerX!+COS(rotation!)*IIF(i& < 32, 640, 64)*uiscale!
    introClouds(i&).ypos = centerY!+SIN(rotation!)*IIF(i& < 32, 300, 30)*uiscale!
    introClouds(i&).size = RND(650, 1000)
    introClouds(i&).rotation = rotation!
    introClouds(i&).speed = IIF(i& < 32, 25, 22+i&/10)  'Wolken 0-31: 25, dann 25, 25, 25 ... 34
    introClouds(i&).opacity = IIF(i& < 32, 1, -i&/20)  'Wolken 0-31: 1, dann -1.6, -1.65, -1.7 ... -6.35
    introClouds(i&).color = IIF(i& < 32, 15, MAX&(0, 19-INT(i&/6)))  'Wolken 0-31: 15, dann 14, 14, 14, 14, 13, 13 ... 0
  NEXT i&
END SUB



'Sortiert die Aktionen nach Runde/Bewegung
FUNCTION SortActions$(chnr&, actions$)
  LOCAL i&, n&, category&, actiontype&, turnnr&, movenr&, nr&, nvictorycond&, ndelete&, unitnr&, episode&, missionnr&, actionlevel&, b$
  LOCAL t&(), a&()
  LOCAL specialVictory AS TAction, missionbriefing AS TAction

  n& = LEN(actions$)/40
  channels(chnr&).info.turnlimit = -1

  'Siegbedingung für Zugang zur Super-Geheim-Mission hinzufügen
  IF channels(chnr&).info.checksum = %ENTRANCETOSUPERSECRET THEN
    specialVictory.category = %ACCAT_VICTORY
    specialVictory.actiontype = %ACTYPE_BONUS_SUPERSECRET
    actions$ = actions$+PEEK$(VARPTR(specialVictory), SIZEOF(specialVictory))
    channels(chnr&).info.bonusmission = %SUPERSECRETMISSION
  END IF

  'Missionbriefing für Battle Isle 3 Mission hinzufügen
  episode& = channels(chnr&).campaign.episode
  IF episode& = 5 THEN
    'ggf. Meldung in Runde 0 Bewegung 0 verschieben
    FOR i& = 0 TO n&-1
      category& = ASC(actions$, i&*40+1)
      turnnr& = CVI(actions$, i&*40+3)
      movenr& = CVI(actions$, i&*40+5)
      actiontype& = ASC(actions$, i&*40+8)
      IF category& = %ACCAT_NORMAL AND actiontype& = %ACTYPE_MESSAGE AND turnnr& = 0 AND movenr& = 0 THEN ASC(actions$, i&*40+5) = 1
    NEXT i&

    'Missionbriefing hinzufügen
    missionnr& = channels(chnr&).info.currentmission-GetEpisodeStartMap&(episode&)
    IF missionnr& >= 0 AND missionnr& <= UBOUND(missionVideoNumbers) AND missionVideoNumbers(missionnr&).briefing > 0 THEN
      missionbriefing.category = %ACCAT_NORMAL
      missionbriefing.actiontype = %ACTYPE_MESSAGE
      missionbriefing.player = channels(chnr&).info.originalplayers AND NOT channels(chnr&).info.aimask
      missionbriefing.actionparam = missionVideoNumbers(missionnr&).briefing
      actions$ = actions$+PEEK$(VARPTR(missionbriefing), SIZEOF(missionbriefing))
    END IF
  END IF

  'Zeitpunkt jeder Aktion ermitteln
  n& = LEN(actions$)/40
  IF n& = 0 THEN
    channels(chnr&).info.nvictoryconditions = 0
    channels(chnr&).info.actionposition = 0
    SortActions$ = actions$
    EXIT FUNCTION
  END IF
  DIM t&(n&-1), a&(n&-1)
  FOR i& = 0 TO n&-1
    category& = ASC(actions$, i&*40+1)
    actionlevel& = GetActionLevel&(category&)
    IF actionlevel& > 0 THEN
      t&(i&) = 777888999  'alle Aktionen aus noch inaktiven Aktionsstufen gant nach hinten schieben
    ELSE
      turnnr& = CVI(actions$, i&*40+3)
      movenr& = CVI(actions$, i&*40+5)
      actiontype& = ASC(actions$, i&*40+8)
      t&(i&) = turnnr&*1000+movenr&
      IF category& = %ACCAT_NORMAL THEN
        t&(i&) = t&(i&)+1000000   'alle normalen Aktionen hinter die Siegbedingungen
      ELSE
        IF actiontype& = %ACTYPE_VC_TURNREACHED THEN
          channels(chnr&).info.turnlimit = CVI(actions$, i&*40+13)
          channels(chnr&).info.turnlimitisvictory = IIF&(category& = %ACCAT_VICTORY, 1, 0)
          t&(i&) = 1000000000
          ndelete& = ndelete&+1
        ELSE
          nvictorycond& = nvictorycond&+1
        END IF
      END IF
    END IF
    a&(i&) = i&
  NEXT i&

  'Aktionen sortieren
  ARRAY SORT t&(0) FOR n&, TAGARRAY a&()

  'Aktionen sortiert zurückschreiben
  FOR i& = 0 TO n&-1-ndelete&
    nr& = a&(i&)
    b$ = b$+MID$(actions$, nr&*40+1, 40)
  NEXT i&
  channels(chnr&).info.nvictoryconditions = nvictorycond&
  channels(chnr&).info.actionposition = nvictorycond&

  SortActions$ = b$
END FUNCTION



'MAC Adresse der Netzwerkkarte ermitteln
FUNCTION GetMacAddress$
  LOCAL r&, sz&, p&, a$, nm$
  LOCAL adapter AS IP_ADAPTER_INFO
  LOCAL buffer() AS IP_ADAPTER_INFO
  DIM buffer(15)

  'Adapter-Informationen für bis zu 16 Adapter lesen
  sz& = SIZEOF(IP_ADAPTER_INFO)*16
  r& = GetAdaptersInfo(buffer(0), sz)
  p& = VARPTR(buffer(0))

  'ersten Adapter mit physikalischer Adresse suchen
  DO
    POKE$ VARPTR(adapter), PEEK$(p&, SIZEOF(IP_ADAPTER_INFO))
    a$ = LEFT$(adapter.Address, 6)
    nm$ = adapter.Description
    IF a$ <> STRING$(6, 0) OR adapter.pNext = 0 THEN EXIT LOOP
    p& = adapter.pNext
  LOOP

  GetMacAddress$ = a$
END FUNCTION



'Missions-Codes laden
FUNCTION LoadMissionnames&
  LOCAL a$, n&

  IF UBOUND(mapnames$()) > 0 THEN EXIT FUNCTION

  'Datei einlesen
  a$ = ReadFileContent("MIS\MAPCODES.TXT", 0)
  IF a$ = "" THEN EXIT FUNCTION
  a$ = DecodeTPWM$(a$)

  'Einträge extrahieren
  n& = TextToArray&(a$, mapnames$())

  'Code für Super-Secret generieren
  REDIM PRESERVE mapnames$(%MAXMISSIONS-1)
  mapnames$(%SUPERSECRETMISSION) = CreateSuperSecretMissionCode$

  LoadMissionnames& = 1
END FUNCTION



'Unterteilt einen Text in Zeilen
SUB MissionTextToRows(BYVAL a$$, maxwidth&, textfont&, idx&)
  LOCAL i&, n&, textrows$$()

  n& = MIN&(20, TextToRows&(a$$, maxwidth&, textfont&, textrows$$()))
  FOR i& = 0 TO n&-1
    missionTextRows$$(i&, idx&) = textrows$$(i&)
  NEXT i&
END SUB



'Missionsbeschreibung und Missionsziel in Zeilen passend zur Karteninfo-Breite unterteilen
SUB PreRenderMissionText(chnr&)
  LOCAL i&, msgid&, plnr&, shopnr&, unittp&, a$$, b$$
  REDIM missionTextRows$$(19, 1)

  IF gameMode& = %GAMEMODE_SERVER THEN EXIT SUB

  'Missionsbeschreibung
  CALL MissionTextToRows(channels(chnr&).info.mapdescription, 590, hGameMessageFont&, 0)

  'Missionsziel
  IF gameMessages$$(%MSG_CUSTOM_BRIEFING) <> "" THEN
    'benutzerdefinierte Missionsbeschreibung
    msgid& = %MSG_CUSTOM_BRIEFING
    a$$ = gameMessages$$(msgid&)
  ELSE
    msgid& = channels(chnr&).info.missionbriefing
    IF msgid& > 0 THEN
      'standard Missionsbeschreibung
      msgid& = GetTextIdForMessage&(msgid&)
      a$$ = GetGameMessageText(msgid&, 0, 0)
    ELSE
      IF channels(chnr&).info.nvictoryconditions > 0 THEN
        'keine Missionsbeschreibung vorhanden -> Missionsziele prüfen
        FOR i& = 0 TO channels(chnr&).info.nvictoryconditions-1
          IF channels(chnr&).actions(i&).category = %ACCAT_VICTORY THEN
            SELECT CASE channels(chnr&).actions(i&).actiontype
            CASE %ACTYPE_VC_PLAYERDEFEATED:
              b$$ = words$$(%WORD_MISSIONOBJECTIVE_EXTINCTPLAYER)
              plnr& = channels(chnr&).actions(i&).actionparam
              REPLACE "%" WITH IIF$(playernames$(plnr&) <> "", playernames$(plnr&), defaultPlayernames$(plnr&)) IN b$$
              a$$ = a$$+b$$+" "
            CASE %ACTYPE_VC_SHOPOCCUPIED:
              IF channels(chnr&).actions(i&).actionparam = localPlayerNr& THEN
                b$$ = words$$(%WORD_MISSIONOBJECTIVE_OCCUPY)
                shopnr& = channels(chnr&).actions(i&).shop
                REPLACE "%" WITH channels(chnr&).info.shopnames(shopnr&) IN b$$
                a$$ = a$$+b$$+" "
              END IF
            CASE %ACTYPE_VC_TURNREACHED:
              b$$ = words$$(%WORD_MISSIONOBJECTIVE_SURVIVE)
              REPLACE "%" WITH FORMAT$(channels(chnr&).actions(i&).actionparam) IN b$$
              a$$ = a$$+b$$+" "
            CASE %ACTYPE_VC_UNITDEAD_PLAYER1 TO %ACTYPE_VC_UNITDEAD_PLAYER6
              b$$ = words$$(%WORD_MISSIONOBJECTIVE_KILLUNIT)
              plnr& = channels(chnr&).actions(i&).shop
              REPLACE "%" WITH IIF$(playernames$(plnr&) <> "", playernames$(plnr&), defaultPlayernames$(plnr&)) IN b$$
              unittp& = channels(chnr&).actions(i&).actionparam
              REPLACE "&" WITH channelsnosave(chnr&).unitclasses(unittp&).uname IN b$$
              a$$ = a$$+b$$+" "
            END SELECT
          END IF
        NEXT i&
        a$$ = TRIM$(a$$)
      ELSE
        'falls keine Missionsziele angegeben, dann gegnerisches HQ erobern
        a$$ = FormatGameMessage(words$$(%WORD_DEFAULT_MISSIONBRIEFING))
      END IF
    END IF
  END IF
  'Steuerzeichen und Platzhalter entfernen oder ersetzen
  a$$ = REMOVE$(ReplaceMessagePlaceholders(a$$), ANY CHR$(0, 1, 2))
  IF UCASE$(LEFT$(a$$, 4)) = "^VOC" THEN a$$ = MID$(a$$, 6)
  CALL MissionTextToRows(a$$, 720, hGameMessageFont&, 1)
END SUB



'Berechnet eine Kontrollsumme über einen Channel
FUNCTION DebugCalculateChannelChecksum$(chnr&)
  LOCAL a$, u$, i&
  LOCAL chinfo AS TChannelInfo

  'server-spezifische Felder ausblenden
  chinfo = channels(chnr&).info
  chinfo.netbufferaccess = 0
  chinfo.hAIThread = 0
'  chinfo.hCommunicationThread = 0

  'AI-spezifische Felder ausblenden
  FOR i& = 0 TO channels(chnr&).info.nunits-1
    u$ = u$+PEEK$(VARPTR(channels(chnr&).units(i&)), SIZEOF(TUnit)-10)
  NEXT i&

  a$ = MKL$(CalculateChecksum&(PEEK$(VARPTR(chinfo), SIZEOF(chinfo)-%MAXSHOPS*32))) _
    + MKL$(CalculateChecksum&(u$)) _
    + MKL$(CalculateChecksum&(PEEK$(VARPTR(channels(chnr&).shops(0)), SIZEOF(TShop)*channels(chnr&).info.nshops))) _
    + MKL$(CalculateChecksum&(PEEK$(VARPTR(channels(chnr&).player(0)), SIZEOF(TPlayer)*%MAXPLAYERS))) _
    + MKL$(channels(chnr&).info.nunits)

  DebugCalculateChannelChecksum$ = a$
END FUNCTION



'Speichert Channel-Daten in einer Datei
SUB DebugSaveChannelToFile(chnr&)
  LOCAL chinfo$, unitdata$

  chinfo$ = PEEK$(VARPTR(channels(chnr&).info), SIZEOF(TChannelInfo)-(%MAXSHOPS-channels(chnr&).info.nshops)*32)
  unitdata$ = PEEK$(VARPTR(channels(chnr&).units(0)), SIZEOF(TUnit)*channels(chnr&).info.nunits)
  CALL WriteFileContent("test\debug-info-"+IIF$(gameMode& = %GAMEMODE_SERVER, "server"+FORMAT$(chnr&), "client")+".dbg", chinfo$)
  CALL WriteFileContent("test\debug-unit-"+IIF$(gameMode& = %GAMEMODE_SERVER, "server"+FORMAT$(chnr&), "client")+".dbg", unitdata$)
END SUB



'Gibt ein Animationsskript an der Konsole aus
SUB DebugPrintAnimation(a$)
  LOCAL i&, nsteps&, t$
  LOCAL ani AS TAnimationScript

  nsteps& = LEN(a$)/SIZEOF(TAnimationScript)
  FOR i& = 0 TO nsteps&-1
    POKE$ VARPTR(ani), MID$(a$, i&*SIZEOF(TAnimationScript)+1, SIZEOF(TAnimationScript))
    SELECT CASE ani.animtype
    CASE %MSGANI_VID: t$ = "Video"
    CASE %MSGANI_SFX: t$ = "Sound"
    CASE %MSGANI_TXT: t$ = "Text "
    END SELECT
    PRINT t$; ani.sequence; ani.starttime; ani.duration; ani.frame
  NEXT i&
  PRINT
END SUB



'Berechnet eine Kontrollsumme über einen String
FUNCTION CalculateChecksum&(a$)
  #REGISTER NONE

  LOCAL p&, i&, n&

  p& = STRPTR(a$)
  n& = LEN(a$)
  IF n& < 4 THEN EXIT FUNCTION

  'Register sichern
  ! PUSH EAX
  ! PUSH EBX
  ! PUSH ECX
  ! PUSH EDX
  ! PUSH ESI

  'Zeiger auf Quellstring laden
  ! MOV ESI, p&
  ! MOV ECX, n&
  ! SHR ECX, 2
  ! XOR EBX, EBX
  ! XOR EDX, EDX
  'Kontrollsumme berechnen
  CCSum_Loop:
  ! LODSD
  ! XOR EBX, EAX
  ! ROL EDX, 1
  ! ADD EDX, EAX
  ! LOOP CCSum_Loop
  ! XOR EBX, EDX
  ! MOV n&, EBX

  'Register wieder herstellen
  ! POP ESI
  ! POP EDX
  ! POP ECX
  ! POP EBX
  ! POP EAX

  CalculateChecksum& = n&
END FUNCTION



'Korrekturen in ULUARGE durchführen (DF00 aktivieren)
SUB FixULUARGE(chnr&)
  LOCAL actions$
  LOCAL a AS TAction

  a.category = %ACCAT_NORMAL
  a.turn = 10
  a.movement = 0
  a.player = 0
  a.actiontype = %ACTYPE_DFLAYER
  a.actionparam = 0

  actions$ = PEEK$(VARPTR(channels(chnr&).actions(0)), channels(chnr&).info.nactions*SIZEOF(TAction))+PEEK$(VARPTR(a), SIZEOF(a))
  actions$ = SortActions$(chnr&, actions$)
  channels(chnr&).info.nactions = LEN(actions$)/40
  POKE$ VARPTR(channels(chnr&).actions(0)), actions$
END SUB



'Korrekturen in GEKEFZU durchführen (Baufahrzeug zur globalen Produktionspalette hinzufügen)
SUB FixGEKEFZU(chnr&)
  channels(chnr&).info.playergpm = channels(chnr&).info.playergpm OR 1
END SUB



'Korrekturen in HOLLEKO durchführen (KI-Einheiten auf der Zielinsel unbeweglich machen)
SUB FixHOLLEKO(chnr&)
  LOCAL unitnr&

  FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
    IF channels(chnr&).units(unitnr&).owner = 1 AND channels(chnr&).units(unitnr&).ypos >= 52 THEN channels(chnr&).units(unitnr&).aimissioncommand = %AIMC_NOMOVE
  NEXT unitnr&
END SUB



'Korrekturen in finaler BI3 Mission durchführen (Allianzänderung)
SUB FixBI3FINAL(chnr&)
  LOCAL actions$
  LOCAL a AS TAction

  a.category = %ACCAT_NORMAL
  a.turn = 24-channels(chnr&).info.difficulty*4
  a.movement = 0
  a.player = 2^5
  a.actiontype = %ACTYPE_ALLIANCE
  a.newally = 2+8+16

  actions$ = PEEK$(VARPTR(channels(chnr&).actions(0)), channels(chnr&).info.nactions*SIZEOF(TAction))+PEEK$(VARPTR(a), SIZEOF(a))
  actions$ = SortActions$(chnr&, actions$)
  channels(chnr&).info.nactions = LEN(actions$)/40
  POKE$ VARPTR(channels(chnr&).actions(0)), actions$
END SUB



'Ermittelt den Namen der Shopdatei zu einer Missionsdatei
FUNCTION GetShopFilename$(f$)
  LOCAL shopfile$, p&

  shopfile$ = f$
  p& = INSTR(-1, shopfile$, ".")
  IF p& > 0 THEN shopfile$ = LEFT$(shopfile$, p&-1)
  shopfile$ = shopfile$+".TXT"
  REPLACE "MIS\" WITH langcode$+"\" IN shopfile$

  GetShopFilename$ = shopfile$
END FUNCTION



'Shopnamen laden
FUNCTION LoadShopnames&(chnr&, f$)
  LOCAL a$, b$, p&, q&, l&, nr&, s$

  'Datei einlesen
  IF f$ = "" THEN
    a$ = orgShopNames$
  ELSE
    IF f$ = "GER\MISS999.TXT" THEN
      a$ = RESOURCE$(RCDATA, 101)
    ELSE
      IF f$ = "ENG\MISS999.TXT" THEN
        a$ = RESOURCE$(RCDATA, 102)
      ELSE
        a$ = ReadFileContent(f$, 0)
      END IF
    END IF
    IF a$ = "" THEN EXIT FUNCTION
    a$ = DecodeTPWM$(a$)

    'Zeilenumbrüche formatieren
    REPLACE CHR$(10) WITH CHR$(13) IN a$

    'Umlaute ersetzen
    REPLACE CHR$(142) WITH "Ä" IN a$
    REPLACE CHR$(153) WITH "Ö" IN a$
    REPLACE CHR$(154) WITH "Ü" IN a$
  END IF
  orgShopNames$ = a$

  'Zeilen verarbeiten
  l& = LEN(a$)
  p& = 1
  WHILE p& <= l&
    q& = INSTR(p&, a$, CHR$(13))
    IF q& = 0 THEN q& = l&+1
    b$ = RTRIM$(MID$(a$, p&, q&-p&))
    IF LEFT$(b$, 1) = "#" AND RIGHT$(b$, 1) = "}" THEN
      'gültige Zeile gefunden
      nr& = VAL(MID$(b$, 2, 4))
      p& = INSTR(b$, "{")
      IF p& > 0 THEN
        s$ = MID$(b$, p&+1, LEN(b$)-p&-1)
        SELECT CASE nr&
        CASE 0: channels(chnr&).info.mapdescription = s$
        CASE 1: channels(chnr&).info.mapshortdescr = s$
        CASE ELSE: IF nr& >= 2 AND nr& < channels(chnr&).info.nshops+2 THEN channels(chnr&).info.shopnames(nr&-2) = s$
        END SELECT
      END IF
    END IF
    p& = q&+1
  WEND

  LoadShopnames& = 1
END FUNCTION



'Lädt eine Mission
FUNCTION LoadMission&(f$, episode&, difficulty&, chnr&)
  LOCAL a$, missdata$, g$, shopfile$, chname$
  LOCAL pMiss&, pMap&, pShop&, pActn&, pDF&, pCustMsg&
  LOCAL mapx&, mapy&, x&, y&, i&, j&, k&, p&, v&, dfnr&, dflen&, missNr&, transporter&, missionnr&, custmsglen&, yMapFactor&
  LOCAL dfLayerCount&, dfindex&, dfdata$
  LOCAL owner&, nshops&, plmask&, nplayers&, nteams&, teammask&, allymatrix$, actions$, germsg$, engmsg$, usermsg$, msgVer$, custMsgUserLang$, wmsg$$
  LOCAL campaign AS TCampaign
  LOCAL playergpm&&, aigpm&&
  LOCAL menutime!

  'Menü während des Ladens einfrieren, damit dieses anschließend korrekt animiert wird
  IF menuOpenTime! > 0 THEN menutime! = menutime! = gametime!-menuOpenTime!

  'Datei einlesen
  IF f$ = "" THEN
    IF orgMissionData$ = "" THEN EXIT FUNCTION
    a$ = orgMissionData$
  ELSE
    IF f$ = "MIS\MISS999.DAT" THEN
      a$ = RESOURCE$(RCDATA, 100)
    ELSE
      a$ = ReadFileContent(f$, 0)
      IF a$ = "" THEN
        LoadMission& = -1
        EXIT FUNCTION
      END IF
    END IF
  END IF

  'Header auswerten
  IF LEFT$(a$, 4) <> "MSSN" THEN EXIT FUNCTION
  pMiss& = CVL(a$, 5)
  pMap& = CVL(a$, 9)
  pShop& = CVL(a$, 13)
  pActn& = CVL(a$, 17)
  dfLayerCount& = MIN&(CVI(a$, 21), %MAXDFLAYER-1)
  pDF& = CVL(a$, 23)-4
  pCustMsg& = CVL(a$, 147)
  IF MID$(a$, pActn&+1, 4) <> "ACTN" OR MID$(a$, pShop&+1, 4) <> "SHOP" OR MID$(a$, pMiss&+1, 4) <> "MISS" OR MID$(a$, pMap&+1, 3) <> "MAP" THEN EXIT FUNCTION
  orgMissionData$ = a$

  'Channel zurücksetzen
  campaign = channels(chnr&).campaign
  chname$ = channels(chnr&).info.cname
  POKE$ VARPTR(channels(chnr&)), STRING$(SIZEOF(TChannel), 0)
  channels(chnr&).campaign = campaign
  channels(chnr&).info.cname = chname$

  'Channel-Info initialisieren
  channels(chnr&).info.state = IIF&(gameMode& <> %GAMEMODE_SERVER, %CHANNELSTATE_INGAME, %CHANNELSTATE_EMPTY)
  lastCampaignTimeUpdate! = gametime!
  g$ = UCASE$(PATHNAME$(NAME, f$))
  IF LEFT$(g$, 4) = "MISS" THEN missionnr& = VAL(MID$(g$, 5))
  channels(chnr&).campaign.episode = episode&
  channels(chnr&).info.currentmission = missionnr&
  channels(chnr&).info.turn = 0
  channels(chnr&).info.movement = 0
  channels(chnr&).info.nunits = 0
  channels(chnr&).info.nshops = 0
  channels(chnr&).info.startenergy = 0
  channels(chnr&).info.checksum = CalculateChecksum&(a$)
  channels(chnr&).info.difficulty = difficulty&

  CALL SetColorSchema(channels(chnr&).campaign.episode)
  CALL ReadUnitDefs&(chnr&)
  CALL ReadTerrainDefs&(chnr&)
  IF episode& = 5 THEN CALL ReadVideoMapping(episode&)

  'Missionsparameter auswerten
  missdata$ = MID$(a$, pMiss&+5, pActn&-pMiss&-4)
  allymatrix$ = MID$(missdata$, 17, 6)
  channels(chnr&).info.nextmission = ASC(missdata$, 7)
  IF episode& = 5 AND channels(chnr&).info.nextmission > 0 THEN channels(chnr&).info.nextmission = channels(chnr&).info.nextmission-1  'bei BI3 ist dieses Feld 1-basiert statt 0-basiert
  IF ASC(missdata$, 8) = 39 THEN channels(chnr&).info.nextmission = -1
  channels(chnr&).info.weather = ASC(missdata$, 9)
  channels(chnr&).info.aimask = ASC(missdata$, 11)
  channels(chnr&).info.requiredvictorycond = ASC(missdata$, 12)
  channels(chnr&).info.missionbriefing = ASC(missdata$, 294)+IsEDTMission&(missionnr&)*256
  channels(chnr&).info.bonusmission = ASC(missdata$, 297)

  'benutzerdefinierte Spielernamen
  IF episode& = 5 THEN CALL ReadPlayerNamesBI3(episode&, missionnr&-GetEpisodeStartMap&(episode&))
  IF ASC(missdata$, 23) > 32 THEN
    FOR i& = 0 TO %MAXPLAYERS-1
      playernames$(i&) = RTRIM$(MID$(missdata$, 23+i&*16, 16))
    NEXT i&
  END IF

  'allgemeine Produktionspalette
  g$ = MID$(missdata$, 153, 128)
  FOR i& = 0 TO 62
    v& = ASC(missdata$, 153+i&*2)
    IF (v& AND 1) = 1 THEN playergpm&& = playergpm&& OR 2^i&
    IF (v& AND 2) = 2 THEN aigpm&& = aigpm&& OR 2^i&
  NEXT i&
  channels(chnr&).info.playergpm = playergpm&&
  channels(chnr&).info.aigpm = aigpm&&

  'Karte extrahieren
  mapx& = CVI(a$, pActn&+5)
  mapy& = CVI(a$, pActn&+7)
  channels(chnr&).info.xsize = mapx&
  channels(chnr&).info.ysize = mapy&
  yMapFactor& = IIF&(mapx& > 64 OR mapy& > 64, 256, 64)
  p& = pActn&+9
  'Terrain
  FOR j& = 0 TO mapy&-1
    FOR i& = 0 TO mapx&-1
      v& = CVI(a$, p&)
      channels(chnr&).zone1(i&, j&) = v&
      p& = p&+2
    NEXT i&
  NEXT j&
  'Overlays
  FOR j& = 0 TO mapy&-1
    FOR i& = 0 TO mapx&-1
      v& = CVI(a$, p&)
      channels(chnr&).zone2(i&, j&) = v&
      p& = p&+2
    NEXT i&
  NEXT j&
  'Einheiten
  FOR j& = 0 TO mapy&-1
    FOR i& = 0 TO mapx&-1
      v& = CVI(a$, p&)
      IF v& >= 0 THEN v& = CreateUnitFromCode&(chnr&, i&, j&, v&)
      channels(chnr&).zone3(i&, j&) = v&
      p& = p&+2
    NEXT i&
  NEXT j&

  'Shop extrahieren
  nshops& = CVI(a$, pShop&+5)
  IF nshops& > %MAXSHOPS THEN EXIT FUNCTION
  channels(chnr&).info.nshops = nshops&
  FOR i& = 0 TO nshops&-1
    POKE$ VARPTR(channels(chnr&).shops(i&)), MID$(a$, pShop&+7+i&*SIZEOF(TShop), SIZEOF(TShop))
    'Daten konvertieren
    x& = channels(chnr&).shops(i&).position AND (yMapFactor&-1)
    y& = INT(channels(chnr&).shops(i&).position/yMapFactor&)
    channels(chnr&).shops(i&).position = x&                                           'X-Position
    channels(chnr&).shops(i&).position2 = y&                                          'Y-Position
    channels(chnr&).shops(i&).owner = MIN&(6, LOG2(channels(chnr&).shops(i&).owner))  'Besitzer
    v& = INSTR($BUILDINGSPRITES, MKI$(channels(chnr&).zone2(x&, y&)))
    IF v& = 0 THEN v& = -1 ELSE v& = INT((v&-1)/2)
    channels(chnr&).shops(i&).sprite = v&
    IF channels(chnr&).shops(i&).unittype = 1 THEN  'AI-Points ignorieren
      IF channels(chnr&).shops(i&).shoptype > 0 THEN
        'Einheiten im Shop erzeugen
        channels(chnr&).zone3(x&, y&) = -2-i&
        FOR j& = 0 TO 15
          v& = channels(chnr&).shops(i&).content(j&)
          IF v& >= 0 THEN
            v& = CreateUnit&(chnr&, x&, y&, v&, channels(chnr&).shops(i&).owner, 0)
            channels(chnr&).shops(i&).content(j&) = v&
          END IF
        NEXT j&
      ELSE
        'Einheiten im Transporter erzeugen
        transporter& = channels(chnr&).zone3(x&, y&)
        IF transporter& >= 0 THEN
          FOR j& = 0 TO 7
            v& = channels(chnr&).shops(i&).content(j&)
            IF v& >= 0 THEN
              v& = CreateUnit&(chnr&, x&, y&, v&, channels(chnr&).shops(i&).owner, 0)
              channels(chnr&).units(transporter&).transportcontent(j&) = v&
            END IF
          NEXT j&
        END IF
      END IF
    ELSE
      'AI-Point in Einheit integrieren
      v& = channels(chnr&).zone3(x&, y&)
      IF v& >= 0 THEN
        channels(chnr&).units(v&).aimissioncommand = channels(chnr&).shops(i&).aicommand
      END IF
    END IF
  NEXT i&
  IF f$ <> "" THEN shopfile$ = GetShopFilename$(f$)
  i& = LoadShopnames&(chnr&, shopfile$)

  'Aktionen extrahieren
  actions$ = MID$(a$, pMap&+5, MIN&(pDF&-pMap&-4, SIZEOF(TAction)*%MAXACTIONS))
  actions$ = SortActions$(chnr&, actions$)
  channels(chnr&).info.nactions = LEN(actions$)/40
  POKE$ VARPTR(channels(chnr&).actions(0)), actions$

  'DF Layer extrahieren
  POKE$ VARPTR(channels(chnr&).dfcount(0)), STRING$(%MAXDFLAYER*2, 0)
  dfindex& = 0
  FOR dfnr& = 0 TO dfLayerCount&-1
    pDF& = CVL(a$, 23+dfnr&*4)-4
    dflen& = CVL(a$, 151+dfnr&*4)
    IF pDF& > 0 AND dflen& > 0 THEN
      dfdata$ = MID$(a$, pDF&+1, dflen&)
      'Layer-Header validieren
      IF LEFT$(dfdata$, 2) <> "DF" OR VERIFY(MID$(dfdata$, 3, 2), "0123456789") > 0 THEN EXIT FOR
'      IF MID$(dfdata$, 5) = NODFLAYER$ THEN ITERATE FOR
      'Kartenänderungen extrahieren
      channels(chnr&).dfoffsets(dfnr&) = dfindex&
      k& = 0
      p& = 5
      WHILE p& <= dflen&
        v& = CVWRD(dfdata$, p&)  'Position
        i& = v& AND (yMapFactor&-1)
        j& = INT(v&/yMapFactor&)
        IF i& >= 0 AND i& < mapx& AND j& >= 0 AND j& < mapy& THEN
          channels(chnr&).dflayer(dfindex&).xpos = i&
          channels(chnr&).dflayer(dfindex&).ypos = j&
          channels(chnr&).dflayer(dfindex&).terrain = CVI(dfdata$, p&+2)
          channels(chnr&).dflayer(dfindex&).overlay = CVI(dfdata$, p&+4)
          v& = CVI(dfdata$, p&+6)
          channels(chnr&).dflayer(dfindex&).unit = v&
          k& = k&+1
          dfindex& = dfindex&+1
          IF dfindex& = %MAXDFRECORDS THEN EXIT FOR
        END IF
        p& = p&+8
      WEND
      channels(chnr&).dfcount(dfnr&) = k&
    END IF
  NEXT dfnr&

  'benutzerdefinierte Meldungen
  IF pCustMsg& > 0 THEN
    msgVer$ = MID$(a$, pCustMsg&-3, 4)
    IF msgVer$ = "CMSG" OR msgVer$ = "UMSG" THEN
      custmsglen& = CVL(a$, 275)
      IF custmsglen& > 0 THEN
        dfdata$ = MID$(a$, pCustMsg&+1, custmsglen&-4)
        p& = 1
        IF msgVer$ = "UMSG" THEN
          custMsgUserLang$ = LEFT$(dfdata$, 3)
          p& = p&+3
        END IF
        FOR i& = 0 TO %MAXCUSTOMMSG-1
          'deutscher Text
          v& = CVI(dfdata$, p&)
          germsg$ = MID$(dfdata$, p&+2, v&)
          p& = p&+2+v&
          'englischer Text
          v& = CVI(dfdata$, p&)
          engmsg$ = MID$(dfdata$, p&+2, v&)
          p& = p&+2+v&
          IF  msgVer$ = "UMSG" THEN
            'Text in benutzerdefinierte Sprache
            v& = CVI(dfdata$, p&)
            usermsg$ = MID$(dfdata$, p&+2, v&)
            p& = p&+2+v&
          END IF
          wmsg$$ = ""
          IF langcode$ = "GER" AND germsg$ <> "" THEN
            wmsg$$ = IIF$(msgVer$ = "CMSG", germsg$, UTF8TOCHR$(germsg$))
          ELSE
            IF langcode$ = "ENG" OR custMsgUserLang$ = "" THEN
              wmsg$$ = IIF$(msgVer$ = "CMSG", engmsg$, UTF8TOCHR$(engmsg$))
            ELSE
              IF UCASE$(custMsgUserLang$) = UCASE$(langcode$) AND usermsg$ <> "" THEN wmsg$$ = UTF8TOCHR$(usermsg$)
            END IF
          END IF
          gameMessages$$(512+i&) = FormatGameMessage(wmsg$$)
        NEXT i&
      END IF
    END IF
  END IF

  'Spieler ermitteln
  FOR i& = 0 TO channels(chnr&).info.nunits-1
    owner& = channels(chnr&).units(i&).owner
    IF owner& < 6  THEN plmask& = plmask& OR 2^owner&
  NEXT i&
  channels(chnr&).info.originalplayers = plmask&
  channels(chnr&).info.aliveplayers = plmask&
  channels(chnr&).info.originalplayercount = 0
  FOR i& = 0 TO 5
    IF (plmask& AND 2^i&) <> 0 THEN channels(chnr&).info.originalplayercount = channels(chnr&).info.originalplayercount+1
  NEXT i&

  'Spieler initialisieren
  FOR i& = 0 TO %MAXPLAYERS-1
    channels(chnr&).player(i&).allymask = ASC(allymatrix$, i&+1) OR 2^i&
    channels(chnr&).player(i&).selectedunit = -1
    channels(chnr&).player(i&).energy = 0
  NEXT i&
  CALL SetTeams(chnr&)

  'Startenergie aller Spieler berechnen
  FOR i& = 0 TO nshops&-1
    IF channels(chnr&).shops(i&).unittype = 1 AND channels(chnr&).shops(i&).shoptype > 0 THEN
      owner& = channels(chnr&).shops(i&).owner
      IF owner& < 6 THEN channels(chnr&).player(owner&).energy = channels(chnr&).player(owner&).energy+channels(chnr&).shops(i&).energy
    END IF
  NEXT i&

  'Startwetter anwenden
  CALL ApplyWeather(chnr&, channels(chnr&).info.weather, 0, channels(chnr&).info.ysize-1)

  'Korrekturen durchführen
  IF missionnr& = 21 THEN CALL FixULUARGE(chnr&)
  IF missionnr& = 29 THEN CALL FixGEKEFZU(chnr&)
  IF missionnr& = 42 THEN CALL FixHOLLEKO(chnr&)
  IF missionnr& = 275 THEN CALL FixBI3FINAL(chnr&)

  'Terrain/Einheiten-Sprites laden falls nötig
  IF gameMode& <> %GAMEMODE_SERVER THEN
    CALL LoadTerrainSpritesAndDef&
    CALL LoadUnitSpritesAndDef&
  END IF

  'Menü-Animation wieder aufnehmen
  IF menutime! > 0 THEN
    CALL UpdateDateTime
    menuOpenTime! = gametime!-menutime!
  END IF

  LoadMission& = 1
END FUNCTION



'Episode (Kampaign) starten
SUB StartEpisode(episode&)
  LOCAL chnr&, missionnr&, r&
  LOCAL emptycampaign AS TCampaign

  'Mission laden
  replayMode&(0) = %REPLAYMODE_OFF
  chnr& = 0
  channels(chnr&).campaign = emptycampaign
  SELECT CASE episode&
  CASE 1  'Battle Isle 2
    missionnr& = 0
  CASE 2:  'Erbe des Titan
    missionnr& = 42
  CASE 3:  'Kitanas Schloß
    missionnr& = 100
  CASE 4:  'Mehrspieler Kampagne
    missionnr& = 150
    CALL OpenLobby(missionnr&)
    EXIT SUB
  CASE 5:  'Battle Isle 3
    missionnr& = 256
  END SELECT
  r& = LoadMission&("MIS\MISS"+FORMAT$(missionnr&, "000")+".DAT", episode&, defaultDifficulty&, chnr&)
  IF r& <= 0 THEN
    IF r& = 0 THEN CALL PrintError(words$$(%WORD_INVALID_MISSIONFILE))
    EXIT SUB
  END IF

  'Karte starten
  CALL InitMap(chnr&, defaultDifficulty&)
END SUB



'Karte starten
SUB InitMap(chnr&, difficulty&)
  LOCAL i&, unitnr&, humanmask&, missionnr&, episode&, relativemissionnr&, isedt&, cutscenenr&

  CALL WaitGameInitialised

  'Schwierigkeit einstellen
  channels(chnr&).info.difficulty = difficulty&
  IF difficulty& = %DIFFICULTY_EASY OR cheatUnlimitedTurns& = 1 THEN channels(chnr&).info.turnlimit = -1

  'Spielnachrichten laden
  episode& = channels(chnr&).campaign.episode
  missionnr& = channels(chnr&).info.currentmission
  relativemissionnr& = missionnr&-GetEpisodeStartMap&(episode&)
  CALL BIDebugLog("Initializing map "+FORMAT$(missionnr&)+" ("+mapnames$(missionnr&)+").")
  isedt& = IsEDTMission&(missionnr&)
  IF gameMode& <> %GAMEMODE_SERVER THEN
    CALL LoadMessages&(isedt&)
  END IF

  'im Einzelspielermodus ersten menschlichen Spieler zum lokalen Spieler machen
  humanmask& = channels(chnr&).info.originalplayers AND NOT channels(chnr&).info.aimask
  IF CountBits&(humanmask&) = 1 THEN localPlayerNr& = LOG2(humanmask&)
  localPlayerMask& = 2^localPlayerNr&
  CALL BIDebugLog("Map has "+FORMAT$(CountBits&(channels(chnr&).info.originalplayers))+" players, local player is player #"+FORMAT$(localPlayerNr&)+".")

  'Sicht und Minimap berechnen
  IF gameMode& <> %GAMEMODE_SERVER THEN gameMode& = IIF&(CountBits&(humanmask&) = 1 OR replayMode&(chnr&) >= %REPLAYMODE_PLAY, %GAMEMODE_SINGLE, %GAMEMODE_CLIENT)
  CALL UpdateSightBlock(chnr&)
  CALL UpdateVisionAll(chnr&)
  IF gameMode& <> %GAMEMODE_CLIENT THEN gameState& = %GAMESTATE_INGAME
  updateMiniMap& = 1
  buttonEndTurn.Enabled = IIF&(LocalPlayersTurn& = 0, 0, 1)

  'Anzahl menschlicher Spieler ermitteln
  IF gameMode& <> %GAMEMODE_SERVER THEN channels(chnr&).info.connectioncount = CountBits&(humanmask&)

  'Karte um HQ zentrieren
  localPlayerHQ& = -1
  CALL ScrollToMapPos(0, 0, 1.0)
  FOR i& = 0 TO channels(chnr&).info.nshops-1
    IF channels(chnr&).shops(i&).owner = localPlayerNr& AND channels(chnr&).shops(i&).shoptype = %SHOPTYPE_HQ THEN
      cursorXPos& = channels(chnr&).shops(i&).position
      cursorYPos& = channels(chnr&).shops(i&).position2
      CALL ScrollToMapPos(cursorXPos&, cursorYPos&, 1.0)
      localPlayerHQ& = i&
      EXIT FOR
    END IF
  NEXT i&

  'Karte um erstbeste Einheit zentrieren
  IF localPlayerHQ& = -1 THEN
    FOR unitnr& = 0 TO channels(chnr&).info.nunits-1
      IF channels(chnr&).units(unitnr&).owner = localPlayerNr& THEN
        cursorXPos& = channels(chnr&).units(unitnr&).xpos
        cursorYPos& = channels(chnr&).units(unitnr&).ypos
        CALL ScrollToMapPos(cursorXPos&, cursorYPos&, 1.0)
        EXIT FOR
      END IF
    NEXT unitnr&
  END IF

  'Missionsbeschreibung Missionsziel vorbereiten
  CALL PreRenderMissionText(chnr&)

  'Zwischensequenz anzeigen
  IF gameMode& = %GAMEMODE_SINGLE OR (gameMode& = %GAMEMODE_CLIENT AND gameState& = %GAMESTATE_INGAME) AND channels(chnr&).info.fromSavegame = 0 AND channels(chnr&).info.turn = 0 THEN
    IF episode& = 5 AND relativemissionnr& >= 0 AND relativemissionnr& <= UBOUND(missionVideoNumbers) THEN
      IF missionVideoNumbers(relativemissionnr&).cutscene > 0 THEN InitVideoCutScene(missionVideoNumbers(relativemissionnr&).cutscene)
    ELSE
      SELECT CASE missionnr&
      CASE 0:
        IF InitCutScene&(1, 206, 2) <> 0 THEN EXIT SUB
      CASE 7:
        IF InitCutScene&(2, 205, 1) <> 0 THEN EXIT SUB
      CASE 15:
        IF InitCutScene&(3, 204, 3) <> 0 THEN EXIT SUB
      CASE 23:
        IF InitCutScene&(4, 201, 4) <> 0 THEN EXIT SUB
      CASE 42:
        IF InitCutScene&(4, 242, 1) <> 0 THEN EXIT SUB
      CASE 47:
        IF InitCutScene&(2, 243, 2) <> 0 THEN EXIT SUB
      CASE 52:
        IF InitCutScene&(1, 244, 6) <> 0 THEN EXIT SUB
      CASE ELSE:
        IF LEFT$(gameMessages$$(%MSG_CUSTOM_CUTSCENE), 1) = CHR$(4) THEN
          cutscenenr& = ASC(MID$(gameMessages$$(%MSG_CUSTOM_CUTSCENE), 2))
          IF cutscenenr& >= 1 AND cutscenenr& <= 4 AND InitCutScene&(cutscenenr&, %MSG_CUSTOM_CUTSCENE, cutscenenr&) <> 0 THEN EXIT SUB
        END IF
      END SELECT
    END IF
  END IF

  CALL InitMap2(chnr&, IIF&(gameState& = %GAMESTATE_NONE OR gameState& = %GAMESTATE_CREATELOBBY OR gameState& = %GAMESTATE_JOINLOBBY, 1, 0))
END SUB



'Karte starten (Teil 2)
SUB InitMap2(chnr&, skipInitHud&)
  LOCAL a$$, missionnr&

  'erste Runde initialisieren
  missionnr& = channels(chnr&).info.currentmission
  gamedataChanged& = 0
  protocolCount& = 0
  lastSelectedTransporter& = -1
  IF channels(chnr&).info.fromSavegame = 0 THEN
    channels(chnr&).info.turn = -1
    CALL InitRound(chnr&)
  ELSE
    a$$ = words$$(%WORD_TURN_NUMBER)
    REPLACE "%" WITH FORMAT$(channels(chnr&).info.turn+1) IN a$$
    CALL AddProtocol("$TITLE$"+a$$)
  END IF
  IF replayMode&(chnr&) < %REPLAYMODE_PLAY THEN mapRandomSeed& = INT(TIMER*100)
  RANDOMIZE mapRandomSeed&

  'Replay Aufzeichnung initalisieren
  IF replayMode&(chnr&) = %REPLAYMODE_OFF THEN CALL StartRecordReplay(chnr&)
  CALL StopAllSoundEffects

  IF skipInitHud& = 0 THEN CALL InitHUD
  IF nSoundTracks& > 0 THEN CALL StartMusic((missionnr& MOD nSoundTracks&)+1)
  CALL BIDebugLog("Initializing map completed.")
END SUB



'Benutzeroberfläche an Spielmodus anpassen
SUB InitHUD
  IF gameMode& <= %GAMEMODE_SINGLE THEN
    'Chat verstecken und Fortschrittbalken auf maximale Größe setzen
    '<linker Rand:40> , <Fortschrittsbalken:X> , <rechter Bereich:400>
    progressbar.XPos = 24*uiscale!
    progressbar.YPos = 1030*uiscale!
    progressbar.Width = windowWidth&-416*uiscale!
    progressbar.Height = 26*uiscale!
    CALL UpdateProgressbar
    editChat.Visible = 0
    buttonChatTeam.Visible = 0
    buttonChatAll.Visible = 0
  END IF

  IF gameMode& = %GAMEMODE_CLIENT THEN
    'Chat zeigen und Fortschrittsbalken verkleinern
    '<linker Rand:24> , <Chat Eingabefeld:X> , <Team Button:40> , <Alle Button:40> , <Fortschrittsbalken:400> , <rechter Bereich:392>
    progressbar.XPos = windowWidth&-792*uiscale!
    progressbar.YPos = 1030*uiscale!
    progressbar.Width = 400*uiscale!
    progressbar.Height = 26*uiscale!
    CALL UpdateProgressbar
    '
    editChat.Visible = 1
    editChat.XPos = 24*uiscale!
    editChat.YPos = 1030*uiscale!
    editChat.Width = windowWidth&-896*uiscale!
    editChat.Height = 26*uiscale!
    '
    buttonChatTeam.Visible = 1
    buttonChatTeam.XPos = windowWidth&-872*uiscale!
    buttonChatTeam.YPos = 1020*uiscale!
    buttonChatTeam.Width = 40*uiscale!
    buttonChatTeam.Height = 46*uiscale!
    '
    buttonChatAll.Visible = 1
    buttonChatAll.XPos = windowWidth&-832*uiscale!
    buttonChatAll.YPos = 1020*uiscale!
    buttonChatAll.Width = 40*uiscale!
    buttonChatAll.Height = 46*uiscale!
  END IF
END SUB



'Spiegelt einen String spaltenweise
FUNCTION MIRRORSTRINGX$(a$, rows&)
  LOCAL b$, cols&, i&, k&, p&, q&, v$

  b$ = a$
  cols& = LEN(a$)/rows&
  p& = 1
  q& = cols&
  FOR i& = 0 TO rows&-1
    FOR k& = 0 TO cols&-1
      ASC(b$, q&) = ASC(a$, p&)
      p& = p&+1
      q& = q&-1
    NEXT k&
    q& = q&+cols&*2
  NEXT i&

  MIRRORSTRINGX$ = b$
END FUNCTION



'Spiegelt einen String zeilenweise
FUNCTION MIRRORSTRINGY$(a$, rows&)
  LOCAL b$, rowlen&, i&, p&

  rowlen& = LEN(a$)/rows&
  p& = LEN(a$)-rowlen&+1
  FOR i& = 0 TO rows&-1
    b$ = b$+MID$(a$, p&, rowlen&)
    p& = p&-rowlen&
  NEXT i&

  MIRRORSTRINGY$ = b$
END FUNCTION



'Straßen/Wege/Gräben/Schienen durch Spiegelung erzeugen (pixeldata$ enthält die 24 Basis-Sprites, die restlichen 40 werden hier erzeugt)
SUB MIRRORROADS(sourcepixeldata$(), destpixels$, texturewidth&)
  LOCAL sprwd&, sprhg&, xoff&, yoff&

  sprwd& = 24
  sprhg& = 24
  xoff& = 96  'Breite eines Sprites im Zielbild in Bytes
  yoff& = 92160  'Höhe eines Sprites im Zielbild in Bytes

  'Sprites spiegeln
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(2), sprhg&), texturewidth&, 4*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(1), sprhg&), texturewidth&, 8*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(4), sprhg&), texturewidth&, 10*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(3), sprhg&), texturewidth&, 12*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(7), sprhg&), texturewidth&, 13*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(5), sprhg&), texturewidth&, 14*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(MIRRORSTRINGY$(sourcepixeldata$(2), sprhg&), sprhg&), texturewidth&, 16*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(4), sprhg&), texturewidth&, 17*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(MIRRORSTRINGY$(sourcepixeldata$(3), sprhg&), sprhg&), texturewidth&, 24*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(MIRRORSTRINGY$(sourcepixeldata$(7), sprhg&), sprhg&), texturewidth&, 25*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(MIRRORSTRINGY$(sourcepixeldata$(10), sprhg&), sprhg&), texturewidth&, 26*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(2), sprhg&), texturewidth&, 32*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(3), sprhg&), texturewidth&, 33*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(11), sprhg&), texturewidth&, 34*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(16), sprhg&), texturewidth&, 35*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(9), sprhg&), texturewidth&, 36*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(10), sprhg&), texturewidth&, 37*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(13), sprhg&), texturewidth&, 38*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(18), sprhg&), texturewidth&, 39*xoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(MIRRORSTRINGY$(sourcepixeldata$(4), sprhg&), sprhg&), texturewidth&, yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(7), sprhg&), texturewidth&, xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(12), sprhg&), texturewidth&, 2*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(17), sprhg&), texturewidth&, 3*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(10), sprhg&), texturewidth&, 4*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(15), sprhg&), texturewidth&, 5*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(14), sprhg&), texturewidth&, 6*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(19), sprhg&), texturewidth&, 7*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(23), sprhg&), texturewidth&, 8*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(5), sprhg&), texturewidth&, 9*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(MIRRORSTRINGY$(sourcepixeldata$(13), sprhg&), sprhg&), texturewidth&, 10*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(MIRRORSTRINGY$(sourcepixeldata$(18), sprhg&), sprhg&), texturewidth&, 11*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(13), sprhg&), texturewidth&, 12*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(14), sprhg&), texturewidth&, 13*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(MIRRORSTRINGY$(sourcepixeldata$(5), sprhg&), sprhg&), texturewidth&, 16*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(8), sprhg&), texturewidth&, 17*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(MIRRORSTRINGY$(sourcepixeldata$(14), sprhg&), sprhg&), texturewidth&, 18*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(MIRRORSTRINGY$(sourcepixeldata$(19), sprhg&), sprhg&), texturewidth&, 19*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(18), sprhg&), texturewidth&, 20*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(sourcepixeldata$(19), sprhg&), texturewidth&, 21*xoff&+yoff&+1, destpixels$, 0, 0)
  CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(sourcepixeldata$(21), sprhg&), texturewidth&, 22*xoff&+yoff&+1, destpixels$, 0, 0)
END SUB



'Fügt ein Sprite mit 8-Bit Farbtiefe in eine Textur mit 32-Bit Farbtiefe ein
SUB InsertSpr8IntoTex32(sprwd&, sprhg&, sprpixels$, texturewidth&, pdest&, texturepixels$, transparentcolor&, recol&)
  LOCAL x&, y&, psrc&, c&

  'Sprite von 8-Bit nach 32-Bit konvertieren
  psrc& = 1
  FOR y& = 0 TO sprhg&-1
    FOR x& = 0 TO sprwd&-1
      c& = ASC(sprpixels$, psrc&)
      IF c& <> transparentcolor& THEN
        IF c& < 8 AND recol& = 255 THEN
          MID$(texturepixels$, pdest&, 4) = CHR$(c&*20+20, c&*8+10, c&*8+20, 255)  'Farbe des Imperiums (BI3)
        ELSE
          IF c& < 8 THEN c& = c&+recol&
          MID$(texturepixels$, pdest&, 4) = MKL$(pal???(c&))
        END IF
      END IF
      psrc& = psrc&+1
      pdest& = pdest&+4
    NEXT x&
    pdest& = pdest&-4*sprwd&+texturewidth&*4
  NEXT y&
END SUB



'Wandelt alle nicht-transparenten Pixel im Bitmap in schwarze Pixel mit vorgegebener Deckkraft um
SUB CreateShadowBitmap(BYREF pixeldata$, BYVAL opacity&)
  LOCAL i&, n&, c$

  n& = LEN(pixeldata$)/4
  IF opacity& = -1 THEN
    'Deckkraft wird durch Rot-Kanal bestimmt
    FOR i& = 0 TO n&-1
      IF CVL(pixeldata$, i&*4+1) <> 0 THEN
        opacity& = INT(ASC(pixeldata$, i&*4+3)/2)
        c$ = MKL$(opacity&*16777216)
        MID$(pixeldata$, i&*4+1, 4) = c$
      END IF
    NEXT i&
  ELSE
    'feste Deckkraft
    c$ = MKL$(opacity&*16777216)
    FOR i& = 0 TO n&-1
      IF CVL(pixeldata$, i&*4+1) <> 0 THEN MID$(pixeldata$, i&*4+1, 4) = c$
    NEXT i&
  END IF
END SUB



'Prüft ob sich in den Pixeldaten eines 24x24 Sprites nicht-transparente Pixel außerhalb des Sechsecks befinden
FUNCTION SpriteHasNonTransparentPixelsOutsideHexagon&(pixeldata$)
  LOCAL p&, rownr&, hexagonwidth&, outsidepixels$

  p& = 1
  FOR rownr& = 1 TO 11
    hexagonwidth& = ASC($spx, rownr&)
    outsidepixels$ = MID$(pixeldata$, p&, 12-hexagonwidth&/2)
    IF VERIFY(outsidepixels$, CHR$(0, 143)) > 0 THEN
      SpriteHasNonTransparentPixelsOutsideHexagon& = 1
      EXIT FUNCTION
    END IF
    p& = p&+24
  NEXT rownr&
END FUNCTION



'Sprites aus BI2 LIB Datei laden
'tp = 0 : Terrain
'tp = 1 : Einheiten (je 2 Sprites sind in der Datei, die übrigen 4 werden durch Spiegelung erzeugt)
'tp = 2 : Straßen
'tp = 3 : Reifenspuren
FUNCTION LoadSpritesFromLib&(tp&, f$, transparentcolor&, recol&)
  LOCAL a$, d$, roadMapping$
  LOCAL k&, x&, y&, p&, q&, c&, wd&, pindex&, sprnr&
  LOCAL blockx&, blocky&, xoff&, yoff&, sprwd&, sprhg&, sprcountsrc&, sprcountdest&
  LOCAL pdest&, textureWidth&, textureHeight&, texturePixels$, sourcePixelData$()
  LOCAL isencrpyted&

  'Datei einlesen
  a$ = ReadFileContent$(f$, 0)
  IF a$ = "" THEN
    LoadSpritesFromLib& = -1
    EXIT FUNCTION
  END IF
  pindex& = CVL(a$)  'Zeiger auf Inhaltsverzeichnis
  sprwd& = 24
  sprhg& = 24
  sprcountsrc& = INT((LEN(a$)-4)/(sprwd&*sprhg&+12))

  'Direct2D Textur für alle Sprites erzeugen (mit 40 Sprites pro Zeile)
  sprcountdest& = sprcountsrc&
  IF tp& = 1 THEN sprcountdest& = sprcountdest&*3
  IF tp& = 2 OR tp& = 3 THEN
    sprcountdest& = 64
    roadMapping$ = CHR$(0,1,2,3,5,7,9,11 , 15,18,19,20,21,22,23,27 , 28,29,30,31,54,55,63,6)
    DIM sourcePixelData$(sprcountsrc&-1)
  END IF
  textureWidth& = 40*sprwd&
  textureHeight& = INT((sprcountdest&+39)/40)*sprhg&
  texturePixels$ = STRING$(textureWidth&*textureHeight&*4, 0)

  'Einzelbilder extrahieren
  FOR sprnr& = 0 TO sprcountsrc&-1
    p& = CVL(a$, pindex&+9)  'Zeiger auf Pixeldaten
    pindex& = pindex&+12

    'Sprite von LIB nach PB konvertieren
    IF isencrpyted& = 0 AND sprnr& = 0 AND SpriteHasNonTransparentPixelsOutsideHexagon&(MID$(a$, p&+1, sprwd&*sprhg&)) <> 0 THEN isencrpyted& = 1
    IF isencrpyted& = 0 THEN
      d$ = MID$(a$, p&+1, sprwd&*sprhg&)
    ELSE
      d$ = STRING$(sprwd&*sprhg&, 0)
      FOR y& = 0 TO sprhg&-1
        blocky& = INT(y&/6)
        yoff& = y& MOD 6
        FOR x& = 0 TO sprwd&-1
          blockx& = INT(x&/6)  '0..3
          xoff& = x& MOD 6     '0..5
          q& = (xoff&*4+blocky&)+(yoff&*4+blockx&)*sprwd&+1
          p& = p&+1
          ASC(d$, q&) = ASC(a$, p&)
        NEXT x&
      NEXT y&
    END IF

    'Position in der Textur berechnen
    SELECT CASE tp&
    CASE 0:
      y& = INT(sprnr&/40)
      x& = sprnr&-y&*40
    CASE 1:
      IF (sprnr& AND 1) = 0 THEN
        y& = INT(sprnr&*3/40)
        x& = sprnr&*3-y&*40
      ELSE
        y& = INT((sprnr&*3-2)/40)
        x& = sprnr&*3-2-y&*40
      END IF
    CASE 2, 3:
      x& = ASC(roadMapping$, sprnr&+1)
      y& = 0
      IF x& > 39 THEN
        x& = x&-40
        y& = 1
      END IF
    END SELECT
    pdest& = x&*sprwd&*4+y&*sprhg&*textureWidth&*4+1

    'Sprite von 8-Bit nach 32-Bit konvertieren
    CALL InsertSpr8IntoTex32(sprwd&, sprhg&, d$, textureWidth&, pdest&, texturePixels$, transparentcolor&, recol&)
    IF tp& = 1 AND (sprnr& AND 1) = 0 THEN
      'Richtung 3 durch Y-Spiegelung erzeugen
      y& = INT((sprnr&*3+3)/40)
      x& = sprnr&*3+3-y&*40
      pdest& = x&*sprwd&*4+y&*sprhg&*textureWidth&*4+1
      CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(d$, sprhg&), textureWidth&, pdest&, texturePixels$, transparentcolor&, recol&)
    END IF
    IF tp& = 1 AND (sprnr& AND 1) = 1 THEN
      'Richtung 2 durch Y-Spiegelung erzeugen
      y& = INT((sprnr&*3-1)/40)
      x& = sprnr&*3-1-y&*40
      pdest& = x&*sprwd&*4+y&*sprhg&*textureWidth&*4+1
      CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGY$(d$, sprhg&), textureWidth&, pdest&, texturePixels$, transparentcolor&, recol&)
      'Richtung 4 durch X/Y-Spiegelung erzeugen
      y& = INT((sprnr&*3+1)/40)
      x& = sprnr&*3+1-y&*40
      pdest& = x&*sprwd&*4+y&*sprhg&*textureWidth&*4+1
      CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(MIRRORSTRINGY$(d$, sprhg&), sprhg&), textureWidth&, pdest&, texturePixels$, transparentcolor&, recol&)
      'Richtung 5 durch X-Spiegelung erzeugen
      y& = INT((sprnr&*3+2)/40)
      x& = sprnr&*3+2-y&*40
      pdest& = x&*sprwd&*4+y&*sprhg&*textureWidth&*4+1
      CALL InsertSpr8IntoTex32(sprwd&, sprhg&, MIRRORSTRINGX$(d$, sprhg&), textureWidth&, pdest&, texturePixels$, transparentcolor&, recol&)
    END IF
    IF tp& = 2 OR tp& = 3 THEN sourcePixelData$(sprnr&) = d$
  NEXT sprnr&

  'restliche Straßen/Wege/Gräben/Schienen durch Spiegelung erzeugen
  IF tp& = 2 OR tp& = 3 THEN CALL MIRRORROADS(sourcePixelData$(), texturePixels$, textureWidth&)

  'Reifenspuren in Transparenzmaske umwandeln
  IF tp& = 3 THEN CALL CreateShadowBitmap(texturePixels$, -1)

  'Direct2D Bitmap erzeugen
  sprnr& = D2D.CreateMemoryBitmap(textureWidth&, textureHeight&, texturePixels$)

  'Bitmap für Einheiten mit verbrauchten Aktionen erzeugen wenn Bitmap für neutrale Einheit erzeugt wurde
  IF recol& = ASC(RIGHT$(playerColors$, 1)) THEN
    CALL CreateShadowBitmap(texturePixels$, 192)
    hUnits&(LEN(playerColors$)) = D2D.CreateMemoryBitmap(textureWidth&, textureHeight&, texturePixels$)
  END IF

  LoadSpritesFromLib& = sprnr&
END FUNCTION



'Shop-Sprites aus BI2 LIB Datei laden und in Spielerfarben neu colorieren
FUNCTION LoadShopSpritesFromLib&(f$, transparentcolor&, shopsprnr$)
  LOCAL a$, d$
  LOCAL i&, x&, y&, p&, q&, wd&, pindex&, npl&, pl&, sprnr&, recol&, destsprnr&, isencrpyted&
  LOCAL blockx&, blocky&, xoff&, yoff&, sprwd&, sprhg&, sprcountsrc&, sprcountdest&
  LOCAL pdest&, textureWidth&, textureHeight&, texturePixels$

  'Datei einlesen
  a$ = ReadFileContent$(f$, 0)
  IF a$ = "" OR shopsprnr$ = "" THEN
    LoadShopSpritesFromLib& = -1
    EXIT FUNCTION
  END IF
  pindex& = CVL(a$)  'Zeiger auf Inhaltsverzeichnis
  sprwd& = 24
  sprhg& = 24
  sprcountsrc& = LEN(shopsprnr$)/2
  npl& = LEN(playerColors$)

  'Direct2D Textur für alle Sprites erzeugen (mit 40 Sprites pro Zeile)
  sprcountdest& = sprcountsrc&*npl&
  textureWidth& = 40*sprwd&
  textureHeight& = INT((sprcountdest&+39)/40)*sprhg&
  texturePixels$ = STRING$(textureWidth&*textureHeight&*4, 0)

  'Einzelbilder extrahieren
  FOR pl& = 1 TO npl&
    recol& = ASC(playerColors$, pl&)
    FOR i& = 0 TO sprcountsrc&-1
      sprnr& = CVI(shopsprnr$, i&*2+1)
      p& = CVL(a$, pindex&+sprnr&*12+9)  'Zeiger auf Pixeldaten

      'Sprite von LIB nach PB konvertieren
      IF isencrpyted& = 0 AND i& = 0 AND SpriteHasNonTransparentPixelsOutsideHexagon&(MID$(a$, p&+1, sprwd&*sprhg&)) <> 0 THEN isencrpyted& = 1
      IF isencrpyted& = 0 THEN
        d$ = MID$(a$, p&+1, sprwd&*sprhg&)
      ELSE
        d$ = STRING$(sprwd&*sprhg&, 0)
        FOR y& = 0 TO sprhg&-1
          blocky& = INT(y&/6)
          yoff& = y& MOD 6
          FOR x& = 0 TO sprwd&-1
            blockx& = INT(x&/6)  '0..3
            xoff& = x& MOD 6     '0..5
            q& = (xoff&*4+blocky&)+(yoff&*4+blockx&)*sprwd&+1
            p& = p&+1
            ASC(d$, q&) = ASC(a$, p&)
          NEXT x&
        NEXT y&
      END IF

      'Position in der Textur berechnen
      y& = INT(destsprnr&/40)
      x& = destsprnr&-y&*40
      pdest& = x&*sprwd&*4+y&*sprhg&*textureWidth&*4+1
      destsprnr& = destsprnr&+1

      'Sprite von 8-Bit nach 32-Bit konvertieren
      CALL InsertSpr8IntoTex32(sprwd&, sprhg&, d$, textureWidth&, pdest&, texturePixels$, transparentcolor&, recol&)
    NEXT i&
  NEXT pl&

  'Direct2D Bitmap erzeugen
  sprnr& = D2D.CreateMemoryBitmap(textureWidth&, textureHeight&, texturePixels$)

  LoadShopSpritesFromLib& = sprnr&
END FUNCTION



'Dateien aus einem LIB Archiv extrahieren
FUNCTION ExtractFromLib&(f$, targetarray$())
  LOCAL a$, b$, g$, filelen&, pindex&, nrecords&, i&, p&, q&

  'Datei einlesen
  a$ = ReadFileContent$(f$, 0)
  IF a$ = "" THEN
    ExtractFromLib& = -1
    EXIT FUNCTION
  END IF

  'Anzahl Einträge ermitteln
  filelen& = LEN(a$)
  pindex& = CVL(a$)
  IF pindex& < 4 OR pindex& > filelen& THEN
    ExtractFromLib& = -1
    EXIT FUNCTION
  END IF
  nrecords& = INT((filelen&-pindex&)/12)
  REDIM targetarray$(nrecords&-1)

  'Einträge extrahieren
  FOR i& = 0 TO nrecords&-1
    p& = CVL(a$, pindex&+i&*12+9)+1
    IF i& = nrecords&-1 THEN q& = pindex&+1 ELSE q& = CVL(a$, pindex&+i&*12+21)+1
    targetarray$(i&) = MID$(a$, p&, q&-p&)
  NEXT i&

  ExtractFromLib& = nrecords&
END FUNCTION



'Palette laden
FUNCTION LoadPalette&(f$)
  LOCAL i&, a$
  DIM pal???(255)

  'Datei einlesen
  a$ = ReadFileContent$(f$, 0)
  IF a$ = "" THEN EXIT FUNCTION

  'Palette extrahieren und Farben von R6:G6:B6 nach B8:G8:R8 konvertieren
  FOR i& = 0 TO 255
    pal???(i&) = ASC(a$, i&*3+1)*65536+ASC(a$, i&*3+2)*256+ASC(a$, i&*3+3)+255*16777216
  NEXT i&

  LoadPalette& = 1
END FUNCTION



'Liefert das Artwork-Handle zu einem Einheitennamen
FUNCTION GetArtworkHandleByUnitclassName&(BYVAL unitclassname$)
  LOCAL namelen&, i&, h&

  CALL FixArtworkFilename(unitclassname$)
  unitclassname$ = unitclassname$+"."
  namelen& = LEN(unitclassname$)
  ARRAY SCAN artworkFilenames$(), FROM 1 TO namelen&, COLLATE UCASE, =unitclassname$, TO i&
  IF i& > 0 THEN h& = hArtwork&(i&-1)

  GetArtworkHandleByUnitclassName& = h&
END FUNCTION



'Korrigiert den Dateinamen eines Artworks
SUB FixArtworkFilename(f$)
  LOCAL p&

  SELECT CASE UCASE$(f$)
  CASE "VADER DF":
    f$ = "VADER-DF"
    EXIT SUB
  CASE "SAMURAI-3", "VADER-DF", "MEDUSA-2", "UX-2", "UX-3", "CITADELLE":
    EXIT SUB
  END SELECT

  p& = INSTR(f$, ANY " -")
  IF p& > 0 THEN f$ = LEFT$(f$, p&-1)
  REPLACE "ö" WITH "OE" IN f$
  REPLACE "." WITH "_" IN f$
  f$ = LEFT$(f$, 8)
  SELECT CASE UCASE$(f$)
  CASE "SUPER": f$ = "Supervir"
  CASE "SHELL": f$ = "Nautilus"
  CASE "DROHNE": f$ = "Thunder"
  CASE "MOEVE": f$ = "MOEWE"
  CASE "UHU": f$ = "U_H_U_"
  END SELECT
END SUB



'Lädt die Datei-Inhalte aller Artwork-Dateien, um diese zum Download zur Verfügung zu stellen (wird nur vom Server aufgerufen)
FUNCTION CacheArtworks&
  LOCAL f$
  DIM artworkCache$(%MAXUNITCLASSES-1), artworkCacheNames$(%MAXUNITCLASSES-1)

  'Bilder laden
  artworkCacheSize& = 0
  f$ = DIR$(EXEPATH$+"ARTWORK\*.gif")
  WHILE f$ <> ""
    artworkCache$(artworkCacheSize&) = ReadFileContent$("ARTWORK\"+f$, 0)
    IF artworkCache$(artworkCacheSize&) <> "" THEN
      artworkCacheNames$(artworkCacheSize&) = UCASE$(PATHNAME$(NAME, f$))
      artworkCacheSize& = artworkCacheSize&+1
    END IF
    f$ = DIR$
  WEND

  CacheArtworks& = artworkCacheSize&
END FUNCTION



'Lädt die Artworks aller Einheiten
FUNCTION LoadArtworks&
  LOCAL i&, f$, a$$, fileext$, filenm$
  LOCAL wd&, hg&, px$, r&, nfiles&, filenamelen&

  'Dateien im ARTWORK Verzeichnis ermitteln
  DIM hArtwork&(1023), artworkFilenames$(1023)
  f$ = DIR$(EXEPATH$+"ARTWORK\*")
  WHILE f$ <> "" AND nfiles& < 1024
    fileext$ = LCASE$(PATHNAME$(EXTN, f$))
    filenm$ = UCASE$(PATHNAME$(NAME, f$))+"."
    IF fileext$ = ".bmp" OR fileext$ = ".gif" THEN
      filenamelen& = LEN(filenm$)
      ARRAY SCAN artworkFilenames$(0) FOR nfiles&, FROM 1 TO filenamelen&, COLLATE UCASE, =filenm$, TO i&
      IF nfiles& = 0 OR i& = 0 THEN
        artworkFilenames$(nfiles&) = f$
        nfiles& = nfiles&+1
      END IF
    END IF
    f$ = DIR$
  WEND
  REDIM PRESERVE hArtwork&(nfiles&-1), artworkFilenames$(nfiles&-1)

  progressbar.MaxProgress = nfiles&
  progressbar.HighlightColor = brushPlayer&(localPlayerNr&)
  initProgressText$$ = words$$(%WORD_LOADING_ARTWORKS)
  initProgress& = 0

  'zufälliges Bild laden
  i& = RND(0, nfiles&-1)

  'Bilder laden
  WHILE hArtwork&(i&) = 0
    CALL EnterSemaphore(semaphore_crttexture&)
    r& = D2DGraphicBitmapLoad&(EXEPATH$+"ARTWORK\"+artworkFilenames$(i&), hArtwork&(i&))
    CALL LeaveSemaphore(semaphore_crttexture&)
    IF r& = 0 THEN
      a$$ = words$$(%WORD_ARTWORK_ERROR)
      REPLACE "%" WITH f$ IN a$$
      CALL PrintError(a$$)
    ELSE
      IF hLoadingScreen& = 0 THEN
        loadingScreenUnitType& = i&
        hLoadingScreen& = hArtwork&(i&)
      END IF
    END IF

    'nächstes Bild laden
    i& = i&+1
    initProgress& = initProgress&+1
    IF (initProgress& AND 7) = 1 THEN D2D.OnRender(0, 0)
    IF i& >= nfiles& THEN i& = 0
  WEND

  LoadArtworks& = 1
END FUNCTION



'Wrapper für D2D.GraphicBitmapLoad
FUNCTION D2DGraphicBitmapLoad&(BYVAL f$, BYREF h&)
  LOCAL errmsg$$

  IF INSTR(f$, ":") = 0 THEN f$ = EXEPATH$+f$
  h& = D2D.GraphicBitmapLoad(f$, 0, 0)
  CALL BIDebugLog("Loading bitmap "+f$+" ("+FORMAT$(h&)+").")
  IF h& < 0 THEN
    errmsg$$ = words$$(%WORD_FILE_NOT_FOUND)
    REPLACE "%" WITH f$ IN errmsg$$
    CALL PrintError(errmsg$$)
    CALL BIDebugLog(errmsg$$)
    EXIT FUNCTION
  END IF

  D2DGraphicBitmapLoad& = -1
END FUNCTION



'Lädt alle BI2020 Bitmaps
FUNCTION LoadBitmaps&
  LOCAL i&
  DIM hUnits&(LEN($playercolorsbi2)), hRoads&(6)

  'Farbpalette
  CALL LoadPalette&("LBM\GAME.PAL")

  'Hintergrundbild
  IF D2DGraphicBitmapLoad&("GFX\BI2020Console.png", hSkin&) = 0 THEN EXIT FUNCTION

  'Dialoge
  IF D2DGraphicBitmapLoad&("GFX\dialogues.png", hDialog&) = 0 THEN EXIT FUNCTION
  IF D2DGraphicBitmapLoad&("GFX\dialogues2.png", hDialog2&) = 0 THEN EXIT FUNCTION

  'Oberflächenelemente
  IF D2DGraphicBitmapLoad&("GFX\hudelements.png", hHudElements&) = 0 THEN EXIT FUNCTION

  'Intro-Elemente
  IF D2DGraphicBitmapLoad&("GFX\intro.png", hIntro&) = 0 THEN EXIT FUNCTION

  'Einheiten-Panels
  IF D2DGraphicBitmapLoad&("GFX\panels.png", hPanels&) = 0 THEN EXIT FUNCTION

  'Gebäude
  IF D2DGraphicBitmapLoad&("GFX\buildings.png", hBuildings&) = 0 THEN EXIT FUNCTION

  'Buttons
  IF D2DGraphicBitmapLoad&("GFX\buttons.png", hButtons&) = 0 THEN EXIT FUNCTION

  'Terrain Sprites
  IF LoadTerrainSpritesAndDef& = 0 THEN EXIT FUNCTION

  'Einheiten Sprites
  IF LoadUnitSpritesAndDef& = 0 THEN EXIT FUNCTION

  'Straßen
  FOR i& = 0 TO 6
    hRoads&(i&) = LoadSpritesFromLib&(IIF&(i& = 4, 3, 2), "LIB\LAYR00"+FORMAT$(i&)+".LIB", 0, 0)
    IF hRoads&(i&) < 0 THEN EXIT FUNCTION
  NEXT i&

  'neues Bitmap für die Minimap
  hMinimap& = D2D.CreateMemoryBitmap(344, 344, REPEAT$(344*344, MKL$(&HFF46130D)))
  IF hMinimap& < 0 THEN
    CALL BIDebugLog("Failed to created memory bitmap for minimap.")
    EXIT FUNCTION
  END IF

  'neues Bitmap für die Kartenvorschau
  hMapPreview& = D2D.CreateMemoryBitmap(240, 240, REPEAT$(240*240, MKL$(&HFF46130D)))
  IF hMapPreview& < 0 THEN
    CALL BIDebugLog("Failed to created memory bitmap for map preview.")
    EXIT FUNCTION
  END IF

  LoadBitmaps& = 1
END FUNCTION



'Lädt die Einheitensprites
FUNCTION LoadUnitSpritesAndDef&
  LOCAL f$, i&, episode&

  episode& = GetEpisodeForMap&(channels(0).info.currentmission)
  IF unitsLoadedForEpisode& <> episode& THEN
    IF ReadUnitDefs&(0) = 0 THEN EXIT FUNCTION
    f$ = AdjustFilenameForEpisode$(EXEPATH$+$UNITSPRITEFILE, episode&, 1)
    FOR i& = 1 TO LEN(playerColors$)
      hUnits&(i&-1) = LoadSpritesFromLib&(1, f$, 143, ASC(playerColors$, i&))
      IF hUnits&(i&-1) < 0 THEN EXIT FUNCTION
    NEXT i&
    unitsLoadedForEpisode& = episode&
  END IF

  LoadUnitSpritesAndDef& = 1
END FUNCTION



'Lädt die Terrainsprites
FUNCTION LoadTerrainSpritesAndDef&
  LOCAL f$, episode&

  episode& = GetEpisodeForMap&(channels(0).info.currentmission)
  IF terrainLoadedForEpisode& <> episode& THEN
    IF ReadTerrainDefs&(0) = 0 THEN EXIT FUNCTION
    f$ = AdjustFilenameForEpisode$(EXEPATH$+$TERRAINSPRITEFILE, episode&, 1)
    hTerrain& = LoadSpritesFromLib&(0, f$, 0, 0)
    IF hTerrain& < 0 THEN EXIT FUNCTION
    hShops& = LoadShopSpritesFromLib&(f$, 0, $BUILDINGSPRITES)
    IF hShops& < 0 THEN EXIT FUNCTION
    terrainLoadedForEpisode& = episode&
  END IF

  LoadTerrainSpritesAndDef& = 1
END FUNCTION



'Lädt eine benutzerdefinierte Animation aus einer PNG Datei (mit 8 Sprites pro Zeile)
FUNCTION LoadCustomAnimation&(animationnr&, BYVAL f$, framewidth&, frameheight&, framecount&)
  LOCAL e$$

  f$ = EXEPATH$+"GFX\"+f$+".png"
  IF D2DGraphicBitmapLoad&(f$, hAnimations&(animationnr&)) = 0 THEN
    e$$ = words$$(%WORD_CUSTOMANIMATION_ERROR)
    REPLACE "%" WITH f$ IN e$$
    CALL PrintError(e$$)
    EXIT FUNCTION
  END IF
  animationWidth&(animationnr&) = framewidth&
  animationHeight&(animationnr&) = frameheight&
  animationFrameCount&(animationnr&) = framecount&

  LoadCustomAnimation& = 1
END FUNCTION



'Konvertiert eine BI2 Animation in eine Textur (mit 8 Sprites pro Zeile)
FUNCTION AnimationToTexture&(a$, animationnr&, BYREF framewidth&, BYREF frameheight&, BYREF framecount&)
  LOCAL hTexture&, i&, k&, n&, c&, psrc&, pdest&, deststart&, framenr&, framestart&, lastframe&, totalsize&, framelen&, textureWidth&, textureHeight&, transparentcolor&
  LOCAL fdata$, prevframedata$, texturePixels$

  'Header auslesen
  IF LEFT$(a$, 4) <> "BBHD" THEN EXIT FUNCTION
  framestart& = CVL(a$, 5)
  lastframe& = CVL(a$, 9)
  totalsize& = CVL(a$, 13)
  framecount& = CVL(a$, 17)
  transparentcolor& = ASC(a$, 25)
  framewidth& = CVI(a$, 27)
  frameheight& = CVI(a$, 29)
  textureWidth& = framewidth&*MIN&(8, framecount&)
  textureHeight& = frameheight&*INT((framecount&+7)/8)
  IF textureWidth& > 4096 OR textureHeight& > 4096 THEN EXIT FUNCTION
  texturePixels$ = STRING$(4*textureWidth&*textureHeight&, 0)

  'Frames extrahieren
  deststart& = 1
  FOR framenr& = 0 TO framecount&-1
    IF MID$(a$, framestart&+1, 4) <> "BBFR" THEN EXIT FOR
    framelen& = CVL(a$, framestart&+7)

    'Pixeldaten extrahieren
    fdata$ = MID$(a$, framestart&+29, framelen&-28)
    IF LEFT$(fdata$, 4) = "TPWM" THEN fdata$ = DecodeTPWM$(fdata$)
    IF LEN(fdata$) < framewidth&*frameheight& THEN
      'letzten Frame wiederholen (bis Kompressionsverfahren entschlüsselt ist)
      fdata$ = prevframedata$
    END IF

    'Pixeldaten von 8-Bit nach 32-Bit konvertieren
    psrc& = 1
    pdest& = deststart&
    FOR k& = 0 TO frameheight&-1
      FOR i& = 0 TO framewidth&-1
        c& = ASC(fdata$, psrc&)
        IF c& <> transparentcolor& THEN MID$(texturepixels$, pdest&, 4) = MKL$(pal???(c&))
        psrc& = psrc&+1
        pdest& = pdest&+4
      NEXT i&
      pdest& = pdest&-4*framewidth&+4*textureWidth&
    NEXT k&

    prevframedata$ = fdata$
    framestart& = framestart&+framelen&
    deststart& = deststart&+framewidth&*4
    IF (framenr& AND 7) = 7 THEN deststart& = deststart&+textureWidth&*(frameheight&-1)*4
  NEXT framenr&

  'Textur erzeugen
  CALL EnterSemaphore(semaphore_crttexture&)
  hTexture& = D2D.CreateMemoryBitmap(textureWidth&, textureHeight&, texturePixels$)
  CALL LeaveSemaphore(semaphore_crttexture&)

  AnimationToTexture& = hTexture&
END FUNCTION



'Konvertiert eine BI2 Animations-Archiv in ein Textur-Array
FUNCTION AnimationListToTextureArray&(f$, startindex&)
  LOCAL i&, nanimations&
  LOCAL animationfiles$()

  'Archiv entpacken
  nanimations& = ExtractFromLib&(f$, animationfiles$())
  IF nanimations& <= 0 THEN EXIT FUNCTION

  'Texturen erstellen
  FOR i& = 0 TO nanimations&-1
    hAnimations&(startindex&+i&) = AnimationToTexture&(animationfiles$(i&), i&, animationWidth&(startindex&+i&), animationHeight&(startindex&+i&), animationFrameCount&(startindex&+i&))
'PRINT startindex&+i&, animationWidth&(startindex&+i&), animationHeight&(startindex&+i&), animationFrameCount&(startindex&+i&)
    initProgress& = initProgress&+1
  NEXT i&

  AnimationListToTextureArray& = nanimations&
END FUNCTION



'Kompiliert ein Animations-Skript
FUNCTION CompileAnimationScript$(BYVAL sourcetext$, BYVAL isedt&)
  LOCAL i&, k&, p&, q&, stepnr&, a$, cmd$, compiled$
  LOCAL videofile&, seq&, start&, duration&, timestamps&()
  LOCAL animsteps() AS TAnimationScript
  DIM animsteps(99), timestamps&(99)

  'Zeilenumbrüche und Blanks vereinheitlichen
  sourcetext$ = UCASE$(sourcetext$)
  REPLACE CHR$(10) WITH CHR$(13) IN sourcetext$
  REPLACE CHR$(9) WITH CHR$(32) IN sourcetext$

  'Befehle suchen
  p& = 1
  WHILE p& <= LEN(sourcetext$)
    q& = INSTR(p&, sourcetext$, CHR$(13))
    IF q& = 0 THEN q& = LEN(sourcetext$)+1
    a$ = MID$(sourcetext$, p&, q&-p&)
    p& = q&+1
    IF LEFT$(a$, 4) = "REM " THEN a$ = ""
    REPLACE CHR$(32) WITH "" IN a$
    q& = INSTR(a$, "(")
    IF q& > 0 THEN
      cmd$ = LEFT$(a$, q&-1)
      a$ = MID$(a$, q&+1)
      q& = INSTR(a$, ")")
      IF q& > 0 THEN a$ = LEFT$(a$, q&-1)

      'Befehl auswerten
      SELECT CASE cmd$
      CASE "PLAY"  'Video darstellen
        videofile& = VAL(PARSE$(a$, ",", 1))
        IF videofile& > 8 THEN ITERATE LOOP
        animsteps(stepnr&).animtype = %MSGANI_VID
        seq& = VAL(PARSE$(a$, ",", 4))+videofile&*128-128+512*isedt&
        IF isedt& = 1 AND animationFrameCount&(seq&) = 0 THEN seq& = seq&-512
        animsteps(stepnr&).sequence = seq&
        animsteps(stepnr&).starttime = VAL(PARSE$(a$, ",", 7))
        animsteps(stepnr&).duration = VAL(PARSE$(a$, ",", 8))
        animsteps(stepnr&).animationspeed = VAL(PARSE$(a$, ",", 6))
        animsteps(stepnr&).freezetime = VAL(PARSE$(a$, ",", 9))
        animsteps(stepnr&).zpos = VAL(PARSE$(a$, ",", 10))
        animsteps(stepnr&).xoffset = VAL(PARSE$(a$, ",", 11))
        animsteps(stepnr&).yoffset = VAL(PARSE$(a$, ",", 12))
      CASE "FX"  'Geräusch abspielen
        animsteps(stepnr&).animtype = %MSGANI_SFX
        seq& = VAL(PARSE$(a$, ",", 3))
        animsteps(stepnr&).sequence = seq&
        animsteps(stepnr&).starttime = VAL(PARSE$(a$, ",", 4))
        animsteps(stepnr&).duration = 25*LEN(DS.WaveData(seq&))/DS.SampleRate(seq&)/2
      CASE "WRITE"  'Text darstellen
        animsteps(stepnr&).animtype = %MSGANI_TXT
        animsteps(stepnr&).sequence = VAL(PARSE$(a$, ",", 2))
        animsteps(stepnr&).starttime = 0  'VAL(PARSE$(a$, ",", 4))
        animsteps(stepnr&).duration = 999 'VAL(PARSE$(a$, ",", 5))
      END SELECT

      timestamps&(stepnr&) = animsteps(stepnr&).starttime
      stepnr& = stepnr&+1
      IF stepnr& = 100 THEN EXIT LOOP
    END IF
  WEND

  'Schritte nach Startzeitpunkt sortieren
  IF stepnr& = 0 THEN EXIT FUNCTION
  ARRAY SORT timestamps&(0) FOR stepnr&, TAGARRAY animsteps()

  'Standbildzeiten berechnen
  FOR i& = 0 TO stepnr&-1
    IF animsteps(i&).animtype = %MSGANI_VID AND animsteps(i&).freezetime = 1 THEN
      seq& = animsteps(i&).sequence
      FOR k& = i&+1 TO stepnr&-1
        IF animsteps(k&).animtype = %MSGANI_VID AND animsteps(k&).sequence = seq& THEN
          animsteps(i&).freezetime = MAX&(0, animsteps(k&).starttime-animsteps(i&).starttime-animsteps(i&).duration)
          EXIT FOR
        END IF
      NEXT k&
      IF animsteps(i&).freezetime = 1 THEN animsteps(i&).freezetime = MAX&(0, animsteps(stepnr&-1).starttime+animsteps(stepnr&-1).duration-animsteps(i&).starttime-animsteps(i&).duration)
    END IF
  NEXT i&

  'kompilierte Schritte zusammenpacken
  compiled$ = PEEK$(VARPTR(animsteps(0)), SIZEOF(TAnimationScript)*stepnr&)

  CompileAnimationScript$ = compiled$
END FUNCTION



'Lädt die Animations-Skripte
'filenr: 0 = BI2 , 1 = EDT
FUNCTION ExtractAnimationScripts&(filenr&)
  LOCAL a$, i&
  LOCAL scriptfiles$()

  'Archiv entpacken
  nAnimationsScripts& = ExtractFromLib&("ANI\ASCR"+FORMAT$(filenr&, "000")+".LIB", scriptfiles$())
  IF nAnimationsScripts& <= 0 THEN EXIT FUNCTION

  'Skripte kompilieren
  FOR i& = 0 TO nAnimationsScripts&-1
    a$ = scriptfiles$(i&)
    IF LEFT$(a$, 4) = "TPWM" THEN a$ = DecodeTPWM$(a$)
    animationsScripts$(i&+filenr&*256) = CompileAnimationScript$(a$, filenr&)
  NEXT i&

  'Skript für Verlust eines eigenen Gebäudes und benutzerdefinierte Missionsbeschreibung hinzufügen
  IF filenr& = 0 THEN
    'Art(1=Person,3=Hintergrund),LNA,LNR,Video,PMO,Geschwindigkeit,Startzeit,Länge,EinfrierenNachEnde,Z,X,Y,DTX,DTY,WIL,WIT,WIW,WIH
    a$ = "play (1, 0,  0, 43,  0,  8,  0,  10,  0,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (3, 0,  0, 69,  0, 12, 10,  290, 2,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Gebäude-Einnahme
       + "play (1, 0,  0, 24,  0, 12, 110, 60,  0,  1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Bildstörung
       + "play (1, 0,  0, 43,  0,  8, 170, 10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "write (0, "+FORMAT$(%MSG_BUILDINGLOST)+")"+CHR$(13,10) _
       + "fx (50,0,21,001,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,110,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,130,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,150,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_BUILDINGLOST0) = CompileAnimationScript$(a$, 0)
    '
    a$ = "play (1, 0,  0, 43,  0,  8,  0,  10,  0,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (3, 0,  0, 69,  0, 12, 10,  290, 2,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Gebäude-Einnahme
       + "play (1, 0,  0, 24,  0, 12, 110, 60,  0,  1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Bildstörung
       + "play (1, 0,  0, 22,  0,  8, 170, 10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Titan-Net Logo
       + "write (0, "+FORMAT$(%MSG_BUILDINGLOST)+")"+CHR$(13,10) _
       + "fx (50,0,21,001,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,110,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,130,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,150,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_BUILDINGLOST1) = CompileAnimationScript$(a$, 0)
    '
    a$ = "play (1, 0,  0, 43,  0,  8,  0,  10,  0,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (3, 0,  0, 69,  0, 12, 10,  290, 2,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Gebäude-Einnahme
       + "play (1, 0,  0, 24,  0, 12, 110, 60,  0,  1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Bildstörung
       + "play (1, 0,  0, 20,  0,  8, 170, 10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Logo Spieler 2 (Orange)
       + "write (0, "+FORMAT$(%MSG_BUILDINGLOST)+")"+CHR$(13,10) _
       + "fx (50,0,21,001,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,110,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,130,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,150,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_BUILDINGLOST2) = CompileAnimationScript$(a$, 0)
    '
    a$ = "play (1, 0,  0, 43,  0,  8,  0,  10,  0,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (3, 0,  0, 69,  0, 12, 10,  290, 2,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Gebäude-Einnahme
       + "play (1, 0,  0, 24,  0, 12, 110, 60,  0,  1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Bildstörung
       + "play (1, 0,  0, 44,  0,  8, 170, 10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Logo Spieler 3 (Grün)
       + "write (0, "+FORMAT$(%MSG_BUILDINGLOST)+")"+CHR$(13,10) _
       + "fx (50,0,21,001,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,110,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,130,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,150,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_BUILDINGLOST3) = CompileAnimationScript$(a$, 0)
    '
    a$ = "play (1, 0,  0, 43,  0,  8,  0,  10,  0,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (3, 0,  0, 69,  0, 12, 10,  290, 2,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Gebäude-Einnahme
       + "play (1, 0,  0, 24,  0, 12, 110, 60,  0,  1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Bildstörung
       + "play (1, 0,  0, 45,  0,  8, 170, 10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Logo Spieler 4 (Weiß)
       + "write (0, "+FORMAT$(%MSG_BUILDINGLOST)+")"+CHR$(13,10) _
       + "fx (50,0,21,001,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,110,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,130,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,150,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_BUILDINGLOST4) = CompileAnimationScript$(a$, 0)
    '
    a$ = "play (1, 0,  0, 43,  0,  8,  0,  10,  0,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (3, 0,  0, 69,  0, 12, 10,  290, 2,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Gebäude-Einnahme
       + "play (1, 0,  0, 24,  0, 12, 110, 60,  0,  1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Bildstörung
       + "play (1, 0,  0, 21,  0,  8, 170, 10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Logo Spieler 5 (Gelb)
       + "write (0, "+FORMAT$(%MSG_BUILDINGLOST)+")"+CHR$(13,10) _
       + "fx (50,0,21,001,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,110,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,130,127,1)"+CHR$(13,10) _
       + "fx (50,0,31,150,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_BUILDINGLOST5) = CompileAnimationScript$(a$, 0)
    '
    a$ = "play (1, 0,  0, 43,  0,  8,  0,  10,  0,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (3, 0,  0, 0,   0, 12, 10,  290, 2,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Büro
       + "play (1, 0,  0, 10,  0,  8, 10,  290, 0,  1,  30, 0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Kommandant
       + "write (0, "+FORMAT$(%MSG_BUILDINGHANDEDOVER)+")"+CHR$(13,10) _
       + "fx (50,0,21,001,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_BUILDINGTOALLY) = CompileAnimationScript$(a$, 0)

    'Art(1=Person,3=Hintergrund),LNA,LNR,Video,PMO,Geschwindigkeit,Startzeit,Länge,EinfrierenNachEnde,Z,X,Y,DTX,DTY,WIL,WIT,WIW,WIH
    a$ = "play (1, 0,  0, 43,  0,  8,  0,  20,  0,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (3, 0,  0, 10,  0, 12, 20,  600, 2,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Hintergrund
       + "play (1, 0,  0, 43,  0,  8, 620, 10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "fx (50,0,21,001,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_COMPUTER) = CompileAnimationScript$(a$, 0)

    a$ = "play (1, 0,  0, 43,   0, 8,   0,  40,   0, 0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (1, 0,  0, 1024, 0, 4,  40, 600,   2, 0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Kitana Logo
       + "play (1, 0,  0, 1025, 0, 7,  40, 600,   0, 1, 54,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Kitana
       + "play (1, 0,  0, 43,   0, 8, 640,  10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "fx (50,0,21,001,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_BRIEFING) = CompileAnimationScript$(a$, 0)

    a$ = "play (1, 0,  0, 43,  0,  8,   0,  20,   0, 0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (3, 0,  0, 27,  0, 12,  20, 300,   2, 0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Hintergrund
       + "play (1, 0,  0, 12,  0,  8,  20, 300,   0, 1, 40,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Nachrichtensprecherin
       + "play (1, 0,  0, 43,  0,  8, 320,  10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "fx (50,0,21,001,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_WEATHER_REPORT) = CompileAnimationScript$(a$, 0)

    a$ = "play (1, 0,  0, 22,  0, 12,  0,  10,  0,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Titan-Net Logo
       + "play (3, 0,  0,  2,  0, 12, 10,  600, 2,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Hintergrund (Achteckige Zentrale)
       + "play (1, 0,  0, 34,  0, 8, 10, 600,  0,  1,  40,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Aisascia
       + "play (1, 0,  0, 22,  0, 12, 620, 10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Titan-Net Logo
       + "fx (50,0,21,001,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_AISASCIA) = CompileAnimationScript$(a$, 0)

    a$ = "play (3, 0,  0,  9,  0, 12, 0,   300, 2,  0,   0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Hintergrund (Brücke)
       + "play (1, 0,  0,  1,  0, 8,  0,   300,  0,  1, 50,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Mol Durag
       + "play (1, 0,  0, 22,  0, 12, 300, 300, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Bildstörung
       + "fx (50,0,21,001,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_MOL_DURAG) = CompileAnimationScript$(a$, 0)

    a$ = "play (1, 0,  0, 43,  0,  8,  0,  20,  0,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (3, 0,  0,  7,  0, 12, 20,  600, 2,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Hintergrund (Militär-Zelt)
       + "play (1, 0,  0, 11,  0, 8, 20, 600,  0,  1,  40,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Beg Beb
       + "play (1, 0,  0, 43,  0,  8, 620, 10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "fx (50,0,21,001,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_MALE) = CompileAnimationScript$(a$, 0)

    a$ = "play (1, 0,  0, 43,  0,  8,  0,  20,  0,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "play (3, 0,  0, 10,  0, 12, 20,  600, 2,  0,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Hintergrund (Computerlabor)
       + "play (1, 0,  0, 41,  0, 8, 20, 600,  0,  1,  40,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'Wissenschaftlerin
       + "play (1, 0,  0, 43,  0,  8, 620, 10, 120, 1,  0,  0,  0,  0,  0,  0, 120, 64)"+CHR$(13,10) _  'ATIKSS Logo
       + "fx (50,0,21,001,127,1)"+CHR$(13,10)
    animationsScripts$(%ANI_FEMALE) = CompileAnimationScript$(a$, 0)
  END IF

  ExtractAnimationScripts& = nAnimationsScripts&
END FUNCTION



'Lädt alle BI2 Animationen
'filenr: 0 = BI2 , 1 = EDT , 2 = KC
FUNCTION LoadAnimations&(filenr&)
  LOCAL a$, i&, animcount&, animlines$()

  'prüfen, ob diese Animation bereits geladen wurde
  IF (cachedAnimations& AND 2^filenr&) <> 0 THEN
    LoadAnimations& = 1
    EXIT FUNCTION
  END IF

  IF UBOUND(hAnimations&) < %MAXANIMATIONS-1 THEN
    REDIM hAnimations&(%MAXANIMATIONS-1), animationWidth&(%MAXANIMATIONS-1), animationHeight&(%MAXANIMATIONS-1), animationFrameCount&(%MAXANIMATIONS-1), animationsScripts$(%MAXANIMATIONS-1)
  END IF

  IF filenr& < 2 THEN
    'Personen
    IF AnimationListToTextureArray&("ANI\APER"+FORMAT$(filenr&, "000")+".LIB", 512*filenr&) = 0 THEN EXIT FUNCTION

    'Hintergründe
    IF AnimationListToTextureArray&("ANI\ABCK"+FORMAT$(filenr&, "000")+".LIB", 512*filenr&+256) = 0 THEN EXIT FUNCTION

    'Skripte
    IF ExtractAnimationScripts&(filenr&) = 0 THEN EXIT FUNCTION
  ELSE
    'benutzerdefinierte Animationen
    a$ = ReadFileContent("GFX\vidanims.txt", 0)
    IF a$ <> "" THEN
      animcount& = TextToArray&(a$, animlines$())
      FOR i& = 0 TO animcount&-1
        a$ = animlines$(i&)
        IF LoadCustomAnimation&(i&+1024, PARSE$(a$, ",", 1), VAL(PARSE$(a$, ",", 2)), VAL(PARSE$(a$, ",", 3)), VAL(PARSE$(a$, ",", 4))) = 0 THEN EXIT FUNCTION
      NEXT i&
    END IF
  END IF

  cachedAnimations& = cachedAnimations& OR 2^filenr&
  LoadAnimations& = 1
END FUNCTION



'Formatiert eine Nachrichtentext
FUNCTION FormatGameMessage(BYVAL a$$) AS WSTRING
  LOCAL startpos&, p&, q&, v&, c&, cmd$$

  'Zeilenumbrüche entfernen, da diese durch DirectX automatisch gesetzt werden
  REPLACE "-^CRT" WITH "" IN a$$
  REPLACE "-^crt" WITH "" IN a$$
  REPLACE "^CRT" WITH " " IN a$$
  REPLACE "^crt" WITH " " IN a$$
  REPLACE CHR$(13,13) WITH CHR$(0) IN a$$  'Absatz
  REPLACE CHR$(13) WITH " " IN a$$

  'Umlaute ersetzen
  REPLACE CHR$(142) WITH "Ä" IN a$$
  REPLACE CHR$(132) WITH "ä" IN a$$
  REPLACE CHR$(153) WITH "Ö" IN a$$
  REPLACE CHR$(148) WITH "ö" IN a$$
  REPLACE CHR$(154) WITH "Ü" IN a$$
  REPLACE CHR$(129) WITH "ü" IN a$$

  'Fonts und Farben ersetzen
  startpos& = 1
  DO
    p& = INSTR(startpos&, a$$, "^")
    IF p& = 0 THEN EXIT LOOP
    cmd$$ = UCASE$(MID$(a$$, p&+1, 3))  'alle Befehle bestehen aus 3 Buchstaben gefolgen von beliebig vielen Ziffern
    q& = p&+4
    WHILE ASC(a$$, q&) >= 48 AND ASC(a$$, q&) <= 57
      q& = q&+1
    WEND
    v& = VAL(MID$(a$$, p&+4, q&-p&-4))
    SELECT CASE cmd$$
    CASE "FON":  'Font (ignorieren)
      a$$ = LEFT$(a$$, p&-1)+MID$(a$$, q&)
      p& = p&-1
    CASE "COL":  'Farbe
      IF v& = 125 OR v& = 254 THEN c& = 2 ELSE c& = 1
      a$$ = LEFT$(a$$, p&-1)+CHR$(c&)+MID$(a$$, q&)
    CASE "VOC":  'Stimme
    CASE "CUT":  'Zwischensequenz
      a$$ = LEFT$(a$$, p&-1)+CHR$(4)+CHR$(v&)+MID$(a$$, q&)
    CASE ELSE
    END SELECT
    startpos& = p&+1
  LOOP

  FormatGameMessage = a$$
END FUNCTION



'Lädt die Nachrichtentexte
'filenr: 0 = BI2 , 1 = EDT
FUNCTION LoadMessages&(filenr&)
  LOCAL i&, nr&, p&, q&, a$, b$, f$, lng$, msg$()

  'Nachrichtentexte laden
  f$ = "\ATXT"+FORMAT$(filenr&, "000")+".LIB"
  nGameMessage& = MIN&(512, ExtractFromLib&(langcode$+f$, msg$()))
  IF nGameMessage& < 1 THEN
    CALL PrintError(words$$(%WORD_GAMEMESSAGES_NOT_FOUND))
    LoadMessages& = 0
    EXIT FUNCTION
  END IF

  'Nachrichten formatieren
  REDIM PRESERVE gameMessages$$(529)  '512 vorgegebene Nachrichten + 16 benutzerdefinierte (aus der Mission) + 2 BI2020 Systemmeldungen
  FOR i& = 0 TO nGameMessage&-1
    b$ = msg$(i&)
    a$ = ""
    q& = 1
    DO
      p& = INSTR(q&, b$, "{")
      IF p& = 0 THEN EXIT LOOP
      q& = INSTR(p&, b$, "}")
      IF q& = 0 THEN EXIT LOOP
      IF a$ <> "" THEN a$ = a$+CHR$(0)
      a$ = a$+MID$(b$, p&+1, q&-p&-1)
    LOOP
    a$ = FormatGameMessage(a$)
    gameMessages$$(i&) = SHRINK$(a$)
  NEXT i&

  'Nachrichten für Verlust eines eigenen Gebäudes hinzufügen
  gameMessages$$(%MSG_BUILDINGLOST) = SHRINK$(FormatGameMessage(words$$(%WORD_MSG_BUILDINGLOST)))
  gameMessages$$(%MSG_BUILDINGHANDEDOVER) = SHRINK$(FormatGameMessage(words$$(%WORD_MSG_BUILDINGHANDEDOVER)))

  LoadMessages& = 1
END FUNCTION



'Lädt die Geräusche
FUNCTION LoadSounds&
  LOCAL i&, h&, nsounds&, samplerate&, channels&
  LOCAL wavedata$, sounds$()

  'Geräusche aus LIB Datei laden (diese enthält VOC Daten)
  nsounds& = ExtractFromLib&(langcode$+"\AFXS000.LIB", sounds$())
  CALL BIDebugLog("Loaded "+FORMAT$(nsounds&)+" sounds from "+langcode$+"\AFXS000.LIB.")
  IF nsounds& <= 0 THEN EXIT FUNCTION
  FOR i& = 0 TO nsounds&-1
    wavedata$ = sounds$(i&)
    CALL DSLoadVocFile&("", wavedata$, samplerate&, channels&)
    h& = DS.AddWaveData(wavedata$, samplerate&, channels&)
    IF i& = 0 THEN hFirstEffect& = h&
  NEXT i&

  '4 parallele Soundchannels erzeugen
  soundchannels(%SOUNDBUFFER_EFFECT1) = DS.CreateSoundChannel(hFirstEffect&)
  soundchannels(%SOUNDBUFFER_EFFECT2) = DS.CreateSoundChannel(hFirstEffect&)
  soundchannels(%SOUNDBUFFER_EFFECT3) = DS.CreateSoundChannel(hFirstEffect&)
  soundchannels(%SOUNDBUFFER_EFFECT4) = DS.CreateSoundChannel(hFirstEffect&)

  soundInitialized& = 1

  LoadSounds& = 1
END FUNCTION



'Ermittelt alle Musik-Dateien
FUNCTION LoadMusic&
  LOCAL f$, p$, n&
  DIM musicfiles$(255)

  'alle Dateien im Ordner MUS ermitteln
  p$ = EXEPATH$+"MUS\"
  f$ = DIR$(p$+"*")
  WHILE f$ <> "" AND n& < 256
    musicfiles$(n&) = "MUS\"+f$
    n& = n&+1
    f$ = DIR$
  WEND

  'falls keine Dateien gefunden wurden, dann Standard-Dateien aus dem Hauptordner nehmen
  IF n& = 0 THEN
    f$ = DIR$(EXEPATH$+"BI2*.OGG")
    WHILE f$ <> "" AND n& < 256
      musicfiles$(n&) = f$
      n& = n&+1
      f$ = DIR$
    WEND
  END IF

  'Dateien sortieren
  IF n& > 0 THEN ARRAY SORT musicfiles$(0) FOR n&
  CALL BIDebugLog("Found "+FORMAT$(n&)+" music files.")

  LoadMusic& = n&
END FUNCTION



'Prüft, ob der offizielle Server eine höhere Version hat als der Client
SUB CheckForUpdates
  updateCheck& = -1
  CALL ConnectToServer&(StringToIP&($HIGHSCORESERVER))
END SUB



'Lädt die Highscore Tabelle (nur Server)
SUB LoadHighscore
  LOCAL a$

  a$ = ReadFileContent$($HIGHSCOREFILE, 0)
  highscoreSize& = LEN(a$)/SIZEOF(THighScore)
  DIM highscoreTable(MAX&(0, highscoreSize&-1))
  POKE$ VARPTR(highscoreTable(0)), a$
END SUB



'Farbschema passend zur Episode setzen
SUB SetColorSchema(episodes&)
  IF gameMode& = %GAMEMODE_SERVER THEN EXIT SUB
  SELECT CASE episodes&
  CASE 5 TO 9
    brushPlayer&(0) = D2D.CreateSolidBrush(128, 128, 160)
    playerColors$ = $playercolorsbi3
  CASE ELSE
    brushPlayer&(0) = D2D.CreateSolidBrush(128, 192, 255)
    playerColors$ = $playercolorsbi2
  END SELECT
END SUB



'Standardwerte festlegen
SUB InitDefaults
  'Standardspielername
  defaultPlayernames$(0) = "Val Haris"
  defaultPlayernames$(1) = "Titan-Net"
  defaultPlayernames$(2) = "Rebels"
  defaultPlayernames$(3) = "Partisans"
  defaultPlayernames$(4) = "Kai"
  defaultPlayernames$(5) = "Guardians"
END SUB



'Erstellt alle Fonts
FUNCTION InitFonts&
  'Fonts, die mit der Fenstergröße skalieren
  hSystemFont& = D2D.CreateFont("Arial", 0, 9*uiscale!)
  hWeaponFont& = D2D.CreateFont("Arial", 1, 10*uiscale!)
  hSmallWeaponFont& = D2D.CreateFont("Arial", 1, 9*uiscale!)
  hCaptionFont& = D2D.CreateFont("Arial", 1, 16*uiscale!)
  hBigCaptionFont& = D2D.CreateFont("Arial", 1, 60*uiscale!)
  hCreditFont& = D2D.CreateFont("Arial", 1, 50*uiscale!)
  hShopCaptionFont& = D2D.CreateFont("Arial", 1, 20*uiscale!)
  hHallOfFameCaptionFont& = D2D.CreateFont("Arial", 1, 28*uiscale!)
  hGameMessageFont& = D2D.CreateFont("Arial", 0, 13*uiscale!)
  hMenuFont& = D2D.CreateFont("Arial", 0, 15*uiscale!)
  hLobbyCaptionFont& = D2D.CreateFont("Arial", 1, 11*uiscale!)

  InitFonts& = hSystemFont& >= 0 AND hWeaponFont& >= 0 AND hSmallWeaponFont& >= 0 AND hCaptionFont& >= 0 AND hBigCaptionFont& >= 0 AND hCreditFont& >= 0 AND hShopCaptionFont& >= 0 _
              AND hHallOfFameCaptionFont& >= 0 AND hGameMessageFont& >= 0 AND hMenuFont& >= 0 AND hLobbyCaptionFont& >= 0
END FUNCTION



'Erstellt alle Pinsel
FUNCTION InitBrushes&
  'Standardpinsel
  brushBlack& = D2D.CreateSolidBrush(0, 0, 0)
  brushWhite& = D2D.CreateSolidBrush(255, 255, 255)
  brushLightGrey& = D2D.CreateSolidBrush(192, 192, 192)
  brushRed& = D2D.CreateSolidBrush(255, 0, 0)
  brushDarkRed& = D2D.CreateSolidBrush(128, 0, 0)
  brushBronze& = D2D.CreateSolidBrush(224, 128, 32)
  brushSilver& = D2D.CreateSolidBrush(208, 208, 208)
  brushGold& = D2D.CreateSolidBrush(255, 192, 64)
  brushBlue& = D2D.CreateSolidBrush(0, 0, 255)
  IF brushBlack& < 0 OR brushWhite& < 0 OR brushLightGrey& < 0 OR brushRed& < 0 OR brushDarkRed& < 0 OR brushBronze& < 0 OR brushSilver& < 0 OR brushGold& < 0 OR brushBlue& < 0 THEN EXIT FUNCTION
  brushBlueTransparent& = D2D.CreateSolidBrush(0, 0, 255, 128)
  brushGoldTransparent& = D2D.CreateSolidBrush(255, 192, 64, 128)
  brushBlack50& = D2D.CreateSolidBrush(0, 0, 0, 128)
  IF brushBlueTransparent& < 0 OR brushGoldTransparent& < 0 OR brushBlack50& < 0 THEN EXIT FUNCTION

  'Menüpinsel
  brushMenuBackground& = D2D.CreateSolidBrush(64, 64, 64, 128)
  brushMenuHighlight& = D2D.CreateSolidBrush(255, 255, 255, 128)
  brushMenuBorder& = D2D.CreateSolidBrush(32, 32, 32)
  IF brushMenuBackground& < 0 OR brushMenuHighlight& < 0 OR brushMenuBorder& < 0 THEN EXIT FUNCTION

  'Controlpinsel
  brushButtonBackground& = D2D.CreateSolidBrush(101, 81, 69)
  brushButtonBorder& = D2D.CreateSolidBrush(154, 130, 117)
  brushButtonShadow& = D2D.CreateSolidBrush(77, 65, 58)
  IF brushButtonBackground& < 0 OR brushButtonBorder& < 0 OR brushButtonShadow& < 0 THEN EXIT FUNCTION

  'Spielpinsel
  brushUnexplored& = D2D.CreateSolidBrush(70, 19, 13)
  IF brushUnexplored& < 0 THEN EXIT FUNCTION

  'Spielerfarben
  REDIM brushPlayer&(6)
  brushPlayer&(0) = D2D.CreateSolidBrush(128, 192, 255)
  brushPlayer&(1) = D2D.CreateSolidBrush(255, 128, 128)
  brushPlayer&(2) = D2D.CreateSolidBrush(255, 128, 0)
  brushPlayer&(3) = D2D.CreateSolidBrush(192, 255, 64)
  brushPlayer&(4) = D2D.CreateSolidBrush(255, 255, 192)
  brushPlayer&(5) = D2D.CreateSolidBrush(255, 255, 0)
  brushPlayer&(6) = D2D.CreateSolidBrush(255, 192, 192)
  IF brushPlayer&(0) < 0 OR brushPlayer&(1) < 0 OR brushPlayer&(2) < 0 OR brushPlayer&(3) < 0 OR brushPlayer&(4) < 0 OR brushPlayer&(5) < 0 OR brushPlayer&(6) < 0 THEN EXIT FUNCTION

  InitBrushes& = -1
END FUNCTION



'Controls erzeugen
SUB InitControls
  LOCAL bleft&, bwd&, bhg&, btop&

  'HUD-Buttons
  bleft& = buttonarea.left
  btop& = INT(buttonarea.top+4*uiscale!)
  bwd& = 40*uiscale!
  bhg& = 46*uiscale!
  D2D.SetBitmapButtonTexture(hButtons&)
  buttonMapInfo = CLASS "DXCONTROL"
  buttonMapInfo.InitBitmapButton(D2D, "", INT(bleft&+0*uiscale!), btop&, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 0, 40, 46, 0, CODEPTR(MapInfoButtonPressed), 1)
  buttonLoadGame = CLASS "DXCONTROL"
  buttonLoadGame.InitBitmapButton(D2D, "", INT(bleft&+40*uiscale!), btop&, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 92, 40, 46, 0, CODEPTR(LoadButtonPressed), 1)
  buttonSaveGame = CLASS "DXCONTROL"
  buttonSaveGame.InitBitmapButton(D2D, "", INT(bleft&+80*uiscale!), btop&, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 46, 40, 46, 0, CODEPTR(SaveButtonPressed), 1)
  buttonMusic = CLASS "DXCONTROL"
  buttonMusic.InitBitmapButton(D2D, "", INT(bleft&+120*uiscale!), btop&, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 138, 40, 46, 0, CODEPTR(MusicButtonPressed), 1)
  buttonProtocol = CLASS "DXCONTROL"
  buttonProtocol.InitBitmapButton(D2D, "", INT(bleft&+160*uiscale!), btop&, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 184, 40, 46, 0, CODEPTR(ProtocolButtonPressed), 1)
  buttonHighscore = CLASS "DXCONTROL"
  buttonHighscore.InitBitmapButton(D2D, "", INT(bleft&+200*uiscale!), btop&, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 230, 40, 46, 0, CODEPTR(ScoreButtonPressed), 1)
  buttonEndTurn = CLASS "DXCONTROL"
  buttonEndTurn.InitBitmapButton(D2D, "", INT(bleft&+240*uiscale!), btop&, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 276, 40, 46, 0, CODEPTR(EndTurnButtonPressed), 1)
  buttonEndTurn.Enabled = 0
  buttonOpenMenu = CLASS "DXCONTROL"
  buttonOpenMenu.InitBitmapButton(D2D, "", INT(bleft&+280*uiscale!), btop&, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 322, 40, 46, 0, CODEPTR(MenuButtonPressed), 1)

  'Shop-Buttons
  buttonShopMove = CLASS "DXCONTROL"
  buttonShopMove.InitBitmapButton(D2D, "", 0, 0, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 368, 40, 46, 0, CODEPTR(MoveButtonPressed), 0)
  buttonShopRefuel = CLASS "DXCONTROL"
  buttonShopRefuel.InitBitmapButton(D2D, "", 0, 0, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 414, 40, 46, 0, CODEPTR(RefuelButtonPressed), 0)
  buttonShopRepair = CLASS "DXCONTROL"
  buttonShopRepair.InitBitmapButton(D2D, "", 0, 0, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 460, 40, 46, 0, CODEPTR(RepairButtonPressed), 0)
  buttonShopBuild = CLASS "DXCONTROL"
  buttonShopBuild.InitBitmapButton(D2D, "", 0, 0, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 506, 40, 46, 0, CODEPTR(BuildButtonPressed), 0)
  buttonShopTrain = CLASS "DXCONTROL"
  buttonShopTrain.InitBitmapButton(D2D, "", 0, 0, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 552, 40, 46, 0, CODEPTR(TrainButtonPressed), 0)

  'Chat
  buttonChatTeam = CLASS "DXCONTROL"
  buttonChatTeam.InitBitmapButton(D2D, "", 1140*uiscale!, 1020*uiscale!, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 598, 40, 46, 0, CODEPTR(ChatToTeam), 0)
  buttonChatAll = CLASS "DXCONTROL"
  buttonChatAll.InitBitmapButton(D2D, "", 1180*uiscale!, 1020*uiscale!, bwd&, bhg&, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 644, 40, 46, 0, CODEPTR(ChatToAll), 0)
  editChat = CLASS "DXCONTROL"
  editChat.InitEdit(D2D, "", "", 24*uiscale!, 1030*uiscale!, 1100*uiscale!, 26*uiscale!, 80, hGameMessageFont&, brushWhite&, brushMenuBorder&, brushButtonShadow&, 0)

  'Close Button
  buttonClose = CLASS "DXCONTROL"
  buttonClose.InitBitmapButton(D2D, "", 0, 0, 71*uiscale!, 67*uiscale!, hSystemFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 691, 71, 67, 0, CODEPTR(CloseButtonPressed), 0)

  'Fortschrittsbalken
  progressbar = CLASS "DXCONTROL"
  progressbar.InitProgressbar(D2D, 100, words$$(%WORD_WELCOME_TO_BATTLEISLE), 24*uiscale!, 1030*uiscale!, windowWidth&-416*uiscale!, 26*uiscale!, hCaptionFont&, brushWhite&, brushMenuBorder&, brushMenuBackground&, brushPlayer&(1), 1)

  'Scrollbalken für Protokoll
  protocolScrollbar = CLASS "DXCONTROL"
  protocolScrollbar.InitScrollbar(D2D, 100, 50, 0, 0, 16, 200, brushMenuBorder&, brushMenuBackground&, brushPlayer&(1), 0)

  'Eingabefeld für Missions-Code
  editMissionCode = CLASS "DXCONTROL"
  editMissionCode.InitEdit(D2D, "", "", 0, 0, 484, 30, 7, hMenuFont&, brushWhite&, brushMenuBorder&, brushButtonShadow&, 0)
  editMissionCode.EditUppercase = 1

  'Eingabefeld für Spielernamen
  editPlayername = CLASS "DXCONTROL"
  editPlayername.InitEdit(D2D, "", "", 0, 0, 484, 30, 16, hMenuFont&, brushWhite&, brushMenuBorder&, brushButtonShadow&, 0)

  'Lobby
  bwd& = 132*uiscale!
  bhg& = 40*uiscale!
  editServerIP = CLASS "DXCONTROL"
  editServerIP.InitEdit(D2D, "", "", 0, 0, 110*uiscale!, 24*uiscale!, 20, hLobbyCaptionFont&, brushWhite&, brushMenuBorder&, brushButtonShadow&, 0)
  editGameName = CLASS "DXCONTROL"
  editGameName.InitEdit(D2D, "", "", 0, 0, 415*uiscale!, 24*uiscale!, 32, hLobbyCaptionFont&, brushWhite&, brushMenuBorder&, brushButtonShadow&, 0)
  buttonConnect = CLASS "DXCONTROL"
  buttonConnect.InitBitmapButton(D2D, words$$(%WORD_CONNECT), 0, 0, bwd&, bhg&, hLobbyCaptionFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 759, 132, 40, 0, CODEPTR(ConnectButtonPressed), 0)
  buttonCreateGame = CLASS "DXCONTROL"
  buttonCreateGame.InitBitmapButton(D2D, words$$(%WORD_CREATE), 0, 0, bwd&, bhg&, hLobbyCaptionFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 759, 132, 40, 0, CODEPTR(CreateGameButtonPressed), 0)
  buttonJoinGame = CLASS "DXCONTROL"
  buttonJoinGame.InitBitmapButton(D2D, words$$(%WORD_JOIN), 0, 0, bwd&, bhg&, hLobbyCaptionFont&, brushWhite&, brushButtonBorder&, brushButtonShadow&, brushButtonBackground&, brushMenuHighlight&, 759, 132, 40, 0, CODEPTR(JoinGameButtonPressed), 0)
  buttonChangeColor = CLASS "DXCONTROL"
  buttonChangeColor.InitBitmapButton(D2D, words$$(%WORD_CHANGECOLOR), 0, 0, bwd&, bhg&, hLobbyCaptionFont&, brushWhite&, brushWhite&, brushWhite&, brushWhite&, brushWhite&, 759, 132, 40, 0, CODEPTR(ChangeColorButtonPressed), 0)
END SUB



'Bereich definieren
SUB InitArea(area AS RECT, x0!, y0!, x1!, y1!)
  area.left = x0!
  area.top = y0!
  area.right = x1!
  area.bottom = y1!
END SUB



'Bildschirmmaske definieren
SUB InitAreas
  LOCAL i&

  'Kartenbereich
  CALL InitArea(maparea, 24*uiscale!, 24*uiscale!, windowWidth&-392*uiscale!, 962*uiscale!)  'Kartenbereich
  CALL InitArea(buttonarea, windowWidth&-344*uiscale!, 8*uiscale!, windowWidth&-24*uiscale!, 54*uiscale!)  'Buttons
  CALL InitArea(minimaparea, windowWidth&-344*uiscale!, 90*uiscale!, windowWidth&-24*uiscale!, 410*uiscale!)  'Minikarte
  CALL InitArea(unitpicarea, windowWidth&-344*uiscale!, 458*uiscale!, windowWidth&-24*uiscale!, 698*uiscale!)  'Einheitenbild
  CALL InitArea(unitinfoarea, windowWidth&-344*uiscale!, 746*uiscale!, windowWidth&-24*uiscale!, 846*uiscale! )  'Einheiteninfos
  CALL InitArea(messagearea, windowWidth&-344*uiscale!, 894*uiscale!, windowWidth&-24*uiscale!, 1056*uiscale!)  'Meldungen

  'Texturbereiche
  CALL InitArea(txarea_menu, 0, 0, 720, 816)  'Hauptmenü
  CALL InitArea(txarea_combat1, 722, 0, 1744, 156)  'kleine Kampfansicht
  CALL InitArea(txarea_highscore, 722, 158, 1854, 607)  'Highscore
  CALL InitArea(txarea_msg, 1856, 0, 2659, 704)  'Spiel-Nachrichten
  CALL InitArea(txarea_shop1, 0, 852, 803, 1491)  'Shop ohne Produktionsmenü
  CALL InitArea(txarea_shop2, 805, 852, 1608, 1491)  'Shop mit Produktionsmenü
  CALL InitArea(txarea_shopenergy, 1610, 852, 1740, 1160)  'Shop Energie/Material Seitenbereich
  CALL InitArea(txarea_playercolors, 1610, 1162, 1660, 1462)  'Farben der 6 Spieler
  CALL InitArea(txarea_prodslot, 1662, 1162, 1740, 1240)  'Produktionsslot Highlight
  CALL InitArea(txarea_energyicon, 1662, 1242, 1702, 1282)  'Energie Icon
  CALL InitArea(txarea_materialicon, 1662, 1282, 1702, 1322)  'Material Icon
  CALL InitArea(txarea_mapinfo, 1742, 706, 2545, 1384)  'Karten-Info
  CALL InitArea(txarea_mapinfoplayer, 722, 609, 1465, 640)  'Spielerhintergrund (Karten-Info)
  CALL InitArea(txarea_combat2, 0, 1493, 876, 1969)  'große Kampfansicht
  CALL InitArea(txarea_mplobby, 878, 1493, 1884, 1963)  'Multiplayer-Lobby
  CALL InitArea(txarea_blankheader, 1886, 1443, 2469, 1547)  'leerer Header
  CALL InitArea(txarea_milopheader, 1886, 1549, 2469, 1653)  'MILOP Header
  CALL InitArea(txarea_blankfooter, 1886, 1655, 2469, 1757)  'leerer Footer
  CALL InitArea(txarea_menuitem, 722, 642, 1276, 702)  'Menüeintrag normal
  CALL InitArea(txarea_menuitempressed, 722, 704, 1276, 764)  'Menüeintrag normal
  CALL InitArea(txarea_menuitemhighlight, 722, 766, 1276, 826)  'Menüeintrag normal
  CALL InitArea(txarea_roundbox, 1278, 643, 1623, 701)  'Kasten mit abgerundeten Ecken
  CALL InitArea(txarea_stars, 1278, 703, 1510, 746)  '5 Sterne
  CALL InitArea(txarea_starborder, 1278, 748, 1510, 791)  '5 Sterne
  CALL InitArea(txarea_gradiantblueblack, 2548, 707, 2578, 1217)  'Farbverlauf Blau nach Schwarz

  'Texturbereiche (zusätzliche Dialog-Elemente)
  CALL InitArea(txarea_wheel_transparent, 0, 0, 848, 849)  'Speichenrad für Mapinfo
  CALL InitArea(txarea_videomsg, 850, 0, 1742, 609)  'Video-Nachrichten

  'Texturbereiche (Intro)
  CALL InitArea(txarea_introbattleisle, 0, 2050, 980, 2186)  'Battle Isle
  CALL InitArea(txarea_intro2020, 0, 2188, 100, 2318)  '2020 (nur erste Ziffer)
  CALL InitArea(txarea_introemblem, 982, 2050, 1962, 2390)  'Emblem
  CALL InitArea(txarea_introhighlight, 1964, 2050, 2044, 2130)  'Lichtreflektion
END SUB



'Grafik-Fenster erzeugen
SUB CREATEWIN
  LOCAL hDC&, winstyle&
  LOCAL RC AS RECT

  'Fenster mittig auf Desktop erzeugen
  IF fullscreenMode& = 1 OR windowWidth& < 800 OR windowHeight& < 600 THEN
    GetClientRect(GetDesktopWindow(), RC)
    windowWidth& = RC.Right-RC.Left
    windowHeight& = RC.Bottom-RC.Top
    winstyle& = %WS_POPUP
    fullscreenMode& = 1
  END IF
  pWindow = CLASS "CWindow"
  IF ISNOTHING(pWindow) THEN EXIT SUB
  hWIN& = pWindow.CreateWindow(%NULL, $WINDOWTITLE, 0, 0, 0, 0, winstyle&, 0, CODEPTR(WindowProc))
  pWindow.ClassStyle = %CS_DBLCLKS
  pWindow.SetClientSize windowWidth&, windowHeight&
  IF fullscreenMode& = 0 THEN pWindow.CenterWindow
  uiscale! = windowHeight&/1080
END SUB



'Client-Update-Thread
THREAD FUNCTION ClientUpdateThread&(BYVAL dummy&)
  CALL CheckForUpdates
END FUNCTION



'Initialisierungs-Thread
THREAD FUNCTION GameInitThread&(BYVAL md&)
  'Artwork aller Einheiten laden
  progressbar.MaxProgress = 200
  progressbar.HighlightColor = brushPlayer&(localPlayerNr&)
'  initProgressText$ = words$(%WORD_LOADING_ARTWORKS)
'  CALL LoadArtworks

  'Animationen laden
  initProgressText$$ = words$$(%WORD_LOADING_ANIMATIONS)
  initProgress& = 0
  IF LoadAnimations&(0) = 0 THEN checkInstallation& = 1
  IF LoadAnimations&(1) = 0 THEN checkInstallation& = 1
  CALL LoadAnimations&(2)

  'Initialisierung abgeschlossen
  initProgressText$$ = words$$(%WORD_WELCOME_TO_BATTLEISLE)
  initProgress& = progressbar.MaxProgress
  initDone& = 1
END FUNCTION
