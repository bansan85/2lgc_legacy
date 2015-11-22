Télécharger msys2 pour application 64 bits.
http://downloads.sourceforge.net/project/msys2/Base/x86_64/msys2-x86_64-20150512.exe
Laisser le dossier d'installation par défaut C:\mingw64

Exécuter msys
msys2_shell.bat

Et la commande suivante :
pacman --needed -Sy bash pacman pacman-mirrors msys2-runtime

Fermer Bash en fermant la croix de la fenêtre (CTRL+D ou exit peuvent échouer) et relancer msys2_shell.bat

Et les commandes suivantes :
pacman -S mingw-w64-i686-suitesparse mingw-w64-i686-gtkmm3 mingw-w64-x86_64-suitesparse mingw-w64-x86_64-gtkmm3 automake1.15 autoconf libtool gcc make libiconv-devel mingw-w64-i686-toolchain mingw-w64-x86_64-toolchain 

Enfin, le projet est maintenant utilisable en lançant le fichier 2lgc.sln.

libeurocode et libprojet présentant une dépendance circulaire, il est
nécessaire de réaliser les opérations de compilation suivante dans l'ordre :
01 libundo (génère .dll et .lib)
02 libeurocode (génère .lib)
03 libprojet (génère .dll et .lib)
04 libeurocode (génère .dll et .lib)
10 codegui (génère codegui)
