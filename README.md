# BST

1. Given a list of student records in a CSV file, 1st task is to construct a B+tree in a binary file where all the leaf nodes are the student records and non leaf nodes hold the keys and offsets of the records which are present in the leaf nodes. 2nd task is to query the B+Tree which is in the binary file based on the student roll numbers ( range scan of roll numbers and perform group by for the result based on college name )
Structure for leaf Node
struct student(size : 64 Bytes )
{
int type;
int rollno;
char name[28];
char college[28];
};
Structure for Non leaf node(size : 64 Bytes)
struct nonLeaf
{
int type;
int offset[3];
int keys[2];
char extra[40];
};
2. Given a CSV file of student records, construct a B+ tree where leaf nodes of B+tree are now leaf pages which can now hold offsets of 10 student records(which are stored in the beginning of the file). Non leaf pages will hold at max of 64 offsets with 63 keys. (size should be exact such that root ( non leaf page ) will be located in the last 512 bytes of the binary file )
Structure of student
struct student(size : 48 Bytes)
{
int rollno;
char name[24];
char college[20];
};
Structure of leafpage(size : 512 bytes)
struct leafpage
{
int pagetype ;
struct student data[10];
char unused[28];
};
Structure of non leaf page(size : 512 Bytes)
struct nonleafpage
{
int pagetype ;
int keys[63];
int offsets[64];
}; 
3. Given a CSV file of student records, construct a B+ tree in a binary file where a table sector should be written in the beginning of the file which will hold the root pages and table ids of different records. For now only one( student record ) type of record is given in the input file. The root ( non leaf page ) offset need to be stored in the table sector.
Additional structure used here is
struct tablesector
{
int tableid[8];
int rootpageid[8];
};
4. Given a CSV files of student and teacher records, construct a B+ tree in a binary file where a table sector should be written in the beginning of the file which will hold the root pages and table ids of different records. Here two( student and teacher records ) types of records are given in the input file. The root ( non leaf page ) offsets need to be stored in the table sector.
Additional structure 
struct teachingstaff
{
int staffid;
char name[24];
char dept[12];
char college[24];
};
5. ( Extension to previous problem ) perform queries on the B+ tree created, given a student roll no. find the college name of the students and search for number of teachers from that college. Write you own Syntax for Queries ,make sure you are trying to support as many queries as possible .

6. To deal with atomicity issues and data transfer breakdown problems a new page called bit vector page is introduced here to tell which pages are empty. Each time a new non leaf page is created a new bit vector page need to be created get updated about new page and the old bit vector page values. The new location is updated in the table sector only after new bit vector page is updated. All the old bit vector pages will be available as free pages and can be identified from the new bit vector page.

Tip :To perform count queries, add a prev pointer  and also table id to leafnode structure for efficiency.
struct LeafPage {
int prevpage;
int type = 1;
int table_id = 1;
struct student dt[10];
char unused[20];
};
