#include <vector>

using namespace std;

class node {
public:
	node* lc;
	node* rc;
	vector<int> index;
	int feature;
	double fvalue;
	node(void) : lc(nullptr), rc(nullptr), feature(-1), fvalue(-1) {}
};