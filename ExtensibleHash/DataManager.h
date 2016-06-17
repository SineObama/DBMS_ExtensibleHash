#pragma once

#include "PageManager.h"
#include "Cache.h"

#define P 128
#define MAXPID 10000000

// ��ҳ���ʵ��������ļ��Ľӿڣ������ѷ�װ���ڣ�����ͬʱ���´����ļ��ķ�ʽ��
class DataManager
{

public:

    DataManager(const char *);
    ~DataManager();

    Cache *getBlockAndLock();
    Cache *getBlockAndLock(index_t pid);
    Cache *writeBlockAndUnlock(Cache *, index_t pid);
    Cache *writeBlockAndUnlock(Cache *);
    Cache *lock(Cache *);
    Cache *unlock(Cache *);
    Cache *replace();

    std::fstream fs;
    int replace_count;

private:

    DataManager();

    Cache *cache;

};
