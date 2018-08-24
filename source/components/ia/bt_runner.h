#ifndef _BT_RUNNER_INC
#define _BT_RUNNER_INC

#include "bt.h"
#include "entity/common_msgs.h"



class bt_runner:public bt
	{
	typedef pair<float, int> Warc;
	struct waypoint {
		int id;
		VEC3 position;
		std::string type;
		std::vector<Warc> neighbours;
	};

	
	float attack_distance;
    float debug_timer = 0.f;
	float speed = 1.0f;

    bool b_disappear = false;
    bool b_appear = false;
    bool b_chase = false;
    bool b_recular = false;
    bool on_wall = false;
	bool b_chase_player = false;

	bool going_right = false;
	bool going_up = true;

	std::vector<int> path;
	std::vector<waypoint> waypoints_map;
	int actual_waypoint;
	int next_waypoint;
	bool chase_started = false;


    VEC3 appearing_position;
	VEC3 tower_center = VEC3::Zero;
	std::string actual_state;

	VEC3 top_jump_position = VEC3::Zero;

	void recalculate_path();
	void calculate_top_jump_position();
	float distance_x_z(VEC3 v1, VEC3 v2);

	void calculate_distances_graph();

	DECL_SIBLING_ACCESS();

	public:
	void load(const json& j, TEntityParseContext& ctx);
	void debugInMenu();
    void load_waypoint(const json& j);

    void appear(const TMsgRunnerAppear& msg);
    void disappear(const TMsgRunnerDisappear& msg);

	static void registerMsgs();


	void create(string);

	// Actions
    int actionStop();
    int actionScream();
    int actionDisappear();
    int actionRecular();
    int actionRecover();
    int actionAttackWall1();
    int actionAttackWall2();
    int actionAttackFloor1();
    int actionAttackFloor2();
    int actionChase();
    int actionAppear();
    int actionHide();

	// Conditions
    bool conditionDisappear();
    bool conditionChase();
    bool conditionRecular();
    bool conditionAttack();
    bool conditionAttackWall();
    bool conditionAttackFloor();
    bool conditionAppear();

    void killPlayer();

	void chase();
	void findPath(int origin, int destiny);
	int findClosestWaypoint(VEC3 position);
	void chase_waypoint();
	void chase_player();
	void walk();
	void jump();

	};

#endif