using Borealis;
using System.Runtime.InteropServices.WindowsRuntime;

namespace Borealis
{
    public enum EmitterShape
    {
        Cone,
        Quad
    }
    [NativeComponent]
    public class ParticleSystem : Component
    {
        public float duration
        {
            get
            {
                InternalCalls.ParticleSystem_GetDuration(GetInstanceID(), out float duration);
                return duration;
            }
            set
            {
                InternalCalls.ParticleSystem_SetDuration(GetInstanceID(), ref value);
            }
        }
        public bool looping
        {
            get
            {
                InternalCalls.ParticleSystem_GetLooping(GetInstanceID(), out bool looping);
                return looping;
            }
            set
            {
                InternalCalls.ParticleSystem_SetLooping(GetInstanceID(), ref value);
            }
        }
        public float startDelay
        {
            get
            {
                InternalCalls.ParticleSystem_GetStartDelay(GetInstanceID(), out float delay);
                return delay;
            }
            set
            {
                InternalCalls.ParticleSystem_SetStartDelay(GetInstanceID(), ref value);
            }
        }
        public float startLifeTime
        { 
            get 
            {
                InternalCalls.ParticleSystem_GetStartLifeTime(GetInstanceID(), out float lifetime);
                return lifetime;
            } 
            set
            {
                InternalCalls.ParticleSystem_SetStartLifeTime(GetInstanceID(), ref value);
            }
        }
        public float startSpeed
        {
            get
            {
                InternalCalls.ParticleSystem_GetStartSpeed(GetInstanceID(), out float speed);
                return speed;
            }
            set
            {
                InternalCalls.ParticleSystem_SetStartSpeed(GetInstanceID(), ref value);
            }
        }
        public bool _3DStartSizeBool
        {
            get
            {
                InternalCalls.ParticleSystem_Get3DStartSizeBool(GetInstanceID(), out bool startSize);
                return startSize;
            }
            set
            {
                InternalCalls.ParticleSystem_Set3DStartSizeBool(GetInstanceID(), ref value);
            }
        }
        public bool randomStartSize
        {
            get
            {
                InternalCalls.ParticleSystem_GetRandomStartSize(GetInstanceID(), out bool startSize);
                return startSize;
            }
            set
            {
                InternalCalls.ParticleSystem_SetRandomStartSize(GetInstanceID(), ref value);
            }
        }
        public Vector3 startSize
        {
            get
            {
                InternalCalls.ParticleSystem_GetStartSize(GetInstanceID(), out Vector3 startSize);
                return startSize;
            }
            set
            {
                InternalCalls.ParticleSystem_SetStartSize(GetInstanceID(), ref value);
            }
        }
        public Vector3 startSize2
        {
            get
            {
                InternalCalls.ParticleSystem_GetStartSize2(GetInstanceID(), out Vector3 startSize);
                return startSize;
            }
            set
            {
                InternalCalls.ParticleSystem_SetStartSize2(GetInstanceID(), ref value);
            }
        }
        public bool _3DStartRotationBool
        {
            get
            {
                InternalCalls.ParticleSystem_Get3DRandomStartRotation(GetInstanceID(), out bool startRotation);
                return startRotation;
            }
            set
            {
                InternalCalls.ParticleSystem_Set3DRandomStartRotation(GetInstanceID(), ref value);
            }
        }
        public bool randomStartRotation
        {
            get
            {
                InternalCalls.ParticleSystem_GetRandomStartRotation(GetInstanceID(), out bool startRotation);
                return startRotation;
            }
            set
            {
                InternalCalls.ParticleSystem_SetRandomStartRotation(GetInstanceID(), ref value);
            }
        }
        public Vector3 startRotation
        {
            get
            {
                InternalCalls.ParticleSystem_GetStartRotation(GetInstanceID(), out Vector3 startSize);
                return startSize;
            }
            set
            {
                InternalCalls.ParticleSystem_SetStartRotation(GetInstanceID(), ref value);
            }
        }
        public Vector3 startRotation2
        {
            get
            {
                InternalCalls.ParticleSystem_GetStartRotation2(GetInstanceID(), out Vector3 startSize);
                return startSize;
            }
            set
            {
                InternalCalls.ParticleSystem_SetStartRotation2(GetInstanceID(), ref value);
            }
        }
        public bool randomStartColor
        {
            get
            {
                InternalCalls.ParticleSystem_GetRandomStartColor(GetInstanceID(), out bool color);
                return color;
            }
            set
            {
                InternalCalls.ParticleSystem_SetRandomStartColor(GetInstanceID(), ref value);
            }
        }
        public Color startColor
        {
            get
            {
                InternalCalls.ParticleSystem_GetStartColor(GetInstanceID(), out Color StartColor2);
                return StartColor2;
            }
            set
            {
                InternalCalls.ParticleSystem_SetStartColor(GetInstanceID(), ref value);
            }
        }
        public Color startColor2
        {
            get
            {
                InternalCalls.ParticleSystem_GetStartColor2(GetInstanceID(), out Color StartColor2);
                return StartColor2;
            }
            set
            {
                InternalCalls.ParticleSystem_SetStartColor2(GetInstanceID(), ref value);
            }
        }
        public bool endColorBool
        {
            get
            {
                InternalCalls.ParticleSystem_GetEndColorBool(GetInstanceID(), out bool color);
                return color;
            }
            set
            {
                InternalCalls.ParticleSystem_SetEndColorBool(GetInstanceID(), ref value);
            }
        }
        public Color endColor
        {
            get
            {
                InternalCalls.ParticleSystem_GetEndColor(GetInstanceID(), out Color StartColor2);
                return StartColor2;
            }
            set
            {
                InternalCalls.ParticleSystem_SetEndColor(GetInstanceID(), ref value);
            }
        }
        public float gravityModifer
        {
            get
            {
                InternalCalls.ParticleSystem_GetGravityModifier(GetInstanceID(), out float gravity);
                return gravity;
            }
            set
            {
                InternalCalls.ParticleSystem_SetGravityModifier(GetInstanceID(), ref value);
            }
        }
        public float simulationSpeed
        {
            get
            {
                InternalCalls.ParticleSystem_GetSimulationSpeed(GetInstanceID(), out float speed);
                return speed;
            }
            set
            {
                InternalCalls.ParticleSystem_SetSimulationSpeed(GetInstanceID(), ref value);
            }
        }
        public uint maxParticles
        {
            get
            {
                InternalCalls.ParticleSystem_GetMaxParticles(GetInstanceID(), out uint max);
                return max;
            }
            set
            {
                InternalCalls.ParticleSystem_SetMaxParticles(GetInstanceID(), ref value);
            }
        }
        public float rateOverTime
        {
            get
            {
                InternalCalls.ParticleSystem_GetRateOverTime(GetInstanceID(), out float speed);
                return speed;
            }
            set
            {
                InternalCalls.ParticleSystem_SetRateOverTime(GetInstanceID(), ref value);
            }
        }

        public EmitterShape emitterShape
        {
            get
            {
                InternalCalls.ParticleSystem_GetShape(GetInstanceID(), out int shape);
                return (EmitterShape)shape;
            }
            set
            {
                InternalCalls.ParticleSystem_SetShape(GetInstanceID(), ref value);
            }
        }

        //cone
        public float angle
        {
            get
            {
                InternalCalls.ParticleSystem_GetAngle(GetInstanceID(), out float angle);
                return angle;
            }
            set
            {
                InternalCalls.ParticleSystem_SetAngle(GetInstanceID(), ref value);
            }
        }
        public float radius
        {
            get
            {
                InternalCalls.ParticleSystem_GetRadius(GetInstanceID(), out float radius);
                return radius;
            }
            set
            {
                InternalCalls.ParticleSystem_SetRadius(GetInstanceID(), ref value);
            }
        }
        public float radiusThickness
        {
            get
            {
                InternalCalls.ParticleSystem_GetRadiusThickness(GetInstanceID(), out float thickness);
                return thickness;
            }
            set
            {
                InternalCalls.ParticleSystem_SetRadiusThickness(GetInstanceID(), ref value);
            }
        }

        //quad
        public Vector3 scale
        {
            get
            {
                InternalCalls.ParticleSystem_GetScale(GetInstanceID(), out Vector3 scale);
                return scale;
            }
            set
            {
                InternalCalls.ParticleSystem_SetScale(GetInstanceID(), ref value);
            }
        }
        public Vector3 rotation
        {
            get
            {
                InternalCalls.ParticleSystem_GetRotation(GetInstanceID(), out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.ParticleSystem_SetRotation(GetInstanceID(), ref value);
            }
        }

        public bool billboard
        {
            get
            {
                InternalCalls.ParticleSystem_GetBillboard(GetInstanceID(), out bool bill);
                return bill;
            }
            set
            {
                InternalCalls.ParticleSystem_SetBillboard(GetInstanceID(), ref value);
            }
        }

        //noise
        public bool useNoise
        {
            get
            {
                InternalCalls.ParticleSystem_GetUseNoise(GetInstanceID(), out bool noise);
                return noise;
            }
            set
            {
                InternalCalls.ParticleSystem_SetUseNoise(GetInstanceID(), ref value);
            }
        }
        public float noiseStrength
        {
            get
            {
                InternalCalls.ParticleSystem_GetNoiseStrength(GetInstanceID(), out float noise);
                return noise;
            }
            set
            {
                InternalCalls.ParticleSystem_SetNoiseStrength(GetInstanceID(), ref value);
            }
        }
        public float noiseFrequency
        {
            get
            {
                InternalCalls.ParticleSystem_GetNoiseFrequency(GetInstanceID(), out float noise);
                return noise;
            }
            set
            {
                InternalCalls.ParticleSystem_SetNoiseFrequency(GetInstanceID(), ref value);
            }
        }
        public float noiseScrollSpeed
        {
            get
            {
                InternalCalls.ParticleSystem_GetNoiseScrollSpeed(GetInstanceID(), out float noise);
                return noise;
            }
            set
            {
                InternalCalls.ParticleSystem_SetNoiseScrollSpeed(GetInstanceID(), ref value);
            }
        }

        public Sprite texture
        {
            get
            {
                InternalCalls.ParticleSystem_GetTexture(GetInstanceID(), out ulong textureID);
                return new Sprite(textureID);
            }
            set
            {
                InternalCalls.ParticleSystem_SetTexture(GetInstanceID(), value.GetInstanceID());
            }
        }
        public void Start()
        {
            InternalCalls.ParticleSystem_Start(InstanceID);
        }

        public void Stop()
        {
            InternalCalls.ParticleSystem_Stop(InstanceID);
        }
    }
}
