
namespace Borealis
{
    public class CameraOcclusion : MonoBehaviour
    {
        public float DefaultZoom = 5f;
        public int OcclusionMask;
        public float ZoomInterpolant = 0.15f;


        public GameObject CamYawPivot;

        public override void Update()
        {
            Vector3 occlusionRayDir = (transform.position - CamYawPivot.transform.position).normalized;
            Ray occlusionRay = new Ray(CamYawPivot.transform.position, occlusionRayDir);

            RaycastHit[] results = Physics.RaycastAll(occlusionRay, DefaultZoom, OcclusionMask);

            float targetZoom = DefaultZoom;

            foreach (RaycastHit result in results)
            {
                targetZoom = Mathf.Min(result.distance, DefaultZoom);
            }

            transform.position = Vector3.Lerp(transform.position, new Vector3(0f, 0f, -targetZoom), ZoomInterpolant);
        }
    }
}
