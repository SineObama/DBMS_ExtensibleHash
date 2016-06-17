#pragma once

#include "VLRPUtil.h"
#include "Page.h"

#define P 128

// ��ҳ���ʵ��������ļ��Ľӿڣ������ѷ�װ���ڣ�����ͬʱ���´����ļ��ķ�ʽ��
class RandomAccessMemory
{

public:

    RandomAccessMemory(const char *);
    ~RandomAccessMemory();

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

    RandomAccessMemory();

    std::fstream fs;

    Page *cache;

    size_t I, O, replaceCount;
    clock_t Itime, Otime;

};
