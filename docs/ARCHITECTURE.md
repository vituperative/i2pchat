# I2PChat Architecture

## Overview

I2PChat is a Qt/C++ instant messaging application that operates over the I2P
anonymous network using the SAM v3 protocol bridge. It provides encrypted,
decentralized chat with file transfer capabilities.

## Directory Layout

```
I2PChat.pro           - Qt project file
src/
  backend/            - Core logic: network, protocol, user management
  gui/                - Qt Widgets UI layer
docs/                 - Documentation, screenshots, release archives
qss/                  - Qt stylesheets
sounds/               - Notification sound files
templates/            - HTML templates
www/                  - Web profile assets
contrib/              - Packaging & platform helpers
debian/               - Debian packaging files
```

## src/backend — Layer Overview

### Session Layer (I2P Rendezvous)

| File | Role |
|------|------|
| `SessionController.h/cpp` | Single SAM session lifecycle — connects to SAM, creates a STREAM session, handles hello handshake, session create, dest generate, naming lookup, and auto-reconnect on disconnect. One per application. |
| `I2PStream.h/cpp` | One per logical connection. Wraps a `QTcpSocket` to SAM, performs STREAM CONNECT (outgoing) or STREAM ACCEPT (incoming), handles the SAM v3 handshake for each stream, reassembles fragmented SAM data, fires `signDataReceived`. Supports unlimited auto-reconnect via `QTimer`. |

### Stream Management Layer

| File | Role |
|------|------|
| `ConnectionManager.h/cpp` | Owns the `SessionController` and all `CI2PStream` objects (two maps: `allStreams` for active connections, `StreamIncomingListener` for accept listeners). Creates/destroys streams, manages stream ID allocation, dispatches incoming streams to `CCore`. |
| `I2PSamMessageAnalyser.h/cpp` | Parses SAM v3 protocol text lines into `SAM_MESSAGE` structs (HELLO_REPLAY, SESSION_STATUS, STREAM_STATUS, NAMING_REPLY, DEST_REPLY). |

### Core / Application Logic

| File | Role |
|------|------|
| `Core.h/cpp` | Central orchestrator. Owns `ConnectionManager`, `UserManager`, `CProtocol`, `CFileTransferManager`, `CSoundManager`. Manages online state machine (OFFLINE → TRYTOCONNECT → ONLINE/INVISIBLE/AWAY/...). Handles stream status callbacks, naming replies, incoming stream dispatch. |
| `Protocol.h/cpp` | Wire protocol for I2PChat peers. Packet format: 4-byte hex length + 4-byte tag + payload. Handles `slotInputUnknown` (initial CHATSYSTEM handshake) and `slotInputKnown` (parsed known packets). Commands (1000-1008): PING, GET_CLIENTVERSION/NAME/USER_INFOS/AVATARIMAGE etc. Messages (0000-0019): chat text, online status updates, block signals, file transfer version negotiation. |
| `PacketManager.h/cpp` | Per-stream reassembly buffer. Raw `QByteArray` accumulates incoming data; `checkifOnePacketIsCompleate()` slices complete packets by the 4-byte hex length header and emits `signAPacketIsCompleate`. |

### User Management

| File | Role |
|------|------|
| `User.h/cpp` | Per-contact model: nickname, I2P destination, protocol version, chat message history, unsent message queue, online state machine, per-user settings (font, color, auto-download, invisibility). |
| `UserManager.h/cpp` | Persisted user list (users.config), CRUD, sorting. |
| `UserBlockManager.h/cpp` | Block list persistence and lookup. |
| `UnsentChatMessageStorage.h/cpp` | Offline message queue persisted to disk. |

### File Transfer

| File | Role |
|------|------|
| `FileTransferManager.h/cpp` | Manages lists of active send/receive transfers, protocol version negotiation guard. |
| `FileTransferSend.h/cpp` | Outgoing file transfer — streams file chunks over a dedicated I2P stream. |
| `FileTransferReceive.h/cpp` | Incoming file transfer — reassembles chunks, writes to disk. |
| `HttpOverStreamObject.h/cpp` | HTTP parsing over I2P stream (for web profile). |

### Utilities

| File | Role |
|------|------|
| `Base.h/cpp` | Base64/Base32 encoding/decoding (ported from i2pd). |
| `ChatmessageChanger.h/cpp` | Text transformation pipeline (emoticons, formatting). |
| `DebugMessageManager.h/cpp` | In-memory ring buffer of debug messages, displayable in debug window. |
| `LoadHTML.cpp` | Embedded HTML for web profile fallback. |
| `SoundManager.h/cpp` | Audio notification dispatcher. |
| `TextEmotionChanger.h/cpp` | Smiley/emoticon substitution. |

## src/gui — UI Layer

| File | Role |
|------|------|
| `form_Main.h/cpp` | Main window: user list, online status combo, tray icon, context menus, window management. |
| `form_chatwidget.h/cpp` | Chat window per contact: message display (HTML), send field, emoticon/color/font controls. |
| `form_settingsgui.h/cpp` | Settings dialog (7 tabs). |
| `form_about.h/cpp` | About dialog. |
| `form_DebugMessages.h/cpp` | Debug log viewer. |
| `form_fileReceive.h/cpp` | Incoming file transfer dialog. |
| `form_fileSend.h/cpp` | Outgoing file transfer dialog. |
| `form_newUser.h/cpp` | Add-contact dialog. |
| `form_rename.h/cpp` | Rename-contact dialog. |
| `form_TopicSubscribe.h/cpp` | (Disabled) Topic subscription. |
| `form_UserSearch.h/cpp` | (Disabled) User search. |

## Data Flow — Connection Establishment

### Outgoing (you connect to a known contact)

```
User clicks "Online"
  → form_Main::onlineComboBoxChanged()
    → CCore::setOnlineStatus(ONLINE)
      [if OFFLINE] → mNextOnlineStatus=ONLINE, mCurrentOnlineStatus=TRYTOCONNECT
        → CCore::init()
          → CConnectionManager::doCreateSession(STREAM, privkey, opts)
            → CSessionController::doConnect()           // TCP to SAM
              → slotConnected() → HELLO VERSION...
                → slotReadFromSocket()
                  → HELLO_REPLAY OK → doSessionCreate()
                    → SESSION STATUS OK → signSessionStreamStatusOK(true)
                      → CCore::slotStreamControllerStatusOK(true)
                        → createStreamObjectsForAllUsers()
                          → createStreamObjectForUser(user)
                            → CI2PStream::doConnect(dest)   // per-contact
                              → SAM handshake → STREAM CONNECT
                                → STREAM STATUS OK → signStreamStatusReceived(OK)
                                  → CCore::slotStreamStatusReceived
                                    → CProtocol::newConnectionChat(ID)
                                      → send "CHATSYSTEM\t0.6\n" over SAM stream
      → remote side receives FIRSTPACKET, processes slotInputUnknown
        → remote sends back version, client info → slotInputKnown replies
```

### Incoming (a contact connects to you)

```
CI2PStream (ACCEPT mode) receives data
  → slotReadFromSocket() → STREAM STATUS + DESTINATION
    → signModeAcceptIncomingStream(ID)
      → CConnectionManager::slotModeAcceptIncomingStream()
        → stream moved from StreamIncomingListener → allStreams
        → emit signIncomingStream(stream)
          → CCore::slotIncomingStream()
            → connect stream→signDataReceived → mProtocol→slotInputUnknown
        → remaining buffered data sent as signDataReceived
          → CProtocol::slotInputUnknown()
            → detects "CHATSYSTEM\t" → parses version
            → if ID<0: CProtocol::newConnectionChat(ID)  // send our FIRSTPACKET back
            → adds/updates user via UserManager
            → setStreamTypeToKnown() → creates PacketManager
              → subsequent data routed through PacketManager → slotInputKnown
```

## Protocol Packet Format

```
Offset  Size  Description
0       4     Packet length in bytes (hex ASCII), excludes these 4 bytes
4       4     Tag (ASCII) — identifies command or message type
8       N     Payload (tag-dependent)
```

### Tags

| Tag | Direction | Meaning |
|-----|-----------|---------|
| 0000 | Response | Echo of ping |
| 0001 | Response | Client version |
| 0002 | Response | Client name |
| 0003 | Msg | Chat message |
| 0004-0008 | Msg | Online status updates |
| 0009 | Response | Protocol version |
| 0010 | Response | Max file transfer protocol |
| 0011-0014 | Response | User info (nick, gender, age, interests) |
| 0015-0016 | Msg | Block notifications |
| 0017-0018 | Response | Min file transfer protocol / avatar image |
| 0019 | Msg | Avatar changed notification |
| 1000-1008 | Command | PING, GET_CLIENTVERSION/NAME/USER_ONLINESTATUS/PROTOCOLVERSION/etc. |

## Online State Machine

```
                    setOnlineStatus(ONLINE/AWAY/...)
USEROFFLINE ──────────────────────────────────────────────► USERTRYTOCONNECT
  ▲                                                              │
  │                                                   session OK │
  │                                                              ▼
  │                                              USERTRYTOCONNECT ──► mNextOnlineStatus
  │                                                                      │
  │                                                              session │
  │                                                              created │
  │                                                                      ▼
  │                                                            USERONLINE / USERAWAY /
  │                                                            USERWANTTOCHAT / USERINVISIBLE /
  │                                                            USERDONT_DISTURB
  │                                                                      │
  └────────────────────── setOnlineStatus(OFFLINE) ◄─────────────────────┘
                               stopCore()
```

Note: `stopCore()` is only called on explicit offline — not on SAM disconnect.
Session loss sets `mCurrentOnlineStatus = USEROFFLINE` in
`slotStreamControllerStatusOK(false)` without cleaning up streams.
