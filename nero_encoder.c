/*
 * @fileoverview    {FileName} se encarga de realizar tareas específicas.
 *
 * @version         2.0
 *
 * @author          Dyson Arley Parra Tilano <dysontilano@gmail.com>
 *
 * @copyright       Dyson Parra
 * @see             github.com/DysonParra
 *
 * History
 * @version 1.0     Implementación realizada.
 * @version 2.0     Documentación agregada.
 */
#include "file_item.h"
#include "message.h"
#include "nero_encoder.h"
#include "operation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void makeBat(MessageList* paths, int selectOption);
static char isVideoFile(char* file);
static char isAudioFile(char* file);

#define elif else if
#define getMessage(message) findMsgVal(messages, (char*)#message)
#define getPath(path) findMsgVal(paths, (char*)#path)

/**
 * Entrada principal del sistema.
 *
 * @param argc cantidad de argumentos pasados en la linea de comandos.
 * @param argv argumentos de la linea de comandos.
 * @return {0} si el programa se ejecutó correctamente.
 */
int main(int argc, char** argv) {
    char* ProgramDir = getFatherPath(argv[0]);                          // Crea array con el directorio en donde se encuentra el ejecurable actual.
    char path[265] = "";                                                // Crea array que tendrá la ruta del archivo con las rutas.
    sprintf(path, "%s\\config\\paths.ini", ProgramDir);                 // Almacena la ruta del archivo de rutas en pathsFile.
    MessageList* paths = makePathList(path, ProgramDir);                // Obtiene los messages del archivo con las rutas.

    // printf("\npaths:\n");                                            // Muestra mensaje.
    // prtMsgList(paths);                                               // Muestra las rutas.

    MessageList* messages = makeMsgList(getPath(messages-NeroAAC));     // Obtiene los messages del archivo con los mensajes a mostar en pantalla.
    // printf("\nMessages:\n");                                         // Muestra mensaje.
    // prtMsgList(messages);                                            // Muestra los mensajes a mostar en pantalla.

    Message* aux = messages->first;                                     // Crea mensaje con el primer mensaje de la lista de mensajes.
    while (aux != NULL) {                                               // Recorre todos los mensajes.
        replaceUTF8CharToAscii(aux->value);                             // Reemplaza los caracteres utf-8 por caracteres ascii en el mensaje actual.
        aux = aux->next;                                                // Pasa al siguiente mensaje.
    }

    // printf("\nMessages:\n");                                         // Muestra mensaje.
    // prtMsgList(messages);                                            // Muestra los mensajes a mostar en pantalla.

    int selectOption = 1;                                               // Crea variable que tendrá la opción seleccionada por el usuario.

#define printMessage(msj) printf("\n%s", getMessage(msj))               // Crea macro para imprimir los mensajes.
    printMessage(selectOptionMsg);                                      // Lista las opciones.
    printMessage(option01Msg);                                          // Opción 01.
    printMessage(option02Msg);                                          // Opción 02.
    printMessage(exitOptMsg);                                           // Salir.
    printf("\n");                                                       // Muestra salto de linea.
    scanf("%d", &selectOption);                                         // Pregunta al usuario por la opción.

    if (selectOption == 1 || selectOption == 2)                         // Si se indicó una opción válida.
        makeBat(paths, selectOption);                                   // Crea el archivo .bat.

    return 0;                                                           // Termina el programa.
}

/**
 * TODO: Definición de {@code makeBat}.
 */
static void makeBat(MessageList* paths, int selectOption) {
    FILE* flags = NULL;                                                         // crea apuntador a archivo para abrir el archivo de flags.
    char neroFlags[2000] = "";                                                  // Crea array para almacenar las flags de nero del archivo de flags.
    char* aux = NULL;                                                           // Crea apuntador a array pare recorrer el archivo de flags.

    aux = neroFlags;                                                            // Asigna a aux las la primera posición de las flags de nero.
    flags = fopen(getPath(flags-NeroAAC), "r+");                                // Abre el archivo de flags de nero.
    while (!feof(flags)) {                                                      // Recorre el archivo con las flags de nero.
        fgets(aux, 2000, flags);                                                // Concatena la siguiente línea del archivo de flags de nero con la actual.

        aux += strlen(aux);                                                     // Pone aux al final de la línea leída.
        if (aux[-1] == '\n')                                                    // Si el último caracter de la linea es un salto de línea.
            aux[-1] = ' ';                                                      // Convierte el salto de línea en espacio.
    }
    fclose(flags);                                                              // Cierra el archivo de flags de nero.
    // printf("\"%s\"\n", neroFlags);                                           // Muestra las flags de nero almacenadas.

    char line[1000] = "";                                                       // Crea array para almacenar cada linea del archivo.
    char* fullPath = NULL;                                                      // Variable que tendrá la ruta absoluta de cada archivo.
    char* fileName = NULL;                                                      // Variable que tendrá el nombre de cada archivo sin la ruta.
    int maxSize = 0;                                                            // Variable que tendrá la cantidad de caracteres de la línea más larga del archivo.
    FILE* input;                                                                // Crea puntador a archivo para abrir el archivo de entrada.
    FILE* output;                                                               // Crea apuntador a archivo para crear el archivo de salida.
    char* ffmpeg = getPath(ffmpeg);                                             // Crea array con la ruta de ffmpeg.
    char* neroAacEnc = getPath(neroAacEnc);                                     // Crea array con la ruta de neroAacEnc.

    if (selectOption == 1)
        system("dir /B /A-D > input.tmp");                                      // Crea el archivo con la lista de archivos del directorio actual.
    else
        system("dir /S /B /A-D > input.tmp");                                   // Crea el archivo con la lista de archivos del directorio actual y los subdirectorios.

    input = fopen("input.tmp", "r+");                                           // Abre el archivo de entrada.
    while (!feof(input)) {                                                      // Ciclo infinito para buscar la línea más larga.
        fgets(line, 1000, input);                                               // Obtiene la siguiente línea del archivo.
        if (feof(input))                                                        // Si terminó de recorrer el archivo.
            break;                                                              // Sale del ciclo.
        elif (!isVideoFile(line) && !isAudioFile(line))                         // Si no es archivo de audio ni de video lo omite.
            continue;                                                           // Pasa al siguiente archivo.

        if (selectOption == 1) {                                                // Si solo se toman los archivos del directorio actual.
            fullPath = getFullPath(line);                                       // Obtiene la ruta absoluta del archivo actual.
            if (maxSize < strlen(fullPath))                                     // Si la ruta es la más larga hasta ahora.
                maxSize = strlen(fullPath);                                     // Almacena la cantidad de caracteres en maxSize.
            free(fullPath);                                                     // Libera la memoria ocupada por fullPath.
        }
        elif (selectOption == 2)                                                // Si se toman los archivos del directorio actual y los subdirectorios.
            if (maxSize < strlen(line))                                         // Si la ruta es la más larga hasta ahora.
            maxSize = strlen(line);                                             // Almacena la cantidad de caracteres en maxSize.
    }
    fclose(input);                                                              // Cierra input.

    // printf("'%d'\n", maxSize);                                               // Muestra la ruta más larga.

    input = fopen("input.tmp", "r+");                                           // Abre el archivo de entrada.
    output = fopen("output.bat", "w+");                                         // Crea el archivo de salida.

    // Ciclo infinito.
    while (!feof(input)) {                                                      // Recorre input.
        fgets(line, 1000, input);                                               // Obtiene la siguiente línea del archivo.
        if (feof(input))                                                        // Si terminó de recorrer el archivo.
            break;                                                              // Sale del ciclo.
        line[strlen(line) - 1] = 0;                                             // Quita salto de línea al final de line.
        // printf("'%s'\n", line);                                              // Muestra la línea.

        if (!isVideoFile(line) && !isAudioFile(line))                           // Si no es archivo de audio ni de video lo omite.
            continue;                                                           // Pasa al siguiente archivo.

        fileName = getName(line);                                               // Almacena en fileName el nombre del archivo.

        fprintf(output, "start \"ffmpeg\" /High /B /WAIT \"%s\"", ffmpeg);      // Almacena la ruta de ffmpeg en el output.

        if (selectOption == 1) {                                                // Si solo se toman los archivos del directorio actual.
            fullPath = getFullPath(line);                                       // Obtiene la ruta absoluta del archivo actual.
            fprintf(output, " -i     \"%s\"", fullPath);                        // Almacena la ruta absoluta del archivo actual en output.

            for (int i = 0; i < maxSize - strlen(fullPath) + 4; i++)            // Almacena tantos espacios como la ruta más larga menos la actual.
                fprintf(output, " ");                                           // Almacena un espacio en output.

            free(fullPath);                                                     // Libera la memoria ocupada por fullPath.
        } else {                                                                // Si se toman los archivos del directorio actual y los subdirectorios.
            fprintf(output, " -i     \"%s\"", line);                            // Almacena el archivo actual en output.
            for (int i = 0; i < maxSize - strlen(line) + 4; i++)                // Almacena tantos espacios como la ruta más larga menos la actual.
                fprintf(output, " ");                                           // Almacena un espacio en output.
        }

        fprintf(output, "-ar 48000 -ac 2 -f wav - | \"%s\" ", neroAacEnc);      // Almacena comando de conversión a wav del archivo en output.
        fprintf(output, "%s -if - -of    \"%s.aac\"\n", neroFlags, fileName);   // Almacena las flags de nero y el nombre del archivo en output.
    }

    fclose(input);                                                              // Cierra el archivo de entrada.
    fclose(output);                                                             // Cierra el archivo de salida.

    while (remove("input.tmp") == -1);                                          // Elimina input.tmp.
}

/**
 * FIXME: Definición de {@code isVideoFile}.
 * Verifica si un archivo es un video.
 */
static char isVideoFile(char* file) {
    char* format = getFormat(file);
    // Obtiene el formato de file.
    if (!cmpArs(format, (char*)".avi", 4) &&
        !cmpArs(format, (char*)".mp4", 4) &&
        !cmpArs(format, (char*)".mkv", 4) &&
        !cmpArs(format, (char*)".m2ts", 5) &&
        !cmpArs(format, (char*)".ts", 3))
        return 0;
    else
        return 1;
}

/**
 * FIXME: Definición de {@code isAudioFile}.
 * Verifica si un archivo es un audio.
 */
static char isAudioFile(char* file) {
    char* format = getFormat(file);
    // Obtiene el formato de file.
    if (!cmpArs(format, (char*)".aac", 4) &&
        !cmpArs(format, (char*)".ac3", 4) &&
        !cmpArs(format, (char*)".mp3", 4) &&
        !cmpArs(format, (char*)".wav", 4) &&
        !cmpArs(format, (char*)".flac", 5))
        return 0;
    else
        return 1;
}
