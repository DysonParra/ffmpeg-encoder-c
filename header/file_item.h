#ifndef FILE_ITEM_H_

#define FILE_ITEM_H_

/* Includes for the header file */

#ifdef __cplusplus
#define LANGUAGE extern "C"
#elif defined __GNUC__
#define LANGUAGE extern
#endif

#ifdef FILE_ITEM_STATIC_
#define DLLIMPORT
#elif defined BUILDING
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

#define CALLING __cdecl
//#define CALLING __stdcall

/* Macros and definitions for the source file. */
#ifdef FILE_ITEM_DEFINITIONS_
#define elif else if
#define null NULL
#define String char*
#endif

DLLIMPORT typedef struct FileItem FileItem;
DLLIMPORT typedef struct FileList FileList;
LANGUAGE DLLIMPORT CALLING FileList* makeFileList(char* path, char* formats);
LANGUAGE DLLIMPORT CALLING void printFileList(FileList* list);
LANGUAGE DLLIMPORT CALLING char* getName(char* file);
LANGUAGE DLLIMPORT CALLING char* getFormat(char* file);
LANGUAGE DLLIMPORT CALLING char* getFullPath(char* filePath);
LANGUAGE DLLIMPORT CALLING char* getFatherPath(char* filePath);
LANGUAGE DLLIMPORT CALLING double getFileSize(char* fileName, int divisions);

struct FileItem {
    /* Es el nombre del {FileItem} */
    char name[1024];
    /* Es el siguiente {FileItem} */
    FileItem* next;
};

struct FileList {
    /* Es el primer {FileItem} */
    FileItem* first;
    /* Es el último {FileItem} */
    FileItem* last;
};

#if !defined BUILDING
#undef LANGUAGE
#undef DLLIMPORT
#undef CALLING
#endif

#endif
