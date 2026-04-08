# Problem 041 - map extra

**ACMOJ Problem ID**: 2672

## Table of Contents

- [Problem 041 - map extra](#problem-041-map-extra)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
    - [Background](#background)
  - [Assignment Description](#assignment-description)
    - [Grade Composition](#grade-composition)
  - [Assignment Requirements](#assignment-requirements)
    - [Input Format](#input-format)
    - [Output Format](#output-format)
    - [Samples](#samples)
    - [Data Constraints](#data-constraints)
  - [Per-Testcase Resource Limits](#per-testcase-resource-limits)
  - [Test Data](#test-data)
  - [Submission Requirements](#submission-requirements)
    - [OJ Git Repository Compilation Process](#oj-git-repository-compilation-process)
    - [Git Configuration Requirements](#git-configuration-requirements)
    - [Submission Guidelines](#submission-guidelines)
    - [Evaluation Notes](#evaluation-notes)
    - [Academic Integrity](#academic-integrity)

## Introduction

### Background

Extended map implementation with performance benchmarking against std::map. Requires efficient iterators.

## Assignment Description

### Grade Composition

| Grading Component | Percentage |
| :--: | :--: |
| Pass **2672. map extra** | 80% |
| Code Review | 20% |

Here are several points that need clarification:

- In the Code Review, we will **strictly examine your code style and repository organization structure, etc.**

- This assignment provides some sample data for testing, stored in the `/workspace/data/041/data_test/` directory. Note that these are not the test cases on the Online Judge. Passing all local test cases does not guarantee that you will pass the OJ tests.

- Besides the provided sample data, we also encourage you to design your own test data based on your program logic to assist debugging.

## Assignment Requirements

### Problem Description

首先强调: Benchmark 仅供参考, 影响因素比较多, 难以保证客观, 测评略[不稳定](https://acm.sjtu.edu.cn/OnlineJudge/code/415585/)

这题要求和 map 几乎一致, 应该几乎没有什么需要改动的. 需要特别注意以下几点:

- `std::map` 的迭代器不同于 `std::vector`, 只要指向的节点 (或者`end()`) 没有被 `erase`, 其会一直保持有效, 并且指向的对象的地址不应该改变. 我们的 `map` 自然也要如此.
- `--end()` 操作是合法的, 除非 `map` 为空.
- `map` 传入的类型不一定保证有默认构造函数, 只有当用到了下标运算符 `[]` 时, 值类型才必须保证可以默认构造.
- 在这个测试中, 我们**保证不会**抛出异常. 你或许可以删除一些不必要的边界检测来加速 (事实上, 标准库不会做不必要的边界检查, 除非用户要求 (比如 `at` 函数)).

特别地, 我们的 `utility.hpp` 也稍微进行了一点改动(应该不会影响原有代码), 使其表现和 `std::pair` 更加接近.

```C++
#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <utility>

namespace sjtu {

template<class T1, class T2>
class pair {
public:
    T1 first;   // first    element
    T2 second;  // second   element

    constexpr pair() = default;
    constexpr pair(const pair &other) = default;
    constexpr pair(pair &&other) = default;

    template<class U1 = T1, class U2 = T2>
    constexpr pair(U1 &&x, U2 &&y)
        : first(std::forward<U1>(x)), second(std::forward<U2>(y)) {}

    template<class U1, class U2>
    constexpr pair(const pair<U1, U2> &other)
        : first(other.first), second(other.second) {}

    template<class U1, class U2>
    constexpr pair(pair<U1, U2> &&other)
        : first(std::move(other.first))
        ,second(std::move(other.second)) {}
};

template<class T1, class T2>
pair(T1, T2) -> pair<T1, T2>;

}

#endif
```

### Input Format

See the problem description above.

### Output Format

See the problem description above.

### Samples

No sample data provided for this problem.

### Data Constraints

See the problem description for constraints.

## Per-Testcase Resource Limits

- **Time Limit (per test case)**: 30000 ms
- **Memory Limit (per test case)**: 256 MiB
- **Disk Usage**: No disk usage is permitted.

## Test Data

The test data for this problem is located at `/workspace/data/041/data_test/`.

## Submission Requirements

### OJ Git Repository Compilation Process

For Git compilation, we will first clone the repository using a command similar to:
```bash
git clone <repo_url> . --depth 1 --recurse-submodules --shallow-submodules --no-local
```

Then we check if there is a `CMakeLists.txt` file. If it exists, we run (if not, a warning message will be displayed):
```bash
cmake .
```

Finally, we check if there is any of `GNUmakefile`/`makefile`/`Makefile` (if cmake was run previously, this will be the generated Makefile). If it exists, we run (if not, a warning message will be displayed):
```bash
make
```

After this process is complete, we will use the `code` file in the project root directory as the compilation result.

A `CMakeLists.txt` file is provided in the project. You can use or modify it as needed. The local environment has gcc-13 and g++-13 available.

### Git Configuration Requirements

**IMPORTANT**: You must create a `.gitignore` file in your project root directory to avoid OJ evaluation conflicts.

The `.gitignore` file should include at least the following entries:

```gitignore
CMakeFiles/
CMakeCache.txt
```

### Submission Guidelines

- The submitted code must be able to compile successfully through the above compilation process
- The compiled executable file name must be `code`
- The program needs to be able to read data from standard input and write results to standard output
- Please ensure the code runs correctly within the given time and space limits
- **You must use C++ or C language** to implement this assignment

### Evaluation Notes

- The evaluation system will test your program using the provided test data
- The program output must exactly match the expected output (including format)
- Exceeding time or memory limits will be judged as the corresponding error type
- Please pay attention to the overall time performance of your code and the time complexity of each part of your algorithm.

### Academic Integrity

If any violations are found during evaluation or code review (including but not limited to using unconventional methods to pass test cases), your final score may be significantly reduced or become **0 points**.
