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
		for(int i=_v.size()/2 -1;i>=0;--i)     //根据_v.size()计算父亲结点的下标，_v.size()/2 -1.根据下标即_v.size()-1计算父亲结点的下标，(_v.size()-1)/2.
		{
			_AdjustDown(_v,i,_v.size());
		}
	}
	//插入一个数据x到最小堆中
	void Push(const T& x)
	{
		_v.push_back(x);
		if(_v.size() > 1)
		{
			_AdjustUp(_v,_v.size()-1);
		}
	}
	 //删除堆顶的数据--堆顶与最后一个数据进行交换
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

	//将根结点向下调整，以小堆为例
	void _AdjustDown(vector<T>& vec,int root,size_t size)  
	{
		int child = root*2+1;
		while(child < size)
		{
			//child为左右孩子中小的结点的下标
			if(child+1 < size && Compare()(vec[child+1],vec[child]))
				++child;
			//若孩子节点小于根结点，则swap
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
	//将一个结点向上调整，以小堆为例
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
		//1.将数组中的所有值构建为结点放入最小堆中
		Heap<Node*,Compare> smallHeap;
		for(int i = 0;i < size; ++i)
		{
			Node* node = new Node(array[i]);
			smallHeap.Push(node);
		}
		if(smallHeap.Empty())
			return;
		//2.获取出最小和次小结点做孩子结点，并构建这两个孩子的父结点进行链接。
		Node* parent = NULL;    
		Node* first = NULL;
		Node* second = NULL;
		while(smallHeap.Size() > 1)   //当只有一个结点的时候，该结点即为它的最终父结点
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
	void LevelOrder()   //层次遍历打印二叉树
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