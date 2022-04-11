#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include<string.h>
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].old_val =0;
    wp_pool[i].new_val =0;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp(char *st)
{
   if(free_==NULL)
   { 
      printf("free链表为空！\n");
      assert(0);      
   }
   WP *temp=free_;
   bool *success=false;
   free_=free_->next;
   strcpy(temp->expr,st);
   temp->old_val=expr(temp->expr,success);
   temp->next=NULL;
   if(head==NULL)	 
       head=temp;
   else
   {
       WP *ptemp=head;
      while(ptemp->next!=NULL)
	    ptemp=ptemp->next;
     ptemp->next=temp;  
   }
   return temp;
}
bool free_wp(int n)
{
   if(head==NULL)
   {
      printf("监视点链表为空！\n");
      assert(0);      
   }
   WP *p=head;
   WP *p0=head;
   if(head->NO==n)
   {
      head=head->next;
      p->next=free_;
      p->old_val=0;
      p->new_val=0;
      free_=p;
      printf("Watchpoint %d deleted\n",n);
      return true;

   }
   else
   {
      p=p->next;
      while(p!=NULL)
      {
	 if(p->NO==n)
	 {
	     p0->next=p->next;
	     p->old_val=0;
	     p->new_val=0;
	     p->next=free_;
	     free_=p;
	     printf("Watchpoint %d deleted\n",n);
	     return true;
	 }
	 p0=p0->next;
	 p=p->next;
      }
   }
   printf("Watchpoint %d is not existed\n",n);
   return false;
}
void list_watchpoint()
{
    WP *p=head;
    if(head==NULL)
    {
	 printf("没有监视点\n");
	 return;
    }
    while(p!=NULL)
    {
	printf( "NO Expr         Old Value\n");
	printf("%d %-13s 0x%08x\n",p->NO,p->expr,p->old_val);
        p=p->next;	
    }
}
