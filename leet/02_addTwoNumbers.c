#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct ListNode {
    int val;
    struct ListNode *next;
};

struct ListNode *addTwoNumbers(struct ListNode *l1, struct ListNode *l2);

struct ListNode *createNumbers(int *numbers, int size);

void printNumbers(struct ListNode *node);

int test1[3] = {2, 4, 3};
int test2[3] = {5, 6, 4};

int main(int argc, const char *const argv[]) {
	struct ListNode *nodeA;
	struct ListNode *nodeB;
	struct ListNode *result;
	
	nodeA = createNumbers(test1, 3);
	nodeB = createNumbers(test2, 3);
	result = addTwoNumbers(nodeA, nodeB);
	printNumbers(result);
}


struct ListNode *addTwoNumbers(struct ListNode *l1, struct ListNode *l2)
{
	struct ListNode *nodeA;
	struct ListNode *nodeB;
	struct ListNode *result = NULL;
	struct ListNode *last = NULL;
	struct ListNode *first = NULL;
	int sum = 0, val = 0, carry = 0;
	
	if (!l1 && !l2)
		return NULL;
	
	for (nodeA = l1, nodeB = l2; (nodeA != NULL || nodeB != NULL); ) {
		if (nodeA && nodeB) {
			sum = nodeA->val + nodeB->val;
		} else if (nodeA) {
			sum = nodeA->val;
		} else {
			sum = nodeB->val;
		}
		
		result = malloc(sizeof(*result));
		sum += carry;
		carry = sum/10;
		val = sum % 10;
	
		result->val = val;
		result->next = NULL;
		
		if (last) {
		    last->next = result;
		} 
		
		if (!first) {
			first = result;
		}
		
		last = result;
		
		if (nodeA)
			nodeA = nodeA->next;
		
		if (nodeB)
			nodeB = nodeB->next;
	}
	
	if (carry != 0) {
		result = malloc(sizeof(*result));
		result->val = carry;
		result->next = NULL;
		last->next = result;
	}
	
	return first;
}

struct ListNode *createNumbers(int *numbers, int size)
{
	int i = 0;
	struct ListNode *result = NULL;
	struct ListNode *last = NULL;
	struct ListNode *first = NULL;
	
	for (i = 0; i < size; i++) {
		result = malloc(sizeof(*result));
		result->val = numbers[i];
		result->next = NULL;
		
		if (last) {
		    last->next = result;
		} 
		
		if (!first) {
			first = result;
		}
		
		last = result;		
	}
	
	return first;
}

void printNumbers(struct ListNode *node) {
	struct ListNode *nodeA;
	for (nodeA = node; nodeA != NULL; nodeA = nodeA->next) {
		printf("%d\n", nodeA->val);
	}
	
}