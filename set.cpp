#include <vector>
#include <iostream>
#include <cassert>
#include <set>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::set;

template <class ValueType>
struct Node {
	ValueType key;
	Node *left;
	Node *right;
	Node *parent;
	int depth;
	Node() : key(0), left(nullptr), right(nullptr), parent(nullptr), 
	         depth(0) {};
};

template <class ValueType>
class Set {
	size_t size_;
	Node<ValueType> *root;
	Node<ValueType> *begin(Node<ValueType> *ptr) const;
	Node<ValueType> *rbegin(Node<ValueType> *ptr) const;
	Node<ValueType> *next(Node<ValueType> *ptr) const;
	Node<ValueType> *prev(Node<ValueType> *ptr) const;
	void leftBig(Node<ValueType> **vertex);
	void rightBig(Node<ValueType> **vertex);
	void leftSmall(Node<ValueType> **vertex);
	void rightSmall(Node<ValueType> **vertex);
	
	int getDepth(Node<ValueType> *ptr);
	void updateDepth(Node<ValueType> * ptr);
	
	int getBalance(Node<ValueType> *ptr);
	void makeBalancePlus(Node<ValueType> **ptr);
	void makeBalanceMinus(Node<ValueType> **ptr);
	Node<ValueType> *recursiveInsert(const ValueType & key, 
	                                Node<ValueType> *cur_root, 
	                                Node<ValueType> *parent = NULL);
	void balance(Node<ValueType> **ptr);
	bool erase(const ValueType & key, Node<ValueType> **ptr);
	void clear(Node<ValueType> *ptr);
	int getLeftDistance(Node<ValueType> *ptr) const;
	int getRightDistance(Node<ValueType> *ptr) const;
	void swap(Node<ValueType> *ptr_a, Node<ValueType> *ptr_b);
	Node<ValueType> *copyNode(Node<ValueType> *parent, Node<ValueType> *ptr);
	Node<ValueType> * find(ValueType & key, Node<ValueType> *ptr) const;
public:
	Set();
	Set(const Set<ValueType> & source);
	size_t size() const { return size_; };
	bool empty() const { return root == nullptr; };
	void insert(const ValueType & key);
	void erase(const ValueType & key);
	Node<ValueType> *begin() const;
	Node<ValueType> *rbegin() const;
	Node<ValueType> *find() const;
	void clear();
	void print() const;
	void print_line() const;
	Node<ValueType> * find(ValueType & key) const;
	~Set();
};

template <class ValueType>
Set<ValueType>::Set() {
	size_ = 0;
	root = nullptr;
}

template <class ValueType>
Set<ValueType>::Set(const Set<ValueType> & source) {
	size_ = source.size();
	root = copyNode(nullptr, source.root);
}

template <class ValueType>
Set<ValueType>::~Set() {
	clear(root);
}

template <class ValueType>
Node<ValueType> * Set<ValueType>::copyNode(Node<ValueType> *parent, Node<ValueType> *ptr) {
	if (ptr == nullptr)
		return nullptr;
	Node<ValueType> *node = new Node<ValueType>;
	node->key = ptr->key;
	node->left = copyNode(node, ptr->left);
	node->right = copyNode(node, ptr->right);
	node->parent = parent;
	return node;
}

template <class ValueType>
void Set<ValueType>::clear(Node<ValueType> *ptr) {
	if (ptr == nullptr)
		return;
	if (ptr->left == nullptr && ptr->right == nullptr) {
		free(ptr);
		size_--;
		return;
	}
	clear(ptr->left);
	clear(ptr->right);
	free(ptr);
	size_--;
}


template <class ValueType>
void Set<ValueType>::clear() {
	clear(root);
	root = nullptr;
}

template <class ValueType>
int Set<ValueType>::getDepth(Node<ValueType> *ptr) {
	if (ptr == nullptr)
		return 0;
	return ptr->depth;
}

template <class ValueType>
void Set<ValueType>::updateDepth(Node<ValueType> *ptr) {
	if (ptr == nullptr)
		return;
	updateDepth(ptr->left);
	updateDepth(ptr->right);
	int depth_left = getDepth(ptr->left);
	int depth_right = getDepth(ptr->right);
	int max_depth = depth_left > depth_right ? depth_left : depth_right;
	ptr->depth = 1 + max_depth;
}

template <class ValueType>
int Set<ValueType>::getBalance(Node<ValueType> *ptr) {
	int depth_left = getDepth(ptr->left);
	int depth_right = getDepth(ptr->right);
	return depth_left - depth_right;
}

template <class ValueType>
void Set<ValueType>::leftSmall(Node<ValueType> **vertex) {
	//cout << "Left small: " << (*vertex)->key << " " << (*vertex)->right->key << endl;
	Node<ValueType> *node_a = *vertex;
	Node<ValueType> *node_b = node_a->right;
	
	if (node_b->left)
		node_b->left->parent = node_a;
	node_b->parent = node_a->parent;
	node_a->parent = node_b;
	
	node_a->right = node_b->left;
	node_b->left = node_a;
	
	*vertex = node_b;
}

template <class ValueType>
void Set<ValueType>::rightSmall(Node<ValueType> **vertex) {
	//cout << "Right small: " << (*vertex)->key << endl;
	Node<ValueType> *node_a = *vertex;
	Node<ValueType> *node_b = node_a->left;
	
	if (node_b->right)
		node_b->right->parent = node_a;
	node_b->parent = node_a->parent;
	node_a->parent = node_b;
	
	node_a->left = node_b->right;
	node_b->right = node_a;

	*vertex = node_b;
}

template <class ValueType>
void Set<ValueType>::leftBig(Node<ValueType> **vertex) {
	//cout << "Left big: " << (*vertex)->key << endl;
	rightSmall(&((*vertex)->right));
	//cout << "Root value: " << (*vertex)->right->key << endl; 
	leftSmall(vertex);
}

template <class ValueType>
void Set<ValueType>::rightBig(Node<ValueType> **vertex) {	
	//cout << "Right big: " << (*vertex)->key << endl;
	leftSmall(&((*vertex)->left));
	rightSmall(vertex);
}

template <class ValueType>
void Set<ValueType>::makeBalancePlus(Node<ValueType> **ptr) {
	int right_balance = getBalance((*ptr)->right);
	if (right_balance == 0 || right_balance == -1)
		leftSmall(ptr);
	if (right_balance == 1) {
		int right_left_balance = getBalance((*ptr)->right->left);
		if (abs(right_left_balance) <= 1)
			leftBig(ptr);
	}
}

template <class ValueType>
void Set<ValueType>::makeBalanceMinus(Node<ValueType> **ptr) {
	int left_balance = getBalance((*ptr)->left);
	if (left_balance == 0 || left_balance == 1)
		rightSmall(ptr);
	if (left_balance == -1) {
		int left_right_balance = getBalance((*ptr)->left->right);
		if (abs(left_right_balance) <= 1)
			rightBig(ptr);
	}
}

template <class ValueType>
Node<ValueType> * Set<ValueType>::recursiveInsert(const ValueType & key, 
	                            Node<ValueType> *cur_root, 
	                            Node<ValueType> *parent) {
	if (cur_root == nullptr) {
		size_++;
		Node<ValueType> *node = new Node<ValueType>;
		node->key = key;
		node->left = node->right = nullptr;
		node->parent = parent;
		//updateDepth(root);
		return node;
	}
	if (key > cur_root->key)
		cur_root->right = recursiveInsert(key, cur_root->right, cur_root);
	else if (key < cur_root->key)
		cur_root->left = recursiveInsert(key, cur_root->left, cur_root);
	else
		return cur_root;
	updateDepth(cur_root);
	int balance = getBalance(cur_root);
	if (abs(balance) > 1) {
		if (balance == -2)
			makeBalancePlus(&cur_root);
		else if (balance == 2)
			makeBalanceMinus(&cur_root);
		//updateDepth(cur_root);
	}
	return cur_root;
}

template <class ValueType>
void Set<ValueType>::insert(const ValueType & key) {
	root = recursiveInsert(key, root);
}

template <class ValueType>
void Set<ValueType>::balance(Node<ValueType> **ptr) {
	updateDepth(root);
	int balance = getBalance(*ptr);
	if (abs(balance) > 1) {
		if (balance == -2)
			makeBalancePlus(ptr);
		else if (balance == 2)
			makeBalanceMinus(ptr);
		updateDepth(root);
	}
}


template <class ValueType>
int Set<ValueType>::getLeftDistance(Node<ValueType> *ptr) const {
	if (ptr->left == nullptr)
		return 0;
	int dist = 1;
	while (ptr->right) {
		ptr = ptr->right;
		dist++;
	}
	return dist;
}

template <class ValueType>
int Set<ValueType>::getRightDistance(Node<ValueType> *ptr) const {
	if (ptr->right == nullptr)
		return 0;
	int dist = 1;
	while (ptr->left) {
		ptr = ptr->left;
		dist++;
	}
	return dist;
}

template <class ValueType>
void Set<ValueType>::swap(Node<ValueType> *ptr_a, Node<ValueType> *ptr_b) {
	ValueType key = ptr_a->key;
	ptr_a->key = ptr_b->key;
	ptr_b->key = key;
}

template <class ValueType>
bool Set<ValueType>::erase(const ValueType & key, Node<ValueType> **ptr) {
	if (ptr == nullptr || *ptr == nullptr)
		return false;

	if (key == (*ptr)->key) {
		if ((*ptr)->left == nullptr && (*ptr)->right == nullptr) {
			free(*ptr);
			*ptr = nullptr;
		} else {
			Node<ValueType> *nearest_ptr;
			int left_dist = getLeftDistance(*ptr);
			int right_dist = getRightDistance(*ptr);
			if ((left_dist <= right_dist && left_dist != 0) || right_dist == 0) {
				nearest_ptr = prev(*ptr);
				swap(*ptr, nearest_ptr);
				erase(key, &((*ptr)->left));
			} else {
				nearest_ptr = next(*ptr);
				swap(*ptr, nearest_ptr);
				erase(key, &((*ptr)->right));
			}
		}
		return true;
	}
	int has_erased = false;
	if (key < (*ptr)->key)
		has_erased = erase(key, &((*ptr)->left));
	else if (key > (*ptr)->key)
		has_erased = erase(key, &((*ptr)->right));
	balance(ptr);
	return has_erased;
}

template <class ValueType>
void Set<ValueType>::erase(const ValueType & key) {
	erase(key, &root);
}

template <class ValueType>
Node<ValueType> * Set<ValueType>::begin(Node<ValueType> *ptr) const {
	if (ptr == nullptr)
		return nullptr;
	while (ptr->left != nullptr)
		ptr = ptr->left;
	return ptr;
}

template <class ValueType>
Node<ValueType> * Set<ValueType>::rbegin(Node<ValueType> *ptr) const {
	if (ptr == nullptr)
		return nullptr;
	while (ptr->right != nullptr)
		ptr = ptr->right;
	return ptr;
}

template <class ValueType>
Node<ValueType> * Set<ValueType>::begin() const {
	return begin(root);
}

template <class ValueType>
Node<ValueType> * Set<ValueType>::rbegin() const {
	return rbegin(root);
}

template <class ValueType>
Node<ValueType> * Set<ValueType>::next(Node<ValueType> *ptr) const {
	/*cout << "N[" << ptr->key;
	if (ptr->parent)
		cout << " " << ptr->parent->key;
	cout << "]" << endl;*/
	if (ptr == nullptr)
		return nullptr;
	Node<ValueType> *cur_ptr = nullptr;
	if (ptr->right != nullptr) {
		cur_ptr = ptr->right;
		while (cur_ptr->left)
			cur_ptr = cur_ptr->left;
		return cur_ptr;
	}
	if (ptr->parent && ptr->parent->right == ptr) {
		while (ptr->parent && ptr->parent->right == ptr)
			ptr = ptr->parent;
		cur_ptr = ptr;
		if (cur_ptr->parent == nullptr || cur_ptr->parent->left != cur_ptr)
			return nullptr;
		if (cur_ptr->parent && cur_ptr->parent->left == cur_ptr)
			cur_ptr = cur_ptr->parent;
		return cur_ptr;
	}
	return ptr->parent;
}

template <class ValueType>
Node<ValueType> * Set<ValueType>::prev(Node<ValueType> *ptr) const {
	if (ptr == nullptr)
		return nullptr;
	//cout << "{" << ptr->key << "}" << endl;
	if (ptr->parent == nullptr) {
		ptr = ptr->left;
		while (ptr->right)
			ptr = ptr->right;
		return ptr;
	}
	// if right child
	if (ptr == ptr->parent->right) {
		if (ptr->left == nullptr)
			return ptr->parent;
		ptr = ptr->left;
		while (ptr->right)
			ptr = ptr->right;
		return ptr;
	}
	// left subtree
	if (ptr->left == nullptr) {
		Node<ValueType> *cur_ptr = ptr->parent;
		while (cur_ptr->parent && cur_ptr != cur_ptr->parent->right)
			cur_ptr = cur_ptr->parent;
		return cur_ptr->parent;
	}
	ptr = ptr->left;
	while (ptr->right)
		ptr = ptr->right;
	return ptr;
}

template <class ValueType>
void Set<ValueType>::print() const {
	if (root == nullptr) {
		cout << "Tree is empty." << endl;
		return;
	}
	std::vector<Node<ValueType>*> one, two;
	int hmax = 6, width = 1  << hmax;
	one.push_back(root);
	for (int level = 0; level < hmax; level++) {
		string filler(width - 1, ' ');
		for (auto elem : one) {
			if (elem) {
				//cout << filler << elem->key"[" << elem->depth << "]" << filler;
				cout << filler << elem->key << "[";
				if (elem->parent)
					cout << elem->parent->key;
				cout << "]" << filler;
				//cout << filler << elem->key << filler;
				two.push_back(elem->left);
				two.push_back(elem->right);
			} else {
				cout << filler << "--" << filler;
				two.push_back(nullptr);
				two.push_back(nullptr);
			}
		}
		cout << endl;
		one = two;
		two.clear();
		width /= 2;
	}
}

template <class ValueType>
void Set<ValueType>::print_line() const {
	Node<ValueType> *beg = begin(root);
	Node<ValueType> *end = rbegin(root);
	cout << beg->key << " " << end->key << " " << endl;
	while (beg != nullptr) {
		cout << beg->key << "(" << beg->depth << ")" << " ";
		beg = next(beg);
	}
	cout << endl;
	while (end != nullptr) {
		cout << end->key << "(" << end->depth << ")" << " ";
		end = prev(end);
	}
	cout << endl << "Size: " << size_ << endl;
}


template <class ValueType>
Node<ValueType> * Set<ValueType>::find(ValueType & key, Node<ValueType> *ptr) const {
	if (ptr == nullptr)
		return nullptr;
	if (ptr->key == key)
		return ptr;
	if (ptr->key > key)
		return find(key, ptr->left);
	return find(key, ptr->right);
}

template <class ValueType>
Node<ValueType> * Set<ValueType>::find(ValueType & key) const {
	return find(key, root);
}

int main() {
	Set<int> tree;
	set<int> stlset;
	int n, m;
	cin >> n;
	for (int i = 0; i < n; i++) {
		int value;
		cin >> value;
		tree.insert(value);
		stlset.insert(value);
	}
	cout << "Tree created." << endl;
	//tree.print();
	cin >> m;
	for (int i = 0; i < m; i++) {
		int x;
		cin >> x;
		bool a = tree.find(x) != nullptr;
		bool b = stlset.find(x) != stlset.end();
		assert(a == b);
	}
	cout << "OK" << endl;
	return 0;
}
