#ifndef VECTOR_H_
#define VECTOR_H_
// TODO: ���̃\�[�X�R�[�h�͌��݊Ǘ�����Ă܂���D���w���C�u�����͌��ݕʂɂĎ������ł�

#define DIM 3
#define NEGLIGIBLE_VALUE    1.0e-5f

extern float VectorNorm(const float* v);                                        // �x�N�g��v�̃m������Ԃ�
extern int NormalizeVector(const float* v, float* ans);                          // �x�N�g��v�𐳋K���������̂�ans�Ɋi�[
extern float VectorInnerProduct(const float* v1, const float* v2);               // �x�N�g���̓���v1�Ev2��Ԃ�
extern void VectorOuterProduct(const float* v1, const float* v2,  float* ans);    // �x�N�g���̊O��v1�~v2��ans�Ɋi�[
extern void VectorScalarProduct(const float a, const float* v, float* ans);               // �x�N�g���̃X�J���[�{a*v��ans�Ɋi�[
extern void VectorAdd(const float* v1, const float* v2, float* ans);              // �x�N�g���̘av1+v2��ans�Ɋi�[
extern void VectorSub(const float* v1, const float* v2, float* ans);              // �x�N�g���̍�v1-v2��ans�Ɋi�[
extern void VectorNeg(const float* v, float* ans);                               // �x�N�g��v��-1�{(���])��ans�Ɋi�[
extern void VectorCopy(const float* v, float* ans);                             // �x�N�g��v�̒l��ans�ɃR�s�[
extern char IsZeroVector(const float* v);                                       // 0�x�N�g���ł�������1��Ԃ�

#endif
