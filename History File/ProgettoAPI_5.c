//Progetto API versione 3.0
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 60
#define RED 0
#define BLACK 1

/* Global, since all function will access them */
struct node *ROOT;
struct node *NILL;

struct node{
	char* key;
	int color;
	struct node *parent,*left,*right;
};

void left_rotate(struct node *x);
void right_rotate(struct node *x);
void tree_print(struct node *x);
void red_black_insert(char * key);
void red_black_insert_fixup(struct node *z);
struct node *tree_search(char * key);
struct node *tree_minimum(struct node *x);
void red_black_transplant(struct node *u, struct node *v);
void red_black_delete(struct node *z);
void red_black_delete_fixup(struct node *x);
void destroyTree(struct node* x);

/* Print tree keys by inorder tree walk */
void tree_print(struct node *x){
	if(x != NILL){
		tree_print(x->left);
		printf("%s,%s\t", x->key,(x->color == 1 ? "b" : "r"));
		tree_print(x->right);
	}

}

struct node *tree_search(char * key){

	struct node *x;
	x = ROOT;
	while(x != NILL && strcmp(x->key,key)!=0){
		if(strcmp(key,x->key ) < 0){
			x = x->left;
		}
		else{
			x = x->right;
		}
	}
	return x;
}

struct node *tree_minimum(struct node *x){
	while(x->left != NILL){
		x = x->left;
	}
	return x;
}

/*
 * Insertion is done by the same procedure for BST Insert. Except new node is colored
 * RED. As it is coloured RED it may violate property 2 or 4. For this reason an
 * auxilary procedure called red_black_insert_fixup is called to fix these violation.
 */

void red_black_insert(char*key){
	struct node *z, *x, *y;

	z = malloc(sizeof(struct node));

    z->key=(char*)malloc(sizeof(char)*strlen(key));
	strcpy(z->key,key);

	z->color = RED;
	z->left = NILL;
	z->right = NILL;
	x = ROOT;
	y = NILL;

	/*
	 * Go through the tree untill a leaf(NILL) is reached. y is used for keeping
	 * track of the last non-NILL node which will be z's parent.
	 */
	while(x != NILL){
		y = x;
		if(strcmp(z->key,x->key)<=0){
			x = x->left;
		}
		else{
			x = x->right;
		}
	}

	if(y == NILL){
		ROOT = z;
	}
	else if(strcmp(z->key,y->key) <=0){
		y->left = z;
	}
	else{
		y->right = z;
	}

	z->parent = y;

	red_black_insert_fixup(z);

}

/*
 * Here is the psudocode for fixing violations.
 *
 * while (z's parent is RED)
 *		if(z's parent is z's grand parent's left child) then
 *			if(z's right uncle or grand parent's right child is RED) then
 *				make z's parent and uncle BLACK
 *				make z's grand parent RED
 *				make z's grand parent new z as it may violate property 2 & 4
 *				(so while loop will contineue)
 *
 *			else(z's right uncle is not RED)
 *				if(z is z's parents right child) then
 *					make z's parent z
 *					left rotate z
 *				make z's parent's color BLACK
 *				make z's grand parent's color RED
 *				right rotate z's grand parent
 *				( while loop won't pass next iteration as no violation)
 *
 *		else(z's parent is z's grand parent's right child)
 *			do exact same thing above just swap left with right and vice-varsa
 *
 * At this point only property 2 can be violated so make root BLACK
 */

void red_black_insert_fixup(struct node *z){
	while(z->parent->color == RED){

		/* z's parent is left child of z's grand parent*/
		if(z->parent == z->parent->parent->left){

			/* z's grand parent's right child is RED */
			if(z->parent->parent->right->color == RED){
				z->parent->color = BLACK;
				z->parent->parent->right->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			}

			/* z's grand parent's right child is not RED */
			else{

				/* z is z's parent's right child */
				if(z == z->parent->right){
					z = z->parent;
					left_rotate(z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				right_rotate(z->parent->parent);
			}
		}

		/* z's parent is z's grand parent's right child */
		else{

			/* z's left uncle or z's grand parent's left child is also RED */
			if(z->parent->parent->left->color == RED){
				z->parent->color = BLACK;
				z->parent->parent->left->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			}

			/* z's left uncle is not RED */
			else{
				/* z is z's parents left child */
				if(z == z->parent->left){
					z = z->parent;
					right_rotate(z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				left_rotate(z->parent->parent);
			}
		}
	}

	ROOT->color = BLACK;

}

/*
 * Lets say y is x's right child. Left rotate x by making y, x's parent and x, y's
 * left child. y's left child becomes x's right child.
 *
 * 		x									y
 *	   / \                                 / \
 *  STA   y			----------->		  x   STC
 *		 / \							 / \
 *	  STB   STC						  STA   STB
 */

void left_rotate(struct node *x){

	struct node *y;

	/* Make y's left child x's right child */
	y = x->right;
	x->right = y->left;

	if(y->left != NILL){
		y->left->parent = x;
	}

	/* Make x's parent y's parent and y, x's parent's child */
	y->parent = x->parent;
	if(y->parent == NILL){
		ROOT = y;
	}
	else if(x == x->parent->left){
		x->parent->left = y;
	}
	else{
		x->parent->right = y;
	}

	/* Make x, y's left child & y, x's parent */
	y->left = x;
	x->parent = y;
}

/*
 * Lets say y is x's left child. Right rotate x by making x, y's right child and y
 * x's parent. y's right child becomes x's left child.
 *
 *			|											|
 *			x											y
 *		   / \										   / \
 *		  y   STA		---------------->			STB	  x
 *		 / \											 / \
 *	  STB   STC										  STC   STA
 */

void right_rotate(struct node *x){

	struct node *y;

	/* Make y's right child x's left child */
	y = x->left;
	x->left = y->right;
	if(y->right != NILL){
		y->right->parent = x;
	}

	/* Make x's parent y's parent and y, x's parent's child */
	y->parent = x->parent;
	if(y->parent == NILL){
		ROOT = y;
	}
	else if(x == x->parent->left){
		x->parent->left = y;
	}
	else{
		x->parent->right = y;
	}

	/* Make y, x's parent and x, y's child */
	y->right = x;
	x->parent = y;
}

/*
 * Deletion is done by the same mechanism as BST deletion. If z has no child, z is
 * removed. If z has single child, z is replaced by its child. Else z is replaced by
 * its successor. If successor is not z's own child, successor is replaced by its
 * own child first. then z is replaced by the successor.
 *
 * A pointer y is used to keep track. In first two case y is z. 3rd case y is z's
 * successor. So in first two case y is removed. In 3rd case y is moved.
 *
 *Another pointer x is used to keep track of the node which replace y.
 *
 * As removing or moving y can harm red-black tree properties a variable
 * yOriginalColor is used to keep track of the original colour. If its BLACK then
 * removing or moving y harm red-black tree properties. In that case an auxilary
 * procedure red_black_delete_fixup(x) is called to recover this.
 */

void red_black_delete(struct node *z){
	struct node *y, *x;
	int yOriginalColor;

	y = z;
	yOriginalColor = y->color;

	if(z->left == NILL){
		x = z->right;
		red_black_transplant(z, z->right);
	}
	else if(z->right == NILL){
		x = z->left;
		red_black_transplant(z, z->left);
	}
	else{
		y = tree_minimum(z->right);
		yOriginalColor = y->color;

		x = y->right;

		if(y->parent == z){
			x->parent = y;
		}
		else{
			red_black_transplant(y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}

		red_black_transplant(z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}

	if(yOriginalColor == BLACK){
		red_black_delete_fixup(x);
	}
}

/*
 * As y was black and removed x gains y's extra blackness.
 * Move the extra blackness of x until
 *		1. x becomes root. In that case just remove extra blackness
 *		2. x becomes a RED and BLACK node. in that case just make x BLACK
 *
 * First check if x is x's parents left or right child. Say x is left child
 *
 * There are 4 cases.
 *
 * Case 1: x's sibling w is red. transform case 1 into case 2 by recoloring
 * w and x's parent. Then left rotate x's parent.
 *
 * Case 2: x's sibling w is black, w's both children is black. Move x and w's
 * blackness to x's parent by coloring w to RED and x's parent to BLACK.
 * Make x's parent new x.Notice if case 2 come through case 1 x's parent becomes
 * RED and BLACK as it became RED in case 1. So loop will stop in next iteration.
 *
 * Case 3: w is black, w's left child is red and right child is black. Transform
 * case 3 into case 4 by recoloring w and w's left child, then right rotate w.
 *
 * Case 4: w is black, w's right child is red. recolor w with x's parent's color.
 * make x's parent BLACK, w's right child black. Now left rotate x's parent. Make x
 * point to root. So loop will be stopped in next iteration.
 *
 * If x is right child of it's parent do exact same thing swapping left<->right
 */

void red_black_delete_fixup(struct node *x){
	struct node *w;

	while(x != ROOT && x->color == BLACK){

		if(x == x->parent->left){
			w = x->parent->right;

			if(w->color == RED){
				w->color = BLACK;
				x->parent->color = RED;
				left_rotate(x->parent);
				w = x->parent->right;
			}

			if(w->left->color == BLACK && w->right->color == BLACK){
				w->color = RED;
				x->parent->color = BLACK;
				x = x->parent;
			}
			else{

				if(w->right->color == BLACK){
					w->color = RED;
					w->left->color = BLACK;
					right_rotate(w);
					w = x->parent->right;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				x->right->color = BLACK;
				left_rotate(x->parent);
				x = ROOT;

			}

		}
		else{
			w = x->parent->left;

			if(w->color == RED){
				w->color = BLACK;
				x->parent->color = BLACK;
				right_rotate(x->parent);
				w = x->parent->left;
			}

			if(w->left->color == BLACK && w->right->color == BLACK){
				w->color = RED;
				x->parent->color = BLACK;
				x = x->parent;
			}
			else{

				if(w->left->color == BLACK){
					w->color = RED;
					w->right->color = BLACK;
					left_rotate(w);
					w = x->parent->left;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				right_rotate(x->parent);
				x = ROOT;

			}
		}

	}

	x->color = BLACK;
}
/* replace node u with node v */
void red_black_transplant(struct node *u, struct node *v){
	if(u->parent == NILL){
		ROOT = v;
	}
	else if(u == u->parent->left){
		u->parent->left = v;
	}
	else{
		u->parent->right = v;
	}

	v->parent = u->parent;
}


void destroyTree(struct node *x)
{
	if (x != NILL)
    {
        destroyTree(x->left);
        destroyTree(x->right);
        free(x->key);
        free(x);
	}
}


int main(){

    NILL = malloc(sizeof(struct node));
	NILL->color = BLACK;
	ROOT = NILL;
    char c[MAX];
    char * id_ent;
    char * id_orig;
    char *id_dest;
    char *id_rel;

    int i,j;
    int cont,k,f;

    while(true)
    {
        //User input
        gets(c);

        //ADDENT
        if (strncmp(c,"addent",6) == 0)
        {
            id_ent = (char*)malloc((strlen(c)-9 )*sizeof(char));
            strncpy(id_ent, c+8, strlen(c)-9);
            id_ent[strlen(c)-9]=0;

            if(tree_search(id_ent)==NILL)
            {
                red_black_insert(id_ent);
            }

            else
                printf("Nodo esistente\n");


            free(id_ent);
            id_ent=NULL;

        }

        //DELENT
        if (strncmp(c,"delent",6) == 0)
        {
            id_ent = (char*)malloc((strlen(c)-9)*sizeof(char));
            strncpy(id_ent, c+8, strlen(c)-9);
            id_ent[strlen(c)-9]=0;
            if(tree_search(id_ent)!=NILL)
                red_black_delete(tree_search(id_ent));
            else
                printf("Il nodo non esiste\n");

            free(id_ent);
            id_ent=NULL;

        }

        //ADDREL
        if (strncmp(c,"addrel",6) == 0)
          {

            i=0,j=7;
            cont=0,k=7,f=7;

            for(i=7;i<strlen(c);i++)
            {
                if(c[i]!= 34&&cont==1)
                {
                    j++;
                    f++;
                    k++;
                }
                if(c[i]==34)
                    cont++;
                if(c[i]!= 34&&cont==3)
                {
                    f++;
                    k++;
                }

                if(c[i]!= 34&&cont==5)
                {
                    f++;
                }
            }

            id_orig = (char*)malloc((j-7)*sizeof(char));
            strncpy(id_orig, c+8, j-7);
            id_orig[j-7]=0;


            id_dest = (char*)malloc((k-j)*sizeof(char));
            strncpy(id_dest, c+j+4, k-j);
            id_dest[k-j]=0;


            id_rel= (char*)malloc((f-k)*sizeof(char));
            strncpy(id_rel, c+k+7, f-k);
            id_rel[f-k]=0;

            if(tree_search(id_orig)!=NILL &&tree_search(id_dest)!=NILL)
            {
                printf("ok\n");

            }
            else
                printf("Qualcosa non quadra!\n");

            free(id_orig);
            free(id_dest);
            free(id_rel);
        }

        //DELREL
        if (strncmp(c,"delrel",6) == 0)
         {
            i=0,j=7;
            cont=0,k=7,f=7;
            for(i=7;i<strlen(c);i++)
            {
                if(c[i]!= 34&&cont==1)
                {
                    j++;
                    f++;
                    k++;
                }
                if(c[i]==34)
                    cont++;
                if(c[i]!= 34&&cont==3)
                {
                    f++;
                    k++;
                }

                if(c[i]!= 34&&cont==5)
                {
                    f++;
                }
            }

            id_orig = (char*)malloc((j-7)*sizeof(char));
            strncpy(id_orig, c+8, j-7);
            id_orig[j-7]=0;


            id_dest = (char*)malloc((k-j)*sizeof(char));
            strncpy(id_dest, c+j+4, k-j);
            id_dest[k-j]=0;


            id_rel= (char*)malloc((f-k)*sizeof(char));
            strncpy(id_rel, c+k+7, f-k);
            id_rel[f-k]=0;
            if(tree_search(id_orig)!=NILL &&tree_search(id_dest)!=NILL)
            {
                printf("ok\n");
            }

            else
                printf("Qualcosa non quadra!\n");

            free(id_orig);
            id_orig=NULL;
            free(id_dest);
            id_dest=NULL;
            free(id_rel);
            id_rel=NULL;
        }

        //REPORT
        if(strcmp(c,"report")==0)
        {
            printf("none\n");
        }

        //END***
        if (strcmp(c,"end") == 0)
            break;
    }

    if(ROOT!=NILL)
    {
         tree_print(ROOT);
    }

    destroyTree(ROOT);
    free(NILL);
    return 0;
}
