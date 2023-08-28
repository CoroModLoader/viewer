<hr>

<div align="center"> 
    <img src="https://raw.githubusercontent.com/CoroModLoader/loader/cc520db1db7edfccbb106faa2f994c4a81c75c4e/assets/logo.svg" height=312/>
</div>

<div align="center"> 

Modding Framework for Coromon  

</div>

---

# Introduction

This repository hosts the source code for the car-archive viewer, which can be used to decompile a solar2d games' lua files

# Building

* Clone Repository

* Setup Frontend
  ```bash
  cd interface
  yarn && yarn build
  saucer embed dist
    ```
* Compile
  ```bash
  mkdir build && cd build
  cmake .. && cmake --build . --config Release   
  ```
* Done!