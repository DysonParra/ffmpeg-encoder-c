#ifndef OPERATION_H_

#define OPERATION_H_

/* Includes for the header file */

#ifdef __cplusplus
#define LANGUAGE extern "C"
#elif defined __GNUC__
#define LANGUAGE extern
#endif

#ifdef OPERATION_STATIC_
#define DLLIMPORT
#elif defined BUILDING
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

#define CALLING __cdecl
//#define CALLING __stdcall

/* Macros and definitions for the source file. */
#ifdef OPERATION_DEFINITIONS_
#define elif else if
#define null NULL
#define String char*
#endif

LANGUAGE DLLIMPORT CALLING void printFile(char* array);
LANGUAGE DLLIMPORT CALLING void printArray(char* array);
LANGUAGE DLLIMPORT CALLING void sprintArray(char* output, char* input);

LANGUAGE DLLIMPORT CALLING int isANumber(char c);
LANGUAGE DLLIMPORT CALLING int validateNumber(char* array, int size);
LANGUAGE DLLIMPORT CALLING double getNumber(char* array);

LANGUAGE DLLIMPORT CALLING int isALetter(char c);
LANGUAGE DLLIMPORT CALLING int iqualChar(char c1, char c2);
LANGUAGE DLLIMPORT CALLING int countCharacters(char* input, char character);

LANGUAGE DLLIMPORT CALLING int compareArrays(char* array1, char* array2, int quantity);
LANGUAGE DLLIMPORT CALLING int findWord(char* array, char* word, int start, int end);
LANGUAGE DLLIMPORT CALLING int findFirstDifferentChar(char* array, int nargs, ...);
LANGUAGE DLLIMPORT CALLING int findLastDifferentChar(char* array, int nargs, ...);
LANGUAGE DLLIMPORT CALLING int getTextBetween(char* output, char* input, char* open, char* close, int number);
LANGUAGE DLLIMPORT CALLING char** splitArray(char* array, char* word);

LANGUAGE DLLIMPORT CALLING void replaceWord(char* input, char* output, char* word, char* replacedWord);
LANGUAGE DLLIMPORT CALLING void invertText(char* input, int start, int end);
LANGUAGE DLLIMPORT CALLING void replaceInvalidCharacters(char* fileName, char* replacedCharacter);
LANGUAGE DLLIMPORT CALLING void replaceUTF8CharToAscii(char* input);
LANGUAGE DLLIMPORT CALLING int convertHtmCharToAscii(char* htmlCharacter);
LANGUAGE DLLIMPORT CALLING void replaceHtmlCharacters(char* input);
LANGUAGE DLLIMPORT CALLING void convertUnicodeCharsToAcii(char* input);

#define cmpArs(array1, array2, quantity) compareArrays(array1, array2, quantity)
#define findFirstDifChar(array, nargs, ...) findFirstDifferentChar(array, nargs, __VA_ARGS__)
#define findLastDifChar(array, nargs, ...) findLastDifferentChar(array, nargs, __VA_ARGS__)

#if !defined BUILDING
#undef LANGUAGE
#undef DLLIMPORT
#undef CALLING
#endif

#endif
