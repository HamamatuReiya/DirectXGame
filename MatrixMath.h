#pragma once
#include "Matrix4x4.h"
#include <math.h>
#include "Vector3.h"

Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2);

// 1.X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

// 2.Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

// 3.Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, Vector3& rotate, const Vector3& translate);
