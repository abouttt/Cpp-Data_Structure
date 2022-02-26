#include "RedBlackTree.h"
#include <iostream>
#include <windows.h>
using namespace std;

enum class ConsoleColor
{
	BLACK = 0,
	RED = FOREGROUND_RED,
	GREEN = FOREGROUND_GREEN,
	BLUE = FOREGROUND_BLUE,
	YELLOW = RED | GREEN,
	WHITE = RED | GREEN | BLUE,
};

void ShowConsoleCursor(bool flag)
{
	HANDLE output = ::GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	::GetConsoleCursorInfo(output, &cursorInfo);
	cursorInfo.bVisible = flag;
	::SetConsoleCursorInfo(output, &cursorInfo);
}

void SetCursorColor(ConsoleColor color)
{
	HANDLE output = ::GetStdHandle(STD_OUTPUT_HANDLE);
	::SetConsoleTextAttribute(output, static_cast<SHORT>(color));
}

void SetCursorPosition(int x, int y)
{
	HANDLE output = ::GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
	::SetConsoleCursorPosition(output, pos);
}

RedBlackTree::RedBlackTree()
{
	// NIL ���� ���ٴ� ���·� ����.
	// �������� ����� �ͺ��ٴ� �ϳ��� ���� �������� ����Ű�� �Ѵ�.
	mNil = new Node(); // Black
	mRoot = mNil;
}

RedBlackTree::~RedBlackTree()
{
	delete mNil;
}

void RedBlackTree::Print()
{
	::system("cls");
	ShowConsoleCursor(false);
	Print(mRoot, 10, 0);
}

void RedBlackTree::Print(Node* node, int x, int y)
{
	if (node == mNil)
	{
		return;
	}

	SetCursorPosition(x, y);

	if (node->color == eColor::Black)
	{
		SetCursorColor(ConsoleColor::BLUE);
	}
	else
	{
		SetCursorColor(ConsoleColor::RED);
	}

	cout << node->key;
	Print(node->left, x - (5 / (y + 1)), y + 1);
	Print(node->right, x + (5 / (y + 1)), y + 1);

	SetCursorColor(ConsoleColor::WHITE);
}

Node* RedBlackTree::Search(Node* node, int key)
{
	if (node == mNil || key == node->key)
	{
		return node;
	}

	if (key < node->key)
	{
		return Search(node->left, key);
	}
	else
	{
		return Search(node->right, key);
	}
}

Node* RedBlackTree::Min(Node* node)
{
	while (node->left != mNil)
	{
		node = node->left;
	}

	return node;
}

Node* RedBlackTree::Max(Node* node)
{
	while (node->right != mNil)
	{
		node = node->right;
	}

	return node;
}

Node* RedBlackTree::Next(Node* node)
{
	if (node->right != mNil)
	{
		return Min(node->right);
	}

	Node* parent = node->parent;

	while (parent != mNil && node == parent->right)
	{
		node = parent;
		parent = parent->parent;
	}

	return parent;
}

void RedBlackTree::Insert(int key)
{
	Node* newNode = new Node();
	newNode->key = key;

	Node* node = mRoot;
	Node* parent = mNil;

	while (node != mNil)
	{
		parent = node;
		if (key < node->key)
		{
			node = node->left;
		}
		else
		{
			node = node->right;
		}
	}

	newNode->parent = parent;

	if (parent == mNil)
	{
		mRoot = newNode;
	}
	else if (key < parent->key)
	{
		parent->left = newNode;
	}
	else
	{
		parent->right = newNode;
	}

	// �˻�
	newNode->left = mNil;
	newNode->right = mNil;
	newNode->color = eColor::Red;

	InsertFixup(newNode);
}

void RedBlackTree::InsertFixup(Node* node)
{
	// 1) p = red, uncle = red
	// -> p = black, uncle = black, pp = red �� �ٲ�
	// 2) p = red, uncle = black (triangle)
	// -> ȸ���� ���� case 3���� �ٲ�
	// 3) p = red, uncle = black (list)
	// -> ���� ���� + ȸ��

	while (node->parent->color == eColor::Red)
	{
		if (node->parent == node->parent->parent->left)
		{
			Node* uncle = node->parent->parent->right;

			if (uncle->color == eColor::Red)
			{
				node->parent->color = eColor::Black;
				uncle->color = eColor::Black;
				node->parent->parent->color = eColor::Red;
				// ��� ���� �ٲپ����� node->parent->parent->parent��
				// ���� Red�� ��� �� �ٲ�� �ϱ� ������ node�� node->parent->parent
				// �� �ٲپ� �ݺ����� ����ǰԲ� �Ѵ�.
				node = node->parent->parent;
			}
			else
			{
				// Triangle Ÿ��
				//          [pp(B)]
				//     [p(R)]     [u(B)]
				//         [n(R)]
				// 
				//  LeftRotate ->
				// 
				//          [pp(B)]
				//     [p(R)]     [u(B)]
				//  [n(R)]
				if (node == node->parent->right)
				{
					node = node->parent;
					LeftRotate(node);
				}

				// ListŸ��
				//          [pp(R)]
				//      [p(B)]    [u(B)]
				//  [n(R)]
				//
				//  RightRotate ->
				//          
				//           [p(B)]    
				//       [n(R)]   [pp(R)]
				//                     [u(B)]

				node->parent->color = eColor::Black;
				node->parent->parent->color = eColor::Red;
				RightRotate(node->parent->parent);
			}
		}
		else // �ݴ�
		{
			Node* uncle = node->parent->parent->left;

			if (uncle->color == eColor::Red)
			{
				node->parent->color = eColor::Black;
				uncle->color = eColor::Black;
				node->parent->parent->color = eColor::Red;
				node = node->parent->parent;
			}
			else
			{
				// Triangle Ÿ��
				if (node == node->parent->left)
				{
					node = node->parent;
					RightRotate(node);
				}

				// List Ÿ��
				node->parent->color = eColor::Black;
				node->parent->parent->color = eColor::Red;
				LeftRotate(node->parent->parent);
			}
		}
	}

	mRoot->color = eColor::Black;
}

void RedBlackTree::Delete(int key)
{
	Node* deleteNode = Search(mRoot, key);
	Delete(deleteNode);
}

// ���� BST ���� ����
void RedBlackTree::Delete(Node* node)
{
	if (node == mNil)
	{
		return;
	}

	if (node->left == mNil)
	{
		eColor color = node->color;
		Node* right = node->right;
		Replace(node, node->right);

		if (color == eColor::Black)
		{
			DeleteFixup(right);
		}
	}
	else if (node->right == mNil)
	{
		eColor color = node->color;
		Node* left = node->left;
		Replace(node, node->left);

		if (color == eColor::Black)
		{
			DeleteFixup(left);
		}
	}
	else
	{
		// ���� ������ ã��
		Node* next = Next(node);
		node->key = next->key;
		Delete(next);
	}
}

void RedBlackTree::DeleteFixup(Node* node)
{
	Node* x = node;

	while (x != mRoot && x->color == eColor::Black)
	{
		if (x == x->parent->left)
		{
			Node* s = x->parent->right;
			if (s->color == eColor::Red)
			{
				s->color = eColor::Black;
				x->parent->color = eColor::Red;
				LeftRotate(x->parent);
				s = x->parent->right;
			}

			if (s->left->color == eColor::Black && s->right->color == eColor::Black)
			{
				s->color = eColor::Red;
				x = x->parent;
			}
			else
			{
				if (s->right->color == eColor::Black)
				{
					s->left->color = eColor::Black;
					s->color = eColor::Red;
					RightRotate(s);
					s = x->parent->right;
				}

				s->color = x->parent->color;
				x->parent->color = eColor::Black;
				s->right->color = eColor::Black;
				LeftRotate(x->parent);
				x = mRoot;
			}
		}
		else
		{
			Node* s = x->parent->left;
			if (s->color == eColor::Red)
			{
				s->color = eColor::Black;
				x->parent->color = eColor::Red;
				RightRotate(x->parent);
				s = x->parent->left;
			}

			if (s->right->color == eColor::Black && s->left->color == eColor::Black)
			{
				s->color = eColor::Red;
				x = x->parent;
			}
			else
			{
				if (s->left->color == eColor::Black)
				{
					s->right->color = eColor::Black;
					s->color = eColor::Red;
					LeftRotate(s);
					s = x->parent->left;
				}

				s->color = x->parent->color;
				x->parent->color = eColor::Black;
				s->left->color = eColor::Black;
				RightRotate(x->parent);
				x = mRoot;
			}
		}
	}

	x->color = eColor::Black;
}

// u ����Ʈ���� v ����Ʈ���� ��ü
// �׸��� delete u
void RedBlackTree::Replace(Node* u, Node* v)
{
	if (u->parent == mNil)
	{
		mRoot = v;
	}
	else if (u == u->parent->left)
	{
		u->parent->left = v;
	}
	else
	{
		u->parent->right = v;
	}

	v->parent = u->parent;

	delete u;
}

//     [y]
//  [x]   [3]
// [1][2]

//     [x]
//  [1]   [y]
//       [2][3]

void RedBlackTree::LeftRotate(Node* x)
{
	Node* y = x->right;

	x->right = y->left;

	if (y->left != mNil)
	{
		y->left->parent = x;
	}

	y->parent = x->parent;

	if (x->parent == mNil)
	{
		mRoot = y;
	}
	else if (x == x->parent->left)
	{
		x->parent->left = y;
	}
	else
	{
		x->parent->right = y;
	}

	y->left = x;
	x->parent = y;
}

void RedBlackTree::RightRotate(Node* y)
{
	Node* x = y->left;

	y->left = x->right;

	if (x->right != mNil)
	{
		x->right->parent = y;
	}

	x->parent = y->parent;

	if (x->parent == mNil)
	{
		mRoot = x;
	}
	else if (y == y->parent->left)
	{
		y->parent->left = x;
	}
	else
	{
		y->parent->right = x;
	}

	x->right = y;
	y->parent = x;
}
