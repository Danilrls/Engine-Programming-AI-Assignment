#include "MovePatrolScript.h"
#include "extern.h"

/* MovePatrolScript*/

void MovePatrolScript::init() {
	next_waypoint = 1;

	waypoints[0] = lm::vec3(-10, 0, 10);
	waypoints[1] = lm::vec3(10, 0, 10);
	waypoints[2] = lm::vec3(10, 0, -10);
	waypoints[3] = lm::vec3(-10, 0, -10);

	player_pos.x = ECS.getComponentFromEntity<Transform>(ECS.getEntity("Player")).m[12];
	player_pos.y = ECS.getComponentFromEntity<Transform>(ECS.getEntity("Player")).m[13];
	player_pos.z = ECS.getComponentFromEntity<Transform>(ECS.getEntity("Player")).m[14];

	angle_check = true;
	last_angle = 0;
	speed = 7;
	rotate_speed = 7;

	//std::cout << ECS.getComponentFromEntity<Transform>(ECS.getEntity("Player")).m[12] << std::endl;
	/*front_norm = transform->front();
	front_norm.normalize();*/

	transform = &ECS.getComponentFromEntity<Transform>(owner_);
	transform->translate(waypoints[0]);

	//position = transform->position();

	
}
void MovePatrolScript::update(float dt) {

	player_pos.x = ECS.getComponentFromEntity<Transform>(ECS.getEntity("Player")).m[12];
	player_pos.y = ECS.getComponentFromEntity<Transform>(ECS.getEntity("Player")).m[13];
	player_pos.z = ECS.getComponentFromEntity<Transform>(ECS.getEntity("Player")).m[14];

	
	lm::vec3 front = transform->front();
	lm::vec3 front_norm = front.normalize();
	lm::vec3 position = transform->position();

	lm::vec3 distance_waypoint = lm::vec3(waypoints[next_waypoint].x - position.x, 0, waypoints[next_waypoint].z - position.z);
	lm::vec3 distance_player = lm::vec3(player_pos.x - position.x, 0, player_pos.z - position.z);
	float distance_waypoint_length = lm::vec3(waypoints[next_waypoint].x - position.x, 0, waypoints[next_waypoint].z - position.z).length();
	float distance_player_length = lm::vec3(player_pos.x - position.x, player_pos.y - position.y, player_pos.z - position.z).length();
	
	/*lm::vec3 direction_waypoint = lm::vec3(waypoints[next_waypoint].x - position.x, waypoints[next_waypoint].y - position.y, waypoints[next_waypoint].z - position.z).normalize();
	lm::vec3 direction_player = lm::vec3(player_pos.x - position.x, player_pos.y - position.y, player_pos.z - position.z).normalize();
	*/
	if (distance_player_length < 15) {
		distance_player.normalize();
		float angle = acos(distance_player.dot(front_norm)) * 180 / 3.1415;
		lm::vec3 rot_axis = lm::vec3(transform->m[4], transform->m[5], transform->m[6]);

		//std::cout << position.x << "---(" << position.x << ")---(" << position.z << ")---(" << distance_waypoint_length << ")---ANGLE(" << angle << ")---(" << next_waypoint << std::endl;
		
		//owner's rotation
		if (angle_check) {
			if (angle > 20) {
				transform->rotateLocal(-dt * rotate_speed, rot_axis);
			}
			else {
				transform->translate(front_norm.x * dt * (speed + 3), 0, front_norm.z * dt * (speed + 3));
			}
			angle_check = false;
			last_angle = angle;
			//std::cout << "|||(" << last_angle << std::endl;
		}
		if(last_angle < angle && !angle_check){
			rotate_speed = -rotate_speed;
			angle_check = true;
			//std::cout << "SMALLER ANGLE ( " << last_angle << std::endl;
		}
		if (last_angle > angle && !angle_check) {
			angle_check = true;
			//std::cout << "BIGGER ANGLE ( " << last_angle << std::endl;
		}
		
	}
	else {
		if (distance_waypoint_length < 1) {
			next_waypoint++;
			if (next_waypoint >= 4) {
				next_waypoint = 0;
			}

		}
		else {
			//Angle and axis calculus
			distance_waypoint.normalize();
			float angle = acos(distance_waypoint.dot(front_norm)) * 180 / 3.1415;
			lm::vec3 rot_axis = lm::vec3(transform->m[4], transform->m[5], transform->m[6]);

			//std::cout << position.x << "---(" << position.x << ")---(" << position.z << ")---(" << distance_waypoint_length << ")---(" << angle << ")---(" << next_waypoint << std::endl;
			
			//owner's rotation
			if (angle > 20) {
				transform->rotateLocal(-dt * 5, rot_axis);
			}
			else {
				transform->translate(front_norm.x * dt * speed, 0, front_norm.z * dt * speed);
			}
		}
	}
	
	//std::cout << ECS.getComponentFromEntity<Transform>(ECS.getEntity("Player")).m[12] << "UPDATING" << ECS.getComponentFromEntity<Transform>(ECS.getEntity("Player")).m[13] << "UPDATING" << ECS.getComponentFromEntity<Transform>(ECS.getEntity("Player")).m[14]  <<std::endl;

}

