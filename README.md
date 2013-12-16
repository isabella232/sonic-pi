## Sonic Pi

This is the source for the Sonic Pi music programming environment. The dependencies for this are:

* supercollider
* ruby1.9.1
* libqscintilla2-8

Compilation
-----------

Build dependencies:

* g++
* make
* libqscintilla2
* qt

To install dependencies for Debian-based systems:
```
sudo apt-get install g++ make libqt4-dev libqscintilla2-dev
```

Or for Red Hat-based:
```
sudo yum install g++ make qt-devel qscintilla-devel
```

To compile:
```
qmake ./app/gui/make-music.pro
make
```

Running
-------

Initially run with
```
./bin/make-music
```
to create directories, setup the workspaces and position the window.

However, if the window positioning is not required, then the program can subsequently be run directly with

```
./make-music
```

File System Structure
---------------------

`./app/gui/` contains the main QT program source files.
`./app/scripts/` contains the scripts to be called when a song is played. They define things such as the `play`, `play_chord` and `play_pattern` functions.

Notes
-----

The current implementation assumes the execution context is a Raspberry Pi. Patches for other platforms will be happily considered.
