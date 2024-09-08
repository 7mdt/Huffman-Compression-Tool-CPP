// INITIALIZE POINTERS TO NULL IF YOU'RE NOT GONNA FUCKING USE THEM
#include <bits/stdc++.h>
#define sz(s) (int)(s.size())

using namespace std;
using ll = long long;

struct node{
	char val;
	ll freq;
	node *left = nullptr, *right = nullptr;
	node(char val, ll freq) : val(val), freq(freq){}
};

struct cmp {
    bool operator()(const node* lhs, node* rhs) const { return lhs->freq > rhs->freq; }
};

class HuffmanTree{

private:
	string path;
	node *root = nullptr;
	unordered_map<char, string> codes;

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
			if(buf[0] == '1')
				ret |= (1 << (7 - i));
			buf.pop_front();
		}

		return ret;
	}

public:

	string getCode(char s){ return codes[s]; }

	void buildTree(unordered_map<char, ll>& freq){

		priority_queue<node*, vector<node*>, cmp> q;

		for(auto &[f, s] : freq){
			q.push(new node(f, s));
		}

		while(q.size() > 1){
			node* left = q.top(); q.pop();
			node* right = q.top(); q.pop();
			node* parent = new node('\0', left->freq + right->freq);
			parent->left = left, parent->right = right;
			q.push(parent);
		}

	    makeCodes(root = q.top());

		return;
	}

	void encode(ifstream& inputFile, ofstream& outputFile){
		inputFile.clear();
		inputFile.seekg(0);

		char byte;
		deque<char> buf;
		
		while(inputFile.read(&byte, 1)){
			
		    add(getCode(byte), buf);

			while(sz(buf) >= 8){
				char toWrite = toBin(buf);
			    outputFile.write(&toWrite, 1);
			}
			
		}

		char padding = 0;
		while(sz(buf) < 8){
			buf.emplace_back('0');
			padding++;
		}
		outputFile.write(&padding, 1);
		
		return;
	}
	
};


signed main(int args, char* argv[]){
	
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

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


	if(arg[2] == "e") {

		string outputPath = arg[1];
		
		while(outputPath.back() != '/')
			outputPath.pop_back();

		outputPath += arg[3] + ".bin";
		
		char byte;
		unordered_map<char, ll> freq;
		ofstream outputFile(outputPath, ios::binary);

		while(inputFile.read(&byte, 1))
			freq[byte]++;

		HuffmanTree huffman;
		huffman.buildTree(freq);
		// huffman.encodeTree(ouputFile);
		huffman.encode(inputFile, outputFile);

		inputFile.close();
		outputFile.close();

		return 0;
	}

}
