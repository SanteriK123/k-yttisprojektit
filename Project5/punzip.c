#include <stdio.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <stdlib.h>

typedef struct
{
    const char *filename;
    char *output;
    size_t outputSize;
} UnzipItem;

void *unzipFile(void *arg)
{
    UnzipItem *item = (UnzipItem *)arg;

    // open file in binary read mode
    FILE *fp = fopen(item->filename, "rb");
    if (!fp)
    {
        perror("fopen");
        return NULL;
    }

    // Allocate some random space for output, realloc later if needed
    size_t alloc = 1024;
    item->output = malloc(1024);
    item->outputSize = 0;

    int count;
    char ch;

    // read 4 byte integer count to variable count
    while (fread(&count, sizeof(int), 1, fp) == 1)
    {
        // read the next character (1 byte) into ch
        if (fread(&ch, sizeof(char), 1, fp) != 1)
        {
            // if reading character fails, assume file is corrupted or wrong format and print error
            fprintf(stderr, "punzip: file format error in %s\n", item->filename);
            fclose(fp);
            return NULL;
        }
        // Ensure enough space for count, and realloc if not
        if (item->outputSize + count > alloc)
        {
            alloc = (item->outputSize + count) * 2;
            item->output = realloc(item->output, alloc);
        }
        // Write the character ch count times to output (this is the unzipping)
        for (int i = 0; i < count; i++)
        {
            item->output[item->outputSize++] = ch;
        }
    }
    fclose(fp);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "punzip: file1.z [file2.z ...]\n");
        exit(1);
    }

    // check args for amount of given files
    int filesAmount = argc - 1;
    pthread_t threads[filesAmount];
    UnzipItem items[filesAmount];

    // Create a thread for each file
    for (int i = 0; i < filesAmount; i++)
    {
        items[i].filename = argv[i + 1];
        items[i].output = NULL;
        items[i].outputSize = 0;
        pthread_create(&threads[i], NULL, unzipFile, &items[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < filesAmount; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Output unzipped data in file order
    for (int i = 0; i < filesAmount; i++)
    {
        fwrite(items[i].output, 1, items[i].outputSize, stdout);
        free(items[i].output);
    }
    return 0;
}