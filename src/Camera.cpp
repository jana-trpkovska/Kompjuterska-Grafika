#include <Camera.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
          MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}
// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
          MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

// Processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
//void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime,const std::vector<Cube>& level) {
//    float velocity = MovementSpeed * deltaTime;
//    if (direction == FORWARD)
//        Position += Front * velocity;
//    if (direction == BACKWARD)
//        Position -= Front * velocity;
//    if (direction == LEFT)
//        Position -= Right * velocity;
//    if (direction == RIGHT)
//        Position += Right * velocity;
//}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime, const std::vector<Cube>& level) {
    float velocity = MovementSpeed * deltaTime;
    glm::vec3 offset(0.0f);

    if (direction == FORWARD)
        offset += Front * velocity;
    if (direction == BACKWARD)
        offset -= Front * velocity;
    if (direction == LEFT)
        offset -= Right * velocity;
    if (direction == RIGHT)
        offset += Right * velocity;

    glm::vec3 newPosition = Position + offset;
    newPosition.y = 1.0f; // Maintain y position at 1.0f

    const float wallBuffer = 0.15f; // Adjust the collision buffer here
    const unsigned int maxCubes = 5; // Number of cubes to skip collision detection for

    for (unsigned int i = 0; i < level.size(); i++) {
        if (i >= level.size() - maxCubes) {
            continue; // Skip collision detection for the last 'maxCubes'
        }

        const auto& cube = level[i];
        glm::vec3 cubeMin = cube.position - (cube.size / 2.0f) - glm::vec3(wallBuffer);
        glm::vec3 cubeMax = cube.position + (cube.size / 2.0f) + glm::vec3(wallBuffer);

        bool collisionX = newPosition.x >= cubeMin.x && newPosition.x <= cubeMax.x;
        bool collisionY = newPosition.y >= cubeMin.y && newPosition.y <= cubeMax.y;
        bool collisionZ = newPosition.z >= cubeMin.z && newPosition.z <= cubeMax.z;

        if (collisionX && collisionY && collisionZ) {
            // Collision detected - prevent movement in that direction
            offset = glm::vec3(0.0f); // Zero out the offset
            break; // Exit the loop early as there's a collision
        }
    }

    Position += offset;
    Position.y = 1.2f; // Ensure camera stays at y = 1.2f
}

// Processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                                  GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires
// input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
    if (Zoom >= 1.0f && Zoom <= 45.0f)
        Zoom -= yoffset;
    if (Zoom <= 1.0f)
        Zoom = 1.0f;
    if (Zoom >= 45.0f)
        Zoom = 45.0f;
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = static_cast<float>(cos(glm::radians(static_cast<double>(Yaw))) *
                                 cos(glm::radians(static_cast<double>(Pitch))));
    front.y = static_cast<float>(sin(glm::radians(static_cast<double>(Pitch))));
    front.z = static_cast<float>(sin(glm::radians(static_cast<double>(Yaw))) *
                                 cos(glm::radians(static_cast<double>(Pitch))));
    Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(
            Front, WorldUp)); // Normalize the vectors, because their length gets
    // closer to 0 the more you look up or down which
    // results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}
