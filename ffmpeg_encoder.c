/*
 * @fileoverview    {FileName}
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
#include "ffmpeg_encoder.h"
#include "file_item.h"
#include "message.h"
#include "operation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void convertVideos(MessageList* paths, int selectOption, int useNeroAAC, FileList* videos, FileList* subtitles);
static void makeLog(time_t start, time_t end, char* logFileName, char* inputFile, char* outputFile, char* ffmpeg, char* aac, char* subtitles);

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
    char* ProgramDir = getFatherPath(argv[0]);                                          // Crea array con el directorio en donde se encuentra el ejecurable actual.
    char path[265] = "";                                                                // Crea array que tendrá la ruta del archivo con las rutas.
    sprintf(path, "%s\\config\\paths.ini", ProgramDir);                                 // Almacena la ruta del archivo de rutas en pathsFile.
    MessageList* paths = makePathList(path, ProgramDir);                                // Obtiene los messages del archivo con las rutas.

    // printf("\npaths:\n");                                                            // Muestra mensaje.
    // prtMsgList(paths);                                                               // Muestra las rutas.

    MessageList* messages = makeMsgList(getPath(messages-ffmpeg));                      // Obtiene los messages del archivo con los mensajes a mostar en pantalla.
    // printf("\nMessages:\n");                                                         // Muestra mensaje.
    // prtMsgList(messages);                                                            // Muestra los mensajes a mostar en pantalla.

    Message* aux = messages->first;                                                     // Crea mensaje con el primer mensaje de la lista de mensajes.
    while (aux != NULL) {                                                               // Recorre todos los mensajes.
        replaceUTF8CharToAscii(aux->value);                                             // Reemplaza los caracteres utf-8 por caracteres ascii en el mensaje actual.
        aux = aux->next;                                                                // Pasa al siguiente mensaje.
    }

    // printf("\nMessages:\n");                                                         // Muestra mensaje.
    // prtMsgList(messages);                                                            // Muestra los mensajes a mostar en pantalla.

    int selectOption = 0;                                                               // Crea variable que tendrá la opción seleccionada por el usuario.
    int useNeroAAC = 0;                                                                 // Crea variable que dirá si se usa neroAAC para convertir los videos.

#define printMessage(msj) printf("\n%s", getMessage(msj))                               // Crea macro para imprimir los mensajes.
    printMessage(selectOptionMsg);                                                      // Lista las opciones.
    printMessage(option01Msg);                                                          // Opción 01.
    printMessage(option02Msg);                                                          // Opción 02.
    printMessage(option03Msg);                                                          // Opción 03.
    printMessage(option04Msg);                                                          // Opción 04.
    printMessage(exitOptMsg);                                                           // Salir.
    printf("\n");                                                                       // Muestra salto de linea.
    scanf("%d", &selectOption);                                                         // Pregunta al usuario por la opción.

    if (selectOption >= 1 && selectOption <= 4) {                                       // Evalúa que sea una opción válida.
        FileList* videos = makeFileList((char*)"", (char*)".avi .mp4 .mkv .m2ts .ts");  // Crea lista de archivos y la llena de archivos de video.
        printf("\nVideos:\n");                                                          // Imprime mensaje.
        printFileList(videos);                                                          // Muestra la lista de videos.

        FileList* subtitles = makeFileList((char*)"", (char*)".ass .str");              // Crea lista de archivos y la llena de archivos de subtitulos.
        printf("\nSubtitles:\n");                                                       // Imprime mensaje.
        printFileList(subtitles);                                                       // Muestra la lista de subtitulos.

        printf("\n");                                                                   // Imprime salto de línea.

        if (selectOption >= 3 && videos->first != NULL) {                               // Si se indicó convertir audio y hay almenos un video.
            printMessage(neroAACMsg);                                                   // Pregunta si quiere usar neroAAC.
            printf("\n");                                                               // Muestra salto de linea.
            scanf("%d", &useNeroAAC);                                                   // Almacena la opción la opción.
        }

        convertVideos(paths, selectOption, useNeroAAC, videos, subtitles);              // Llama al de conversión de videos.
    }

#undef printMessage                                                                     // Borra la macro para imprimir los mensajes.

    return 0;
}

/**
 * FIXME: Definición de {@code convertVideos}.
 * Convierte los videos.
 */
static void convertVideos(MessageList* paths, int selectOption, int useNeroAAC, FileList* videos, FileList* subtitles) {
    char neroFlags[2000] = " ";                                                             // Crea array para almacenar las flags de nero del archivo de flags.
    char audioFlags[2000] = " ";                                                            // Crea array para almacenar las flags de audio del archivo de flags.
    char videoFlags[2000] = " ";                                                            // Crea array para almacenar las flags de video del archivo de flags.
    char container[20] = "";                                                                // Crea array para almacenar la flag con el contenedor del archivo de salida.
    char* aux = NULL;                                                                       // Crea apuntador a array pare recorrer cada archivo de flags.
    FILE* flags = NULL;                                                                     // Crea apuntador archivo para abrir todos los archivos de flags.
    char** videoSplits = NULL;                                                              // Crea variable para obtener cada flag de video.

    aux = &neroFlags[1];                                                                    // Asigna a aux las la primera posición de las flags de nero.
    flags = fopen(getPath(flags-NeroAAC), "r+");                                            // Abre el archivo de flags de nero.
    while (!feof(flags)) {                                                                  // Recorre el archivo con las flags de nero.
        fgets(aux, 2000, flags);                                                            // Concatena la siguiente línea del archivo de flags de nero con la actual.

        aux += strlen(aux);                                                                 // Pone aux al final de la línea leída.
        if (aux[-1] == '\n')                                                                // Si el último caracter de la linea es un salto de línea.
            aux[-1] = ' ';                                                                  // Convierte el salto de línea en espacio.
    }
    fclose(flags);                                                                          // Cierra el archivo de flags de nero.

    aux = &audioFlags[1];                                                                   // Asigna a aux las la primera posición de las flags de audio.
    flags = fopen(getPath(flags-Audio), "r+");                                              // Abre el archivo de flags de audio.
    while (!feof(flags)) {                                                                  // Recorre el archivo con las flags de audio.
        fgets(aux, 2000, flags);                                                            // Concatena la siguiente línea del archivo de flags de audio con la actual.

        aux += strlen(aux);                                                                 // Pone aux al final de la línea leída.
        if (aux[-1] == '\n')                                                                // Si el último caracter de la linea es un salto de línea.
            aux[-1] = ' ';                                                                  // Convierte el salto de línea en espacio.
    }
    fclose(flags);                                                                          // Cierra el archivo de flags de audio.

    aux = &videoFlags[1];                                                                   // Asigna a aux las la primera posición de las flags de video.
    flags = fopen(getPath(flags-Video), "r+");                                              // Abre el archivo de flags de video.
    while (!feof(flags)) {                                                                  // Recorre el archivo con las flags de video.
        fgets(aux, 2000, flags);                                                            // Concatena la siguiente línea del archivo de flags de video con la actual.

        aux += strlen(aux);                                                                 // Pone aux al final de la línea leída.
        if (aux[-1] == '\n')                                                                // Si el último caracter de la linea es un salto de línea.
            aux[-1] = ' ';                                                                  // Convierte el salto de línea en espacio.
    }
    fclose(flags);                                                                          // Cierra el archivo de flags de video.
    replaceWord(videoFlags, videoFlags, "\t", " ");                                         // Reemplaza tabulaciones por espacios en las flags de video.

    videoSplits = splitArray(videoFlags, (char*)" -");                                      // Separa las flags de video obtenidas.
    sprintf(videoFlags, "%s", videoSplits[0]);                                              // Agrega la primera flag al array de flags de vidoe.
    for (int i = 1; i <= 1000; i++)                                                         // Recorre las flags de video obtenidas.
        if (videoSplits[i] != NULL) {                                                       // Si no es la última flag.
            // printf("\"%s\"\n", videoSplits[i]);
            if (!strncmp(videoSplits[i], (char*)"container", 9)) {                          // Si la flag tiene el contenedor.
                sprintf(container, "%s", &videoSplits[i][10]);                              // Almacena el contenedor en container.
                replaceWord(container, container, " ", "");                                 // Quita espacios al contenedor.
            } else                                                                          // Si no es un flag de audio.
                sprintf(videoFlags, "%s -%s", videoFlags, videoSplits[i]);                  // Agrega la flag actual al vector de flags de video.
        } else                                                                              // Si terminó de recorrer las flags.
            break;                                                                          // Sale del ciclo.

    /*-
    printf("\nFlags used:\n");                                                              // Imprime mensaje.
    printf("\"%s\"\n", neroFlags);                                                          // Muestra las flags de nero almacenadas.
    printf("\"%s\"\n", audioFlags);                                                         // Muestra las flags de audio almacenadas.
    printf("\"%s\"\n", videoFlags);                                                         // Muestra las flags de video almacenadas.
    printf("\"%s\"\n", container);                                                          // Muestra el contenedor de salida.
    */

    FileItem* vid = videos->first;                                                          // Crea FileItem vid con el primer FileItem de la lista de videos.
    FileItem* subs = NULL;                                                                  // Crea FileItem subs para recorrer la lista de subtitulos.
    char outputName[1024] = "";                                                             // Variable que tendrá el nombre de salida de cada archivo de video.

    time_t start, end;                                                                      // Crea indicadores de tiempo.
    char buf[5000] = "";                                                                    // Crea array para almacenar el comando de conversión o remux de cada video.
    char aac[1000] = "";                                                                    // Crea array para almacenar los comandos de encodeo de audio con nero aac.
    char* subName = NULL;                                                                   // Crea array para almacenar e archivo de subtitulos asociado a cada video (si lo hay).
    char log[20] = "ffmpeg.log";                                                            // Crea array de char con el nombre del archivo log.
    FILE* info = NULL;                                                                      // Crea apuntador a archivo para abrir el archivo con el id de video y de audio.
    char line[50] = "";                                                                     // Crea array para recorrer cada línea de 'info.tmp'.
    int videoId = 0;                                                                        // Crea variable para almacenar cada id de cada video.
    char* ffmpeg = getPath(ffmpeg);                                                         // Crea array con la ruta de ffmpeg.
    char* ffprobe = getPath(ffprobe);                                                       // Crea array con la ruta de ffprobe.
    char* neroAacEnc = getPath(neroAacEnc);                                                 // Crea array con la ruta de neroAacEnc.

    while (vid != NULL) {                                                                   // Mientras no recorra toda la lista de videos.
        start = time(NULL);                                                                 // Toma la hora de inicio de la conversión.
        getFormat(vid->name)[0] = 0;                                                        // Quita el formato al archivo de video.

        // Evalúa la opción escogida para poner nombre al archivo de salida.
        if (selectOption == 1)
            sprintf(outputName, "%s (Remux aud-vid)%s", vid->name, container);
        elif (selectOption == 2)
            sprintf(outputName, "%s (Remux aud convert vid)%s", vid->name, container);
        elif (selectOption == 3)
            sprintf(outputName, "%s (Remux vid convert aud)%s", vid->name, container);
        elif (selectOption == 4)
            sprintf(outputName, "%s (Convert aud-vid)%s", vid->name, container);

        printf("Output is \"%s\"\n", outputName);                                           // Muestra el nombre del archivo de salida.
        vid->name[strlen(vid->name)] = '.';                                                 // Pone formato al video.

        if (selectOption == 3 || selectOption == 4) {                                       // Si se indicó convertir audio.
            if (useNeroAAC) {                                                               // Si se indicó usar neroAACEnc.
                sprintf(buf, "start \"Ffprobe\" /High /B /WAIT \"%s\"", ffprobe);           // Almacena en buf la ruta de ffprobe.
                sprintf(buf, "%s -v quiet -select_streams v:0 -show_entries", buf);         // Indica a buf que se mostrara información del primer stream con el video.
                sprintf(buf, "%s stream=index -i \"%s\" > info.tmp", buf, vid->name);       // Indica que mostrará el indice del video y redireccionará la salida a un archivo.
                system(buf);                                                                // Ejecuta buf.
                // printf("%s\n", buf);                                                     // Muestra buf.
                info = fopen("info.tmp", "r+");                                             // Abre info.tmp.
                fgets(line, 50, info);                                                      // Almacena en line la primera línea de info.tmp.
                if (strcmp(line, "[STREAM]")) {                                             // Si el video actual tiene un stream de video.
                    fgets(line, 50, info);                                                  // Almacena en line el id del video.
                    videoId = atoi(&line[6]);                                               // Almacena en videoId el id del video.
                } else                                                                      // Si el video actual no tiene un stream de video.
                    videoId = -1;                                                           // Almacena -1 indicando que no hay stream de video.
                fclose(info);                                                               // Cierra info.tmp.
                while (remove("info.tmp") == -1);                                           // Elimina info.tmp.

                // printf("Id is: %d\n", videoId);                                          // Muestra el id del video.

                sprintf(aac, "start \"ffmpeg\" /High /B /WAIT \"%s\"", ffmpeg);             // Almacena la ruta de ffmpeg en aac.
                sprintf(aac, "%s -i \"%s\" -ar 48000 -ac 2 -f wav - |", aac, vid->name);    // Almacena el nombre del video en aac.
                sprintf(aac, "%s \"%s\" %s -if - -of", aac, neroAacEnc, neroFlags);         // Almacena la ruta de nero y las flags de nero en aac.
                sprintf(aac, "%s \"audio.m4a\"\n", aac);                                    // Almacena nombre del archivo de salida en aac.
                // printf("'%s'\n", aac);                                                   // Muestra el comando de conversión de audio.
                system(aac); // Ejecuta el comando de conversión de audio.

                sprintf(buf, "start \"ffmpeg\" /High /B /WAIT \"%s\"", ffmpeg);             // Almacena la ruta de ffmpeg en buf.
                sprintf(buf, "%s -i \"%s\" -i \"audio.m4a\"", buf, vid->name);              // Almacena el nombre del video y ruta del audio en buf.
                sprintf(buf, "%s -map 0:%d -map 1:0  -c:a copy -strict -2", buf, videoId);  // Agrega id del video y del audio en buf.
            } else {                                                                        // Si no se indicó usar neroAac.
                sprintf(buf, "start \"ffmpeg\" /High /B /WAIT \"%s\"", ffmpeg);             // Almacena la ruta de ffmpeg en buf.
                sprintf(buf, "%s -i \"%s\" %s", buf, vid->name, audioFlags);                // Almacena el nombre del video y las flags de audio en buf.
            }
        } else {                                                                            // Si se indicó no convertir audio.
            sprintf(buf, "start \"ffmpeg\" /High /B /WAIT \"%s\"", ffmpeg);                 // Almacena la ruta de ffmpeg en buf.
            sprintf(buf, "%s -i \"%s\"  -c:a copy -strict -2", buf, vid->name);             // Almacena el nombre del video y comando de copia de audio en buf.
        }

        // printf("'%s'\n", buf);                                                           // Muestra el comando de conversión.

        if (selectOption == 2 || selectOption == 4) {                                       // Si se indicó convertir video.
            getFormat(vid->name)[0] = 0;                                                    // Quita el formato al archivo de video.
            subs = subtitles->first;                                                        // A subs le asigna el primer archivo de la lista de subtitulos.

            while (subs != NULL) {                                                          // Recorre la lista de subtitulos para ver si hay alguno con igual nombre al video.
                if (compareArrays(subs->name, vid->name, strlen(vid->name)))                // Si el nombre de los subtitulos es igual al del video.
                    break;                                                                  // Sale del ciclo.
                subs = subs->next;                                                          // Pasa al siguiente archivo.
            }

            vid->name[strlen(vid->name)] = '.';                                             // Pone el formato al archivo de video.

            if (subs != NULL) {                                                             // Si hay archivo de subtitulos asociado al video.
                subName = subs->name;                                                       // Almacena el nombre de los subtitulos.
                sprintf(buf, "%s -vf \"subtitles='%s'\"", buf, subName);                    // Almacena los subtitulos en el comando de conversión de video.
            } else                                                                          // Si no hay archivo de subtitulos.
                subName = NULL;                                                             // Pone vacío el nombre de los subtitulos.

            sprintf(buf, "%s %s  \"%s\"  > NUL\n", buf, videoFlags, outputName);            // Almacena las flags de video en el comando de conversión de video.
        } else                                                                              // Si no se indicó convertitr video.
            sprintf(buf, "%s  -c:v copy  \"%s\"  > NUL\n", buf, outputName);                // Almacena comando para hacer remux al video.

        // printf("'%s'\n", buf);                                                           // Muestra el comando de conversión.
        system(buf);                                                                        // Ejecuta el comando de conversión.
        end = time(NULL);                                                                   // Toma la hora de fin de la conversión.
        makeLog(start, end, log, vid->name, outputName, buf, aac, subName);                 // Agrega información al log.

        if (useNeroAAC && (selectOption == 3 || selectOption == 4))                         // Si se utilizó el neroAac.
            remove("audio.m4a");                                                            // Elimina audio.m4a.

        vid = vid->next;                                                                    // Pasa al siguiente video.
    }
}

/**
 * FIXME: Definición de {@code makeLog}.
 * Agrega información al archivo de log indicado por logFileName.
 */
static void makeLog(time_t start, time_t end, char* logFileName, char* inputFile, char* outputFile, char* ffmpeg, char* aac, char* subtitles) {
    struct tm* tm;                                                                                      // Crea indicadores de tiempo.
    tm = localtime(&start);                                                                             // A start le asigna la hora de inicio de la conversión.
    int hour = tm->tm_hour;                                                                             // A hour le asigna la hora de inicio de la conversión.
    int min = tm->tm_min;                                                                               // A min le asigna los minutos de inicio de la conversión.
    int sec = tm->tm_sec;                                                                               // A sec le asigna los segundos de inicio de la conversión.
    tm = localtime(&end);                                                                               // A end le asigna la hora de fin de la conversión.
    double inputSize = 0.0;                                                                             // Variable para almacenar el tamaño del archivo de entrada.
    double outputSize = 0.0;                                                                            // Variable para almacenar el tamaño del archivo de salida.
    FILE* log = fopen(logFileName, "r+");                                                               // Abre el log para edición.

    if (log == NULL) {                                                                                  // Si el log no existe (primera información a escribir en el log).
        log = fopen(logFileName, "w");                                                                  // Crea el log.
        fprintf(log, "Encodes:\n");                                                                     // Escribe encabezado en el log.
    } else                                                                                              // Si el log existe.
        log = fopen(logFileName, "a");                                                                  // Se ubica al final del archivo.

    // Escribe nombre del archivo de salida, hora de inicio y hora de fin de la conversión.
    fprintf(log, "\n\nInput file:       %s\n", inputFile);
    fprintf(log, "Output file:     %s\n", outputFile);

    int totalSeconds = (int)(end - start);                                                              // Calcula la cantidad de segundos que tardó la conversión o descarga.

    inputSize = getFileSize(inputFile, 2);
    fprintf(log, "Input size:       %4.2f MB\n", inputSize);
    outputSize = getFileSize(outputFile, 2);
    fprintf(log, "Output size:     %4.2f MB\n", outputSize);

    if (!inputSize)
        fprintf(log, "Percent output:   %4.2f%c\n", 0.0, 37);
    else
        fprintf(log, "Percent output:   %4.2f%c\n", (outputSize / inputSize) * 100, 37);

    if (aac[0] != 0)                                                                                    // Si se utilizó neroAac.
        fprintf(log, "NeroAAC command:   %s", aac);                                                     // Escribe el comando de conversión aac.

    if (subtitles != NULL)                                                                              // Si el video llevaba subtitulos.
        fprintf(log, "Subtitles:         %s\n", subtitles);                                             // Escribe el nombre de los subtitulos.

    fprintf(log, "FFmpeg command:   %s", ffmpeg);                                                       // Escribe el comando de conversión.
    fprintf(log, "Date:             %02d/%02d/%d\n", tm->tm_mday, tm->tm_mon + 1, 1900 + tm->tm_year);  // Escribe la fecha de la conversión.
    fprintf(log, "Start time:       %02d:%02d:%02d\n", hour, min, sec);                                 // Escribe la hora de inicio de la conversión.
    fprintf(log, "End time:         %02d:%02d:%02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);            // Escribe la hora de fin de la conversión.

    hour = totalSeconds / 3600;                                                                         // Calcula la cantidad de horas que tardó la conversión.
    totalSeconds %= 3600;
    min = totalSeconds / 60;                                                                            // Calcula la cantidad de minutos que tardó la conversión.
    totalSeconds %= 60;
    sec = totalSeconds;                                                                                 // Calcula la cantidad de segundos que tardó la conversión.

    fprintf(log, "Time Elapsed:   %02d:%02d:%02d\n", hour, min, sec);                                   // Escribe cuanto tardó la conversión en el log.
    fclose(log);                                                                                        // Cierra el log.
}
