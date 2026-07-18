# Changelog

## July, 2026

* Offline file-offer queuing with inline accept/reject, cancel icon, and persistent queue across restarts
* Transient (non-persistent) destination option
* Userlist: persistent sort mode, dynamic re-sort by online/new-message timestamps
* Auto-away with settings toggle
* Chat: 🤖⏳ header icons, ⏳ removed after send, bubble sizing/padding/CSS fixes, message order fix, pending→send notification suppression
* ECIES/ED25519 defaults; ECIES (Ratchet) for new profiles; DSA_SHA1 removed
* Optional profile web page at .b32 address
* Auth dialog: Block button, immediate nickname apply
* Debug Messages: monospace 8pt, single-div HTML layout
* Minimal theme as sole default; log display improvements (newline trim, 9pt)
* Sound defaults aligned with real files; pending→send notification suppressed

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
