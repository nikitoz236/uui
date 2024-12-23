#include <stdio.h>
#include <stdint.h>

int main()
{
	uint32_t data[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

	int32_t (* data_ptr)[4] = &data;

	printf("%d\n", data_ptr[2][2]);
	printf("%d\n", data_ptr[1][0]);
	return 0;
}
