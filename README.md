# I2P Chat AKA I2P Qt Messenger
[![Build status appveyor MinGW32](https://ci.appveyor.com/api/projects/status/0tanjnojnlpksug6?svg=true)](https://ci.appveyor.com/project/wipedlifepotato/i2pchat)
[![Build Status travis linux(xenial)](https://travis-ci.org/wipedlifepotato/i2pchat.svg?branch=master)](https://travis-ci.org/wipedlifepotato/i2pchat)
## Screenshots

![screenshot-roster](https://vituperative.github.io/i2pchat/screenshots/main.png) ![screenshot-chat](https://vituperative.github.io/i2pchat/screenshots/chat.png)


## Features

 * The communication goes «directly over i2p» from client to client, no server is required.[1] Note: The connections to the so-called «seedless» servers were removed from this project ; see : https://github.com/hypnosis-i2p/i2pchat/issues/40 . —hypn.

## How to run it

You need to enable SAM in your router on <a href="http://127.0.0.1:7657/configclients">java i2p configclients page</a> or i2pd's i2pd.conf [sam] section to make I2P Chat work over your I2P router.

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
***
* June, 2020: dr\|z3d starts work on renovating the user interface.
***
* 5 Jan, 2017: Original repo at http://git.repo.i2p/w/I2P-Messenger-QT.git was fully merged here.
### History

Original developer of this messenger went away.

## License

The license of this software is GPLv2.

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
### Download Alpha Version
 #### Linux:
 [Debian/Ubuntu deb for x86_64(old)](https://github.com/wipedlifepotato/i2pchat/releases/download/alpha0.1/i2p-messenger_amd64.deb)
 #### Windows:
 [WINDOWS32 will works on 64bit too](https://ci.appveyor.com/project/wipedlifepotato/i2pchat/build/artifacts)

## Running

On Linux, `make` creates `I2P-Messenger` executable in the current folder. Run it with `./I2P-Messenger`. When ran, switch yourself to online. It will generate your Destination address (a key) on first connect to SAM.

## Old files and sites

 * http://echelon.i2p/qti2pmessenger/macos → Mac OS X versions, not tested;
 * http://echelon.i2p/qti2pmessenger/ → old eche|on's site for I2P Chat (versions up to 0.2.24);
 * https://launchpad.net/~i2p.packages/+archive/ubuntu/i2p/+sourcepub/4824794/+listing-archive-extra → Kill Your TV's PPA (v.0.2.25).
 * http://git.repo.i2p/w/I2P-Messenger-QT.git/shortlog → original repo (fully merged into this /i2pchat)

## Forum thread (English)

http://forum.i2p/viewtopic.php?t=2474

## List of relevant repositories (in Russian)

я два репа на гитхабе создал
 * один для антиквариата по части и2п массажника https://github.com/hypnosis-i2p/I2PQtMessengerLegacy ;
 * другой для разработки https://github.com/hypnosis-i2p/i2pchat .

## Footnotes

[1] http://forum.i2p/viewtopic.php?p=11071#11071
