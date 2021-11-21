#ifndef QUATERNION_H_
#define QUATERNION_H_

// TODO: ���̃\�[�X�R�[�h�͌��݊Ǘ�����Ă܂���D���w���C�u�����͌��ݕʂɂĎ������ł�

// #define ADCS_PI   3.14159265358979323846
// #define ADCS_PI_2 1.57079632679489661923


void Quat2DCM(const float* q, float DCM[][3]);      // �N�H�[�^�j�I��q�ɑΉ�����DCM���쐬
void DCM2Quat(const float DCM[][3], float* q);      // DCM�ɑΉ�����N�H�[�^�j�I��q���쐬
void Angle2Quat(float Z, float Y, float X, float* q); // �e���܂��̉�]�pZ, Y, X(�I�C���[�p)����N�H�[�^�j�I��q���쐬
void QuatMul(const float* q1, const float* q2, float* ans);    // �N�H�[�^�j�I����q1*q2��ans�Ɋi�[
float QuatNorm(const float* q);                    // q�̃m������Ԃ�
int NormalizeQuat(const float* q, float* ans);      // q�𐳋K���������̂�ans�Ɋi�[
void Quat0Positive(const float* q, float* ans);     // q�̃X�J���[�����𐳂ɂ������̂�ans�Ɋi�[
void QuatInverse(const float* q, float* ans);       // q�̋t�N�H�[�^�j�I����ans�Ɋi�[
// double ADCS_sin(double x);  // �O�p�֐��̎p���n�����imath���C�u�������L��������Ă��Ȃ��ꍇ�Ɏg���j
// double ADCS_cos(double x);  // �O�p�֐��̎p���n�����imath���C�u�������L��������Ă��Ȃ��ꍇ�Ɏg���j

#endif
