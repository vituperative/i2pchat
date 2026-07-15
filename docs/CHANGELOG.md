# Changelog

## July, 2026

* Offline file-offer queuing — select files while offline, delivered automatically on reconnect via new protocol tags (FILE_OFFER / FILE_OFFER_ACCEPTED / FILE_OFFER_REJECTED)
* Accept/Reject file offers inline in chat via clickable anchor links
* Cancel icon (✕) on pending messages and file offers to remove them from the queue before they're sent
* Persistent file-offer queue across restarts (INI storage alongside unsent text messages)
* Add non-persistent (transient per-session) destination option
* userlist: persist sort mode across restarts
* implement auto-away with checkable group box in settings, Core timer, and global event filter
* userlist: re-sort dynamically on online state changes and new messages
* userlist: add real timestamp tracking for date added, last communication, and last online sort modes
* about dialog: show short commit hash, collapse equal file-transfer versions
* fix: implement copyB32 b32 address derivation
* chat: fix message ordering — newest at bottom, oldest at top
* pending messages: show plain text with (pending) tag and remove Offline tab; inline editing removed
* Set ECIES and ED25519 as default sigtype/encryption
* Add support for optional web page to display user profile at .b32 address
* Remove insecure DSA_SHA1 from Signature Types
* Add ECIES (Ratchet) encryption type to new profiles

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
