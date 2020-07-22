# I2PChat - Instant Messenger

### Screenshots

![screenshot-roster](https://vituperative.github.io/i2pchat/screenshots/main.png) ![screenshot-chat](https://vituperative.github.io/i2pchat/screenshots/chat.png)

### Downloads (pre-built binaries)

* Latest Windows (Win32/64) build from: <a href="https://ci.appveyor.com/project/wipedlifepotato/i2pchat/build/artifacts">https://ci.appveyor.com/project/wipedlifepotato/i2pchat/build/artifacts</a>

### Features

 * Direct peer-to-peer communications without server requirements
 * File transfer between contacts
 * Control online visibility on a per-contact basis
 * Optional, customizable b32.i2p web page to display profile
 * Emoticon support

### Current news

* July, 2020
  * Add support for optional web page to display user profile at .b32 address
  * Remove insecure DSA_SHA1 from Signature Types
  * Add ECIES (Ratchet) encryption type to new profiles (UI option coming soon!)

* June, 2020:
   * Fixed crash of close chat window
   * Fixed crash of url link in chat
   * Added $HOME/.i2pchat/ directory support for using from /usr/bin
   * Pre inited optarg
   * Core changes
   * Ci pre-inited. Works for windows now
   * Created .deb package for ubuntu/debian x86_64
   * Created Windows build for 32 bit, which will works on 64 bits
   * Design changes
   * Fix offline message crash
* June, 2020: dr\|z3d starts work on renovating the user interface
* 5 Jan, 2017: Original repo at http://git.repo.i2p/w/I2P-Messenger-QT.git was fully merged here

### Build instructions

 * Install prerequisites:

```
sudo apt-get install qt5-qmake qt5-default build-essential libqt5multimedia5 qtmultimedia5-dev libqt5svg5-dev
```

 * To prepare for compilation, run qmake:
   - Release: `qmake I2PChat.pro "CONFIG += release"`
   - Debug: `qmake I2PChat.pro "CONFIG += debug"`

 * To complile:
   - `make -j NUMBER_OF_PROCESSOR_CORES` e.g `make -j8`
   - or `make` to compile single-threaded

### Running

On Linux, `make` creates `I2PChat` executable in the current folder. Run it with `./I2PChat`.

* You will need to enable the SAM application bridge in your router: for Java I2P via <a href="http://127.0.0.1:7657/configclients">Client Configuration</a> or for i2pd via i2pd.conf's [SAM] section.
* As of version 0.2.31, the DSA_SHA1 Signature type is no longer available. The recommended (and default) Signature Type is now: ECDSA_SHA512_P521
* Select 'Online' from the dropdown menu on the main window. When you first go online, your unique address (Destination) will be created when connecting to SAM
* Your settings and contacts will be stored in `~/.i2pchat/` on Linux-based systems, or `%APPDATA%\Roaming\I2PChat\` on Windows

### License

Licensed under GPLv2.