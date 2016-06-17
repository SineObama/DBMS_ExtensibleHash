#pragma once

#include "PageManager.h"
#include "Page.h"

#define P 128

// ��ҳ���ʵ��������ļ��Ľӿڣ������ѷ�װ���ڣ�����ͬʱ���´����ļ��ķ�ʽ��
class DataManager
{

public:

    DataManager(const char *);
    ~DataManager();

    Page *getAndLock();
    Page *getAndLock(index_t pid);
    Page *writeAndUnlock(Page *, index_t pid);
    Page *writeAndUnlock(Page *);
    Page *lock(Page *);
    Page *unlock(Page *);
    Page *replace();

    size_t getIOs();
    clock_t getIOtime();
    size_t getReplacements();

private:

    DataManager();

    std::fstream fs;

    Page *cache;

    size_t I, O, replaceCount;
    clock_t Itime, Otime;

};
