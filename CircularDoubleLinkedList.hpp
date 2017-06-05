#pragma once
#ifndef __CIRCULAR_DOUBLE_LINKED_LIST_H__
#define __CIRCULAR_DOUBLE_LINKED_LIST_H__

#include <iostream>

///FUNCTION_INDEX////////////////////////////////////////////////////////
/*
1. void Init_D_List();
2. void PrintAllNode();
3. T InsertHead(T data);
4. T InsertTail(T data);
5. T popHead(void);
6. T peek(int target);
7. int getSize(void);
8. bool ClearAllNode(void);
*/
/////////////////////////////////////////////////////////////////////////


template <typename T>
class CircularDoubleLinkedList
{
private:
	typedef struct node_
	{
		T _data;
		struct node_ *next;
		struct node_ *prev;
	}node;
	node *head, *tail;
	int _nodeCnt;

public:

	CircularDoubleLinkedList(void)
	{
		Init_D_List();
	}
	~CircularDoubleLinkedList(void)
	{
		ClearAllNode();
	}

	void Init_D_List(void)
	{
		head = (node*)malloc(sizeof(node));
		tail = (node*)malloc(sizeof(node));
		head->next = tail;
		head->prev = head;
		tail->next = head->next; //����ť�� �̷���.
		tail->prev = head;
		_nodeCnt = 0; //������ Head, Tail�� ������ ���� ����.
	}

	void PrintAllNode(void)
	{
		node* t;
		T putData;

		if (head->next == tail) //isEmpty.
		{
			std::cout << "List is empty\n" << std::endl;
			return;
		}

		t = head->next;
		while (t != tail)
		{
			putData = t->_data;
			t = t->next;
			printf("[%d] ", putData);
		}
		std::cout << std::endl;
	}


	T InsertHead(T data)
	{
		//Circular Queueó�� ������ �� �Լ��� ������ �� ��������. �ٵ� �ϴ� �����.
		node* t;
		t = (node*)malloc(sizeof(node));
		if (!t)
		{
			fprintf(stderr, "Err(InsertHead) : allocation err.\n");
			return -1;
		}

		t->_data = data;
		t->next = head->next;
		t->prev = head;
		t->next->prev = t;
		head->next = t;

		_nodeCnt++;

		return data;
	}

	T InsertTail(T data)
	{
		//Circular Queue�� push������ �ϰ� �� ����.
		node* t;
		t = (node*)malloc(sizeof(node));
		if (!t)
		{
			fprintf(stderr, "Err(InsertTail) : allocation err.\n");
			return -1;
		}

		t->_data = data;
		t->next = tail;
		t->prev = t->next->prev;
		t->prev->next = t;
		tail->prev = t;

		_nodeCnt++;

		return data;
	}

	T popHead(void)
	{
		node* t;
		T k;

		t = head->next;
		k = t->_data;
		t->next->prev = head;
		head->next = t->next;
		t->next = nullptr;
		t->prev = nullptr;

		free(t);

		_nodeCnt--;

		return k;
	}

	T peek(int target)
	{
		node* t;
		T k;
		int iCnt = 1;

		if (target > _nodeCnt)
		{
			fprintf(stderr, "Err(peek) : target is over then node count.\n");
			return -1;
		}

		t = head->next;
		while (iCnt < target) //Ÿ�ټ��� n�� �� n-1 ��ŭ ���ϴ�.
		{
			t = t->next;
			iCnt++;
		}

		k = t->_data; //�׸��� �ش簪��.
		return k;	//��������.
	}

	int getSize(void)
	{
		return _nodeCnt;
	}

	bool ClearAllNode(void)
	{
		node* t;
		node* k;
		t = (node*)malloc(sizeof(node));
		if (!t)
		{
			fprintf(stderr, "Err(peek) : allocation err.\n");
			return false;
		}

		t = head->next; //set ��ŸƮ.
		while (t != tail)
		{
			t->prev->next = nullptr; //�� ��忡�� t�� ���� ��� ����.
			t->prev = nullptr; //t�� prev ����.
			k = t;		//���̳�尡 t�ڸ��� ����.
			t = t->next; //t�� ���� ���� ����.

			free(k);	//���̳�� free();
			_nodeCnt--;	//�ϳ� �پ����� _nodeCnt--;

			if (t == tail)
			{
				t->prev->next = nullptr; //�� ��忡�� t�� ���� ��� ����.
				t->prev = nullptr;
				t->next = nullptr;
				head->prev = nullptr;
				head->next = nullptr;

				free(tail); //���, ���ϱ��� �� ������
				free(head);
				return true; //�ٳ����� ��.
			}
		}
		return false;
	}
};


#endif
