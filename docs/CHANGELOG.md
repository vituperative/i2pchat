# Changelog

## July, 2026

* Add non-persistent (transient per-session) destination option
* userlist: persist sort mode across restarts
* implement auto-away with checkable group box in settings, Core timer, and global event filter
* userlist: re-sort dynamically on online state changes and new messages
* userlist: add real timestamp tracking for date added, last communication, and last online sort modes
* about dialog: show short commit hash, collapse equal file-transfer versions
* fix: implement copyB32 b32 address derivation
* fix: doCreateSession now returns true on success path
* fix: ChatmessageChanger color default uses valid hex string instead of QFont
* fix: add null guards to getUserByI2P_Destination in file transfer handlers
* fix: add null guard to listWidget item access in copyDestination/copyB32
* fix: return QString() instead of 0 in getStreamControllerBridgeName
* chat: fix message ordering — newest at bottom, oldest at top
* remove inline editing for pending messages — plain text with (pending) tag only
* Remove Offline tab; show pending messages inline with in-place editing
* Fix memory leaks, threading UB, logic errors, and deprecated API usage
* Add build script, linter config, and dev toolchain
* Add ARCHITECTURE.md overview
* Plug heap-memory leaks in QMessageBox dialogs
* Set ECIES and ED25519 as default sigtype/encryption
* Add support for optional web page to display user profile at .b32 address
* Remove insecure DSA_SHA1 from Signature Types
* Add ECIES (Ratchet) encryption type to new profiles (UI option coming soon!)

## January, 2026

* Add proper online status management during SAM connection loss/recovery
* Add auto-reconnect for SAM session and fix contact list icons
* Fix emoticon display and avatar in user list
* Fix settings persistence and avatar display
* Fix config persistence, nickname display, contact sorting, and network connection

## December, 2025

* Enable user list sorting functionality
* Add Move to Top/Bottom user positioning options
* Implement per-user auto-download configuration
* Enable Copy B32 Address functionality with intelligent handling

## November, 2025

* Simplify message processing and enable emoticon handling
* Enhance emoticon detection with punctuation boundary support
* Improve file transfer performance with buffering and larger packets
* Enable automatic URL linking in chat messages
* Remove blocking QMessageBox from SAM disconnection handler

## June, 2020

* Fixed crash of close chat window
* Fixed crash of url link in chat
* Added $HOME/.i2pchat/ directory support for using from /usr/bin
* Created .deb package for ubuntu/debian x86_64
* Created Windows build for 32 bit
* Fix offline message crash
* dr|z3d starts work on renovating the user interface, WipedLife starts work on the backend

## January, 2017

* Original repo at http://git.repo.i2p/w/I2P-Messenger-QT.git was fully merged here
