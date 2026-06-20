# ClipLink

ClipLink 是一个跨平台局域网剪贴板同步工具。它由一个 TCP 转发服务端和 Qt 6 桌面客户端组成，只同步纯文本。

![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-312E81)
![Language](https://img.shields.io/badge/C%2B%2B-17-14B8A6)

## 功能

- 局域网 TCP 转发：一个服务端可连接多个桌面客户端。
- 纯文本剪贴板同步：本地复制后自动同步，并显示最近历史。
- 防回环：接收远端内容后不会无限重复发送。
- 图形界面：连接状态、同步历史、详情查看与手动发送。
- 连接恢复：断线后最多自动尝试重新连接 3 次。
- 跨平台构建：基于 CMake 和 Qt 6。

## 目标

- `cliplink_core`：协议与去重逻辑。
- `cliplink_server`：TCP 消息转发服务。
- `cliplink_client`：Windows、macOS、Linux 可构建的桌面客户端。
- `cliplink_tests`：核心协议、去重与网络行为的自动化测试。

## 构建

需要 CMake 3.21+、C++17 编译器与 Qt 6 的 Core、Network、Widgets、Test 模块。

macOS（Homebrew Qt Base）示例：

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qtbase
cmake --build build
ctest --test-dir build --output-on-failure
```

Windows（Qt 在线安装器 + MSVC）示例：

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:/Qt/6.x/msvc2022_64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Linux 示例：

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64
cmake --build build
ctest --test-dir build --output-on-failure
```

## 运行

先启动转发服务（默认端口为 `45454`）：

```bash
./build/cliplink_server --port 45454
```

再在两台设备上分别启动客户端：

```bash
./build/cliplink_client
```

在客户端点击“连接服务器”，输入服务端局域网地址。连接成功后，在任意一端复制纯文本，其他已连接客户端会收到同步内容；也可以在底部输入框手动发送文本。

单客户端也会显示本地复制的历史记录。要验证设备间同步，请同时运行两个客户端（或在另一台局域网设备运行客户端）。

## GitHub Actions

每次推送到 `main` 或创建 Pull Request 时，GitHub Actions 会在 Windows、macOS、Linux 上安装 Qt 6、编译 Release 版本、运行测试，并分别上传 `cliplink-windows`、`cliplink-macos`、`cliplink-linux` 产物。产物包含：

- `cliplink_server.exe`
- `cliplink_client.exe`

## 验收步骤

1. 启动一个服务端和两个客户端。
2. 两个客户端都连接到服务端，状态显示“已连接”。
3. 在客户端 A 复制一段文本，确认客户端 B 的剪贴板与同步历史出现该文本。
4. 关闭服务端，确认两个客户端的状态变为“未连接”。
5. 重新启动服务端并在客户端点击连接，确认可再次同步。

## 限制

第一版同步纯文本。macOS 和 Linux 上可能需要允许应用访问系统剪贴板；Wayland 的剪贴板策略由桌面环境控制。
