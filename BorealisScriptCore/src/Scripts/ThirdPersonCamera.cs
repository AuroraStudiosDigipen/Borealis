using Borealis;
using System.Threading;

public class ThirdPersonCamera : MonoBehaviour
{
    public float YawRotSpeed = 180f;
    public float PitchRotSpeed = 180f;
    public float MaxPitchAngle = 85f;
    public float MinPitchAngle = -85f;
    public static bool InvertPitch = true;

    private float YawAngle = 0f;
    private float PitchAngle = 0f;

    public bool unlock = true;

    public GameObject CamYawPivot;
    public GameObject CamMovePivot;
    public GameObject CamPitchPivot;
    // Start is called before the first frame update
    public override void Start()
    {
        //Lock and hide mouse cursor
        Cursor.visible = false;
        unlock = true;
    }

    // Update is called once per frame
    public override void Update()
    {
        if (unlock == true)
        {
            YawAngle = CamYawPivot.transform.localRotation.y;
            PitchAngle = CamPitchPivot.transform.localRotation.x;
            unlock = false;
        }
        else
        {        //Yaw Camera Rotation
            YawAngle += Input.GetAxis("Horizontal") * YawRotSpeed * Time.deltaTime;
            CamYawPivot.transform.localRotation = new Vector3(0f, YawAngle, 0f);

            //Pitch Camera Rotation
            PitchAngle += Input.GetAxis("Vertical") * (InvertPitch ? -1.0f : 1.0f) * PitchRotSpeed * Time.deltaTime;
            PitchAngle = Mathf.Clamp(PitchAngle, MinPitchAngle, MaxPitchAngle);
            CamPitchPivot.transform.localRotation = new Vector3 (PitchAngle, 0f, 0f);
        }
    }
}
