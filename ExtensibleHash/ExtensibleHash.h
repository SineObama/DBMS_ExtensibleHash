#pragma once

#include "Vector.h"
#include "myString.h"
#include "DataManager.h"

//#define NDEBUG

//#define MOST
#define BUFFER_SIZE 1000  // ���浥����¼

#ifndef NDEBUG
#define debug_memcpy(dst, src, size) memcpy(dst, src, size)
#else
#define debug_memcpy(dst, src, size)
#endif // !NDEBUG

typedef int key_t;

/*	��ϣ�����ļ���ʽ��
    ��0ҳ�൱�ڡ��ļ�ͷ������2��size_t�������ִ�����ҳ����ȫ�����
    ��1ҳ��ʼ˳��洢Ŀ¼��Ŀ¼ÿһ����2�����֣�index_t���͵�ҳid��size_t���͵ľֲ����*/
class ExtensibleHash
{

public:

    ExtensibleHash(const char *);
    ~ExtensibleHash();

    int insert(std::pair<key_t, char *>);
    Vector<myString> query(key_t);

    const char *check(int maxKey);

    DataManager dm;

private:

    //index_t *index2pid(index_t);

    size_t pages, gdept;

    static const size_t index_l = sizeof(index_t);
    static const size_t key_l = sizeof(key_t);
    static const size_t size_l = sizeof(size_t);
    static const size_t item_l = index_l + size_l;
    static size_t N;  // ÿһҳ�ܴ�ŵ�Ŀ¼������ȡ2�������η�

};
