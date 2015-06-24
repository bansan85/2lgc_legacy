Télécharger msys2 pour application 32 bits.
http://sourceforge.net/projects/msys2/files/Base/i686/msys2-i686-20150512.exe/download
Laisser le dossier d'installation par défaut C:\mingw32

Exécuter msys
mingw32_shell.bat

Et la commande suivante :
pacman --needed -Sy bash pacman pacman-mirrors msys2-runtime

Fermer Bash et relancer mingw32_shell.bat

Et les commandes suivantes :
pacman -S mingw-w64-i686-suitesparse mingw-w64-i686-gtkmm3 automake1.15 autoconf libtool gcc make libiconv-devel

Enfin, le projet est maintenant utilisable en lançant le fichier 2lgc.sln.

libeurocode et libprojet présentant une dépendance circulaire, il est
nécessaire de réaliser les opérations de compilation suivante dans l'ordre :
1 libundo (génère .dll et .lib)
2 libprojet (génère .lib)
3 libeurocode (génère .dll et .lib)
4 libprojet (génère .dll et .lib)
5 codegui (génère codegui)
