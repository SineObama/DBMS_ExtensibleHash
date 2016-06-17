#pragma once

#include "Vector.h"
#include "myString.h"
#include "DataManager.h"

//#define MOST
#define BUFFER_SIZE 1000  // ���浥����¼

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

private:

    //index_t *index2pid(index_t);

    //const char *checkItem(const char *item, size_t gpages);

    DataManager dm;

    size_t pages, gdept;

    static const size_t index_l = sizeof(index_t);
    static const size_t key_l = sizeof(key_t);
    static const size_t size_l = sizeof(size_t);
    static const size_t item_l = index_l + size_l;
    static size_t N;  // ÿһҳ�ܴ�ŵ�Ŀ¼������ȡ2�������η�
};
