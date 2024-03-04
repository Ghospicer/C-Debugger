#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct buf{
    char line[80]; // a program line
    int link; // link to the next line
};
struct buf Buffer[100]; // max 100 lines
int head; //head node

void display_buffer();
int posY = 0;

char filename[20];
void read_to_buffer() {
    FILE *file;
    char line[80];

    // Open the file in read mode
    file = fopen(filename, "r");

    head = 0;

    // Read each line from the file and store it in Buffer
    int i = 0;
    while (fgets(line, sizeof(line), file) != NULL && i < 100) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0'; // Replace newline with null terminator if present
        }
        strcpy(Buffer[i].line, line); // Copy each line to the buffer
        Buffer[i-1].link = i; //link the last node to the previous one
        Buffer[i++].link = -1; //set the link of the last node
    }

    fclose(file);
}

int dir_count = 0;
void add_to_buffer(){
    char line[80]; // Buffer to store the input string
    int index = 0; // Index to keep track of characters in the input buffer
    
    clear();
    printw("Enter @var variable_name or @trace variable_name: ");
    refresh();

    int ch;
    while ((ch = getch()) != '\n' && index < 79) {
        line[index++] = ch; // Store the character in the buffer
        refresh(); // Refresh the screen to show the character immediately
    }
    line[index] = '\0';
    strcpy(Buffer[99-dir_count].line, line);
    int node_to_add = head;
    for(int i = 0; i < posY; i++){
        node_to_add = Buffer[node_to_add].link;
    }
    Buffer[99-dir_count].link = Buffer[node_to_add].link;
    Buffer[node_to_add].link = 99-dir_count++;
    display_buffer(++posY);
}

void write_to_file() {
    FILE *file;
    file = fopen(filename, "w"); // Open the file in write mode

    if (file == NULL) {
        return;
    }

    int i = head;

    while (Buffer[i].link != -1) {
        fprintf(file, "%s\n", Buffer[i].line); // Write each line from the buffer to the file
        i = Buffer[i].link;
    }
    fprintf(file, "%s\n", Buffer[i].line); // Write the last line

    fclose(file); // Close the file
}

void display_buffer() {
    clear(); // Clear the screen before displaying the buffer
    
    int i = head;
    while (Buffer[i].link != -1) {
        printw("%s\n", Buffer[i].line); // Print each line from the buffer
        i = Buffer[i].link;
    }
    printw("%s\n", Buffer[i].line); // for the last line
    
    wmove(stdscr, posY, 0);

    refresh(); // Refresh the screen after printing
}

int main()
{
    strcpy(filename, "myprog.c");
    read_to_buffer();
    int ch;
    initscr();
    refresh();
    while ((ch = getch()) != 'x'){
        if (ch == 'd'){
            display_buffer(posY);
        }
        if (ch == 'w') {
            display_buffer(posY);
            if(posY > 0){
                wmove(stdscr, --posY, 0);
            }
        }
        if (ch=='s'){
            display_buffer(posY);
            int line_count = 0;
            int i = head;
            while (Buffer[i].link != -1) {
                i = Buffer[i].link;
                line_count++;
            }
            if(posY < line_count){
                wmove(stdscr, ++posY, 0);
            }
        }
        if (ch=='i'){
            add_to_buffer(posY);
        }
    }
    endwin();
    strcpy(filename, "temp.c");
    write_to_file();
    read_to_buffer();
    
    char Tr_var[20]; // assume that each variable name is 1 character
    int nTrvar = 0; // number of variables in the array
    
    int i = head;
    int assignment_count = 0;
    while (Buffer[i].link != -1) {
        char *line = Buffer[i].line;

        // Check for @var and replace with add_TT(variable_name);
        char *var_occurrence = strstr(line, "@var");
        if (var_occurrence != NULL) {
            char *var_name = var_occurrence + strlen("@var");
            while (*var_name == ' ') {
                var_name++; // Skip any leading spaces
            }
            Tr_var[nTrvar++] = *var_name;
            sprintf(var_occurrence, "add_TT(\'%c\');", *var_name);
        }

        // Check for @trace and replace with display_TT()
        char *trace_occurrence = strstr(line, "@trace");
        if (trace_occurrence != NULL) {
            char *trace_name = trace_occurrence + strlen("@trace");
            while (*trace_name == ' ') {
                trace_name++; // Skip any leading spaces
            }
            sprintf(trace_occurrence, "display_TT(\'%c\');", *trace_name);
        }

        // Check for an assignment statement and add update_TT()
        char *assignment = strstr(line, "=");
        if (assignment != NULL) {
            for(int j = 0; j < nTrvar; j++){
                if(Tr_var[j] == *(assignment-1)){
                    char line[80];
                    sprintf(line, "update_TT(\'%c\', %c);", Tr_var[j], Tr_var[j]);
                    strcpy(Buffer[99-assignment_count].line, line);
                    Buffer[99-assignment_count].link = Buffer[i].link;
                    Buffer[i].link = 99-assignment_count++;
                    break;
                }
            }
        }
        i = Buffer[i].link;
    }
    strcpy(filename, "expanded.c");
    write_to_file();
    return 0;
}
