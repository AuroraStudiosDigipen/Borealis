using Borealis;
using System.Security.Policy;

public class PushAndPull : MonoBehaviour
{
    public bool isBeingPushedOrPulled = false; // Track if the object is being pushed/pulled
    private Transform playerTransform;
    private Rigidbody rb;
    public float colliderBoundX;
    public float colliderBoundY;
    public float colliderBoundZ;

    [Header("Movement Settings")]
    public float moveSpeed = 2f; // Speed at which the object will move
    public float interactionDistance = 3f; // Distance to check for interaction with the player

    public override void Start()
    {
        //rb = GetComponent<Rigidbody>();
        //// Set to kinematic initially
        //rb.isKinematic = true;
    }

    private void AlignPlayerToObject()
    {
        // Calculate the closest side (left, right, up, or down) of the object relative to the player
        Vector3 directionToPlayer = (playerTransform.position - gameObject.transform.position).normalized;
        Vector3 closestSide = Vector3.zero;

        // Determine the side of the object the player is closest to
        if (Mathf.Abs(directionToPlayer.x) > Mathf.Abs(directionToPlayer.z))
        {
            closestSide = directionToPlayer.x > 0 ? transform.right : -transform.right; // Left or right
        }
        else
        {
            closestSide = directionToPlayer.z > 0 ? transform.forward : -transform.forward; // Forward or backward
        }

        // Calculate offset based on the collider bounds
        float offsetDistance = colliderBoundX + 0.5f; // Adjust offset

        // Place the player at the middle of the closest side
        Vector3 sidePosition = gameObject.transform.position + closestSide * offsetDistance;
        playerTransform.position = new Vector3(sidePosition.x, playerTransform.position.y, sidePosition.z);
    }

    // Called when player starts pushing or pulling the object
    public void StartPushingOrPulling(Transform player)
    {
        isBeingPushedOrPulled = true;
        playerTransform = player;

        // Disable kinematic so the object can be moved
        //rb.isKinematic = false;

        AlignPlayerToObject();
    }

    // Called when player stops pushing or pulling
    public void StopPushingOrPulling()
    {
        isBeingPushedOrPulled = false;
        playerTransform = null;

        // Re-enable kinematic to stop physics-based interactions
        //rb.isKinematic = true;
    }

    // Moves the object relative to the player's input
    public void MoveWithPlayer()
    {
        if (playerTransform == null)
        {
            Debug.LogError("playerTransform is null! Cannot move the object.");
            return;
        }

        // Get player movement input
        float verticalInput = Input.GetAxisRaw("Vertical");   // W/S (forward/backward)
        float horizontalInput = Input.GetAxisRaw("Horizontal"); // A/D (left/right)

        Vector3 moveDirection = Vector3.zero;

        // We use the object's forward and right directions to control the player's movement relative to the object
        Vector3 objectForward = transform.forward;
        Vector3 objectRight = transform.right;

        // Find out which side the player is on (1, 2, 3, or 4)
        Vector3 playerOffset = playerTransform.position - transform.position;

        Vector3 objectSize = new Vector3 ( colliderBoundX*2, colliderBoundY*2, colliderBoundZ *2 ); // Get the object's size from its collider

        if (Mathf.Abs(playerOffset.z) > Mathf.Abs(playerOffset.x))
        {
            // Player is on side 1 (below) or 4 (above)
            if (playerOffset.z > 0)
            {
                // Player is on side 4 (above)
                moveDirection += objectForward * -verticalInput; // Reverse forward/backward for this side
                moveDirection += objectRight * -horizontalInput; // Reverse left/right for this side
            }
            else
            {
                // Player is on side 1 (below)
                moveDirection += objectForward * verticalInput;
                moveDirection += objectRight * horizontalInput;
            }
        }
        else
        {
            // Player is on side 2 (right) or 3 (left)
            if (playerOffset.x > 0)
            {
                // Player is on side 2 (right)
                moveDirection += objectForward * horizontalInput;
                moveDirection += objectRight * -verticalInput;
            }
            else
            {
                // Player is on side 3 (left)
                moveDirection += objectForward * -horizontalInput;
                moveDirection += objectRight * verticalInput;
            }
        }

        if (moveDirection != Vector3.zero)
        {
            // Move the object
            transform.position += moveDirection.normalized * moveSpeed * Time.deltaTime;

            // Calculate offset based on the collider's bounds instead of the object's scale
            float xOffset = colliderBoundX + 1f; // Use collider bounds to calculate X offset
            float zOffset = colliderBoundZ + 1f; // Use collider bounds to calculate Z offset

            // Recalculate player position and lock them relative to the side of the object
            if (Mathf.Abs(playerOffset.x) > Mathf.Abs(playerOffset.z))
            {
                // Player is on the X-axis (left or right)
                playerTransform.position = new Vector3(
                    transform.position.x + Mathf.Sign(playerOffset.x) * xOffset,
                    playerTransform.position.y,
                    transform.position.z
                );
            }
            else
            {
                // Player is on the Z-axis (above or below)
                playerTransform.position = new Vector3(
                    transform.position.x,
                    playerTransform.position.y,
                    transform.position.z + Mathf.Sign(playerOffset.z) * zOffset
                );
            }
        }
    }

}
