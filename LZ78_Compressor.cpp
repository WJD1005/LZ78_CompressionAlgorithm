#include <iostream>
#include <chrono>
#include <sys/stat.h>
#include "LZ78.h"


int main() {
	string inputFileName, outputFileName;
	struct stat statbuf;
	unsigned long long inputSize, outputSize;  // ��λB
	double compressionRatio;  // ѹ���ȣ�ѹ��ǰ��ѹ����
	double time;  // ��λms

	// �������
	LZ78 lz78 = LZ78(16);  // ����볤�ȿ����Լ�ָ����ʵ��������볤�ȵ�16������

	cout << "������Ҫѹ�����ļ��ļ�����";
	cin >> inputFileName;
	outputFileName = inputFileName.substr(0, inputFileName.length() - 4) + ".lzc";  // ѹ���ļ��Զ�����Ϊԭ�ļ�.lzc
	cout << "����ѹ������" << endl;

	// ��ʼѹ������ʱ��������ȡ��д��ʱ�䣩
	auto start = std::chrono::steady_clock::now();
	lz78.compress(inputFileName, outputFileName);  
	auto end = std::chrono::steady_clock::now();
	time = std::chrono::duration<double, std::milli>(end - start).count();  // ��msΪ��λ

	// ����ѹ����
	stat(inputFileName.c_str(), &statbuf);
	inputSize = statbuf.st_size;
	stat(outputFileName.c_str(), &statbuf);
	outputSize = statbuf.st_size;
	compressionRatio = (double)inputSize / outputSize;

	// �����Ϣ
	cout << "ѹ����ɣ�ѹ���ļ�����Ϊ" << outputFileName << endl;
	cout << "ѹ����ʱ��" << time << "ms" << endl;
	cout << "ѹ��ǰ��С��" << inputSize << "B" << endl;
	cout << "ѹ�����С��" << outputSize << "B" << endl;
	cout << "ѹ���ȣ�" << compressionRatio << endl;
	cout << "ѹ���ʣ�" << 100 / compressionRatio << "%" << endl;

	system("pause");

	return 0;
}