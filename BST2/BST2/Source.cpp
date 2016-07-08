#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
struct Student
{
	int rollno;
	char name[24];
	char college[20];
};

struct leafNode{
	int pageType;
	struct Student data[10];
	char unused[28];
};

struct NonLeafNode{
	int pageType;
	int keys[63];
	int offsets[64];
};

void get_student(FILE *fcsv, struct Student std){
	memset(&std, 0, sizeof(struct Student));
	fscanf(fcsv, "%d,%28[^,],%32[^\n]", &std.rollno, std.name, std.college);
}



struct leafNode create_leaf(struct Student *stds){
	struct leafNode leaf;
	for (int i = 0; i < 10; i++){
		leaf.data[i] = stds[i];
	}
	leaf.pageType = 1;
	return leaf;
}

void main(){
	FILE *fcsv = fopen("data.csv", "r");
	FILE *fbin = fopen("students.bin", "wb+");
	struct NonLeafNode root;
	memset(&root, 0, sizeof(root));
	fwrite(&root, sizeof(root), 1, fbin);
	printf("%d,", ftell(fbin));
	int count = 1;
	int keys_count = 0;
	struct leafNode lp;
	while (!feof(fcsv))
	{
		struct Student stds[10];
		for (int i = 0; i < 10; i++){
			memset(&stds[i], 0, sizeof(struct Student));
			//memset(&lp.data[i], 0, sizeof(lp.data[i]));
			//fscanf(fcsv, "%d,%28[^,],%32[^\n]", &lp.data[i].rollno, lp.data[i].name, lp.data[i].college);
			fscanf(fcsv, "%d,%28[^,],%32[^\n]", &stds[i].rollno, stds[i].name, stds[i].college);
		}
		lp = create_leaf(stds);
		fseek(fbin, 512 * count, 0);
		fwrite(&lp, sizeof(lp), 1, fbin);
		printf("%d,", ftell(fbin));
		count++;
		if (keys_count != 0)
			root.keys[keys_count - 1] = lp.data[0].rollno;
		root.offsets[keys_count] = keys_count * 512;
		keys_count++;

	}
	fseek(fbin, 0, 0);
	fwrite(&root, sizeof(root), 1, fbin);
	printf("\n%d,", ftell(fbin));
	printf("%d\n", count);
	for (int i = 0; i < 63; i++){
		printf("%d,", root.keys[i]);
	}
	fclose(fcsv);
	fclose(fbin);
	getchar();
}

