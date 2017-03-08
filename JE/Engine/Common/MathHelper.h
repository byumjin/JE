//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <Windows.h>
//#include <xnamath.h>
#include <DirectXMath.h>
#include <cmath>

using namespace DirectX;

XMFLOAT3 operator+ (const XMFLOAT3& val1, const XMFLOAT3& val2);
XMFLOAT3 operator- (const XMFLOAT3& val1, const XMFLOAT3& val2);
XMFLOAT3 operator* (const XMFLOAT3& val1, const XMFLOAT3& val2);
XMFLOAT3 operator* (const FLOAT& val1, const XMFLOAT3& val2);
XMFLOAT3 operator* (const XMFLOAT3& val1, const FLOAT& val2);
XMFLOAT3 operator/ (const XMFLOAT3& val1, const FLOAT& val2);



static const XMVECTOR g_UnitQuaternionEpsilon =
{
	1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f
};
static const XMVECTOR g_UnitVectorEpsilon =
{
	1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f
};
static const XMVECTOR g_UnitPlaneEpsilon =
{
	1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f
};

class MathHelper
{
public:

	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF()*(b-a);
	}

	//(degree->radian)
	static float ConvertToRadians(float fDegrees)
	{
		return fDegrees * (XM_PI / 180.0f);
	}

	
	//(radian->degree)
	static float ConvertToDegrees(float fRadians)
	{
		return fRadians * (180.0f / XM_PI);
	}


	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}
	 
	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b-a)*t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x); 
	}

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y);

	static XMFLOAT3 XMFLOAT3_CROSS(XMFLOAT3 F1, XMFLOAT3 F2)
	{
		return XMFLOAT3(
			(F1.y * F2.z) - (F1.z * F2.y),
			(F1.z * F2.x) - (F1.x * F2.z),
			(F1.x * F2.y) - (F1.y * F2.x));
	}

	static float XMFLOAT3_DOT(XMFLOAT3 F1, XMFLOAT3 F2)
	{
		return float((F1.x * F2.x) + (F1.y * F2.y) + (F1.z * F2.z));
	}

	static XMFLOAT3 XMFLOAT3_NORMALIZE(XMFLOAT3 F1)
	{
		float length = sqrt((F1.x*F1.x) + (F1.y * F1.y) + (F1.z * F1.z));
		if (length != 0.0f)
		{
			return XMFLOAT3(F1.x / length, F1.y / length, F1.z / length);
		}

		return F1;
	}

	static XMFLOAT3 XMVECTOR_TO_XMFLOAT3(XMVECTOR vector)
	{
		XMFLOAT3 fvalue;
		XMStoreFloat3(&fvalue, vector);
		return fvalue;
	}

	static XMFLOAT4 XMVECTOR_TO_XMFLOAT4(XMVECTOR vector)
	{
		XMFLOAT4 fvalue;
		XMStoreFloat4(&fvalue, vector);
		return fvalue;
	}

	static XMVECTOR XMFLOAT3_TO_XMVECTOR(XMFLOAT3 fvalue, float w)
	{
		XMVECTOR vector;
		return vector = XMLoadFloat4(&XMFLOAT4(fvalue.x, fvalue.y, fvalue.z, w));		
	}

	static XMFLOAT4 XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT4 fvalue1, XMFLOAT4X4 fvalue2)
	{
		XMFLOAT4 result;
		result.x = fvalue1.x*fvalue2._11 + fvalue1.y*fvalue2._21 + fvalue1.z*fvalue2._31 + fvalue1.w*fvalue2._41;
		result.y = fvalue1.x*fvalue2._12 + fvalue1.y*fvalue2._22 + fvalue1.z*fvalue2._32 + fvalue1.w*fvalue2._42;
		result.z = fvalue1.x*fvalue2._13 + fvalue1.y*fvalue2._23 + fvalue1.z*fvalue2._33 + fvalue1.w*fvalue2._43;
		result.w = fvalue1.x*fvalue2._14 + fvalue1.y*fvalue2._24 + fvalue1.z*fvalue2._34 + fvalue1.w*fvalue2._44;

		return result;
	}

	static XMFLOAT4 XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT3 fvalue, XMFLOAT4X4 fvalue2)
	{
		XMFLOAT4 fvalue1 = XMFLOAT4(fvalue.x, fvalue.y, fvalue.z, 1.0f);
		XMFLOAT4 result;
		result.x = fvalue1.x*fvalue2._11 + fvalue1.y*fvalue2._21 + fvalue1.z*fvalue2._31 + fvalue1.w*fvalue2._41;
		result.y = fvalue1.x*fvalue2._12 + fvalue1.y*fvalue2._22 + fvalue1.z*fvalue2._32 + fvalue1.w*fvalue2._42;
		result.z = fvalue1.x*fvalue2._13 + fvalue1.y*fvalue2._23 + fvalue1.z*fvalue2._33 + fvalue1.w*fvalue2._43;
		result.w = fvalue1.x*fvalue2._14 + fvalue1.y*fvalue2._24 + fvalue1.z*fvalue2._34 + fvalue1.w*fvalue2._44;

		return result;
	}

	static XMFLOAT3 XMFLOAT3_MUL_SCALAR(XMFLOAT3 fvalue, float scalar)
	{
		return XMFLOAT3(fvalue.x*scalar, fvalue.y*scalar, fvalue.z*scalar);
	}

	static XMFLOAT4 XMFLOAT3_TO_XMFLOAT4(XMFLOAT3 fvalue, float w)
	{
		return XMFLOAT4(fvalue.x, fvalue.y, fvalue.z, w);
	}

	static XMFLOAT3 XMFLOAT4_TO_XMFLOAT3(XMFLOAT4 fvalue)
	{
		return XMFLOAT3(fvalue.x, fvalue.y, fvalue.z);
	}

	static XMFLOAT4 XMFLOAT4_DIV_W(XMFLOAT4 fvalue1)
	{
		fvalue1.x = fvalue1.x / fvalue1.w;
		fvalue1.y = fvalue1.y / fvalue1.w;
		fvalue1.z = fvalue1.z / fvalue1.w;
		fvalue1.w = 1.0f;
		
		return fvalue1;
	}

	static FLOAT Distance(XMFLOAT3 fvalue1, XMFLOAT3 fvalue2)
	{
		fvalue1.x = fvalue1.x - fvalue2.x;
		fvalue1.x = fvalue1.x*fvalue1.x;

		fvalue1.y = fvalue1.y - fvalue2.y;
		fvalue1.y = fvalue1.y*fvalue1.y;

		fvalue1.z = fvalue1.z - fvalue2.z;
		fvalue1.z = fvalue1.z*fvalue1.z;
		
		return  sqrt(fvalue1.x + fvalue1.y + fvalue1.z);
	}

	static XMFLOAT3 QuateriontoEuler(XMFLOAT4 q1) {

		XMFLOAT3 Euler;

		float test = q1.x*q1.y + q1.z*q1.w;
		if (test > 0.499f) { // singularity at north pole
			Euler.y = 2.0f * atan2(q1.x, q1.w);
			Euler.z = XM_PI / 2.0f;
			Euler.x = 0;
			return Euler;
		}
		if (test < -0.499) { // singularity at south pole
			Euler.y = -2 * atan2(q1.x, q1.w);
			Euler.z = -XM_PI / 2.0f;
			Euler.x = 0;
			return Euler;
		}
		float sqx = q1.x*q1.x;
		float sqy = q1.y*q1.y;
		float sqz = q1.z*q1.z;
		Euler.y = atan2(2.0f * q1.y*q1.w - 2.0f * q1.x*q1.z, 1.0f - 2.0f * sqy - 2.0f * sqz);
		Euler.z = asin(2.0f * test);
		Euler.x = atan2(2.0f * q1.x*q1.w - 2.0f * q1.y*q1.z, 1.0f - 2.0f * sqx - 2.0f * sqz);

		return Euler;
	}

	static void RemoveFloatError(XMFLOAT4X4 *fvalue1)
	{
		if (abs(fvalue1->_11) < 0.0000001f) fvalue1->_11 = 0.0f;
		if (abs(fvalue1->_12) < 0.0000001f) fvalue1->_12 = 0.0f;
		if (abs(fvalue1->_13) < 0.0000001f) fvalue1->_13 = 0.0f;
		if (abs(fvalue1->_14) < 0.0000001f) fvalue1->_14 = 0.0f;
		if (abs(fvalue1->_21) < 0.0000001f) fvalue1->_21 = 0.0f;
		if (abs(fvalue1->_22) < 0.0000001f) fvalue1->_22 = 0.0f;
		if (abs(fvalue1->_23) < 0.0000001f) fvalue1->_23 = 0.0f;
		if (abs(fvalue1->_24) < 0.0000001f) fvalue1->_24 = 0.0f;
		if (abs(fvalue1->_31) < 0.0000001f) fvalue1->_31 = 0.0f;
		if (abs(fvalue1->_32) < 0.0000001f) fvalue1->_32 = 0.0f;
		if (abs(fvalue1->_33) < 0.0000001f) fvalue1->_33 = 0.0f;
		if (abs(fvalue1->_34) < 0.0000001f) fvalue1->_34 = 0.0f;
		if (abs(fvalue1->_41) < 0.0000001f) fvalue1->_41 = 0.0f;
		if (abs(fvalue1->_42) < 0.0000001f) fvalue1->_42 = 0.0f;
		if (abs(fvalue1->_43) < 0.0000001f) fvalue1->_43 = 0.0f;
		if (abs(fvalue1->_44) < 0.0000001f) fvalue1->_44 = 0.0f;
	}

	static XMMATRIX InverseTranspose(CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det, A));
	}

	static XMMATRIX Inverse(CXMMATRIX M)
	{
		//XMMATRIX A = M;
		
		//A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(M);

		return XMMatrixInverse(&det, M);
	}

	static XMVECTOR RandUnitVec3();
	static XMVECTOR RandHemisphereUnitVec3(XMVECTOR n);

	static const float Infinity;
	static const float Pi;

	
	


	//-----------------------------------------------------------------------------
	// Return TRUE if the vector is a unit vector (length == 1).
	//-----------------------------------------------------------------------------
	static inline BOOL XMVector3IsUnit(FXMVECTOR V)
	{
		XMVECTOR Difference = XMVector3Length(V) - XMVectorSplatOne();

		return XMVector4Less(XMVectorAbs(Difference), g_UnitVectorEpsilon);
	}

	static BOOL IntersectRayTriangle(FXMVECTOR Origin, FXMVECTOR Direction, FXMVECTOR V0, CXMVECTOR V1, CXMVECTOR V2,
		FLOAT* pDist)
	{
		assert(pDist);
		assert(XMVector3IsUnit(Direction));

		static const XMVECTOR Epsilon =
		{
			1e-20f, 1e-20f, 1e-20f, 1e-20f
		};

		XMVECTOR Zero = XMVectorZero();

		XMVECTOR e1 = V1 - V0;
		XMVECTOR e2 = V2 - V0;

		// p = Direction ^ e2;
		XMVECTOR p = XMVector3Cross(Direction, e2);

		// det = e1 * p;
		XMVECTOR det = XMVector3Dot(e1, p);

		XMVECTOR u, v, t;

		if (XMVector3GreaterOrEqual(det, Epsilon))
		{
			// Determinate is positive (front side of the triangle).
			XMVECTOR s = Origin - V0;

			// u = s * p;
			u = XMVector3Dot(s, p);

			XMVECTOR NoIntersection = XMVectorLess(u, Zero);
			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(u, det));

			// q = s ^ e1;
			XMVECTOR q = XMVector3Cross(s, e1);

			// v = Direction * q;
			v = XMVector3Dot(Direction, q);

			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(v, Zero));
			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(u + v, det));

			// t = e2 * q;
			t = XMVector3Dot(e2, q);

			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(t, Zero));

			if (XMVector4EqualInt(NoIntersection, XMVectorTrueInt()))
				return FALSE;
		}
		else if (XMVector3LessOrEqual(det, -Epsilon))
		{
			// Determinate is negative (back side of the triangle).
			XMVECTOR s = Origin - V0;

			// u = s * p;
			u = XMVector3Dot(s, p);

			XMVECTOR NoIntersection = XMVectorGreater(u, Zero);
			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(u, det));

			// q = s ^ e1;
			XMVECTOR q = XMVector3Cross(s, e1);

			// v = Direction * q;
			v = XMVector3Dot(Direction, q);

			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(v, Zero));
			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(u + v, det));

			// t = e2 * q;
			t = XMVector3Dot(e2, q);

			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(t, Zero));

			if (XMVector4EqualInt(NoIntersection, XMVectorTrueInt()))
				return FALSE;
		}
		else
		{
			// Parallel ray.
			return FALSE;
		}

		XMVECTOR inv_det = XMVectorReciprocal(det);

		t *= inv_det;

		// u * inv_det and v * inv_det are the barycentric cooridinates of the intersection.

		// Store the x-component to *pDist
		XMStoreFloat(pDist, t);

		return TRUE;
	}


};

#endif // MATHHELPER_H