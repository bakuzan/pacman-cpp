# Pacman C++

An implementation of Pacman in C++ using SFML.

Most likely this will not be of use to anyone.

## Questions

### The environment cannot find SFML

- Check the `.vscode\tasks.json` args and `.vscode\c_cpp_properties.json` includePaths.
- Update them to point to wherever it is that your SFML files are located.

### After building a file, I ran the .exe and it errored; unable to find sfml libraries

- Open a cmd prompt.
- Enter `set PATH=<project_path>\dlls;%PATH%` to add the folder to your PATH for the session.
- You can check it was added by running `for %i in ("%PATH:;=" "%") do @echo %i` to list the paths in the PATH
- Run `<my_file>.exe` or whatever you were trying to run.

### How do I make the pch.h.gch file?

Run the following command in your project directory: `g++ -x c++-header -I"<your_path_to>/SFML/include" -o pch.h.gch pch.h`
