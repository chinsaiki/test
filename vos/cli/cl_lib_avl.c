#include "config.h"

#include "cl_lib_avl.h"

/* avl - manipulates AVL trees.
   Derived from libavl for manipulation of binary trees.
   Copyright (C) 1998-2000 Free Software Foundation, Inc.
 
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
 
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.
 
   The author may be contacted at <pfaffben@msu.edu> on the Internet,
   or as Ben Pfaff, 12167 Airport Rd, DeWitt MI 48820, USA through
   more mundane means. */

/* Creates and returns a new AVL tree.  Returns a null pointer if a
   memory allocation error occurs. */

extern int rand( void );

struct cl_lib_avl_tree *cl_lib_avl_create( void )
{
    struct cl_lib_avl_tree *tree = VOS_Malloc( sizeof( struct cl_lib_avl_tree ), MODULE_RPU_VOS );
    if ( tree == NULL )
        return NULL;
    VOS_MemZero ( ( char * ) tree,  sizeof ( struct cl_lib_avl_tree ) );
    return tree;
}

/* Searches TREE for matching key.  Returns avl node if found, a null pointer
   otherwise. */

struct cl_lib_avl_node* cl_lib_avl_search( struct cl_lib_avl_tree *tree, void *key )
{
    struct cl_lib_avl_node *node;
    int cmp_result;

    if ( tree == NULL )
    {
		VOS_ASSERT(0);
		return NULL;
    }
    node = tree->root;
    for ( ;; )
    {
        if ( node == NULL )
            return NULL;
        cmp_result = ( *tree->cmp ) ( key, node->key );
        if ( cmp_result == 0 )
            return node;
        else if ( cmp_result == 1 )
            node = node->link[ 1 ];
        else
            node = node->link[ 0 ];
    }
}

/* Allocates a new node in TREE at *NODE.  Sets the node's parent to
   PARENT, and initializes the other fields appropriately. */

static struct cl_lib_avl_node *cl_lib_avl_new_node()
{
    struct cl_lib_avl_node *node = VOS_Malloc( sizeof( struct cl_lib_avl_node ), MODULE_RPU_VOS );

    if ( node == NULL )
        return NULL;
    VOS_MemZero( ( char * ) node,  sizeof( struct cl_lib_avl_node ) );
    return node;
}

/* Inserts key into TREE.  Returns avl node that the key will be inserted,
or NULL if a memory allocation error occurred. */

struct cl_lib_avl_node* cl_lib_avl_insert( struct cl_lib_avl_tree *tree, void *key )
{
    struct cl_lib_avl_node **v, *w, *x, *y, *z;
    struct cl_lib_avl_node *pnode;
    int x_cmp_result, z_cmp_result;

    if ( tree == NULL )
    {
		VOS_ASSERT(0);
		return NULL;
    }
    v = &tree->root;
    x = z = tree->root;

    /*if tree root is NULL, insert key to root */
    if ( x == NULL )
    {
        pnode = tree->root = cl_lib_avl_new_node();
        if ( pnode == NULL )
            return NULL;
        tree->count++;
        return pnode;
    }

    /* seach key ,return avl node if key has existed,otherwise create an avl node
       and return this new node */
    for ( ;; )
    {

        int dir;
        z_cmp_result = ( *tree->cmp ) ( key, z->key );
        if ( z_cmp_result == 0 )
            return z;

        dir = ( z_cmp_result == 1 );

        /* y points to next node that will be cheched ,insert an new node if y is NULL */
        y = z->link[ dir ];
        if ( y == NULL )
        {
            pnode = y = z->link[ dir ] = cl_lib_avl_new_node();
            tree->count++;
            if ( y == NULL )
                return NULL;
            break;
        }

        /* use x to trace last passed node that bal is not zero ,
           v is a pointer which points to x */
        if ( y->bal != 0 )
        {
            v = &z->link[ dir ];
            x = y;
        }
        z = y;
    }

    /* update bal of node between x and y */
    x_cmp_result = ( *tree->cmp ) ( key, x->key );
    {
	int temindex=( x_cmp_result == 1 ) ;
       w = z = x->link[temindex];
    }
    while ( z != y )
        if ( ( *tree->cmp ) ( key, z->key ) == -1 )
        {
            z->bal = -1;
            z = z->link[ 0 ];
        }
        else
        {
            z->bal = + 1;
            z = z->link[ 1 ];
        }

    /* update bal of node x,here has two switch */

    /* when y lies in left subtree of x ,rotate and adjust */
    if ( x_cmp_result == -1 )
    {
        if ( x->bal != -1 )
            x->bal--;
        else if ( w->bal == -1 )
        {
            *v = w;
            x->link[ 0 ] = w->link[ 1 ];
            w->link[ 1 ] = x;
            x->bal = w->bal = 0;
        }
        else
        {
            VOS_ASSERT( w->bal == + 1 );
            *v = z = w->link[ 1 ];
            w->link[ 1 ] = z->link[ 0 ];
            z->link[ 0 ] = w;
            x->link[ 0 ] = z->link[ 1 ];
            z->link[ 1 ] = x;
            if ( z->bal == -1 )
            {
                x->bal = 1;
                w->bal = 0;
            }
            else if ( z->bal == 0 )
                x->bal = w->bal = 0;
            else
            {
                VOS_ASSERT( z->bal == + 1 );
                x->bal = 0;
                w->bal = -1;
            }
            z->bal = 0;
        }
    }

    /* when y lies in right subtree of x ,rotate and adjust */
    else
    {
        if ( x->bal != + 1 )
            x->bal++;
        else if ( w->bal == + 1 )
        {
            *v = w;
            x->link[ 1 ] = w->link[ 0 ];
            w->link[ 0 ] = x;
            x->bal = w->bal = 0;
        }
        else
        {
            VOS_ASSERT( w->bal == -1 );
            *v = z = w->link[ 0 ];
            w->link[ 0 ] = z->link[ 1 ];
            z->link[ 1 ] = w;
            x->link[ 1 ] = z->link[ 0 ];
            z->link[ 0 ] = x;
            if ( z->bal == + 1 )
            {
                x->bal = -1;
                w->bal = 0;
            }
            else if ( z->bal == 0 )
                x->bal = w->bal = 0;
            else
            {
                VOS_ASSERT( z->bal == -1 );
                x->bal = 0;
                w->bal = 1;
            }
            z->bal = 0;
        }
    }
    return pnode;
}

/* Deletes an avl node matching key from TREE.  Returns 1 if such an
   node was deleted, 0 if none was found. */

int cl_lib_avl_delete( struct cl_lib_avl_tree *tree, void* key )
{
    struct cl_lib_avl_node *ap[ 32 ];
    int ad[ 32 ];
    int z_cmp_result;
    int k = 1;

    struct cl_lib_avl_node **y, *z;

    if ( tree == NULL )
    {
		VOS_ASSERT(0);
		return 0;
    }

    ad[ 0 ] = 0;

    /*  ap[0] = (struct cl_lib_avl_node *) tree;*/
    ap[ 0 ] = ( struct cl_lib_avl_node * ) &tree->root;

    /* search the key, save passed node to ap[32] and direction to ad[32] */
    z = tree->root;
    for ( ;; )
    {
        int dir;
        if ( z == NULL )
            return 0;
        z_cmp_result = ( *tree->cmp ) ( key, z->key );
        if ( z_cmp_result == 0 )
            break;
        dir = ( z_cmp_result == 1 );
        ap[ k ] = z;
        ad[ k++ ] = dir;
        z = z->link[ dir ];
    }

    tree->count--;
    y = &ap[ k - 1 ] ->link[ ad[ k - 1 ] ];
    if ( z->link[ 1 ] == NULL )
        *y = z->link[ 0 ];
    else
    {
        struct cl_lib_avl_node *x = z->link[ 1 ];
        if ( x->link[ 0 ] == NULL )
        {
            x->link[ 0 ] = z->link[ 0 ];
            *y = x;
            x->bal = z->bal;
            ad[ k ] = 1;
            ap[ k++ ] = x;
        }
        else
        {
            struct cl_lib_avl_node *w = x->link[ 0 ];
            int j = k++;

            ad[ k ] = 0;
            ap[ k++ ] = x;
            while ( w->link[ 0 ] != NULL )
            {
                x = w;
                w = x->link[ 0 ];
                ad[ k ] = 0;
                ap[ k++ ] = x;
            }
            ad[ j ] = 1;
            ap[ j ] = w;
            w->link[ 0 ] = z->link[ 0 ];
            x->link[ 0 ] = w->link[ 1 ];
            w->link[ 1 ] = z->link[ 1 ];
            w->bal = z->bal;
            *y = w;
        }
    }

    /* free avl node  */
    VOS_Free( z );

    /* adjust bal of node,give stack top node to w while stack top has avl node */
    VOS_ASSERT( k > 0 );
    while ( --k )
    {
        struct cl_lib_avl_node * w, *x;

        w = ap[ k ];

        /* ad[k] is 0, delete avl node from left subtree of w */
        if ( ad[ k ] == 0 )
        {
            if ( w->bal == -1 )
            {
                w->bal = 0;
                continue;
            }
            else if ( w->bal == 0 )
            {
                w->bal = 1;
                break;
            }

            VOS_ASSERT( w->bal == + 1 );

            x = w->link[ 1 ];
            VOS_ASSERT( x != NULL );

            if ( x->bal > -1 )
            {
                w->link[ 1 ] = x->link[ 0 ];
                x->link[ 0 ] = w;
                ap[ k - 1 ] ->link[ ad[ k - 1 ] ] = x;
                if ( x->bal == 0 )
                {
                    x->bal = -1;
                    break;
                }
                else
                    w->bal = x->bal = 0;
            }
            else
            {
                VOS_ASSERT( x->bal == -1 );
                z = x->link[ 0 ];
                x->link[ 0 ] = z->link[ 1 ];
                z->link[ 1 ] = x;
                w->link[ 1 ] = z->link[ 0 ];
                z->link[ 0 ] = w;
                if ( z->bal == + 1 )
                {
                    w->bal = -1;
                    x->bal = 0;
                }
                else if ( z->bal == 0 )
                    w->bal = x->bal = 0;
                else
                {
                    VOS_ASSERT( z->bal == -1 );
                    w->bal = 0;
                    x->bal = + 1;
                }
                z->bal = 0;
                ap[ k - 1 ] ->link[ ad[ k - 1 ] ] = z;
            }
        }

        /* ad[k] is 1, delete avl node from right subtree of w */
        else
        {
            VOS_ASSERT( ad[ k ] == 1 );
            if ( w->bal == + 1 )
            {
                w->bal = 0;
                continue;
            }
            else if ( w->bal == 0 )
            {
                w->bal = -1;
                break;
            }

            VOS_ASSERT( w->bal == -1 );

            x = w->link[ 0 ];
            VOS_ASSERT( x != NULL );

            if ( x->bal < + 1 )
            {
                w->link[ 0 ] = x->link[ 1 ];
                x->link[ 1 ] = w;
                ap[ k - 1 ] ->link[ ad[ k - 1 ] ] = x;
                if ( x->bal == 0 )
                {
                    x->bal = + 1;
                    break;
                }
                else
                    w->bal = x->bal = 0;
            }
            else if ( x->bal == + 1 )
            {
                z = x->link[ 1 ];
                x->link[ 1 ] = z->link[ 0 ];
                z->link[ 0 ] = x;
                w->link[ 0 ] = z->link[ 1 ];
                z->link[ 1 ] = w;
                if ( z->bal == -1 )
                {
                    w->bal = 1;
                    x->bal = 0;
                }
                else if ( z->bal == 0 )
                    w->bal = x->bal = 0;
                else
                {
                    VOS_ASSERT( z->bal == 1 );
                    w->bal = 0;
                    x->bal = -1;
                }
                z->bal = 0;
                ap[ k - 1 ] ->link[ ad[ k - 1 ] ] = z;
            }
        }
    }

    return 1;
}

/* Helper function for avl_walk().  Recursively prints data from each
   node in tree rooted at NODE in in-order. */

/*
static void avl_call_walk(const struct cl_lib_avl_node *node)
{
  if (node == NULL)
    return;
  avl_call_walk(node->link[0]);
  VOS_Printf("%d  ", *((int *)(node->eventlist->head->data)));   
  avl_call_walk(node->link[1]);
}
*/

/* Prints all the data keys in TREE in in-order. */

/*
void avl_walk(const struct cl_lib_avl_tree *tree)
{
  VOS_ASSERT(tree != NULL);
  avl_call_walk(tree->root);
}
*/

/* Prints all the data keys in TREE in in-order, using an iterative
   algorithm. */

/*
void avl_traverse(const struct cl_lib_avl_tree *tree)
{
  struct cl_lib_avl_node *stack[32];
  int count;
  struct cl_lib_avl_node *node;
  int n;
 
  VOS_ASSERT(tree != NULL);
  count = 0;
  node = tree->root;
  for (;;) {
    while (node != NULL) {
      stack[count++] = node;
      node = node->link[0];
    }
    if (count == 0)
      return;
    node = stack[--count];
   n=*((int *)(node->eventlist->head->data));
    node = node->link[1];
  }
}
*/

/* Destroys tree rooted at NODE. */

static void cl_lib_avl_node_subtree_destroy( struct cl_lib_avl_node *node )
{
    if ( node == NULL )
        return ;
    cl_lib_avl_node_subtree_destroy( node->link[ 0 ] );
    cl_lib_avl_node_subtree_destroy( node->link[ 1 ] );
    VOS_Free( node );
}


/* Destroys TREE. */
void cl_lib_avl_destroy( struct cl_lib_avl_tree *tree )
{
    if ( tree == NULL )
    {
		VOS_ASSERT(0);
		return ;
    }
    cl_lib_avl_node_subtree_destroy( tree->root );
    VOS_Free( tree );
}

/* Returns the number of avl nodes in TREE. */
int cl_lib_avl_count( const struct cl_lib_avl_tree *tree )
{
    if ( tree == NULL )
    {
		VOS_ASSERT(0);
		return 0;
    }
    return tree->count;
}

/* Return avl node that key is minimal, NULL if tree root is NULL */
struct cl_lib_avl_node* cl_lib_avl_get_min_node( struct cl_lib_avl_tree *tree )
{
    struct cl_lib_avl_node *stack[ 32 ];
    int count;
    struct cl_lib_avl_node *node;

    if ( tree == NULL )
    {
		VOS_ASSERT(0);
		return NULL;
    }
    count = 0;
    node = tree->root;
    while ( node != NULL )
    {
        stack[ count++ ] = node;
        node = node->link[ 0 ];
    }
    if ( count == 0 )
        return NULL;
    node = stack[ --count ];
    return node;
}

/* Print the structure of node NODE of an AVL tree, which is LEVEL
   levels from the top of the tree.  Uses different delimiters to
   visually distinguish levels. */
/*
void print_structure(struct cl_lib_avl_node *node, int level)
{
VOS_ASSERT(level <= 100);
if (node == NULL) {
 VOS_Printf(" nil");
 return;
}

VOS_Printf(" %c%d", "([{`/"[level % 5], node->data);
if (node->link[0] || node->link[1]) {
 print_structure(node->link[0], level + 1);
 if (node->link[1])
   print_structure(node->link[1], level + 1);
}
VOS_Printf("%c", ")]}'\\"[level % 5]);
}
*/

/* Examine NODE in an AVL tree.  *COUNT is increased by the number of
   nodes in the tree, including the current one.  If the node is the
   root of the tree, PARENT should be INT_MIN, otherwise it should be
   the parent node value.  If this node is a left child of its parent
   node, DIR should be -1.  Otherwise, if it is not the root, it is a
   right child and DIR must be +1.  Sets *OKAY to 0 if an error is
   found.  Returns the height of the tree rooted at NODE. */ 
/*
int
recurse_tree(struct cl_lib_avl_node *node, int *count, int parent, int dir, int *okay)
{
  int lh, rh;
  if (node == NULL)
    return 0;
 
  VOS_ASSERT(count != NULL);
  (*count)++;
  lh = recurse_tree(node->link[0], count, node->data, -1, okay);
  rh = recurse_tree(node->link[1], count, node->data, +1, okay);
  if (rh - lh != node->bal) {
   VOS_Printf(" Node %d is unbalanced: right height=%d, left height=%d, "
	   "difference=%d, but balance factor=%d.\n", node->data,
	   rh, lh, rh - lh, node->bal);
    *okay = 0;
  }
  else if (*okay && (node->bal < -1 || node->bal > +1)) {
    VOS_Printf(" Node %d has balance factor %d not between -1 and +1.\n",
	   node->data, node->bal);
    *okay = 0;
  }
 
  if (parent != INT_MIN) {
    VOS_ASSERT(dir == -1 || dir == +1);
    if (dir == -1 && node->data > parent) {
     VOS_Printf(" Node %d is smaller than its left child %d.\n",
	     parent, node->data);
      *okay = 0;
    }
    else if (dir == +1 && node->data < parent) {
     VOS_Printf(" Node %d is larger than its right child %d.\n",
	     parent, node->data);
      *okay = 0;
    }
  }
 
  return 1 + (lh > rh ? lh : rh);
}
*/ 
/* Checks that TREE's structure is kosher and verifies that the values
   in ARRAY are actually in the tree.  There must be as many elements
   in ARRAY as there are nodes in the tree.  Exits the program if an
   error was encountered. */ 
/*
 
void verify_tree(struct cl_lib_avl_tree *tree, void array[])
{
  int count = 0;
  int okay = 1;
 
  recurse_tree(tree->root, &count, INT_MIN, 0, &okay);
  if (count != tree->count) {
    VOS_Printf(" Tree has %d nodes, but tree count is %d.\n", count, tree->count); 
    okay = 0;
  }
 
  if (okay) {
    int i;
    for (i = 0; i < tree->count; i++)
      if (!cl_lib_avl_search(tree, &array[i])) {
	VOS_Printf("Tree does not contain expected value %d.\n", array[i]);
	okay = 0;
      }
  }
 
  if (!okay) {
    VOS_Printf("Error(s) encountered, aborting execution.\n");
    exit(EXIT_FAILURE);
  }
}
*/

/* Arrange the N elements of ARRAY in random order.
   this function is used for testing */

void cl_lib_shuffle( int array[], int n )
{
    int i;
    for ( i = 0; i < n; i++ )
    {
        int j = i + rand() % ( n - i );
        int t = array[ j ];
        array[ j ] = array[ i ];
        array[ i ] = t;
    }
}

/* Arrange the N elements of ARRAY in random order.
   this function is used for testing */

void cl_lib_test_shuffle( int array1[], int array2[], int n )
{
    int i;
    for ( i = 0;i < n;i++ )
    {
        int j = i + rand() % ( n - i );
        int t1 = array1[ j ];
        int t2 = array2[ j ];
        array1[ j ] = array1[ i ];
        array2[ j ] = array2[ i ];
        array1[ i ] = t1;
        array2[ i ] = t2;
    }
}


/* Choose and display an initial random seed.
   Based on code by Lawrence Kirby <fred@genesis.demon.co.uk>. */

/*
void randomize(int argc, char **argv)
{
  unsigned seed;
 
   Obtain a seed value from the command line if provided, otherwise
     from the computer's realtime clock. 
  if (argc < 2) {
    time_t timeval = time(NULL);
    unsigned char *ptr = (unsigned char *) &timeval;
    unsigned long i;
 
    seed = 0;
    for (i = 0; i < sizeof timeval; i++)
      seed = seed * (UCHAR_MAX + 2U) + ptr[i];
  }
  else
    seed = strtoul(argv[1], NULL, 0);
 
    It is more convenient to deal with small seed values when
     debugging by hand. 
  seed %= 32768;
 
  VOS_Printf("Seed value = %d\n", seed);
  srand(seed);
}
*/

/* Simple stress test procedure for the AVL tree routines.  Does
  the following:

  * Generate a random number seed.  By default this is generated from
  the current time.  You can also pass an integer seed value on the
  command line if you want to test the same case.  The seed value is
  displayed.

  * Create a tree and insert the integers from 0 up to TREE_SIZE - 1
  into it, in random order.  Verifies and displays the tree structure
  after each insertion.
  
  * Removes each integer from the tree, in a different random order.
  Verifies and displays the tree structure after each deletion.

  * Destroys the tree.

  This is pretty good test code if you write some of your own AVL
  tree routines.  If you do so you will probably want to modify the
  code below so that it increments the random seed and goes on to new
  test cases automatically. */

/* Size of tree used for testing. */

/*
int main(int argc, char **argv)
{
  int array[TREE_SIZE];
  struct cl_lib_avl_tree *tree;
  int i;
 
 
  for (i = 0; i < TREE_SIZE; i++)
    array[i] = i;
  cl_lib_shuffle(array, TREE_SIZE);
  tree = cl_lib_avl_create();
 
  for (i = 0; i < TREE_SIZE; i++) {
    int result = cl_lib_avl_insert(tree, &array[i]);
    if (result != 1) {
      VOS_Printf("Error %d inserting element %d, %d, into tree.\n",
	     result, i, array[i]);
      exit(EXIT_FAILURE);
    }
    VOS_Printf("Inserted %d: ", array[i]);
 
  avl_traverse(tree);
 
    putchar('\n');
  }
 
  cl_lib_shuffle(array, TREE_SIZE);
 
  for (i = 0; i < TREE_SIZE; i++) {
    if (cl_lib_avl_delete(tree, &array[i]) == 0) {
      VOS_Printf("Error removing element %d, %d, from tree.\n", i, array[i]);
      exit(EXIT_FAILURE);
    }
    VOS_Printf("Removed %d: ", array[i]);
 
	 avl_traverse(tree);
     putchar('\n');
  }
  cl_lib_avl_destroy(tree);
  VOS_Printf("Success!\n");
  return EXIT_SUCCESS;
}
*/

/*
void main()
{ int i;
int array[5]={1,1,1,1,1};
struct cl_lib_avl_tree *tree=cl_lib_avl_create();
for(i=0;i<5;i++)
cl_lib_avl_insert(tree,array[i]);
avl_call_walk(tree->root);
}*/ 
/*
int test_time_used(void)
{
  return EXIT_SUCCESS;
}
*/

/* this function is used for testing avl tree
struct cl_lib_thread *avl_test_delete(struct cl_lib_avl_tree *tree, struct cl_lib_thread *thread)
{
  struct cl_lib_avl_node *ap[32];
  int ad[32];
  int k = 1;
  struct cl_lib_thread_list *threadlist;
  struct cl_lib_avl_node **y, *z;
  
  VOS_ASSERT(tree != NULL);
 
  ad[0] = 0;
 
  ap[0] = (struct cl_lib_avl_node *)&tree->root;
 
  z = tree->root;
  for (;;) {
    int dir;
    if (z == NULL)
      return NULL;
    if((*tree->cmp)(&thread->u.sands,z->key)==0)
      break;
    dir = ((*tree->cmp)(&thread->u.sands,z->key)==1);
    ap[k] = z;
    ad[k++] = dir;
    z = z->link[dir];
  }
  
   tree->count--;
  y = &ap[k - 1]->link[ad[k - 1]];
  if (z->link[1] == NULL)
    *y = z->link[0];
  else {
    struct cl_lib_avl_node *x = z->link[1];
    if (x->link[0] == NULL) {
      x->link[0] = z->link[0];
      *y = x;
      x->bal = z->bal;
      ad[k] = 1;
      ap[k++] = x;
    }
    else {
      struct cl_lib_avl_node *w = x->link[0];
      int j = k++;
 
      ad[k] = 0;
      ap[k++] = x;
      while (w->link[0] != NULL) {
	x = w;
	w = x->link[0];
	ad[k] = 0;
	ap[k++] = x;
      }
      ad[j] = 1;
      ap[j] = w;
      w->link[0] = z->link[0];
      x->link[0] = w->link[1];
      w->link[1] = z->link[1];
      w->bal = z->bal;
      *y = w;
    }
  }
  threadlist=(struct cl_lib_thread_list *)z->data;
  cl_lib_thread_list_delete(threadlist,thread);
  if(threadlist->head)
  	return thread;
  VOS_Free(z->key);
  VOS_Free(threadlist);
  VOS_Free(z);
  VOS_ASSERT(k > 0);
  while (--k) {
    struct cl_lib_avl_node *w, *x;
 
    w = ap[k];
 
    if (ad[k] == 0) {
      if (w->bal == -1) {
	w->bal = 0;
	continue;
      }
      else if (w->bal == 0) {
	w->bal = 1;
	break;
      }
 
      VOS_ASSERT(w->bal == +1);
 
      x = w->link[1];
      VOS_ASSERT(x != NULL);
 
      if (x->bal > -1) {
	w->link[1] = x->link[0];
	x->link[0] = w;
	ap[k - 1]->link[ad[k - 1]] = x;
	if (x->bal == 0) {
	  x->bal = -1;
	  break;
	}
	else
	  w->bal = x->bal = 0;
      }
      else {
	VOS_ASSERT(x->bal == -1);
	z = x->link[0];
	x->link[0] = z->link[1];
	z->link[1] = x;
	w->link[1] = z->link[0];
	z->link[0] = w;
	if (z->bal == +1) {
	  w->bal = -1;
	  x->bal = 0;
	}
	else if (z->bal == 0)
	  w->bal = x->bal = 0;
	else {
	  VOS_ASSERT(z->bal == -1);
	  w->bal = 0;
	  x->bal = +1;
	}
	z->bal = 0;
	ap[k - 1]->link[ad[k - 1]] = z;
      }
    }
    
    else {
      VOS_ASSERT(ad[k] == 1);
      if (w->bal == +1) {
	w->bal = 0;
	continue;
      }
      else if (w->bal == 0) {
	w->bal = -1;
	break;
      }
 
      VOS_ASSERT(w->bal == -1);
 
      x = w->link[0];
      VOS_ASSERT(x != NULL);
 
      if (x->bal < +1) {
	w->link[0] = x->link[1];
	x->link[1] = w;
	ap[k - 1]->link[ad[k - 1]] = x;
	if (x->bal == 0) {
	  x->bal = +1;
	  break;
	}
	else
	  w->bal = x->bal = 0;
      }
      else if (x->bal == +1) {
	z = x->link[1];
	x->link[1] = z->link[0];
	z->link[0] = x;
	w->link[0] = z->link[1];
	z->link[1] = w;
	if (z->bal == -1) {
	  w->bal = 1;
	  x->bal = 0;
	}
	else if (z->bal == 0)
	  w->bal = x->bal = 0;
	else {
	  VOS_ASSERT(z->bal == 1);
	  w->bal = 0;
	  x->bal = -1;
	}
	z->bal = 0;
	ap[k - 1]->link[ad[k - 1]] = z;
      }
    }
  }
 
  return thread;
}
*/

