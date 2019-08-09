#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAYLENGHT 10


// --------------------------------------------------------------------------------------- OUTBOUND RELATION



typedef struct outbound_relation {
    void* relation;
    void* entity;
    struct outbound_relation* next;
}* out_rel;

out_rel newOutboundRelation(void* relation, void* entity) {
    out_rel n = malloc(sizeof(struct outbound_relation));
    n->next = NULL;
    n->entity = entity;
    n->relation = relation;
    return n;
}

void addOutboundRelation(out_rel* head, out_rel val) {
    val->next = *head;
    *head = val;
}

void delAllOutboundRelationByRelation(out_rel* head, void* relation) {
    out_rel* curr = head;
    while (*curr != NULL) {
        out_rel ele = *curr;
        if(ele->relation == relation) {
            *curr = ele->next;
            free(ele);
        } else
            curr = &(ele->next);
    }
}

void delAllOutboundRelationByEntity(out_rel* head, void* entity) {
    out_rel* curr = head;
    while (*curr != NULL) {
        out_rel ele = *curr;
        if(ele->entity == entity) {
            *curr = ele->next;
            free(ele);
        } else
            curr = &(ele->next);
    }
}



// --------------------------------------------------------------------------------------- RELATION SUMMARY



typedef struct relation_summary {
    char* rel;
    size_t count;
    struct relation_summary* next;
}* relationSummaryPtr;

typedef struct relation_element {
    relationSummaryPtr summaryPointer;
    struct relation_element* next;
}* relationElementPtr;

relationSummaryPtr initNewSummary(char* key) {
    relationSummaryPtr summary = (relationSummaryPtr)malloc(sizeof(struct relation_summary));
    summary->count = 0;
    summary->next = NULL;
    summary->rel = key;
    return summary;
}

relationSummaryPtr getSummary(relationSummaryPtr* head, char* key) {
    if (*head == NULL) {
        *head = initNewSummary(key);
        return *head;
    }
    
    relationSummaryPtr curr = *head, prec=NULL;
    
    while (curr != NULL) {
        if (curr->rel == key /*strcmp(curr->relation, key) == 0*/)
            return curr;
        
        prec = curr;
        curr = curr->next;
    }
    
    prec->next = initNewSummary(key);
    return prec->next;
}

void freeSummary(relationSummaryPtr list) {
    while(list != NULL) {
        relationSummaryPtr tmp = list;
        list = list->next;
        free(tmp);
    }
}

char* relations[ARRAYLENGHT];
static size_t relations_len = 0;

char* addRelation(char* relation) {
    for(size_t i = 0; i < relations_len; i++) {
        if(strcmp(relations[i], relation) == 0)
            return relations[i];
    }
    relations[relations_len] = malloc((strlen(relation) + 1) * sizeof(char));
    strcpy(relations[relations_len], relation);
    relations[relations_len][strlen(relation)] = 0;
    
    return relations[relations_len++];
}



// --------------------------------------------------------------------------------------- RED BLACK TREE



enum { RED, BLACK };
typedef int COLOR;

typedef struct red_black_node {
    COLOR color;
    struct red_black_node* right, * left, * parent;
}* rb_node;

typedef struct Relation_Node {
    struct red_black_node node;
    
    //extra data for relation
    char* key;
    void* entity;
    
    relationElementPtr relationList;
}* relationnode;

typedef struct Entity_Node {
    struct red_black_node node;
    
    //extra data for entity
    char* key;
    
    out_rel outbound;
    relationnode inbound;
    
    relationSummaryPtr summary;
}* entitynode;

entitynode ROOT;
rb_node NILL;

void init() {
    NILL = malloc(sizeof(struct red_black_node));
    NILL->color = BLACK;
    NILL->left = NULL;
    NILL->right = NULL;
}

void initRBnode(rb_node n) {
    n->color = RED;
    n->parent = NILL;
    n->left = NILL;
    n->right = NILL;
}

bool isNill(void* n) {
    return (rb_node)n == NILL;
}
void* getNill() {
    return NILL;
}

void* up(void* n) {
    return ((rb_node)n)->parent;
}
void* left(void* n) {
    return ((rb_node)n)->left;
}
void* right(void* n) {
    return ((rb_node)n)->right;
}

void* treeSearch(void* n, char* key, int (*cmp)(void*, char*)) {
    while (!isNill(n)) {
        if ((*cmp)(n, key) < 0)
            n = ((rb_node)n)->right;
        else if ((*cmp)(n, key) > 0)
            n = ((rb_node)n)->left;
        else
            return n;
    }
    return n;
}

void inorderTreeWalk(const rb_node n, void (*f)(void*)) {
    if (!isNill(n)) {
        inorderTreeWalk(n->left, f);
        (*f)(n);
        inorderTreeWalk(n->right, f);
    }
}

static void* treeMinimum(rb_node n) {
    while (!isNill(n->left))
        n = n->left;
    return n;
}

static void* treeSuccessor(rb_node n) {
    if (!isNill(n->right))
        return treeMinimum(n->right);
    rb_node y = n->parent;
    while (!isNill(y) && n == y->right) {
        n = y;
        y = y->parent;
    }
    return y;
}

static void leftRotate(rb_node* root, rb_node node) {
    rb_node right = node->right;
    node->right = right->left;
    if (!isNill(right->left)) {
        right->left->parent = node;
    }
    right->parent = node->parent;
    if (isNill(node->parent)) {
        *root = right;
    } else if (node == node->parent->left) {
        node->parent->left = right;
    } else {
        node->parent->right = right;
    }
    
    right->left = node;
    node->parent = right;
}


static void rightRotate(rb_node* root, rb_node node) {
    rb_node left = node->left;
    node->left = left->right;
    if (!isNill(left->right)) {
        left->right->parent = node;
    }
    
    left->parent = node->parent;
    if (isNill(node->parent)) {
        *root = left;
    } else if (node == node->parent->right) {
        node->parent->right = left;
    } else {
        node->parent->left = left;
    }
    
    left->right = node;
    node->parent = left;
}

static void insertFixUp(rb_node* root, rb_node x) {
    while(x != *root && x->parent->color == RED) {
        rb_node x_parent = x->parent;
        if (x_parent->color == RED) {
            if (x_parent == x_parent->parent->left) {
                rb_node y = x_parent->parent->right;
                if (y->color == RED) {
                    x_parent->color = BLACK;
                    y->color = BLACK;
                    x_parent->parent->color = RED;
                    x = x_parent->parent;
                } else {
                    if (x == x_parent->right) {
                        x = x_parent;
                        leftRotate(root, x);
                        x_parent = x->parent;
                    }
                    x_parent->color = BLACK;
                    x_parent->parent->color = RED;
                    rightRotate(root, x_parent->parent);
                }
            } else {
                rb_node y = x_parent->parent->left;
                if (y->color == RED) {
                    x_parent->color = BLACK;
                    y->color = BLACK;
                    x_parent->parent->color = RED;
                    x = x_parent->parent;
                } else {
                    if (x == x_parent->left) {
                        x = x_parent;
                        rightRotate(root, x);
                        x_parent = x->parent;
                    }
                    x_parent->color = BLACK;
                    x_parent->parent->color = RED;
                    leftRotate(root, x_parent->parent);
                }
            }
        }
    }
    (*root)->color = BLACK;
}

void insertNode(rb_node* root, rb_node parent, rb_node node, int (*cmp)(void*, void*)) {
    
    if (isNill(parent))
        *root = node;
    else if ((*cmp)(parent, node) > 0)
        parent->left = node;
    else
        parent->right = node;
    
    insertFixUp(root, node);
}

static void deleteFixUp(rb_node* root, rb_node x, rb_node x_parent) {
    while(x != *root && x->color == BLACK) {
        rb_node y;
        if(x == x_parent->left) {
            y = x_parent->right;
            if(y->color == RED) {
                y->color = BLACK;
                x_parent->color = RED;
                leftRotate(root, x_parent);
                y = x_parent->right;
            }
            if(y->left->color == BLACK && y->right->color == BLACK) {
                y->color = RED;
                x = x_parent;
                x_parent = x_parent->parent;
            } else {
                if(y->right->color == BLACK) {
                    y->left->color = BLACK;
                    y->color = RED;
                    rightRotate(root, y);
                    y = x_parent->right;
                }
                y->color = x_parent->color;
                x_parent->color = BLACK;
                y->right->color = BLACK;
                leftRotate(root, x_parent);
                x = *root;
            }
        } else {
            y = x_parent->left;
            if(y->color == RED) {
                y->color = BLACK;
                x_parent->color = RED;
                rightRotate(root, x_parent);
                y = x_parent->left;
            }
            if(y->left->color == BLACK && y->right->color == BLACK) {
                y->color = RED;
                x = x_parent;
                x_parent = x_parent->parent;
            } else {
                if(y->left->color == BLACK) {
                    y->right->color = BLACK;
                    y->color = RED;
                    leftRotate(root, y);
                    y = x_parent->left;
                }
                y->color = x_parent->color;
                x_parent->color = BLACK;
                y->left->color = BLACK;
                rightRotate(root, x_parent);
                x = *root;
            }
        }
    }
    x->color = BLACK;
}

static void chgParentPtr(rb_node* root, rb_node parent, rb_node old, rb_node new) {
    if(isNill(parent)) {
        if(*root == old) *root = new;
        return;
    }
    
    if(parent->left == old) parent->left = new;
    if(parent->right == old) parent->right = new;
}
static void chgChildPtr(rb_node child, rb_node old, rb_node new) {
    if(isNill(child)) return;
    
    if(child->parent == old) child->parent = new;
}

static void swapPtr(rb_node* x, rb_node* y) {
    rb_node t = *x; *x = *y; *y = t;
}
static void swapColor(rb_node x, rb_node y) {
    COLOR t = x->color; x->color = y->color; y->color = t;
}

void* deleteNode(rb_node* root, rb_node z) {
    rb_node y, x;
    
    if (isNill(z->left) || isNill(z->right))
        y = z;
    else {
        y = treeSuccessor(z);
        //equivalent of copying the value, but here we don't know the content of the node
        //but just the pointers, so we swap the two nodes
        
        //color depends on the position of the node
        swapColor(y, z);
        
        //x & y parents will point to the respective swapped child
        chgParentPtr(root, z->parent, z, y);
        if(z->right != y)
            chgParentPtr(root, y->parent, y, z);
        
        //as well their children will change parents
        chgChildPtr(y->left, y, z);
        chgChildPtr(y->right, y, z);
        chgChildPtr(z->left, z, y);
        if(z->right != y)
            chgChildPtr(z->right, z, y);
        
        //if y is the right child of x, we did not change the pointer, so we do it now
        if(z->right == y) {
            z->right = z;
            y->parent = y;
        }
        
        //swap internal node pointers
        swapPtr(&z->parent, &y->parent);
        swapPtr(&z->left, &y->left);
        swapPtr(&z->right, &y->right);
        
        //swap the pointers so that y is the one to be deleted
        swapPtr(&z, &y);
    }
    
    if (!isNill(y->left))
        x = y->left;
    else
        x = y->right;
    
    if(!isNill(x))
        x->parent = y->parent;
    
    if (isNill(y->parent))
        *root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    
    if (y->color == BLACK)
        deleteFixUp(root, x, y->parent);
    
    //unlink completely
    y->parent = getNill();
    y->left = getNill();
    y->right = getNill();
    y->color = BLACK;
    return y;
}



// --------------------------------------------------------------------------------------- RELATION TREE



int relationNodeChar_cmp(void* node, char* key) {
    return strcmp(((relationnode)node)->key, key);
}

int relation2relation_cmp(void* node1, void* node2) {
    return strcmp(((relationnode)node1)->key, ((relationnode)node2)->key);
}

void insertRelation(entitynode dest, entitynode orig, relationSummaryPtr summary) {
    
    relationnode node = dest->inbound, parent = getNill();
    
    while (!isNill(node)) {
        parent = node;
        if (strcmp(orig->key, node->key) < 0)
            node = left(node);
        else if (strcmp(orig->key, node->key) > 0)
            node = right(node);
        else {
            relationElementPtr curr = node->relationList, prec = NULL;
            while (curr != NULL) {
                if (curr->summaryPointer == summary)
                    return;
                prec = curr;
                curr = curr->next;
            }
            
            relationElementPtr relation = (relationElementPtr)malloc(sizeof(struct relation_element));
            relation->next = NULL;
            relation->summaryPointer = summary;
            prec->next = relation;
            summary->count++;
            
            return;
        }
    }
    
    relationnode z = malloc(sizeof(struct Relation_Node));
    initRBnode(&z->node);
    
    z->key = orig->key;
    z->entity = orig;
    relationElementPtr relation = (relationElementPtr)malloc(sizeof(struct relation_element));
    relation->next = NULL;
    relation->summaryPointer = summary;
    z->relationList = relation;
    z->node.parent = &parent->node;
    summary->count++;
    
    insertNode(&dest->inbound, &parent->node, &z->node, relation2relation_cmp);
    
    if(dest != orig) {
        //store the new relation into the outbound array of origin
        addOutboundRelation(&orig->outbound, newOutboundRelation(z, dest));
    }
}

relationnode deleteRelation2(relationnode* root, relationnode z, relationSummaryPtr summary) {
    relationElementPtr tmp = z->relationList, prev = NULL;
    
    if (summary != NULL) {
        while(tmp != NULL) {
            if(tmp->summaryPointer == summary) {
                if(prev == NULL)
                    z->relationList = tmp->next;
                else
                    prev->next = tmp->next;
                
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
            relationElementPtr tmp = z->relationList;
            z->relationList = z->relationList->next;
            free(tmp);
        }
    }
    if(z->relationList == NULL) {
        //a node with empty relation list does not need to be kept in memory
        return deleteNode(root, &z->node);
    }
    return getNill();
}

void deleteRelation(relationnode* root, entitynode orig, relationSummaryPtr summary)
{
    relationnode z = treeSearch(*root, orig->key, relationNodeChar_cmp);
    if (isNill(z))
        return;
    
    relationnode deleted = deleteRelation2(root, z, summary);
    
    if(!isNill(deleted)) {
        delAllOutboundRelationByRelation(&orig->outbound, deleted);
        if(deleted == *root)
            *root = getNill();
        
        free(deleted);
    }
}

void freeRelationList(relationElementPtr list) {
    while(list != NULL) {
        relationElementPtr tmp = list;
        list = list->next;
        free(tmp);
    }
}

void freeRelationNode(relationnode node) {
    free(node);
}

void destroyTreeRelation(relationnode root)
{
    if (isNill(root))
        return;
    
    destroyTreeRelation(left(root));
    freeRelationList(root->relationList);
    destroyTreeRelation(right(root));
    
    freeRelationNode(root);
}



// --------------------------------------------------------------------------------------- ENTITY TREE



int entitycmp(void* node, char* key) {
    return strcmp(((entitynode)node)->key, key);
}

int entity2entity_cmp(void* node1, void* node2) {
    return strcmp(((entitynode)node1)->key, ((entitynode)node2)->key);
}

void insertEntity(entitynode* root, char* key) {
    entitynode node = *root, parent = getNill();
    while (!isNill(node)) {
        parent = node;
        if (strcmp(key, node->key) < 0) {
            node = left(node);
        }
        else if (strcmp(key, node->key) > 0) {
            node = right(node);
        }
        else {
            //entity already existing
            return;
        }
    }
    entitynode z = malloc(sizeof(struct Entity_Node));
    initRBnode(&z->node);
    
    z->key = (char*)malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(z->key, key);
    z->summary = NULL;
    z->inbound = getNill();
    z->outbound = NULL;
    
    z->node.parent = &parent->node;
    insertNode(root, &parent->node, &z->node, entity2entity_cmp);
}

void relationCleanup(entitynode n, char* key) {
    while(n->outbound != NULL) {
        out_rel curr = n->outbound;
        entitynode e = (entitynode)curr->entity;
        relationnode r = (relationnode)curr->relation;
        
        relationnode deleted = deleteRelation2(&e->inbound, r, NULL);
        if(!isNill(deleted))
            free(deleted);
        
        n->outbound = n->outbound->next;
        free(curr);
    }
}

void inboundRelationCleanup(relationnode root, entitynode z)
{
    if (isNill(root))
        return;
    
    inboundRelationCleanup(left(root), z);
    
    entitynode e = (entitynode)root->entity;
    
    delAllOutboundRelationByEntity(&e->outbound, z);
    freeRelationList(root->relationList);
    
    inboundRelationCleanup(right(root), z);
    
    freeRelationNode(root);
}

void deleteEntity(entitynode* root, char* key) {
    entitynode z = treeSearch(*root, key, entitycmp);
    if(isNill(z))
        return;
    
    entitynode deleted = deleteNode(root, &z->node);
    //clean all outbound relations
    relationCleanup(deleted, key);
    
    //clean all inbound relations (outbounds for the others)
    inboundRelationCleanup(deleted->inbound, z);
    
    //destroyTreeRelation(deleted->root);
    freeSummary(deleted->summary);
    free(deleted->key);
    free(deleted);
}



// --------------------------------------------------------------------------------------- REPORT



typedef struct Entity_List {
    char* name;
    struct Entity_List* next;
}* entitylist;

struct Report_Summary {
    relationSummaryPtr summaryPointer;
    entitylist list;
};

struct Report_Summary report[ARRAYLENGHT];

void isort(int size) {
    struct Report_Summary key = report[size - 1];
    int i = size - 2;
    while (i >= 0 && strcmp(report[i].summaryPointer->rel, key.summaryPointer->rel) > 0) {
        report[i + 1] = report[i];
        i--;
    }
    report[i + 1] = key;
}

void exploreElement(entitynode root) {
    relationSummaryPtr curr = root->summary;
    while (curr != NULL) {
        int i = 0;
        bool found = false;
        if(curr->count > 0) {
            while (report[i].summaryPointer != NULL) {
                if (curr->rel == report[i].summaryPointer->rel) {
                    //current relation is already on my report list!
                    found = true;
                    if (curr->count > report[i].summaryPointer->count) {
                        //replace current relation number & clean the list of entities & add new first element
                        report[i].summaryPointer = curr;
                        entitylist* tmp = &(report[i].list);
                        while (*tmp != NULL) {
                            entitylist ele = *tmp;
                            *tmp = ele->next;
                            free(ele);
                        }
                        
                        entitylist new_node = malloc(sizeof(struct Entity_List));
                        new_node->name = root->key;
                        new_node->next = NULL;
                        report[i].list = new_node;
                        break;
                    }
                    else if (curr->count == report[i].summaryPointer->count) {
                        //add current entity to existing list
                        entitylist new = malloc(sizeof(struct Entity_List));
                        new->name = root->key;
                        new->next = NULL;
                        
                        if (report[i].list == NULL) {
                            report[i].list = new;
                            break;
                        }
                        entitylist curr = report[i].list;
                        while (curr->next != NULL)
                            curr = curr->next;
                        curr->next = new;
                        break;
                    }
                }
                i++;
            }
            if (!found) {
                //current relation is not in the list, need to add it in order
                report[i].summaryPointer = curr;
                report[i].list = malloc(sizeof(struct Entity_List));
                report[i].list->name = root->key;
                report[i].list->next = NULL;
                isort(i + 1);
            }
        }
        curr = curr->next;
    }
}

void createReport(entitynode root) {
    inorderTreeWalk(root, exploreElement);
}

void printReport() {
    int i = 0;
    while (report[i].summaryPointer != NULL) {
        printf("\"%s\" ", report[i].summaryPointer->rel);
        
        entitylist* curr = &report[i].list;
        while (*curr != NULL) {
            entitylist ele = *curr;
            printf("\"%s\" ", ele->name);
            *curr = ele->next;
            free(ele);
        }
        printf("%zu;", report[i].summaryPointer->count);
        report[i].list = NULL;
        report[i].summaryPointer = NULL;
        i++;
        if (report[i].summaryPointer != NULL && report[i].summaryPointer->count > 0)
            printf(" ");
        
    }
    if (i == 0)
        printf("none");
    
    printf("\n");
}



// --------------------------------------------------------------------------------------- MAIN



int main() {
    init();
    
    ROOT = getNill();
    
    char* ent1 = NULL, * ent2 = NULL, * rel = NULL;
    char c[150];
    
    while(true) {
        gets(c);
        
        //ADDENT
        if (strncmp(c, "addent", 6) == 0) {
            ent1 = strtok(c + 7, "\"");
            insertEntity(&ROOT, ent1);
        }
        
        //DELENT
        if (strncmp(c, "delent", 6) == 0) {
            ent1 = strtok(c + 7, "\"");
            deleteEntity(&ROOT, ent1);
        }
        
        //ADDREL
        if (strncmp(c, "addrel", 6) == 0) {
            ent1 = strtok(c + 7, "\"");
            ent2 = strtok(NULL, "\" \"");
            rel = strtok(NULL, "\" \"");
            
            char* relkey = addRelation(rel);
            entitynode n_orig = treeSearch(ROOT, ent1, entitycmp);
            entitynode n_dest = treeSearch(ROOT, ent2, entitycmp);
            if (!isNill(n_orig) && !isNill(n_dest)) {
                relationSummaryPtr summary = getSummary(&n_dest->summary, relkey);
                insertRelation(n_dest, n_orig, summary);
            }
        }
        
        //DELREL
        if (strncmp(c, "delrel", 6) == 0)
        {
            ent1 = strtok(c + 7, "\"");
            ent2 = strtok(NULL, "\" \"");
            rel = strtok(NULL, "\" \"");
            
            entitynode n_orig = (entitynode)treeSearch(ROOT, ent1, entitycmp);
            entitynode n_dest = (entitynode)treeSearch(ROOT, ent2, entitycmp);
            
            char* relkey = addRelation(rel);
            if (!isNill(n_orig) && !isNill(n_dest)) {
                relationSummaryPtr summary = getSummary(&n_dest->summary, relkey);
                deleteRelation(&n_dest->inbound, n_orig, summary);
            }
        }
        
        //REPORT
        if (strcmp(c, "report") == 0) {
            createReport(ROOT);
            printReport();
        }
        
        //END
        if (strcmp(c, "end") == 0) {
            break;
        }
    }
    return 0;
}
