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
    if (!head || list_empty(head))
        return NULL;

    element_t *node = list_entry(head->next, element_t, list);
    if (sp && bufsize > 0) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&node->list);

    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove_head(head->prev->prev, sp, bufsize);
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
    if (!head || list_empty(head))
        return false;

    bool del = false;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        element_t *cur = list_entry(node, element_t, list);
        if (safe != head &&
            strcmp(cur->value, list_entry(safe, element_t, list)->value) == 0) {
            del = true;
            list_del(node);
            q_release_element(cur);
        } else if (del) {
            del = false;
            list_del(node);
            q_release_element(cur);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || head->next == head || head->next->next == head)
        return;

    struct list_head *cur = head->next;
    struct list_head *tmp = NULL;
    while (cur->next != head) {
        tmp = cur->next;
        list_move(tmp, cur->prev);
        if (cur != head && cur->next != head)
            cur = cur->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k < 2)
        return;

    struct list_head *node = head->next;
    struct list_head *group_head = NULL;
    int counts = 0;

    while (node != head) {
        counts++;

        if (counts == 1)
            group_head = node;
        node = node->next;
        if (counts == k) {
            struct list_head *group_tail = node->prev;
            struct list_head *prev_group = group_head->prev;
            struct list_head *next_group = node;
            struct list_head group;
            INIT_LIST_HEAD(&group);
            list_cut_position(&group, prev_group, group_tail);
            q_reverse(&group);
            list_splice(&group, prev_group);
            counts = 0;
            node = next_group;
        }
    }
}

void merge(struct list_head *left, struct list_head *right, bool descend)
{
    struct list_head dummy;
    INIT_LIST_HEAD(&dummy);
    const element_t *tmp1 = NULL, *tmp2 = NULL;
    while (!list_empty(left) && !list_empty(right)) {
        tmp1 = list_first_entry(left, element_t, list);
        tmp2 = list_first_entry(right, element_t, list);
        bool useLeft = (descend ^ (strcmp(tmp1->value, tmp2->value) < 0));

        if (useLeft) {
            list_move_tail(left->next, &dummy);
        } else {
            list_move_tail(right->next, &dummy);
        }
    }

    if (list_empty(left)) {
        list_splice_tail_init(right, &dummy);
    } else {
        list_splice_tail_init(left, &dummy);
    }
    list_splice_tail_init(&dummy, left);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || head->next->next == head)
        return;

    struct list_head *slow = head;
    struct list_head *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    struct list_head *mid = slow;
    struct list_head left;
    list_cut_position(&left, head, mid);

    q_sort(&left, descend);
    q_sort(head, descend);
    merge(head, &left, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int counts = 1;

    struct list_head *node = head->prev;
    element_t *entry = list_entry(node, element_t, list);
    const char *min = entry->value;

    node = node->prev;
    while (node != head) {
        entry = list_entry(node, element_t, list);
        struct list_head *tmp = node->prev;

        if (strcmp(entry->value, min) > 0) {
            list_del(node);
            q_release_element(entry);
        } else {
            if (strcmp(entry->value, min) < 0)
                min = entry->value;
            counts++;
        }
        node = tmp;
    }
    return counts;
}


/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of i */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int counts = 1;

    struct list_head *node = head->prev;
    element_t *entry = list_entry(node, element_t, list);
    const char *max = entry->value;

    node = node->prev;
    while (node != head) {
        entry = list_entry(node, element_t, list);
        struct list_head *tmp = node->prev;

        if (strcmp(entry->value, max) < 0) {
            list_del(node);
            q_release_element(entry);
        } else {
            if (strcmp(entry->value, max) > 0)
                max = entry->value;
            counts++;
        }
        node = tmp;
    }
    return counts;
}


/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head)
        return 0;

    if (list_empty(head) || list_is_singular(head))
        return q_size(list_entry(head->next, queue_contex_t, chain)->q);

    queue_contex_t *first = list_entry(head->next, queue_contex_t, chain);
    struct list_head *node = head->next->next;

    while (node != head) {
        queue_contex_t *cur = list_entry(node, queue_contex_t, chain);
        if (cur->q) {
            merge(first->q, cur->q, descend);
        }
        node = node->next;
    }
    return first->size;
}