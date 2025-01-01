#include <iostream>
#include <chrono>
#include "LZ78.h"


int main() {
	string inputFileName, outputFileName;
	double time;  // 单位ms

	// 定义对象
	LZ78 lz78 = LZ78(16);  // 最长编码长度要和压缩时相同

	cout << "请输入要解压的文件文件名：";
	cin >> inputFileName;
	outputFileName = inputFileName.substr(0, inputFileName.length() - 4) + "_decompress.txt";  // 解压文件自动命名
	cout << "正在解压……" << endl;

	// 开始解压并计时（包含读取和写入时间）
	auto start = std::chrono::steady_clock::now();
	lz78.decompress(inputFileName, outputFileName);
	auto end = std::chrono::steady_clock::now();
	time = std::chrono::duration<double, std::milli>(end - start).count();  // 以ms为单位

	// 输出信息
	cout << "解压完成，解压文件保存为" << outputFileName << endl;
	cout << "解压耗时：" << time << "ms" << endl;

	system("pause");

	return 0;
}