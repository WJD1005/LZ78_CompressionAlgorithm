#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
using namespace std;


#define CODE_SIZE_LIMIT_BIT 24  // 最长索引变长编码长度二进制位数的内定最大大小，定义对象时的指定的大小不可超过该值

// LZ78压缩算法类，输出编码为变长索引编码+8位ASCII编码，支持256个ASCII字符，引入变长索引编码以获得更大压缩比
// 最大索引变长编码长度可通过对象参数自定义，以获得不同的性能，会同时指定字典最大长度
// 最大字典长度和最大索引变长编码长度是等效的，因为变长编码长度是根据当前字典长度来确定的

class LZ78
{
public:
	unsigned int maxCodeSizeBit;  // 最大索引变长编码长度二进制位数，会同时指定最大字典大小
	unsigned int maxDicSize;  // 最大字典长度
	// 索引变长编码用
	unsigned char encodeByte = 0;  // 编码缓存字节
	unsigned char encodeBitMask = 0x80;  // 按位写入编码掩码，始终指着要写的下一位
	unsigned int decodeByteIndex = 0;  // 解码读字节指针
	unsigned char decodeByte = 0;  // 解码缓存字节
	unsigned char decodeBitMask = 0x80;  // 按位读出编码掩码，始终指着要读的下一位

	LZ78(unsigned int cusMaxDicSize);
	unsigned int compress(const string& inputFilePath, const string& outputFilePath);
	unsigned int decompress(const string& inputFilePath, const string& outputFilePath);

private:
	void encodeWrite(ofstream &outputFile, unsigned int value, unsigned int length);
	void encodeWriteEnd(ofstream &outputFile);
	unsigned int decodeRead(string& input, unsigned int length);
	void decodeReadEnd(void);
};