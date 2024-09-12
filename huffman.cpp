// INITIALIZE POINTERS TO NULL IF YOU'RE NOT GONNA FUCKING USE THEM

#include <iostream>
#include <deque>
#include <vector>
#include <fstream>
#include <cstdint>
#include <unordered_map>

#define str(s) to_string(s)
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

class convert{
public:
	static char* ll_to_bytes(const ll& a){
		char* ret = new char[8];
		for(int i = 0; i < 8; i++)
			ret[i] = (a >> (8 * i)) & 0xFF;
		return ret;
	}
	static ll bytes_to_ll(const char* arr){
		ll ret = 0;
		for(int i = 0; i < 8; i++)
			ret |= (ll)(arr[i]) << (64 - i * 8);
		return ret;
	}
};

class min_heap{
private:
	size_t size = 0, capacity = 1;
	node **arr = nullptr;

	int p(int i){ return ((i - 1) >> 1); }
	int l(int i){ return ((i << 1) | 1); }
	int r(int i){ return ((i << 1) + 2); }

	void bubbleUp(int i){
		if(i <= 0 || arr[p(i)]->freq > arr[i]->freq ||
			(arr[p(i)]->freq == arr[i]->freq && arr[p(i)]->val > arr[i]->val))
			return;
		swap(arr[i], arr[p(i)]);
		bubbleUp(p(i));
	}

	void heapify(int i){
		int left = l(i);
		int right = r(i);
		int mn = i;
		if(left < size && (arr[left]->freq < arr[i]->freq || 
			(arr[left]->freq == arr[i]->freq && arr[left]->val < arr[i]->val)))
			mn = left;
		if(right < size && (arr[right]->freq < arr[mn]->freq ||
			(arr[right]->freq == arr[mn]->freq && arr[right]->val < arr[i]->val)))
			mn = right;
		if(mn != i){
			swap(arr[mn], arr[i]);
			heapify(mn);
		}
	}

public:
	
	min_heap(){ arr = new node*[capacity]; }
	
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

class huffman_tree{

public:
	uint64_t outputBitLength = 0, inputLength = 0;

private:
	string path;
	node *root = nullptr;
	unordered_map<char, string> codes;
	
	void make_codes(node* root){
		if(root->left == nullptr && root->right == nullptr)
			return void(codes[root->val] = path);

		path += '0', make_codes(root->left), path.pop_back();
	    path += '1', make_codes(root->right), path.pop_back();
	}

	void add(const string& s, deque<char>& buf){
		for(auto& i : s)
			buf.emplace_back(i);
		
		return;
	}
	
	char toBin(deque<char>& buf){
		char ret = 0x00;
		for(int i = 0; i < 8; i++){
			ret |= (buf[0] == '1') << (7 - i);
			buf.pop_front();
		}
		return ret;
	}

public:
	huffman_tree(unordered_map<char, ll>& freq) {
		min_heap heap;
		for(auto& [f, s] : freq)
			inputLength += s;

		for(auto &[f, s] : freq)
		    heap.insert(new node(f, s));

		while(heap.len() > 1){
			node *l = heap.extract();
			node *r = heap.extract();
			heap.insert(new node('\0', l->freq + r->freq, l, r));
		}

		root = heap.extract();
		make_codes(root);
	}

	string getCode(char s){ return codes[s]; }

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

		for(int i = 0; i < 8; i++){
			toWrite = (outputBitLength >> (8 * i)) & 0xFF;
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

	if(sz(arg) != 4 || (arg[2] != "e" && arg[2] != "d"))
		throw runtime_error("usage:\n./huffman input e/d output");

	ifstream inputFile(arg[1], ios::binary);

	if(!inputFile)
		throw runtime_error("incorrect input path!");

	ofstream outputFile(arg[1] + ".bin", ios::binary);

	if(arg[2] == "e") {
		unordered_map<char, ll> freq;
		
		// char byte;
		
		// while(inputFile.read(&byte, 1))
		// 	freq[byte]++;

		// inputFile.clear();
		// inputFile.seekg(0);

		// huffman_tree huffman(freq);
		// huffman.encode(inputFile, outputFile);
		char* test = convert::ll_to_bytes(0x3f3f3f3f);
		for(int i = 0; test[i]; i++) cout << test[i];
		cout << '\n';
		cout << convert::bytes_to_ll(test) << '\n';

		inputFile.close();
		outputFile.close();

		return 0;
	}
	else if(arg[2] == "d") {

	}
}
