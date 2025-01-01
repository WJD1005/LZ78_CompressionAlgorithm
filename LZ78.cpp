#include "LZ78.h"


// 指定最大索引变长编码长度（同时指定字典最大大小）构造对象
LZ78::LZ78(unsigned int cusMaxCodeSizeBit) {
	if (cusMaxCodeSizeBit > CODE_SIZE_LIMIT_BIT) {
		cusMaxCodeSizeBit = CODE_SIZE_LIMIT_BIT;
	}
	maxCodeSizeBit = cusMaxCodeSizeBit;
	maxDicSize = (1 << maxCodeSizeBit) - 1;  // 计算最大大小
}

// 压缩
unsigned int LZ78::compress(const string &inputFilePath, const string &outputFilePath) {
	unordered_map<string, unsigned int> dic;  // 使用哈希表作为字典，拉满查询速度
	string input;
	string prefix = "", ch;
	unsigned int index;
	unsigned int indexLen = 1;  // 索引变长编码长度
	unsigned int indexLenMask = 2;  // 索引长度检测掩码，从第2位开始检测
	
	// 读文件
	ifstream inputFile(inputFilePath, ios::in);  // 文本模式读
	if (!inputFile) {
		cout << "Read file error!" << endl;
		return 0;
	}
	stringstream buffer;
	buffer << inputFile.rdbuf();
	input = buffer.str();
	
	// 打开要写入压缩数据的文件
	ofstream outputFile(outputFilePath, ios::out | ios::binary);  // 二进制写入

	// LZ78压缩主算法
	for (unsigned int i = 0; i < input.length(); ++i) {
		ch = input.substr(i, 1);  // 取出当前字符

		// 查找字典中是否有当前缀符串
		if (dic.count(prefix + ch)) {
			prefix = prefix + ch;  // 有的话就继续拼接
		}
		else {
			// 没有的话开始编码输出
			if (prefix.empty()) {
				index = 0;  // 如果前缀是空的输出索引为0
			}
			else {
				index = dic[prefix];  // 前缀不空就在字典里找前缀的索引
			}
			// 由于索引采用变长编码的缘故，这里需要在进行特殊的编码输出，即每编满一字节再输出
			encodeWrite(outputFile, index, indexLen);  // 编码索引
			encodeWrite(outputFile, (unsigned char)ch[0], 8);  // 编码字符

			// 将新的缀符串加入到字典中
			// 先看看字典到最大长度没有
			if (dic.size() == maxDicSize) {
				dic.clear();  // 已经到最大长度了，清空整个字典
				indexLen = 1;  // 复位一些记录变量
				indexLenMask = 2;
			}
			dic[prefix + ch] = dic.size() + 1;
			// 判断当前字典最大索引位长以改变索引变长编码长度
			if (dic.size() >= indexLenMask) {
				++indexLen;  // 长度已经变长了一位
				indexLenMask <<= 1;
			}
			prefix.clear();  // 清空前缀记录
		}
	}
	// 处理一下到末尾都有前缀的特殊情况，此时prefix不是空的
	if (!prefix.empty()) {
		encodeWrite(outputFile, dic[prefix], indexLen);  // 把最后的前缀索引写入，没有字符只有前缀索引
	}
	encodeWriteEnd(outputFile);
	inputFile.close();
	outputFile.close();
	return 1;
}

// 解压
unsigned int LZ78::decompress(const string& inputFilePath, const string& outputFilePath) {
	unordered_map<unsigned int, string> dic;  // 使用哈希表作为字典，拉满查询速度，要注意解码字典键值对是反的
	string input;
	string prefix = "", ch, str;
	unsigned int index, temp;
	unsigned int indexLen = 1;  // 索引变长编码长度
	unsigned int indexLenMask = 2;  // 索引长度检测掩码，从第2位开始检测

	// 读文件
	ifstream inputFile(inputFilePath, ios::in | ios::binary);  // 二进制读文件
	if (!inputFile) {
		cout << "Read file error!" << endl;
		return 0;
	}
	stringstream buffer;
	buffer << inputFile.rdbuf();
	input = buffer.str();

	// 打开要写入解压数据的文件
	ofstream outputFile(outputFilePath, ios::out);  // 文本模式写入

	// LZ78解压主算法
	// 因为采用了变长编码只能慢慢一点点解码
	while (1) {
		index = decodeRead(input, indexLen);  // 按当前长度读一段二进制表示索引
		if (index == 0xFFFFFFFF) {
			inputFile.close();
			outputFile.close();
			return 1;  // 结束
		}
		temp = decodeRead(input, 8);  // 再读8位符号
		// 处理一下压缩最后前缀完全匹配只存索引没有字符的情况，此时字符应该读不出来
		if (temp == 0xFFFFFFFF) {
			outputFile.write((const char*)dic[index].c_str(), dic[index].length());  // 特殊情况下最后没有字符，直接写入前缀就结束
			inputFile.close();
			outputFile.close();
			return 1;
		}
		else {
			ch = (unsigned char)temp;
		}
		
		// 如果索引不为0则查字典找前缀，并再拼上当前符号加到字典里面
		if (index != 0) {
			prefix = dic[index];
			// 先看看字典到最大长度没有
			if (dic.size() == maxDicSize) {
				dic.clear();  // 已经到最大长度了，清空整个字典
				indexLen = 1;  // 复位一些记录变量
				indexLenMask = 2;
			}
			dic[dic.size() + 1] = prefix + ch;
		}
		else {
			// 索引为0则直接把符号加到字典里
			// 先看看字典到最大长度没有
			if (dic.size() == maxDicSize) {
				dic.clear();  // 已经到最大长度了，清空整个字典
				indexLen = 1;  // 复位一些记录变量
				indexLenMask = 2;
			}
			dic[dic.size() + 1] = ch;
		}
		// 判断当前字典最大索引位长以更新索引变长编码长度，不然会全部解码错误
		if (dic.size() >= indexLenMask) {
			++indexLen;  // 长度已经变长了一位
			indexLenMask = indexLenMask << 1;
		}
		// 写入解压数据
		str = prefix + ch;
		outputFile.write((const char*)str.c_str(), str.length());
		prefix.clear();  // 清空前缀
	}
	decodeReadEnd();
}

///////////////////////////下面是一些实现索引变长编解码的辅助函数///////////////////////////

// 变长编码写入函数，主要方式就是在缓存字节中先按位编码，编满一个字节才能写入文件
// value中靠低位储存数据，将会按从高到低的顺序将低length位的二进制编码到输出
// 但是目前觉得这种位操作变长编码的实现还是有点慢，只能为提高压缩率在时间上做出牺牲
// 毕竟一旦使用了变长编码就不可能能做到字节对齐，而写入最小单位是字节，不对齐没法写入，只能位操作缓存
void LZ78::encodeWrite(ofstream &outputFile, unsigned int value, unsigned int length) {
	unsigned int readBitMask = 1 << (length - 1);  // 按位读掩码，左移到需要编码的最高位开始
	for (int i = 0; i < length; ++i) {
		if (value & readBitMask) {
			encodeByte |= encodeBitMask;  // 读1则写1
		}
		if (encodeBitMask == 0x01) {
			outputFile.write((char*)&encodeByte, 1);  // 一字节满了，写入缓存字节
			encodeBitMask = 0x80;  // 写完一字节了，掩码回到最高位
			encodeByte = 0;  // 缓存字节清空
		}
		else {
			encodeBitMask >>= 1;  // 持续右移写缓存字节
		}
		readBitMask >>= 1;  // 持续右移读原码
	}
}

// 编码写入结束函数，将剩余未凑够一字节的比特补0凑够后写入
void LZ78::encodeWriteEnd(ofstream &outputFile) {
	// 由于初始时缓存字节全是0，所以无需补直接输出整个缓存字节即可
	outputFile.write((char*)&encodeByte, 1);
	// 复原所有变量
	encodeByte = 0;
	encodeBitMask = 0x80;
}

// 变长编码解码函数，需要慢慢按位长解码
// 实际上就是将length位的码解码转换为unsigned int返回
unsigned int LZ78::decodeRead(string& input, unsigned int length) {
	unsigned int writeBitMask = 1 << (length - 1);  // 按位写掩码，左移到解码目的数最高位开始
	unsigned int value = 0;
	for (int i = 0; i < length; ++i) {
		// 如果按位读出编码掩码在最高位的话，证明要读下一字节了
		if (decodeBitMask == 0x80) {
			// 没读完才继续往后读
			if (decodeByteIndex < input.size()) {
				decodeByte = input[decodeByteIndex];
				++decodeByteIndex;
			}
			else {
				return 0xFFFFFFFF;  // 表示读完了，由于限制了最大长度不可能会到这个数的
			}
		}
		if (decodeByte & decodeBitMask) {
			value |= writeBitMask;  // 读1写1
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
	// 只是复原一些变量
	decodeByteIndex = 0;
	decodeByte = 0;
	decodeBitMask = 0x80;
}
