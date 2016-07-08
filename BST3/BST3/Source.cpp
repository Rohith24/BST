#include<stdio.h>
#include<string.h>
#include<limits.h>
struct Student
{
	int rollno;
	char name[24];
	char college[20];
};

struct leafNode{
	int tableid;
	int pageType;
	struct Student data[10];
	char unused[24];
};

struct NonLeafNode{
	int pageType;
	int keys[63];
	int offsets[64];
};

struct TableSector{
	int tableid[8];
	int rootid[8];
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
	leaf.tableid = 1;
	leaf.pageType = 1;
	return leaf;
}

void main(){
	FILE *fcsv = fopen("data.csv", "r");
	FILE *fbin = fopen("students.bin", "wb+");
	struct TableSector ts;
	memset(&ts, 0, sizeof(ts));
	fwrite(&ts, sizeof(ts), 1, fbin);
	int count = 0;
	int keys_count = 0;
	struct leafNode lp;
	struct NonLeafNode nln;
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
		fseek(fbin, (512 * count) + sizeof(ts), 0);
		fwrite(&lp, sizeof(lp), 1, fbin);
		printf("%d,", ftell(fbin));
		count++;
		if (keys_count != 0)
			nln.keys[keys_count - 1] = lp.data[0].rollno;
		nln.offsets[keys_count] = keys_count * 512;
		keys_count++;
		if (count != 1){
			int rootoffset = ftell(fbin);
			fwrite(&nln, sizeof(nln), 1, fbin);
			count++;
			fseek(fbin, 0, 0);
			ts.rootid[0] = rootoffset;
			ts.tableid[0] = 1;
			fwrite(&ts, sizeof(ts), 1, fbin);
		}
	}
	printf("\n");
	struct NonLeafNode root;
	fseek(fbin, ts.rootid[0], 0);
	fread(&root, sizeof(struct NonLeafNode), 1, fbin);
	for (int i = 0; i < 63; i++){
		printf("%d,", root.keys[i]);
	}
	fclose(fcsv);
	fclose(fbin);
	getchar();
}
