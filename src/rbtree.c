#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>

// 새로운 트리 생성
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  if (p == NULL) {
    return NULL;
  }

  node_t *nil_node = (node_t *)malloc(sizeof(node_t));

  p->nil = nil_node;
  nil_node->color = RBTREE_BLACK;
  p->root = nil_node;

  return p;
}

void search_delete(rbtree *t, node_t *node) {
  if (node == t->nil) {
    return;
  }

  search_delete(t, node->left);
  search_delete(t, node->right);
  free(node);
  node = NULL;
}

void delete_rbtree(rbtree *t) {
  if (t == NULL) {
    return;
  }

  if (t->root != t->nil) {
    search_delete(t, t->root);
  }
  free(t->nil);
  t->nil = NULL;
  free(t);
  t = NULL;
}

// 왼쪽으로 회전
void left_rotate(rbtree *t, node_t *x) {
  node_t *y;

  y = x->right;
  x->right = y->left;

  if (y->left != t->nil) {
    y->left->parent = x;
  }
  y->parent = x->parent;

  // 루트 노드였을 경우
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}

// 오른쪽으로 회전
void right_rotate(rbtree *t, node_t *x) {
  node_t *y;

  y = x->left;
  x->left = y->right;

  if (y->right != t->nil) {
    y->right->parent = x;
  }
  y->parent = x->parent;

  // 루트 노드였을 경우
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->right) {
    x->parent->right = y;
  } else {
    x->parent->left = y;
  }

  y->right = x;
  x->parent = y;
}

// 삽입 이후에 fix
void rb_insert_fixup(rbtree *t, node_t *node) {
  node_t *uncle;
  while (node->parent->color == RBTREE_RED) {
    // 새로운 노드의 부모가 조부모의 왼쪽노드일 때
    if (node->parent == node->parent->parent->left) {
      uncle = node->parent->parent->right;

      // # case1. 삼촌의 색깔이 빨간색일 때
      if (uncle->color == RBTREE_RED) {
        node->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;

        node->parent->parent->color = RBTREE_RED;
        node = node->parent->parent;
      }

      // 삼촌의 색깔이 검은색일 때
      else {
        // #case2. 부모의 오른쪽일 때
        if (node == node->parent->right) {
          node = node->parent;
          left_rotate(t, node);
        }

        // #case3. 부모의 왼쪽일 때
        node->parent->color = RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        right_rotate(t, node->parent->parent);
      }
    }

    // 새로운 노드의 부모가 조부모의 오른쪽 노드일 때
    else {
      uncle = node->parent->parent->left;

      // # case1. 삼촌의 색깔이 빨간색일 때
      if (uncle->color == RBTREE_RED) {
        node->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;

        node->parent->parent->color = RBTREE_RED;
        node = node->parent->parent;
      }

      // 삼촌의 색깔이 검은색일 때
      else {
        // #case2. 부모의 오른쪽일 때
        if (node == node->parent->left) {
          node = node->parent;
          right_rotate(t, node);
        }

        // #case3. 부모의 왼쪽일 때
        node->parent->color = RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        left_rotate(t, node->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
  // t->root->parent = t->nil;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *parent, *curr, *new_node;

  parent = t->nil;
  curr = t->root;

  new_node = (node_t *)calloc(1, sizeof(node_t));

  while (curr != t->nil) {
    parent = curr;
    // 새로운 루트의 키가 현 루트의 키보다 작다
    // 이때는 왼쪽 서브 트리로 이동한다
    if (key < curr->key) {
      curr = curr->left;
    }
    // 새로운 루트의 키가 현 루트의 키보다 크다
    // 이때는 오른쪽 서브 트리로 이동한다
    else {
      curr = curr->right;
    }
  }

  new_node->parent = parent;

  // 새로운 노드가 루트노드 일 때
  if (parent == t->nil) {
    t->root = new_node;
  } else if (key < parent->key) {
    parent->left = new_node;
  } else {
    parent->right = new_node;
  }

  new_node->key = key;
  new_node->color = RBTREE_RED;
  new_node->left = t->nil;
  new_node->right = t->nil;
  rb_insert_fixup(t, new_node);

  return t->root;
}

// key에 해당하는 node를 반환
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *curr = t->root;

  // 루트의 값이 nil이 아닐 때까지 탐색한다
  while (curr != t->nil && curr != NULL) {
    // 루트의 값이 찾고자하는 키의 값보다 작다
    // 오른쪽 서브 트리로 이동한다
    if (curr->key < key) {
      curr = curr->right;
    }
    // 루트의 값이 찾고자하는 키의 값보다 크다
    // 왼쪽 서브 트리로 이동
    else if (curr->key > key) {
      curr = curr->left;
    }
    // 루트의 값이 찾고자 하는 값이다
    else {
      return curr;
    }
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *curr = t->root;

  if (curr == t->nil) {
    return NULL;
  }

  // 그 다음 왼쪽 자식이 없다.(즉 해당 Curr 노드가 가장 작은 노드이다)
  while (curr->left != t->nil) {
    curr = curr->left;
  }

  return curr;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *curr = t->root;

  if (curr == t->nil) {
    return NULL;
  }

  // 그 다음 오른쪽 자식이 없다.(즉 해당 Curr 노드가 가장 큰 노드이다)
  while (curr->right != t->nil) {
    curr = curr->right;
  }

  return curr;
}

// 선임자를 찾는다
node_t *find_successor(rbtree *t, node_t *curr) {
  node_t *tmp = curr;

  if (curr == t->nil) {
    return NULL;
  }

  while (tmp->left != t->nil) {
    tmp = tmp->left;
  }

  return tmp;
}

// 자식 노드를 찾는다
node_t *find_child(rbtree *t, node_t *curr) {
  // case1. 자식이 하나도 없다면
  if (curr->left == t->nil && curr->right == t->nil) {
    return t->nil;
  }

  // case2. 왼쪽 자식만 있는 경우
  else if (curr->left != t->nil && curr->right == t->nil) {
    return curr->left;
  }

  // case3. 오른쪽 자식만 있는 경우
  else if (curr->left == t->nil && curr->right != t->nil) {
    return curr->right;
  }

  // case4. 왼쪽 오른쪽 두 자식이 모두 있는 경우
  else {
    return find_successor(t, curr->right);
  }
}

void transplant(rbtree *t, node_t *deleted_node, node_t *replaced_node) {
  // case1. 바꾸기 전의 노드가 루트 노드
  if (deleted_node->parent == t->nil) {
    t->root = replaced_node;
  }

  // case2. 부모의 왼쪽 자식이였을 경우
  else if (deleted_node == deleted_node->parent->left) {
    deleted_node->parent->left = replaced_node;
  }

  // case3. 부모의 오른쪽 자식이였을 경우
  else {
    deleted_node->parent->right = replaced_node;
  }

  replaced_node->parent = deleted_node->parent;
}

void rb_erase_fixup(rbtree *t, node_t *curr) {
  node_t *sibiling;
  while (curr != t->root && curr->color == RBTREE_BLACK) {
    // 루트 노드라면
    // if (curr->parent == t->root) { // t -> nil => t -> root
    //   curr->color = RBTREE_BLACK;
    //   return;
    // }

    // red-and-black일 때는 black으로 변경한다
    // if (curr->color == RBTREE_RED) {
    //   curr->color = RBTREE_BLACK;
    //   return;
    // }

    // doubly black이 부모의 왼쪽 노드일 때
    if (curr == curr->parent->left) {
      sibiling = curr->parent->right;

      // case1
      if (sibiling->color == RBTREE_RED) {
        sibiling->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_RED;

        left_rotate(t, curr->parent);
        sibiling = curr->parent->right;
      }

      // case 2,3,4를 해결한다
      // sibiling->color == RBTREE_BLACK일 때
      // case2 s.child 둘다 black일 때
      if (sibiling->left->color == RBTREE_BLACK &&
          sibiling->right->color == RBTREE_BLACK) {
        sibiling->color = RBTREE_RED;
        curr = curr->parent;
      }

      // case3
      else {
        if (sibiling->right->color == RBTREE_BLACK) {  // 변경
          sibiling->left->color = RBTREE_BLACK;
          sibiling->color = RBTREE_RED;

          right_rotate(t, sibiling);
          sibiling = curr->parent->right;
        }

        // case4
        sibiling->color = curr->parent->color;
        curr->parent->color = RBTREE_BLACK;
        sibiling->right->color = RBTREE_BLACK;

        // 부모를 기준으로 왼쪽 회전
        left_rotate(t, curr->parent);

        curr = t->root;
      }
    }

    // doubly black이 부모의 오른쪽 노드일 때
    else {
      sibiling = curr->parent->left;

      // case1
      if (sibiling->color == RBTREE_RED) {
        sibiling->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_RED;

        right_rotate(t, curr->parent);
        sibiling = curr->parent->left;
      }

      // case2,3,4를 해결한다.
      // 즉 sibiling->color == RBTREE_BLACK일 떄이다
      // case2
      if (sibiling->left->color == RBTREE_BLACK &&
          sibiling->right->color == RBTREE_BLACK) {
        sibiling->color = RBTREE_RED;
        curr = curr->parent;
      }

      // case3
      else {
        if (sibiling->left->color == RBTREE_BLACK) {  // 변ㄴ경
          sibiling->right->color = RBTREE_BLACK;
          sibiling->color = RBTREE_RED;

          left_rotate(t, sibiling);
          sibiling = curr->parent->left;
        }

        // case4
        sibiling->color = curr->parent->color;
        curr->parent->color = RBTREE_BLACK;
        sibiling->left->color = RBTREE_BLACK;

        // 부모를 기준으로 왼쪽 회전
        right_rotate(t, curr->parent);

        curr = t->root;
      }
    }
  }
  curr->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  if (p == NULL || p == t->nil) {
    return -1;
  }
  node_t *x;
  node_t *y = p;
  color_t y_original_color = y->color;

  if (p->left == t->nil)  // 삭제하려는 노드가 오른쪽 자식만 있을때
  {
    x = p->right;  // 새로운 포인터를 삭제하려는 노드의 오른쪽 자식으로
    transplant(t, p,
               p->right);  // 삭제하려는 노드와 삭제하려는 오른쪽 노드 위치 바꿈
  } else if (p->right == t->nil)  // 삭제하려는 노드가 왼쪽 자식만 있을때
  {
    x = p->left;  // 새로운 포인터를 삭제하려는 노드의 왼쪽 자식으로
    transplant(t, p,
               p->left);  // 삭제하려는 노드와 삭제하려는 왼쪽 노드 위치 바꿈
  } else  // if (p -> right != t -> nil && p -> left != t -> nil) // 삭제하려는
          // 노드가 자식 둘다 있을때
  {
    y = find_successor(t, p->right);  // 후임자 찍는 포인터
    y_original_color = y->color;      // 후임자 색깔 저장
    x = y->right;  // 후임자의 오른쪽 자식 찍는 포인터

    if (y->parent == p)  // 후임자의 부모노드가 삭제하려는 노드일 때
    {
      x->parent = y;  //
    } else            // if (y -> parent != p)
    {
      transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }

    transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }

  if (y_original_color == RBTREE_BLACK) {
    rb_erase_fixup(t, x);
  }

  if (t->root == p) {
    t->root = x;
  }

  free(p);

  return 0;
}

int inorder_search(const rbtree *t, node_t *p, int idx, key_t *arr, int n) {
  if (p == t->nil || idx >= n) {
    return idx;
  }

  idx = inorder_search(t, p->left, idx, arr, n);
  arr[idx++] = p->key;
  idx = inorder_search(t, p->right, idx, arr, n);
  return idx;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  inorder_search(t, t->root, 0, arr, n);
  printf("\n");
  return 0;
}