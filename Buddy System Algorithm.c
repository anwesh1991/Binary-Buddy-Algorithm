#include<stdio.h>
#include<malloc.h>
#define mainsize 6

/*Structures*/

struct displaycheck
{
    int a;
    void *ptr;
}s1[64];

struct track
{
    void *a;
    int b;
}s2[64];

/*Global Variables*/
int t=0;
void *freelist[mainsize];

/*Tracking functions*/

void keeptrack(void *block, int size)
{
    s2[t].a=block;
    s2[t].b=size;
    t++;
}

void showtrack()
{
    int i;
    printf("\n\nFollowing are the allocations that have been made\n\n");
    for(i=0;i<t;i++)
    printf("%d.Block of size %d at address %p\n",i+1,s2[i].b,s2[i].a);
}

void changetrack(int x)
{
    int i;
    for(i=x;i<t;i++)
    {
        s2[i].a=s2[i+1].a;
        s2[i].b=s2[i+1].b;
    }
    t--;
}

/*Allocation function*/

void *alloc(int size)
{
    int i;
    for(i=0;(1<<i)<size;i++);
    if(i>mainsize)
    {
        printf("\nFreespace unavailable\n\n");
        return NULL;
    }
    else if(freelist[i]!=NULL)
    {
        void *block;
        block=freelist[i];
        freelist[i]=*(void **)freelist[i];
        return block;
    }
    else
    {
        void *block,*buddy;
        block=alloc(1<<(i+1));
        if(block!=NULL)
        {
            buddy=( (void *) ( ((int)block) ^ (1<<i) ) );
            *(void **)buddy=freelist[i];
            freelist[i]=buddy;
        }
    }
}

/*Deallocation function*/

void dealloc(void *block, int size)
{

    int i;
    void **p;
    void *buddy;
    for(i=0;(1<<i)<size;i++);
    buddy=( (void *) ( ((int)block) ^ (1<<i) ) );
    p=&freelist[i];
    while((*p!=NULL)&&(*p!=buddy))
    {
        p=(void**)*p;
    }
    if(*p!=buddy)
    {
        *(void **)block=freelist[i];
        freelist[i]=block;
    }
    else
    {
        *p=*(void**)buddy;
        if(block>buddy)
        {
            dealloc(buddy,(1<<i+1));
        }
        else
        {
            dealloc(block,(1<<i+1));
        }
    }
}

/*Structure alteration post allocation/deallocation*/

void alter1(void *p1, int x)
{
    int i,j;
    for(i=0;i<64;i++)
        {
        if(s1[i].ptr==p1)
        {
            for(j=0;j<x;j++,i++)
            s1[i].a=1;
            break;
        }
    }
}

void alter2(int x)
{

    int i,j;
    for(i=0;i<64;i++)
    {
        if(s1[i].ptr==s2[x].a)
        {
            for(j=0;j<s2[x].b;j++,i++)
            s1[i].a=0;
            //printf("\n%d",s1[i].a);
            break;
        }
    }
}


/*Display function*/

void display()
{
    void *p,*q,*r;
    printf("\n\nfreelist[0]=%p\nfreelist[1]=%p\nfreelist[2]=%p\nfreelist[3]=%p\nfreelist[4]=%p\nfreelist[5]=%p\nfreelist[6]=%p\n",freelist[0],freelist[1],freelist[2],freelist[3],freelist[4],freelist[5],freelist[6]);

    int i,j,k=0;
    printf("\n\nCurrently this is the status of the block\n\n");
    j=1;
    printf("0");

    for(i=0;i<=64;i++)
    {
        if(i==j)
        {
            printf("%d",j);
            j=j<<1;
        }
    if(s1[i].a==1)
    printf("/");
    else
    printf(" ");
}

//end of display
}

/*Main Function*/

int main()
{
    int i,x,c,j,k=0,choice;
    void *p1,*p2;
    freelist[6]=malloc(64);
    x=(int)freelist[mainsize];

    /*While loop to make the initial freelist pointer point to a location where mainsize no. of least significant bits=0*/

    while(1)
    {
        for(i=0;i<6;i++)
        {
        if(x&1==1)
            goto label1;
            else
            x=x>>1;
        }
        label1:
        if(i==6)
        break;
        freelist[6]+=2;
        x=(int)freelist[6];
    }

    //intializing structure variable
    void *temp=freelist[6];
    for(i=0;i<64;i++,temp++)
    s1[i].ptr=temp;

    display();

    //main loop

    label2:
    printf("\n\nWhat would you like to do?\n1.Allocate\n2.Deallocate\n3.Display\n4.Show allocations made\n5.Exit\n");
    scanf("%d",&c);
    switch(c)
    {

        case 1:
        printf("\nEnter memory in bytes to be allocated\n");
        scanf("%d",&x);
        p1=alloc(x);
        if(p1==NULL) break;
        while(1)
        {
            if(x==1<<0||x==1<<1||x==1<<2||x==1<<3||x==1<<4||x==1<<5||x==1<<6)
            break;
            else
            x++;
        }
        keeptrack(p1,x);
        alter1(p1,x);
        display();
        break;


        case 2:
        showtrack();
        printf("\nWhich allocation would you like to deallocate?\n");
        scanf("%d",&choice);
        dealloc(s2[choice-1].a,s2[choice-1].b);
        alter2(choice-1);
        changetrack(choice-1);
        display();
        break;


        case 3:
        display();
        break;


        case 4:showtrack();
        break;


        default:goto label3;
        break;
    }

    goto label2;
    label3:
    return 0;
}
