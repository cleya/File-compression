#include"HuffmanTree.h"
#include<iostream>
#include<assert.h>
#include<vector>
#include<windows.h>
using namespace std;

typedef long long LongType;
struct CharInfo          //相当于HuffmanTree中的weight
{
	unsigned char _ch;  //字符(注意unsigned char!!!)
	LongType _count;    //次数
	string _code;       //编码

	CharInfo(int count = 0)
		:_ch(0)
		,_count(count)
	{}
	bool operator < (const CharInfo& info)const 
	{
		return _count < info._count;
	}
	bool operator != (const CharInfo& info)const 
	{
		return _count != info._count;
	}
	CharInfo operator + (const CharInfo& info)
	{
		return CharInfo(info._count + _count);
	}
};

class FileCompress
{
public:
	FileCompress()
	{
		for(int i = 0;i < 256;i++)
		{
			_infos[i]._ch = i;
		}
	}
public:
	void Compress(const char* filename)
	{
		assert(filename);
		LongType totalSize = 0;   //文件中所有字符的总数

		//1.打开文件，统计字符出现的次数
		FILE* p_file = fopen(filename,"rb");
		while(1)
		{
			char ch = fgetc(p_file);  //从fp指向的文件中读取一个字符
			//EOF作为文件结束标志，十进制为-1.当数据以二进制形式存放到文件中时，会有-1的出现，因此不能采用EOF作为二进制文件的结束标志。feof函数既可用以判断二进制文件又可用以判断文本文件。
			if(feof(p_file))
				break;
			_infos[(unsigned char)ch]._count++;
			++totalSize;
		}
		//2.根据字符出现的次数构建Huffman树
		HuffmanTree<CharInfo> tree;
		tree.CreateHuffmanTree(_infos,256);
		//3.生成每个字符对应的Huffman编码
		_GenerateHuffmanCode(tree.GetRootNode());

		//4.将压缩编码写入压缩文件中
		string compressedFile = filename;
		compressedFile += ".compression";
		FILE* p_comFile = fopen(compressedFile.c_str(),"wb"); //新建一个二进制文件只允许写
		assert(p_comFile);

		  ////每次对编码读一位，当满8位即为1个字的时候，对字进行读取，写入到压缩文件中。
		fseek(p_file,0,SEEK_SET);    //设置文件指针p_file的位置，指向文件开头的位置
		int index = 0;
		char value = 0;
		while(1)
		{
			char ch = fgetc(p_file);
			if(feof(p_file))
				break;
			string& code = _infos[(unsigned char)ch]._code;
			for(int i=0;i<code.size();++i)
			{
				value <<= 1;                                                                                                
				if(code[i] == '1')
				{
					value |= 1;
				}                         //
				if (++index == 8)
				{
					fputc(value,p_comFile);//  将字符inCh写到文件指针p_comFile所指向的文件的当前写指针的位置。
					index = 0;
					value = 0;
				}
			}
		}
		if(index != 0)   
		{
			value <<= (8 - index);
			fputc(value,p_file);
		}

		//5.将Huffman树的信息写配置文件，方便解压缩时重建Huffman树
		char str[20];
		string configFile = filename;
		configFile += ".config";
		FILE* p_conFile = fopen(configFile.c_str(),"wb");   //新建一个二进制文件，只允许写
		assert(p_conFile);

		//totalSize是long long型，当是大文件，文件中字符的总个数超过整型的表示范围时，需要分两段来写，先写高位的四个字节，再写低位的四个字节。
		//_itoa(totalSize >> 32,str,10);          //取高四位
		//fputs(str,p_conFile);                   //把str所指向的字符串写到文件指针变量fp所指向的文件中
		//fputc('\n',p_conFile);                  //把字符ch写到文件指针变量fp所指向的文件中
		//_itoa(totalSize & 0xffffffff,str,10);   //取低四位
		//fputs(str,p_comFile);
		//fputc('\n',p_conFile);

		for(int i=0;i<256;++i)
		{
			string chInfo;
			if(_infos[i]._count > 0)
			{
				chInfo += _infos[i]._ch ;
				chInfo += ',';
				chInfo += _itoa(_infos[i]._count,str,10);
				chInfo += '\n';
				fputs(chInfo.c_str(),p_conFile);
			}
		}
		fclose(p_file);
		fclose(p_comFile);
		fclose(p_conFile);
	}
	void Uncompress(const char* filename)
	{
		//1.读取配置文件中Huffman树的信息
		string configFile = filename;
		configFile += ".config";
		FILE* f_conFile = fopen(configFile.c_str(),"rb");//打开一个二进制文件
		string line;
		char ch = 0; 

		//读取文件的字符个数
		/*LongType totalCount = 0;
		_readLine(f_conFile,line);
		totalCount = atoi(line.c_str());
		totalCount <<= 32;
		line.clear();
		_readLine(f_conFile,line);
		totalCount += atoi(line.c_str());
		line.clear();*/

		while(_readLine(f_conFile,line))
		{
			//若读到一个空行，即统计的字符为空行，则对应字符为换行符。
			if(!line.empty())
			{
				ch = line[0];
				_infos[(unsigned char)ch]._count = atoi(line.substr(2).c_str());
				line.clear();
			}
			else
				line = '\n';
		}

		//2.根据读出的配置信息，重建Huffman树
		HuffmanTree<CharInfo> tree;
		tree.CreateHuffmanTree(_infos,256);
		 //Huffman树的根结点的权值就是文件中所有字符出现次数的总和
		HuffmanNode<CharInfo>* root = tree.GetRootNode();
		LongType totalCount = 0;
		totalCount = root->_weight._count;
		cout<<"解压缩"<<totalCount<<"个字符"<<endl;

		//3.读取压缩信息，根据重建的Huffman树解压缩
		string uncompressedFile = filename;
		uncompressedFile += ".Uncompression";
		FILE* p_uncomFile = fopen(uncompressedFile.c_str(),"wb");

		string compressedFile = filename;
		compressedFile += ".compression";
		FILE* p_comFile = fopen(compressedFile.c_str(),"rb");

		//4.根据压缩文件字符编码在Huffman树中寻找对应的字符
		int pos = 8;
		HuffmanNode<CharInfo>* cur = root;
		ch = fgetc(p_comFile);
		while(1)
		{
			--pos;
			if(ch & (1<<pos))
				cur = cur->_right;
			else
				cur = cur->_left;
			if(cur->_left == NULL && cur->_right == NULL)
			{
				fputc(cur->_weight._ch,p_uncomFile);
				cur = root;
				if(--totalCount == 0)
					break;
			}
			if(pos == 0)
			{
				ch = fgetc(p_comFile);
				pos = 8;
			}
		}
		fclose(p_comFile);
		fclose(p_uncomFile);
		fclose(f_conFile);

	}

protected:
	//生成Huffman编码
	void _GenerateHuffmanCode(HuffmanNode<CharInfo>* root)
	{
		if(root == NULL)
			return;
		_GenerateHuffmanCode(root->_left);
		_GenerateHuffmanCode(root->_right);

		//叶子结点为编码结点，顺着叶子结点向上寻找Huffman编码
		if(root->_left == NULL && root->_right == NULL) 
		{
			HuffmanNode<CharInfo>* cur = root;
			HuffmanNode<CharInfo>* parent = cur->_parent;
			string& code = _infos[cur->_weight._ch]._code;
			while(parent)
			{
				if(parent->_left == cur)
				{
					code += '0';
				}
				else
				{
					code += '1';
				}	
				cur = parent;
				parent = cur->_parent;
			}
			 //code是从叶子结点到根结点，所以要逆置编码。string中reverse()函数，颠倒字符串中字符的次序。
			reverse(code.begin(),code.end());

		}
	}
	/*void _GenerateHuffmanCode(HuffmanNode<CharInfo>* root,string code)
	{
		if(root)
		{
			_GenerateHuffmanCode(root->_left,code + '0');
			_GenerateHuffmanCode(root->_right,code + '1');
			if(root->_left == NULL && root->_right == NULL)
			{
				_infos[root->_weight._ch]._code = code;
			} 
		}
	}*/
	bool _readLine(FILE* fp,string& line)
	{
		assert(fp);
		char ch = fgetc(fp);
		if(feof(fp))
			return false;
		while(ch != '\n')
		{
			line += ch;
			ch = fgetc(fp);;
			if(feof(fp))
				break;
		}
		return true;
	}
private:
	CharInfo _infos[256];    //最多有256个字符
};

void TestCompress()
{
	FileCompress fc;

	int begin = GetTickCount();
	fc.Compress("Input.BIG");
	int end = GetTickCount();

	cout<<"Compress:"<<end - begin<<endl;
}
void TestUncompress()
{
	FileCompress fc;
	int begin = GetTickCount();
	fc.Uncompress("Input.BIG");
	int end = GetTickCount();

	cout<<"Uncompress:"<<end - begin<<endl;
}

int main()
{
	TestCompress();
	TestUncompress();
	getchar();
	return 0;
}