#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>

void printTree(rbtree *t, node_t *cur, int level, int isLeft) {
  if (cur == t->nil) {
    return;
  }

  // 오른쪽 자식 노드 출력
  printTree(t, cur->right, level + 1, 0);

  // 현재 노드 출력
  for (int i = 0; i < level - 1; i++) {
    printf("    ");
  }
  if (level > 0) {
    printf(isLeft ? " \\_ " : " /⎺ ");  // 왼쪽 자식일 경우 "\\" 출력, 오른쪽
                                        // 자식일 경우 "/" 출력
  }
  if (cur->color == RBTREE_RED) {
    printf("\x1b[31m%d\x1b[0m\n", cur->key);
  } else {
    printf("%d\n", cur->key);
  }

  // 왼쪽 자식 노드 출력
  printTree(t, cur->left, level + 1, 1);
}

// 새로운 트리 생성
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  if (p == NULL) {
    return NULL;
  }

  node_t *nil_node = (node_t *)malloc(sizeof(node_t));

  nil_node->color = RBTREE_BLACK;

  p->nil = nil_node;
  p->root = nil_node;

  return p;
}

void search_delete(rbtree *t, node_t *node) {
  if (node->left != t->nil) search_delete(t, node->left);
  if (node->right != t->nil) search_delete(t, node->right);
  free(node);
}

void delete_rbtree(rbtree *t) {
  node_t *node = t->root;
  if (node != t->nil) search_delete(t, node);
  free(t->nil);
  free(t);
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
  t->root->parent = t->nil;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *parent, *curr, *new_node;

  parent = t->nil;
  curr = t->root;

  new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node->key = key;
  new_node->color = RBTREE_RED;
  new_node->left = t->nil;
  new_node->right = t->nil;

  while (curr != t->nil) {
    parent = curr;
    // 새로운 루트의 키가 현 루트의 키보다 작다
    // 이때는 왼쪽 서브 트리로 이동한다
    if (new_node->key < curr->key) {
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
  } else if (new_node->key < parent->key) {
    parent->left = new_node;
  } else {
    parent->right = new_node;
  }

  rb_insert_fixup(t, new_node);

  return t->root;
}

// key에 해당하는 node를 반환
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *curr = t->root;

  // 루트의 값이 nil이 아닐 때까지 탐색한다
  while (curr != t->nil) {
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

  if (curr == NULL) {
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

  if (curr == NULL) {
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
  rbtree *temp = (rbtree *)calloc(1, sizeof(rbtree));
  temp->root = curr->left;
  temp->nil = t->nil;

  node_t *max = rbtree_max(temp);

  free(temp);

  return max;
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
    return find_successor(t, curr);
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
  else if (deleted_node == deleted_node->parent->right) {
    deleted_node->parent->right = replaced_node;
  }
  replaced_node->parent = deleted_node->parent;
}

void rb_erase_fixup(rbtree *t, node_t *curr) {
  int replaced = 0;

  while (1) {
    // 현재 노드가 루트 노드이고 이미 루트 노드의 색이 검은색인 경우
    if (curr->parent == t->nil && replaced == 1) {
      curr->color = RBTREE_BLACK;
      return;
    }

    // 현재 노드의 색이 빨간색인 경우
    if (curr->color == RBTREE_RED) {
      curr->color = RBTREE_BLACK;
      return;
    }

    node_t *sibling;

    // 현재 노드가 부모의 왼쪽 자식인 경우
    if (curr == curr->parent->left) {
      sibling = curr->parent->right;

      // case1: 형제 노드의 색이 빨간색인 경우
      if (sibling->color == RBTREE_RED) {
        sibling->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_RED;
        left_rotate(t, curr->parent);
      } else {
        // case2, case3, case4를 해결
        if (sibling->color == RBTREE_BLACK &&
            sibling->left->color == RBTREE_BLACK &&
            sibling->right->color == RBTREE_BLACK) {
          sibling->color = RBTREE_RED;
          curr = curr->parent;
          replaced = 1;
          if (curr->color == RBTREE_BLACK) {
            return;
          }
        } else {
          if (sibling->left->color == RBTREE_RED &&
              sibling->right->color == RBTREE_BLACK) {
            sibling->left->color = RBTREE_BLACK;
            sibling->color = RBTREE_RED;
            right_rotate(t, sibling);
          }
          sibling->color = curr->parent->color;
          curr->parent->color = RBTREE_BLACK;
          sibling->right->color = RBTREE_BLACK;
          left_rotate(t, curr->parent);
          curr = t->root;
          replaced = 1;
        }
      }
    }
    // 현재 노드가 부모의 오른쪽 자식인 경우
    else {
      sibling = curr->parent->left;

      // case1: 형제 노드의 색이 빨간색이고 부모 노드의 색이 검은색인 경우
      if (sibling->color == RBTREE_RED && curr->parent->color == RBTREE_BLACK) {
        sibling->color = RBTREE_BLACK;
        curr->parent->color = RBTREE_RED;
        right_rotate(t, curr->parent);
      } else {
        // case2, case3, case4를 해결
        if (sibling->color == RBTREE_BLACK &&
            sibling->left->color == RBTREE_BLACK &&
            sibling->right->color == RBTREE_BLACK) {
          sibling->color = RBTREE_RED;
          curr = curr->parent;
          replaced = 1;
          if (curr->color == RBTREE_BLACK) {
            return;
          }
        } else {
          if (sibling->left->color == RBTREE_BLACK &&
              sibling->right->color == RBTREE_RED) {
            sibling->right->color = RBTREE_BLACK;
            sibling->color = RBTREE_RED;
            left_rotate(t, sibling);
          }
          sibling->color = curr->parent->color;
          curr->parent->color = RBTREE_BLACK;
          sibling->left->color = RBTREE_BLACK;
          right_rotate(t, curr->parent);
          curr = t->root;
          replaced = 1;
        }
      }
    }
  }
}

int rbtree_erase(rbtree *t, node_t *p) {
  printTree(t, t->root, 0, 0);
  // 1. 삭제되는 노드를 찾는다.
  node_t *curr = rbtree_find(t, p->key);
  node_t *replaced_node;

  // 삭제되는 노드의 자녀가 0,1개인 경우 삭제되는 색은 삭제되는 노드의 색이다
  color_t deleted_color = curr->color;

  // 2. 삭제되는 색깔을 찾는다.

  // case1. 자식이 하나도 없다면
  if (curr->left == t->nil && curr->right == t->nil) {
    replaced_node = t->nil;
    transplant(t, curr, replaced_node);
  }

  // case2. 왼쪽 자식만 있는 경우
  else if (curr->left != t->nil && curr->right == t->nil) {
    replaced_node = curr->left;
    transplant(t, curr, replaced_node);
  }

  // case3. 오른쪽 자식만 있는 경우
  else if (curr->left == t->nil && curr->right != t->nil) {
    replaced_node = curr->right;
    transplant(t, curr, replaced_node);
  }

  // case4. 왼쪽 오른쪽 두 자식이 모두 있는 경우
  else {
    // 선임자를 선택한다
    replaced_node = find_successor(t, curr);

    // 자녀가 2개인 경우 삭제되는 노드의 색인 삭제되는 노드의 successor의 색이다
    deleted_color = replaced_node->color;

    // 선임자가 삭제될 노드의 오른쪽 자식인 경우
    if (replaced_node->parent == curr) {
      replaced_node->left->parent = replaced_node;
    } else {
      transplant(t, replaced_node, replaced_node->left);
      replaced_node->left = curr->left;
      replaced_node->left->parent = replaced_node;
    }

    transplant(t, curr, replaced_node);
    replaced_node->right = curr->right;
    replaced_node->right->parent = replaced_node;
    replaced_node->color = curr->color;
  }

  // 삭제되는 색깔이 검은색일 경우 재조정을 한다
  if (deleted_color == RBTREE_BLACK) {
    printf("%d\n", replaced_node->key);
    rb_erase_fixup(t, replaced_node);
  }

  free(curr);
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

void rbtree_to_print(node_t *t, node_t *nil) {
  // TODO: implement to_print
  if (t == nil) {
    printf("노드 바닥이에용\n");
    return;
  }
  printf("key = %d, color = %d \n", t->key, t->color);
  rbtree_to_print(t->left, nil);
  rbtree_to_print(t->right, nil);
}
