#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
#include <pthread.h>
#include <fstream>
using namespace std;

struct thread_data {
    int thread_id;
    char chunk[1024];
};

pthread_mutex_t mutexLock = PTHREAD_MUTEX_INITIALIZER;

void make_chunks_function(const string &text, int chunk_number, char chunks[][1024]) {

    int wordCount = 0;
    char temp[1024];

    strcpy(temp, text.c_str());

    char *words[1000];

    char *temp2 = strtok(temp, " ");

    while (temp2 != nullptr) {

        words[wordCount++] = temp2;
        temp2 = strtok(nullptr, " ");

    }

    int size = wordCount / chunk_number;
    int rem = wordCount % chunk_number;
    int index = 0;

    for (int i = 0; i < chunk_number; ++i) {

        chunks[i][0] = '\0';
        int chunkSize = size + (i < rem ? 1 : 0);

        for (int j = 0; j < chunkSize; ++j) {
            strcat(chunks[i], words[index++]);
            strcat(chunks[i], " ");
        }

        int len = strlen(chunks[i]);
        if (len > 0 && chunks[i][len - 1] == ' ') {
            chunks[i][len - 1] = '\0';
        }

    }
}

void *thread_mapper_function(void *arg) {

    thread_data *data = (thread_data *)arg;
    int thread_id = data->thread_id;
    char *chunk = data->chunk;
    
    pthread_mutex_lock(&mutexLock);
    
    cout << "Mapper processing chunk: " << chunk << endl;
    
    pthread_mutex_unlock(&mutexLock);
    
    string pipe_name = "pipe_" + to_string(thread_id);
    
    if (mkfifo(pipe_name.c_str(), 0666) == -1) {
        cerr << "Error: Failed to create pipe " << pipe_name << endl;
        pthread_exit(nullptr);
    }
    
    int pipeFd = open(pipe_name.c_str(), O_WRONLY);
    if (pipeFd < 0) {
        cerr << "Error: Failed to open pipe " << pipe_name << " for writing" << endl;
        pthread_exit(nullptr);
    }
    
    if (thread_id == 0) {
        string chunk_message_size = to_string(thread_id) + "\n";
        write(pipeFd, chunk_message_size.c_str(), chunk_message_size.size());
    }
    
    char word[50];
    int i = 0, j = 0;
    while (chunk[i] != '\0') {
        if (chunk[i] == ' ' || chunk[i] == '\n') {
            if (j > 0) {
                word[j] = '\0';
                string output = string(word) + " 1\n";
                pthread_mutex_lock(&mutexLock);
                cout << "Writing to pipe " << pipe_name << ": " << output;
                write(pipeFd, output.c_str(), output.size());
                pthread_mutex_unlock(&mutexLock);
                j = 0;
            }
        } else {
            word[j++] = chunk[i];
        }
        ++i;
    }
    if (j > 0) {
        word[j] = '\0';
        string output = string(word) + " 1\n";
        pthread_mutex_lock(&mutexLock);
        cout << "Writing to pipe " << pipe_name << ": " << output;
        write(pipeFd, output.c_str(), output.size());
        pthread_mutex_unlock(&mutexLock);
    }
    close(pipeFd);
    pthread_exit(nullptr);
}

int main() {

    pthread_mutex_init(&mutexLock, nullptr);
    
    string input;
    int chunk_number;
    
    cout << "Enter the input text: ";
    getline(cin, input);
    
    cout << "Enter the number of chunks: ";
    cin >> chunk_number;
    
    cin.ignore();


    ofstream outfile("numPipes.txt");
    outfile << chunk_number << endl;
    outfile.close();

    
    char chunks[chunk_number][1024];
    make_chunks_function(input, chunk_number, chunks);
    
    for (int i = 0; i < chunk_number; ++i) {
        cout << "Chunk " << i << ": [" << chunks[i] << "]" << endl;
    }
    
    pthread_t threads[chunk_number];
    thread_data thread_data[chunk_number];
    
    for (int i = 0; i < chunk_number; ++i) {
    
        thread_data[i].thread_id = i;
        strcpy(thread_data[i].chunk, chunks[i]);
        
        pthread_create(&threads[i], nullptr, thread_mapper_function, (void *)&thread_data[i]);
    }
    
    for (int i = 0; i < chunk_number; ++i) {
        pthread_join(threads[i], nullptr);
    }
    pthread_mutex_destroy(&mutexLock);
    return 0;
}