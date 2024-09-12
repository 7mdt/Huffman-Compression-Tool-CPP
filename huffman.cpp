// INITIALIZE POINTERS TO NULL IF YOU'RE NOT GONNA FUCKING USE THEM
#include <iostream>
#include <deque>
#include <vector>
#include <fstream>
#include <cstdint>
#include <iomanip>
#include <hash_map>
#include <unordered_map>

#define str(n) to_string(n)
#define sz(s) (int)(s.size())
#define hash_map unordered_map

using namespace std;
using ll = long long;

char byte;

struct node {
    char val = '\0';
    ll freq = 0;
    node *left = nullptr, *right = nullptr;

    node(char val, ll freq) : val(val), freq(freq) {}

    node(char val, ll freq, node *left, node *right) : val(val), freq(freq), left(left), right(right) {}
};

class convert {
public:
    static const char *ll_to_bytes(const ll &a) {
        char *ret = new char[8];
        for (int i = 0; i < 8; i++)
            ret[i] = (a >> (8 * i)) & 0xFF;
        return ret;
    }

    static const ll bytes_to_ll(const char arr[]) {
        ll ret = 0;
        for (int i = 0; i < 8; i++)
            ret |= (((unsigned char) arr[i]) << (i * 8));
        return ret;
    }
};

class min_heap {
private:
    size_t size = 0, capacity = 1;
    node **arr = nullptr;

    static int p(int i) { return ((i - 1) >> 1); }

    static int l(int i) { return ((i << 1) | 1); }

    static int r(int i) { return ((i << 1) + 2); }

    void bubbleUp(int i) {
        if (i <= 0 || arr[p(i)]->freq < arr[i]->freq ||
            (arr[p(i)]->freq == arr[i]->freq && arr[p(i)]->val > arr[i]->val))
            return;
        swap(arr[i], arr[p(i)]);
        bubbleUp(p(i));
    }

    void heapify(int i) {
        int left = l(i);
        int right = r(i);
        int mn = i;
        if (left < size && (arr[left]->freq < arr[i]->freq ||
                            (arr[left]->freq == arr[i]->freq && arr[left]->val < arr[i]->val)))
            mn = left;
        if (right < size && (arr[right]->freq < arr[mn]->freq ||
                             (arr[right]->freq == arr[mn]->freq && arr[right]->val < arr[i]->val)))
            mn = right;
        if (mn != i) {
            swap(arr[mn], arr[i]);
            heapify(mn);
        }
    }

public:
    min_heap() { arr = new node *[capacity]; }

    int len() { return size; }

    node *extract() {
        if (size <= 0)
            return nullptr;
        node *ret = arr[0];
        arr[0] = arr[--size];
        heapify(0);
        return ret;
    }

    void insert(node *k) {
        if (size == capacity) {
            capacity <<= 1;
            node **dummy = new node *[capacity];
            for (int i = 0; i < size; i++)
                dummy[i] = arr[i];
            delete[] arr;
            arr = dummy;
        }
        arr[size++] = k;
        bubbleUp(size - 1);
    }
};

class huffman_tree {

public:
    uint64_t outputBitLength = 0, inputLength = 0;

private:
    string path;
    node *root = nullptr;
    hash_map<char, string> codes;
    hash_map<char, ll> freq;

    void make_codes(node *node) {
        if (node->left == nullptr && node->right == nullptr)
            return void(codes[node->val] = path);

        path += '0', make_codes(node->left), path.pop_back();
        path += '1', make_codes(node->right), path.pop_back();
    }

    char toBin(deque<char> &buf) {
        char ret = 0x00, limit = min(sz(buf), 8);
        for (int i = 0; i < limit; i++) {
            ret |= (buf[0] == '1') << (7 - i);
            buf.pop_front();
        }
        return ret;
    }

public:
    huffman_tree(hash_map<char, ll> &freq) {
        min_heap heap;
        for (auto &[f, s]: freq)
            inputLength += s;

        for (auto &[f, s]: freq)
            heap.insert(new node(f, s));

        while (heap.len() > 1) {
            node *l = heap.extract();
            node *r = heap.extract();
            heap.insert(new node('\0', l->freq + r->freq, l, r));
        }

        this->freq = freq;
        root = heap.extract();
        make_codes(root);
    }

    string getCode(char s) { return codes[s]; }

    void encode(ifstream &inputFile, ofstream &outputFile) {

        char toWrite = sz(codes);
        deque<char> buf;

        outputFile.write(&toWrite, 1);
        for (const auto &[f, s]: freq) {
            outputFile.write(&f, 1);
            outputFile.write(convert::ll_to_bytes(s), 8);
        }

        while (inputFile.read(&byte, 1)) {
            for (char &bit: huffman_tree::getCode(byte)) {
                buf.emplace_back(bit);
                ++outputBitLength;
            }
            while (sz(buf) >= 8) {
                toWrite = toBin(buf);
                outputFile.write(&toWrite, 1);
            }
        }

        toWrite = toBin(buf);
        outputFile.write(&toWrite, 1);
        outputFile.write(convert::ll_to_bytes(outputBitLength), 8);

        inputFile.clear();
        inputFile.seekg(0);

        return;
    }

    void decode(ifstream &inputFile, ofstream &outputFile, ll bitsToRead) {
        int idx = -1;
        node *cur = root;
        while (bitsToRead-- >= 0) {
            if (idx == -1) {
                idx = 7;
                inputFile.read(&byte, 1);
            }
            if (cur->left == nullptr && cur->right == nullptr) {
                outputFile.write(&cur->val, 1);
                cur = root;
            }
            cur = (byte & (1 << idx)) ? cur->right : cur->left;
            idx--;
        }
        return;
    }

};

signed main(int args, char *argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    if (args != 3) {
        cerr << ("usage:\n./huffman input e/d\n");
        exit(-1);
    }

    vector<string> arg;
    for (int i = 0; i < args; i++)
        arg.emplace_back(argv[i]);

    ifstream inputFile(arg[1], ios::binary);

    if (!inputFile) {
        cerr << "incorrect path\n";
        exit(-2);
    }

    if (arg[2] == "e") {
        ofstream outputFile(arg[1] + ".bin", ios::binary);

        hash_map<char, ll> freq;
        while (inputFile.read(&byte, 1))
            freq[byte]++;
        inputFile.clear();
        inputFile.seekg(0);

        huffman_tree huffman(freq);
        huffman.encode(inputFile, outputFile);

        inputFile.close();
        outputFile.close();

        return 0;
    }
    else if (arg[2] == "d") {
        ofstream outputFile(arg[1] + "out", ios::binary);

        ll bytesToRead;
        char freqToRead, val, buffer[8];
        hash_map<char, ll> freq;

        inputFile.seekg(-8, ios::end);
        inputFile.read(buffer, 8);
        inputFile.seekg(0);
        inputFile.clear();
        inputFile.read(&freqToRead, 1);
        bytesToRead = convert::bytes_to_ll(buffer);

        while (freqToRead--) {
            inputFile.read(&val, 1);
            inputFile.read(buffer, 8);
            freq[val] = convert::bytes_to_ll(buffer);
        }

        huffman_tree tree(freq);
        tree.decode(inputFile, outputFile, bytesToRead);

        return 0;
    }

    cerr << "invalid second argument\n";
}
