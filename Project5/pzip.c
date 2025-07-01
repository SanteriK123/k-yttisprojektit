#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

// This holds the RLE, both the character and its count
typedef struct
{
    char ch;
    int count;
} RLEItem;

// This struct is used for all the data needed by each thread
typedef struct
{
    char *data;
    size_t start;
    size_t end;
    RLEItem *output;
    int outputSize;
} ThreadItem;

// This function compresses a chunk using the run-length encoding algorithm
void *rleCompress(void *arg)
{
    ThreadItem *compressItem = (ThreadItem *)arg;
    char *data = compressItem->data;
    size_t i = compressItem->start;
    size_t end = compressItem->end;

    // Allocate random capacity for the output array
    int capacity = 128;
    compressItem->output = malloc(sizeof(RLEItem) * capacity);
    compressItem->outputSize = 0;

    if (i >= end)
    {
        // if chunk is empty return
        return NULL;
    }

    char curr = data[i];
    int count = 1;

    // loop through the data chunk and do rle on  it
    for (i = i + 1; i < end; i++)
    {
        char next = data[i];
        if (next == curr)
        {
            // if next character is same as current, increase run length count
            count++;
        }
        else
        {
            if (compressItem->outputSize >= capacity)
            {
                // if output becomes full just double the capacity
                capacity *= 2;
                compressItem->output = realloc(compressItem->output, sizeof(RLEItem) * capacity);
            }
            // Store the current run in the output array
            compressItem->output[compressItem->outputSize].ch = curr;
            compressItem->output[compressItem->outputSize].count = count;
            compressItem->outputSize++;

            // go to the next character
            curr = next;
            count = 1;
        }
    }
    // handle the last item after loop
    if (compressItem->outputSize >= capacity)
    {
        // if output becomes full just double the capacity
        capacity *= 2;
        compressItem->output = realloc(compressItem->output, sizeof(RLEItem) * capacity);
    }
    compressItem->output[compressItem->outputSize].ch = curr;
    compressItem->output[compressItem->outputSize].count = count;
    compressItem->outputSize++;
    return NULL;
}

// used linux man pages for help here: https://man7.org/linux/man-pages/man2/mmap.2.html
// read all the given files into a single buffer using mmap for performance
char *readAllFiles(int argc, char *argv[], size_t *totalSize)
{
    size_t total = 0;
    char *buffer = NULL;

    // Loop through all the files provided as arguments
    for (int i = 1; i < argc; i++)
    {
        // Open the file for reading
        int fileDescriptor = open(argv[i], O_RDONLY);
        if (fileDescriptor == -1)
        {
            perror("open");
            exit(1);
        }

        struct stat fileStats;
        if (fstat(fileDescriptor, &fileStats) == -1)
        {
            perror("fstat");
            exit(1);
        }
        // mmap for fast file reading, as recommended by assignment
        char *fileData = mmap(NULL, fileStats.st_size, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);
        if (fileData == MAP_FAILED)
        {
            perror("mmap");
            exit(1);
        }

        // reallocate the buffer to hold new file size amount of data
        buffer = realloc(buffer, total + fileStats.st_size);
        // Copy the file data into the buffer
        memcpy(buffer + total, fileData, fileStats.st_size);
        // Update the total size of the buffer
        total += fileStats.st_size;

        // unmap the file data and close the file descriptor
        munmap(fileData, fileStats.st_size);
        close(fileDescriptor);
    }

    *totalSize = total;
    return buffer;
}

// This function merges the RLE results from multiple threads
void mergeChunks(ThreadItem *threadItem, int processors)
{

    // Find the first thread thta has output
    int first = -1;
    for (int i = 0; i < processors; i++)
    {
        if (threadItem[i].outputSize > 0)
        {
            first = i;
            break;
        }
    }
    if (first == -1)
    {
        return;
    }
    // Start with the first RLE item from the first thread
    RLEItem prev = threadItem[first].output[0];

    for (int i = first; i < processors; i++)
    {
        int startJ = 0;
        if (i == first)
        {
            startJ = 1;
        }
        for (int j = startJ; j < threadItem[i].outputSize; j++)
        {
            RLEItem curr = threadItem[i].output[j];
            if (curr.ch == prev.ch)
            {
                prev.count += curr.count;
            }
            else
            {
                fwrite(&prev.count, sizeof(int), 1, stdout);
                fwrite(&prev.ch, sizeof(char), 1, stdout);
                prev = curr;
            }
        }
    }
    // Output the last run after all threads are processed
    // This is because the last run might not have been output in the loop
    fwrite(&prev.count, sizeof(int), 1, stdout);
    fwrite(&prev.ch, sizeof(char), 1, stdout);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s file1 [file2 ...]\n", argv[0]);
        exit(1);
    }

    // Variable to hold the total size of all files read
    // size_t is used since file size can't be negative
    size_t totalSize;
    char *data = readAllFiles(argc, argv, &totalSize);

    // Get available processors and for each create a thread and a ThreadItem
    int processors = get_nprocs();
    pthread_t threads[processors];
    ThreadItem threadItem[processors];

    // Divide the data into chunks for each thread to process
    size_t chunkSize = totalSize / processors;

    // multithreaded RLE compression
    // used this for help: https://www.geeksforgeeks.org/thread-functions-in-c-c/
    for (int i = 0; i < processors; i++)
    {
        threadItem[i].data = data;
        threadItem[i].start = i * chunkSize;
        if (i == processors - 1)
        {
            // Last thread end is the total size
            threadItem[i].end = totalSize;
        }
        else
        {
            // Equally divide the data to threads
            threadItem[i].end = (i + 1) * chunkSize;
        }
        // Create a thread for each chunk and call rleCompress
        pthread_create(&threads[i], NULL, rleCompress, &threadItem[i]);
    }

    for (int i = 0; i < processors; i++)
    {
        // Wait for each thread to finish
        pthread_join(threads[i], NULL);
    }

    // Merge the results from the multiple threads
    // This is done single threaded
    mergeChunks(threadItem, processors);

    // Free any allocated memory
    for (int i = 0; i < processors; i++)
    {
        free(threadItem[i].output);
    }
    free(data);
    return 0;
}