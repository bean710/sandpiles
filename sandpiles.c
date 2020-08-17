#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "sandpiles.h"

unsigned int height = 11;
unsigned int width = 11;

clock_t bal_time = 0;
clock_t add_time = 0;
clock_t get_time = 0;

void print_binary(unsigned short n)
{
	while (n) {
		if (n & 1)
			printf("1");
		else
			printf("0");

		n >>= 1;
	}
	printf("\n");
}

void print_grid(unsigned short *grid)
{
	unsigned short val;

	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			val = getval(grid, j, i);
			if (val == 3)
				printf("\033[101m%u \033[0m", getval(grid, j, i));
			else if (val == 2)
				printf("\033[102m%u \033[0m", getval(grid, j, i));
			else if (val == 1)
				printf("\033[106m%u \033[0m", getval(grid, j, i));
			else
				printf("%u ", getval(grid, j, i));
		}
		printf("\n");
	}
}

void addval(unsigned short *grid, unsigned int x, unsigned int y, short val)
{
	clock_t begin = clock();

	if (x >= width || y >= height || x < 0 || y < 0)
		return;

	unsigned int coord = (y * width + x) / 5;
	unsigned int offset = (y * width + x) % 5;

	unsigned short original = grid[coord];
	unsigned short newVal = original;
	newVal = newVal >> (offset * 3);
	newVal = newVal & 0b111;
	newVal += val;

	unsigned short replace = original;
	unsigned short mask = ~(0b111 << (3 * offset));
	replace = replace & mask;
	mask = newVal << (3 * offset);
	replace = replace | mask;

	grid[coord] = replace;

	clock_t end = clock();
	add_time += begin - end;
}

unsigned short getval(unsigned short *grid, unsigned int x, unsigned int y)
{
	clock_t begin = clock();

	if (x >= width || y >= height || x < 0 || y < 0)
		return -1;

	unsigned int coord = (y * width + x) / 5;
	unsigned int offset = (y * width + x) % 5;

	clock_t end = clock();
	get_time += begin - end;

	return (grid[coord] >> (offset * 3) & 0b111);
}

bool needsbalanced(unsigned short *grid)
{
	unsigned int memcells = height * width / 5;
	unsigned short mask = 0b0100100100100100;
	for (unsigned int i = 0; i < memcells; i++)
		if (grid[i] & mask) return (true);

	return (false);
}

void balance(unsigned short *grid, bool detailed)
{
	clock_t begin = clock();

	while (needsbalanced(grid))
	{
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				if (getval(grid, j, i) > 3)
				{
					addval(grid, j, i, -4);
					addval(grid, j, i - 1, 1);
					addval(grid, j, i + 1, 1);
					addval(grid, j - 1, i, 1);
					addval(grid, j + 1, i, 1);
					if (detailed)
					{
						print_grid(grid);
						printf("\n");
					}
				}
			}
		}
	}

	clock_t end = clock();
	bal_time += begin - end;
}

bool is_symetrical(unsigned short *grid)
{
	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width / 2; j++)
		{
			if (getval(grid, j, i) != getval(grid, width - j - 1, i))
			{
				printf("%u, %u\n", j, i);
				return (false);
			}
		}
	}
	return (true);
}

int main(void)
{
	unsigned short *grid = calloc((height * width / 5), sizeof(unsigned short));
	unsigned int step = 0;

	for (unsigned int middle = 1500; middle > 0; middle -= 4)
	{
		//printf("Step: %u\n", step);
		addval(grid, width / 2, height / 2, 4);
		balance(grid, false);
		step++;
	}

	print_grid(grid);
	printf("Memory allocated: %lu bytes\n", (height * width / 5) * sizeof(unsigned short));
	printf("Using ints would've been %lu bytes\n", (height * width) * sizeof(int));

	printf("Get time: %f\n", (double)(get_time / CLOCKS_PER_SEC));
	printf("Add time: %f\n", (double)(add_time / CLOCKS_PER_SEC));
	printf("Bal time: %f\n", (double)(bal_time / CLOCKS_PER_SEC));

	free(grid);
}