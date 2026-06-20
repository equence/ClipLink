# ClipLink

ClipLink 是一个跨平台局域网剪贴板同步工具。它由一个 TCP 转发服务端和 Qt 6 桌面客户端组成，只同步纯文本。

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

## 验收步骤

1. 启动一个服务端和两个客户端。
2. 两个客户端都连接到服务端，状态显示“已连接”。
3. 在客户端 A 复制一段文本，确认客户端 B 的剪贴板与同步历史出现该文本。
4. 关闭服务端，确认两个客户端的状态变为“未连接”。
5. 重新启动服务端并在客户端点击连接，确认可再次同步。

## 限制

第一版同步纯文本。macOS 和 Linux 上可能需要允许应用访问系统剪贴板；Wayland 的剪贴板策略由桌面环境控制。
