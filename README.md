````markdown
# Vectorization Benchmark and Assembly Analysis

This project demonstrates **automatic and manual vectorization** using **AVX2 SIMD instructions** in C++. It benchmarks and inspects how vectorization affects the generated assembly and execution performance.

---

## 🔬 What is Vectorization?

**Vectorization** is the process of converting scalar operations (one element at a time) into vector operations (multiple elements at once). Modern CPUs support **SIMD (Single Instruction, Multiple Data)** which allows processing of multiple data points using a single instruction.

For example:

```cpp
// Scalar loop
for (int i = 0; i < n; ++i)
    a[i] = b[i] + c[i];
````

Can be vectorized to use AVX2 (256-bit wide) instructions:

```cpp
// Manual vectorized version using AVX2 intrinsics
__m256i vb = _mm256_loadu_si256((__m256i*)&b[i]);
__m256i vc = _mm256_loadu_si256((__m256i*)&c[i]);
__m256i va = _mm256_add_epi32(vb, vc);
_mm256_storeu_si256((__m256i*)&a[i], va);
```

---

## 🧠 What Does This Project Do?

### ✅ Vector Addition

This project compares two functions:

* `add`: scalar loop that adds two vectors element-wise.
* `add_vectorized`: manually vectorized version using AVX2 intrinsics.

### 🛠 Macros: Disabling Optimization

To ensure a clean comparison, the `add_vectorized` function can be protected from compiler optimizations using a macro:

```cpp
DISABLE_OPTIMIZATION
void add_vectorized(...) {
    ...
}
RESTORE_OPTIMIZATION
```

These macros ensure the compiler doesn’t optimize away or auto-vectorize this part, so you have control over what’s generated.

### ⏱️ Timestamping

The code uses `std::chrono` to time both functions and prints a side-by-side benchmark in a table.

```bash
+-------------------+----------------+
| Function          | Time (ms)      |
+-------------------+----------------+
| add               | 0.0151         |
| add_vectorized    | 0.0043         |
+-------------------+----------------+
```

---

## 🔍 Parsing the Assembly

After the build, the CMake script generates an assembly dump using:

* **Linux/macOS**: `objdump -d -M intel`
* **Windows (MSVC)**: `dumpbin /DISASM`

The C++ program takes the path to the assembly file and searches for the function symbols (mangled names) for `add` and `add_vectorized`.

```bash
./Vectorization analysis/assembly.txt
```

It then prints the extracted assembly instructions for manual inspection.

---

## 💡 What Are We Looking For?

We look for **evidence of vectorization** — that is, the use of **AVX registers (YMM)** instead of scalar registers.

In the disassembly output, this looks like:

```asm
vmovdqu     ymm0, ymmword ptr [...]
vpaddd      ymm0, ymm0, ymmword ptr [...]
vmovdqu     ymmword ptr [...], ymm0
```

This confirms that AVX2 256-bit registers (`ymm0`, `ymm1`, etc.) are being used. These instructions are only generated if the compiler is told to target AVX2 (e.g., via `/arch:AVX2` on MSVC or `-mavx2` on GCC/Clang).

---

## 🧪 Build & Run Instructions (Local)

### 🧱 CMake Flags

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/Vectorization build/analysis/assembly.txt
```

Make sure your CMake file includes:

```cmake
target_compile_options(Vectorization PRIVATE -O3 -mavx2)  # For GCC/Clang
# or
target_compile_options(Vectorization PRIVATE /O2 /arch:AVX2)  # For MSVC
```

---

## 📂 File Structure

```
.
├── CMakeLists.txt
├── main.cpp                 # Main benchmarking and analysis logic
├── compiler_config.h        # Macros to disable/restore optimization
├── analysis/
│   └── assembly.txt         # Generated disassembly output
```

---

## 📌 Summary

* Demonstrates scalar vs vectorized computation.
* Confirms AVX2 usage by inspecting assembly for `ymm` registers.
* Uses macros to control compiler optimization.
* Provides platform-independent support via GitHub Actions.

---

