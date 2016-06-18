// ExtensibleHash.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ExtensibleHash.h"

#define MAXKEY 6000000
#define QUERY_TIMES 100000
//#define QUERY_FROM_FILE

int main(int argc, char **argv) {

    printf("������ʹ��%d���ڴ�ҳ����%sλ��չ�Ĺ�ϣ������\n", P,
#ifdef MOST
        "��"
#else
        "��"
#endif // MOST 
    );

    // Ĭ��·��Ϊ��ǰ·��
    char path[BUFFER_SIZE] = "./";
    if (argc > 1)  // ��һ������Ĭ��Ϊ·��
        strcpy_s(path, argv[1]);

    const char *dataFilename = "lineitem.tbl",
        *indexFilename = "hashindex.out",
        *inputFilename = "testinput.in",
        *outputFilename = "testoutput.out";
    char fullpath[BUFFER_SIZE] = {};

    // ����Ƿ���������ļ�
    bool indexExist = true;
    sprintf_s(fullpath, "%s%s", path, indexFilename);
    std::ifstream indexFile(fullpath);
    if (!indexFile.is_open())
        indexExist = false;
    else
        indexFile.close();

    sprintf_s(fullpath, "%s%s", path, indexFilename);
    RandomAccessMemory::openfile(fullpath);
    RandomAccessMemory *RAM = RandomAccessMemory::getInstance();
    ExtensibleHash hash(fullpath);
    char line[BUFFER_SIZE] = {};

    if (!indexExist) {
        sprintf_s(fullpath, "%s%s", path, dataFilename);
        std::ifstream dataFile(fullpath);
        if (!dataFile.is_open()) {
            printf("�������ļ� \"%s\" ʧ��\n", dataFilename);
            system("pause");
            return 0;
        }

        int start = clock();
        printf("���ڴ��������ļ�������\n");

        while (!dataFile.eof()) {
            dataFile.getline(line, sizeof(line));
            std::stringstream ss(line);
            int orderKey;
            ss >> orderKey;
            int state = ss.rdstate();
            if (state)
                break;
            hash.insert(std::make_pair(orderKey, line));
        }
        dataFile.close();

        int end = clock();
        printf("����������ʱ�� %dms\n", end - start);
        printf("����IO��ʱ�� %dms��IO������%d\n", RAM->getIOtime(), RAM->getIOs());
    }
    else {
        printf("�����ļ� \"%s\" �Ѵ��ڡ�\n", indexFilename);
    }

    //hash.check(6100000);

    printf("��ϣĿ¼ҳ����%d\n", hash.getListPages());
    printf("��ϣͰ����%d\n", hash.getBuckets());

    printf("��ѯ���Կ�ʼ��\n");
#ifdef QUERY_FROM_FILE
    sprintf_s(fullpath, "%s%s", path, inputFilename);
    std::ifstream inputFile(fullpath);
    if (!inputFile.is_open()) {
        printf("�������ļ� \"%s\" ʧ��\n", inputFilename);
        system("pause");
        return 0;
    }
    sprintf_s(fullpath, "%s%s", path, outputFilename);
    std::ofstream outputFile(fullpath);
    size_t count = 0;
    clock_t lastIOtime = hash.dm.getIOtime();
    while (!inputFile.eof()) {
        inputFile.getline(line, sizeof(line));
        key_t key;
        sscanf_s(line, "%d", &key);
        if (key == -1)
            break;
        count++;
        clock_t begin = clock();
        auto vector = hash.query(key);
        clock_t end = clock();
        clock_t curIOtime = hash.dm.getIOtime();
        printf("��ѯ%d��ɡ���ʱ%dms������IO��ʱ%dms\n", count, end - begin, curIOtime - lastIOtime);
        lastIOtime = curIOtime;
        auto string = vector.get();
        size_t len = vector.size();
        for (size_t i = 0; i < len; i++) {
            outputFile << string[i].c_str() << "\n";
        }
        outputFile << "-1\n";
    }
    inputFile.close();
    outputFile.close();
#else
    sprintf_s(fullpath, "%s%s", path, outputFilename);
    std::ofstream outputFile(fullpath);
    size_t count = 0;
    clock_t totaltime = 0, IOtime = 0;
    clock_t lastIOtime = RAM->getIOtime();
    srand(time(NULL));
    while (count <= QUERY_TIMES) {
        count++;
        key_t key = ((rand() << 15) + rand()) % MAXKEY;
        clock_t begin = clock();
        auto vector = hash.query(key);
        clock_t end = clock();
        totaltime += end - begin;
        clock_t curIOtime = RAM->getIOtime();
        IOtime += curIOtime - lastIOtime;
        lastIOtime = curIOtime;
        auto string = vector.get();
        size_t len = vector.size();
        for (size_t i = 0; i < len; i++) {
            outputFile << string[i].c_str() << "\n";
        }
        outputFile << "-1\n";
    }
    outputFile.close();
    printf("���%d�������ѯ��ʱ%dms������IO��ʱ%dms\n", QUERY_TIMES, totaltime, IOtime);
#endif // QUERY_FROM_FILE
    printf("��ѯ���Խ�����\n");

    printf("������������\n");
    system("pause");
    return 0;
}
