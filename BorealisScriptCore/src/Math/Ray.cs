
namespace Borealis
{
    public class Ray
    {
        public Vector3 origin;
        public Vector3 direction;

        public Ray(Vector3 Origin, Vector3 Direction)
        {
            origin = Origin;
            direction = Direction;
        }

        public Vector3 GetPoint(float distance)
        {
            return origin + direction * distance;
        }
    }
}
