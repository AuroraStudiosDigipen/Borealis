
using System;
using System.Runtime.InteropServices;

namespace Borealis
{
    public struct Vector3
    {
        public float x;
        public float y;
        public float z;

        // Static Properties
        public static Vector3 back => new Vector3(0, 0, -1);
        public static Vector3 down => new Vector3(0, -1, 0);
        public static Vector3 forward => new Vector3(0, 0, 1);
        public static Vector3 left => new Vector3(-1, 0, 0);
        public static Vector3 negativeInfinity => new Vector3(float.NegativeInfinity, float.NegativeInfinity, float.NegativeInfinity);
        public static Vector3 one => new Vector3(1, 1, 1);
        public static Vector3 positiveInfinity => new Vector3(float.PositiveInfinity, float.PositiveInfinity, float.PositiveInfinity);
        public static Vector3 right => new Vector3(1, 0, 0);
        public static Vector3 up => new Vector3(0, 1, 0);
        public static Vector3 zero => new Vector3(0, 0, 0);


        // Properties
        public float magnitude
        {
            get
            {
                return Mathf.Sqrt(y * y + x * x + z * z);
            }
        }

        public Vector3 normalized
        {
            get
            {
                return Normalized(this);
            }
        }
        public float sqrMagnitude
        {
            get
            {
                return x * x + y * y + z * z;
            }
        }

        public float this[int index]
        {
            get
            {
                if (index == 0) return x;
                if (index == 1) return y;
                if (index == 2) return z;
                throw new IndexOutOfRangeException("Invalid index for Vector3");
            }
            set
            {
                if (index == 0) x = value;
                else if (index == 1) y = value;
                else if (index == 2) z = value;
                else throw new IndexOutOfRangeException("Invalid index for Vector3");
            }
        }


        // Constructor
        public Vector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public Vector3 (Vector3 vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
        }

        // Public Methods
        public override bool Equals(object obj)
        {
            return obj is Vector3 vector &&
                   x == vector.x &&
                   y == vector.y &&
                   z == vector.z;
        }

        public override int GetHashCode()
        {
            return (x, y, z).GetHashCode();
        }

        public void Set(float newX, float newY, float newZ)
        {
            x = newX;
            y = newY;
            z = newZ;
        }

        public override string ToString()
        {
            return $"({x}, {y}, {z})";
        }

        // Static Methods
        public static float Angle(Vector3 from, Vector3 to)
        {
            return Mathf.Acos(Mathf.Clamp(Dot(from.normalized, to.normalized), -1.0F, 1.0F)) * 57.29578F;
        }

        public static Vector3 ClampMagnitude(Vector3 vector, float maxLength)
        {
            if (vector.sqrMagnitude > maxLength * maxLength)
            {
                return vector.normalized * maxLength;
            }
            return vector;
        }
        public static Vector3 Cross(Vector3 lhs, Vector3 rhs)
        {
            return new Vector3(lhs.y * rhs.z - lhs.z * rhs.y,
                               lhs.z * rhs.x - lhs.x * rhs.z,
                              lhs.x * rhs.y - lhs.y * rhs.x);
        }

        public static float Distance(Vector3 a, Vector3 b)
        {
            return (a - b).magnitude;
        }

        public static float Dot(Vector3 lhs, Vector3 rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
        }

        //CatmullRom
        public static Vector3 CMSpline(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t)
        {
            t = Mathf.Clamp01(t);
            float t2 = t * t;
            float t3 = t2 * t;

            return 0.5f * (
                (2f * p1) +
                (-p0 + p2) * t +
                (2f * p0 - 5f * p1 + 4f * p2 - p3) * t2 +
                (-p0 + 3f * p1 - 3f * p2 + p3) * t3
            );
        }
        public static Vector3 CMSplineUnclamped(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;

            return 0.5f * (
                (2f * p1) +
                (-p0 + p2) * t +
                (2f * p0 - 5f * p1 + 4f * p2 - p3) * t2 +
                (-p0 + 3f * p1 - 3f * p2 + p3) * t3
            );
        }



        // Quadratic: 3 vectors
        public static Vector3 Bezier(Vector3 p0, Vector3 p1, Vector3 p2, float t)
        {
            t = Mathf.Clamp01(t);
            float u = 1 - t;
            return u * u * p0 + 2 * u * t * p1 + t * t * p2;
        }

        // Cubic: 4 vectors
        public static Vector3 Bezier(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t)
        {
            t = Mathf.Clamp01(t);
            float u = 1 - t;
            float tt = t * t;
            float uu = u * u;
            float uuu = uu * u;
            float ttt = tt * t;

            return (uuu * p0) + (3 * uu * t * p1) + (3 * u * tt * p2) + (ttt * p3);
        }

        public static Vector3 BezierUnclamped(Vector3 p0, Vector3 p1, Vector3 p2, float t)
        {
            float u = 1 - t;
            return u * u * p0 + 2 * u * t * p1 + t * t * p2;
        }

        public static Vector3 BezierUnclamped(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t)
        {
            float u = 1 - t;
            float tt = t * t;
            float uu = u * u;
            float uuu = uu * u;
            float ttt = tt * t;

            return (uuu * p0) + (3 * uu * t * p1) + (3 * u * tt * p2) + (ttt * p3);
        }

        public static Vector3 CubErp(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float t)
        {
            t = Mathf.Clamp01(t);
            float tt = t * t;
            float ttt = tt * t;

            return a * (-0.5f * t + ttt - 0.5f * tt) +
                   b * (1.5f * tt - 2.5f * ttt + 1.0f) +
                   c * (-1.5f * tt + 2.0f * ttt + 0.5f * t) +
                   d * (0.5f * tt - 0.5f * ttt);
        }
        public static Vector3 CubErpUnclamped(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float t)
        {
            float tt = t * t;
            float ttt = tt * t;

            return a * (-0.5f * t + ttt - 0.5f * tt) +
                   b * (1.5f * tt - 2.5f * ttt + 1.0f) +
                   c * (-1.5f * tt + 2.0f * ttt + 0.5f * t) +
                   d * (0.5f * tt - 0.5f * ttt);
        }

        public static Vector3 LogErp(Vector3 start, Vector3 end, float t)
        {
            t = Mathf.Clamp01(t);
            return start + (end - start) * Mathf.Log10(1 + 9 * t);
        }
        public static Vector3 LogErpUnclamped(Vector3 start, Vector3 end, float t)
        {
            return start + (end - start) * Mathf.Log10(1 + 9 * t);
        }

        public static Vector3 ExErp(Vector3 start, Vector3 end, float t)
        {
            t = Mathf.Clamp01(t);
            return Lerp(start, end, Mathf.Pow(2, 10 * (t - 1)));
        }

        public static Vector3 ExErpUnclamped(Vector3 start, Vector3 end, float t)
        {
            return Lerp(start, end, Mathf.Pow(2, 10 * (t - 1)));
        }


        public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
        {
            t = Mathf.Clamp01(t);
            return new Vector3(a.x + (b.x - a.x) * t,
                        a.y + (b.y - a.y) * t,
                        a.z + (b.z - a.z) * t);
        }

        public static Vector3 LerpUnclamped(Vector3 a, Vector3 b, float t)
        {
            return new Vector3(a.x + (b.x - a.x) * t,
                         a.y + (b.y - a.y) * t,
                         a.z + (b.z - a.z) * t);
        }

        public static Vector3 Max(Vector3 lhs, Vector3 rhs)
        {
            return new Vector3(Mathf.Max(lhs.x, rhs.x), Mathf.Max(lhs.y, rhs.y), Mathf.Max(lhs.z, rhs.z));
        }

        public static Vector3 Min(Vector3 lhs, Vector3 rhs)
        {
            return new Vector3(Mathf.Min(lhs.x, rhs.x), Mathf.Min(lhs.y, rhs.y), Mathf.Min(lhs.z, rhs.z));
        }

        public static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta)
        {
            Vector3 a = target - current;
            float magnitude = a.magnitude;
            if (magnitude <= maxDistanceDelta || magnitude == 0f)
            {
                return target;
            }
            return current + a / magnitude * maxDistanceDelta;
        }

        public static void Normalize(Vector3 value)
        {
            float magnitude = value.magnitude;
            if (magnitude > 1e-5)
            {
                value /= magnitude;
            }
            else
            {
                value = zero;
            }
        }

        public static Vector3 Project(Vector3 vector, Vector3 onNormal)
        {
            float num = Dot(onNormal, onNormal);
            if (num < Mathf.Epsilon)
            {
                return zero;
            }
            return onNormal * Dot(vector, onNormal) / num;
        }

        public static Vector3 ProjectOnPlane(Vector3 vector, Vector3 planeNormal)
        {
            return vector - Project(vector, planeNormal);
        }

        public static Vector3 Reflect(Vector3 inDirection, Vector3 inNormal)
        {
            return -2.0F * Dot(inNormal, inDirection) * inNormal + inDirection;
        }

        public static Vector3 Scale(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
        }

        public static float SignedAngle(Vector3 from, Vector3 to, Vector3 axis)
        {
            float num = Mathf.Acos(Mathf.Clamp(Dot(from.normalized, to.normalized), -1.0F, 1.0F)) * 57.29578F;
            float num2 = Mathf.Sign(Dot(axis, Cross(from, to)));
            return num * num2;
        }

        public static Vector3 Slerp(Vector3 a, Vector3 b, float t)
        {
            float num = Mathf.Clamp(Dot(a.normalized, b.normalized), -1.0F, 1.0F);
            float num2 = Mathf.Acos(num);
            if (num2 < 1E-06f)
            {
                return Lerp(a, b, t);
            }
            float num3 = 1.0F / Mathf.Sin(num2);
            return (Mathf.Sin((1.0F - t) * num2) * num3 * a + Mathf.Sin(t * num2) * num3 * b);
        }

        public static Vector3 SlerpUnclamped(Vector3 a, Vector3 b, float t)
        {
            float num = Mathf.Clamp(Dot(a.normalized, b.normalized), -1.0F, 1.0F);
            float num2 = Mathf.Acos(num);
            if (num2 < 1E-06f)
            {
                return Lerp(a, b, t);
            }
            float num3 = 1.0F / Mathf.Sin(num2);
            return (Mathf.Sin((1.0F - t) * num2) * num3 * a + Mathf.Sin(t * num2) * num3 * b);
        }

        public static Vector3 SmoothDamp(Vector3 current, Vector3 target, ref Vector3 currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
        {
            smoothTime = Mathf.Max(0.0001F, smoothTime);
            float num = 2.0F / smoothTime;
            float num2 = num * deltaTime;
            float num3 = 1.0F / (1.0F + num2 + 0.479999989F * num2 * num2 + 0.234999999F * num2 * num2 * num2);
            Vector3 vector = current - target;
            Vector3 vector2 = target;
            float maxLength = maxSpeed * smoothTime;
            vector = ClampMagnitude(vector, maxLength);
            target = current - vector;
            Vector3 vector3 = (currentVelocity + num * vector) * deltaTime;
            currentVelocity = (currentVelocity - num * vector3) * num3;
            Vector3 vector4 = target + (vector + vector3) * num3;
            if (Dot(vector2 - current, vector4 - vector2) > 0.0F)
            {
                vector4 = vector2;
                currentVelocity = (vector4 - vector2) / deltaTime;
            }
            return vector4;
        }

        // Operators
        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
        }
        public static Vector3 operator *(Vector3 a, float d)
        {
            return new Vector3(a.x * d, a.y * d, a.z * d);
        }

        public static Vector3 operator *(float d, Vector3 a)
        {
            return new Vector3(a.x * d, a.y * d, a.z * d);
        }

        public static Vector3 operator /(Vector3 a, float d)
        {
            return new Vector3(a.x / d, a.y / d, a.z / d);
        }

        public static Vector3 operator -(Vector3 a)
        {
            return new Vector3(-a.x, -a.y, -a.z);
        }

        public static bool operator ==(Vector3 lhs, Vector3 rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
        }

        public static bool operator !=(Vector3 lhs, Vector3 rhs)
        {
            return !(lhs == rhs);
        }

        private static Vector3 Normalized(Vector3 vector)
        {
            float magnitude = vector.magnitude;
            if (magnitude > 1e-5)
            {
                return vector / magnitude;
            }
            return zero;
        }


    }
}
