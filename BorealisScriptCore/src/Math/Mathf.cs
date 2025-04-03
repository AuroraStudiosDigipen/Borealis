using System;

namespace Borealis
{
    public struct Mathf
    {
        public static float Deg2Rad
        {
            get
            {
                return (PI * 2) / 360;
            }
        }

        public static float Rad2Deg
        {
            get
            {
                return 360 / (PI * 2);
            }
        }

        public static float Epsilon
        {
            get
            {
                return 1.401298E-45f;
            }
        }

        public const float Infinity = float.PositiveInfinity;

        public static float NegativeInfinity
        {
            get
            {
                return float.NegativeInfinity;
            }
        }

        public static float PI
        {
            get
            {
                return 3.141593f;
            }
        }

        public static float Abs(float f)
        {
            return Math.Abs(f);
        }

        public static bool Approximately(float a, float b)
        {
            return Abs(b - a) < Max(1E-06f * Max(Abs(a), Abs(b)), Epsilon * 8);
        }

        public static float Acos(float f)
        {
            return (float)Math.Acos(f);
        }

        public static float Asin(float f)
        {
            return (float)Math.Asin(f);
        }

        public static float Atan(float f)
        {
            return (float)Math.Atan(f);
        }

        public static float Atan2(float y, float x)
        {
            return (float)Math.Atan2(y, x);
        }

        public static float Ceil(float f)
        {
            return (float)Math.Ceiling(f);
        }

        public static int CeilToInt(float f)
        {
            return (int)Math.Ceiling(f);
        }

        public static float Clamp(float value, float min, float max)
        {
            return value < min ? min : (value > max ? max : value);
        }

        public static float Clamp01(float value)
        {
            return value < 0 ? 0 : (value > 1 ? 1 : value);
        }

        public static int ClosestPowerOfTwo(int value)
        {
            return (int)Math.Pow(2, Math.Round(Math.Log(value) / Math.Log(2)));
        }

        public static Color CorrelatedColorTemperatureToRGB(float kelvin)
        {
            float temp = kelvin / 100.0f;

            float r = 0, g = 0, b = 0;

            if (temp <= 66)
            {
                r = 1.0f;
                g = 0.390f * temp - 0.148f;
                b = (temp <= 19) ? 0 : (temp - 10) * 0.07f;

                g = Clamp(g);
                b = Clamp(b);
            }
            else
            {
                r = 1.0f;
                g = 0.8f * (temp - 60) / 40.0f + 0.2f;
                b = 0.8f * (temp - 60) / 60.0f;

                g = Clamp(g);
                b = Clamp(b);
            }

            return new Color(r, g, b, 1.0f);
        }

        public static float Cos(float f)
        {
            return (float)Math.Cos(f);
        }

        public static float DeltaAngle(float current, float target)
        {
            float num = Repeat(target - current, 360);
            if (num > 180)
            {
                num -= 360;
            }
            return num;
        }

        public static float Exp(float power)
        {
            return (float)Math.Exp(power);
        }

        public static ushort FloatToHalf(float val)
        {
            // Convert the float to an integer bit representation
            uint floatBits = BitConverter.ToUInt32(BitConverter.GetBytes(val), 0);

            // Extract the sign, exponent, and mantissa
            uint sign = (floatBits >> 31) & 0x1;
            uint exponent = (floatBits >> 23) & 0xFF;
            uint mantissa = floatBits & 0x7FFFFF;

            // Handle special cases
            if (exponent == 0xFF)
            {
                // Inf or NaN
                return (ushort)((((ushort)sign) << 15) | 0x7C00 | (mantissa != 0 ? 0x0200 : 0x0000));
            }

            // Handle zero and denormalized numbers
            if (exponent == 0)
            {
                return (ushort)(sign << 15);
            }

            // Normalized number
            int halfExponent = (int)exponent - 127 + 15; // Exponent adjustment
            if (halfExponent >= 0x1F)
            {
                // Overflow, return infinity
                return (ushort)((sign << 15) | 0x7C00);
            }
            if (halfExponent <= 0)
            {
                // Underflow, return zero
                return (ushort)(sign << 15);
            }

            // Round the mantissa to fit 10 bits
            uint halfMantissa = mantissa >> 13;

            return (ushort)((((ushort)sign) << 15) | (((ushort)halfExponent) << 10) | ((ushort)halfMantissa));
        }

        private static readonly int[] perm = {
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
        8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
        35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
        134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
        55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
        18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
        250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
        189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,
        43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,
        97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,
        239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,
        254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
        };

        private static int Hash(int x) => perm[x & 255];

        private static float Grad(int hash, float x, float y)
        {
            int h = hash & 3;
            float u = h < 2 ? x : y;
            float v = h < 2 ? y : x;
            return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? 2 * v : -2 * v);
        }

        private static float Fade(float t) => t * t * t * (t * (t * 6 - 15) + 10);
        public static float PerlinNoise(float x, float y)
        {
            int X = Mathf.FloorToInt(x) & 255;
            int Y = Mathf.FloorToInt(y) & 255;

            x -= Mathf.Floor(x);
            y -= Mathf.Floor(y);

            float u = Fade(x);
            float v = Fade(y);

            int aa = Hash(X) + Y;
            int ab = Hash(X) + Y + 1;
            int ba = Hash(X + 1) + Y;
            int bb = Hash(X + 1) + Y + 1;

            float gradAA = Grad(Hash(aa), x, y);
            float gradBA = Grad(Hash(ba), x - 1, y);
            float gradAB = Grad(Hash(ab), x, y - 1);
            float gradBB = Grad(Hash(bb), x - 1, y - 1);

            float lerpX1 = Lerp(gradAA, gradBA, u);
            float lerpX2 = Lerp(gradAB, gradBB, u);
            return Lerp(lerpX1, lerpX2, v) * 0.5f + 0.5f;
        }
        public static float Floor(float f)
        {
            return (float)Math.Floor(f);
        }

        public static int FloorToInt(float f)
        {
            return (int)Math.Floor(f);
        }

        public static float GammaToLinearSpace(float value)
        {
            const float gamma = 2.2f; // Typical gamma value used in sRGB

            if (value <= 0.04045f)
            {
                // For very dark values, use the linear approximation
                return value / 12.92f;
            }
            else
            {
                // Apply the gamma correction formula
                return (float)Math.Pow((value + 0.055f) / 1.055f, gamma);
            }
        }

        public static float HalfToFloat(ushort val)
        {
            uint sign = (uint)((val >> 15) & 0x1);
            uint exponent = (uint)((val >> 10) & 0x1F);
            uint mantissa = (uint)(val & 0x3FF);

            if (exponent == 0x1F)
            {
                // Inf or NaN
                if (mantissa == 0)
                {
                    return sign == 0 ? float.PositiveInfinity : float.NegativeInfinity;
                }
                else
                {
                    return float.NaN;
                }
            }

            if (exponent == 0)
            {
                // Zero or denormalized number
                if (mantissa == 0)
                {
                    return sign == 0 ? 0.0f : -0.0f;
                }
                else
                {
                    // Denormalized number
                    float denormalized = (float)mantissa / (1 << 10);
                    return (sign == 0 ? 1 : -1) * denormalized * (1 << -14);
                }
            }

            // Normalized number
            float normalized = (float)(mantissa | (1 << 10)) / (1 << 10);
            float exponentAdjustment = (float)(exponent - 15 + 127);
            return (sign == 0 ? 1 : -1) * normalized * (float)Math.Pow(2, exponentAdjustment - 127);
        }
    
        public static float InverseLerp(float a, float b, float value)
        {
            if (a == b)
            {
                return 0f;
            }
            return (value - a) / (b - a);
        }

        public static bool IsPowerOfTwo(int value)
        {
            return (value > 0) && (value & (value - 1)) == 0;
        }

        public static float Lerp(float a, float b, float t)
        {
            return a + (b - a) * Clamp01(t);
        }

        public static float LerpAngle(float a, float b, float t)
        {
            float delta = Repeat(b - a, 360f);
            if (delta > 180f)
            {
                delta -= 360f;
            }
            return a + delta * Clamp01(t);
        }

        public static float LerpUnclamped(float a, float b, float t)
        {
            return a + (b - a) * t;
        }

        public static float LinearToGammaSpace(float value)
        {
            return (value <= 0.0031308f) ? value * 12.92f : 1.055f * (float)Math.Pow(value, 1f / 2.4f) - 0.055f;
        }


        public static float Log(float f)
        {
            return (float)Math.Log(f);
        }

        public static float Log(float f, float p)
        {
            return (float)Math.Log(f, p);
        }

        public static float Log10(float f)
        {
            return (float)Math.Log10(f);
        }

        public static float Max(float a, float b)
        {
            return a > b ? a : b;
        }

        public static float Min(float a, float b)
        {
            return a < b ? a : b;
        }

        public static float MoveTowards(float current, float target, float maxDelta)
        {
            return (Math.Abs(target - current) <= maxDelta) ? target : current + Math.Sign(target - current) * maxDelta;
        }

        public static float MoveTowardsAngle(float current, float target, float maxDelta)
        {
            float delta = DeltaAngle(current, target);
            return MoveTowards(current, current + delta, maxDelta);
        }

        public static int NextPowerOfTwo(int value)
        {
            if (value < 1) return 1;
            value--;
            value |= value >> 1;
            value |= value >> 2;
            value |= value >> 4;
            value |= value >> 8;
            value |= value >> 16;
            return value + 1;
        }

        public static float PerlinNoise(float x, float y)
        {
            // Implementing a simple 2D Perlin noise algorithm or using a library
            return 0f; // Placeholder
        }

        public static float PerlinNoise1D(float x)
        {
            // Implementing a simple 1D Perlin noise algorithm or using a library
            return 0f; // Placeholder
        }

        public static float PingPong(float value, float length)
        {
            return length - Math.Abs((value % (2 * length)) - length);
        }

        public static float Pow(float f, float p)
        {
            return (float)Math.Pow(f, p);
        }

        public static float Repeat(float t, float length)
        {
            return t - Floor(t / length) * length;
        }

        public static float Round(float f)
        {
            return (float)Math.Round(f);
        }

        public static int RoundToInt(float f)
        {
            return (int)Math.Round(f);
        }

        public static float Sign(float f)
        {
            return (f > 0f) ? 1f : (f < 0f) ? -1f : 0f;
        }

        public static float Sin(float f)
        {
            return (float)Math.Sin(f);
        }

        public static float SmoothDamp(float current, float target, ref float velocity, float smoothTime, float maxSpeed = float.PositiveInfinity, float deltaTime = 0.02f)
        {
            smoothTime = Math.Max(0.0001f, smoothTime);
            float omega = 2f / smoothTime;
            float x = omega * deltaTime;
            float exp = 1f / (1f + x + 0.48f * x * x + 0.235f * x * x * x);
            float change = current - target;
            float originalTo = target;
            float maxChange = maxSpeed * smoothTime;
            change = ClampMagnitude(change, maxChange);
            target = current - change;
            float temp = (velocity + omega * change) * deltaTime;
            velocity = (velocity - omega * temp) * exp;
            float output = target + (change + temp) * exp;
            if (Dot(originalTo - current, output - originalTo) > 0f)
            {
                output = originalTo;
                velocity = (output - originalTo) / deltaTime;
            }
            return output;
        }

        public static float SmoothDampAngle(float current, float target, ref float velocity, float smoothTime, float maxSpeed = float.PositiveInfinity, float deltaTime = 0.02f)
        {
            target = Repeat(target - current, 360f) + current;
            return SmoothDamp(current, target, ref velocity, smoothTime, maxSpeed, deltaTime);
        }

        public static float SmoothStep(float edge0, float edge1, float x)
        {
            float t = Clamp01((x - edge0) / (edge1 - edge0));
            return t * t * (3f - 2f * t);
        }

        public static float Sqrt(float f)
        {
            return (float)Math.Sqrt(f);
        }

        public static float Tan(float f)
        {
            return (float)Math.Tan(f);
        }
        private static float ClampMagnitude(float vector, float maxLength)
        {
            if (vector > maxLength)
            {
                return maxLength;
            }
            return vector;
        }

        private static float Dot(float a, float b)
        {
            return a * b;
        }
        private static float Clamp(float value)
        {
            return Math.Max(0.0f, Math.Min(1.0f, value));
        }
    };

}