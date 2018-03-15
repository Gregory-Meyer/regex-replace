#include <algorithm>

#include <string>  
#include <cstring>  
#include <stdlib.h>
#include <stdio.h>
#include "time.h"

#include <chrono>

using namespace std;

void GetLen(FILE * fp, int &len);
void ChangeExtension(char *fname, const char *newExt);
int StrReplace(char *target, const char *needle, const char *replacement, int len);
void CPPReplace(std::string &data);

int main(int c, char**v)
{
    FILE *fp = stdin;
    int len = 0;
    char *buf = NULL;
    char fname[256];
    time_t totalC = 0;

    if (c < 2)
    {
        puts("I need filename(s), like this...");
        puts("fixcsv.exe file.txt");
        return 0;
    }

    for (int i = 1; i < c; i++)
    {
        fp = fopen(v[i], "r");
        GetLen(fp, len);
        buf = (char*)calloc(1, len * 2);
        if (!buf)
        {
            puts("Wow! That file's too big for me. Out of memory!");
            return -1;
        }

        fread(buf, len, 1, fp);
        fclose(fp);
        // std::string data = buf;
        const auto start = std::chrono::high_resolution_clock::now();
        int result = StrReplace(buf, ",", ",\n", len * 2);
        if (result)
        {
            puts("Something went wrong. I have to quit!");
            return -1;
        }
        const auto end = std::chrono::high_resolution_clock::now();
        const auto elapsed = end - start;

        using DurationT = std::chrono::duration<double, std::ratio<1>>;

        const DurationT elapsed_seconds = elapsed;
        printf("C Runtime: %f\n", elapsed_seconds.count());
        strcpy(fname, v[i]);
        ChangeExtension(fname, "csv");
        fp = fopen(fname, "w");
        fwrite(buf, strlen(buf), 1, fp);
        fclose(fp);
    }
    // printf("Total Runtimes\nCPP with std::string: %f\nC with stdlib: %f\n", (float)totalCPP / CLOCKS_PER_SEC, (float)totalC / CLOCKS_PER_SEC);
    // printf("In this example, C was %f times faster than C++\n", (float)(totalCPP / totalC));
    // puts("Press return to continue");
    // getchar();
    return 0;
}

void GetLen(FILE * fp, int &len)
{
    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    rewind(fp);
}

void ChangeExtension(char *fname, const char *newExt)
{
#define EVAL_MAX_LEN (300)

    size_t nLen;
    char szOut[EVAL_MAX_LEN] = { 0 };
    char szPath[EVAL_MAX_LEN];
    strcpy(szPath, fname);
    nLen = strlen(szPath);
    if ((nLen > 0) && (nLen < EVAL_MAX_LEN))
    {

        while (nLen)
        {
            if (szPath[nLen] == '.')
            {
                szPath[nLen] = '\0';
                break;
            }
            nLen--;
        }
        sprintf(szOut, "%s.%s", szPath, newExt);
        strcpy(fname, szOut);
    }
    return;
}

int StrReplace(char *const target, const char *const needle,
               const char *const replacement, int len) {
    char *const buffer = new char[len];

    if (!buffer) {
        puts("Wow! That file's too big for me. Out of memory!");
        return -1;
    }

    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (true) {
        const char *p = strstr(tmp, needle);
        if (p == NULL)
        {
            strcpy(insert_point, tmp);
            break;
        }
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;
        tmp = p + needle_len;
    }

    strcpy(target, buffer);
    delete[] buffer;
    return 0;
}
