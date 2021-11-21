#pragma section REPRO
#include "vector.h"

// C++�ɂ�mathf.h�͂Ȃ� (math.h�ɓ�������Ă���B)
#include <src_user/Settings/sils_define.h>
#ifndef SILS_FW
#include <mathf.h>
#endif
#include <math.h>


// �x�N�g��v�̃m������Ԃ�
float VectorNorm(const float* v)
{
  return (float)sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

// �x�N�g��v�𐳋K���������̂�ans�Ɋi�[
int NormalizeVector(const float* v, float* ans)
{
  float norm = VectorNorm(v);
  if (norm < NEGLIGIBLE_VALUE)
  {
    ans[0] = v[0];
    ans[1] = v[1];
    ans[2] = v[2];
    return -1;
  }
  else
  {
    ans[0] = v[0] / norm;
    ans[1] = v[1] / norm;
    ans[2] = v[2] / norm;
    return 0;
  }
}
// �x�N�g���̓���v1�Ev2��Ԃ�
float VectorInnerProduct(const float* v1, const float* v2)
{
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
// �x�N�g���̊O��v1�~v2��ans�Ɋi�[
void VectorOuterProduct(const float* v1, const float* v2, float* ans)
{
  ans[0] = v1[1] * v2[2] - v1[2] * v2[1];
  ans[1] = v1[2] * v2[0] - v1[0] * v2[2];
  ans[2] = v1[0] * v2[1] - v1[1] * v2[0];
}
// �x�N�g���̃X�J���[�{a*v��ans�Ɋi�[
void VectorScalarProduct(const float a, const float* v, float* ans)
{
  ans[0] = a * v[0];
  ans[1] = a * v[1];
  ans[2] = a * v[2];
}
// �x�N�g���̘av1+v2��ans�Ɋi�[
void VectorAdd(const float* v1, const float* v2, float* ans)
{
  ans[0] = v1[0] + v2[0];
  ans[1] = v1[1] + v2[1];
  ans[2] = v1[2] + v2[2];
}
// �x�N�g���̍�v1-v2��ans�Ɋi�[
void VectorSub(const float* v1, const float* v2, float* ans)
{
  ans[0] = v1[0] - v2[0];
  ans[1] = v1[1] - v2[1];
  ans[2] = v1[2] - v2[2];
}
// �x�N�g��v��-1�{(���])��ans�Ɋi�[
void VectorNeg(const float* v, float* ans)
{
  ans[0] = -v[0];
  ans[1] = -v[1];
  ans[2] = -v[2];
}
// �x�N�g��v�̒l��ans�ɃR�s�[
void VectorCopy(const float* v, float* ans)
{
  ans[0] = v[0];
  ans[1] = v[1];
  ans[2] = v[2];
}

// 0�x�N�g���ł�������1��Ԃ�
extern char IsZeroVector(const float* v)
{

  if ((fabsf(v[0]) < NEGLIGIBLE_VALUE) && (fabsf(v[1]) < NEGLIGIBLE_VALUE) && (fabsf(v[2]) < NEGLIGIBLE_VALUE))
  {
    return 1;
  }
  return 0;
}

#pragma section
