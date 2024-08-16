#include <stdio.h>
#include <assert.h> // for assert()
#include <ctype.h> // for isspace()
// #include <math.h>  // added to test this file

typedef struct {
    int lineCount;
    int linesWithCodeCount;
    int cplusplusCommentCount;
    int cCommentCount;
    int cPreprocessorDirectiveCount;
    int foundCodeOnLine;
} CodeStats;

void codeStats_init(CodeStats *codeStats) {
    codeStats -> lineCount = 0;
    codeStats -> linesWithCodeCount = 0;
    codeStats -> cplusplusCommentCount = 0;
    codeStats -> cCommentCount = 0;
    codeStats -> cPreprocessorDirectiveCount = 0;
}

void codeStats_print(CodeStats codeStats, char *fileName) {
    printf("     file: %s\n", fileName);
    printf("     line count: %d\n", codeStats.lineCount);
    printf("     line with code: %d\n", codeStats.linesWithCodeCount);
    printf("     line with c plus plus comments: %d\n", codeStats.cplusplusCommentCount);
    printf("     line with c comments: %d\n", codeStats.cCommentCount);
    printf("     line with preprocesssor directive count: %d\n", codeStats.cPreprocessorDirectiveCount);
}

#define HANDLE_NEWLINE  \
    do {  \
        codeStats->lineCount++;  \
        codeStats->linesWithCodeCount += codeStats->foundCodeOnLine;  \
        codeStats->foundCodeOnLine = 0;  \
    } while (0)
/* random test to check c comments -2 */
void codeStats_accumulate(CodeStats *codeStats, char *fileName) {
    FILE *f = fopen(fileName, "r");
    int ch;
    enum {
        START,
        FOUND_SLASH,
        CPP_COMMENT,
        C_COMMENT,
        C_STAR,
        IN_STRING,
        ESC_STRING,
        IN_CHAR,
        ESC_CHAR,
        C_PREPROCESSOR,
        ESC_PROCESSOR,
    } state = START;

    assert(f);
    while ((ch = getc(f)) != EOF) {
        switch (state) {

        case START:
            if (ch == '\n') {
                HANDLE_NEWLINE;
            }
            if (ch == '/') {
                state = FOUND_SLASH;
            }
            if (ch == '\"') {
                state = IN_STRING;
            }
            if (ch == '#') {
                codeStats->cPreprocessorDirectiveCount++;
                state = C_PREPROCESSOR;
            }
            else if (!isspace(ch)) {
                codeStats->foundCodeOnLine = 1;
            }
            break;

        case FOUND_SLASH:
            switch(ch) {
            case '\n':
                codeStats->foundCodeOnLine = 1;
                HANDLE_NEWLINE;
                state = START;
                break;
            
            case '/':
                codeStats -> cplusplusCommentCount++;
                state = CPP_COMMENT;
                break;

            case '*':
                codeStats -> cCommentCount++;
                state = C_COMMENT;
                break;

            default:
                codeStats->foundCodeOnLine = 1;
                break;
            }
            break;

        case CPP_COMMENT:
            switch(ch) {
            case '\n':
                state = START;
                codeStats -> lineCount++;
                break;

            default:
                break;
            }
            break;

        case C_COMMENT:
            switch(ch) {
            case '\n':
                HANDLE_NEWLINE;
                break;

            case '*':
                codeStats->cCommentCount++;
                state = C_STAR;
                break;
            
            default:
                break;
            }
            break;

        case C_STAR:
            switch(ch) {
            case '\n':
                HANDLE_NEWLINE;
                break;

            case '/':
                codeStats->foundCodeOnLine = 1;
                state = START;
                 break;

            default:
                state = C_COMMENT;
                break;
            }
            break;

        case IN_STRING:
            switch(ch) {
            case '\\':
                state = ESC_STRING;                
                break;

            case '"':
                state = START;
                break;

            default:
                break;
            }
            break;

        case ESC_STRING:
            switch(ch) {
            case '\n':
                HANDLE_NEWLINE;
                codeStats->foundCodeOnLine = 1;
                state = IN_STRING;
                break;

            default:
                state = IN_STRING;
                break;
            }
            break;

        case IN_CHAR:
            switch(ch) {
                case '\\':
                    state = ESC_CHAR;                
                    break;

                case '\'':
                    state = START;                
                    break;


                default:
                    break;
            }
            break;

        case ESC_CHAR:
            switch(ch) {

            case '\n':
                HANDLE_NEWLINE;
                codeStats->foundCodeOnLine = 1;
                state = IN_CHAR;
                break;

            default:
                state = IN_CHAR;
                break;
            }
            break;

        case C_PREPROCESSOR:
            switch(ch) {
                case '\n':
                    codeStats->lineCount++;
                    state = START;
                    break;

                case '\\':
                    state = ESC_PROCESSOR;                
                    break;
                
                default:
                    break;
            }
            break;
        
        case ESC_PROCESSOR:
            switch(ch) {
                case '\n':
                    codeStats->lineCount++;
                    state = C_PREPROCESSOR;
                    break;
            
            default:
                break;
            }
            break;

        default:
            assert(0);
            break;
        }
    }
    fclose(f);
    assert(state == START);
}

int main(int argc, char *argv[])
{
    CodeStats codeStats;
    int i;

    for (i = 1; i < argc; i++) {
        codeStats_init(&codeStats);
        codeStats_accumulate(&codeStats, argv[i]);
        codeStats_print(codeStats, argv[i]); // no "&" -- see why?
        if (i != argc-1)   // if this wasn't the last file ...
            printf("\n");  // ... print out a separating newline
    }
    return 0;
}
// hehe test. this is comment #6
/* test - 4 */
