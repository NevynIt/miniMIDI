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

    bool Mount();
    bool Unmount();
    bool WriteFile(const std::string &path, const void *data, unsigned int size);
    bool ReadFile(const std::string& path, std::string& data);

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

    bool mounted;
};

#endif // MOD_SD_H