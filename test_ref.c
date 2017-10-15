#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tst.h"

/** constants insert, delete, max word(s) & stack nodes */
enum { INS, DEL, WRDMAX = 256, STKMAX = 512, LMAX = 1024 };
#define REF INS
#define CPY DEL

/* timing helper function */
static double tvgetf(void)
{
    struct timespec ts;
    double sec;

    clock_gettime(CLOCK_REALTIME, &ts);
    sec = ts.tv_nsec;
    sec /= 1e9;
    sec += ts.tv_sec;

    return sec;
}

/* simple trim '\n' from end of buffer filled by fgets */
static void rmcrlf(char *s)
{
    size_t len = strlen(s);
    if (len && s[len - 1] == '\n')
        s[--len] = 0;
}

#define IN_FILE "cities.txt"
#define SHUF "cities1.txt"
#define SORT "cities2.txt"


int main(int argc, char **argv)
{
    char word[WRDMAX] = "";
    char *sgl[LMAX] = {NULL};
    tst_node *root = NULL, *res = NULL, *root_1 = NULL, *root_2 = NULL;
    int rtn = 0, idx = 0, sidx = 0, tmp = 0;
    FILE *fp = fopen(IN_FILE, "r");
    FILE *output = fopen("output.txt", "w");
    double t1, t2;

    //origin
    if (!fp) { /* prompt, open, validate file for reading */
        fprintf(stderr, "error: file open failed '%s'.\n", argv[1]);
        exit(0);
    }

    t1 = tvgetf();
    while ((rtn = fscanf(fp, "%s", word)) != EOF) {
        char *p = word;
        /* FIXME: insert reference to each string */
        if (!tst_ins_del(&root, &p, INS, CPY)) {
            fprintf(stderr, "error: memory exhausted, tst_insert.\n");
            fclose(fp);
            exit(0);
        }
        idx++;
    }
    t2 = tvgetf();

    fclose(fp);
    printf("ternary_tree, loaded %d words in %.6f sec\n", idx, t2 - t1);
    fprintf(output,"origin %lf\n",t2 - t1);

    //SHUF
    fp = fopen(SHUF, "r");
    if (!fp) { /* prompt, open, validate file for reading */
        fprintf(stderr, "error: file open failed '%s'.\n", argv[1]);
        exit(0);
    }

    t1 = tvgetf();
    while ((rtn = fscanf(fp, "%s", word)) != EOF) {
        char *p = word;
        /* FIXME: insert reference to each string */
        if (!tst_ins_del(&root_1, &p, INS, CPY)) {
            fprintf(stderr, "error: memory exhausted, tst_insert.\n");
            fclose(fp);
            exit(0);
        }
        tmp++;
    }
    t2 = tvgetf();

    fclose(fp);
    printf("ternary_tree1, loaded %d words in %.6f sec\n", idx, t2 - t1);
    fprintf(output,"shuffle %lf\n",t2 - t1);
    tmp = 0;

    //SORT
    fp = fopen(SORT, "r");
    if (!fp) { /* prompt, open, validate file for reading */
        fprintf(stderr, "error: file open failed '%s'.\n", argv[1]);
        exit(0);
    }

    t1 = tvgetf();
    while ((rtn = fscanf(fp, "%s", word)) != EOF) {
        char *p = word;
        /* FIXME: insert reference to each string */
        if (!tst_ins_del(&root_2, &p, INS, CPY)) {
            fprintf(stderr, "error: memory exhausted, tst_insert.\n");
            fclose(fp);
            exit(0);
        }
        tmp++;
    }
    t2 = tvgetf();

    fclose(fp);
    printf("ternary_tree2, loaded %d words in %.6f sec\n", idx, t2 - t1);
    fprintf(output,"sort %lf\n",t2 - t1);

    fclose(output);

    for (;;) {
        printf(
            "\nCommands:\n"
            " a  add word to the tree\n"
            " f  find word in tree\n"
            " s  search words matching prefix\n"
            " d  delete word from the tree\n"
            " q  quit, freeing all data\n\n"
            "choice: ");
        fgets(word, sizeof word, stdin);

        switch (*word) {
            char *p = NULL;
        case 'a':
            printf("enter word to add: ");
            if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(word);
            p = word;
            t1 = tvgetf();
            /* FIXME: insert reference to each string */
            res = tst_ins_del(&root, &p, INS, CPY);
            t2 = tvgetf();
            if (res) {
                idx++;
                printf("  %s - inserted in %.6f sec. (%d words in tree)\n",
                       (char *) res, t2 - t1, idx);
            } else
                printf("  %s - already exists in list.\n", (char *) res);
            break;
        case 'f':
            printf("find word in tree: ");
            if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(word);
            t1 = tvgetf();
            res = tst_search(root, word);
            t2 = tvgetf();
            if (res)
                printf("  found %s in %.6f sec.\n", (char *) res, t2 - t1);
            else
                printf("  %s not found.\n", word);
            break;
        case 's':
            printf("find words matching prefix (at least 1 char): ");
            if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(word);
            t1 = tvgetf();
            res = tst_search_prefix(root, word, sgl, &sidx, LMAX);
            t2 = tvgetf();
            if (res) {
                printf("  %s - searched prefix in %.6f sec\n\n", word, t2 - t1);
                for (int i = 0; i < sidx; i++)
                    printf("suggest[%d] : %s\n", i, sgl[i]);
            } else
                printf("  %s - not found\n", word);
            break;
        case 'd':
            printf("enter word to del: ");
            if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(word);
            p = word;
            printf("  deleting %s\n", word);
            t1 = tvgetf();
            /* FIXME: remove reference to each string */
            res = tst_ins_del(&root, &p, DEL, CPY);
            t2 = tvgetf();
            if (res)
                printf("  delete failed.\n");
            else {
                printf("  deleted %s in %.6f sec\n", word, t2 - t1);
                idx--;
            }
            break;
        case 'q':
            tst_free_all(root);
            tst_free_all(root_1);
            tst_free_all(root_2);
            return 0;
            break;
        default:
            fprintf(stderr, "error: invalid selection.\n");
            break;
        }
    }

    return 0;
}
