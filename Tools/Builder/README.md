# Builder

A C++ GUI application for compiling N64 projects. This tool was written to supersede the old batch files and buggy KMC make system used early in the project.

### Compilation

Before you begin, because this tool is very early in its life, it is not very flexible. If you happened to have installed libultra in a path different than the norm (C:\ultra) then you must open `main.h` and modify the path stored in the `LIBULTRAPATH` macro.

If you are on Windows, ensure you have wxWidgets installed (instructions available [here](https://docs.wxwidgets.org/trunk/plat_msw_install.html)), and a copy of Visual Studio 2019 with [Platform Toolset v140_xp](https://stackoverflow.com/questions/58755564/visual-studio-2019-how-to-use-platform-toolset-v140-xp/58755565#58755565) installed. If all of that is set up, then simply open the solution file in Visual Studio and compile. 

Once compiled, you should move the compiled program over to the project's main source directory, since it will only seek files relative to the executable's path.

### Credits

This application uses [wxWidgets](https://www.wxwidgets.org/) in order to make the GUI application cross-platform.