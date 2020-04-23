#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 20

enum {RED,BLACK};
typedef int COLOR;

typedef struct Entity_Node{

    char name[MAX];
    COLOR color;
    struct Entity_Node *right,*left,*parent;

}*entitynode;

typedef entitynode* EntityTree;

entitynode initilize(entitynode,char*);
entitynode grandparent(entitynode);
entitynode uncle (entitynode);
entitynode sibling(entitynode);

entitynode findNode(EntityTree,char*);
void replaceNode(EntityTree, entitynode, entitynode);

void insertNode(EntityTree,char*);
void insertNode_FixUp(entitynode);

void removeNode(EntityTree,char*);
void removeNode_FixUp(entitynode);
void rotateRight(entitynode);
void rotateLeft(entitynode);

void destroy(entitynode);

entitynode initilize(entitynode p, char* n)
{
	entitynode tree = (entitynode)malloc(sizeof(struct Entity_Node));
	tree->left = tree->right = NULL;
	tree->parent = p;
	strcpy(tree->name,n);
	tree->color = RED;
	return tree;
}
entitynode grandparent(entitynode n)
{
	if (n == NULL || n->parent == NULL)
		return NULL;

	return n->parent->parent;
}
entitynode uncle(entitynode n)
{
	entitynode g = grandparent(n);
	if (n == NULL || g == NULL)
		return NULL;
	if (n->parent == g->left)
		return g->right;
	else
		return g->left;
}
entitynode sibling(entitynode n)
{
	if (n == n->parent->left)
		return n->parent->right;
	else
		return n->parent->left;
}
int colorOf(entitynode n)
{
	return n == NULL ? BLACK : n->color;
}
void insertNode(EntityTree t, char* v)
{

	int pl = 0;
	entitynode ptr, btr = NULL, newNode;

	for (ptr = *t; ptr != NULL;
	btr = ptr, ptr = ((pl = (strcmp(ptr->name,v)>0 )) ? ptr->left : ptr->right));

		newNode = initilize(btr, v);

	if (btr != NULL)
		(pl) ? (btr->left = newNode) : (btr->right = newNode);

	insertNode_FixUp(newNode);

	ptr = newNode;
	for (ptr = newNode; ptr != NULL; btr = ptr, ptr = ptr->parent);
	*t = btr;
}
void insertNode_FixUp(entitynode n)
{
	entitynode u = uncle(n), g = grandparent(n), p = n->parent;
	if (p == NULL)
		n->color = BLACK;
	else if (p->color == BLACK)
		return;
	else if (u != NULL && u->color == RED)
	{
		p->color = BLACK;
		u->color = BLACK;
		g->color = RED;

		insertNode_FixUp(g);
	}
	else
	{
		if (n == p->right && p == g->left)
		{
			rotateLeft(p);
			n = n->left;
		}
		else if (n == p->left && p == g->right)
		{
			rotateRight(p);
			n = n->right;
		}

		g = grandparent(n);
		p = n->parent;

		p->color = BLACK;
		g->color = RED;

		if (n == p->left)
			rotateRight(g);
		else
			rotateLeft(g);
	}
}
void replaceNode(EntityTree t, entitynode o, entitynode n)
{
	if (o->parent == NULL)
		*t = n;
	else
	{
		if (o == o->parent->left)
			o->parent->left = n;
		else
			o->parent->right = n;
	}

	if (n != NULL)
		n->parent = o->parent;
}

void removeNode(EntityTree t, char* v)
{
	entitynode n = findNode(t,v),c;

	if (n == NULL)
		return;
	if (n->left != NULL && n->right != NULL)
	{
		entitynode pred = n->left;
		while (pred->right != NULL)
			pred = pred->right;
        strcpy(n->name,pred->name);
		n = pred;
	}

	c = n->right == NULL ? n->left : n->right;
	if (n->color == BLACK)
	{
		n->color = colorOf(c);
		removeNode_FixUp(n);
	}

	replaceNode(t, n, c);
	free(n);
}

void removeNode_FixUp(entitynode n)
{
	if (n->parent == NULL)
		return;

	entitynode s = sibling(n);
	if (colorOf(s) == RED)
	{
		n->parent->color = RED;
		s->color = BLACK;
		if (n == n->parent->left)
			rotateLeft(n->parent);
		else
			rotateRight(n->parent);
	}

	s = sibling(n);
	if (colorOf(n->parent) == BLACK && colorOf(s) == BLACK &&
		colorOf(s->left) == BLACK && colorOf(s->right) == BLACK)
	{
		s->color = RED;
		removeNode_FixUp(n->parent);
	}
	else if (colorOf(n->parent) == RED && colorOf(s) == BLACK &&
		colorOf(s->left) == BLACK && colorOf(s->right) == BLACK)
	{
		s->color = RED;
		n->parent->color = BLACK;
	}
	else
	{
		if (n == n->parent->left && colorOf(s) == BLACK &&
			colorOf(s->left) == RED && colorOf(s->right) == BLACK)
		{
			s->color = RED;
			s->left->color = BLACK;
			rotateRight(s);
		}
		else if (n == n->parent->right && colorOf(s) == BLACK &&
			colorOf(s->right) == RED && colorOf(s->left) == BLACK)
		{
			s->color = RED;
			s->right->color = BLACK;
			rotateLeft(s);
		}

		s->color = colorOf(n->parent);
		n->parent->color = BLACK;
		if (n == n->parent->left)
		{
			s->right->color = BLACK;
			rotateLeft(n->parent);
		}
		else
		{
			s->left->color = BLACK;
			rotateRight(n->parent);
		}
	}
}

void rotateRight(entitynode tree)
{
	entitynode c = tree->left;
	entitynode p = tree->parent;

	if (c->right != NULL)
		c->right->parent = tree;

	tree->left = c->right;
	tree->parent = c;
	c->right = tree;
	c->parent = p;

	if (p != NULL)
	{
		if (p->right == tree)
			p->right = c;
		else
			p->left = c;
	}
	printf("rotation %s, right\n", tree->name);
}
void rotateLeft(entitynode tree)
{
	entitynode c = tree->right;
	entitynode p = tree->parent;

	if (c->left != NULL)
		c->left->parent = tree;

	tree->right = c->left;
	tree->parent = c;
	c->left = tree;
	c->parent = p;

	if (p != NULL)
	{
		if (p->left == tree)
			p->left = c;
		else
			p->right = c;
	}
	printf("rotation %s, left\n", tree->name);
}

entitynode findNode(EntityTree t, char* v)
{
    printf("%s",v);
	entitynode p=NULL;
	for (p = *t; p != NULL && (strcmp(p->name,v) !=0) ; p = ((strcmp(p->name,v)>0) ? p->left : p->right));
	return p;
}


void printNode(entitynode n)
{
	printf("%s(%s) ", n->name, (n->color == BLACK ? "b" : "r"));
}
void inorderIterator(entitynode n, void(*func)(entitynode))
{
	if (n == NULL)
		return;
	inorderIterator(n->left, func);
	func(n);
	inorderIterator(n->right, func);
}
void inorderPrint(EntityTree t)
{
	inorderIterator(*t, printNode);
	printf("\n");
}


void destroy(entitynode tree)
{
	if (tree == NULL)
		return;
	destroy(tree->left);
	destroy(tree->right);
	free(tree);
}


int main(){

    char id_orig[MAX]="";
    char id_dest[MAX]="";
    char id_rel[MAX]="";
    char id_ent[MAX]="";

    entitynode tree=NULL;
    int i,j;
    int cont,k,f;

    //Dichiarazione Variabili
    char c[MAX];
    char *buffer ;

    while(true)
    {

        //User input
        gets(c);
        buffer=(char*)malloc(strlen(c)*sizeof(char));
        assert(buffer!=NULL);
        strcpy(buffer,c);


        //ADDENT
        if (strncmp(buffer,"addent",6) == 0)
        {

            i=0,j=0;
            for(i=7;i<strlen(buffer);i++)
            {
                if(buffer[i]!= 34)
                {
                    id_ent[j]=buffer[i];
                    j++;
                }


            }
            if (findNode(&tree,id_ent)==NULL)
            {
                insertNode(&tree,id_ent);

            }

            else
                printf("esistente\n");

            id_ent[j]=0;
        }

        //DELENT
        if (strncmp(buffer,"delent",6) == 0)
        {

                i=0,j=0;
                for(i=7;i<strlen(buffer);i++)
                {
                if(buffer[i]!= 34)
                {
                    id_ent[j]=buffer[i];
                    j++;
                }

                }
                removeNode(&tree,id_ent);
                id_ent[j]=0;

        }

        //ADDREL
        if (strncmp(buffer,"addrel",6) == 0)
          {

            i=0,j=0;
            cont=0,k=0,f=0;


            for(i=7;i<strlen(buffer);i++)
            {

                if(buffer[i]!= 34&&cont==1)
                {
                    id_orig[j]=c[i];
                    j++;
                }
                if(buffer[i]==34)
                    cont++;
                if(buffer[i]!= 34&&cont==3)
                {
                    id_dest[k]=buffer[i];
                    k++;
                }

                if(buffer[i]!= 34&&cont==5)
                {
                    id_rel[f]=buffer[i];
                    f++;
                }
            }


            id_orig[j]=0;
            id_dest[k]=0;
            id_rel[f]=0;

        }


        //DELREL
        if (strncmp(buffer,"delrel",6) == 0)
         {
            i=0,j=0;
            cont=0,k=0,f=0;
            for(i=7;i<strlen(buffer);i++)
            {
                if(buffer[i]!= 34&&cont==1)
                {
                    id_orig[j]=buffer[i];
                    j++;
                }
                if(buffer[i]==34)
                    cont++;
                if(buffer[i]!= 34&&cont==3)
                {
                    id_dest[k]=c[i];
                    k++;
                }

                if(buffer[i]!= 34&&cont==5)
                {
                    id_rel[f]=buffer[i];
                    f++;
                }
            }
            id_orig[j]=0;
            id_dest[k]=0;
            id_rel[f]=0;

        }



        //REPORT
        if(strcmp(buffer,"report")==0)
        {
            if(strlen(id_rel)!=0)
            {
                printf("\"%s\" \"%s\";\n",id_rel,id_dest);

            }
            else
                printf("none\n");
        }




        //END***
        if (strcmp(buffer,"end") == 0)
            break;



    }

    //Dealloco spazio risevato a buffer
    free(buffer);
    inorderPrint(&tree);
    destroy(tree);


    return 0;
}
