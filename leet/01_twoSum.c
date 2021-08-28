#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* twoSum(int* nums, int numsSize, int target, int* returnSize);

int main(int argc, const char *const argv[])
{
	int nums[4] = {2,7,11,15};
	int rsize;
	int *result;
	
	result = twoSum(nums, 4, 9, &rsize);
	
	if (result && rsize == 2) {
		printf("[%d, %d] \n", result[0], result[1]);
	}
}

int* twoSum(int* nums, int numsSize, int target, int* returnSize)
{
	int i, j;
	int *result = NULL;
	int diff;
	
	if (numsSize <= 1) {
		*returnSize = 0;
		return NULL;
	}
	
	for (i = 0; i < numsSize; i++) {
		diff = target - nums[i];
		for (j = i + 1; j < numsSize; j++) {	
			if (nums[j] == diff) {
				result = malloc(sizeof(int) * 2);
				*returnSize = 2;
				result[0] = i;
				result[1] = j;
				return result;
			}
		}
	}
	
	*returnSize = 0;
	return NULL;
}