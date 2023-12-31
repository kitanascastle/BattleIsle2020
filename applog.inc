'Schreibt einen Eintrag in ein Logbuch
'AppName = Name der Anwendung
'F       = Dateiname des Logbuchs (inklusive Pfad)
'A       = zu schreibender Logeintrag
SUB APPLOG(APPNAME$,F$,A$)
  LOCAL D$,T$,E$
  LOCAL NR&

  'Datum und Uhrzeit ermitteln
  D$=DATE$
  T$=TIME$

  'Eintrag zusammensetzen
  E$=D$+"/"+T$+" "+APPNAME$+"   "+A$

  'Eintrag an Datei anhängen
  NR&=FREEFILE
  OPEN F$ FOR APPEND AS NR&
  PRINT# NR&,E$
  CLOSE NR&
END SUB



FUNCTION APPALLOCSHAREDMEMORY&(MEMNAME$,L&)
END FUNCTION



SUB APPFREESHAREDMEMORY(PMEM&)
END SUB



FUNCTION APPTIME???
END FUNCTION



SUB REGISTERSERVERAPP(APPNAME$,APPINFO$)
END SUB



SUB UNREGISTERSERVERAPP
END SUB



SUB APPHEARTBEAT
END SUB



FUNCTION APPGETALLENTRIES$
END FUNCTION



SUB REMOVESERVERAPP(BYVAL APPNR&)
END SUB



SUB APPSENDMESSAGE(MSG$, APPNR&)
END SUB



FUNCTION APPREADMESSAGE$(APPNR&)
END FUNCTION



FUNCTION APPREADMESSAGEWITHTIMEOUT$(APPNR&, TIMEOUTMS&)
END FUNCTION
