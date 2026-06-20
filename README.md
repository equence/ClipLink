# ClipLink

ClipLink 是一个跨平台局域网剪贴板同步工具。它由一个 TCP 转发服务端和 Qt 6 桌面客户端组成，只同步纯文本。

## 目标

- `cliplink_core`：协议与去重逻辑。
- `cliplink_server`：TCP 消息转发服务。
- `cliplink_client`：Windows、macOS、Linux 可构建的桌面客户端。
- `cliplink_tests`：核心协议、去重与网络行为的自动化测试。

## 开发环境

需要 CMake 3.21+、C++17 编译器与 Qt 6 的 Core、Network、Widgets、Test 模块。

macOS（Homebrew Qt Base）示例：

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qtbase
cmake --build build
ctest --test-dir build --output-on-failure
```
