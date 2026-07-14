# I2PChat (formerly I2P-Messenger)
## Screenshots

![screenshot-roster](https://vituperative.github.io/i2pchat/screenshots/main.png) ![screenshot-chat](https://vituperative.github.io/i2pchat/screenshots/chat.png)

## Features

 * Direct peer-to-peer communications without server requirements
 * File transfer between contacts
 * Control online visibility on a per-contact basis
 * Optional, customizable b32.i2p web page to display profile
 * Emoticon support

## Current news

* July, 2026
  * Set ECIES and ED25519 as default sigtype/encryption
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
* June, 2020: dr\|z3d starts work on renovating the user interface, WipedLife starts work on the backend
* 5 Jan, 2017: Original repo at http://git.repo.i2p/w/I2P-Messenger-QT.git was fully merged here

## Build instructions

 * Note: Qt 5.14 or later required (Qt 6 is not yet supported)
 * For Qt versions prior to v5.14, you can try building with an older version of the code, available from: https://github.com/vituperative/i2pchat/archive/706c908ee267051da12c51f2b87c0d9c293f69d8.zip

#### Dependencies

<details><summary>Debian / Ubuntu (including 24.04+, trixie+)</summary>

```
sudo apt-get install -y build-essential qt5-qmake qtbase5-dev qtmultimedia5-dev libqt5svg5-dev
```
</details>

<details><summary>Fedora</summary>

```
sudo dnf install make qt5-qtmultimedia-devel qt5-qtsvg-devel qt5-qtbase-devel
```
</details>

#### Quick build

```
bash build.sh
```

Run `bash build.sh --help` for all options (incremental by default, `--clean` for full rebuild, `--format` and `--tidy` for linting).

#### Manual compilation

```
qmake I2PChat.pro "CONFIG += release"
make -j$(nproc)
```

## Downloads (pre-built binaries)

 * [Latest CI build (Linux)](https://github.com/vituperative/i2pchat/actions/workflows/build.yml)
 * 0.2.37 Linux binary (stripped) with custom profile template and event audio samples (Qt 5.14 or later required): <a href="https://vituperative.github.io/i2pchat/I2PChat0.2.37-Linux.zip">I2PChat0.2.37-Linux.zip</a>

## Running

On Linux, using `bash build.sh` creates a stripped binary at `dist/I2PChat`. For manual builds, `make` creates `I2PChat` in the project root. Run it with `./I2PChat`.

* You will need to enable the SAM application bridge in your router: for Java I2P via <a href="http://127.0.0.1:7657/configclients">Client Configuration</a> or for i2pd via i2pd.conf's [SAM] section.
* As of version 0.2.31, the DSA_SHA1 Signature type is no longer available. The recommended (and default) Signature Type is now: EdDSA_SHA512_Ed25519
* Select 'Online' from the dropdown menu on the main window. When you first go online, your unique address (Destination) will be created when connecting to SAM
* Your settings and contacts will be stored in `~/.i2pchat/` on Linux-based systems, or `%APPDATA%\Roaming\I2PChat\` on Windows

## License

Licensed under GPLv2.
