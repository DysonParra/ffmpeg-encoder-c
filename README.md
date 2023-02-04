# Information of the project.
This repository is responsible of do specific tasks.

# Project struct
- header  
  Contains all header files (.h) used in some source file (.c).  
- icon  
  Contains the icon file (.ico) that the source file will have, the name of the file must be iqual to the main source file except the extension.
- object  
  Temp folder that is created while the source files are compiled.
- project root dir  
  The root of the project contains all source files (.c), dynamic libs (.dll) and binary file (.exe)

# Binary and library
The operation is equal for all c repositories of this user.  
Use the command: "make all" and all files are created  

There's two type of libraries:
- Static:  
  Is a static lib file (.a) that include a header file (.h) and that will be merged with the main source file in only one binary file (.exe).
- Dynamic:  
  There's two types
    * Local:  
      Is a shared lib file (.dll) that include a header file (.h) and a source file (.c) different to the main source file.
    * External:  
      Is a shared lib file (.dll) that include a header file (.h), is external because the current project has not the source file (.c), so the libray was created in another project and used in current.

# Makefile
There's some important variables:
  - appName  
    The name of the source file with main method withouth extension.
  - prefferDynamic  
    If there are a dynamic lib (.dll) and a static lib (.a) for the same source file (.c), when create the binary file it need to decide which use to generate the binary, so if the value is 0 use the static, and else use the dynamic.

Generally the only one task used is 'all', but these task call another tasks, and the more importants are:
  - make clean  
    Erase the content of the folder object, the binary file (.exe), static libs (.a) and local dynamic libs (.dll).
  - make libName$(dLibMakeFormat)  
    Generate a dynamic lib (local) using the file with name libName.c
  - make libName$(sLibMakeFormat)  
     Generate a static lib using the file with name libName.c
  - make app  
    Generate the binary file (.exe), but the source file needs to include all headers that will use in the source code, for example:  
    #include "flag.h"  
    #include "flag_processor.h"  
    No need to specify that are in the folder header, the task read the file, identify all user included libraries in the source file (in this example two) and generate the command to link the libraries with the binary file considering the value of "prefferDynamic", so if you include more libraries no need to modify the MakeFile  
  - make appR  
    It executes make app and then run the app.
  - make appN
    Generate or try to generate the binary file (.exe) withouth identify the libraries to link.
  - make appNR
    It executes make appN and then run the app.
