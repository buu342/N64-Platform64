# Builder

A C++ GUI application for compiling N64 projects. This tool was written to supersede the old batch files and buggy KMC make system used early in the project.

### Compilation

If you are on Windows, ensure you have wxWidgets installed (instructions available [here](https://docs.wxwidgets.org/trunk/plat_msw_install.html)), and a copy of Visual Studio 2019 with [Platform Toolset v140_xp](https://stackoverflow.com/questions/58755564/visual-studio-2019-how-to-use-platform-toolset-v140-xp/58755565#58755565) installed. If all of that is set up, then simply open the solution file in Visual Studio and compile. 

### Usage

I recommend placing the executable in the same folder as your project's source, as that will make it so that the program loads the project from the get-go instead of requiring you to manually open the project folder.

If you are using the tool for the first time, hover over to `Settings->Preferences` and ensure that the program config (which is the bottom half of the menu, where the Libultra path is stored) is set up correctly. 

If you are attempting to build someone else's project, you can hover over to `Settings->Import project settings` to load a .nbp file which contains the settings to use for that specific project.

#### Segments

By default, all objects are placed in the codesegment. This can be changed by double clicking C file in the project tree and changing the name of the segment. 

During compilation, each segment name will be stored as an environment variable, which you can use to point to the correct path in your spec file. For example, if I have a segment named `codesegment`, then the `CODESEGMENT` environment variable will be created which contains the path to the file. In your spec file, simply use `$(CODESEGMENT)` to have it use the path in the environment variable.

#### Force ROM rebuild

If you make changes to the spec file or to files which are linked into the final ROM through inclusion in the spec file, the tool will not detect these changes and thus will not rebuild the ROM (this can be fixed in the future). You can force just the creation of a new ROM (skipping the recompilation of unchanged objects) through the `Force ROM rebuild` option in the `Build` menu.

### Credits

This application uses [wxWidgets](https://www.wxwidgets.org/) in order to make the GUI application cross-platform.
