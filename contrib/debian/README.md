
Debian
====================
This directory contains files used to package straksd/straks-qt
for Debian-based Linux systems. If you compile straksd/straks-qt yourself, there are some useful files here.

## straks: URI support ##


straks-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install straks-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your straks-qt binary to `/usr/bin`
and the `../../share/pixmaps/straks128.png` to `/usr/share/pixmaps`

straks-qt.protocol (KDE)

