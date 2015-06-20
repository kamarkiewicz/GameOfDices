#include <stdio.h>
#include "../src/DryEngine/List.h"

void list (drList *self)
{
    drListIter it = drList_begin (self);

    while (!drListIter_equal (&it, drList_end (self))) {
        assert (it.curr != NULL);
        printf ("%d ", *((int*) drListIter_get (it)));
        drListIter_next (&it);
    }

    printf("\n");
}

int cmp (int *x, int *y){
    if (*x < *y) return -1;
    if (*x > *y) return 1;
    return 0;
}

int main(int argc, char *argv[])
{
	drList *var = drList_create (NULL);

	int t[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (int i = 1; i <= 10; i++)
        drList_insert (var, &t[i], i < 6);

    list (var); // 10 9 8 7 6 1 2 3 4 5

    //drListIter it = drList_begin (var);
    for (int i = 1; i <= 4; i++){
        assert (!drList_empty (var));
        drList_delete (var, i < 3);
    }

    list (var); // 8 7 6 1 2 3

    drList_sort (var, COMPARATOR(cmp));
    list (var); // 1 2 3 6 7 8


    //drListIter it = drList_begin ();

    drList_destroy (var);
}
