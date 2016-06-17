#pragma once

#include "VLRPUtil.h"
#include "Page.h"

#define P 8

// ��ҳ���ʵ��������ļ��Ľӿڣ������ѷ�װ���ڣ�����ͬʱ���´����ļ��ķ�ʽ��
class RandomAccessMemory
{

public:

    // �����������������ļ��ſ��Ի�ȡ����
    static void openfile(const char *);
    static RandomAccessMemory *getInstance();

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

    static bool opened;

    RandomAccessMemory();
    ~RandomAccessMemory();

    static std::fstream fs;

    static Page *cache;

    static size_t I, O, replaceCount;
    static clock_t Itime, Otime;

};
