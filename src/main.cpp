#include "main.h"
#include <string>
#define DIGITAL_SENSOR_PORT 'H'

// defining all the motors and controllers 
pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::Controller partner(pros::E_CONTROLLER_PARTNER);
pros::Motor left_mtr_front(20);
pros::Motor left_mtr_back(10);
pros::Motor right_mtr_front(11);
pros::Motor right_mtr_back(1);

/* Lift is the actual lift (the one that rotates up) */
/* Claw is the other one (the rotator with two beams) <-- pneumatics */

pros::Motor left_claw(9);
pros::Motor right_claw(12);
pros::Motor left_lift(8);
pros::Motor right_lift(2); // port 2 is broken
pros::ADIDigitalOut piston(DIGITAL_SENSOR_PORT);

int speed = 2; 
bool locked = true; // for breaking
bool combined = true; // for moving the claws arms at the same time
bool piston_state = false; // for pneumatics: when the state is true they're able to shoot down


// displays all the current controlable states to the controller screens of both partner and master
void display_fn()
{
	master.clear();
	while (true)
	{
		double battery = pros::battery::get_capacity();
		master.set_text(0, 0, std::to_string(battery).substr(0, 2) + "% " + std::to_string(pros::competition::get_status()) + " " + std::to_string(left_claw.get_position()).substr(0, 2));
		pros::delay(50);
		partner.set_text(0, 0, std::to_string(battery).substr(0, 2) + "% " + std::to_string(pros::competition::get_status()) + " " + std::to_string(right_claw.get_position()).substr(0, 2));
		pros::delay(50);

		// speed is fast, the denominator for reducing the analog movement is smaller (1/2 vs 1/1)
		if (speed == 1)
		{
			master.set_text(1, 0, "Sp(X): F");
			pros::delay(50);
			partner.set_text(1, 0, "Sp(X): F");
		}
		
		// speed is slow
		else
		{
			master.set_text(1, 0, "Sp(X): S");
			pros::delay(50);
			partner.set_text(1, 0, "Sp(X): S");
		}
		pros::delay(50);

		// breaks are on 
		if (locked)
		{
			master.set_text(1, 9, "Br(A): On ");
			pros::delay(50);
			partner.set_text(1, 9, "Br(A): On ");
		}

		// breaks are off
		else
		{
			master.set_text(1, 9, "Br(A): Off");
			pros::delay(50);
			partner.set_text(1, 9, "Br(A): Off");
		}
		pros::delay(50);

		// lifts are moving at the same time
		if (combined)
		{
			master.set_text(2, 0, "Com(Y): On ");
			pros::delay(50);
			partner.set_text(2, 0, "Com(Y): On ");
		}

		// lifts are not moving at the same time
		else
		{
			master.set_text(2, 0, "Com(Y): Off");
			pros::delay(50);
			partner.set_text(2, 0, "Com(Y): Off");
		}
		pros::delay(50);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
	pros::Task display_task(display_fn);
}

void disabled() {}

void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	
	const int DRIVE_MOTOR_LEFT = 1;
	const int DRIVE_MOTOR_RIGHT = 2;


	// always going to be moving at 10, in the opcontrol movement, uses analong to move, analog eventually
    // equals 0 so then it would stop moving, since this is always equal to 10 its going to keep moving at 10
	
	// drive forward 
	// left_mtr_back = 32;
	// left_mtr_front = 32;
	// right_mtr_back = 32 * -1;
	// right_mtr_front = 32 * -1;
	// pros::delay(12000);

	// left_mtr_front.move_absolute(100, 100);
	// left_mtr_back.move_absolute(100, 100);
	// right_mtr_front.move_absolute(100, 100);
	// right_mtr_back.move_absolute(100, 100);
	// while (!((left_mtr_front.get_position() < 105) && (left_mtr_front.get_position() > 95))) {
    // // Continue running this loop as long as the motor is not within +-5 units of its goal
    // 	pros::delay(2);
	// }

	left_mtr_back.move_velocity(32);
	left_mtr_front.move_velocity(32); 
	right_mtr_back.move_velocity(32);
	right_mtr_front.move_velocity(32); 	
	pros::delay(12000);
	// bot stops moving 
	// left_mtr_back = 0; 	// sets the movement to 0 (we know this isn't practical)
	// left_mtr_front = 0;
	// right_mtr_back = 0;
	// right_mtr_front = 0;
	left_mtr_back.move_velocity(0);
	left_mtr_front.move_velocity(0); 
	right_mtr_back.move_velocity(0);
	right_mtr_front.move_velocity(0); 	
	
	// shoot pistons 
	piston.set_value(piston_state);
	piston_state = true;

	// keep them down
	pros::delay(20000); // (milliseconds)

	// bring them back up
	piston.set_value(piston_state);
	piston_state = false;

	// lift arms
	right_claw.move_velocity(-50);
	left_claw.move_velocity(50);
	pros::delay(3000);
	right_claw.move_velocity(0);
	left_claw.move_velocity(0);	

	// go back
	// left_mtr_back = 32;
	// left_mtr_front = 32;
	// right_mtr_back = 32 * -1;
	// right_mtr_front = 32 * -1;	
	left_mtr_back.move_velocity(-32);
	left_mtr_front.move_velocity(-32); 
	right_mtr_back.move_velocity(-32);
	right_mtr_front.move_velocity(-32);
	pros::delay(12000); 
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

// goes through all these if statements sequentially, brakes override movement, then actual movement
// gets the current analog position, while driver is driving, then it sets the bot speed/movement to that
// then it simply checks all the other variables controlling functionality on the device
// this is all done very fast, so this process just keeps repeating 
// basically: keeps looping and changing the states of the motors very fast
void opcontrol()
{
	// if brakes are supposed to be true
	if (locked)
	{
		// actually set the claws to be braked: 
		left_claw.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
		right_claw.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
	}

	// sets the brakes for the lift
	left_lift.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
	right_lift.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);

	while (true)
	{
		int left = master.get_analog(ANALOG_LEFT_Y);
		int right = master.get_analog(ANALOG_RIGHT_Y);

		// movement is different here: V drives in a penguin movement, one side moves at a time
		left_mtr_back = left / speed;
		left_mtr_front = left / speed;
		right_mtr_back = right / speed * -1;
		right_mtr_front = right / speed * -1;

		// checks if the claws should move at the same time or not 
		if (combined)
		{
			// much simpler when combined is true: 

			// if R1 (front side, not the top) is being pressed, move both the claws up
			if (master.get_digital(DIGITAL_R2))
			{
				right_claw.move_velocity(-50);
				left_claw.move_velocity(50);
			}

			// if the other button is being pressed, move both claws down
			else if (master.get_digital(DIGITAL_R1))
			{
				right_claw.move_velocity(50);
				left_claw.move_velocity(-50);
			}

			// if none are pressed, don't move them at all
			else
			{
				right_claw.move_velocity(0);
				left_claw.move_velocity(0);
			}
		}

		// if they aren't supposed to be moving together (combined on), then you use all the 
		// buttons on the side front to control the claw (R1, R2, L1, L2)
		// three states the motor can be in: moving up, down, or not at all
		else
		{

			if (master.get_digital(DIGITAL_L2))
			{
				left_claw.move_velocity(50); // moving up
			}
			else if (master.get_digital(DIGITAL_L1))
			{
				left_claw.move_velocity(-50); // moving down
			}
			else
			{
				left_claw.move_velocity(0); // not moving
			}
			
			if (master.get_digital(DIGITAL_R2))
			{
				right_claw.move_velocity(-50);
			}
			else if (master.get_digital(DIGITAL_R1))
			{
				right_claw.move_velocity(50);
			}
			else
			{
				right_claw.move_velocity(0);
			}
		}


		// moves the lift arms (back of the bot)
		// three states as well: moving up, moving down, or none at all
		if (master.get_digital(DIGITAL_UP))
		{	
			// the amount it moves at once is the speed since it keeps moving
			// at this amount until the state is changed (a different button/ lack of press)
			left_lift.move_velocity(-45);
			right_lift.move_velocity(45);
		}
		else if (master.get_digital(DIGITAL_DOWN))
		{
			left_lift.move_velocity(45);
			right_lift.move_velocity(-45);
		}
		else
		{
			left_lift.move_velocity(0);
			right_lift.move_velocity(0);
		}

		// changes the speed, parter or master controller can do this
		if (master.get_digital_new_press(DIGITAL_X) || partner.get_digital_new_press(DIGITAL_X))
		{
			// makes speed slower
			if (speed == 1)
			{	
				// this is because the denominator is now bigger (100/2 vs 100/1 --> 50 vs 100)
				speed = 2; 
			}

			// makes speed faster
			else
			{
				speed = 1;
			}
		}
		
		// brakes the claws movement (pneumatics side)
		if (master.get_digital_new_press(DIGITAL_A) || partner.get_digital_new_press(DIGITAL_A))
		{
			if (locked)
			{
				locked = false;
				left_claw.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_COAST);
				right_claw.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_COAST);
			}
			else
			{
				locked = true;
				left_claw.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
				right_claw.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
			}
		}

		// changes whether or not the claws move in sync
		// if (master.get_digital_new_press(DIGITAL_Y) || partner.get_digital_new_press(DIGITAL_Y))
		// {
		// 	if (combined)
		// 	{
		// 		combined = false;
		// 	}
		// 	else
		// 	{
		// 		combined = true;
		// 	}
		// }

		// ????? (maybe moves the claw to a neutral position or moves it to be up at an angle)
		if (master.get_digital_new_press(DIGITAL_B) || partner.get_digital_new_press(DIGITAL_B))
		{
			left_claw.move_absolute(200, 100);
			right_claw.move_absolute(-200, 100);

			// while the claw moves to the position, delay to give time for it to move 
			while (!((left_claw.get_position() < 205) && (left_claw.get_position() > 195)))
			{
				pros::delay(2); // waits for the movement to get to that place
			}
			while (!((right_claw.get_position() < -195) && (right_claw.get_position() > -205)))
			{
				pros::delay(2);
			}
		}

		// left on dpad
		// changes the piston state so that it shoots down
		if (master.get_digital_new_press(DIGITAL_LEFT) || partner.get_digital_new_press(DIGITAL_LEFT))
		{
			
			if (piston_state)
			{
				piston.set_value(true);
				piston_state = false;
			}


			else
			{
				piston.set_value(false);
				piston_state = true;
			}
		}

		pros::delay(10);
	}
}