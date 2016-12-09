#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "opengl.h"
#include "concurrent_queue.h"
#include "util.h"
#include "planet.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <bitset>
#include <deque>
#include <mutex>
#include <atomic>
#include <thread>

#define PI        3.14159265358979323846264338327950288 

/// Manages the view of the scene
class Camera
{
public:
	Camera();
	/// Needed before scene rendering,ratio is the screen w/h ratio
	void update(float ratio);
	/// Returns a modifiable reference to the point the view is looking at
	glm::dvec3 &getCenter();
	/** Returns a modifiable reference to the the polar coordinates of the view
	 * around the center point (theta, phi, distance), theta and phi in radians **/
	glm::vec3 &getPolarPosition();
	/// Returns the actual position of the camera in cartesian coordinates
	const glm::dvec3 &getPosition();
	/// Returns a modifiable reference to the up vector of the view
	glm::vec3 &getUp();
	/// Returns the generated projection matrix
	const glm::mat4 &getProjMat();
	/// Returns the generated view matrix
	const glm::mat4 &getViewMat();

	/// Near frustum plane
	float getNear();
	void setNear(float near);

	/// Far frustum plane
	float getFar();
	void setFar(float far);

	/// Field of view on y axis
	float getFovy();
	void setFovy(float fovy);

private:
	glm::vec3 polarPos;
	glm::dvec3 pos;
	glm::dvec3 center;
	glm::vec3 up;
	float fovy;
	float near;
	float far;
	glm::mat4 proj_mat;
	glm::mat4 view_mat;

};

class Input
{
public:
	Input(GLFWwindow **win);
	bool isPressed(int key);
	bool isHeld(int key);
private:
	GLFWwindow **win;
	std::bitset<512> pressed;
};

class Game
{
public:
	Game();
	~Game();
	void init();
	void update(double dt);
	void render();
	bool isRunning();
	static void loadTexture(const std::string &filename, Texture &tex);

private:
	bool isPressedOnce(int key);

	void generateModels();
	void loadShaders();
	void loadSkybox();
	void loadPlanetFiles();
	void loadSettingsFile();

	glm::mat4 computeLightMatrix(const glm::vec3 &light_dir,const glm::vec3 &light_up, float planet_size, float ring_outer);
	void computeRingMatrix(glm::vec3 toward_view, glm::vec3 rings_up, float size, glm::mat4 &near_mat, glm::mat4 &far_mat);
	void renderPlanet(Planet &p);

	std::deque<Planet> planets; // Main planet collection
	Planet *focused_planet; // Planet the view follows
	int focused_planet_id;
	double epoch; // Seconds since January 1st 1950 00:00
	int time_warp_index;
	std::vector<double> time_warp_values;

	// THREADING RELATED STUFF
	std::deque<std::thread> tl_threads; // Texture loading Threads
	std::thread *screenshot_thread;
	std::atomic<bool> save; // Indicates if the screenshot thread has to save the framebuffer to a file now
	int thread_count; // Number of threads
	std::atomic<bool> quit; // boolean for killing threads

	/// Textures to load associated with their filename 
	static concurrent_queue<std::pair<std::string,Texture*>> textures_to_load;
	/// Loaded mipmap levels waiting to be used by opengl
	concurrent_queue<TexMipmapData> textures_to_update;

	unsigned char *screenshot_buffer;

	glm::vec3 light_position;
	Renderable skybox_obj, planet_obj, atmos_obj, ring_obj, flare_obj; // meshes
	Texture flare_tex;
	Shader skybox_shader,         
		planet_shader, 
		atmos_shader,
		ring_shader,
		sun_shader,
		flare_shader,
		post_default,
		post_hdr;
	Skybox skybox;
	
	// INTERACTION RELATED STUFF
	double pre_mouseposx, pre_mouseposy; // previous cursor position
	glm::vec3 view_speed;
	float max_view_speed, view_smoothness;

	bool is_switching;
	int switch_frames;
	int switch_frame_current;
	float switch_previous_dist;
	Planet *switch_previous_planet;

	float sensitivity;

	PostProcessing post_processing;
	float ssaa_factor;
	DDSLoader dds_loader;

	Camera camera;
	Input input;
	GLFWwindow *win;
	int win_w,win_h;
	bool fullscreen;

	float ratio;

	Texture no_night, no_clouds;

};

#endif