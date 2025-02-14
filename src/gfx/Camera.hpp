#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Camera {
    glm::vec3 up, forward, right;

    glm::mat4 model, view, proj;
    
    float yaw, pitch, fov, aspect;
    float yawSensitivity = 0.1f;
    float pitchSensitivity = 0.1f;

    Camera() {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.z = sin(glm::radians(pitch));
        front.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        forward = glm::normalize(front);
        right = glm::normalize(glm::cross(forward, glm::vec3(0, 0, 1)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, forward));

        model = glm::mat4(1.0f);
        view = glm::mat4(1.0f);
        proj = glm::perspective(glm::radians(90.f), 1.0f, 0.1f, 1000.0f);
        view = glm::translate(view, glm::vec3(0.0f, -5.0f, 0.0f));
    }
    void setFOV(float fov) {
        this->fov = fov;
        proj = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
    }
    void viewport(float aspect, float fov) {
        this->aspect = aspect;
        this->fov = fov;
        proj = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
    }
    void translate(glm::vec3 pos) {
        view = glm::lookAt(pos, pos + forward, up);
    }
    void rotateBy(int dYaw, int dPitch) {
        this->yaw -= dYaw * yawSensitivity;

        this->pitch -= dPitch * pitchSensitivity;
        if (this->pitch > 89.0f)
			this->pitch = 89.0f;
		if (this->pitch < -89.0f)
			this->pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.z = sin(glm::radians(pitch));
        front.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        forward = glm::normalize(front);
    
        right = glm::normalize(glm::cross(forward, glm::vec3(0,0,1)));
        up = glm::normalize(glm::cross(right, forward));
    }
};
