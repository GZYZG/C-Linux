#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

/*
description:this program implments command ls -al
user can input file or directory,if input is the mixture of these two,totalsize is each directory's space size
author:gzy
date:2018.11.20 00:11 Tuesday
*/


//get file's type
char fileType(mode_t mode);

//get all files in a directory
void printFilesInDir(char* cwd); 

//get not directory file state infomation

//get file's mod
void getFileMod(mode_t mode, char* mod);

//get user name and group name from userid
void getNameFromID(uid_t id,char* ugname);

//print file's infomation
void printFileInfo(char* filename);

//seprate string
void sepString(char* str, char sep,char* save);

long int totalsize = 0;

int main(int argc, char** argv)
{
	struct stat status;
	char mod[10];
	//current working dir
	if(argc == 1){
		//if doesn't give the file or directory list
		char cwd[56];
		
		getcwd(cwd,sizeof(cwd));
		printFilesInDir(cwd);
		//return 0;
	}else if(argc >= 2){
		int count = 1;
		char c;
		//struct stat status;
		
		while(count <= argc-1){
			stat(argv[count], &status);
			c = fileType(status.st_mode);
			//printf("mode is %c \n",c);
			if(c == 'd'){
				//printf("now is %s \n",argv[count]);
				totalsize = 0;
				printf("\n%s:\n",argv[count]);
				printFilesInDir(argv[count]);
				printf("\n");
			}else if(c != '*'){
				printFileInfo(argv[count]);
			}
			count++;

		}
		
		//return 0;

	}
	//printf("real total size:%-6ldBytes\n",totalsize);
	
	
	
	return 0;
}


void printFilesInDir(char* cwd)
{
	/*
	cwd:direcory that need to be print
	this func will get all the files' absolute path in cwd,and call printFileInfo to show each file's info
	after print all files' info ,the function will print these files' total size,not memory block numbers 
	*/

/*
	struct stat{
		dev_t st_dev;	device id of inode
		ino_t st_ino;	inode id
		mode_t st_mode;	file format
		nlink_t st_nlink;	link of files
		uid_t st_uid;	file's user
		gid_t st_gid;	file's user's group
		dev_t st_rdev;	
		off_t st_size;	file's size
		time_t st_atime;	access time
		time_t st_mtime;	modify time
		time_t st_ctime;	change time
	}
*/
	DIR *dir;
	struct dirent *entry;
	int i = 0;
	dir = opendir(cwd);
	char* allfiles[10];
	while((entry = readdir(dir)) != NULL){
		allfiles[i] = (char*)malloc(strlen(cwd)+strlen(entry->d_name)+2);
		if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0){
			strcpy(allfiles[i],entry->d_name);		
		}else{
			strcpy(allfiles[i], cwd);
			strcat(allfiles[i],"/");
			strcat(allfiles[i],entry->d_name);
		}
		
		printFileInfo(allfiles[i]);
		i++;
	}

	close((long int)dir);
	printf("real total size:%-6ldBytes in directory %s \n",totalsize, cwd);
	totalsize = 0;

}

char fileType(mode_t mode)
{
	/*
	mode:a file's mode
	return:the file's type
	if this file's type is not in ('-','d','c','b','p',l') then return a '*'
	*/
	//mode is a 4 Bytes integer
	//printf("%d\n",sizeof(mode));
	if((mode & S_IFMT) == S_IFREG){
		return '-';
	}else if((mode & S_IFMT) == S_IFDIR){
		return 'd';
	}else if((mode & S_IFMT) == S_IFCHR){
		return 'c';
	}else if((mode & S_IFMT) == S_IFBLK){
		return 'b';
	}else if((mode & S_IFMT) == S_IFIFO){
		return 'p';
	}else if((mode & S_IFMT) == S_IFLNK){
		return 'l';
	}
	
	return '*';
	


}

void getFileMod(mode_t mode, char* mod)
{
	/*
	mode:a file's stat mode
	mod:the file's mod is saved as a str in mod
	*/
	//get owner's mod
	if(mode&S_IRUSR){
		mod[0] = 'r';
	}else{
		mod[0] = '-';
	}

	if(mode&S_IWUSR){
		mod[1] = 'w';
	}else{
		mod[1] = '-';
	}
	
	if(mode&S_IXUSR){
		mod[2] = 'x';
	}else{
		mod[2] = '-';
	}
	
	//get same group mod
	if(mode&S_IRGRP){
		mod[3] = 'r';
	}else{
		mod[3] = '-';
	}

	if(mode&S_IWGRP){
		mod[4] = 'w';
	}else{
		mod[4] = '-';
	}
	
	if(mode&S_IXGRP){
		mod[5] = 'x';
	}else{
		mod[5] = '-';
	}
	
	//get other mod
	if(mode&S_IROTH){
		mod[6] = 'r';
	}else{
		mod[6] = '-';
	}

	if(mode&S_IWOTH){
		mod[7] = 'w';
	}else{
		mod[7] = '-';
	}
	
	if(mode&S_IXOTH){
		mod[8] = 'x';
	}else{
		mod[8] = '-';
	}

	mod[9] = '\0';
}


void getNameFromID(uid_t id,char* ugname)
{
	/*
	id:the userid that you get
	ugname:the user's name and group name of id belongs to
	username and groupname is seperated by a space
	*/
	struct passwd *passwd;
	passwd = getpwuid(id);
	strcat(ugname, passwd->pw_name);
	strcat(ugname, " ");
	struct group *grp;
	grp = getgrgid(passwd->pw_gid);
	strcat(ugname, grp->gr_name);
	strcat(ugname, "\0");
	

}

void sepString(char* str, char sep,char* save)
{	
	/*
	str:the string is going to be seperate
	sep:the seperator	
	save:the last part of str after the last sep occurs
	if no sep in str,than the str is assigned to save
	*/
	int i = 0;
	for(i = strlen(str)-1; i >= 0; i--){
		if(str[i] == sep){
			strcpy(save, str+i+1);
			return;
		}

	}
	if(i == 0){
		strcpy(save,str);
	}
	return;


}

void printFileInfo(char* filename)
{
	/*
	filename:the file's name need to be print
	*/
	
	struct stat status;
	char name[strlen(filename)];
	strcpy(name,filename);
	char mod[10];
	char ugname[20] = "";
	char *chgtime;
	stat(filename, &status);
	getFileMod(status.st_mode, mod);
	getNameFromID(status.st_uid, ugname);
	chgtime = ctime(&(status.st_ctime));
	//notice the diff of strlen and sizeof
	chgtime[strlen(chgtime)-1] = '\0';
	char cwd[256];
	getcwd(cwd,sizeof(cwd));
	
	sepString(filename,'/', name);
	printf("%c%s %ld %s %-6ld %s %s\n",fileType(status.st_mode), mod, status.st_nlink, ugname, status.st_size, chgtime, name);
	totalsize += status.st_size;

}












