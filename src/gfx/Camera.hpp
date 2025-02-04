#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GFX {
	class Camera {
		glm::vec3 up, forward, right;

		glm::mat4 model, view, proj;
        
        float yaw, pitch, fov, aspect;
        float yawSensitivity = 0.1f;
        float pitchSensitivity = 0.1f;
    public:
		Camera() {
			up = glm::vec3(0, 0, 1);
			forward = glm::vec3(0, 1, 0);
			right = glm::vec3(1, 0, 0);

			model = glm::mat4(1.0f);
			view = glm::mat4(1.0f);
			view = glm::translate(view, glm::vec3(0.0f, -5.0f, 0.0f));
		}
        void fov(float fov) {
            this->fov = fov;
            proj = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
        }
        void viewport(float aspect, float fov) {
            this->aspect = aspect;
            this->fov = fov;
            proj = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
        }
        void translate(glm::vec3 pos) {
            view = glm::translate(view, pos);
        }
        void lookAt(glm::vec3 pos) {
            view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), pos, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        void rotateBy(int yaw, int pitch) {
            this->yaw += yaw * yawSensitivity;
            this->pitch += pitch * pitchSensitivity;
            glm::quat q = glm::quat(glm::vec3(this->pitch, this->yaw, 0.0f));
            view = glm::mat4_cast(q);
        }
        glm::mat4 getModel() const {
            return model;
        }
        glm::mat4 getProjection() const {
            return proj;
        }
        glm::mat4 getView() const {
            return view;
        }
	};
}
#endif