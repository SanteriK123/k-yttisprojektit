#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Node
{
    char *line;
    struct Node *next;
} Node;

void freeMemory(Node *head)
{
    /* Simple function to free all dynamically allocated memory,
    including the line */
    Node *curr = head;
    while (curr != NULL)
    {
        Node *temp = curr->next;
        free(curr->line);
        free(curr);
        curr = temp;
    }
}

bool checkIfSameFile(char *path1, char *path2)
{
    /* Function to check for situation where the file names are different,
    but they're in reality hardlinked to the same file */
    /* Code is from: https://stackoverflow.com/a/15717682 but I adjusted it
    to work better for the needs of this assignment */
    FILE *f1 = fopen(path1, "r");
    if (!f1)
    {
        fprintf(stderr, "reverse: cannot open file '%s'\n", path1);
        exit(1);
    }

    FILE *f2 = fopen(path2, "w");
    if (!f2)
    {
        fprintf(stderr, "reverse: cannot open file '%s'\n", path2);
        fclose(f1);
        exit(1);
    }

    bool sameFile = true;
    int c1, c2;

    /* This loop simply compares each byte of both files, and if there's
    a difference, it breaks and returns samefile as false */
    while ((c1 = getc(f1)) != EOF && (c2 = getc(f2)) != EOF)
    {
        if (c1 != c2)
        {
            sameFile = false;
            break;
        }
    }
    if (getc(f1) != EOF || getc(f2) != EOF)
        sameFile = false;
    fclose(f1);
    fclose(f2);
    return sameFile;
}

void printLinkedList(FILE *out, Node *head)
{
    Node *temp = head;
    while (temp != NULL)
    {
        /* Check for newline, if there's no newline then add it */
        if (temp->line[strlen(temp->line) - 1] == '\n')
        {
            fprintf(out, "%s", temp->line);
        }
        else
        {
            fprintf(out, "%s\n", temp->line);
        }
        temp = temp->next;
    }
}

int main(int argc, char *argv[])
{
    /* I will implement this program with a linked list
    as its easy to dynamically allocate size for the data
    and we can insert data at the beginning instead of the end,
    which will effectively print the input in reverse when we you
    traverse the linked list from start to end */

    /* Some variables that will be used for the whole program */
    Node *head = NULL;
    FILE *input = stdin, *output = stdout;
    char *buffer = NULL;
    size_t size = 0;

    /* Handle more than 2 arguments */
    if (argc > 3)
    {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    /* Handle first argument */
    if (argc > 1)
    {
        if ((input = fopen(argv[1], "r")) == NULL)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }
    /* Handle second argument */
    if (argc > 2)
    {
        if (strcmp(argv[1], argv[2]) == 0)
        {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
        if (checkIfSameFile(argv[1], argv[2]))
        {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
        if ((output = fopen(argv[2], "w")) == NULL)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
            exit(1);
        }
    }

    /* Use getline to read a line from stream and then add 
    the buffer to the beginning of the linked list */
    while (getline(&buffer, &size, input) != -1)
    {
        Node *newNode = malloc(sizeof(Node));
        if (!newNode)
        {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
        /* Duplicate buffer and add it to the new node */
        newNode->line = strdup(buffer);
        if (!newNode->line)
        {
            fprintf(stderr, "malloc failed\n");
            free(newNode);
            exit(1);
        }
        /* Insert new node at the beginning, make new node the head */
        newNode->next = head;
        head = newNode;
    }
    /* Free the buffer and then use fprintf to output the data in reverse */
    free(buffer);
    printLinkedList(output, head);

    /* Close files and free dynamically allocated memory */
    fclose(input);
    fclose(output);
    freeMemory(head);
    return (0);
}