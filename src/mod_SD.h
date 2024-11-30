#ifndef MOD_SD_H
#define MOD_SD_H

#include "Module.h"
#include <string>
extern "C" {
#include "ff_stdio.h"
}

class mod_SD : public Module {
public:
    void Init() override;
    void Tick() override;
    void Test() override;

    bool Mount(); //for use by the microcontroller application
    bool Unmount();
    bool Mounted() { return open_count>0; }

    //these fail if the card is locked
    bool WriteFile(const char *path, const void *data, unsigned int size);
    const std::string ReadFile(const char *path);
    bool AppendFile(const char *path, const void *data, unsigned int size);

    FF_FILE *fopen(const char *pcFile, const char *pcMode);
    int fclose(FF_FILE *pxStream);
    int stat(const char *pcFileName, FF_Stat_t *pxStatBuffer);
    size_t fwrite(const void *pvBuffer, size_t xSize, size_t xItems, FF_FILE *pxStream);
    size_t fread(void *pvBuffer, size_t xSize, size_t xItems, FF_FILE *pxStream);
    int chdir(const char *pcDirectoryName);
    char *getcwd(char *pcBuffer, size_t xBufferLength);
    int mkdir(const char *pcPath);
    int fputc(int iChar, FF_FILE *pxStream);
    int fgetc(FF_FILE *pxStream);
    int rmdir(const char *pcDirectory);
    int remove(const char *pcPath);
    long ftell(FF_FILE *pxStream);
    int fseek(FF_FILE *pxStream, int iOffset, int iWhence);
    int findfirst(const char *pcDirectory, FF_FindData_t *pxFindData);
    int findnext(FF_FindData_t *pxFindData);
    FF_FILE *truncate(const char *pcFileName, long lTruncateSize);
    int seteof(FF_FILE *pxStream);
    int rename(const char *pcOldName, const char *pcNewName, int bDeleteIfExists);
    char *fgets(char *pcBuffer, size_t xCount, FF_FILE *pxStream);

    //lock the card for direct access using diskio functions
    bool lock();
    bool unlock();

    uint32_t sectors = 0;
    uint32_t open_count = 0;
    bool incr_count();
    bool decr_count();
    bool locked = false;
};

#endif // MOD_SD_H