#include <iostream>
#include <chrono>
#include "LZ78.h"


int main() {
	string inputFileName, outputFileName;
	double time;  // ��λms

	// �������
	LZ78 lz78 = LZ78(16);  // ����볤��Ҫ��ѹ��ʱ��ͬ

	cout << "������Ҫ��ѹ���ļ��ļ�����";
	cin >> inputFileName;
	outputFileName = inputFileName.substr(0, inputFileName.length() - 4) + "_decompress.txt";  // ��ѹ�ļ��Զ�����
	cout << "���ڽ�ѹ����" << endl;

	// ��ʼ��ѹ����ʱ��������ȡ��д��ʱ�䣩
	auto start = std::chrono::steady_clock::now();
	lz78.decompress(inputFileName, outputFileName);
	auto end = std::chrono::steady_clock::now();
	time = std::chrono::duration<double, std::milli>(end - start).count();  // ��msΪ��λ

	// �����Ϣ
	cout << "��ѹ��ɣ���ѹ�ļ�����Ϊ" << outputFileName << endl;
	cout << "��ѹ��ʱ��" << time << "ms" << endl;

	system("pause");

	return 0;
}