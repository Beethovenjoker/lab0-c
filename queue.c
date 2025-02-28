#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *node = malloc(sizeof(struct list_head));

    if (!node)
        return NULL;
    INIT_LIST_HEAD(node);
    return node;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *entry = NULL, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        list_del(&entry->list);
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;

    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }

    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;

    element_t *node = list_entry(head->next, element_t, list);
    strncpy(sp, node->value, bufsize);
    list_del(&node->list);

    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;

    element_t *node = list_entry(head->prev, element_t, list);
    strncpy(sp, node->value, bufsize);
    list_del(&node->list);

    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}


/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head)
        return false;

    struct list_head *left = head->prev;
    struct list_head *right = head->next;
    while (left != right && left->prev != right) {
        left = left->prev;
        right = right->next;
    }
    list_del(right);
    q_release_element(container_of(right, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || head->next == head || head->next->next == head)
        return;

    struct list_head *first = head->next;
    struct list_head *second = first->next;
    while (first != head && second != head) {
        first->prev->next = second;
        second->next->prev = first;

        first->next = second->next;
        second->prev = first->prev;
        first->prev = second;
        second->next = first;

        first = first->next;
        second = first->next;
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
