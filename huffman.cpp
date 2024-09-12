// INITIALIZE POINTERS TO NULL IF YOU'RE NOT GONNA FUCKING USE THEM
// MEMORY ISN'T FREE, DELETE THE ALLOCATED MEMORY AFTER USING IT
#include <iostream>
#include <deque>
#include <vector>
#include <fstream>
#include <cstdint>
#include <iomanip>
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
            ret |= ((ll) ((unsigned char) arr[i]) << (i * 8));
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
            (arr[p(i)]->freq == arr[i]->freq && arr[p(i)]->val < arr[i]->val))
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
    uint64_t output_bits_length = 0, input_length = 0;

private:
    string path;
    node *root = nullptr;
    hash_map<char, string> codes;
    hash_map<char, ll> freq;

    void make_codes(node *node) {
        if (node->left == nullptr && node->right == nullptr) {
            output_bits_length += (ll) (sz(path)) * freq[node->val];
            input_length += (ll) freq[node->val];
            return void(codes[node->val] = path);
        }
        path += '0', make_codes(node->left), path.pop_back();
        path += '1', make_codes(node->right), path.pop_back();
    }

    char extract_buffer(deque<char> &buf) {
        char ret = 0;
        for (int i = 0; i < 8; i++) {
            if (buf.empty())
                break;
            ret |= (buf[0] == '1') << (7 - i);
            buf.pop_front();
        }
        return ret;
    }

public:
    huffman_tree(hash_map<char, ll> &freq) {
        min_heap heap;

        for (auto &x: freq) {
            auto f = x.first;
            auto s = x.second;
            heap.insert(new node(f, s));
        }

        while (heap.len() > 1) {
            node *l = heap.extract();
            node *r = heap.extract();
            heap.insert(new node('\0', l->freq + r->freq, l, r));
        }

        this->freq = freq;
        root = heap.extract();
        make_codes(root);
    }

    void encode(ifstream &input_file, ofstream &output_file) {
        input_file.clear();
        input_file.seekg(0);

        char to_write;
        deque<char> buf;

        output_file.write(convert::ll_to_bytes(sz(codes)), 8);
        for (const auto &x: freq) {
            auto f = x.first;
            auto s = x.second;
            output_file.write(&f, 1);
            output_file.write(convert::ll_to_bytes(s), 8);
        }

        while (input_file.read(&byte, 1)) {
            for (char &bit: codes[byte])
                buf.emplace_back(bit);
            while (sz(buf) >= 8)
                output_file.write(&(to_write = extract_buffer(buf)), 1);
        }

        if (!buf.empty())
            output_file.write(&(to_write = extract_buffer(buf)), 1);

        return;
    }

    void decode(ifstream &input_file, ofstream &output_file, ll bits_to_read) {
        int idx = -1;
        node *cur = root;
        while (bits_to_read-- >= 0) {
            if (idx <= -1) {
                idx = 7;
                input_file.read(&byte, 1);
            }
            if (cur->left == nullptr && cur->right == nullptr) {
                output_file.write(&cur->val, 1);
                cur = root;
            }
            cur = (byte & (1 << idx--)) ? cur->right : cur->left;
        }
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

    string arg[] = {argv[0], argv[1], argv[2]};

    ifstream input_file(arg[1], ios::binary);

    if (!input_file) {
        cerr << "incorrect path\n";
        exit(-2);
    }

    cout << fixed << setprecision(2);
    if (arg[2] == "e") {
        ofstream output_file(arg[1] + ".bin", ios::binary);

        hash_map<char, ll> freq;
        while (input_file.read(&byte, 1))
            freq[byte]++;

        huffman_tree tree(freq);
        tree.encode(input_file, output_file);

        input_file.close();
        output_file.close();

        float before = float(tree.input_length) / float(1024 * 1024);
        float after = float(tree.output_bits_length / 8 + 1 + sz(freq) * 9) / float(1024 * 1024);
        cout << "file size before compression: " << before << "mb\n";
        cout << "file size after compression: " << after << "mb\n";
        cout << "compression ratio: " << (before - after) / before * 100.0 << "%\n";

        return 0;
    } else if (arg[2] == "d") {
        if (sz(arg[1]) <= 4 || arg[1].substr(sz(arg[1]) - 4) != ".bin") {
            cerr << "invalid file name, file name should end with .bin";
            exit(-4);
        }

        ofstream output_file(arg[1].substr(0, sz(arg[1]) - 4), ios::binary);

        char val, buffer[8];
        ll freq_length;
        hash_map<char, ll> freq;

        input_file.read(buffer, 8);
        freq_length = convert::bytes_to_ll(buffer);

        for (int i = 0; i < freq_length; i++) {
            input_file.read(&val, 1);
            input_file.read(buffer, 8);
            freq[val] = convert::bytes_to_ll(buffer);
        }

        huffman_tree tree(freq);
        tree.decode(input_file, output_file, tree.output_bits_length);

        input_file.close();
        output_file.close();

        return 0;
    }

    cerr << "invalid second argument\n";
    exit(-3);
}