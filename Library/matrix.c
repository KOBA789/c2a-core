#pragma section REPRO
#include "matrix.h"
#include <math.h>

// �s��ƃx�N�g���̒萔�{�̊|�Z
// c = C*Ab
void MatrixVector(float A[][DIM], float* b, float* c, float C)
{

  int i, j;

  for (i = 0; i < DIM; i++)
  {
    c[i] = 0.0f;
    for (j = 0; j < DIM; j++)
    {
      c[i] += A[i][j] * b[j];
    }
  }
  for (i = 0; i < DIM; i++)
  {
    c[i] = C * c[i];
  }
}

// �s��ƍs��̊|�Z
void MatrixMatrix(float A[][DIM], float B[][DIM], float C[][DIM])
{

  int i, j, k;

  for (i = 0; i < DIM; i++)
  {
    for (j = 0; j < DIM; j++)
    {
      C[i][j] = 0.0f;
      for (k = 0; k < DIM; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }

}

// �t�s��̌v�Z
int MatrixInverse(float A[][DIM], float B[][DIM])
{

  float det = A[0][0] * A[1][1] * A[2][2]
    + A[1][0] * A[2][1] * A[0][2]
    + A[2][0] * A[0][1] * A[1][2]
    - A[0][0] * A[2][1] * A[1][2]
    - A[2][0] * A[1][1] * A[0][2]
    - A[1][0] * A[0][1] * A[2][2];

  if (fabs((double)det) < 1e-10)
  {
    B[0][0] = (A[1][1] * A[2][2] - A[1][2] * A[2][1]);
    B[0][1] = (A[0][2] * A[2][1] - A[0][1] * A[2][2]);
    B[0][2] = (A[0][1] * A[1][2] - A[0][2] * A[1][1]);

    B[1][0] = (A[1][2] * A[2][0] - A[1][0] * A[2][2]);
    B[1][1] = (A[0][0] * A[2][2] - A[0][2] * A[2][0]);
    B[1][2] = (A[0][2] * A[1][0] - A[0][0] * A[1][2]);

    B[2][0] = (A[1][0] * A[2][1] - A[1][1] * A[2][0]);
    B[2][1] = (A[0][1] * A[2][0] - A[0][0] * A[2][1]);
    B[2][2] = (A[0][0] * A[1][1] - A[0][1] * A[1][0]);

    return -1;
  }
  else
  {

    B[0][0] = (A[1][1] * A[2][2] - A[1][2] * A[2][1]) / det;
    B[0][1] = (A[0][2] * A[2][1] - A[0][1] * A[2][2]) / det;
    B[0][2] = (A[0][1] * A[1][2] - A[0][2] * A[1][1]) / det;

    B[1][0] = (A[1][2] * A[2][0] - A[1][0] * A[2][2]) / det;
    B[1][1] = (A[0][0] * A[2][2] - A[0][2] * A[2][0]) / det;
    B[1][2] = (A[0][2] * A[1][0] - A[0][0] * A[1][2]) / det;

    B[2][0] = (A[1][0] * A[2][1] - A[1][1] * A[2][0]) / det;
    B[2][1] = (A[0][1] * A[2][0] - A[0][0] * A[2][1]) / det;
    B[2][2] = (A[0][0] * A[1][1] - A[0][1] * A[1][0]) / det;

    return 0;
  }
}

void nonSquareMatrixDot(float* A, float* B, float* C, int n, int m, int l)
{
  unsigned char k, j, i;
  for (i = 0; i < n; i++)
  {
    for (j = 0; j < l; j++)
    {
      float sum = 0.0f;
      for (k = 0; k < m; k++)
      {
        sum += A[i * m + k] * B[k * l + j];
      }
      C[i * l + j] = sum;
    }
  }
}

// A(3,6)�s��̓]�u���Ƃ���B�ɑ��
void trans36(float A[3][6], float B[6][3])
{
  int i, j;
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < 3; j++)
    {
      B[i][j] = A[j][i];
    }
  }
}
// A(6,3)�s��̓]�u���Ƃ���B�ɑ��
void trans63(float A[6][3], float B[3][6])
{
  int i, j;
  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 6; j++)
    {
      B[i][j] = A[j][i];
    }
  }
}
// A(6,6)�s��̓]�u���Ƃ���B�ɑ��
void trans66(float A[6][6], float B[6][6])
{
  int i, j;
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < 6; j++)
    {
      B[i][j] = A[j][i];
    }
  }
}
// (6,3)�s���(6,3)�s��̐ς��Ƃ���C�ɑ��
void mult6663(float A[6][6], float B[6][3], float C[6][3])
{
  int i, j, k;
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < 3; j++)
    {
      C[i][j] = 0.0f;
      for (k = 0; k < 6; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}
// (3,6)�s���(6,3)�s��̐ς��Ƃ���C�ɑ��
void mult3663(float A[3][6], float B[6][3], float C[3][3])
{
  int i, j, k;
  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 3; j++)
    {
      C[i][j] = 0.0f;
      for (k = 0; k < 6; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}
// (3,3)�s���(3,6)�s��̐ς��Ƃ���C�ɑ��
void mult3336(float A[3][3], float B[3][6], float C[3][6])
{
  int i, j, k;
  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 6; j++)
    {
      C[i][j] = 0.0f;
      for (k = 0; k < 3; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}
// (6,3)�s���(3,3)�s��̐ς��Ƃ���C�ɑ��
void mult6333(float A[6][3], float B[3][3], float C[6][3])
{
  int i, j, k;
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < 3; j++)
    {
      C[i][j] = 0.0f;
      for (k = 0; k < 3; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}
// (6,3)�s���(3,6)�s��̐ς��Ƃ���C�ɑ��
void mult6336(float A[6][3], float B[3][6], float C[6][6])
{
  int i, j, k;
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < 6; j++)
    {
      C[i][j] = 0.0f;
      for (k = 0; k < 3; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}
// (6,6)�s���(6,6)�s��̐ς��Ƃ���C�ɑ��
void mult6666(float A[6][6], float B[6][6], float C[6][6])
{
  int i, j, k;
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < 6; j++)
    {
      C[i][j] = 0.0f;
      for (k = 0; k < 6; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}
// (6,3)�s��Ɛ����R�̃x�N�g���̐ς��Ƃ���C�ɑ��
void multMatvec633(float A[6][3], float b[3], float c[6])
{
  int i, j;
  for (i = 0; i < 6; i++)
  {
    c[i] = 0.0f;
    for (j = 0; j < 3; j++)
    {
      c[i] += A[i][j] * b[j];
    }
  }
}
// (4,4)�s��Ɛ���4�̃x�N�g���̐ς��Ƃ���C�ɑ��
void multMatvec444(float A[4][4], float b[4], float c[4])
{
  int i, j;
  for (i = 0; i < 4; i++)
  {
    c[i] = 0.0f;
    for (j = 0; j < 4; j++)
    {
      c[i] += A[i][j] * b[j];
    }
  }
}
// (4,4)�s��ɃX�J���[��������C�ɑ��
void multCMat44(float a, float B[4][4], float C[4][4])
{
  int i, j;
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      C[i][j] = a * B[i][j];
    }
  }
}
// (6,6)�s��ɃX�J���[��������C�ɑ��
void multCMat66(float a, float B[6][6], float C[6][6])
{
  int i, j;
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < 6; j++)
    {
      C[i][j] = a * B[i][j];
    }
  }
}
// (6,6)�̒P�ʍs��𐶐�
void unitalize66(float A[6][6])
{
  int i, j;
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < 6; j++)
    {
      A[i][j] = 0.0f;
    }
    A[i][i] = 1.0f;
  }
}
// �ӂ���(3,3)�s��̘a��������C�ɑ��
void MatrixAdd33(float A[3][3], float B[3][3], float C[3][3])
{
  int i, j;
  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 3; j++)
    {
      C[i][j] = A[i][j] + B[i][j];
    }
  }
}
// �ӂ���(6,6)�s��̘a��������C�ɑ��
void MatrixAdd66(float A[6][6], float B[6][6], float C[6][6])
{
  int i, j;
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < 6; j++)
    {
      C[i][j] = A[i][j] + B[i][j];
    }
  }
}
void MatrixMinus66(float A[6][6], float B[6][6], float C[6][6])
{
  int i, j;
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < 6; j++)
    {
      C[i][j] = A[i][j] - B[i][j];
    }
  }
}

// 2��3�����̃x�N�g���̘a���o��
void add3(float* a, float* b, float* temp)
{
  int i;
  for (i = 0; i < 3; i++)
  {
    temp[i] = a[i] + b[i];
  }
}

// 2�̂S�����̃x�N�g���̘a���o��
void add4(float* a, float* b, float* temp)
{
  int i;
  for (i = 0; i < 4; i++)
  {
    temp[i] = a[i] + b[i];
  }
}

void TransMat34(float A[3][4], float A_t[4][3])
{
  int i, j;
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 3; j++)
    {
      A_t[i][j] = A[j][i];
    }
  }
}

void mult3443(float A[3][4], float B[4][3], float C[3][3])
{
  int i, j, k;
  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 3; j++)
    {
      C[i][j] = 0.0f;
      for (k = 0; k < 4; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}

void mult4334(float A[4][3], float B[3][4], float C[4][4])
{
  int i, j, k;
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      C[i][j] = 0.0f;
      for (k = 0; k < 3; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}


void mult4333(float A[4][3], float B[3][3], float C[4][3])
{
  int i, j, k;
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 3; j++)
    {
      C[i][j] = 0.0f;
      for (k = 0; k < 3; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}

void multMatvec433(float A[4][3], float b[3], float c[4])
{
  int i, j;
  for (i = 0; i < 4; i++)
  {
    c[i] = 0.0f;
    for (j = 0; j < 3; j++)
    {
      c[i] += A[i][j] * b[j];
    }
  }
}

void MatrixSub44(float A[4][4], float B[4][4], float C[4][4])
{
  int i, j;
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      C[i][j] = A[i][j] - B[i][j];
    }
  }
}

void sub4(float* a, float* b, float* temp)
{
  int i;
  for (i = 0; i < 4; i++)
  {
    temp[i] = a[i] - b[i];
  }
}

#pragma section
