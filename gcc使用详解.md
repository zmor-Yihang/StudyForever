# GCC 使用详解

GCC（GNU Compiler Collection）是 GNU 项目开发的一套编程语言编译器，支持多种编程语言，包括 C、C++、Objective-C、Fortran、Ada 和 Go 等。

## 基本语法

```bash
gcc [选项] 文件名
```

## 常用选项

### 基本编译选项
- `-c`：只编译不链接，生成目标文件(.o)
- `-o <文件名>`：指定输出文件名
- `-E`：只进行预处理，输出到标准输出
- `-S`：编译到汇编语言，生成.s文件
- `-v`：显示详细的编译过程信息

### 调试选项
- `-g`：生成调试信息
- `-ggdb`：生成 gdb 专用的调试信息
- `-gstabs`：生成 stabs 格式的调试信息

### 优化选项
- `-O1`：基本优化
- `-O2`：标准优化
- `-O3`：高级优化
- `-Os`：优化代码大小
- `-Ofast`：启用所有-O3优化并放宽一些数学标准
- `-Og`：优化调试体验

### 警告选项
- `-Wall`：显示所有常见的警告
- `-Wextra`：显示额外的警告
- `-Werror`：将警告视为错误
- `-Wpedantic`：要求严格遵循ISO C/C++标准
- `-Wunused`：警告未使用的变量或函数
- `-Wshadow`：警告局部变量遮蔽外部变量

### 链接选项
- `-L<目录>`：添加库文件搜索路径
- `-l<库名>`：链接指定的库
- `-static`：静态链接
- `-shared`：生成共享库
- `-rpath=<路径>`：设置运行时库搜索路径
- `-pthread`：链接pthread库

### 预处理器选项
- `-D<宏名>[=<值>]`：定义宏
- `-U<宏名>`：取消宏定义
- `-I<目录>`：添加头文件搜索路径
- `-include <文件>`：在编译前包含指定文件

### 语言标准选项
- `-ansi`：支持ANSI标准
- `-std=c99`：使用C99标准
- `-std=c11`：使用C11标准
- `-std=c++98`：使用C++98标准
- `-std=c++11`：使用C++11标准
- `-std=c++14`：使用C++14标准

## 编译过程

GCC 的编译过程分为四个阶段，每个阶段都有其特定的功能：

1. **预处理（Preprocessing）**
   - 处理源代码中的预处理指令，如 `#include`、`#define`、`#ifdef` 等
   - 展开头文件包含，替换宏定义，处理条件编译
   - 使用 `-E` 选项可以只执行预处理阶段，输出结果到标准输出
   - 可以使用 `-dD` 选项保留宏定义信息

2. **编译（Compilation）**
   - 将预处理后的文件转换为汇编语言
   - 进行词法分析、语法分析、语义分析和优化
   - 生成汇编代码文件（.s）
   - 使用 `-S` 选项可以只执行编译阶段

3. **汇编（Assembly）**
   - 将汇编代码转换为目标文件（.o）
   - 目标文件包含机器代码和符号表
   - 使用 `-c` 选项可以只执行汇编阶段

4. **链接（Linking）**
   - 将一个或多个目标文件与库文件链接生成可执行文件
   - 解析符号引用，分配最终地址
   - 处理静态库和动态库的链接

## 实际示例

### 简单编译
```bash
gcc hello.c -o hello
```

### 多文件编译
```bash
gcc file1.c file2.c -o program
```

### 分步编译
```bash
# 预处理
gcc -E hello.c -o hello.i

# 编译为汇编
gcc -S hello.i -o hello.s

# 汇编为目标文件
gcc -c hello.s -o hello.o

# 链接
gcc hello.o -o hello
```

### 带调试信息的编译
```bash
gcc -g hello.c -o hello
```

### 优化编译
```bash
gcc -O2 hello.c -o hello
```

### 条件编译示例
```bash
# 定义宏DEBUG
gcc -DDEBUG hello.c -o hello

# 定义带值的宏
gcc -DVERSION=\"1.0\" hello.c -o hello
```

### 指定标准和警告级别
```bash
# 使用C99标准并开启所有警告
gcc -std=c99 -Wall hello.c -o hello

# 将警告作为错误处理
gcc -Wall -Werror hello.c -o hello
```

## 库的使用

### 静态库
```bash
# 编译源文件为目标文件
gcc -c file1.c file2.c

# 创建静态库
ar rcs libmylib.a file1.o file2.o

# 使用静态库
gcc main.c -L. -lmylib -o program
```

### 动态库
```bash
# 编译位置无关代码
gcc -fPIC -c file1.c file2.c

# 创建动态库
gcc -shared -o libmylib.so file1.o file2.o

# 使用动态库
gcc main.c -L. -lmylib -o program
```

### 指定库版本
```bash
# 链接特定版本的库
gcc main.c -L/usr/local/lib -lmylib -lmylib2 -o program
```

## 常见问题

1. **找不到头文件**：使用 `-I<目录>` 添加头文件搜索路径
   ```bash
   gcc -I/usr/local/include hello.c -o hello
   ```

2. **找不到库文件**：使用 `-L<目录>` 添加库文件搜索路径
   ```bash
   gcc -L/usr/local/lib hello.c -lmylib -o hello
   ```

3. **运行时找不到动态库**：
   - Linux系统：设置 `LD_LIBRARY_PATH` 环境变量
   - macOS系统：设置 `DYLD_LIBRARY_PATH` 环境变量
   ```bash
   export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
   ```

4. **undefined reference to ...**：
   - 确保正确链接了所需的库
   - 检查库的链接顺序，依赖的库应放在被依赖项之后

5. **warning: implicit declaration of function**：
   - 包含正确的头文件
   - 检查函数声明是否正确

6. **relocation R_X86_64_32 against ...**：
   - 编译共享库时需要添加 `-fPIC` 选项

## 其他有用的选项

- `-D<宏名>[=<值>]`：定义宏
- `-U<宏名>`：取消宏定义
- `-I<目录>`：添加头文件搜索路径
- `-std=<标准>`：指定语言标准（如 c99, c11, c++11 等）
- `-include <文件>`：在编译前包含指定文件
- `-M`：输出文件的依赖关系
- `-MM`：输出用户头文件的依赖关系（忽略系统头文件）
- `-MF <文件>`：将依赖关系写入指定文件
- `-MT <目标>`：指定依赖关系中的目标
- `-time`：显示各个阶段的耗时
- `-pipe`：使用管道而不是临时文件进行编译过程