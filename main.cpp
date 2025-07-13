#include <iostream>
#include <vector>
#include <random>
#include <immintrin.h> 
#include "compiler_config.h"
#include <fstream>
#include <string>
#include <chrono>

void add(int *__restrict a, int *__restrict b, int *__restrict c, int n) {
    for (int i = 0; i < n; ++i)
        a[i] = b[i] + c[i];
}

DISABLE_OPTIMIZATION

void add_vectorized(int *a, int *b, int *c, int n) {
    int i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256i vb = _mm256_loadu_si256((__m256i*)&b[i]);
        __m256i vc = _mm256_loadu_si256((__m256i*)&c[i]);
        __m256i va = _mm256_add_epi32(vb, vc);
        _mm256_storeu_si256((__m256i*)&a[i], va);
    }
    for (; i < n; ++i) {
        a[i] = b[i] + c[i];
    }
}

RESTORE_OPTIMIZATION

bool printFunctionAssembly(std::ifstream& asmFile, const std::string& functionName) {
    std::string line;
    bool inFunc = false;
    bool printedHeader = false;
    while (std::getline(asmFile, line)) {
        if (!inFunc) {
            if (line.find(functionName) != std::string::npos) {
                inFunc = true;
                std::cout << "\n--- Assembly for " << functionName << " ---\n";
                printedHeader = true;
            }
        } else {
            if (line.empty() || line.find("ENDP") != std::string::npos || line.find(".cfi_endproc") != std::string::npos) {
                // End of function
                return true;  // success
            }
            std::cout << line << '\n';  // Print the assembly line
        }
    }
    return printedHeader;  // return true if we found and printed something
}

void PrintFullAssembly(const std::string & assemblyFilePath)
{
    std::ifstream asmFile(assemblyFilePath);
    if (!asmFile) {
        std::cerr << "Failed to open assembly file: " << assemblyFilePath << "\n";
        return;
    }

    std::cout << "\n--- Full Assembly Output ---\n\n";

    std::string line;
    while (std::getline(asmFile, line)) {
        std::cout << line << '\n';
    }

    std::cout << "\n--- End of Full Assembly ---\n\n";

    asmFile.close();
}

int main(int argc ,char * argv[]){
     int n = 64;
   
    std::vector<int> a(n,0);
    std::vector<int> b(n);
    std::vector<int> c(n);


    std::random_device rd;                     
    std::mt19937 gen(rd());                   
    std::uniform_int_distribution<> dist(1000,100000);  

    for(int i = 0; i < n; i++){
        b[i] = dist(gen);
        c[i] = dist(gen);
    }
    // Time add()
    auto start_add = std::chrono::high_resolution_clock::now();
    add(a.data(), b.data(), c.data(), n);
    auto end_add = std::chrono::high_resolution_clock::now();
    auto duration_add = std::chrono::duration<double, std::nano>(end_add - start_add).count();

    // Time add_vectorized()
    auto start_vec = std::chrono::high_resolution_clock::now();
    add_vectorized(a.data(), b.data(), c.data(), n);
    auto end_vec = std::chrono::high_resolution_clock::now();
    auto duration_vec = std::chrono::duration<double, std::nano>(end_vec - start_vec).count();


    if (argc < 2) {
        std::cerr << "Error: Please provide the assembly file path.\n";
        return 1;
    }
    std::string assemblyFilePath = argv[1];
    
    std::ifstream asmFile(assemblyFilePath);
    if (!asmFile) {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    #if defined(_MSC_VER)
        const std::string mangledRunName = "add@@YAXPEIAH00H@Z"; // MSVC
        const std::string mangledRunVectorizedName = "?add_vectorized@@YAXPEAH00H@Z"; // MSVC
    #else
        const std::string mangledRunName = "_Z3addPiiii";     // GCC/Clang
        const std::string mangledRunVectorizedName = "_Z13add_vectorizedPiiii";     // GCC/Clang
    #endif

    bool foundAdd = printFunctionAssembly(asmFile, mangledRunName);

    if (!foundAdd) {
        std::cerr << "Failed to find assembly for " << mangledRunName << '\n';
    }

    // Reset file stream to start for second function
    asmFile.clear();
    asmFile.seekg(0, std::ios::beg);

    bool foundAddVectorized = printFunctionAssembly(asmFile, mangledRunVectorizedName);

    if (!foundAddVectorized) {
        std::cerr << "Failed to find assembly for " << mangledRunVectorizedName << '\n';
    }

    PrintFullAssembly(assemblyFilePath);
    
    asmFile.close();


    std::cout << "Timing results\n";
    std::cout << "\n+----------------------+---------------------+\n";
    std::cout << "| Function             | Time (nanoseconds)  |\n";
    std::cout << "+----------------------+---------------------+\n";
    std::cout << "| add                  | " << std::fixed << duration_add << "          |\n";
    std::cout << "| add_vectorized       | " << std::fixed << duration_vec << "          |\n";
    std::cout << "+----------------------+---------------------+\n";

    return 0;
}
