<img align="center" src=".github/Logo.png" width="470" height="98" />

---
[![Build Status](https://dev.azure.com/buu342/Platform64/_apis/build/status/buu342.N64-Platform64?branchName=master)](https://dev.azure.com/buu342/Platform64/_build/latest?definitionId=1&branchName=master)

A free and open source 2.5D side-scrolling adventure game for the Nintendo 64.


## Compilation

Below are instructions for compiling different parts of Platform64.

### Engine
<details><summary>N64</summary>

1) Ensure you have libultra installed.
2) If you haven't already, you must first compile the [Builder](Tools/Builder) tool, and preferably place it in the project's source directory. Launch the tool once and ensure it is configured to match your libultra install.
3) Execute `Builder.exe` and go to `Settings->Import project settings`. Load `platform.nbp`.
4) Press the compile button to generate the ROM.
</details>

### Tools
<details><summary>Builder</summary>

Please consult the [Tools/Builder](Tools/Builder#builder) folder for compilation instructions.
</details>

<details><summary>MemMapper</summary>

Please consult the [MemMapper](https://github.com/buu342/CPP-MemMapper) repository for compilation instructions.
</details>

<details><summary>NRDC</summary>

Please consult the [NRDC](https://github.com/buu342/N64-NRDC) repository for compilation instructions.
</details>

<details><summary>UNFLoader</summary>

Please consult the [UNFLoader](https://github.com/buu342/N64-UNFLoader) repository for compilation instructions.
</details>

### Credits

Special thanks to Silkicons and Farm Fresh Icons.