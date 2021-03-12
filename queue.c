#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))

/*
 * node used to store lists of list
 */
typedef struct _node {
    struct _node *next;
    list_ele_t *value;
} node;

// raw memory pool for node
node nodepool[300000];
// allocator for the pool, will increment each time
node *allocptr = nodepool;

#define nodealloc() (allocptr++)

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL)
        return NULL;

    q->tail = q->head = NULL;
    q->size = 0;
    return q;
}

/*
 * Free one node, and it's element
 * return next node
 */
list_ele_t *free_ele(list_ele_t *node)
{
    list_ele_t *next = node->next;
    free(node->value);
    free(node);
    return next;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q == NULL)
        return;

    list_ele_t *node = q->head;
    while (node) {
        node = free_ele(node);
    }
    free(q);
}

/*
 * a strcpy
 */
char *strcopy(char *destination, const char *source)
{
    // return if no memory is allocated to the destination
    if (destination == NULL) {
        return NULL;
    }
    // take a pointer pointing to the beginning of destination string
    char *ptr = destination;

    // copy the C-string pointed by source into the array
    // pointed by destination
    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }

    // include the terminating null character
    *destination = '\0';
    return ptr;
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (q == NULL)
        return false;

    list_ele_t *newh;
    // allocate the buffer of list node
    newh = malloc(sizeof(list_ele_t));
    // check if it's successful allocated
    if (newh == NULL) {
        return false;
    }
    // check s isNULL
    if (s == NULL) {
        newh->value = NULL;
    } else {
        // malloc a string buffer
        newh->value = malloc(strlen(s) + 1);
        // check if it's successful allocated
        if (newh->value == NULL) {
            free(newh);
            return false;
        }
        // copy string
        strcopy(newh->value, s);
    }

    newh->next = q->head;
    q->head = newh;
    if (q->size == 0) {
        q->tail = newh;
    }
    ++q->size;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (q == NULL) {
        return false;
    }
    // check s isNULL
    if (s == NULL) {
        return false;
    }
    list_ele_t *newt;
    // allocate the buffer of list node
    newt = malloc(sizeof(list_ele_t));
    // check if it's successful allocated
    if (newt == NULL) {
        return false;
    }
    // malloc a string buffer
    newt->value = malloc(strlen(s) + 1);
    // check if it's successful allocated
    if (newt->value == NULL) {
        free(newt);
        return false;
    }
    // copy string
    strcopy(newt->value, s);
    newt->next = NULL;
    // insert to the Queue's tail
    if (q->size == 0) {
        q->tail = q->head = newt;
    } else {
        q->tail = q->tail->next = newt;
    }
    ++q->size;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (q == NULL || q->size == 0)
        return false;

    // to copy string
    int len = min(bufsize - 1, strlen(q->head->value));
    // sp = malloc(len + 1);
    if (sp == NULL) {
        return false;
    }
    strncpy(sp, q->head->value, len + 1);
    sp[len] = '\0';
    if (q->size == 1) {
        free_ele(q->head);
        q->head = q->tail = NULL;
        q->size = 0;
        return true;
    }
    list_ele_t *to_free = q->head;
    q->head = q->head->next;
    // free the node
    free_ele(to_free);
    --q->size;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q == NULL)
        return 0;
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (q == NULL || q->size == 0 || q->size == 1)
        return;

    // swap head and tail
    list_ele_t *tmp = q->head;
    q->head = q->tail;
    q->tail = tmp;
    // tmp nodes
    list_ele_t *fir = q->tail, *sec = q->tail->next, *thr = q->tail->next->next;
    fir->next = NULL;
    // reverse sec's next pointer
    while (thr) {
        sec->next = fir;
        tmp = sec;
        sec = thr;
        thr = thr->next;
        fir = tmp;
    }
    sec->next = fir;
}

void print_list(list_ele_t *l)
{
    printf("[ ");
    while (l) {
        printf("%s ", l->value);
        l = l->next;
    }
    printf("]\n");
}

void print_tim(node *nodes)
{
    printf("nodes : \n");
    while (nodes) {
        print_list(nodes->value);
        nodes = nodes->next;
    }
}

/*
 * Reverse elements in a list
 * return the new head (original tail)
 */
list_ele_t *reverse(list_ele_t *l)
{
    if (l == NULL || l->next == NULL)
        return l;

    list_ele_t *fir = l, *sec = l->next, *thr = l->next->next, *tmp;
    fir->next = NULL;
    // reverse sec's next pointer
    while (thr) {
        sec->next = fir;
        tmp = sec;
        sec = thr;
        thr = thr->next;
        fir = tmp;
    }
    sec->next = fir;

    // print_list(sec);
    return sec;
}

/*
 * merge 2 list with ascending order
 * return the head of the merged list
 */
list_ele_t *merge_2list(list_ele_t *l1, list_ele_t *l2)
{
    list_ele_t *ret = NULL, **slot = &ret;

    while (l1 && l2) {
        if (strcmp(l1->value, l2->value) < 0) {
            *slot = l1;
            slot = &l1->next;
            l1 = l1->next;
        } else {
            *slot = l2;
            slot = &l2->next;
            l2 = l2->next;
        }
    }
    if (l1)
        *slot = l1;
    else
        *slot = l2;

    return ret;
}

/*
 * merge sort.
 * return the head of sorted list
 */
list_ele_t *merge_sort(list_ele_t *l, int size)
{
    if (size <= 1)
        return l;

    int size1 = size / 2, size2 = size - size1;  // size of left and right list
    list_ele_t *tmp = l;
    // split list into size1, size2
    for (int i = 0; i < size1 - 1; ++i) {
        tmp = tmp->next;
    }
    list_ele_t *right_head = tmp->next;
    tmp->next = NULL;

    return merge_2list(merge_sort(l, size1), merge_sort(right_head, size2));
}

/*
 * to buttom up to merge all the lists into one
 * by divide and conquer to reduce complexity
 * return the final merged listnode
 */
list_ele_t *tim_merge(node *nodes)
{
    node *cur = nodes, *next = cur->next, **backslot = &cur;
    // find backslot, it will point to the last NULL position
    while (*backslot)
        backslot = &(*backslot)->next;

    // merge the head of two
    while (next) {
        list_ele_t *mer = merge_2list(cur->value, next->value);

        // push back the new value
        *backslot = nodealloc();
        (*backslot)->value = mer;
        backslot = &(*backslot)->next;
        *backslot = NULL;

        // update
        cur = next->next;  // the last must have a thing, because backslot push
                           // at back
        next = cur->next;
    }

    return cur->value;
}

/*
 * tim sort.
 * normal tim sort without threshold
 * go up, then go down, then go up ...
 */
list_ele_t *tim_sort(list_ele_t *cur, int size)
{
    node *nodes = NULL, **slot = &nodes;
    bool goup = true;
    // split list into up, down sublists
    list_ele_t *next = cur->next, *elehead = cur;
    while (next) {
        bool cmp = strcmp(cur->value, next->value) < 0;
        if (goup) {
            if (cmp) {  // continue to go up
                cur = cur->next;
            } else {  // exchange
                cur->next = NULL;
                cur = next;
                node *newnode = *slot = nodealloc();
                newnode->value = elehead;
                elehead = next;
                slot = &(newnode->next);
                goup = goup ? false : true;
            }
        } else {         // now does down
            if (!cmp) {  // continue to go down
                cur = cur->next;
            } else {  // exchange
                cur->next = NULL;
                cur = next;
                node *newnode = *slot = nodealloc();
                newnode->value = reverse(elehead);
                elehead = next;
                slot = &(newnode->next);
                goup = goup ? false : true;
            }
        }
        next = next->next;
    }
    // last one
    node *newnode = *slot = nodealloc();
    newnode->value = goup ? elehead : reverse(elehead);
    slot = &(*slot)->next;
    *slot = NULL;  // let nodes to NULL terminate
    // merge nodes
    return tim_merge(nodes);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (q == NULL || q->size == 0 || q->size == 1)
        return;

    q->head = tim_sort(q->head, q->size);
    // find tail
    list_ele_t *tmp = q->head;
    while (tmp->next) {
        tmp = tmp->next;
    }
    q->tail = tmp;

    // print_tim, to let it used
    print_tim(NULL);
}
