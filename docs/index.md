# I2P Chat AKA I2P Qt Messenger

## Screenshots

![screenshot-roster](https://vituperative.github.io/i2pchat/screenshots/main.png) ![screenshot-chat](https://vituperative.github.io/i2pchat/screenshots/chat.png)

## Downloads (pre-built binaries)

* Latest Windows (Win32/64) build from: <a href="https://ci.appveyor.com/project/wipedlifepotato/i2pchat/build/artifacts">https://ci.appveyor.com/project/wipedlifepotato/i2pchat/build/artifacts</a>

## Features

 * The communication goes «directly over i2p» from client to client, no server is required.[1] Note: The connections to the so-called «seedless» servers were removed from this project ; see : https://github.com/hypnosis-i2p/i2pchat/issues/40 . —hypn.

## How to run it

You need to enable the SAM application bridge in your router on <a href="http://127.0.0.1:7657/configclients">Java I2P configclients page</a> or i2pd's i2pd.conf [SAM] section to make I2P Chat work over your I2P router.

## Project status, news and history

### Project status

Now the development was resumed by Hypnosis-i2p and R4SAS, with a lot more folks as testers &amp; bugs+issues reporters.

### Current news

* June, 2020:
   * Fixed crash of close chat window
   * Fixed crash of url link in chat
   * Added $HOME/.i2pchat/ directory support for using from /usr/bin
   * Pre inited optarg
   * Core changes.
   * Ci pre-inited. Works for windows now.
   * Created .deb package for ubuntu/debian x86_64
   * Created Windows build for 32 bit, which will works on 64 bits.
   * Design changes.
   * Fix offline message crash
* June, 2020: dr\|z3d starts work on renovating the user interface.
* 5 Jan, 2017: Original repo at http://git.repo.i2p/w/I2P-Messenger-QT.git was fully merged here.

### History

Original developer of this messenger went away.

## License

Licensed under GPLv2.

### Build instructions

 * Install prerequisites:
```
sudo apt-get install qt5-qmake qt5-default build-essential libqt5multimedia5 qtmultimedia5-dev libqt5svg5-dev
```
 * Run qmake for either release:
```
qmake I2P-Messenger.pro "CONFIG += release"
```
or for debug:
```
qmake I2P-Messenger.pro "CONFIG += debug"
```
 * And after that,
```
make -j NUMBER_OF_PROCESSOR_CORES
```
or simply
```
make
```

## Running

On Linux, `make` creates `I2P-Messenger` executable in the current folder. Run it with `./I2P-Messenger`.

* Before going online, check the network settings, and for best results, set the Signature Type to ECDSA_SHA512_P521
* Select 'Online' from the drodown menu on the main window. When you first go online, your unique address (Destination) will be created when connecting to SAM
* Your settings and contacts will be stored in ~/.i2pchat/ on Linux-based systems, or %APPDATA%\Roaming\I2P-Messenger\ on Windows
