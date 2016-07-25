#pragma once

#include<iostream>
#include<queue>
#include<vector>
using namespace std;

template<class T,class Compare = less<T>>
class Heap
{
public:
	Heap()
	{}
	Heap(const T* array,size_t size)
	{
		for(int i=0;i<size;++i)
		{
			_v.push_back(array[i]);
		}
		for(int i=_v.size()/2 -1;i>=0;--i)     //����_v.size()���㸸�׽����±꣬_v.size()/2 -1.�����±꼴_v.size()-1���㸸�׽����±꣬(_v.size()-1)/2.
		{
			_AdjustDown(_v,i,_v.size());
		}
	}
	//����һ������x����С����
	void Push(const T& x)
	{
		_v.push_back(x);
		if(_v.size() > 1)
		{
			_AdjustUp(_v,_v.size()-1);
		}
	}
	 //ɾ���Ѷ�������--�Ѷ������һ�����ݽ��н���
	void Pop()   
	{
		size_t size = _v.size();
		swap(_v[0],_v[size-1]);
		_v.pop_back();
		if(_v.size() > 1)
			_AdjustDown(_v,0,_v.size());
	}
	const T& Top()
	{
		return _v[0];
	}

	//����������µ�������С��Ϊ��
	void _AdjustDown(vector<T>& vec,int root,size_t size)  
	{
		int child = root*2+1;
		while(child < size)
		{
			//childΪ���Һ�����С�Ľ����±�
			if(child+1 < size && Compare()(vec[child+1],vec[child]))
				++child;
			//�����ӽڵ�С�ڸ���㣬��swap
			if(Compare()(vec[child],vec[root]))
			{
				swap(vec[child],vec[root]);
				root = child;
				child = root*2+1;
			}
			else
				break;
		}
	}
	//��һ��������ϵ�������С��Ϊ��
	void _AdjustUp(vector<T>& vec,int pos)
	{
		int child = pos;
		int parent = (pos-1)/2;
		while(child > 0)
		{
			if(Compare()(vec[child],vec[parent]))
			{
				swap(vec[child],vec[parent]);
				child = parent;
				parent = (child-1)/2;
			}
			else
				break;
		}	
	}
	bool Empty()
	{
		return _v.empty();
	}
	size_t Size()
	{
		return _v.size();
	}

private:
	vector<T> _v;
};


template<class T>
struct HuffmanNode
{
	T _weight;
	HuffmanNode<T>* _parent;
	HuffmanNode<T>* _left;
	HuffmanNode<T>* _right;

	HuffmanNode(const T& weight)
		:_weight(weight)
		,_parent(NULL)
		,_left(NULL)
		,_right(NULL)
	{}
};

template<class T>
class HuffmanTree
{
	typedef HuffmanNode<T> Node;
public:
	HuffmanTree()
		:_root(NULL)
	{}
	~HuffmanTree()
	{
		Destroy(_root);
	}
	Node* GetRootNode()
	{
		return _root;
	}
	void Destroy(Node*& root)
	{
		if(root)
		{
			Destroy(root->_left);
			Destroy(root->_right);

			delete root;
			root = NULL;
		}

	}
	void CreateHuffmanTree(const T* array,size_t size)
	{
		struct Compare
		{
			bool operator()(Node* node1,Node* node2)
			{
				return node1->_weight < node2->_weight;
			}
		};
		//1.�������е�����ֵ����Ϊ��������С����
		Heap<Node*,Compare> smallHeap;
		for(int i = 0;i < size; ++i)
		{
			Node* node = new Node(array[i]);
			smallHeap.Push(node);
		}
		if(smallHeap.Empty())
			return;
		//2.��ȡ����С�ʹ�С��������ӽ�㣬���������������ӵĸ����������ӡ�
		Node* parent = NULL;    
		Node* first = NULL;
		Node* second = NULL;
		while(smallHeap.Size() > 1)   //��ֻ��һ������ʱ�򣬸ý�㼴Ϊ�������ո����
		{
			first = smallHeap.Top();
			smallHeap.Pop();
			second = smallHeap.Top();
			smallHeap.Pop();
			parent = new Node(first->_weight + second->_weight);

			parent->_left = first;
			parent->_right = second;
			first->_parent = parent;
			second->_parent = parent;

			smallHeap.Push(parent);
		}
		_root = parent;
	}
	void LevelOrder()   //��α�����ӡ������
	{
		queue<Node*> q;
		if(_root)
			q.push(_root);
		while(!q.empty())
		{
			Node* node = q.front();
			cout<<node->_weight<<" ";
			if(node->_left)
				q.push(node->_left);
			if(node->_right)
				q.push(node->_right);
			q.pop();
		}
		cout<<endl;
	}

private:
	Node* _root;
};