<img align="center" src=https://i.imgur.com/0yYZKe1.png width="475" height="115" />

---

A 3D platformer engine for the Nintendo64.
</br>
</br>
**NOTICE**
</br>
The Git of the project can be in a messy or untested state. If you wish to download the compiled tools or ROMs, or the source code for the respective versions of said tools or ROMs, then please visit the releases page.
</br>
</br>
## What's included</br>
Platform64 - The game itself</br>
NRDC - A ROM header registerer</br>
File Listener - A tool to automatically detect changes to a file and execute the 64Drive loader program
</br>
</br>
## How to use the tools
### NRDC
```
nrdc.exe romname.n64
or
nrdc.exe romname.n64 "ROMNAME" MANUFACTURER ID REGION
```

### File Listener
```
listener.exe romname.n64
```
</br>

## Compilation Instructions</br>
### Platform64</br>
Ensure you have the SDK installed and already working</br>
Place NRDC in the same directory as makeme.bat (not required but will leave your ROMs unregistered)</br>
Edit the setup lines of makeme.bat if needed</br>
Run makeme.bat</br>

### NRDC
Run makeme.bat in an OS with access to GCC.</br>

### File Listener
Run makeme.bat in an OS with access to GCC.</br>