#include "stdafx.h"
#include "ExtensibleHash.h"

size_t ExtensibleHash::N = 0;

ExtensibleHash::ExtensibleHash(const char *filename) : dm(filename)
{
	// �ֶ���ʼ��N�����̬��������
	if (N == 0) {
		N = L / item_l;
		if (N == 0)
			throw std::exception("defined page-length is too small even for two number!");
		int max = 1 << 30;
		while (N & max == 0 && max != 0)
			max >>= 1;
		N = max;
	}

	char *page = dm.read(0);
	size_t *p = (size_t *)page;
	num = *p;
	if (num == 0) {  // һ�������ļ�
		// д���0ҳ
		num = *p = 3;
		gdept = *(p + 1) = 0;  // ������ڰ�д
		dm.write(page, 0, false);

		// д���1ҳ��Ŀ¼
		page = dm.read(1);
		*(index_t *)page = 2;
		*(index_t *)(page + index_l) = 0;
		dm.write(page, 1, false);

		// д��հ׵ĵ�2ҳ
		//page = dm.read(2);
		//dm.write(page, 2, false);
	}
	else {
		gdept = *(p + 1);
	}
}

ExtensibleHash::~ExtensibleHash() {}

int ExtensibleHash::insert(std::pair<key_t, char *> pair) {
	int watch[L / sizeof(int)];  // debug
	// �������ݼ�¼
	key_t key = pair.first;// wrong after 522, at 650
	char *val = pair.second;
	char *record = new char[key_l + strlen(val) + 1];
	*(key_t *)record = key;
	memcpy(record + key_l, val, strlen(val) + 1);

	// ���±���������gǰ׺����global����ʾĿ¼��أ�l����local����ʾ���ݼ�¼���
	while (true) {
		size_t gsize = 1 << gdept;
		index_t index = key % gsize;
		index_t gpid = index / N + 1;
		char *gpage = dm.read(gpid);
		index_t lpid = *(index_t *)(gpage + index % N * item_l);
		{
			char *lpage = dm.read(lpid);
			//dm.fs.seekg(15 * L);
			//dm.fs.read((char *)watch, L);

			VLRUtil pm(lpage);
			if (pm.insert(record, key_l + strlen(val)) == 0) {
				dm.write(lpage, lpid, false);
				break;
			}
		}
		// ����ʧ�ܣ�Ͱ���ˣ�
#ifdef MOST
		// todo
		exit(1);
#else
		size_t ldept = *(size_t *)(gpage + index % N * item_l + index_l);


		if (ldept == gdept) {  // ��ҪĿ¼�ӱ�
			if (gsize >= N) {  // ��Ҫ��������ҳ��ת�ƣ��ڳ�λ�÷�Ŀ¼���ټӱ�
				size_t pages = gsize / N;  // ת�Ƶ�ҳ��
				index_t begin = pages + 1, end = begin + pages;  // ת�Ƶķ�Χ

				// ת������ҳ
				size_t off = num - begin;
				for (index_t i = begin; i < end; i++) {
					char *p = dm.read(i);
					dm.write(p, i + off, false);
				}

				// ����Ŀ¼���ӱ������ƣ�
				for (index_t i = 1; i < begin; i++) {
					char *p = dm.read(i);
					char *cur = p;
					for (index_t j = 0; j < N; j++, cur += item_l) {
						index_t *index = (index_t *)cur;
						if (*index >= begin && *index < end) {
							*index += off;
						}
					}
					dm.write(p, i, true);
					dm.write(p, i + pages, true);
				}

				// �޸ĵ�0ҳ���ݣ�������ҳ��
				char *mpage = dm.read(0);
				size_t *mp = (size_t *)mpage;
				*mp = num = num + pages;
				dm.write(mpage, 0, false);
			}
			else {  // �ڵ�1ҳ�ڲ�Ŀ¼�ӱ������ƣ�
				char *p = dm.read(1);
				memcpy(p + gsize * item_l, p, gsize * item_l);
				dm.write(p, 1, false);
			}

			// �޸ĵ�0ҳ���ݣ�ȫ�����+1
			char *mpage = dm.read(0);
			size_t *mp = (size_t *)mpage;
			*(mp + 1) = ++gdept;
			dm.write(mpage, 0, false);

			//index_t i = 0, lastSrcPid = 0, lastDstPid = 0;
			//char *src = NULL, *dst = NULL;
			//for (; i < gsize; i++) {
			//	index_t srcPid = i / N + 1, dstPid = (i + gsize) / N + 1;
			//	if (srcPid != lastSrcPid) {
			//		lastSrcPid = srcPid;
			//		src = dm.read(srcPid);
			//	}
			//	if (dstPid != lastDstPid) {
			//		if (lastDstPid != 0)
			//			dm.write(dst, lastDstPid, false);
			//		lastDstPid = dstPid;
			//		dst = dm.read(dstPid);
			//	}
			//	memcpy(dst + (i + gsize) % N * item_l, src + i % N * item_l, item_l);
			//}
			//if (lastDstPid != 0)
			//	dm.write(dst, lastDstPid, false);
		}

		// ���ӽ�СĿ¼��ı������
		index_t index1 = index & ~(1 << ldept);
		index_t gpid1 = index1 / N + 1;
		char *gpage1 = dm.read(gpid1);
		size_t *ldept1 = (index_t *)(gpage1 + index1 % N * item_l + index_l);
		(*ldept1)++;
		dm.write(gpage1, gpid1, false);

		// ���ӽϴ�Ŀ¼��ı�����ȣ����ı�ҳid
		index_t index2 = index | (1 << ldept);
		index_t gpid2 = index2 / N + 1;
		char *gpage2 = dm.read(gpid2);
		index_t *lpid2 = (index_t *)(gpage2 + index2 % N * item_l);
		size_t *ldept2 = (index_t *)(gpage2 + index2 % N * item_l + index_l);
		index_t newpid = num;
		*lpid2 = newpid;
		(*ldept2)++;
		dm.write(gpage2, gpid2, false);

		// ��ȡ�յĻ���ҳ����ŷ��Ѻ��2ҳ
		char *lpage = dm.read(lpid);
		char *newpage1 = dm.read(newpid);
		char *newpage2 = dm.read(newpid + 1);

		VLRUtil pm(lpage), pm1(newpage1), pm2(newpage2);
		for (int i = 0;; i++) {
			char item[1000] = {};
			size_t length = sizeof(item);
			int state = pm.get(item, &length, i);
			if (state == -1)
				break;
			else if (state == -2)
				exit(1);
			int key = *(int *)item;
			if (key & (1 << ldept))
				pm2.insert(item, length);
			else
				pm1.insert(item, length);
		}

		dm.write(newpage1, lpid, true);
		dm.write(newpage2, newpid, true);

		// �޸ĵ�0ҳ���ݣ���ҳ��+1
		char *mpage = dm.read(0);
		size_t *mp = (size_t *)mpage;
		*mp = ++num;
		dm.write(mpage, 0, false);
#endif // MOST
	}

	delete[] record;
	return 0;
}

Vector<myString> ExtensibleHash::query(key_t key) {
	return Vector<myString>();
}

// ��Ŀ¼��ȡͰ��λ��
//index_t *ExtensibleHash::index2pid(index_t index) {
//    index_t pid = index / N + 1;
//    char *page = dm.read(pid);
//    pid = *((index_t *)(page)+index % N);
//    return &pid;
//}
