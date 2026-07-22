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
docs/                 - Local documentation (ARCHITECTURE.md, CHANGELOG.md, LICENSE)
site/                 - GitHub Pages website (index.html, screenshots/)
www/                  - Web profile assets (b32.i2p profile templates)
qss/                  - Qt stylesheets
sounds/               - Notification sound files
```

## src/backend — Layer Overview

### Session Layer (I2P Rendezvous)

| File                      | Role                                                                                                                                                                                                                                                                              |
| ------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `SessionController.h/cpp` | Single SAM session lifecycle — connects to SAM, creates a STREAM session, handles hello handshake, session create, dest generate, naming lookup, and auto-reconnect on disconnect. One per application.                                                                           |
| `I2PStream.h/cpp`         | One per logical connection. Wraps a `QTcpSocket` to SAM, performs STREAM CONNECT (outgoing) or STREAM ACCEPT (incoming), handles the SAM v3 handshake for each stream, reassembles fragmented SAM data, fires `signDataReceived`. Supports unlimited auto-reconnect via `QTimer`. |

### Stream Management Layer

| File                          | Role                                                                                                                                                                                                                                                      |
| ----------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `ConnectionManager.h/cpp`     | Owns the `SessionController` and all `CI2PStream` objects (two maps: `allStreams` for active connections, `StreamIncomingListener` for accept listeners). Creates/destroys streams, manages stream ID allocation, dispatches incoming streams to `CCore`. |
| `I2PSamMessageAnalyser.h/cpp` | Parses SAM v3 protocol text lines into `SAM_MESSAGE` structs (HELLO_REPLAY, SESSION_STATUS, STREAM_STATUS, NAMING_REPLY, DEST_REPLY).                                                                                                                     |

### Core / Application Logic

| File                  | Role                                                                                                                                                                                                                                                                                                                                                                                      |
| --------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Core.h/cpp`          | Central orchestrator. Owns `ConnectionManager`, `UserManager`, `CProtocol`, `CFileTransferManager`, `CSoundManager`. Manages online state machine (OFFLINE → TRYTOCONNECT → ONLINE/INVISIBLE/AWAY/...). Handles stream status callbacks, naming replies, incoming stream dispatch.                                                                                                        |
| `Protocol.h/cpp`      | Wire protocol for I2PChat peers. Packet format: 4-byte hex length + 4-byte tag + payload. Handles `slotInputUnknown` (initial CHATSYSTEM handshake) and `slotInputKnown` (parsed known packets). Commands (1000-1008): PING, GET_CLIENTVERSION/NAME/USER_INFOS/AVATARIMAGE etc. Messages (0000-0019): chat text, online status updates, block signals, file transfer version negotiation. |
| `PacketManager.h/cpp` | Per-stream reassembly buffer. Raw `QByteArray` accumulates incoming data; `checkIfOnePacketIsComplete()` slices complete packets by the 4-byte hex length header and emits `signAPacketIsComplete`.                                                                                                                                                                                     |

### User Management

| File                             | Role                                                                                                                                                                                            |
| -------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `User.h/cpp`                     | Per-contact model: nickname, I2P destination, protocol version, chat message history, unsent message queue, online state machine, per-user settings (font, color, auto-download, invisibility). |
| `UserManager.h/cpp`              | Persisted user list (users.config), CRUD, sorting.                                                                                                                                              |
| `UserBlockManager.h/cpp`         | Block list persistence and lookup.                                                                                                                                                              |
| `UnsentChatMessageStorage.h/cpp` | Offline message queue persisted to disk.                                                                                                                                                        |

### File Transfer

| File                         | Role                                                                                |
| ---------------------------- | ----------------------------------------------------------------------------------- |
| `FileTransferManager.h/cpp`  | Manages lists of active send/receive transfers, protocol version negotiation guard. |
| `FileTransferSend.h/cpp`     | Outgoing file transfer — streams file chunks over a dedicated I2P stream.           |
| `FileTransferReceive.h/cpp`  | Incoming file transfer — reassembles chunks, writes to disk.                        |
| *(functionality in LoadHTML.cpp)* | HTTP response construction and template loading for web profile.                   |

### Utilities

| File                        | Role                                                                  |
| --------------------------- | --------------------------------------------------------------------- |
| `Base.h/cpp`                | Base64/Base32 encoding/decoding (ported from i2pd).                   |
| `ChatmessageChanger.h/cpp`  | Text transformation pipeline (emoticons, formatting).                 |
| `DebugMessageManager.h/cpp` | In-memory ring buffer of debug messages, displayable in debug window. |
| `LoadHTML.cpp`              | Embedded HTML for web profile fallback.                               |
| `SoundManager.h/cpp`        | Audio notification dispatcher.                                        |
| `TextEmotionChanger.h/cpp`  | Smiley/emoticon substitution.                                         |

## src/gui — UI Layer

| File                        | Role                                                                                       |
| --------------------------- | ------------------------------------------------------------------------------------------ |
| `form_Main.h/cpp`           | Main window: user list, online status combo, tray icon, context menus, window management.  |
| `form_chatwidget.h/cpp`     | Chat window per contact: message display (HTML), send field, emoticon/color/font controls. |
| `form_settingsgui.h/cpp`    | Settings dialog (7 tabs).                                                                  |
| `form_about.h/cpp`          | About dialog.                                                                              |
| `form_DebugMessages.h/cpp`  | Debug log viewer.                                                                          |
| `form_fileReceive.h/cpp`    | Incoming file transfer dialog.                                                             |
| `form_fileSend.h/cpp`       | Outgoing file transfer dialog.                                                             |
| `form_newUser.h/cpp`        | Add-contact dialog.                                                                        |
| `form_rename.h/cpp`         | Rename-contact dialog.                                                                     |
| `form_TopicSubscribe.h/cpp` | (Disabled) Topic subscription.                                                             |
| `form_UserSearch.h/cpp`     | (Disabled) User search.                                                                    |

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

## SAM Protocol Integration

I2PChat communicates with the local I2P router over TCP via the SAM v3 bridge
(`127.0.0.1:7656`). All SAM I/O is managed by `SessionController` and
`CI2PStream`.

### SAM v3 Handshake Flow

```
I2PChat → SAM:  HELLO VERSION MIN=3.1 MAX=3.3
SAM → I2PChat:  HELLO REPLY RESULT=OK VERSION=3.3

I2PChat → SAM:  SESSION CREATE ID=i2pchat STYLE=STREAM DESTINATION=TRANSIENT
                [SIGNATURE_TYPE=EdDSA_SHA512_Ed25519]
SAM → I2PChat:  SESSION STATUS RESULT=OK DESTINATION=<base64>
```

### Key Management

- **TRANSIENT** destinations — server generates an ephemeral keypair; not
  persisted. Used when the user enables "Transient identity" in settings.
- **Persistent (base64 private key)** — a previously-generated key is sent back
  to SAM on reconnect, restoring the same I2P identity. Stored in
  `~/.i2pchat/`.
- **Signature types:** `EdDSA_SHA512_Ed25519` (EdDSA, default for new profiles)
  and `DSA_SHA1` (legacy).

### Socket Stealing (v3)

After `SESSION CREATE`, the control socket is still a command channel. When
`STREAM CONNECT` or `STREAM ACCEPT` executes:

1. SAM marks the handler as `stolenSocket = true`
2. The TCP connection becomes a transparent bidirectional pipe to the I2P peer
3. Two internal pipe threads shuttle bytes: client→I2P and I2P→client
4. From I2PChat's perspective, the `QTcpSocket` is now a raw stream — reads
   and writes go directly to the remote peer

### Naming Lookup

```
I2PChat → SAM:  NAMING LOOKUP NAME=<b32|hostname>
SAM → I2PChat:  NAMING REPLY RESULT=OK NAME=<name> VALUE=<base64>
```

Used to resolve .b32.i2p addresses and hostnames to full I2P destinations.

### Stream Session Lifecycle

1. `STREAM CONNECT` (outgoing) — steals the control socket, bidirectional pipe
   to peer
2. `STREAM ACCEPT` (incoming) — SAM accepts an incoming connection, steals the
   socket, delivers the remote destination via a status line before the pipe
   starts
3. `STREAM CLOSE` — socket close propagates; SAM notifies `I2PStream` via
   `signDisconnected`
4. Auto-reconnect — on disconnect, `SessionController` re-establishes the SAM
   session and `CConnectionManager` recreates per-contact streams

### Limits

| Item                   | Value                                |
| ---------------------- | ------------------------------------ |
| SAM HELLO timeout      | 75 s                                 |
| Stream send size       | 1–32768 bytes per message            |
| Stale session expiry   | 10 min                               |
| Control socket timeout | 60 s (first cmd), 3 min (subsequent) |

## Protocol Packet Format

```
Offset  Size  Description
0       4     Packet length in bytes (hex ASCII), excludes these 4 bytes
4       4     Tag (ASCII) — identifies command or message type
8       N     Payload (tag-dependent)
```

### Tags

| Tag       | Direction   | Meaning                                                             |
| --------- | ----------- | ------------------------------------------------------------------- |
| 0000      | Response    | Echo of ping                                                        |
| 0001      | Response    | Client version                                                      |
| 0002      | Response    | Client name                                                         |
| 0003      | Msg         | Chat message                                                        |
| 0004-0008 | Msg         | Online status updates                                               |
| 0009      | Response    | Protocol version                                                    |
| 0010      | Response    | Max file transfer protocol                                          |
| 0011-0014 | Response    | User info (nick, gender, age, interests)                            |
| 0015-0016 | Msg         | Block notifications                                                 |
| 0017-0018 | Response    | Min file transfer protocol / avatar image                           |
| 0019      | Msg         | Avatar changed notification                                         |
| 1000-1008 | Command     | PING, GET_CLIENTVERSION/NAME/USER_ONLINESTATUS/PROTOCOLVERSION/etc. |

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
