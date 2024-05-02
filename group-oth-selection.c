#include <stdio.h>

#define NUM_OF_ELEMENTS (8)

int
main(void)
{
  int way; /* 方向 */
  int x, y; /* すでに置かれた石の位置 */
  int row, column; /* 隣接要素の位置 */
  int row_vector, column_vector;

  printf("すでに置かれた石の位置\n");
  printf("何行目(左から右に0〜7): ");
  scanf("%d", &x);
  printf("何列目(下から上に0〜7): ");
  scanf("%d", &y);
  printf("方向(上が0で時計回りに0〜7): ");
  scanf("%d", &way);
  
  /* 隣接要素の位置計算 */
  switch(way) {
  case 0: row_vector = 1; column_vector = 0; break;
  case 1: row_vector = 1; column_vector = 1; break;
  case 2: row_vector = 0; column_vector = 1; break;
  case 3: row_vector = -1; column_vector = 1; break;
  case 4: row_vector = -1; column_vector = 0;  break;
  case 5: row_vector = -1; column_vector = -1; break;
  case 6: row_vector = 0; column_vector = -1; break;
  case 7: row_vector = 1; column_vector = -1; break;
  default: break;
  }

  row = x + row_vector;
  column = y + column_vector;

/* 隣接要素の有無の判定 */
  if((row < 0) || (row > NUM_OF_ELEMENTS - 1))
    printf("隣接要素がありません．\n");
  else if((column < 0) || (column > NUM_OF_ELEMENTS - 1))
    printf("隣接要素がありません．\n");
  else 
    printf("隣接要素の位置は(%d,%d)です\n", row, column);

  return 0;
}
