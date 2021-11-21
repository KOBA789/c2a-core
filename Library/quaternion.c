#pragma section REPRO
#include "quaternion.h"

// C++�ɂ�mathf.h�͂Ȃ� (math.h�ɓ�������Ă���B)
#include <src_user/Settings/sils_define.h>
#ifndef SILS_FW
#include <mathf.h>
#endif
#include <math.h>


void Quat2DCM(const float* q, float DCM[][3]) // �N�H�[�^�j�I��q�ɑΉ�����DCM���쐬
{
  DCM[0][0] = q[3] * q[3] + q[0] * q[0] - q[1] * q[1] - q[2] * q[2];
  DCM[0][1] = 2.0f * (q[0] * q[1] + q[3] * q[2]);
  DCM[0][2] = 2.0f * (q[0] * q[2] - q[3] * q[1]);
  DCM[1][0] = 2.0f * (q[0] * q[1] - q[3] * q[2]);
  DCM[1][1] = q[3] * q[3] - q[0] * q[0] + q[1] * q[1] - q[2] * q[2];
  DCM[1][2] = 2.0f * (q[1] * q[2] + q[3] * q[0]);
  DCM[2][0] = 2.0f * (q[0] * q[2] + q[3] * q[1]);
  DCM[2][1] = 2.0f * (q[1] * q[2] - q[3] * q[0]);
  DCM[2][2] = q[3] * q[3] - q[0] * q[0] - q[1] * q[1] + q[2] * q[2];
}

void DCM2Quat(const float DCM[][3], float* q) // DCM�ɑΉ�����N�H�[�^�j�I��q���쐬
{
  q[3] = sqrtf(DCM[0][0] + DCM[1][1] + DCM[2][2] + 1.0f) * 0.5f;
  if (q[3] < 1.0e-8f)
  {
    q[3] = 0.0f;
    q[0] = sqrtf((DCM[0][0] + 1.0f) * 0.5f);
    q[1] = sqrtf((DCM[1][1] + 1.0f) * 0.5f);
    q[2] = sqrtf((DCM[2][2] + 1.0f) * 0.5f);
  }
  else
  {
    q[0] = (DCM[1][2] - DCM[2][1]) / (4.0f * q[3]);
    q[1] = (DCM[2][0] - DCM[0][2]) / (4.0f * q[3]);
    q[2] = (DCM[0][1] - DCM[1][0]) / (4.0f * q[3]);
  }
  NormalizeQuat(q, q);
}

void Angle2Quat(float Z, float Y, float X, float* q) // �e���܂��̉�]�pZ,Y,X(�I�C���[�p)����N�H�[�^�j�I��q���쐬
{
  float qX[4], qY[4], qZ[4];
  float q_temp[4];

  qX[0] = sinf(X / 2.0f);
  qX[1] = 0.0f;
  qX[2] = 0.0f;
  qX[3] = cosf(X / 2.0f);

  qY[0] = 0.0f;
  qY[1] = sinf(Y / 2.0f);
  qY[2] = 0.0f;
  qY[3] = cosf(Y / 2.0f);

  qZ[0] = 0.0f;
  qZ[1] = 0.0f;
  qZ[2] = sinf(Z / 2.0f);
  qZ[3] = cosf(Z / 2.0f);

  QuatMul(qZ, qY, q_temp);
  QuatMul(q_temp, qX, q);
}

void QuatMul(const float* q1, const float* q2, float* ans) // �N�H�[�^�j�I����q1*q2��ans�Ɋi�[
{
  float tmp[4];
  tmp[0] =  q1[3] * q2[0] - q1[2] * q2[1] + q1[1] * q2[2] + q1[0] * q2[3];
  tmp[1] =  q1[2] * q2[0] + q1[3] * q2[1] - q1[0] * q2[2] + q1[1] * q2[3];
  tmp[2] = -q1[1] * q2[0] + q1[0] * q2[1] + q1[3] * q2[2] + q1[2] * q2[3];
  tmp[3] = -q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] + q1[3] * q2[3];
  NormalizeQuat(tmp, ans);
}

float QuatNorm(const float* q) // q�̃m������Ԃ�
{
  return sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
}

int NormalizeQuat(const float* q, float* ans) // q�𐳋K���������̂�ans�Ɋi�[
{
  float norm;
  norm = QuatNorm(q);
  if (norm < 1e-10)
  {
    ans[0] = q[0];
    ans[1] = q[1];
    ans[2] = q[2];
    ans[3] = q[3];
    return -1;
  }
  else
  {
    ans[0] = q[0] / norm;
    ans[1] = q[1] / norm;
    ans[2] = q[2] / norm;
    ans[3] = q[3] / norm;
    return 0;
  }
  Quat0Positive(ans, ans); // TODO: ����͐�΂Ɏ��s����Ȃ����A�o�O�H
}

void Quat0Positive(const float* q, float* ans) // q�̃X�J���[�����𐳂ɂ������̂�ans�Ɋi�[
{
  int i;

  if (q[3] < 0.0)
  {
    for (i = 0; i < 4; i++)
    {
    ans[i] = -q[i];
    }
  }
  else
  {
    for (i = 0; i < 4; i++)
    {
      ans[i] = q[i];
    }
  }
}

void QuatInverse(const float* q, float* ans)  // q�̋t�N�H�[�^�j�I����ans�Ɋi�[
{
  ans[0] = -q[0];
  ans[1] = -q[1];
  ans[2] = -q[2];
  ans[3] =  q[3];
}

// double ADCS_sin(double x)
// {
//   // Returns y = sin(x), x in radians.
//   // Calculates Sine function by minimax polynomials.
//
//   double x_evaled; // in [0, pi/2).
//   double y_abs; // = |y| = sin(x_evaled).
//   char y_sign;  // = sgn(y). 1 or -1.
//   double x_, x_squared, x_tmp;
//
//   // Polynomial coefficients
//   const double a1 =  1.570791011;
//   const double a3 = -0.6458928493;
//   const double a5 =  0.0794343442;
//   const double a7 = -0.004333095;
//
//   // First, let x = mod(x, 2*pi) (0 <= x < 2*pi)
//   while (x < 0.0)
//   {
//     x += 2.0 * ADCS_PI;
//   }
//   while (x >= 2.0 * ADCS_PI)
//   {
//     x -= 2.0 * ADCS_PI;
//   }
//
//   // Find such an x_evaled in [0, pi/2) that |sin(x)| = sin(x_evaled).
//   if (x < ADCS_PI / 2.0) // 0 <= x < pi/2
//   {
//     x_evaled = x;
//     y_sign = 1;
//   }
//   else if (x < ADCS_PI) // pi/2 <= x < pi
//   {
//     // sin(x) = sin(pi - x)
//     x_evaled = ADCS_PI - x;
//     y_sign = 1;
//   }
//   else if (x < ADCS_PI * 3 / 2) // pi <= x < 3/2*pi
//   {
//     // sin(x) = -sin(x - pi)
//     x_evaled = x - ADCS_PI;
//     y_sign = -1;
//   }
//   else // 3/2*pi <= x < 2*pi
//   {
//     // sin(x) = -sin(2*pi - x)
//     x_evaled = 2.0 * ADCS_PI - x;
//     y_sign = -1;
//   }
//
//   // Calculate sin(x_evaled).
//   // Calculate y_abs by approximate polynomials
//   // y_abs = a1*x_ + a3*x_^3 + a5*x_^5 + a7*x_^7
//   x_ = x_evaled * 2 / ADCS_PI;
//   x_squared = x_ * x_;
//   // a1*x
//   x_tmp = x_;
//   y_abs = a1 * x_tmp;
//   // a3*x^3
//   x_tmp *= x_squared;
//   y_abs += a3 * x_tmp;
//   // a5*x^5
//   x_tmp *= x_squared;
//   y_abs += a5 * x_tmp;
//   // a7*x^7
//   x_tmp *= x_squared;
//   y_abs += a7 * x_tmp;
//
//   return y_sign * y_abs;
// }
//
// double ADCS_cos(double x)
// {
//   // Returns y = cos(x), x in radians
//   // cos(x) = sin(pi/2 - x)
//   return ADCS_sin(ADCS_PI / 2.0 - x);
// }

#pragma section
