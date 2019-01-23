#pragma once
#include "ScriptSystem.h"
#include "includes.h"




class MovePatrolScript : public Script {
public:
	MovePatrolScript(int owner) : Script(owner) {}
	void init();
	void update(float dt);

	int next_waypoint;
	bool angle_check;
	float last_angle;
	float rotate_speed;
	float speed;
	lm::vec3 position;
	lm::vec3 waypoints [4];
	lm::vec3 player_pos;
	Transform* transform;
	lm::vec3 front_norm;
};

#pragma once
