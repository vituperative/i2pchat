# Changelog

## July, 2026

* Offline file-offer queuing with inline accept/reject, cancel icon, and persistent queue across restarts
* Transient (non-persistent) destination option
* Multi-file static web server with HTTP Basic Auth, per-user folder mapping, cookie-based sessions, directory listing, rate-limited login (3 failed = 1h ban), configurable docroot, and /login+/logout endpoints
* On-the-fly HTML/CSS/JS minification
* Custom-styled directory listings via `filelist.css` (dark/light theme, file-type icons)
* Userlist: persistent sort mode, dynamic re-sort by online/new-message timestamps
* Auto-away with settings toggle
* Chat: header icons, removed after send, bubble sizing/padding/CSS fixes, message order fix, pending→send notification suppression
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

* Fix crash on close chat window, URL links, and offline messages
* Add $HOME/.i2pchat/ directory support
* Add .deb package for Debian/Ubuntu
* Add 32-bit Windows build
* UI renovation and backend overhaul

## January, 2017

* Original I2P-Messenger-QT repository merged
