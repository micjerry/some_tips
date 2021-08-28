#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int check_letter(char *start, int length, char letter);
int lengthOfLongestSubstring(char *s);

int main(int argc, const char *const argv[])
{
	int max_length;
	
	max_length = lengthOfLongestSubstring(argv[1]);
	printf("max substr %d \n", max_length);

}

int lengthOfLongestSubstring(char *s)
{
	char *letter = s;
	char *start = s;
	int repeat;
	int length = 0;
	int max_length = 0;
	
	while (*letter != '\0') {
		repeat = check_letter(start, length, *letter);
		if (repeat) {
			start++;
			letter = start;
			if (length > max_length) {
				max_length = length;
			}
			length = 0;
			continue;
		}
		letter++;
		length++;
	}
	
	if (length > max_length) {
		max_length = length;
	}
	
	return max_length;
}

int check_letter(char *start, int length, char letter)
{
	int i = 0;
	
	if (length == 0) {
		return 0;
	}
	
	for (i = 0; i < length; i++) {
		if (start[i] == letter) {
			return 1;
		}
	}
	
	return 0;
}