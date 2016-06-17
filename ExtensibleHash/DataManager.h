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

    //char *getAndLock();
    // ���ػ����е����ݵ�ַ
    //char *readAndLock(index_t pid);
    // ��ҳidд���ļ�
    //void writeAndUnlock(char *, index_t pid, bool check = true);

    Cache *getBlockAndLock();
    Cache *getBlockAndLock(index_t pid);
    Cache *writeBlockAndUnlock(Cache *, index_t pid);
    Cache *writeBlockAndUnlock(Cache *);
    Cache *lock(Cache *);
    Cache *unlock(Cache *);
    Cache *replace();

    std::fstream fs;

private:

    DataManager();

    // �Ӵ��̶�ȡҳ���û��������һҳ�������������ҳ������ҳ������0�Է�����������ҳ�����
    char *readAndReplace(index_t pid);

    Cache *cache;

    //int cur;  // ����ʱ��ҳ���㷨����ʾ��һ������ҳ��λ��

};
