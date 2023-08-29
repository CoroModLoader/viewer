<hr>

<div align="center"> 
    <img src="https://raw.githubusercontent.com/CoroModLoader/loader/cc520db1db7edfccbb106faa2f994c4a81c75c4e/assets/logo.svg" height=312/>
</div>

<div align="center"> 

Modding Framework for Coromon  

</div>

---

# Introduction

A tool to decompile Solar2D resource files.

# Screenshot

![screenshot](assets/screenshot.png)

# Requirements

* [unluac](https://sourceforge.net/projects/unluac/files/Unstable/)

# Building

* Clone Repository

* Setup Frontend
  ```bash
  cd interface
  yarn && yarn build
    ```

* Compile
  ```bash
  mkdir build && cd build
  cmake .. && cmake --build . --config Release   
  ```

* Done!