#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <bitset>
using namespace std;

struct HuffmanNode {
    char character;
    unsigned freq;
    HuffmanNode *left, *right;

    HuffmanNode(char c, unsigned f) : character(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HuffmanNode* l, HuffmanNode* r) {
        return l->freq > r->freq;
    }
};

unordered_map<char, string> huffmanCodes;

void generateCodes(HuffmanNode* root, string str) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanCodes[root->character] = str;
    }

    generateCodes(root->left, str + "0");
    generateCodes(root->right, str + "1");
}

HuffmanNode* buildHuffmanTree(unordered_map<char, int>& freq) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> minHeap;

    for (auto& pair : freq) {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); 
        minHeap.pop();
        HuffmanNode* right = minHeap.top(); 
        minHeap.pop();

        HuffmanNode* internal = new HuffmanNode('\0', left->freq + right->freq);
        internal->left = left;
        internal->right = right;

        minHeap.push(internal);
    }

    return minHeap.top();
}

void writeCompressedFile(string inputFile, string outputFile) {
    ifstream input(inputFile, ios::binary);
    ofstream output(outputFile, ios::binary);

    if (!input.is_open() || !output.is_open()) {
        cerr << "Error opening files.\n";
        return;
    }

    unordered_map<char, int> freq;
    char ch;
    while (input.get(ch)) {
        freq[ch]++;
    }
    input.clear();
    input.seekg(0);

    HuffmanNode* root = buildHuffmanTree(freq);
    generateCodes(root, "");

    string encodedStr = "";
    unsigned short numOfChars = huffmanCodes.size();
    unsigned int totalBits = 0;

    char pad = 0;
    output.write(&pad, 1);
    output.write(reinterpret_cast<char*>(&numOfChars), sizeof(numOfChars));

    for (auto& pair : huffmanCodes) {
        output.write(&pair.first, sizeof(pair.first));
        unsigned short codeLength = pair.second.length();
        output.write(reinterpret_cast<char*>(&codeLength), sizeof(codeLength));
        output.write(pair.second.c_str(), codeLength);
    }

    while (input.get(ch)) {
        encodedStr += huffmanCodes[ch];
    }

    pad = (8 - (encodedStr.length() % 8)) % 8;
    for (int i = 0; i < pad; i++) {
        encodedStr += "0";
    }
    output.seekp(0);
    output.write(&pad, 1);
    output.seekp(0, ios::end);
    
    for (size_t i = 0; i < encodedStr.length(); i += 8) {
        bitset<8> bits(encodedStr.substr(i, 8));
        unsigned char byte = bits.to_ulong();
        output.write(reinterpret_cast<char*>(&byte), 1);
    }

    cout << "File successfully compressed.\n";

    input.close();
    output.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = inputFile.substr(0, inputFile.find_last_of('.')) + "-compressed.bin";

    writeCompressedFile(inputFile, outputFile);

    return 0;
}
