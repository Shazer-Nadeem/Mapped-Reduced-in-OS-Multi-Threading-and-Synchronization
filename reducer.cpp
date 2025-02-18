#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
#include <fstream>

using namespace std;

void pipe_read(const string &pipe_name, char data1[][50], int value[], int &size) {
    int pipeFd = open(pipe_name.c_str(), O_RDONLY);
    if (pipeFd < 0) {
        perror("Error opening pipe for reading");
        return;
    }

    char buffer[256] = {0};
    char data[1024] = {0};
    size = 0;

    int bytes_read;
    while ((bytes_read = read(pipeFd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        strcat(data, buffer);
    }

    if (bytes_read < 0) {
        perror("Error reading from pipe");
    }

    char *temp2 = strtok(data, "\n");
    while (temp2 != nullptr) {
        char key[50];
        int value2;
        if (sscanf(temp2, "%s %d", key, &value2) == 2) {
            strcpy(data1[size], key);
            value[size] = value2;
            size++;
        } else {
            
        }
        temp2 = strtok(nullptr, "\n");
    }

    close(pipeFd);
}

void shuffle_function(char key[][50], int value[], int size, char unique_key[][50], int grouped_value[][100], int &unique_key_count) {
    unique_key_count = 0;

    for (int i = 0; i < size; ++i) {
        if (strlen(key[i]) == 0) {
            continue;
        }

        bool found = false;

        for (int j = 0; j < unique_key_count; ++j) {
            if (strcmp(key[i], unique_key[j]) == 0) {
                grouped_value[j][grouped_value[j][0] + 1] = value[i];
                grouped_value[j][0]++;
                found = true;
                break;
            }
        }

        if (!found) {
            strcpy(unique_key[unique_key_count], key[i]);
            grouped_value[unique_key_count][0] = 1;
            grouped_value[unique_key_count][1] = value[i];
            unique_key_count++;
        }
    }

    cout << "\n--- Shuffle Phase Results ---" << endl;
    for (int i = 0; i < unique_key_count; ++i) {
        cout << "Key: " << unique_key[i] << ", Value: ";
        for (int j = 1; j <= grouped_value[i][0]; ++j) {
            cout << grouped_value[i][j] << " ";
        }
        cout << endl;
    }
}

void reducer_function(char unique_key[][50], int grouped_value[][100], int unique_key_count) {
    cout << "\n--- Reduce Phase ---" << endl;
    for (int i = 0; i < unique_key_count; ++i) {
        int sum = 0;
        for (int j = 1; j <= grouped_value[i][0]; ++j) {
            sum += grouped_value[i][j];
        }
        cout << "Key: " << unique_key[i] << ", Aggregated Value: " << sum << endl;
    }
}

int main() {

    ifstream infile("numPipes.txt");

    int numPipes;
    infile >> numPipes;
    infile.close();
    remove("numPipes.txt");

    char data1[100][50];
    int value[100];
    int totalSize = 0;

    cout << "--- Reading from Pipes ---" << endl;
    for (int i = 0; i < numPipes; ++i) {
        string pipe_name = "pipe_" + to_string(i);
        int size;
        pipe_read(pipe_name, data1 + totalSize, value + totalSize, size);
        totalSize += size;
    }

    char unique_key[100][50];
    int grouped_value[100][100];
    int unique_key_count;
    
    shuffle_function(data1, value, totalSize, unique_key, grouped_value, unique_key_count);

    reducer_function(unique_key, grouped_value, unique_key_count);

    for (int i = 0; i < numPipes; ++i) {

        string pipe_name = "pipe_" + to_string(i);
        remove(pipe_name.c_str());

    }

    return 0;
}