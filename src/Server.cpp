// not gonna lie, chutiyap hai ye
// not gonna fucking touch this project again
// ~ Jarvis77 signing off
#include <cstring>
#include <iostream>
#include <fstream>

#define CAST(value) static_cast<unsigned char>(value)
#define MASK '\x80'
#define MASK2 '\x7F'

using namespace std;

const unsigned short HEADER_SIZE = 100;
const unsigned short PAGE_HEADER_SIZE = 8;

// record array
// length of array
// offset after read (caller make sure offset is less than len)
size_t var_int_decode(const char *record, const size_t len, size_t &offset)
{
    size_t ret(0);
    do
    {
        ret <<= 7;
        ret += (record[offset] & MASK2);
        ++offset;
    } while (record[offset - 1] & MASK);
    return ret;
}
// https://www.sqlite.org/fileformat.html#record_format
size_t serial_type_code_to_bytes(size_t type_code)
{
    switch (type_code)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    {
        return type_code;
    };
    case 5:
    {
        return 6;
    };
    case 6:
    case 7:
    {
        return 8;
    };
    case 8:
    case 9:
    {
        return 0;
    }
    case 10:
    case 11:
    {
        throw std::logic_error("not implemented");
        return 0;
    };
    default: // for case 0,1,2,3,4
    {
        if (type_code % 2)
        {
            return (type_code - 13) / 2;
        }
        else
        {
            return (type_code - 12) / 2;
        };
    };
    }
}
void print_table_name_from_record(const char *record, const size_t len)
{
    // record id
    // header decode
    // std::cout << "; len: " << len << std::endl;
    size_t offset(0);
    size_t record_id = var_int_decode(record, len, offset);
    size_t record_len = offset;
    size_t header_size = var_int_decode(record, len, offset);
    size_t type_len = var_int_decode(record, len, offset);
    size_t table_name_len = var_int_decode(record, len, offset);
    size_t type_byte_len = serial_type_code_to_bytes(type_len);
    size_t table_name_bytes = serial_type_code_to_bytes(table_name_len);
    for (int i = 0; i < table_name_bytes; i++)
    {
        cout << record[i + record_len + header_size + type_byte_len];

        
    }
}


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
        
        unsigned short page_size = (CAST(buffer[1]) | (CAST(buffer[0]) << 8));
        database_file.seekg(HEADER_SIZE + 3);
        database_file.read(buffer , 2);

        // cout<<static_cast<unsigned char>(buffer[0])<<" "<<(static_cast<unsigned char>(buffer[1]) << 8)<<endl;
        
        unsigned short table_count = (CAST(buffer[1]) | (CAST(buffer[0]) << 8));
        
        cout << "database page size: " << page_size << endl;
        cout << "number of tables: " << table_count << endl;
    }

    if(command == ".tables"){
        ifstream database_file(database_file_path, ios::binary);
        if (!database_file) {
            cerr << "Failed to open the database file" << endl;
            return 1;
        }

        database_file.seekg(HEADER_SIZE);
        char page_buffer[PAGE_HEADER_SIZE];
        database_file.read(page_buffer , PAGE_HEADER_SIZE);

        if(page_buffer[0] == '\x0D'){
            unsigned short rows = (CAST(page_buffer[3]) | (CAST(page_buffer[4]) << 8));
            char ptrs[2 * rows];

            database_file.read(ptrs , 2 * rows);
            bool separator = false;
            for (int r = 0; r < rows; ++r)
            {
                // std::cout << "rows count: " << r << "|";
                if (separator)
                    std::cout << " ";
                // Record address
                int addr = ((CAST(ptrs[r * 2]) << 8) | CAST(ptrs[2 * r + 1]));
                // std::cout << ptrs[r * 2] << ": " << ptrs[r * 2 + 1] << ";" << r << "|";
                database_file.seekg(addr);
                // Get the record length
                size_t buf_size(0);
                char tmp[1];
                size_t offset = 0;
                do
                {
                    database_file.read(tmp, 1);
                    buf_size <<= 7;
                    buf_size += (tmp[0] & MASK2);
                    // std::cout << ";" << tmp << ";buf_size: " << buf_size << "|";
                    ++offset;
                } while (tmp[0] & MASK);
                // read tbe rest buffer (beside header)
                char record[buf_size - offset];
                database_file.read(record, buf_size - offset);
                // std::cout << (tmp[0] & MASK) << "addr:" << addr << " buf_size: " << buf_size << " offset:" << offset << std::endl;
                print_table_name_from_record(record, buf_size - offset);
                separator = true;
            }
        }
        else
        {
            logic_error("Invalid page type");
        }


    }

    return 0;
}

