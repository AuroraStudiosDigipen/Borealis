using Microsoft.CodeAnalysis.CSharp.Syntax;
using System.Runtime.CompilerServices;

namespace Borealis
{
    public class MonoBehaviour : Behaviour
    {
        virtual public void Awake() { }
        virtual public void Start() { }
        virtual public void Update() { }
        virtual public void FixedUpdate() { }
        virtual public void LateUpdate() { }
        virtual public void OnEnable() { }
        virtual public void OnDisable() { }
        virtual public void OnDestroy() { }
        virtual public void OnApplicationQuit() { }
        virtual public void OnApplicationPause() { }
        virtual public void OnApplicationFocus() { }
        virtual public void OnGUI() { }
        virtual public void OnDrawGizmos() { }
        virtual public void OnDrawGizmosSelected() { }
        virtual public void OnValidate() { }
        virtual public void Reset() { }
        virtual public void OnBecameVisible() { }
        virtual public void OnBecameInvisible() { }
        virtual public void OnPreCull() { }
        virtual public void OnPreRender() { }
        virtual public void OnPostRender() { }
        virtual public void OnRenderObject() { }
        virtual public void OnWillRenderObject() { }
        virtual public void OnRenderImage() { }
        virtual public void OnAudioFilterRead() { }
        virtual public void OnParticleCollision() { }
        virtual public void OnJointBreak() { }
        virtual public void OnAnimatorMove() { }
        virtual public void OnAnimatorIK() { }

        private void SetInstanceID(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);
        }
        protected T GetComponent<T>() where T : Component, new()
        {
            T output = new T();
            output.gameObject = gameObject;
            output.transform = gameObject.transform;
            return output;
        }
        new private ulong GetInstanceID()
        {
            return InstanceID;
        }

    }
}
