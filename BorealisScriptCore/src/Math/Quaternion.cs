using System;
using System.IO;
using System.Runtime.CompilerServices;

namespace Borealis
{
    public struct Quaternion
    {
        public static Quaternion identity => new Quaternion(0, 0, 0, 1);
        public static Quaternion Default()
        {
            return new Quaternion(0, 0, 0, 1);
        }

        public Quaternion(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        public Quaternion (Vector3 rotation)
        {
            Vector3 c = new Vector3(Mathf.Cos(Mathf.Deg2Rad * rotation.x * 0.5f), Mathf.Cos(Mathf.Deg2Rad * rotation.y * 0.5f), Mathf.Cos(Mathf.Deg2Rad * rotation.z * 0.5f) );
            Vector3 s = new Vector3(Mathf.Sin(Mathf.Deg2Rad * rotation.x * 0.5f), Mathf.Sin(Mathf.Deg2Rad * rotation.y * 0.5f), Mathf.Sin(Mathf.Deg2Rad * rotation.z * 0.5f));

            w = c.x * c.y * c.z + s.x * s.y * s.z;
            x = s.x * c.y * c.z - c.x * s.y * s.z;
            y = c.x * s.y * c.z + s.x * c.y * s.z;
            z = c.x * c.y * s.z - s.x * s.y * c.z;
        }

        public static Quaternion QuaternionFromAxes(Vector3 right, Vector3 up, Vector3 forward)
        {
            float r11 = right.x, r12 = right.y, r13 = right.z;
            float r21 = up.x, r22 = up.y, r23 = up.z;
            float r31 = forward.x, r32 = forward.y, r33 = forward.z;

            float x = 0, y = 0, z = 0, w = 0;
            float trace = r11 + r22 + r33;
            if (trace > 0)
            {
                float s = 0.5f / Mathf.Sqrt(trace + 1.0f);
                w = 0.25f / s;
                x = (r32 - r23) * s;
                y = (r13 - r31) * s;
                z = (r21 - r12) * s;
            }
            else
            {
                if (r11 > r22 && r11 > r33)
                {
                    float s = 2.0f * Mathf.Sqrt(1.0f + r11 - r22 - r33);
                    w = (r32 - r23) / s;
                    x = 0.25f * s;
                    y = (r12 + r21) / s;
                    z = (r13 + r31) / s;
                }
                else if (r22 > r33)
                {
                    float s = 2.0f * Mathf.Sqrt(1.0f + r22 - r11 - r33);
                    w = (r13 - r31) / s;
                    x = (r12 + r21) / s;
                    y = 0.25f * s;
                    z = (r23 + r32) / s;
                }
                else
                {
                    float s = 2.0f * Mathf.Sqrt(1.0f + r33 - r11 - r22);
                    w = (r21 - r12) / s;
                    x = (r13 + r31) / s;
                    y = (r23 + r32) / s;
                    z = 0.25f * s;
                }


                //float trace = right.x + up.y + forward.z;
                //float x = 0, w = 0, y = 0, z = 0;
                //if (trace > 0.0f)
                //{
                //    w = Mathf.Sqrt(1.0f + right.x + up.y + forward.z) / 2.0f;
                //    x = (forward.y - up.z) / (4.0f * w);
                //    y = (right.z - forward.x) / (4.0f * w);
                //    z = (up.x - right.y) / (4.0f * w);
                //}
                //else
                //{
                //    if (right.x > up.y && right.x > forward.z)
                //    {
                //        x = Mathf.Sqrt(1.0f + right.x - up.y - forward.z) / 2.0f;
                //        w = (forward.y - up.z) / (4.0f * x);
                //        y = (right.y + up.x) / (4.0f * x);
                //        z = (right.z + forward.x) / (4.0f * x);
                //    }

                //    if (up.y > right.x && up.y > forward.z)
                //    {
                //        y = Mathf.Sqrt(1.0f + up.y - right.x - forward.z) / 2.0f;
                //        w = (right.z - forward.x) / (4.0f * y);
                //        x = (right.y + up.x) / (4.0f * y);
                //        z = (up.z + forward.y) / (4.0f * y);
                //    }

                //    if (forward.z > right.x && forward.z > up.y)
                //    {
                //        z = Mathf.Sqrt(1.0f + forward.z - up.y - right.x) / 2.0f;
                //        w = (up.x - right.y) / (4.0f * z);
                //        x = (right.z + forward.x) / (4.0f * z);
                //        y = (up.z + forward.y) / (4.0f * z);
                //    }
                //}
            }
                return new Quaternion(x, y, z, w);

        }
        private Vector3 threeaxisrot(float r11, float r12, float r21, float r31, float r32)
        {
            float x = Mathf.Atan2(r31, r32);
            float y = Mathf.Asin(r21);
            float z = Mathf.Atan2(r11, r12);
            return new Vector3(x * Mathf.Rad2Deg, y * Mathf.Rad2Deg, z * Mathf.Rad2Deg);
        }
        public Vector3 eulerAngles
        {
            get
            {
                return threeaxisrot(2 * (x * y + w * z),
                     w * w + x * x - y * y - z * z,
                    -2 * (x * z - w * y),
                     2 * (y * z + w * x),
                     w * w - x * x - y * y + z * z);
            }
            set
            {

                Vector3 c = new Vector3(Mathf.Cos(Mathf.Deg2Rad * value.x * 0.5f), Mathf.Cos(Mathf.Deg2Rad * value.y * 0.5f), Mathf.Cos(Mathf.Deg2Rad * value.z * 0.5f));
                Vector3 s = new Vector3(Mathf.Sin(Mathf.Deg2Rad * value.x * 0.5f), Mathf.Sin(Mathf.Deg2Rad * value.y * 0.5f), Mathf.Sin(Mathf.Deg2Rad * value.z * 0.5f));

                w = c.x * c.y * c.z + s.x * s.y * s.z;
                x = s.x * c.y * c.z - c.x * s.y * s.z;
                y = c.x * s.y * c.z + s.x * c.y * s.z;
                z = c.x * c.y * s.z - s.x * s.y * c.z;
            }
        }
        public float this[int index]
        {
            get
            {
                if (index == 0) return x;
                if (index == 1) return y;
                if (index == 2) return z;
                if (index == 3) return w;
                throw new IndexOutOfRangeException("Invalid index for Quaternion");
            }
            set
            {
                if (index == 0) x = value;
                else if (index == 1) y = value;
                else if (index == 2) z = value;
                else if (index == 3) w = value;
                else throw new IndexOutOfRangeException("Invalid index for Quaternion");
            }
        }

        public float w;
        public float x;
        public float y;
        public float z;

        public void Set(float newX, float newY, float newZ, float newW)
        {
            x = newX;
            y = newY;
            z = newZ;
            w = newW;
        }

        //public void SetFromToRotation(Vector3 fromDirection, Vector3 toDirection)
        //{
        //    Quaternion q = FromToRotation(fromDirection, toDirection);
        //    Set(q.x, q.y, q.z, q.w);
        //}

        public void SetLookRotation(Vector3 view, Vector3 up = default)
        {
            if (up == default)
                up = Vector3.up;
            Quaternion q = LookRotation(view, up);
            Set(q.x, q.y, q.z, q.w);
        }

        public void ToAngleAxis(out float angle, out Vector3 axis)
        {
            // Normalize quaternion
            Quaternion q = Normalize(this);

            float halfAngle = Mathf.Acos(q.w);
            angle = 2.0f * Mathf.Rad2Deg * halfAngle;
            float s = Mathf.Sqrt(1.0f - q.w * q.w);
            if (s < 0.001f)
            {
                axis = new Vector3(q.x, q.y, q.z); // Use this vector for axis
            }
            else
            {
                axis = new Vector3(q.x / s, q.y / s, q.z / s);
            }
        }

        public override string ToString()
        {
            return $"({x}, {y}, {z}, {w})";
        }

        public static float Angle(Quaternion a, Quaternion b)
        {
            float dot = Dot(a, b);
            return Mathf.Acos(Mathf.Clamp(2.0f * dot * dot - 1.0f, -1.0f, 1.0f)) * Mathf.Rad2Deg;
        }

        public static float Dot(Quaternion a, Quaternion b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }


        public static Quaternion Euler(float x, float y, float z)
        {
            Quaternion q = new Quaternion();
            q.eulerAngles = new Vector3(x, y, z);
            return q;
        }

        //public static Quaternion FromToRotation(Vector3 fromDirection, Vector3 toDirection)
        //{
        //    Vector3 axis = Vector3.Cross(fromDirection, toDirection);
        //    float angle = Mathf.Acos(Mathf.Clamp(Vector3.Dot(fromDirection.normalized, toDirection.normalized), -1.0f, 1.0f));
        //    return AngleAxis(angle * Mathf.Rad2Deg, axis);
        //}

        //private static Quaternion AngleAxis(float angle, Vector3 axis)
        //{
        //    // Normalize the axis
        //    axis = axis.normalized;

        //    // Convert angle from degrees to radians
        //    float halfAngle = Mathf.Deg2Rad * angle * 0.5f;

        //    // Compute the sine of the angle
        //    float sinHalfAngle = Mathf.Sin(halfAngle);

        //    // Compute the quaternion components
        //    float w = Mathf.Cos(halfAngle);
        //    float x = axis.x * sinHalfAngle;
        //    float y = axis.y * sinHalfAngle;
        //    float z = axis.z * sinHalfAngle;

        //    return new Quaternion(x, y, z, w);
        //}

        public static Quaternion Inverse(Quaternion rotation)
        {
            return new Quaternion(-rotation.x, -rotation.y, -rotation.z, rotation.w);
        }

        public static Quaternion Lerp(Quaternion a, Quaternion b, float t)
        {
            t = Mathf.Clamp01(t);
            return new Quaternion(
                Mathf.Lerp(a.x, b.x, t),
                Mathf.Lerp(a.y, b.y, t),
                Mathf.Lerp(a.z, b.z, t),
                Mathf.Lerp(a.w, b.w, t)
            );
        }

        public static Quaternion LerpUnclamped(Quaternion a, Quaternion b, float t)
        {
            return new Quaternion(
                Mathf.LerpUnclamped(a.x, b.x, t),
                Mathf.LerpUnclamped(a.y, b.y, t),
                Mathf.LerpUnclamped(a.z, b.z, t),
                Mathf.LerpUnclamped(a.w, b.w, t)
            );
        }

        public static Quaternion LookRotation(Vector3 forward, Vector3 upwards = default)
        {
            if (upwards == default)
                upwards = Vector3.up;

            // Ensure forward and upwards are normalized
            Vector3 normalizedForward = forward.normalized;
            Vector3 normalizedUpwards = upwards.normalized;
            
            // Compute the right vector (cross product of up and forward)
            Vector3 right = Vector3.Cross(normalizedUpwards, normalizedForward);

            // Recompute the up vector (cross product of forward and right) to ensure orthonormal vectors
            normalizedUpwards = Vector3.Cross(normalizedForward, right);

            // Now we can build the quaternion from the orthonormal basis (right, up, forward)
            // Assuming you have a constructor or method to create a quaternion from these three vectors
            return QuaternionFromAxes(right, normalizedUpwards, normalizedForward);
        }

        public static Quaternion Normalize(Quaternion q)
        {
            float magnitude = Mathf.Sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
            return new Quaternion(q.x / magnitude, q.y / magnitude, q.z / magnitude, q.w / magnitude);
        }

        public static Quaternion Slerp(Quaternion a, Quaternion b, float t)
        {
            float dot = Dot(a, b);
            dot = Mathf.Clamp(dot, -1.0f, 1.0f);

            float theta = Mathf.Acos(dot) * Mathf.Rad2Deg;
            float sinTheta = Mathf.Sin(theta);

            if (Mathf.Abs(sinTheta) < 0.001f)
            {
                return Lerp(a, b, t);
            }

            float aCoeff = Mathf.Sin((1 - t) * theta) / sinTheta;
            float bCoeff = Mathf.Sin(t * theta) / sinTheta;

            return new Quaternion(
                aCoeff * a.x + bCoeff * b.x,
                aCoeff * a.y + bCoeff * b.y,
                aCoeff * a.z + bCoeff * b.z,
                aCoeff * a.w + bCoeff * b.w
            );
        }

        public static Quaternion RotateTowards(Quaternion from, Quaternion to, float maxDegreesDelta)
        {
            float angle = Angle(from, to);
            if (angle == 0.0f)
                return to;

            float t = Mathf.Min(1.0f, maxDegreesDelta / angle);
            return Slerp(from, to, t);
        }

        public static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t)
        {
            float dot = Dot(a, b);
            dot = Mathf.Clamp(dot, -1.0f, 1.0f);

            float theta = Mathf.Acos(dot);
            float sinTheta = Mathf.Sin(theta);

            if (Mathf.Abs(sinTheta) < 0.001f)
            {
                return LerpUnclamped(a, b, t);
            }

            float aCoeff = Mathf.Sin((1 - t) * theta) / sinTheta;
            float bCoeff = Mathf.Sin(t * theta) / sinTheta;

            return new Quaternion(
                aCoeff * a.x + bCoeff * b.x,
                aCoeff * a.y + bCoeff * b.y,
                aCoeff * a.z + bCoeff * b.z,
                aCoeff * a.w + bCoeff * b.w
            );
        }


        public static Quaternion operator *(Quaternion lhs, Quaternion rhs)
        {
            return new Quaternion(
                lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
                lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
                lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
                lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
            );
        }

        public static Vector3 operator *(Quaternion rotation, Vector3 point)
        {
            float num = rotation.x * 2f;
            float num2 = rotation.y * 2f;
            float num3 = rotation.z * 2f;
            float num4 = rotation.x * num;
            float num5 = rotation.y * num2;
            float num6 = rotation.z * num3;
            float num7 = rotation.x * num2;
            float num8 = rotation.x * num3;
            float num9 = rotation.y * num3;
            float num10 = rotation.w * num;
            float num11 = rotation.w * num2;
            float num12 = rotation.w * num3;
            Vector3 result;
            result.x = (1f - (num5 + num6)) * point.x + (num7 - num12) * point.y + (num8 + num11) * point.z;
            result.y = (num7 + num12) * point.x + (1f - (num4 + num6)) * point.y + (num9 - num10) * point.z;
            result.z = (num8 - num11) * point.x + (num9 + num10) * point.y + (1f - (num4 + num5)) * point.z;
            return result;
        }

        public static bool operator ==(Quaternion lhs, Quaternion rhs)
        {
            return Mathf.Approximately(lhs.x, rhs.x) && Mathf.Approximately(lhs.y, rhs.y) &&
                   Mathf.Approximately(lhs.z, rhs.z) && Mathf.Approximately(lhs.w, rhs.w);
        }

        public static bool operator !=(Quaternion lhs, Quaternion rhs)
        {
            return !(lhs == rhs);
        }

        public override bool Equals(object obj)
        {
            if (obj is Quaternion)
            {
                Quaternion other = (Quaternion)obj;
                return Mathf.Approximately(x, other.x) && Mathf.Approximately(y, other.y) &&
                       Mathf.Approximately(z, other.z) && Mathf.Approximately(w, other.w);
            }
            return false;
        }

        public override int GetHashCode()
        {
            return (x, y, z, w).GetHashCode();
        }
    }
}
