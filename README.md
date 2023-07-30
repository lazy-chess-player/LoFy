# LoFy 解释器

一个使用纯 C 语言编写的简单类 Python 语言解释器。

## 功能特性

- **数据类型**: 整数 (int)、浮点数 (float)、字符串 (string)、布尔值 (bool)
- **变量**: 动态类型变量与赋值
- **算术运算**: `+`, `-`, `*`, `/`
- **比较运算**: `==`, `!=`, `>`, `<`, `>=`, `<=`
- **流程控制**:
  - `if condition: statement`
  - `if condition: statement else: statement`
- **循环结构**: `while condition: statement`
- **内置函数**: `print()`
- **REPL**: 交互式命令行环境

## 编译指南

本项目支持使用 GCC 和 Make (或 Windows 上的 MinGW) 进行构建。

```bash
mingw32-make
# 或者在 Linux 上
make
```

## 运行方法

启动交互式解释器 (REPL):

```bash
./lofy.exe
```

## 示例代码

### 基础运算

```python
>>> x = 10
>>> y = 2.5
>>> print(x * y)
25.000000
```

### 条件判断

```python
>>> x = 10
>>> if x > 5: print("x is big") else: print("x is small")
x is big
```

### 循环

```python
>>> i = 0
>>> while i < 3: i = i + 1
>>> print(i)
3
```

## 开发日志

- **2023-07-12**: 项目初始化，实现基础词法与语法分析。
- **2023-07-15**: 新增布尔类型与比较运算符支持。
- **2023-07-18**: 实现 If-Else 条件控制流。
- **2023-07-25**: 实现 While 循环结构。
