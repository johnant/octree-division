﻿ /* 
 * File:   main.c
 * Author: Ioannis Antoniadis 7137
 * pthreads Version Started 09-11-2013  
 * pthreads Version Finished 23-11-2013
 * OpenMp Version Started 19-11-2013
 * OpenMp Version Finished 24-11-2013
 * Created on 3 Νοέμβριος 2013, 4:23 μμ
 */


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#define MAX_THREADS 16
#define S 20

typedef struct
{
    int level, boxid, parent, child[8], n, start, colleague[26],counter,*points,child_index,colleague_cnt;
    double center[3], length;
} Box;

/*Define a matrix of boxes and leaves*/
Box *box;
Box *leaf; 

float N = pow(2,20);
double **A,**B;
int divisionTime;
int colleaguesTime;
int filesTime;
int idCounter=1; 
int leafCounter=0; 
int maxLevel=0;
int *levelNodes;
int totalThreads = 0;
int totalPoints = 0;
int threads;

pthread_mutex_t leafMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t boxMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;

/*Function Prototypes*/
void checkBox(int position);
void createOctants(Box *cube);
void findColleagues();
void fileInsertion();
void generator();
void boxesFile();
void leavesFile();
void betaFile();
void alphaFile();
void colleaguesFile();
void timeFile();
void createB();

int main(int argc, char** argv) {
    
    int i,j, leafSum=0;
    struct timeval start,end;
    
/*Original function generation 3-D points (x,y,z) that belong on the first 
octant of the unit sphere in three-dimentional space */
        generator();

    box = (Box*)malloc(sizeof(Box)); 
    if(box==NULL){
        exit(3);}
    
        gettimeofday(&start, NULL);
    
/*BEGINNING OF CALCULATIONS - INITIALIZATION OF THE ROOT */
    box[0].boxid = 1;
    box[0].level = 0;
    box[0].parent = 0;
    box[0].center[0] = 0.5;
    box[0].center[1] = 0.5;
    box[0].center[2] = 0.5;
    box[0].length = 1;
    box[0].start=0;
    box[0].n=N; 
    box[0].points = (int*)malloc(N*sizeof(int));
    for(i=0;i<N;i++){   
        box[0].points[i]=i;
    }
    for(i=0;i<26;i++){
        box[0].colleague[i]=0;
    }
    
/*MAIN THREAD BEGINS THREAD EXECUTING CALCULATING OCTREES!!*/
    checkBox(0);
    printf("\n\nTotal n measured in boxes = %d",totalPoints);
    
/*OPTIONAL - JUST FOR VERIFICATION*/  
    for(i=0;i<leafCounter;i++){
        leafSum+=leaf[i].n;    
    }
    printf("\nTotal n measured in leaves is %d",leafSum);
  
    gettimeofday(&end, NULL);
    divisionTime = ((end.tv_sec * 1000000 + end.tv_usec) -(start.tv_sec * 1000000 + start.tv_usec));
  
/*******FIND COLLEAGUES - PTHREADS VERSION*******/
 
    gettimeofday(&start, NULL);   
    remove("colleagues.txt");
    /*First, we find the number of nodes in every single level*/
    
    levelNodes = (int*)malloc((maxLevel+1)*sizeof(int));
    if(levelNodes==NULL){
        exit(2);
    }
    for(i=0;i<maxLevel+1;i++){
        levelNodes[i]=0;}
    for(i=0;i<idCounter;i++){
        levelNodes[(box[i].level)]++;}
    for(i=0;i<maxLevel+1;i++){
        printf("\nlevelNodes[%d] = %d",i,levelNodes[i]);}
  
    printf("\nmax level is %d",maxLevel);
 
    
    findColleagues();
      
    gettimeofday(&end, NULL);
    colleaguesTime = ((end.tv_sec * 1000000 + end.tv_usec) -(start.tv_sec * 1000000 + start.tv_usec));

    /*Move all points from A to B with sequential order*/ 
    createB();
    
 /*FILESYSTEM PTHREADS VERSION - SAVES IMPORTANT DATA IN FILES*/
    gettimeofday(&start, NULL);
   /* pthread_t t1,t2,t3,t4,t5,t6;
    printf("\nSaving in files...");
        pthread_create(&t1,NULL,&boxesFile,NULL);
        pthread_create(&t2,NULL,&leavesFile,NULL);
        pthread_create(&t3,NULL,&alphaFile,NULL);
        pthread_create(&t4,NULL,&betaFile,NULL);
        pthread_create(&t5,NULL,&timeFile,NULL);
        pthread_create(&t6,NULL,&colleaguesFile,NULL);
        
        pthread_join(t1,NULL);
        pthread_join(t2,NULL);
        pthread_join(t3,NULL);
        pthread_join(t4,NULL);
        pthread_join(t5,NULL);
        pthread_join(t6,NULL);
     */   
        gettimeofday(&end, NULL);
    filesTime = ((end.tv_sec * 1000000 + end.tv_usec) -(start.tv_sec * 1000000 + start.tv_usec));
    
    printf("\n\nOctree Division time is %d",divisionTime);
    printf("\nColleagues finding time is %d",colleaguesTime);
    printf("\nSaving data in files time is %d",filesTime);
    printf("\nTotal calculation time is %d",divisionTime+colleaguesTime+filesTime);
    printf("\n\n***END OF PROGRAM***");
    
    return (EXIT_SUCCESS);
}

void checkBox(int boxIndex)
{
   //printf("\nInside checkBox(id = %d)",boxIndex+1);
    int i;
    Box cube,parent;
    
/*BOX MUTEX LOCK*/ 
    pthread_mutex_lock(&boxMutex);
    cube = box[boxIndex];
    parent = box[cube.parent-1];
    
    pthread_mutex_unlock(&boxMutex);
/*BOX MUTEX UNLOCK*/

    /*Array with points indexes that belong to the cube*/
    if(cube.boxid==1){
        printf("\nroot !");
      }
    else{
        cube.points = (int*)malloc(parent.n*sizeof(int));
/*Checking how many points are included in the cube*/
    for(i=0;i<parent.n;i++){   
        if(fabsf(cube.center[0]-A[parent.points[i]][0])<cube.length/2){
            if(fabsf(cube.center[1]-A[parent.points[i]][1])<cube.length/2){
                if(fabsf(cube.center[2]-A[parent.points[i]][2])<cube.length/2){      
                    cube.n++;
                    cube.points[cube.n-1]=parent.points[i];
                }}}}}
        //printf("\nNumber of points in cube = %d",cube.n);    
    
    if(cube.n==0){ 
        pthread_mutex_lock(&boxMutex);
        box[boxIndex].boxid = 0;
        box[box[boxIndex].parent-1].child[cube.child_index] = 0;
        pthread_mutex_unlock(&boxMutex);
    }
        else if(cube.n<=S){         
/*LEAF MUTEX LOCK*/         
        pthread_mutex_lock(&leafMutex);
        leafCounter++;
        leaf = (Box*)realloc(leaf,leafCounter*sizeof(Box));
        leaf[leafCounter-1]=cube;
        totalPoints += cube.n;
        pthread_mutex_unlock(&leafMutex);
/*LEAF MUTEX UNLOCK*/   
        pthread_mutex_lock(&boxMutex);
        box[boxIndex] = cube;
        pthread_mutex_unlock(&boxMutex);
    } 
    else{ 
            pthread_mutex_lock(&boxMutex);
            box[boxIndex] = cube ;
            pthread_mutex_unlock(&boxMutex);
            createOctants(&cube);
    }
}

void createOctants(Box *cube)
{
   //printf("\nInside createOctans(parrent id = %d)",cube->boxid);
    int i=0,j=0;
    
/*BOX MUTEX LOCK*/ 
    /*This cube will be a parent of 8 others. We need to save his data so the 8 children can access it!*/
    pthread_mutex_lock(&boxMutex);
    box = (Box*)realloc(box,(8+idCounter)*sizeof(Box));

    /*Give the appropriate values at box[] referring to nodes*/
    for(i=0; i<8; i++)
    {
        idCounter++;
        
        box[idCounter-1].boxid=idCounter;// boxid of new child
        box[cube->boxid-1].child[i]=idCounter; // pass the data of the child in parent box
        //printf("\nChild %d of parent id = %d has id = %d",i+1,cube->boxid,box[cube->boxid-1].child[i]);
        box[idCounter-1].length=cube->length/2;
        box[idCounter-1].parent=cube->boxid;
        box[idCounter-1].level=cube->level+1;
        box[idCounter-1].n = 0;
        box[idCounter-1].child_index = i;
        box[idCounter-1].colleague_cnt = 0;
        for(j=0;j<26;j++){
            box[idCounter-1].colleague[j]=0;
        }}   
    
    if(cube->level+1 > maxLevel){
            maxLevel = cube->level+1;}
    cube->counter=idCounter;  
    
    box[cube->counter-8].center[0]=cube->center[0]-cube->length/4;//0.25
    box[cube->counter-8].center[1]=cube->center[1]-cube->length/4;//0.25
    box[cube->counter-8].center[2]=cube->center[2]-cube->length/4;//0.25
    
    box[cube->counter-7].center[0]=cube->center[0]-cube->length/4;//0.25
    box[cube->counter-7].center[1]=cube->center[1]-cube->length/4;//0.25
    box[cube->counter-7].center[2]=cube->center[2]+cube->length/4;//0.75
    
    box[cube->counter-6].center[0]=cube->center[0]+cube->length/4;//0.75
    box[cube->counter-6].center[1]=cube->center[1]-cube->length/4;//0.25
    box[cube->counter-6].center[2]=cube->center[2]-cube->length/4;//0.25
    
    box[cube->counter-5].center[0]=cube->center[0]+cube->length/4;//0.75
    box[cube->counter-5].center[1]=cube->center[1]-cube->length/4;//0.25
    box[cube->counter-5].center[2]=cube->center[2]+cube->length/4;//0.75
    
    box[cube->counter-4].center[0]=cube->center[0]-cube->length/4;//0.25
    box[cube->counter-4].center[1]=cube->center[1]+cube->length/4;//0.75
    box[cube->counter-4].center[2]=cube->center[2]-cube->length/4;//0.25
    
    box[cube->counter-3].center[0]=cube->center[0]-cube->length/4;//0.25
    box[cube->counter-3].center[1]=cube->center[1]+cube->length/4;//0.75
    box[cube->counter-3].center[2]=cube->center[2]+cube->length/4;//0.75
    
    box[cube->counter-2].center[0]=cube->center[0]+cube->length/4;//0.75
    box[cube->counter-2].center[1]=cube->center[1]+cube->length/4;//0.75
    box[cube->counter-2].center[2]=cube->center[2]-cube->length/4;//0.25
    
    box[cube->counter-1].center[0]=cube->center[0]+cube->length/4;//0.75
    box[cube->counter-1].center[1]=cube->center[1]+cube->length/4;//0.75
    box[cube->counter-1].center[2]=cube->center[2]+cube->length/4;//0.75
    
pthread_mutex_unlock(&boxMutex);
/*BOX MUTEX UNLOCK*/

    if(totalThreads<MAX_THREADS){
/*THREAD MUTEX LOCK*/ 
        pthread_mutex_lock(&threadMutex);
        totalThreads+=8;
        pthread_mutex_unlock(&threadMutex);
/*THREAD MUTEX UNLOCK*/
        //printf("\n8 threads about to be born !"); 
        pthread_t t1,t2,t3,t4,t5,t6,t7,t8;

         pthread_create(&t1,NULL,&checkBox,cube->counter-8);
         pthread_create(&t2,NULL,&checkBox,cube->counter-7);
         pthread_create(&t3,NULL,&checkBox,cube->counter-6);
         pthread_create(&t4,NULL,&checkBox,cube->counter-5);
         pthread_create(&t5,NULL,&checkBox,cube->counter-4);
         pthread_create(&t6,NULL,&checkBox,cube->counter-3);
         pthread_create(&t7,NULL,&checkBox,cube->counter-2);
         pthread_create(&t8,NULL,&checkBox,cube->counter-1);

      printf("\n8 threads were born !");

         pthread_join(t1,NULL);
         pthread_join(t2,NULL);
         pthread_join(t3,NULL);
         pthread_join(t4,NULL);
         pthread_join(t5,NULL);
         pthread_join(t6,NULL);
         pthread_join(t7,NULL);
         pthread_join(t8,NULL);
         printf("\nThe 8 threads of parent %d have finished!",cube->boxid);
/*THREAD MUTEX LOCK*/
         pthread_mutex_lock(&threadMutex);
         totalThreads-=8;
         pthread_mutex_unlock(&threadMutex);
/*THREAD MUTEX UNLOCK*/
         free(cube->points);
    }
    else {
       // printf("\nSequential call of checkBox from the 8 chlidren of parent %d",cube->boxid);
     checkBox(cube->counter-8);
     checkBox(cube->counter-7);
     checkBox(cube->counter-6);
     checkBox(cube->counter-5);
     checkBox(cube->counter-4);
     checkBox(cube->counter-3);
     checkBox(cube->counter-2);
     checkBox(cube->counter-1);
    }
  
}

void findColleagues(){
    
    int level,i,j,m,parent_id,childID,colleagueID;
    double dist0,dist1,dist2;

    /*calculate the distances between them and decide whether they are colleagues!*/
    for (level = 0;level<maxLevel+1;level++){
        for(i=1;i<idCounter;i++){
            if (box[i].level==level){
                parent_id = box[i].parent;
                if (parent_id!=0){
                    for (j=0;j<8;j++){
                        childID = box[parent_id-1].child[j];
                        if (childID!=0){
                            if (box[i].boxid!=box[childID-1].boxid){
                                box[i].colleague[box[i].colleague_cnt++]=box[childID-1].boxid;}}}
            
      
                    for (j=0;j<26;j++){
                        colleagueID = box[parent_id-1].colleague[j]; 
                        if (colleagueID!=0){
                            if(box[colleagueID-1].n>S){
                            for (m=0;m<8;m++){
                                childID = box[colleagueID-1].child[m];
                                if (childID!=0){
                                    if (box[i].boxid!=box[childID-1].boxid){
                                        dist0=box[childID-1].center[0]-box[i].center[0];
                                        dist1=box[childID-1].center[1]-box[i].center[1];
                                        dist2=box[childID-1].center[2]-box[i].center[2]; 
                                        if(sqrt(dist0*dist0+dist1*dist1+dist2*dist2)<=sqrt(3)*box[i].length){    
                                           box[i].colleague[box[i].colleague_cnt++]=box[childID-1].boxid;
                                        }}}}}}}}}}}
 
}  

void boxesFile(){
    
    remove("boxes.txt");
    
    FILE *fp;
    int i,j;
    fp=fopen("boxes.txt","a+");
    fprintf(fp,"ID_____________number of points__________LEVEL\n");
    for(i=0;i<idCounter;i++){
    fprintf(fp,"%d____________________%d_________________%d\n",box[i].boxid, box[i].n, box[i].level);    
    }
    fclose(fp);
    fflush(fp);
    
   }

void betaFile(){
    
    remove("beta.txt");
    
    FILE *fp;
    int i,j;
    
    fp=fopen("beta.txt","a+");
    fprintf(fp,"x______________________y_____________________z\n"); 
    for(i=0;i<N;i++){
        fprintf(fp,"%f______________%f__________________%f\n",B[i][0],B[i][1],B[i][2]);
    }
    fclose(fp);
    fflush(fp);
    
}

void alphaFile(){
    remove("alpha.txt");
    
    FILE *fp;
    int i,j;
    
      fp=fopen("alpha.txt","a+");
    fprintf(fp,"x______________________y_____________________z\n"); 
    for(i=0;i<N;i++){
        fprintf(fp,"%f______________%f__________________%f\n",A[i][0],A[i][1],A[i][2]);
    }
    fclose(fp);
    fflush(fp);
}

void leavesFile(){
    
    remove("leaves.txt");
    
    FILE *fp;
    int i,j;
    
    fp=fopen("leaves.txt","a+");
    fprintf(fp,"ID_____________number of points__________LEVEL\n");
    for(i=0;i<leafCounter;i++){   
    fprintf(fp,"%d____________________%d_________________%d\n",leaf[i].boxid,leaf[i].n,leaf[i].level);    
    }
    fclose(fp);
    fflush(fp);
}

void colleaguesFile(){
    
    remove("colleagues.txt");
    
    FILE *fp;
    int i,j;
    
    fp=fopen("colleagues.txt","a+");
    for(i=0;i<idCounter;i++)
    {               
        if (box[i].boxid!=0)
        {
            fprintf(fp,"\nColleagues__of__leaf_box__with__id__%d_:__",box[i].boxid);
            for(j=0;j<26;j++)
            {
                if(box[i].colleague[j]!=0)
                {
                    fprintf(fp,"__%d____",box[i].colleague[j]);
                }
            }
        }
    }
    
    fclose(fp);
    fflush(fp);
}

void timeFile(){
 
    remove("time.txt");
    
    FILE *fp;
    int i,j;
 
    fp=fopen("time.txt","a+");
    fprintf(fp,"Division_Time_is_%d\n",divisionTime);
    fprintf(fp,"Finding_Colleagues_Time_is_%d\n",colleaguesTime); 
    fprintf(fp,"Creating_Files_Time_is_%d\n",filesTime);
    fprintf(fp,"Total_Time_is_%d\n",divisionTime+colleaguesTime+filesTime); 
    fclose(fp);
    fflush(fp);
  
}

void generator(){
    
    int i;
    
    A = (double**)malloc(N * sizeof(double*));
    if(A==NULL)
    {
        exit(22);
    }
    
    /*Generator of random points on first octant of unit sphere*/
    srand(time(NULL));
    //srand(0);
    double y_max = 0, root = 0;
    for(i=0 ; i<N ; i++)
    {
        A[i] = (double*)malloc(3 * sizeof(double));
        /*points for which it states x^2+y^2+z^2=1 and 0<=x,y,z<=1*/ 
        A[i][0] = ((double)rand()/(double)RAND_MAX);
        /*y_max = 1 - x^2*/
        y_max = sqrt(1 - pow(A[i][0],2));
        A[i][1] = ((double)rand()/(double)RAND_MAX)*y_max;
        /*z = 1 - x^2 - y^2*/
        A[i][2] = sqrt(1-pow(A[i][0],2) - pow(A[i][1],2));  
    }
}

void createB(){
    int i,j,counter=0;
    
    B=(double**) malloc(sizeof(double*)*N);
    if(B==NULL){
        exit(1);} 
    
    for(i=0;i<N;i++){
        B[i]= (double*)malloc(sizeof(double)*3);}
    
    for(i=0;i<leafCounter;i++){
		leaf[i].start = counter;
        for(j=0;j<leaf[i].n;j++){
            B[counter][0]=A[leaf[i].points[j]][0];
            B[counter][1]=A[leaf[i].points[j]][1];
            B[counter][2]=A[leaf[i].points[j]][2];
            counter++;
        }
    }
    printf("\nSuccesfully moved all points from A to B with sequential order !");
}