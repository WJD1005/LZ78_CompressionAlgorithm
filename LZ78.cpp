#include "LZ78.h"


// ָ����������䳤���볤�ȣ�ͬʱָ���ֵ�����С���������
LZ78::LZ78(unsigned int cusMaxCodeSizeBit) {
	if (cusMaxCodeSizeBit > CODE_SIZE_LIMIT_BIT) {
		cusMaxCodeSizeBit = CODE_SIZE_LIMIT_BIT;
	}
	maxCodeSizeBit = cusMaxCodeSizeBit;
	maxDicSize = (1 << maxCodeSizeBit) - 1;  // ��������С
}

// ѹ��
unsigned int LZ78::compress(const string &inputFilePath, const string &outputFilePath) {
	unordered_map<string, unsigned int> dic;  // ʹ�ù�ϣ����Ϊ�ֵ䣬������ѯ�ٶ�
	string input;
	string prefix = "", ch;
	unsigned int index;
	unsigned int indexLen = 1;  // �����䳤���볤��
	unsigned int indexLenMask = 2;  // �������ȼ�����룬�ӵ�2λ��ʼ���
	
	// ���ļ�
	ifstream inputFile(inputFilePath, ios::in);  // �ı�ģʽ��
	if (!inputFile) {
		cout << "Read file error!" << endl;
		return 0;
	}
	stringstream buffer;
	buffer << inputFile.rdbuf();
	input = buffer.str();
	
	// ��Ҫд��ѹ�����ݵ��ļ�
	ofstream outputFile(outputFilePath, ios::out | ios::binary);  // ������д��

	// LZ78ѹ�����㷨
	for (unsigned int i = 0; i < input.length(); ++i) {
		ch = input.substr(i, 1);  // ȡ����ǰ�ַ�

		// �����ֵ����Ƿ��е�ǰ׺����
		if (dic.count(prefix + ch)) {
			prefix = prefix + ch;  // �еĻ��ͼ���ƴ��
		}
		else {
			// û�еĻ���ʼ�������
			if (prefix.empty()) {
				index = 0;  // ���ǰ׺�ǿյ��������Ϊ0
			}
			else {
				index = dic[prefix];  // ǰ׺���վ����ֵ�����ǰ׺������
			}
			// �����������ñ䳤�����Ե�ʣ�������Ҫ�ڽ�������ı����������ÿ����һ�ֽ������
			encodeWrite(outputFile, index, indexLen);  // ��������
			encodeWrite(outputFile, (unsigned char)ch[0], 8);  // �����ַ�

			// ���µ�׺�������뵽�ֵ���
			// �ȿ����ֵ䵽��󳤶�û��
			if (dic.size() == maxDicSize) {
				dic.clear();  // �Ѿ�����󳤶��ˣ���������ֵ�
				indexLen = 1;  // ��λһЩ��¼����
				indexLenMask = 2;
			}
			dic[prefix + ch] = dic.size() + 1;
			// �жϵ�ǰ�ֵ��������λ���Ըı������䳤���볤��
			if (dic.size() >= indexLenMask) {
				++indexLen;  // �����Ѿ��䳤��һλ
				indexLenMask <<= 1;
			}
			prefix.clear();  // ���ǰ׺��¼
		}
	}
	// ����һ�µ�ĩβ����ǰ׺�������������ʱprefix���ǿյ�
	if (!prefix.empty()) {
		encodeWrite(outputFile, dic[prefix], indexLen);  // ������ǰ׺����д�룬û���ַ�ֻ��ǰ׺����
	}
	encodeWriteEnd(outputFile);
	inputFile.close();
	outputFile.close();
	return 1;
}

// ��ѹ
unsigned int LZ78::decompress(const string& inputFilePath, const string& outputFilePath) {
	unordered_map<unsigned int, string> dic;  // ʹ�ù�ϣ����Ϊ�ֵ䣬������ѯ�ٶȣ�Ҫע������ֵ��ֵ���Ƿ���
	string input;
	string prefix = "", ch, str;
	unsigned int index, temp;
	unsigned int indexLen = 1;  // �����䳤���볤��
	unsigned int indexLenMask = 2;  // �������ȼ�����룬�ӵ�2λ��ʼ���

	// ���ļ�
	ifstream inputFile(inputFilePath, ios::in | ios::binary);  // �����ƶ��ļ�
	if (!inputFile) {
		cout << "Read file error!" << endl;
		return 0;
	}
	stringstream buffer;
	buffer << inputFile.rdbuf();
	input = buffer.str();

	// ��Ҫд���ѹ���ݵ��ļ�
	ofstream outputFile(outputFilePath, ios::out);  // �ı�ģʽд��

	// LZ78��ѹ���㷨
	// ��Ϊ�����˱䳤����ֻ������һ������
	while (1) {
		index = decodeRead(input, indexLen);  // ����ǰ���ȶ�һ�ζ����Ʊ�ʾ����
		if (index == 0xFFFFFFFF) {
			inputFile.close();
			outputFile.close();
			return 1;  // ����
		}
		temp = decodeRead(input, 8);  // �ٶ�8λ����
		// ����һ��ѹ�����ǰ׺��ȫƥ��ֻ������û���ַ����������ʱ�ַ�Ӧ�ö�������
		if (temp == 0xFFFFFFFF) {
			outputFile.write((const char*)dic[index].c_str(), dic[index].length());  // ������������û���ַ���ֱ��д��ǰ׺�ͽ���
			inputFile.close();
			outputFile.close();
			return 1;
		}
		else {
			ch = (unsigned char)temp;
		}
		
		// ���������Ϊ0����ֵ���ǰ׺������ƴ�ϵ�ǰ���żӵ��ֵ�����
		if (index != 0) {
			prefix = dic[index];
			// �ȿ����ֵ䵽��󳤶�û��
			if (dic.size() == maxDicSize) {
				dic.clear();  // �Ѿ�����󳤶��ˣ���������ֵ�
				indexLen = 1;  // ��λһЩ��¼����
				indexLenMask = 2;
			}
			dic[dic.size() + 1] = prefix + ch;
		}
		else {
			// ����Ϊ0��ֱ�Ӱѷ��żӵ��ֵ���
			// �ȿ����ֵ䵽��󳤶�û��
			if (dic.size() == maxDicSize) {
				dic.clear();  // �Ѿ�����󳤶��ˣ���������ֵ�
				indexLen = 1;  // ��λһЩ��¼����
				indexLenMask = 2;
			}
			dic[dic.size() + 1] = ch;
		}
		// �жϵ�ǰ�ֵ��������λ���Ը��������䳤���볤�ȣ���Ȼ��ȫ���������
		if (dic.size() >= indexLenMask) {
			++indexLen;  // �����Ѿ��䳤��һλ
			indexLenMask = indexLenMask << 1;
		}
		// д���ѹ����
		str = prefix + ch;
		outputFile.write((const char*)str.c_str(), str.length());
		prefix.clear();  // ���ǰ׺
	}
	decodeReadEnd();
}

///////////////////////////������һЩʵ�������䳤�����ĸ�������///////////////////////////

// �䳤����д�뺯������Ҫ��ʽ�����ڻ����ֽ����Ȱ�λ���룬����һ���ֽڲ���д���ļ�
// value�п���λ�������ݣ����ᰴ�Ӹߵ��͵�˳�򽫵�lengthλ�Ķ����Ʊ��뵽���
// ����Ŀǰ��������λ�����䳤�����ʵ�ֻ����е�����ֻ��Ϊ���ѹ������ʱ������������
// �Ͼ�һ��ʹ���˱䳤����Ͳ������������ֽڶ��룬��д����С��λ���ֽڣ�������û��д�룬ֻ��λ��������
void LZ78::encodeWrite(ofstream &outputFile, unsigned int value, unsigned int length) {
	unsigned int readBitMask = 1 << (length - 1);  // ��λ�����룬���Ƶ���Ҫ��������λ��ʼ
	for (int i = 0; i < length; ++i) {
		if (value & readBitMask) {
			encodeByte |= encodeBitMask;  // ��1��д1
		}
		if (encodeBitMask == 0x01) {
			outputFile.write((char*)&encodeByte, 1);  // һ�ֽ����ˣ�д�뻺���ֽ�
			encodeBitMask = 0x80;  // д��һ�ֽ��ˣ�����ص����λ
			encodeByte = 0;  // �����ֽ����
		}
		else {
			encodeBitMask >>= 1;  // ��������д�����ֽ�
		}
		readBitMask >>= 1;  // �������ƶ�ԭ��
	}
}

// ����д�������������ʣ��δ�չ�һ�ֽڵı��ز�0�չ���д��
void LZ78::encodeWriteEnd(ofstream &outputFile) {
	// ���ڳ�ʼʱ�����ֽ�ȫ��0���������貹ֱ��������������ֽڼ���
	outputFile.write((char*)&encodeByte, 1);
	// ��ԭ���б���
	encodeByte = 0;
	encodeBitMask = 0x80;
}

// �䳤������뺯������Ҫ������λ������
// ʵ���Ͼ��ǽ�lengthλ�������ת��Ϊunsigned int����
unsigned int LZ78::decodeRead(string& input, unsigned int length) {
	unsigned int writeBitMask = 1 << (length - 1);  // ��λд���룬���Ƶ�����Ŀ�������λ��ʼ
	unsigned int value = 0;
	for (int i = 0; i < length; ++i) {
		// �����λ�����������������λ�Ļ���֤��Ҫ����һ�ֽ���
		if (decodeBitMask == 0x80) {
			// û����ż��������
			if (decodeByteIndex < input.size()) {
				decodeByte = input[decodeByteIndex];
				++decodeByteIndex;
			}
			else {
				return 0xFFFFFFFF;  // ��ʾ�����ˣ�������������󳤶Ȳ����ܻᵽ�������
			}
		}
		if (decodeByte & decodeBitMask) {
			value |= writeBitMask;  // ��1д1
		}
		if (decodeBitMask == 0x01) {
			decodeBitMask = 0x80;
		}
		else {
			decodeBitMask >>= 1;
		}
		writeBitMask >>= 1;
	}
	return value;
}

void LZ78::decodeReadEnd(void) {
	// ֻ�Ǹ�ԭһЩ����
	decodeByteIndex = 0;
	decodeByte = 0;
	decodeBitMask = 0x80;
}
