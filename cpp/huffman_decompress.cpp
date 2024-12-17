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

unordered_map<string, char> reverseHuffmanCodes;

HuffmanNode* rebuildHuffmanTree() {
    HuffmanNode* root = new HuffmanNode('\0', 0);
    for (auto& pair : reverseHuffmanCodes) {
        HuffmanNode* current = root;
        for (char bit : pair.first) {
            if (bit == '0') {
                if (!current->left) {
                    current->left = new HuffmanNode('\0', 0);
                }
                current = current->left;
            } else if (bit == '1') {
                if (!current->right) {
                    current->right = new HuffmanNode('\0', 0);
                }
                current = current->right;
            }
        }
        current->character = pair.second;
    }
    return root;
}

void decodeString(HuffmanNode* root, string& binaryData, const string& outputFile) {
    ofstream output(outputFile, ios::binary);
    if (!output.is_open()) {
        cerr << "Error opening output file.\n";
        return;
    }

    HuffmanNode* current = root;
    for (char bit : binaryData) {
        current = (bit == '0') ? current->left : current->right;

        if (!current->left && !current->right) {
            output.put(current->character);
            current = root;
        }
    }

    output.close();
    cout << "File successfully decompressed.\n";
}

void readCompressedFile(string inputFile) {
    ifstream input(inputFile, ios::binary);
    if (!input.is_open()) {
        cerr << "Error opening input file.\n";
        return;
    }

    char pad;
    input.read(&pad, 1);
    unsigned short numOfChars;
    input.read(reinterpret_cast<char*>(&numOfChars), sizeof(numOfChars));

    reverseHuffmanCodes.clear();
    for (int i = 0; i < numOfChars; ++i) {
        char ch;
        input.read(&ch, sizeof(ch));

        unsigned short codeLength;
        input.read(reinterpret_cast<char*>(&codeLength), sizeof(codeLength));

        string code(codeLength, '0');
        input.read(&code[0], codeLength);

        reverseHuffmanCodes[code] = ch;
    }

    string binaryData = "";
    while (input.peek() != EOF) {
        unsigned char byte;
        input.read(reinterpret_cast<char*>(&byte), 1);
        bitset<8> bits(byte);
        binaryData += bits.to_string();
    }

    binaryData = binaryData.substr(0, binaryData.size() - pad);

    HuffmanNode* root = rebuildHuffmanTree();

    string outputFile = inputFile.substr(0, inputFile.find_last_of('.')) + "-decompressed.txt";
    decodeString(root, binaryData, outputFile);

    input.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <compressed_file>\n";
        return 1;
    }

    string inputFile = argv[1];
    readCompressedFile(inputFile);

    return 0;
}
