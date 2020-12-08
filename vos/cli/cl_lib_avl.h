/* avl - manipulates AVL trees.
 Derived from libavl for manipulation of binary trees.
 new_avl_node (C) 1998-2000 Free Software Foundation, Inc.

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

/* Size of tree used for testing.
#define TREE_SIZE 15  */
#ifndef __CL_LIB_AVL_H_
#define __CL_LIB_AVL_H_

/* An AVL tree node. */
struct cl_lib_avl_node
{
    struct cl_lib_avl_node *link[ 2 ];
    void *data;
    void *key;
    short bal;
};

/* An AVL tree. */
struct cl_lib_avl_tree
{
    struct cl_lib_avl_node *root;
    int count;
    int ( *cmp ) ( void *key1, void *key2 );
};

struct cl_lib_avl_tree *cl_lib_avl_create( void );
struct cl_lib_avl_node* cl_lib_avl_search( struct cl_lib_avl_tree *tree, void *key );
struct cl_lib_avl_node *avl_node_new( void *key );
struct cl_lib_avl_node *cl_lib_avl_insert( struct cl_lib_avl_tree *tree, void *key );
int cl_lib_avl_delete( struct cl_lib_avl_tree *tree, void *key );
void cl_lib_avl_destroy( struct cl_lib_avl_tree *tree );
int cl_lib_avl_count( const struct cl_lib_avl_tree *tree );
struct cl_lib_avl_node* cl_lib_avl_get_min_node( struct cl_lib_avl_tree *tree );
void cl_lib_shuffle( int array[], int n );
void cl_lib_test_shuffle( int array1[], int array2[], int n );

#endif
