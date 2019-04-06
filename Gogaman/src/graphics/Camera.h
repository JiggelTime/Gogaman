#ifndef CAMERA_H
#define CAMERA_H

#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

enum Camera_Movement 
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//Default camera values
const float YAW			= -90.0f;
const float PITCH		=  0.0f;
const float SPEED		=  2.5f;
const float SENSITIVITY	=  0.1f;
const float ZOOM		=  80.0f;

class Camera
{
public:
	//Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	//Euler Angles
	float Yaw;
	float Pitch;

	//Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	//Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	//Constructor with scalars
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	//Returns the view matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	//Processes input from keyboard
	void ProcessKeyboardInput(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	//Processes input from mouse
	void ProcessMouseInput(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		//Constrain the yaw value between 0-360
		Yaw = glm::mod(Yaw + xoffset, 360.0f);
		Pitch += yoffset;

		//Constrain pitch
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		//Update vectors using updated Euler angles
		updateCameraVectors();
	}

	//Processes input from mouse scroll-wheel
	void ProcessMouseScrollInput(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 90.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 90.0f)
			Zoom = 90.0f;
	}

private:
	//Calculates the front vector from the updated Euler angles
	void updateCameraVectors()
	{
		//Calculate new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		//Re-calculate Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif