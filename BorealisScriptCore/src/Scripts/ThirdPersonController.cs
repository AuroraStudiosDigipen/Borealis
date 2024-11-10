using Borealis;
public class ThirdPersonController : MonoBehaviour
{
    [Header("Custom Properties")]
    public float MoveSpeed = 5f;
    public float JumpSpeed = 3f;
    public float Gravity = 9.81f;

    [Header("External References")]
    //public Rigidbody CC;
    public GameObject CamYawPivot;
    public GameObject CamMovePivot;
    public GameObject CamPitchPivot;
    public GameObject PlayerModel;

    private Rigidbody rb;
    private Vector3 VerticalVelocity;
    private bool IsMoving = false;

    // Start is called before the first frame update
    public override void Start()
    {
        rb = new Rigidbody(PlayerModel.GetInstanceID());
    }

    // Update is called once per frame
    public override void Update()
    {
        //Set velocity to 0 if no input is given
        Vector3 dir = Vector3.zero;

        //Handle Movement Input
        if (Input.GetKey(KeyCode.W))
          {
            dir += CamYawPivot.transform.forward;
        }

        if (Input.GetKey(KeyCode.A))
            dir -= CamYawPivot.transform.right;

        if (Input.GetKey(KeyCode.S))
            dir -= CamYawPivot.transform.forward;

        if (Input.GetKey(KeyCode.D))
            dir += CamYawPivot.transform.right;


        //Determine whether a movement input was given
        IsMoving = dir != Vector3.zero;

        //Adjust the rotation of the model whenever the player moves
        if (IsMoving)
        {
            PlayerModel.transform.forward = CamYawPivot.transform.forward;
        }

        ////Handle Gravity 
        //if (VerticalVelocity == Vector3.zero)
        //{
        //    //Small gravity applied when character is grounded to ensure grounded flag stays active
        //    VerticalVelocity = Vector3.down * 0.5f;

        //    //Jump
        //    if (Input.GetKeyDown(KeyCode.Space))
        //        VerticalVelocity = Vector3.up * JumpSpeed;
        //}
        //else
        //{
        //    //Increase gravity for every frame we're not contacting the ground
        //    VerticalVelocity += Vector3.down * Gravity * Time.deltaTime;
        //}

        ////Apply Gravity
        //dir += VerticalVelocity;

        //Apply movement
        rb.Move(dir * MoveSpeed * Time.deltaTime);
    }

    //public override void OnCollisionEnter(ulong id)
    //{
    //    GameObject obj = new GameObject(id);

    //    VerticalVelocity = Vector3.zero;
    //}

    //public override void OnCollisionStay(ulong id)
    //{
    //    GameObject obj = new GameObject(id);

    //    VerticalVelocity = Vector3.zero;
    //}
}