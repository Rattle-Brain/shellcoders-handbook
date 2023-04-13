#include <stdio.h>
#include <stdlib.h>


int triangle (int width, int height){
	int array[5] = {0,1,2,3,4};
	int area;

	area = width * height/2;
	return (area);
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		perror("Syntax error: ./triangle <width> <height>\n");
		exit(EXIT_FAILURE);
	}

	int w = atoi(argv[1]);
	int h = atoi(argv[2]);
	int a = 0;

	if(w < 1 || h < 1)
	{
		perror("Both dimensions have to be at least 1\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		a = triangle(w, h);
		printf("Area of the tirangle for width(%d) and height(%d) is: %d\n", w, h, a);
		return 0;
	}

}
