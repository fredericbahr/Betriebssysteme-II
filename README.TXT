Aufgabenstellung
================

Insgesamt sollen Sie die Lösung so erweitern, dass Sie mit Ihren Kommilitonen Link-State-Pakete (Textformat) per UDP austauschen können.
Die Steuerung des Programms soll über eine MSG-Queue oder über einen UNIX-Domain-Socket erfolgen. Sie sollen also die Vorlage erweitern und zudem ein Steuerungsprogramm implementieren, mit dem Sie festlegen mit welchen Nachbarn (IP-Adresse und Port) Sie einen LINK aufbauen und darüber Link-State-Pakete (LSP) austauschen. Aus dieser Information entsteht zugleich Ihr eigenes LSP.
Der Aufbau der LSP sei wie folgt
Zeile 1: ipadresse <-- eigene IP-Adresse
Zeile 2-n: ipadresse <-- IP-Adresse des Nachbarn

== Beispiel fuer simson01 ==
141.57.9.22
141.57.9.23
141.57.9.24

STATUS des Programms
====================
übersetzen mit: make
ausführen: ./smile

Standardmäßig öffnet das Programm einen UDP und einen TCP Socket jeweils an Port 24473.
Den TCP-Socket können Sie mit dem Browser (https://127.0.0.1:24473) anfragen oder per "nc 127.0.0.1 24473".
Das Programm fungiert gegenwärtig als UDP-Echo-Server. Jede Nachricht die Sie per "ns -u 127.0.0.1 24473" senden, wird an Sie zurück gegeben. Außerdem wird die letzte Nachricht gespeichert und kann nun auch per TCP abgerufen werden.

Wenn Sie das Programm per STRG-C oder kill <ProzessID> beenden, werden alle offenen Sockets geschlossen und der Speicher vollständig freigegeben. Das soll auch in Ihrer Lösung so sein.

VORGEHEN
========
Drei Vorlesungswochen(RN+BS) Zeit für
1. Auseinandersetzung mit dem vorhandenen Quellcode
2. Implementierung Steuerungsprogramm
3. Implementierung Link-State-Fluten und optional Berechnung und Ausgabe der Topologie im Browser

Abgabe KW 3
Verteidigung KW4/5

== HINWEIS ==
Sie können mehrere Instanzen des Programm auf Ihrem Rechner laufen lassen. Die müssen an der Konsole jeweils mit anderen Portnummer starten. So können Sie Ihr "eigenes Netz" aufbauen.

Literaturhinweis und Quelle der Inspiration
===========================================
http://openbook.rheinwerk-verlag.de/linux_unix_programmierung/Kap09-000.htm

Prüfen auf SPEICHERLECKS mit VALGRIND
=====================================
jeanm@jeanm 1x_ls]$ valgrind ./smile 
==132111== Memcheck, a memory error detector
==132111== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==132111== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==132111== Command: ./smile
==132111== 
Daemon gestartet. PID: 132111
^Cint
Daemon gestoppt
GOING 2 FREE MEMORY and FILES graceful!
closing web_server_socket
closing udp_peer_socket
reached end of cleanup
==132111== 
==132111== HEAP SUMMARY:
==132111==     in use at exit: 0 bytes in 0 blocks
==132111==   total heap usage: 1 allocs, 1 frees, 1,024 bytes allocated
==132111== 
==132111== All heap blocks were freed -- no leaks are possible
==132111== 
==132111== For lists of detected and suppressed errors, rerun with: -s
==132111== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)


DATEIEN
=======
── cleanup.c
├── daemon
│   ├── daemon.c
│   ├── daemon.h
│   ├── Makefile
│   └── template.c
├── init.c
├── Makefile
├── project.h
├── README.TXT
├── signale
│   ├── Makefile
│   ├── signale.c
│   ├── signale.h
│   └── test.c
├── template.c
└── worker.c

