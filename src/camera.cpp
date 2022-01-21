#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    front(glm::vec3(0, -1, 0)),
    movement_speed(SPEED),
    mouse_sensitivity(SENSITIVITY),
    zoom(ZOOM)
{
    this->position = position;
    this->world_up = up;
    this->yaw = -90.0;
    this->pitch = 0.0;

    updateCameraVectors();
}


Camera::Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch) :
    front(glm::vec3(0, 0, 0)),
    movement_speed(SPEED),
    mouse_sensitivity(SENSITIVITY),
    zoom(ZOOM)
{
    this->position = glm::vec3(pos_x, pos_y, pos_z);
    this->world_up = glm::vec3(up_x, up_y, up_z);
    this->yaw = yaw;
    this->pitch = pitch;

    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(Camera_Movement direction, float delta_time)
{
    float velocity = movement_speed * delta_time * position.y / 6;
    glm::vec3 new_position;

    previous_moves.clear();

    printf("pos %f %f %f\n", position.x, position.y, position.z);

    if (direction == FORWARD)
    {
        new_position = position + up * velocity;
        if (new_position.x < 90)
            position = new_position;
    }
    if (direction == BACKWARD)
    {
        new_position = position - up * velocity;
        if (new_position.x > -90)
            position = new_position;
    }
    if (direction == LEFT)
    {
        new_position = position - right * velocity;
        if (new_position.z > -180)
            position = new_position;
    }
    if (direction == RIGHT)
    {
        new_position = position + right * velocity;
;
        if (new_position.z < 180)
            position = new_position;
    }
    if (direction == UP)
    {
        new_position = position - front * velocity;
        if (new_position.y < 450)
            position = new_position;
    }
    if (direction == DOWN)
    {
        new_position = position + front * velocity;
        if (new_position.y > 0)
            position = new_position;
    }
}

void Camera::processMouseMovement(float x_offset, float y_offset, GLboolean constrain_pitch)
{
    x_offset *= mouse_sensitivity;
    y_offset *= mouse_sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    if (constrain_pitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float y_offset)
{
    zoom -= (float)y_offset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    // glm::vec3 front_vector;
    // front_vector.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    // front_vector.y = sin(glm::radians(pitch));
    // front_vector.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    // front = glm::normalize(front_vector);


    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up    = glm::normalize(glm::cross(right, front));
}

void Camera::undoMove(float delta_time)
{
    for (Camera_Movement direction : previous_moves)
    {
        float velocity = movement_speed * delta_time;
        switch (direction)
        {
            case FORWARD:
                position -= front * velocity;
                break;
            case BACKWARD:
                position += front * velocity;
                break;
            case LEFT:
                position += right * velocity;
                break;
            case RIGHT:
                 position -= right * velocity;
                break;
            case UP:
                position -= up * velocity;
                break;
            case DOWN:
                position += up * velocity;
            default:
                break;
        }
    }
}