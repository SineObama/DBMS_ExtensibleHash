// ExtensibleHash.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ExtensibleHash.h"

int main(int argc, char **argv) {

    // Ĭ��·��Ϊ��ǰ·��
    char path[BUFFER_SIZE] = "./";
    if (argc > 1)  // ��һ������Ĭ��Ϊ·��
        strcpy_s(path, argv[1]);

    const char *dataFilename = "lineitem6.tbl",
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
        printf("����IO��ʱ�� %dms\n", hash.dm.getIOtime());
    }
    else {
        printf("�����ļ� \"%s\" �Ѵ��ڡ�\n", indexFilename);
    }

    //hash.check(6100000);

    printf("��ϣĿ¼ҳ����%d\n", hash.getListPages());
    printf("��ϣͰ����%d\n", hash.getBuckets());

    printf("��ѯ���Կ�ʼ��\n");

    sprintf_s(fullpath, "%s%s", path, inputFilename);
    std::ifstream inputFile(fullpath);
    if (!inputFile.is_open()) {
        printf("�������ļ� \"%s\" ʧ��\n", inputFilename);
        system("pause");
        return 0;
    }
    sprintf_s(fullpath, "%s%s", path, outputFilename);
    std::ofstream outputFile(fullpath);
    while (!inputFile.eof()) {
        inputFile.getline(line, sizeof(line));
        key_t key;
        sscanf_s(line, "%d", &key);
        if (key == -1)
            break;
        auto vector = hash.query(key);
        auto string = vector.get();
        size_t len = vector.size();
        for (size_t i = 0; i < len; i++) {
            outputFile << string[i].c_str() << "\n";
        }
        outputFile << "-1\n";
    }
    inputFile.close();
    outputFile.close();


    printf("��ѯ���Խ�����\n");

    printf("������������\n");
    system("pause");
    return 0;
}
