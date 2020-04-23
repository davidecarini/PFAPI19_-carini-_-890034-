#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { RED, BLACK };
typedef int COLOR;

typedef struct Relation_Summary {
	char relation[25];
	int count;
	struct Relation_Summary* next;
}*relationSummary;

typedef struct Relation_List {
	relationSummary summaryPointer;
	struct Relation_List* next;
}*relationlist;

typedef struct Relation_Node {
	void* entityPointer;
	COLOR color;
	struct Relation_Node* right, * left, * parent;
	relationlist relationList;
}*relationnode;

typedef struct Entity_Node {
	char* key;
	COLOR color;
	struct Entity_Node* right, * left, * parent;
	relationnode root;
	relationSummary summary;
}*entitynode;

typedef struct Report_Summary {
	relationSummary summaryPointer;
	char** entityName;
	int len;
	struct Report_summary* next;
}*reportSummary;

entitynode NILL, ROOT;
relationnode NILLR;
reportSummary  reportlist;


///////////////////////////////////////RELATION/////////////////////////////////////////////

relationnode treeSuccessorRelation(relationnode);
relationnode treeMinimumRelation(relationnode);
void rbInsertRelation(relationnode*, entitynode, relationSummary);
void rbInsertFixUpRelation(relationnode*, relationnode);
void rbDeleteRelation(relationnode*, char*, relationSummary);
void rbDeleteFixUpRelation(relationnode*, relationnode);
void rightRotateRelation(relationnode*, relationnode);
void leftRotateRelation(relationnode*, relationnode);
relationnode treeSearchRelation(relationnode, char*);
relationSummary initNewSummary(char*);
relationSummary getSummary(relationSummary*, char*);
void inorderTreeWalkRelation(relationnode);
reportSummary inorderTreeSummary(entitynode);
 

reportSummary inorderTreeSummary(entitynode root)
{
	if (root != NILL)
	{
		inorderTreeSummary(root->left);
	
		inorderTreeSummary(root->right);
	}
}


relationSummary initNewSummary(char* key) {
	
	relationSummary summary = (relationSummary)malloc(sizeof(struct Relation_Summary));
	summary->count = 0;
	summary->next = NULL;
	strcpy(summary->relation, key);
	return summary;
}

relationSummary getSummary(relationSummary* head, char* key) {
	
	relationSummary tmp = *head;

	while (tmp != NULL) {
		if (strcmp(tmp->relation, key) == 0)
			return tmp;

		tmp = tmp->next;
	}
	tmp = initNewSummary(key);
	*head = tmp;
	return tmp;
}

relationnode treeSearchRelation(relationnode root, char* key)
{
	entitynode n = (entitynode)root->entityPointer;
	if (root == NILLR || strcmp(key, n->key) == 0)
		return root;
	if (strcmp(key, n->key) < 0)
		return treeSearchRelation(root->left, key);
	else
		return treeSearchRelation(root->right, key);
	return root;

}

void inorderTreeWalkRelation(relationnode root)
{
	relationlist node = root->relationList;


	if (root != NILLR)
	{
		inorderTreeWalkRelation(root->left);
		printf("[%s: ", ((entitynode)root->entityPointer)->key);
		while (node != NULL)
		{
			printf("%s", node->summaryPointer->relation);
			node = node->next;
			if (node != NULL)
				printf(", ");
		}
		printf("]");
		inorderTreeWalkRelation(root->right);
	}
}

relationnode treeMinimumRelation(relationnode root)
{
	while (root->left != NILLR)
	{
		root = root->left;
	}
	return root;
}


relationnode treeSuccessorRelation(relationnode root)
{
	if (root->right != NILLR)
		return treeMinimumRelation(root->right);
	relationnode y = root->parent;
	while (y != NILLR && root == y->right)
	{
		root = y;
		y = y->parent;
	}
	return y;
}


void rbInsertRelation(relationnode* root, entitynode orig, relationSummary summary) {

	relationnode z, x, y;
	x = *root;
	y = NILLR;

	while (x != NILLR) {
		y = x;
		if (strcmp(orig->key, ((entitynode)x->entityPointer)->key) < 0)
			x = x->left;
		else if (strcmp(orig->key, ((entitynode)x->entityPointer)->key) > 0)
			x = x->right;
		else
		{
			relationlist curr = x->relationList, last = NULL;
			while (curr != NULL)
			{
				if (curr->summaryPointer == summary)
					return;
				last = curr;
				curr = curr->next;
			}

			relationlist relation = (relationlist)malloc(sizeof(struct Relation_List));
			relation->next = NULL;
			relation->summaryPointer = summary;
			last->next = relation;
			summary->count++;
			
			
		}
	}
	 
	z = (relationnode)malloc(sizeof(struct Relation_Node));
	z->entityPointer = orig;
	relationlist relation = (relationlist)malloc(sizeof(struct Relation_List));
	relation->next = NULL;
	relation->summaryPointer = summary;
	z->relationList = relation;
	z->parent = y;
	summary->count++;
	

	if (y == NILLR) {
		*root = z;
	}
	else if (strcmp(((entitynode)z->entityPointer)->key, ((entitynode)y->entityPointer)->key) < 0) {
		y->left = z;
	}
	else {
		y->right = z;
	}
	z->color = RED;
	z->left = NILLR;
	z->right = NILLR;
	rbInsertFixUpRelation(root, z);
}

void rbInsertFixUpRelation(relationnode* root, relationnode z) {

	if (z == *root)
		(*root)->color = BLACK;

	else
	{
		relationnode x;
		x = z->parent;
		if (x->color == RED)
		{
			if (x == x->parent->left)
			{
				relationnode y = x->parent->right;
				if (y->color == RED)
				{
					x->color = BLACK;
					y->color = BLACK;
					x->parent->color = RED;
					rbInsertFixUpRelation(root, x->parent);
				}
				else
				{

					if (z == x->right)
					{
						z = x;
						leftRotateRelation(root, z);
						x = z->parent;
					}
					x->color = BLACK;
					x->parent->color = RED;
					rightRotateRelation(root, x->parent);
				}
			}
			else
			{
				relationnode y = x->parent->left;
				if (y->color == RED)
				{
					x->color = BLACK;
					y->color = BLACK;
					x->parent->color = RED;
					rbInsertFixUpRelation(root, x->parent);
				}
				else
				{
					if (z == x->left)
					{
						z = x;
						rightRotateRelation(root, z);
						x = z->parent;
					}
					x->color = BLACK;
					x->parent->color = RED;
					leftRotateRelation(root, x->parent);
				}
			}
		}
	}
}


void leftRotateRelation(relationnode* root, relationnode x) {

	relationnode y;

	y = x->right;
	x->right = y->left;

	if (y->left != NILLR) {
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == NILLR) {
		*root = y;
	}
	else if (x == x->parent->left) {
		x->parent->left = y;
	}
	else {
		x->parent->right = y;
	}

	y->left = x;
	x->parent = y;
}



void rightRotateRelation(relationnode* root, relationnode x) {

	relationnode y;
	y = x->left;
	x->left = y->right;
	if (y->right != NILLR) {
		y->right->parent = x;
	}

	y->parent = x->parent;
	if (x->parent == NILLR) {
		*root = y;
	}
	else if (x == x->parent->right) {
		x->parent->right = y;
	}
	else {
		x->parent->left = y;
	}

	y->right = x;
	x->parent = y;

}

void rbDeleteRelation(relationnode* root, char* key, relationSummary summary)
{
	relationnode z = treeSearchRelation(*root, key);
	if (z == NILLR)
	{
		return;
	}
		
	relationlist tmp = z->relationList, prev = NULL;

	if (summary != NULL) {
		while (tmp != NULL) {
			if (tmp->summaryPointer == summary) {
				if (prev == NULL)
					z->relationList = tmp->next;
				else
					prev = tmp->next;

				summary->count--;
			
				free(tmp);
				break;
			}

			prev = tmp;
			tmp = tmp->next;
		}
	}
	else {
		//cycle all relations and update the main summary while cleaning up memory
		while (z->relationList != NULL) {
			z->relationList->summaryPointer->count--;
			relationlist tmp = z->relationList;
			z->relationList = z->relationList->next;
			free(tmp);
		}
		
	}
	if (z->relationList == NULL) {
		//a node with empty relation list does not need to be kept in memory
		relationnode y, x;

		if (z->left == NILLR || z->right == NILLR)
			y = z;
		else
			y = treeSuccessorRelation(z);
		if (y->left != NILLR)
			x = y->left;
		else
			x = y->right;
		x->parent = y->parent;
		if (y->parent == NILLR)
			* root = x;
		else if (y == y->parent->left)
			y->parent->left = x;
		else
			y->parent->right = x;
		if (y != z)
		{
			z->entityPointer = y->entityPointer;
			z->relationList = y->relationList;
		}
		if (y->color == BLACK)
			rbDeleteFixUpRelation(root, x);
	}
}


void rbDeleteFixUpRelation(relationnode* root, relationnode n)
{
	if (n->color == RED || n->parent == NILLR)
		n->color = BLACK;
	else if (n == n->parent->left)
	{
		relationnode w = n->parent->right;
		if (w->color == RED)
		{
			w->color = BLACK;
			n->parent->color = RED;
			leftRotateRelation(root, n->parent);
			w = n->parent->right;
		}
		if (w->left->color == BLACK && w->right->color == BLACK)
		{
			w->color = RED;
			rbDeleteFixUpRelation(root, n->parent);
		}
		else
		{
			if (w->right->color == BLACK)
			{
				w->left->color = BLACK;
				w->color = RED;
				rightRotateRelation(root, w);
				w = n->parent->right;

			}
			w->color = n->parent->color;
			n->parent->color = BLACK;
			w->right->color = BLACK;
			leftRotateRelation(root, n->parent);
		}
	}
	else
	{
		relationnode w = n->parent->left;
		if (w->color == RED)
		{
			w->color = BLACK;
			n->parent->color = RED;
			rightRotateRelation(root, n->parent);
			w = n->parent->left;
		}
		if (w->left->color == BLACK && w->right->color == BLACK)
		{
			w->color = RED;
			rbDeleteFixUpRelation(root, n->parent);
		}
		else
		{
			if (w->left->color == BLACK)
			{
				w->right->color = BLACK;
				w->color = RED;
				leftRotateRelation(root, w);
				w = n->parent->left;

			}
			w->color = n->parent->color;
			n->parent->color = BLACK;
			w->left->color = BLACK;
			rightRotateRelation(root, n->parent);
		}

	}
}


entitynode treeSuccessor(entitynode);
entitynode treeMinimum(entitynode);
void inorderTreeWalk(entitynode, int);
void rbInsert(char*);
void rbInsertFixUp(entitynode);
void rbDelete(char*);
void rbDeleteFixUp(entitynode);
void rightRotate(entitynode);
void leftRotate(entitynode);
entitynode treeSearch(entitynode, char*);
void destroyTree(entitynode);
void destroyTreeRelation(relationnode);


entitynode treeSearch(entitynode x, char* key)
{
	if (x == NILL || strcmp(key, x->key) == 0)
		return x;
	if (strcmp(key, x->key) < 0)
		return treeSearch(x->left, key);
	else
		return treeSearch(x->right, key);
	return x;

}


void inorderTreeWalk(entitynode n, int h)
{
	if (n != NILL)
	{
		inorderTreeWalk(n->left, h + 1);
		inorderTreeWalkRelation(n->root);
		printf("%s\n", n->key);
		inorderTreeWalk(n->right, h + 1);
	}
}


entitynode treeMinimum(entitynode n)
{
	while (n->left != NILL)
	{
		n = n->left;
	}
	return n;
}


entitynode treeSuccessor(entitynode n)
{
	if (n->right != NILL)
		return treeMinimum(n->right);
	entitynode y = n->parent;
	while (y != NILL && n == y->right)
	{
		n = y;
		y = y->parent;
	}
	return y;
}

void rbInsert(char* key)
{
	entitynode z, x, y;
	z = malloc(sizeof(struct Entity_Node));
	z->key = (char*)malloc(sizeof(char) * (strlen(key) + 1));
	strcpy(z->key, key);
	z->summary = NULL;

	z->root = malloc(sizeof(struct Relation_Node));

	x = ROOT;
	y = NILL;
	z->root = NILLR;

	while (x != NILL) {
		y = x;
		if (strcmp(z->key, x->key) < 0) {
			x = x->left;
		}
		else {
			x = x->right;
		}
	}
	z->parent = y;

	if (y == NILL) {
		ROOT = z;
	}
	else if (strcmp(z->key, y->key) < 0) {
		y->left = z;
	}
	else {
		y->right = z;
	}
	z->color = RED;
	z->left = NILL;
	z->right = NILL;
	rbInsertFixUp(z);

}

void rbInsertFixUp(entitynode z) {

	if (z == ROOT)
		ROOT->color = BLACK;

	else
	{
		entitynode x;
		x = z->parent;
		if (x->color == RED)
		{
			if (x == x->parent->left)
			{
				entitynode y = x->parent->right;
				if (y->color == RED)
				{
					x->color = BLACK;
					y->color = BLACK;
					x->parent->color = RED;
					rbInsertFixUp(x->parent);
				}
				else
				{
					if (z == x->right)
					{
						z = x;
						leftRotate(z);
						x = z->parent;
					}
					x->color = BLACK;
					x->parent->color = RED;
					rightRotate(x->parent);
				}
			}
			else
			{
				entitynode y = x->parent->left;
				if (y->color == RED)
				{
					x->color = BLACK;
					y->color = BLACK;
					x->parent->color = RED;
					rbInsertFixUp(x->parent);
				}
				else
				{
					if (z == x->left)
					{
						z = x;
						rightRotate(z);
						x = z->parent;
					}
					x->color = BLACK;
					x->parent->color = RED;
					leftRotate(x->parent);
				}
			}
		}
	}
}


void leftRotate(entitynode x) {

	entitynode y;

	y = x->right;
	x->right = y->left;

	if (y->left != NILL) {
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == NILL) {
		ROOT = y;
	}
	else if (x == x->parent->left) {
		x->parent->left = y;
	}
	else {
		x->parent->right = y;
	}

	y->left = x;
	x->parent = y;

}


void rightRotate(entitynode x) {

	entitynode y;
	y = x->left;
	x->left = y->right;
	if (y->right != NILL) {
		y->right->parent = x;
	}

	y->parent = x->parent;
	if (x->parent == NILL) {
		ROOT = y;
	}
	else if (x == x->parent->right) {
		x->parent->right = y;
	}
	else {
		x->parent->left = y;
	}

	y->right = x;
	x->parent = y;
}

void relationCleanup(entitynode n, char* key) {
	if (n != NILL)
	{
		relationCleanup(n->left, key);
		rbDeleteRelation(&n->root, key, NULL);
		relationCleanup(n->right, key);
	}
}

void rbDelete(char* key)
{

	entitynode z = treeSearch(ROOT, key);
	if (z == NILL)
		return;
	relationCleanup(ROOT, key);


	entitynode y, x;

	if (z->left == NILL || z->right == NILL)
		y = z;
	else
		y = treeSuccessor(z);
	if (y->left != NILL)
		x = y->left;
	else
		x = y->right;
	x->parent = y->parent;
	if (y->parent == NILL)
		ROOT = x;
	else if (y == y->parent->left)
		y->parent->left = x;
	else
		y->parent->right = x;
	if (y != z) {
		strcpy(z->key, y->key);
		z->summary = y->summary;
	}

	if (y->color == BLACK)
		rbDeleteFixUp(x);

}

void rbDeleteFixUp(entitynode n)
{
	if (n->color == RED || n->parent == NILL)
		n->color = BLACK;
	else if (n == n->parent->left)
	{
		entitynode w = n->parent->right;
		if (w->color == RED)
		{
			w->color = BLACK;
			n->parent->color = RED;
			leftRotate(n->parent);
			w = n->parent->right;
		}
		if (w->left->color == BLACK && w->right->color == BLACK)
		{
			w->color = RED;
			rbDeleteFixUp(n->parent);
		}
		else
		{
			if (w->right->color == BLACK)
			{
				w->left->color = BLACK;
				w->color = RED;
				rightRotate(w);
				w = n->parent->right;

			}
			w->color = n->parent->color;
			n->parent->color = BLACK;
			w->right->color = BLACK;
			leftRotate(n->parent);
		}

	}
	else
	{
		entitynode w = n->parent->left;
		if (w->color == RED)
		{
			w->color = BLACK;
			n->parent->color = RED;
			rightRotate(n->parent);
			w = n->parent->left;
		}
		if (w->left->color == BLACK && w->right->color == BLACK)
		{
			w->color = RED;
			rbDeleteFixUp(n->parent);
		}
		else
		{
			if (w->left->color == BLACK)
			{
				w->right->color = BLACK;
				w->color = RED;
				leftRotate(w);
				w = n->parent->left;

			}
			w->color = n->parent->color;
			n->parent->color = BLACK;
			w->left->color = BLACK;
			rightRotate(n->parent);
		}

	}
}

void destroyTreeRelation(relationnode root)
{
	if (root == NILLR)
		return;
	destroyTreeRelation(root->left);
	destroyTreeRelation(root->right);
	free(root->relationList);
	free(root);

}

void destroyTree(entitynode tree)
{
	if (tree == NILL)
		return;
	destroyTreeRelation(tree->root);
	destroyTree(tree->left);
	destroyTree(tree->right);
	free(tree->key);
	free(tree);
}



/////////////////////////////////////MAIN//////////////////////////////////////////

int main() {

	NILL = malloc(sizeof(struct Entity_Node));
	NILL->color = BLACK;
	ROOT = NILL;
	NILLR = malloc(sizeof(struct Relation_Node));
	NILLR->color = BLACK;

	reportlist = malloc(sizeof(struct Report_Summary));
	reportlist->len = 0;
	reportlist->summaryPointer = NULL;

	char* id_ent;
	char* id_orig;
	char* id_dest;
	char* id_rel;
	int cont;

	while (true)
	{

		char* c = malloc(1);
		int character;
		int i = 0;
		while ((character = getchar()) != '\n' && character != EOF)
		{
			c[i++] = character;
			c = realloc(c, i + 1);
		}
		c[i] = '\0';

		//ADDENT
		if (strncmp(c, "addent", 6) == 0)
		{

			id_ent = (char*)malloc((strlen(c) - 8) * sizeof(char));
			strncpy(id_ent, c + 8, strlen(c) - 9);
			id_ent[strlen(c) - 9] = 0;


			if (treeSearch(ROOT, id_ent) == NILL)
			{
				rbInsert(id_ent);
			}

			free(id_ent);
			id_ent = NULL;
		}

		//DELENT
		if (strncmp(c, "delent", 6) == 0)
		{
			id_ent = (char*)malloc((strlen(c) - 8) * sizeof(char));
			strncpy(id_ent, c + 8, strlen(c) - 9);
			id_ent[strlen(c) - 9] = 0;
			rbDelete(id_ent);


			free(id_ent);
			id_ent = NULL;
		}

		//ADDREL

		if (strncmp(c, "addrel", 6) == 0)
		{
			id_dest = "";
			id_orig = "";
			id_rel = "";
			cont = 0;

			char* cmdstring;
			cmdstring = strtok(c, "\"");
			while (cmdstring != NULL)
			{
				switch (cont)
				{
				case 1:
					id_orig = (char*)malloc((strlen(cmdstring) + 1) * sizeof(char));
					strcpy(id_orig, cmdstring);
					id_orig[strlen(cmdstring)] = 0;
					break;

				case 3:
					id_dest = (char*)malloc((strlen(cmdstring) + 1) * sizeof(char));
					strcpy(id_dest, cmdstring);
					id_dest[strlen(cmdstring)] = 0;
					break;
				case 5:
					id_rel = (char*)malloc((strlen(cmdstring) + 1) * sizeof(char));
					strcpy(id_rel, cmdstring);
					id_rel[strlen(cmdstring)] = 0;
					break;
				}
				cmdstring = strtok(NULL, "\"");
				cont++;

			}

			entitynode n_orig = NILL, n_dest = NILL;

			n_orig = treeSearch(ROOT, id_orig);
			n_dest = treeSearch(ROOT, id_dest);
			if (n_orig != NILL && n_dest != NILL)
			{
				relationSummary summary = getSummary(&n_dest->summary, id_rel);
				rbInsertRelation(&n_dest->root, n_orig, summary);

			}
			free(id_orig);
			id_orig = NULL;
			free(id_dest);
			id_dest = NULL;
			free(id_rel);
			id_rel = NULL;
		}

		//DELREL
		if (strncmp(c, "delrel", 6) == 0)
		{
			id_dest = "";
			id_orig = "";
			id_rel = "";
			cont = 0;

			char* cmdstring;
			cmdstring = strtok(c, "\"");
			while (cmdstring != NULL)
			{
				switch (cont)
				{
				case 1:
					id_orig = (char*)malloc((strlen(cmdstring) + 1) * sizeof(char));
					strcpy(id_orig, cmdstring);
					id_orig[strlen(cmdstring)] = 0;
					break;

				case 3:
					id_dest = (char*)malloc((strlen(cmdstring) + 1) * sizeof(char));
					strcpy(id_dest, cmdstring);
					id_dest[strlen(cmdstring)] = 0;
					break;
				case 5:
					id_rel = (char*)malloc((strlen(cmdstring) + 1) * sizeof(char));
					strcpy(id_rel, cmdstring);
					id_rel[strlen(cmdstring)] = 0;
					break;
				}
				cmdstring = strtok(NULL, "\"");
				cont++;

			}

			entitynode n_orig = NILL, n_dest = NILL;

			n_orig = (entitynode)treeSearch(ROOT, id_orig);
			n_dest = (entitynode)treeSearch(ROOT, id_dest);

			if (n_orig != NILL && n_dest != NILL)
			{
				relationSummary summary = getSummary(&n_dest->summary, id_rel);
				rbDeleteRelation(&n_dest->root, id_orig, summary);

			}

			free(id_orig);
			id_orig = NULL;
			free(id_dest);
			id_dest = NULL;
			free(id_rel);
			id_rel = NULL;
		}

		//REPORT
		if (strcmp(c, "report") == 0)
		{
			
			 
			inorderTreeSummary(ROOT);
		}

		//END
		if (strcmp(c, "end") == 0)
		{
			free(c);
			break;
		}

		free(c);
	}


	if (ROOT != NILL)
		inorderTreeWalk(ROOT, 0);

	printf("\n");
	destroyTree(ROOT);

	free(NILL);
	free(NILLR);

	return 0;
}
