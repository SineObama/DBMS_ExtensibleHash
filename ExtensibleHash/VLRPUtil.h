#pragma once

#define L 8192

typedef unsigned int index_t;

// �ṩ�䳤��¼ҳ�ĸ�����
class VLRPUtil
{

public:

    VLRPUtil(char *);
    ~VLRPUtil();

    // ����ֵint(0)��ʾ������(-1)��ʾ����
    int insert(const char *);
    int insert(const char *, size_t);
    int get(char *, size_t *, index_t) const;
    size_t getSpace() const;

private:

    char *_data;
    size_t *_n;
    index_t *_end;

    static const size_t size_l = sizeof(size_t);
    static const size_t index_l = sizeof(index_t);
    static const size_t item_l = size_l + index_l;

};
