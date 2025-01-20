#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
int is_whitespace(char c);
char* reverse_string(char *, int);
void print_words(char *, int);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    char *buffer_pointer = buff;
    char *input_pointer = user_str;
    int count = 0;
    int str_len = 0;

    while (*input_pointer != '\0' && count < len) {
        if (!is_whitespace(*input_pointer)) { // add all non whitespace characters to buffer
            *buffer_pointer++ = *input_pointer ;
            count++;
        } else if (count > 0 && *(buffer_pointer - 1) != ' ') { // at most one single space is added beween words
            *buffer_pointer++ = ' '; 
            count++;
        } // count is incremented every time a character is added to keep track of the curremt buffer size used
        input_pointer++;
    }

    if (*input_pointer != '\0') { // since the loop above stops at '\0' or when max buffer size is reached, this will only be true if the string is longer than BUFFER_SIZE
        return -1; 
    }

    str_len = buffer_pointer - buff; // calculates the length of string using memory pointers 

    while (count < len) { // fill the rest of the space with "."
        *buffer_pointer++ = '.'; 
        count++; 
    }

    return str_len;
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int word_counter = 0;
    int in_a_word = 0;

    for (int i = 0; i < str_len; i++){
        if (!is_whitespace(buff[i]) && !in_a_word){
            word_counter++;
            in_a_word = 1;
        } else if (is_whitespace(buff[i])){
            in_a_word = 0;
        }
    }
    return word_counter;
}

int is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n');
}

char* reverse_string(char *buff, int len){
    int i = 0;
    char *reversed_string = (char *)malloc(len + 1); // malloc a new string that fits the string exactly

    while (i < len && buff[i] != '\0') {
        reversed_string[i] = buff[len - 1 - i]; // copy the string in reverse
        i++;
    }
    reversed_string[i] = '\0';
    return reversed_string;
}

void print_words(char *buff, int str_len) {
    printf("Word Print\n");
    printf("----------\n");
    int word_count = 1;
    char *start_of_word = buff;
    char *end_of_word = buff;

    while (start_of_word < buff + str_len) {

        while (start_of_word < buff + str_len && is_whitespace(*start_of_word)) { // ignore leading whitespace
            start_of_word++;
        }
        end_of_word = start_of_word;

        while (end_of_word < buff + str_len && !is_whitespace(*end_of_word)) { // put the end_of_word pointer at the end of the current word
            end_of_word++;
        }

        if (start_of_word < end_of_word) {
            printf("%d. ", word_count++); // print word number
            for (char *ptr = start_of_word; ptr < end_of_word; ptr++) { //print everything betweeen the two pointers
                putchar(*ptr);
            }
            printf(" (%ld)\n", end_of_word - start_of_word); // end_of_word - start_of_word = length of word
        }
        start_of_word = end_of_word;
    }
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    /*
    This is safe because if argc is less than 2, then the program knows argv[1] does not exist and exits.
    If argc is greater than 2, then the program checks to see if argv[1] is a standard input option. If not, then it exits.
    */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    /*
    This statement checks if the argument count is less than 3. If it is, then it means it is missing an argument.
    There should be three arguments, program name, option, and string.
    */
    
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char *)malloc(BUFFER_SZ);
    if (buff == NULL){
        printf("Error allocating buffer, error");
        exit(99);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    // printf("User String Length: %d\n", user_str_len);
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        case 'r':
            char *reversed_string = reverse_string(buff, user_str_len);
            printf("Reversed String: %s\n", reversed_string);
            free(reversed_string); // a reverse streing was malloced in reverse_string function
            break;

        case 'w':
            print_words(buff, user_str_len);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
/*
If the functions only took the buffer pointer,
then the function would not know when to stop,
or we would have to set a predetermined size for the buffer in the function,
making the function less versatile.
Passing the length of the buffer and string as an argument, lets the functions work with string lengths and buffers of any size.
This makes the functions reusable and easier to maintain. It also helps manage overflowing/oversized buffers.
*/