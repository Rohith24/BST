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

struct TeachingStaff{
	int staffid;
	char name[24];
	char dept[12];
	char college[24];
};

struct TleafNode{
	int tableid;
	int pageType;
	struct TeachingStaff data[7];
	char unused[56];
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

char* getCollege(int id)
{
	FILE *f = fopen("students.bin", "r+b");
	struct TableSector ts;
	memset(&ts, 0, sizeof(ts));
	fread(&ts, sizeof(ts), 1, f);
	fseek(f, ts.rootid[0], SEEK_SET);
	struct NonLeafNode nlp;
	memset(&nlp, 0, sizeof(nlp));
	fread(&nlp, sizeof(nlp), 1, f);
	int i = 0;
	while (1)
	{
		if (id < nlp.keys[i]){
			fseek(f, nlp.offsets[i], SEEK_SET);
			break;
		}
		else if (i == 62){
			fseek(f, nlp.offsets[63], SEEK_SET);
			break;
		}
		i++;
	}
	struct leafNode slp;
	memset(&slp, 0, sizeof(slp));
	fread(&slp, sizeof(slp), 1, f);
	char *clg = (char*)malloc(sizeof(char)* 20);
	for (i = 0; i<10; i++)
	{
		//printf("%d\t", slp.data[i].rollno);
		if (id == slp.data[i].rollno){
			strcpy(clg, slp.data[i].college);
			printf("\n%s\n", clg);
			return clg;
		}
	}
	return "";
}

void getStaffCount(char college[24]){
	FILE *fbin = fopen("students.bin", "rb+");
	int count = 0;
	fseek(fbin, 0, 0);
	struct TableSector ts;
	fread(&ts, sizeof(ts), 1, fbin);
	struct NonLeafNode tnln;
	fseek(fbin, ts.rootid[1], 0);
	fread(&tnln, sizeof(tnln), 1, fbin);
	for (int i = 0; i < 64; i++){
		int offset = tnln.offsets[i];
		fseek(fbin, offset, 0);
		struct TleafNode tln;
		fread(&tln, sizeof(tln), 1, fbin);
		for (int j = 0; j < 7; j++){
			if (strcmp(tln.data[j].college, college) == 0)
				count++;
		}
	}
	printf("%d", count);
}

void main(){
	FILE *fcsv = fopen("data.csv", "r");
	FILE *fbin = fopen("students.bin", "wb+");
	FILE *fscsv = fopen("teacherdata.csv", "r");
	struct TableSector ts;
	memset(&ts, 0, sizeof(ts));
	fwrite(&ts, sizeof(ts), 1, fbin);
	int std_count = 0, staff_count = 0;
	int std_keys_count = 0, staff_keys_count = 0;
	struct leafNode lp;
	struct TleafNode tlp;
	memset(&lp, 0, sizeof(struct leafNode));
	memset(&tlp, 0, sizeof(struct TleafNode));
	struct NonLeafNode nln, tnln;
	int nonleaf_count = 0;
	memset(&nln, 0, sizeof(struct NonLeafNode));
	memset(&tnln, 0, sizeof(struct NonLeafNode));
	int prev = INT_MAX;
	printf("Student id's are:");
	while (!feof(fcsv))
	{
		struct Student stds[10];
		for (int i = 0; i < 10; i++){
			memset(&stds[i], 0, sizeof(struct Student));
			fscanf(fcsv, "%d,%28[^,],%32[^\n]", &stds[i].rollno, stds[i].name, stds[i].college);
			printf("%d,", stds[i].rollno);
			if (i < 7){
				memset(&tlp.data[i], 0, sizeof(tlp.data[i]));
				tlp.pageType = 1;
				fscanf(fscsv, "%d,%24[^,],%12[^,],%24[^\n]", &tlp.data[i].staffid, tlp.data[i].name, tlp.data[i].college, tlp.data[i].dept);
			}
		}
		lp = create_leaf(stds);
		lp.tableid = 1;
		fseek(fbin, (512 * (std_count + staff_count)) + sizeof(ts), 0);
		//printf("std- %d\t", ftell(fbin));
		fwrite(&lp, sizeof(lp), 1, fbin);
		//printf("%d\n", ftell(fbin));
		std_count++;

		if (std_keys_count != 0)
			nln.keys[std_keys_count - 1] = lp.data[0].rollno;
		nln.offsets[std_keys_count] = (std_count + staff_count - 1) * 512 + sizeof(ts);
		std_keys_count++;

		if (std_count != 1){
			//printf("%d\n", ftell(fbin));
			int std_rootoffset = ftell(fbin);
			nln.pageType = 2;
			fwrite(&nln, sizeof(nln), 1, fbin);
			nonleaf_count++;
			std_count++;
			fseek(fbin, 0, 0);
			ts.rootid[0] = std_rootoffset;
			ts.tableid[0] = 1;
			fwrite(&ts, sizeof(ts), 1, fbin);
		}

		tlp.tableid = 2;
		fseek(fbin, (512 * (std_count + staff_count)) + sizeof(ts), 0);
		//printf("staff- %d\t", ftell(fbin));
		prev = ftell(fbin);
		fwrite(&tlp, sizeof(tlp), 1, fbin);
		//printf("%d\n", ftell(fbin));
		staff_count++;

		if (staff_keys_count != 0)
			tnln.keys[staff_keys_count - 1] = tlp.data[0].staffid;
		tnln.offsets[staff_keys_count] = (std_count + staff_count - 1) * 512 + sizeof(ts);
		staff_keys_count++;

		if (staff_count != 1){
			int staff_rootoffset = ftell(fbin);
			tnln.pageType = 2;
			fwrite(&tnln, sizeof(tnln), 1, fbin);
			nonleaf_count++;
			staff_count++;
			fseek(fbin, 0, 0);
			ts.rootid[1] = staff_rootoffset;
			ts.tableid[1] = 2;
			fwrite(&ts, sizeof(ts), 1, fbin);
		}
	}

	fclose(fcsv);
	fclose(fbin);
	int id;
	printf("\nEnter Student id:");
	scanf("%d", &id);
	getStaffCount(getCollege(id));
	getchar();
	getchar();
}