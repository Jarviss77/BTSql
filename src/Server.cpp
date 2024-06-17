#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;

const unsigned short HEADER_SIZE = 100;
int main(int argc, char* argv[]) {
    // Flush after every cout / cerr
    cout << unitbuf;
    cerr << unitbuf;

    
    cout << "Logs from your program will appear here" << endl;

    if (argc != 3) {
        cerr << "Expected two arguments" << endl;
        return 1;
    }

    string database_file_path = argv[1];
    string command = argv[2];

    if (command == ".dbinfo") {
        ifstream database_file(database_file_path, ios::binary);
        if (!database_file) {
            cerr << "Failed to open the database file" << endl;
            return 1;
        }

        database_file.seekg(16);
        
        char buffer[2];
        database_file.read(buffer, 2);
        
        unsigned short page_size = (static_cast<unsigned char>(buffer[1]) | (static_cast<unsigned char>(buffer[0]) << 8));
        database_file.seekg(HEADER_SIZE + 3);
        database_file.read(buffer , 2);

        // cout<<static_cast<unsigned char>(buffer[0])<<" "<<(static_cast<unsigned char>(buffer[1]) << 8)<<endl;
        
        unsigned short table_count = (static_cast<unsigned char>(buffer[1]) | (static_cast<unsigned char>(buffer[0]) << 8));
        
        cout << "database page size: " << page_size << endl;
        cout << "No. of page tables: " << table_count << endl;
    }

    return 0;
}
