# I2PChat (formerly I2P-Messenger)

End-to-end encrypted peer-to-peer messenger over I2P. Uses the SAM bridge for anonymous, serverless communication with full file transfer support.

## Screenshots

![screenshot-roster](https://vituperative.github.io/i2pchat/screenshots/main.png) ![screenshot-chat](https://vituperative.github.io/i2pchat/screenshots/chat.png)

## Features

* Peer-to-peer messaging and file transfer over I2P — no central server
* End-to-end encryption via I2P's garlic routing
* Contact list with online status indicators (Online, Away, Invisible, Do Not Disturb)
* Userlist sorting — alphabetically, by date added, last communication, or last online
* Online visibility control per contact (visible, hidden, blocked)
* Request authorization dialog for new incoming connections
* Transient (non-persistent) identity option for session-only keys
* Auto-away detection with configurable timeout
* Inline pending messages — compose while offline, sent automatically on reconnection
* Emoticon support with punctuation-boundary detection
* Automatic URL linking in chat messages
* Optional b32.i2p web profile page with avatar, bio, and interests
* File transfer with per-user auto-download configuration and pipelined transfers
* Drag-and-drop image upload with inline display
* Copy b32 address and raw destination with right-click
* Move contacts to top/bottom of list via right-click menu
* Offline message queue — messages are delivered when the contact comes online
* Configurable tunnel length, quantity, and backup quantities
* ECIES (Ratchet) and EdDSA signature types for new destinations
* Customizable chat appearance (font, color, style)
* Sound notifications for online/offline, messages, and file transfers
* Debug logging for troubleshooting

## Build instructions

#### Dependencies

Qt 5.14 or later (Qt 6 is not yet supported).

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

Run `bash build.sh --help` for all options (incremental by default, `--clean` for full rebuild, `--format` and `--tidy` for linting, `--appimage` to build a portable AppImage).

#### Manual compilation

```
qmake I2PChat.pro "CONFIG += release"
make -j$(nproc)
```

## Downloads

* [CI builds (all platforms)](https://github.com/vituperative/i2pchat/actions/workflows/build.yml) — Linux, Windows, macOS

## Running

On Linux, `bash build.sh` creates a stripped binary at `dist/I2PChat`. Run `bash build.sh --appimage` to produce a portable AppImage at `dist/I2PChat-x86_64.AppImage`. Manual builds produce `I2PChat` in the project root. Run it with `./I2PChat`.

On macOS, the CI produces a bundled `.app` via `macdeployqt`. Run it by double-clicking `I2PChat.app`.

* Enable the SAM application bridge in your router: Java I2P via [Client Configuration](http://127.0.0.1:7657/configclients), or i2pd via `i2pd.conf`'s `[SAM]` section.
* Select **Online** from the dropdown menu on the main window. Your unique I2P destination is created automatically on first SAM connection.
* Settings and contacts are stored in `~/.i2pchat/` (Linux) or `%APPDATA%\Roaming\I2PChat\` (Windows).
* Default signature type: EdDSA_SHA512_Ed25519. DSA_SHA1 is no longer available.

## Changelog

See [docs/CHANGELOG.md](docs/CHANGELOG.md).

## License

Licensed under AGPL-3.0-or-later. Originally forked from [I2P-Messenger](http://www.i2p-messenger.de/) (GPL-2.0-or-later).
