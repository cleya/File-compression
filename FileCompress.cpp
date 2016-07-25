#include"HuffmanTree.h"
#include<iostream>
#include<assert.h>
#include<vector>
#include<windows.h>
using namespace std;

typedef long long LongType;
struct CharInfo          //�൱��HuffmanTree�е�weight
{
	unsigned char _ch;  //�ַ�(ע��unsigned char!!!)
	LongType _count;    //����
	string _code;       //����

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
		LongType totalSize = 0;   //�ļ��������ַ�������

		//1.���ļ���ͳ���ַ����ֵĴ���
		FILE* p_file = fopen(filename,"rb");
		while(1)
		{
			char ch = fgetc(p_file);  //��fpָ����ļ��ж�ȡһ���ַ�
			//EOF��Ϊ�ļ�������־��ʮ����Ϊ-1.�������Զ�������ʽ��ŵ��ļ���ʱ������-1�ĳ��֣���˲��ܲ���EOF��Ϊ�������ļ��Ľ�����־��feof�����ȿ������ж϶������ļ��ֿ������ж��ı��ļ���
			if(feof(p_file))
				break;
			_infos[(unsigned char)ch]._count++;
			++totalSize;
		}
		//2.�����ַ����ֵĴ�������Huffman��
		HuffmanTree<CharInfo> tree;
		tree.CreateHuffmanTree(_infos,256);
		//3.����ÿ���ַ���Ӧ��Huffman����
		_GenerateHuffmanCode(tree.GetRootNode());

		//4.��ѹ������д��ѹ���ļ���
		string compressedFile = filename;
		compressedFile += ".compression";
		FILE* p_comFile = fopen(compressedFile.c_str(),"wb"); //�½�һ���������ļ�ֻ����д
		assert(p_comFile);

		  ////ÿ�ζԱ����һλ������8λ��Ϊ1���ֵ�ʱ�򣬶��ֽ��ж�ȡ��д�뵽ѹ���ļ��С�
		fseek(p_file,0,SEEK_SET);    //�����ļ�ָ��p_file��λ�ã�ָ���ļ���ͷ��λ��
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
					fputc(value,p_comFile);//  ���ַ�inChд���ļ�ָ��p_comFile��ָ����ļ��ĵ�ǰдָ���λ�á�
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

		//5.��Huffman������Ϣд�����ļ��������ѹ��ʱ�ؽ�Huffman��
		char str[20];
		string configFile = filename;
		configFile += ".config";
		FILE* p_conFile = fopen(configFile.c_str(),"wb");   //�½�һ���������ļ���ֻ����д
		assert(p_conFile);

		//totalSize��long long�ͣ����Ǵ��ļ����ļ����ַ����ܸ����������͵ı�ʾ��Χʱ����Ҫ��������д����д��λ���ĸ��ֽڣ���д��λ���ĸ��ֽڡ�
		//_itoa(totalSize >> 32,str,10);          //ȡ����λ
		//fputs(str,p_conFile);                   //��str��ָ����ַ���д���ļ�ָ�����fp��ָ����ļ���
		//fputc('\n',p_conFile);                  //���ַ�chд���ļ�ָ�����fp��ָ����ļ���
		//_itoa(totalSize & 0xffffffff,str,10);   //ȡ����λ
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
		//1.��ȡ�����ļ���Huffman������Ϣ
		string configFile = filename;
		configFile += ".config";
		FILE* f_conFile = fopen(configFile.c_str(),"rb");//��һ���������ļ�
		string line;
		char ch = 0; 

		//��ȡ�ļ����ַ�����
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
			//������һ�����У���ͳ�Ƶ��ַ�Ϊ���У����Ӧ�ַ�Ϊ���з���
			if(!line.empty())
			{
				ch = line[0];
				_infos[(unsigned char)ch]._count = atoi(line.substr(2).c_str());
				line.clear();
			}
			else
				line = '\n';
		}

		//2.���ݶ�����������Ϣ���ؽ�Huffman��
		HuffmanTree<CharInfo> tree;
		tree.CreateHuffmanTree(_infos,256);
		 //Huffman���ĸ�����Ȩֵ�����ļ��������ַ����ִ������ܺ�
		HuffmanNode<CharInfo>* root = tree.GetRootNode();
		LongType totalCount = 0;
		totalCount = root->_weight._count;
		cout<<"��ѹ��"<<totalCount<<"���ַ�"<<endl;

		//3.��ȡѹ����Ϣ�������ؽ���Huffman����ѹ��
		string uncompressedFile = filename;
		uncompressedFile += ".Uncompression";
		FILE* p_uncomFile = fopen(uncompressedFile.c_str(),"wb");

		string compressedFile = filename;
		compressedFile += ".compression";
		FILE* p_comFile = fopen(compressedFile.c_str(),"rb");

		//4.����ѹ���ļ��ַ�������Huffman����Ѱ�Ҷ�Ӧ���ַ�
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
	//����Huffman����
	void _GenerateHuffmanCode(HuffmanNode<CharInfo>* root)
	{
		if(root == NULL)
			return;
		_GenerateHuffmanCode(root->_left);
		_GenerateHuffmanCode(root->_right);

		//Ҷ�ӽ��Ϊ�����㣬˳��Ҷ�ӽ������Ѱ��Huffman����
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
			 //code�Ǵ�Ҷ�ӽ�㵽����㣬����Ҫ���ñ��롣string��reverse()�������ߵ��ַ������ַ��Ĵ���
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
	CharInfo _infos[256];    //�����256���ַ�
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