//
//  Game.cpp
//
//  Copyright � 2018 Alun Evans. All rights reserved.
//

#include "Game.h"
#include "Shader.h"
#include "extern.h"
#include "Parsers.h"
#include "MovePlatformScript.h"
#include "MovePatrolScript.h"
#include <functional>

Game::Game() {

}

int createFree(float aspect, ControlSystem& sys) {
	int ent_player = ECS.createEntity("Player");
	Camera& player_cam = ECS.createComponentForEntity<Camera>(ent_player);
	ECS.getComponentFromEntity<Transform>(ent_player).translate(0.0f, 3.0f, 5.0f);
	player_cam.position = lm::vec3(0.0f, 3.0f, -20.0f);
	player_cam.forward = lm::vec3(0.0f, 0.0f, 1.0f);
	player_cam.setPerspective(60.0f*DEG2RAD, aspect, 0.1f, 10000.0f);

	ECS.main_camera = ECS.getComponentID<Camera>(ent_player);

	sys.control_type = ControlTypeFree;

	return ent_player;
}

int createPlayer(float aspect, ControlSystem& sys) {
    int ent_player = ECS.createEntity("Player");
    Camera& player_cam = ECS.createComponentForEntity<Camera>(ent_player);
    ECS.getComponentFromEntity<Transform>(ent_player).translate(0.0f, 3.0f, -20.0f);
    player_cam.position = lm::vec3(0.0f, 3.0f, 5.0f);
    player_cam.forward = lm::vec3(0.0f, 0.0f, 1.0f);
    player_cam.setPerspective(60.0f*DEG2RAD, aspect, 0.01f, 10000.0f);
    
    //FPS colliders
    int ent_down_ray = ECS.createEntity("Down Ray");
    Transform& down_ray_trans = ECS.createComponentForEntity<Transform>(ent_down_ray);
    down_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& down_ray_collider = ECS.createComponentForEntity<Collider>(ent_down_ray);
    down_ray_collider.collider_type = ColliderTypeRay;
    down_ray_collider.direction = lm::vec3(0.0,-1.0,0.0);
    down_ray_collider.max_distance = 100.0f;
    
    int ent_left_ray = ECS.createEntity("Left Ray");
    Transform& left_ray_trans = ECS.createComponentForEntity<Transform>(ent_left_ray);
    left_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& left_ray_collider = ECS.createComponentForEntity<Collider>(ent_left_ray);
    left_ray_collider.collider_type = ColliderTypeRay;
    left_ray_collider.direction = lm::vec3(-1.0,0.0,0.0);
    left_ray_collider.max_distance = 2.0f;
    
    int ent_right_ray = ECS.createEntity("Right Ray");
    Transform& right_ray_trans = ECS.createComponentForEntity<Transform>(ent_right_ray);
    right_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& right_ray_collider = ECS.createComponentForEntity<Collider>(ent_right_ray);
    right_ray_collider.collider_type = ColliderTypeRay;
    right_ray_collider.direction = lm::vec3(1.0,0.0,0.0);
    right_ray_collider.max_distance = 2.0f;
    
    int ent_forward_ray = ECS.createEntity("Forward Ray");
    Transform& forward_ray_trans = ECS.createComponentForEntity<Transform>(ent_forward_ray);
    forward_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& forward_ray_collider = ECS.createComponentForEntity<Collider>(ent_forward_ray);
    forward_ray_collider.collider_type = ColliderTypeRay;
    forward_ray_collider.direction = lm::vec3(0.0,0.0,-1.0);
    forward_ray_collider.max_distance = 2.0f;
    
    int ent_back_ray = ECS.createEntity("Back Ray");
    Transform& back_ray_trans = ECS.createComponentForEntity<Transform>(ent_back_ray);
    back_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& back_ray_collider = ECS.createComponentForEntity<Collider>(ent_back_ray);
    back_ray_collider.collider_type = ColliderTypeRay;
    back_ray_collider.direction = lm::vec3(0.0,0.0,1.0);
    back_ray_collider.max_distance = 2.0f;
    
    //set all colliders for FPS control
	sys.FPS_collider_down = ECS.getComponentID<Collider>(ent_down_ray);
	sys.FPS_collider_left = ECS.getComponentID<Collider>(ent_left_ray);
	sys.FPS_collider_right = ECS.getComponentID<Collider>(ent_right_ray);
	sys.FPS_collider_forward = ECS.getComponentID<Collider>(ent_forward_ray);
	sys.FPS_collider_back = ECS.getComponentID<Collider>(ent_back_ray);
    
	ECS.main_camera = ECS.getComponentID<Camera>(ent_player);

	sys.control_type = ControlTypeFPS;

    return ent_player;
}

void tmpDrawSingleImage() {
	/*MAKE A SINGLE CHARACTER*/

	//initialise freetype
	FT_Library ft;
	FT_Init_FreeType(&ft);

	//load a font-face
	FT_Face face;
	FT_New_Face(ft, "data/fonts/arial.ttf", 0, &face);

	// width and height - width of 0 = automatic
	FT_Set_Pixel_Sizes(face, 0, 92);

	//load a character from the font-face into freetype buffer. FT-LOAD-RENDER means: 'make an image'
	FT_Load_Char(face, 'X', FT_LOAD_RENDER);

	//engine stuff - create GUI Text component
	int single_glyph = ECS.createEntity("single glyph");
	GUIText& el = ECS.createComponentForEntity<GUIText>(single_glyph);
	//set anchor and size
	el.anchor = GUIAnchorTopLeft;
	el.width = face->glyph->bitmap.width;
	el.height = face->glyph->bitmap.rows;
	//set color
	el.color = lm::vec3(1.0, 1.0, 0.0);

	//create texture according to size
	glGenTextures(1, &el.texture);
	glBindTexture(GL_TEXTURE_2D, el.texture);

	// disable default 4-byte alignment as freetype creates textures as single byte greyscale
	// so set byte-alignment to 1
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//set gl texture with pointer to freetype buffer. Note the data is 1 channel per pixel (GL_RED) and 1 byte per channel (GL_UNSIGNED_BYTE)
	glTexImage2D(GL_TEXTURE_2D, 0,
		GL_RED,	// SINGLE CHANNEL per pixel
		face->glyph->bitmap.width,
		face->glyph->bitmap.rows,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE, //1 unsigned byte (0-255) per channel
		face->glyph->bitmap.buffer //pointer to data
	);

	// Set texture filtering options because we're not using mipmaps, then unbind
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	//reset alignment and clean up FT
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}
	
//Nothing here yet
void Game::init() {


	//******* INIT SYSTEMS *******

	//init systems except debug, which needs info about scene
	control_system_.init();
	graphics_system_.init(window_width_, window_height_);
	script_system_.init(&control_system_),
		gui_system_.init(window_width_, window_height_);

	//******** RESOURCES AND LEVEL ********** //

	//level and player
	Parsers::parseJSONLevel("data/assets/level4.json", graphics_system_);
	createPlayer((float)window_width_ / (float)window_height_, control_system_);

	//script to move gun - exposes a toggleMove function
	MovePlatformScript* move_plaform_script = new MovePlatformScript(ECS.getEntity("FPS_gun"));
	script_system_.registerScript(move_plaform_script);

	//Creation of entities patrol
	int patrol = ECS.createEntity("Patrol");
	Collider& patrol_collider = ECS.createComponentForEntity<Collider>(patrol);
/*
	int patrol1 = ECS.createEntity("Patrol_1");
	Collider& patrol_collider1 = ECS.createComponentForEntity<Collider>(patrol1);*/

	//Script to move the patrol
	MovePatrolScript* move_patrol_script = new MovePatrolScript(ECS.getEntity("Patrol"));
	script_system_.registerScript(move_patrol_script);

	//MovePatrolScript* move_patrol_script1 = new MovePatrolScript(ECS.getEntity("Patrol_1"));
	//script_system_.registerScript(move_patrol_script1);



	//******** GUI ******** //


	/* LOAD A GUI IMAGE */
	int gui_image = ECS.createEntity("gui image");
	GUIElement& eli = ECS.createComponentForEntity<GUIElement>(gui_image);
	eli.texture = Parsers::parseTexture("data/assets/gui_test.tga");
	eli.anchor = GUIAnchorTopRight;
	eli.offset = lm::vec2(-10, -10);

	eli.onClick = std::bind(&MovePlatformScript::toggleMove, move_plaform_script);


	int second_gui_image = ECS.createEntity("second gui image");
	GUIElement& eli2 = ECS.createComponentForEntity<GUIElement>(gui_image);
	eli2.texture = Parsers::parseTexture("data/assets/gui_test.tga");
	eli2.anchor = GUIAnchorTopLeft;
	eli2.offset = lm::vec2(-10, -10);

	eli2.onClick = std::bind(&MovePlatformScript::toggleMoveDown, move_plaform_script);
	/* Draw a single glyph character */
	tmpDrawSingleImage();
	
	//TODO
	// - in GUISystem::update - scale and position the image and character


	/*MAKE A MULTI CHARACTER TEXTURE*/

	//create entity and component
	int multi_glyph = ECS.createEntity("multi glyph");
	GUIText& el_multi = ECS.createComponentForEntity<GUIText>(multi_glyph);

	// set text properties
	el_multi.width = 300;
	el_multi.height = 200;
	el_multi.font_face = "data/fonts/arial.ttf";
	el_multi.font_size = 32;
	el_multi.text = "The quick brown\nFox jumps over the\nlazy Dog!";
	el_multi.color = lm::vec3(1.0, 0.0, 1.0);

	//TODO
	// - in GUISystem::createTextTexture - write texture depending on text of element

	


    //******* INIT DEBUG SYSTEM *******
	//Setup after loading the resources
	gui_system_.lateInit();
	script_system_.lateInit();
    debug_system_.init();
    debug_system_.setActive(true);
}

//Entry point for game update code
void Game::update(float dt) {
	//update each system in turn

	//update input
	control_system_.update(dt);

	//collision
	collision_system_.update(dt);

	//render
	graphics_system_.update(dt);

    //debug
    debug_system_.update(dt);

	//gui
	gui_system_.update(dt);

	//scripts
	script_system_.update(dt);
   
}
//update game viewports
void Game::update_viewports(int window_width, int window_height) {
	window_width_ = window_width;
	window_height_ = window_height;

	auto& cameras = ECS.getAllComponents<Camera>();
	for (auto& cam : cameras) {
		cam.setPerspective(60.0f*DEG2RAD, (float)window_width_ / (float) window_height_, 0.01f, 10000.0f);
	}

	graphics_system_.updateMainViewport(window_width_, window_height_);
	gui_system_.updateViewport(window_width_, window_height_);
}


