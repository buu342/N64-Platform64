@echo off
for %%f in (*.png) do (
    if "%%~xf"==".png" bin2c.exe %%f icon_%%~nf.h icon_%%~nf_png
)