# ClipLink Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a cross-platform Qt 6 desktop client and TCP relay server that synchronize clipboard text across devices on a LAN.

**Architecture:** A small shared `cliplink_core` library owns the length-prefixed JSON protocol, message identity cache, and clipboard-record model. The server only validates and relays messages. The Qt Widgets client separates network transport, clipboard integration, and the main window so each can be tested independently.

**Tech Stack:** C++17, Qt 6 Core/Network/Widgets/Test, CMake, JSON encoded UTF-8 messages.

---

## Planned file structure

- `CMakeLists.txt`: top-level project, Qt dependency discovery, targets and tests.
- `src/core/protocol.h/.cpp`: framing and JSON encode/decode.
- `src/core/recent_ids.h/.cpp`: bounded UUID de-duplication cache.
- `src/server/main.cpp`, `src/server/relay_server.h/.cpp`: headless TCP broadcast server.
- `src/client/main.cpp`, `src/client/network_client.h/.cpp`: desktop client boot and persistent connection.
- `src/client/clipboard_watcher.h/.cpp`: `QClipboard` integration and loop prevention.
- `src/client/main_window.h/.cpp`: polished Qt Widgets interface and history display.
- `tests/test_protocol.cpp`, `tests/test_recent_ids.cpp`: automated core behavior tests; each QtTest source is built as its own executable because each defines its own test `main`.
- `README.md`: build/run instructions for Windows, macOS, Linux and demo procedure.
- `.gitignore`: local build, Qt Creator and brainstorming-session artifacts.

### Task 1: Repository hygiene and build skeleton

**Files:**
- Create: `.gitignore`, `CMakeLists.txt`, `README.md`

- [ ] **Step 1: Define the expected CMake target layout in the README**

```text
cliplink_core
cliplink_server
cliplink_client
cliplink_tests
```

- [ ] **Step 2: Create the smallest CMake project that configures without source targets**

```cmake
cmake_minimum_required(VERSION 3.21)
project(ClipLink LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Network Widgets Test)
enable_testing()
```

- [ ] **Step 3: Configure to verify the dependency error is understandable or configuration succeeds**

Run: `cmake -S . -B build`

Expected: either a generated `build/` directory or a clear message that Qt 6 must be installed and discoverable through `CMAKE_PREFIX_PATH`.

- [ ] **Step 4: Add build artifacts and temporary brainstorming state to `.gitignore`**

```gitignore
build/
.superpowers/
.DS_Store
```

- [ ] **Step 5: Commit only the new repository files**

```bash
git add .gitignore CMakeLists.txt README.md
git commit -m "chore: initialize ClipLink project"
```

### Task 2: Implement and test the network message protocol

**Files:**
- Create: `src/core/protocol.h`, `src/core/protocol.cpp`, `tests/test_protocol.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Write a failing QtTest for an encoded clipboard message round trip**

```cpp
void ProtocolTest::clipboardMessageRoundTrips() {
    const auto message = cliplink::Message::clipboard(
        "message-1", "device-a", "Laptop", "hello");
    const auto decoded = cliplink::decodeFrame(cliplink::encodeFrame(message));
    QCOMPARE(decoded.value(), message);
}
```

- [ ] **Step 2: Run the test and verify it fails because protocol symbols do not exist**

Run: `cmake --build build --target cliplink_tests && ctest --test-dir build --output-on-failure`

Expected: compile failure referring to missing `cliplink::Message`, `encodeFrame`, or `decodeFrame`.

- [ ] **Step 3: Implement the minimal protocol API**

```cpp
namespace cliplink {
struct Message { QString type, id, deviceId, deviceName, text; };
QByteArray encodeFrame(const Message &message);
std::optional<Message> decodeFrame(const QByteArray &frame);
}
```

The frame starts with a four-byte big-endian payload length followed by a UTF-8 JSON object. Reject payloads over 16 KiB and JSON missing `type` or `id`.

- [ ] **Step 4: Run the protocol test and verify it passes**

Run: `ctest --test-dir build --output-on-failure`

Expected: one passing `ProtocolTest` suite.

- [ ] **Step 5: Add a failing invalid-frame test, implement rejection, then re-run tests**

```cpp
void ProtocolTest::rejectsOversizeFrame() {
    QByteArray frame(4, '\0');
    frame[3] = 0x41;
    QVERIFY(!cliplink::decodeFrame(frame).has_value());
}
```

Expected: malformed or oversize frames return `std::nullopt` without crashing.

- [ ] **Step 6: Commit the protocol slice**

```bash
git add CMakeLists.txt src/core/protocol.* tests/test_protocol.cpp
git commit -m "feat: add framed clipboard protocol"
```

### Task 3: Implement and test loop-prevention state

**Files:**
- Create: `src/core/recent_ids.h`, `src/core/recent_ids.cpp`, `tests/test_recent_ids.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Write a failing bounded-cache behavior test**

```cpp
void RecentIdsTest::recognizesSeenIdsAndEvictsOldest() {
    cliplink::RecentIds ids(2);
    QVERIFY(!ids.containsOrInsert("one"));
    QVERIFY(!ids.containsOrInsert("two"));
    QVERIFY(ids.containsOrInsert("one"));
    QVERIFY(!ids.containsOrInsert("three"));
    QVERIFY(!ids.containsOrInsert("two"));
}
```

- [ ] **Step 2: Run the dedicated test executable and verify the missing class causes failure**

Run: `cmake --build build --target cliplink_recent_ids_tests`

Expected: compile failure naming `cliplink::RecentIds`.

- [ ] **Step 3: Implement `RecentIds` with a `QQueue<QString>` and `QSet<QString>`**

```cpp
class RecentIds {
public:
    explicit RecentIds(int capacity);
    bool containsOrInsert(const QString &id);
};
```

- [ ] **Step 4: Run all core tests and verify they pass**

Run: `ctest --test-dir build --output-on-failure`

Expected: `ProtocolTest` and `RecentIdsTest` pass.

- [ ] **Step 5: Commit the loop-prevention slice**

```bash
git add CMakeLists.txt src/core/recent_ids.* tests/test_recent_ids.cpp
git commit -m "feat: prevent clipboard synchronization loops"
```

### Task 4: Implement the TCP relay server

**Files:**
- Create: `src/server/main.cpp`, `src/server/relay_server.h`, `src/server/relay_server.cpp`
- Modify: `CMakeLists.txt`, `README.md`

- [ ] **Step 1: Add a failing integration test for relaying one valid frame to another socket**

```cpp
void RelayServerTest::forwardsClipboardFrameToOtherClient() {
    cliplink::RelayServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));
    // Connect sender and receiver, send one encoded clipboard frame,
    // then compare the receiver frame with the sender frame.
}
```

- [ ] **Step 2: Run it and verify failure due to the absent relay server**

Run: `cmake --build build --target cliplink_tests && ctest --test-dir build --output-on-failure`

Expected: compile failure naming `cliplink::RelayServer`.

- [ ] **Step 3: Implement a minimal `RelayServer`**

```cpp
class RelayServer : public QObject {
    Q_OBJECT
public:
    bool listen(const QHostAddress &address, quint16 port);
};
```

Maintain a `QSet<QTcpSocket*>`; buffer incoming bytes per socket; only broadcast fully decoded `clipboard` frames to sockets other than the sender. Delete sockets on disconnect.

- [ ] **Step 4: Run the relay integration test and verify it passes**

Run: `ctest --test-dir build --output-on-failure`

Expected: server relays the exact valid message once.

- [ ] **Step 5: Add `cliplink_server` main that accepts `--port 45454`**

```cpp
QCoreApplication app(argc, argv);
cliplink::RelayServer server;
return server.listen(QHostAddress::Any, 45454) ? app.exec() : 1;
```

- [ ] **Step 6: Commit the relay server**

```bash
git add CMakeLists.txt src/server README.md tests
git commit -m "feat: add TCP clipboard relay server"
```

### Task 5: Implement the client transport and clipboard integration

**Files:**
- Create: `src/client/network_client.h`, `src/client/network_client.cpp`, `src/client/clipboard_watcher.h`, `src/client/clipboard_watcher.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Write a failing test that received remote text is applied once and never re-sent**

```cpp
void ClipboardWatcherTest::remoteMessageDoesNotCreateOutboundLoop() {
    cliplink::RecentIds ids(20);
    cliplink::ClipboardWatcher watcher(&ids);
    watcher.applyRemoteText("message-1", "from another device");
    QVERIFY(watcher.wasApplied("message-1"));
    QVERIFY(!watcher.shouldSendCurrentClipboard());
}
```

- [ ] **Step 2: Run it and verify failure because `ClipboardWatcher` is absent**

Run: `cmake --build build --target cliplink_tests && ctest --test-dir build --output-on-failure`

Expected: compile failure naming `cliplink::ClipboardWatcher`.

- [ ] **Step 3: Implement `NetworkClient` and `ClipboardWatcher` minimally**

`NetworkClient` connects to one configured host/port, emits `connected`, `disconnected`, `errorOccurred`, and `clipboardReceived(Message)`. `ClipboardWatcher` listens to `QClipboard::dataChanged`, only emits nonempty text whose ID is not in `RecentIds`, and wraps remote writes in a guard flag.

- [ ] **Step 4: Run all automated tests and verify they pass**

Run: `ctest --test-dir build --output-on-failure`

Expected: all core, relay, and clipboard tests pass.

- [ ] **Step 5: Commit client synchronization logic**

```bash
git add CMakeLists.txt src/client tests
git commit -m "feat: synchronize clipboard text in client"
```

### Task 6: Build the polished Qt Widgets UI

**Files:**
- Create: `src/client/main.cpp`, `src/client/main_window.h`, `src/client/main_window.cpp`
- Modify: `CMakeLists.txt`, `README.md`

- [ ] **Step 1: Write a failing QtTest for new history entries appearing in newest-first order**

```cpp
void MainWindowTest::showsNewestClipboardEntryFirst() {
    cliplink::MainWindow window;
    window.appendHistory({"id-a", "Desktop", "first"});
    window.appendHistory({"id-b", "Laptop", "second"});
    QCOMPARE(window.historyPreviewAt(0), QString("second"));
}
```

- [ ] **Step 2: Run it and verify failure because `MainWindow` is absent**

Run: `cmake --build build --target cliplink_tests && ctest --test-dir build --output-on-failure`

Expected: compile failure naming `cliplink::MainWindow`.

- [ ] **Step 3: Implement the minimum visual hierarchy**

Create a `QMainWindow` with a 12 px rounded central card, indigo title/status strip, green/amber/red connection indicator, left-side `QListWidget` history, right-side read-only `QTextEdit` details, manual-send `QPlainTextEdit`, and a `QToolButton` pause toggle. Apply these colors through a local stylesheet: indigo `#312E81`, teal `#14B8A6`, amber `#F59E0B`, red `#EF4444`, background `#F8FAFC`.

- [ ] **Step 4: Wire UI signals to the client components**

Connect manual send to `NetworkClient::sendClipboardText`; connect received messages to `ClipboardWatcher::applyRemoteText` and `MainWindow::appendHistory`; connect network status signals to the status indicator. Show a modal settings dialog for host, port, and device name.

- [ ] **Step 5: Run all automated tests and launch a visual smoke test**

Run: `ctest --test-dir build --output-on-failure`

Run: `build/cliplink_server --port 45454` and `build/cliplink_client`

Expected: tests pass; client window shows the designed layout and can connect to localhost.

- [ ] **Step 6: Commit the desktop UI**

```bash
git add CMakeLists.txt src/client README.md tests
git commit -m "feat: add polished ClipLink desktop interface"
```

### Task 7: Cross-platform documentation and end-to-end verification

**Files:**
- Modify: `README.md`

- [ ] **Step 1: Document CMake configuration for the three supported platforms**

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/<compiler>
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

- [ ] **Step 2: Verify the complete local demonstration**

Run one server and two clients, copy text in client A, and confirm client B shows the same content, source device, time, and one history entry.

- [ ] **Step 3: Verify recovery behavior**

Stop the server, confirm clients show disconnected state, restart it, and confirm the manually triggered reconnect succeeds.

- [ ] **Step 4: Commit final documentation and verification notes**

```bash
git add README.md
git commit -m "docs: add ClipLink build and demo guide"
```
