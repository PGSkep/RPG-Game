#ifndef	LIST_H
#define LIST_H

template <class T>
class ListS
{
	struct Node
	{
		T data;
		Node* next;

		Node()
		{
			next = nullptr;
		}
		Node(const Node& _node)
		{
			data = _node.data;
			next = _node.next;
		}
	};

	Node* head;
	Node* tail;
	size_t count;

public:
	ListS()
	{
		head = nullptr;
		tail = nullptr;
		count = 0;
	}
	struct Iterator
	{
		Node* prevNode;
		Node* node;

		Iterator()
		{
			prevNode = nullptr;
			node = nullptr;
		}
		void Next()
		{
			if (node != nullptr)
			{
				prevNode = node;
				node = node->next;
			}
		}
	};

	Iterator GetHead();
	Iterator GetTail();

	void PushFront(T const& _data);
	void PushBack(T const& _data);
	void PushBefore(Iterator& _iterator, T const& _data);
	void PushAfter(Iterator& _iterator, T const& _data);

	//void PopFront();
	//void PopBack();
	void Pop(Iterator& _iterator);

	void Clear();
};

template <class T>
typename ListS<T>::Iterator ListS<T>::GetHead()
{
	Iterator iter;
	iter.node = head;

	return iter;
}
template <class T>
typename ListS<T>::Iterator ListS<T>::GetTail()
{
	Iterator iter;
	iter.node = tail;

	return iter;
}

template <class T>
void ListS<T>::PushFront(T const& _data)
{
	Node* newNode = new Node;
	newNode->data = _data;
	newNode->next = head;

	if (tail == nullptr)
		tail = newNode;

	head = newNode;
}
template <class T>
void ListS<T>::PushBack(T const& _data)
{
	Node* newNode = new Node;
	newNode->data = _data;
	newNode->next = nullptr;

	if (tail != nullptr)
	{
		tail->next = newNode;
		tail = newNode;
	}
	else
	{
		head = newNode;
		tail = newNode;
	}
}

template <class T>
void ListS<T>::Pop(Iterator& _iterator)
{
	if (_iterator.node == nullptr)
		return;

	if (_iterator.prevNode != nullptr)
		_iterator.prevNode->next = _iterator.node->next;
	else
		head = _iterator.node->next;

	Node* tempNode = _iterator.node->next;
	delete _iterator.node;
	_iterator.node = tempNode;
}

template <class T>
void ListS<T>::Clear()
{
	Node* tempNode;
	while (head != nullptr)
	{
		tempNode = head->next;
		delete head;
		head = tempNode;
	}

	tail = nullptr;
}

#endif