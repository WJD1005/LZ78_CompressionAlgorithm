#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
using namespace std;


#define CODE_SIZE_LIMIT_BIT 24  // ������䳤���볤�ȶ�����λ�����ڶ�����С���������ʱ��ָ���Ĵ�С���ɳ�����ֵ

// LZ78ѹ���㷨�࣬�������Ϊ�䳤��������+8λASCII���룬֧��256��ASCII�ַ�������䳤���������Ի�ø���ѹ����
// ��������䳤���볤�ȿ�ͨ����������Զ��壬�Ի�ò�ͬ�����ܣ���ͬʱָ���ֵ���󳤶�
// ����ֵ䳤�Ⱥ���������䳤���볤���ǵ�Ч�ģ���Ϊ�䳤���볤���Ǹ��ݵ�ǰ�ֵ䳤����ȷ����

class LZ78
{
public:
	unsigned int maxCodeSizeBit;  // ��������䳤���볤�ȶ�����λ������ͬʱָ������ֵ��С
	unsigned int maxDicSize;  // ����ֵ䳤��
	// �����䳤������
	unsigned char encodeByte = 0;  // ���뻺���ֽ�
	unsigned char encodeBitMask = 0x80;  // ��λд��������룬ʼ��ָ��Ҫд����һλ
	unsigned int decodeByteIndex = 0;  // ������ֽ�ָ��
	unsigned char decodeByte = 0;  // ���뻺���ֽ�
	unsigned char decodeBitMask = 0x80;  // ��λ�����������룬ʼ��ָ��Ҫ������һλ

	LZ78(unsigned int cusMaxDicSize);
	unsigned int compress(const string& inputFilePath, const string& outputFilePath);
	unsigned int decompress(const string& inputFilePath, const string& outputFilePath);

private:
	void encodeWrite(ofstream &outputFile, unsigned int value, unsigned int length);
	void encodeWriteEnd(ofstream &outputFile);
	unsigned int decodeRead(string& input, unsigned int length);
	void decodeReadEnd(void);
};