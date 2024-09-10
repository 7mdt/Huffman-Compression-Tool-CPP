// INITIALIZE POINTERS TO NULL IF YOU'RE NOT GONNA FUCKING USE THEM

#include <bits/stdc++.h>

#define sz(s) (int)(s.size())

using namespace std;
using ll = long long;

struct node{
	char val = '\0';
	ll freq = 0;
	node *left = nullptr, *right = nullptr;
	node() {}
	node(char val, ll freq) : val(val), freq(freq) {}
	node(char val, ll freq, node *left, node *right) : val(val), freq(freq), left(left), right(right) {}
};

class minHeap{
private:
	size_t size = 0, capacity = 1;
	node **arr = nullptr;

	int p(int i){ return ((i - 1) >> 1); }
	int l(int i){ return ((i << 1) + 1); }
	int r(int i){ return ((i << 1) + 2); }

	void bubbleUp(int i){
		if(i <= 0 || arr[i]->freq >= arr[p(i)]->freq)
			return;
		swap(arr[i], arr[p(i)]);
		bubbleUp(p(i));
	}

	void heapify(int i){
		int left = l(i);
		int right = r(i);
		int mn = i;
		if(left < size && arr[left]->freq < arr[i]->freq)
			mn = left;
		if(right < size && arr[right]->freq < arr[mn]->freq)
			mn = right;
		if(mn != i){
			swap(arr[mn], arr[i]);
			heapify(mn);
		}
	}

public:
	
	minHeap(){ arr = new node*[capacity]; }
	
	int len() { return size; }

	node* peek() { return size > 0 ? arr[0] : nullptr; }

	node* extract() {
		if(size <= 0)
			return nullptr;
		node* ret = arr[0];
	    arr[0] = arr[--size];
		heapify(0);
		return ret;
	}
	
	void insert(node *k) {
		if(size == capacity){
			capacity <<= 1;
			node **dummy = new node*[capacity];
			for(int i = 0; i < size; i++)
				dummy[i] = arr[i];
			delete[] arr;
			arr = dummy;
		}
		arr[size++] = k;
		bubbleUp(size - 1);
	}
};


class HuffmanTree{

private:
	string path;
	node *root = nullptr;
	unordered_map<char, string> codes;
	unordered_map<char, size_t> freq;
	uint64_t outputBitLength = 0, inputLength = 0;
	
	void makeCodes(node* root){
		if(root->left == nullptr && root->right == nullptr){
		    codes[root->val] = path;
			cout << root->val << ' ' << path << '\n';
			return;
		}

		path += '0', makeCodes(root->left), path.pop_back();
	    path += '1', makeCodes(root->right), path.pop_back();
	}

	void add(const string& s, deque<char>& buf){
		for(auto& i : s)
			buf.emplace_back(i);
		
		return;
	}
	
	char toBin(deque<char>& buf){
		char ret = 0x00;
		for(int i = 0; i < 8; i++){
			ret |= ((buf[0] == '1') << (7 - i));
			buf.pop_front();
		}
		return ret;
	}

public:
	HuffmanTree(ifstream& inputFile) {
		char byte;
		while(inputFile.read(&byte, 1)){
			freq[byte]++;
			inputLength++;
		}
		inputFile.clear();
		inputFile.seekg(0);
	}
	
	string getCode(char s){ return codes[s]; }

	void buildTree(){
		minHeap heap;

		for(auto &[f, s] : freq)
		    heap.insert(new node(f, s));

		while(heap.len() > 1){
			node *l = heap.extract();
			node *r = heap.extract();
			heap.insert(new node('\0', l->freq + r->freq, l, r));
		}

		root = heap.extract();
		makeCodes(root);

		return;
	}

	void encode(ifstream& inputFile, ofstream& outputFile){
		char byte, toWrite;
		deque<char> buf;

	    while(inputFile.read(&byte, 1)){
		    add(getCode(byte), buf);
			while(sz(buf) >= 8){
				toWrite = toBin(buf);
			    outputFile.write(&toWrite, 1);
				outputBitLength += 8;
			}
		}

		outputBitLength += sz(buf);
		
		while(sz(buf) < 8)
			buf.emplace_back('0');

		toWrite = toBin(buf);
		outputFile.write(&toWrite, 1);

	    for(uint64_t mask = (0xFFULL << 56), shift = 56; mask > 0; mask >>= 8, shift -= 8){
			toWrite = (mask & outputBitLength) >> shift;
			outputFile.write(&toWrite, 1);
		}

		inputFile.clear();
		inputFile.seekg(0);

		return;
	}
};


signed main(int args, char* argv[]){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
	cout.tie(nullptr);

	vector<string> arg;

	for(int i = 0; i < args; i++)
		arg.emplace_back(argv[i]);

	if(sz(arg) != 4 || (arg[2] != "e" && arg[2] != "d")) {
		cerr << "usage:\n./huffman input e\\d output\n";
		return -1;
	}

	ifstream inputFile(arg[1], ios::binary);

	if(!inputFile) {
		cerr << "error opening file\n";
		return -2;
	}

	string outputPath = arg[1];

	while(!outputPath.empty() && outputPath.back() != '/')
		outputPath.pop_back();

	outputPath += arg[3] + ".bin";

	ofstream outputFile(outputPath, ios::binary);

	if(arg[2] == "e") {
		HuffmanTree huffman(inputFile);
		huffman.buildTree();
		// huffman.encodeTree(ouputFile); // will implement tree encoding soon
		huffman.encode(inputFile, outputFile);
		inputFile.close();
		outputFile.close();

		return 0;
	}

}
