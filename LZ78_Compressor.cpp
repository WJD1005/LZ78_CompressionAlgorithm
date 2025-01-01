#include <iostream>
#include <chrono>
#include <sys/stat.h>
#include "LZ78.h"


int main() {
	string inputFileName, outputFileName;
	struct stat statbuf;
	unsigned long long inputSize, outputSize;  // 单位B
	double compressionRatio;  // 压缩比：压缩前比压缩后
	double time;  // 单位ms

	// 定义对象
	LZ78 lz78 = LZ78(16);  // 最长编码长度可以自己指定，实际上最长编码长度到16差不多得了

	cout << "请输入要压缩的文件文件名：";
	cin >> inputFileName;
	outputFileName = inputFileName.substr(0, inputFileName.length() - 4) + ".lzc";  // 压缩文件自动命名为原文件.lzc
	cout << "正在压缩……" << endl;

	// 开始压缩并计时（包含读取和写入时间）
	auto start = std::chrono::steady_clock::now();
	lz78.compress(inputFileName, outputFileName);  
	auto end = std::chrono::steady_clock::now();
	time = std::chrono::duration<double, std::milli>(end - start).count();  // 以ms为单位

	// 计算压缩比
	stat(inputFileName.c_str(), &statbuf);
	inputSize = statbuf.st_size;
	stat(outputFileName.c_str(), &statbuf);
	outputSize = statbuf.st_size;
	compressionRatio = (double)inputSize / outputSize;

	// 输出信息
	cout << "压缩完成，压缩文件保存为" << outputFileName << endl;
	cout << "压缩耗时：" << time << "ms" << endl;
	cout << "压缩前大小：" << inputSize << "B" << endl;
	cout << "压缩后大小：" << outputSize << "B" << endl;
	cout << "压缩比：" << compressionRatio << endl;
	cout << "压缩率：" << 100 / compressionRatio << "%" << endl;

	system("pause");

	return 0;
}