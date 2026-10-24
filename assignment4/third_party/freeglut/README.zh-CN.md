# freeglut 3.8.0（Windows / Visual Studio 2022 / x64 和 Win32）

来源：https://github.com/freeglut/freeglut/releases/tag/v3.8.0

这是从官方源码在本机用 Visual Studio 2022 编译的静态库。源码压缩包保存在 `../freeglut-3.8.0.tar.gz`，版权许可见 `COPYING`。

在 assignment3 的项目属性中设置：

1. **C/C++ → 常规 → 附加包含目录**：`$(SolutionDir)third_party\freeglut\include`
2. **C/C++ → 预处理器 → 预处理器定义**：添加 `FREEGLUT_STATIC`
3. **链接器 → 常规 → 附加库目录**：x64 使用 `$(SolutionDir)third_party\freeglut\lib\x64`；Win32 使用 `$(SolutionDir)third_party\freeglut\lib\x86`

然后可以在源码中使用 `#include <GL/glut.h>`。freeglut 的头文件在 MSVC 下会自动选择 `freeglut_staticd.lib`（Debug）或 `freeglut_static.lib`（Release），并链接 Windows SDK 自带的 `opengl32.lib`、`glu32.lib` 等系统库。不需要复制 DLL。

两个平台均提供 Debug 和 Release 静态库。构建这些库时显式启用了 freeglut 自带的 `XParseGeometry` 替代实现，以免链接时缺少该符号。
