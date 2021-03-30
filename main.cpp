#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <math.h>
#include <queue>
#include <filesystem>
#include <chrono>
using namespace std::chrono;
using namespace std;
namespace fs= std::__fs::filesystem;
struct MinHeapNode {
    char data;
    unsigned freq;
    string code;
    MinHeapNode *left, *right;
    MinHeapNode(char data, unsigned freq)
    {
        left = right = NULL;
        this->data = data;
        this->freq = freq;
        this->code = "";
    }
};
struct compare {

    bool operator()(MinHeapNode* l, MinHeapNode* r)

    {
        return (l->freq > r->freq);
    }
};

void printCodes(struct MinHeapNode* root, string str)
{

    if (!root)
        return;

    if (root->data != -1) {
        root->code = root->code + str;
    }
    printCodes(root->left, str + "0");
    printCodes(root->right, str + "1");
}

void getCode(struct MinHeapNode* root, vector<pair<char , string>>& codes){

    if(root->left != NULL){
        getCode(root->left, codes);
    }

    if (root->right != NULL){
        getCode(root->right, codes);
    }

    if(root->right == NULL && root->left == NULL){  //leaf node
        pair<unsigned , string> temp;
        temp.first = root->data;
        temp.second = root->code;
        codes.push_back(temp);
    }
}
void HuffmanCodes(vector<char> data, vector<int> freq, int size, vector<pair<char , string>>& codes)
{
    struct MinHeapNode *left, *right, *top;
    priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap;

    for (int i = 0; i < size; ++i)
        minHeap.push(new MinHeapNode(data[i], freq[i]));
    while (minHeap.size() != 1) {

        left = minHeap.top();
        minHeap.pop();

        right = minHeap.top();
        minHeap.pop();

        top = new MinHeapNode(-1, left->freq + right->freq);

        top->left = left;
        top->right = right;

        minHeap.push(top);
    }

    printCodes(minHeap.top(), "");

    getCode(minHeap.top(), codes);

}
vector<bitset<1>> bitCompression(string message){
    char c;
    vector<bitset<1>> result;

    for(int i = 0; i<message.size();i++) {
        c = message[i];
        bitset<1> temp (c);
        result.push_back(temp);
    }
    return result;

}
string char_to_str(char ch)
{
    bitset<8> temp(ch);
    return temp.to_string();
}

vector<unsigned char> toByte(vector<bitset<1>> bits){
    vector<unsigned char> result;
    bitset<8> byte;
    int i =7;
    for(auto it=bits.begin();it<=bits.end();++it){
        if(i==-1){
            char c = char(byte.flip().to_ulong());
            result.push_back(c);
            i = 7;
        }
        if (*it == 1)
            byte.set(i);
        else if(*it == 0)
            byte.set(i,0);
        i--;
    }
    return result;
}

string messagecompression(string message,vector<pair<char,string>> codes,int& padding){
    string compressed = "";
    char temp;
    for(int i = 0; i<message.size();i++){
        temp = message[i];
        auto it =  find_if(codes.begin(), codes.end(), [temp](const pair<char ,string>& p ){ return p.first == temp; });
        int idx =  it - codes.begin();//index of char in the code map
        compressed+=codes[idx].second;
    }
    padding = compressed.size()%8;
    if(padding!=0)
        for(int i = 0 ;i<8-padding;i++){
            compressed+='0';
        }
    return compressed;
}
string messageDecompress(vector<pair<char , string>>codes,string message,int padding){
    while (padding!=0)//removing the padding
    {
        message.pop_back();
        padding--;
    }
    string decompressed = "";
    string temp = "";
    bool found;
    for (int i = 0 ; i<message.size();i++){
        temp +=message[i];
        auto it =  find_if(codes.begin(), codes.end(), [temp](const pair<char ,string>& p ){ return p.second == temp; });
        int idx = it - codes.begin();//index of char in the code map
        if(idx==codes.size())
            found = false;
        else
            found = true;
        if(found){
            decompressed += codes[idx].first;
            temp = "";
        }
    }
    return decompressed;
}
string chartobin ( unsigned char c )
{
//    static unsigned
    char bin[CHAR_BIT +1 ] = { 0 };
    int i;

    for ( i = CHAR_BIT - 1; i >= 0; i-- )
    {
        bin[i] = (c % 2) + '0';
        if(bin[i]=='0')
            bin[i]='1';
        else
            bin[i] = '0';
        c /= 2;
    }


    return bin;
}
vector<string>makeOrdinaryCodes(vector<char>  elements){
    vector<string> result;
    for (int i =0; i<elements.size(); i++) {
        bitset<8> code(i);
        result.push_back(code.to_string());
    }
    return result;
}
string readFromFile(const char* filename, vector<char>& element, vector<int>& freq){
    string message="";
    FILE* file = fopen(filename, "r");
    while (!feof(file)){
        char temp;
        bool found;
        fscanf(file, "%c", &temp);
        message+=temp;
        found = find(element.begin(), element.end(), temp) != element.end();
        if(found){
            auto it = find(element.begin(), element.end(), temp);
            freq[it - element.begin()]++;
        }
        else{
            element.push_back(temp);
            freq.push_back(1);
        }


    }
    fclose(file);
    freq[freq.size()-1]--;
    message.pop_back();
    return message;
}
void printCodesTable(vector<pair<char , string>>& codes, vector<string>& oldCodes){
    cout<<"Byte\t\tCode\t\tNew Code"<<endl<<"-------------------------------------------------------------"<<endl;
    auto it2 = oldCodes.begin();
    int i = 0;
    for(auto&it : codes){
        cout<<i++<<"\t\t"<<*it2<<"\t\t"<<it.second<<endl;
        it2++;
    }
    cout<<"-------------------------------------------------------------"<<endl;
}
void compressFile(string message, int& padding, vector<pair<char , string>>& codes, const char* filename){
    string after = messagecompression(message,codes,padding);
    vector<bitset<1>> result = bitCompression(after);
    vector<unsigned char> resultByte = toByte(result);
    FILE* write = fopen(filename, "w");
    fprintf(write, "Padding:%d Size:%d||codes:", padding, codes.size());
    for(auto&it : codes){
        fprintf(write, "%c ", it.first);
        for(auto& it2 : it.second)
            fprintf(write, "%c", it2);
        fprintf(write, " ");
    }

    for(auto& it: resultByte)
         fprintf(write, "%c", it);
    fclose(write);
    cout<<endl<<"Compression Ratio = "<<(float)resultByte.size()/(float)message.size()<<endl;
}

void decompressDir(const char* inputfile,const  char* outputfile){
    FILE* r = fopen(inputfile, "r+");
           string output="";
           vector<unsigned char > reads;
           int padding_2, codesN, fileNumber;
     fscanf(r, "Message number:%d ", &fileNumber);
    vector<int> filesize(fileNumber);
    for(int i = 0; i<fileNumber;i++){
        fscanf(r, "%d ", &filesize[i]);
    }
           fscanf(r, "Padding:%d Size:%d||codes:", &padding_2, &codesN);
           vector<pair<char, string>> codes_2(codesN);
           for (int i= 0; i<codesN; i++) {
               char temp;
               char ctemp;
               fscanf(r, "%c ", &ctemp);
               codes_2[i].first = ctemp;
               while (1) {
                   fscanf(r, "%c", &temp);
                   if(temp == ' ') break;
                   codes_2[i].second.push_back(temp);
               }

           }
           fscanf(r, " ");
           cout<<endl<<"----------------wait--------------"<<endl;
           while (!feof(r)){
               unsigned char temp;
               fscanf(r, "%c", &temp);
               reads.push_back(temp);
           }

           fclose(r);
    output = "";
    reads.pop_back();


    for(int i = 0 ; i<reads.size();i++){
        output += chartobin(reads[i]);
    }
    string decompressmessage;
    if(padding_2 != 0 )
    decompressmessage = messageDecompress(codes_2, output, 8-padding_2);
    else    decompressmessage = messageDecompress(codes_2, output, 0);

    vector<string> decompres(fileNumber);
    int i = 0;
    int z =0;
    while (fileNumber != 0 ) {
        for(int j = 0 ; j<filesize[i];j++){
            decompres[i] += decompressmessage[z];
            z++;
        }
        char * temp;
        string dir(outputfile);
        dir+="/message";
        dir+=to_string(i+1);
        dir+=".txt";
       // strcpy(temp,dir.c_str());
        FILE* fptr = fopen(dir.c_str(), "w");
        for(int g = 0 ; g <decompres[i].size();g++)
            fprintf(fptr,"%c",decompres[i][g]);
        fclose(fptr);
        i++;
        fileNumber--;
    }



}
void compressDir(string message, int& padding, vector<pair<char , string>>& codes,const char* filename, vector<string>& messages){
    string after = messagecompression(message,codes,padding);
    vector<bitset<1>> result = bitCompression(after);
    vector<unsigned char> resultByte = toByte(result);
    FILE* write = fopen(filename, "w");
    fprintf(write, "Message number:%d ", messages.size());
    for(int i = 0; i<messages.size(); i++){
        fprintf(write, "%d ", messages[i].size());
    }
    fprintf(write, "Padding:%d Size:%d||codes:", padding, codes.size());
    for(auto&it : codes){
        fprintf(write, "%c ", it.first);
        for(auto& it2 : it.second)
            fprintf(write, "%c", it2);
        fprintf(write, " ");
    }

    for(auto& it: resultByte)
         fprintf(write, "%c", it);
    fclose(write);
    cout<<endl<<"Compression Ratio = "<<(float)resultByte.size()/(float)message.size()<<endl;
}
void decompress(const char* inputfile,const char* outputfile){
     FILE* r = fopen(inputfile, "r+");
        string output="";
        vector<unsigned char > reads;
        int padding_2, codesN;
        fscanf(r, "Padding:%d Size:%d||codes:", &padding_2, &codesN);
        vector<pair<char, string>> codes_2(codesN);
        for (int i= 0; i<codesN; i++) {
            char temp;
            char ctemp;
            fscanf(r, "%c ", &ctemp);
            codes_2[i].first = ctemp;
            while (1) {
                fscanf(r, "%c", &temp);
                if(temp == ' ') break;
                codes_2[i].second.push_back(temp);
            }

        }
        fscanf(r, " ");

        while (!feof(r)){
            unsigned char temp;
            fscanf(r, "%c", &temp);
            reads.push_back(temp);

        }

        fclose(r);
        output = "";
        reads.pop_back();

        for(int i = 0 ; i<reads.size();i++){
            output += chartobin(reads[i]);
        }
    FILE *f = fopen(outputfile, "w");
    string decompressmessage;
        if(padding_2 != 0 )
        decompressmessage = messageDecompress(codes_2, output, 8-padding_2);
        else    decompressmessage = messageDecompress(codes_2, output, 0);
       fprintf(f, "%s", decompressmessage.c_str());
       fclose(f);

}
int main() {
    cout<<"Enter the path of File/Directory"<<endl;
    string s1;
    getline(cin, s1);
    const char* inputPath = s1.c_str();
    cout<<"Choose one of the following:\n-------------------------------------\n";
    cout<<"1-Compress File\n2-Compress Directory\n3-Decompress File\n4-Decompress Directory\n";
    int choice;
    cin>>choice;
    switch (choice) {
        case 1:{
            auto start = high_resolution_clock::now();
            vector<char> element;
            vector<int > freq;
            string message = readFromFile(inputPath, element, freq);
            vector<pair<char , string>> codes;
            vector<string> oldCodes = makeOrdinaryCodes(element);
            HuffmanCodes(element, freq, element.size(), codes);
            printCodesTable(codes, oldCodes);
            int padding;
            string dir = s1.substr(0, s1.find_last_of("\\/"));
            dir+="/compressedFile.txt";
            const char* outputPath = dir.c_str();
            compressFile(message, padding, codes, outputPath);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout<<endl<<"The execution time = "<<duration.count()<<"\tmicroseconds"<<endl<<endl;

        }
            break;
        case 2:{
            auto start = high_resolution_clock::now();
            vector<char> element;
            vector<int > freq;
            vector<string> file;
            string path =inputPath;
            for(const auto & entry : fs::directory_iterator(path)){
                file.push_back(entry.path());
            }
            for (auto &it : file) {
                cout<<it<<endl;
            }
            cout<<file.size()<<endl;
            vector<string> messages(file.size());
            string mainMessage = "";

            int j = 0;
            for (int i = file.size()-1; i>= 0; i--) {
                messages[j] = readFromFile(file[i].c_str(), element, freq);
                mainMessage += messages[j++];
                }
            vector<pair<char , string>> codes;
            HuffmanCodes(element, freq, element.size(), codes);
            vector<string> oldCodes = makeOrdinaryCodes(element);
            printCodesTable(codes, oldCodes);
            int padding;
            string dir = s1.substr(0, s1.find_last_of("\\/"));
            dir+="/compressedDirectory.txt";
            const char* outputPath = dir.c_str();
            compressDir(mainMessage, padding, codes, outputPath, messages);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout<<endl<<"The execution time = "<<duration.count()<<"\tmicroseconds"<<endl<<endl;
        }
            break;
        case 3:{
            auto start = high_resolution_clock::now();
            string dir = s1.substr(0, s1.find_last_of("\\/"));
            dir+="/originalFile.txt";
            decompress(inputPath, dir.c_str());
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout<<endl<<"The execution time = "<<duration.count()<<"\tmicroseconds"<<endl<<endl;

        }
            break;
        case 4:{
            auto start = high_resolution_clock::now();
            string dir = s1.substr(0, s1.find_last_of("\\/"));
            decompressDir(inputPath, dir.c_str());
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout<<endl<<"The execution time = "<<duration.count()<<"\tmicroseconds"<<endl<<endl;
        }
            break;
        default:
            break;
    }

    return 0;
}

