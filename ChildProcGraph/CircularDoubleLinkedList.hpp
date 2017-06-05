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
		tail->next = head->next; //원형큐라서 이렇다.
		tail->prev = head;
		_nodeCnt = 0; //언제나 Head, Tail을 제외한 수가 나옴.
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
		//Circular Queue처럼 쓰려면 이 함수는 쓸일이 잘 없을꺼임. 근데 일단 만든다.
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
		//Circular Queue의 push역할을 하게 될 꺼임.
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
		while (iCnt < target) //타겟수가 n일 때 n-1 만큼 돕니다.
		{
			t = t->next;
			iCnt++;
		}

		k = t->_data; //그리고 해당값을.
		return k;	//리턴하죠.
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

		t = head->next; //set 스타트.
		while (t != tail)
		{
			t->prev->next = nullptr; //전 노드에서 t로 오는 노드 끊고.
			t->prev = nullptr; //t의 prev 끊고.
			k = t;		//더미노드가 t자리로 가고.
			t = t->next; //t가 다음 노드로 가고.

			free(k);	//더미노드 free();
			_nodeCnt--;	//하나 줄었으니 _nodeCnt--;

			if (t == tail)
			{
				t->prev->next = nullptr; //전 노드에서 t로 오는 노드 끊고.
				t->prev = nullptr;
				t->next = nullptr;
				head->prev = nullptr;
				head->next = nullptr;

				free(tail); //헤드, 테일까지 다 날리고
				free(head);
				return true; //다날리기 끝.
			}
		}
		return false;
	}
};


#endif
