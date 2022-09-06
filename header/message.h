#ifndef MESSAGE_H_

#define MESSAGE_H_

/* Includes for the header file */

#ifdef __cplusplus
#define LANGUAGE extern "C"
#elif defined __GNUC__
#define LANGUAGE extern
#endif

#ifdef MESSAGE_STATIC_
#define DLLIMPORT
#elif defined BUILDING
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

#define CALLING __cdecl
//#define CALLING __stdcall

/* Macros and definitions for the source file. */
#ifdef MESSAGE_DEFINITIONS_
#define elif else if
#define null NULL
#define String char*
#endif

DLLIMPORT typedef struct Message Message;
DLLIMPORT typedef struct MessageList MessageList;
LANGUAGE DLLIMPORT CALLING void newMessage(MessageList* list, char* name, char* value);
LANGUAGE DLLIMPORT CALLING void setMessageName(Message* aux, char* name);
LANGUAGE DLLIMPORT CALLING void setMessageValue(Message* aux, char* value);
LANGUAGE DLLIMPORT CALLING MessageList* makeMessageList(char* filePath);
LANGUAGE DLLIMPORT CALLING MessageList* makePathList(char* filePath, char* ProgramDir);
LANGUAGE DLLIMPORT CALLING void printMessageList(MessageList* list);
LANGUAGE DLLIMPORT CALLING int compareMessageName(Message* aux, char* array);
LANGUAGE DLLIMPORT CALLING Message* findMessage(MessageList* list, char* name);

#define makeMsgList(filePath) makeMessageList(filePath)
#define findMsg(list, name) findMessage(list, name)
#define prtMsgList(list) printMessageList(list)
#define findMsgVal(list, name)  findMsg(list,  name)->value

struct Message {
    /* Es el nombre del Message */
    char* name;
    /* Es el valor del Message */
    char* value;
    /* Es el siguiente Message */
    Message* next;
};

struct MessageList {
    /* Es el primer Message */
    Message* first;
    /* Es el último Message */
    Message* last;
};

#if !defined BUILDING
#undef LANGUAGE
#undef DLLIMPORT
#undef CALLING
#endif

#endif
