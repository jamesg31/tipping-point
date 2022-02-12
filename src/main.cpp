#include "main.h"
#include <string>
#define DIGITAL_SENSOR_PORT 'H'

pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::Controller partner(pros::E_CONTROLLER_PARTNER);
pros::Motor left_mtr_front(20);
pros::Motor left_mtr_back(10);
pros::Motor right_mtr_front(11);
pros::Motor right_mtr_back(1);

/* Lift is the actual lift (the one that rotates up) */
/* Claw is the other one (the rotator with two beams) */

pros::Motor left_claw(9);
pros::Motor right_claw(12);
pros::Motor left_lift(8);
pros::Motor right_lift(3); //port 2 is broken
pros::ADIDigitalOut piston(DIGITAL_SENSOR_PORT);

int speed = 2;
bool locked = true;
bool combined = true;
bool piston_state = false;

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
		if (speed == 1)
		{
			master.set_text(1, 0, "Sp(X): F");
			pros::delay(50);
			partner.set_text(1, 0, "Sp(X): F");
		}
		else
		{
			master.set_text(1, 0, "Sp(X): S");
			pros::delay(50);
			partner.set_text(1, 0, "Sp(X): S");
		}
		pros::delay(50);
		if (locked)
		{
			master.set_text(1, 9, "Br(A): On ");
			pros::delay(50);
			partner.set_text(1, 9, "Br(A): On ");
		}
		else
		{
			master.set_text(1, 9, "Br(A): Off");
			pros::delay(50);
			partner.set_text(1, 9, "Br(A): Off");
		}
		pros::delay(50);
		if (combined)
		{
			master.set_text(2, 0, "Com(Y): On ");
			pros::delay(50);
			partner.set_text(2, 0, "Com(Y): On ");
		}
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

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
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
	left_mtr_back = 32;
	left_mtr_front = 32;
	right_mtr_back = 32 * -1;
	right_mtr_front = 32 * -1;
	// pros::delay(2);
	// drive forward 
	
	// shoot pistons 
	piston.set_value(piston_state);
	piston_state = true;

	piston.set_value(piston_state);
	piston_state = false;

	// lift armsx`
	right_claw.move_velocity(-50);
	left_claw.move_velocity(50);
	// go back
	left_mtr_back = 32;
	left_mtr_front = 32;
	right_mtr_back = 32 * -1;
	right_mtr_front = 32 * -1;
	// max speed
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
void opcontrol()
{
	if (locked)
	{
		left_claw.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
		right_claw.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
	}

	left_lift.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
	right_lift.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);

	while (true)
	{
		int left = master.get_analog(ANALOG_LEFT_Y);
		int right = master.get_analog(ANALOG_RIGHT_Y);

		left_mtr_back = left / speed;
		left_mtr_front = left / speed;
		right_mtr_back = right / speed * -1;
		right_mtr_front = right / speed * -1;

		// master claw
		if (combined)
		{
			if (master.get_digital(DIGITAL_R2))
			{
				right_claw.move_velocity(-50);
				left_claw.move_velocity(50);
			}
			else if (master.get_digital(DIGITAL_R1))
			{
				right_claw.move_velocity(50);
				left_claw.move_velocity(-50);
			}
			else
			{
				right_claw.move_velocity(0);
				left_claw.move_velocity(0);
			}
		}
		else
		{

			if (master.get_digital(DIGITAL_L2))
			{
				left_claw.move_velocity(50);
			}
			else if (master.get_digital(DIGITAL_L1))
			{
				left_claw.move_velocity(-50);
			}
			else
			{
				left_claw.move_velocity(0);
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

		if (master.get_digital(DIGITAL_UP))
		{
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

		if (master.get_digital_new_press(DIGITAL_X) || partner.get_digital_new_press(DIGITAL_X))
		{
			if (speed == 1)
			{
				speed = 2;
			}
			else
			{
				speed = 1;
			}
		}

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

		if (master.get_digital_new_press(DIGITAL_B) || partner.get_digital_new_press(DIGITAL_B))
		{
			left_claw.move_absolute(200, 100);
			right_claw.move_absolute(-200, 100);
			while (!((left_claw.get_position() < 205) && (left_claw.get_position() > 195)))
			{
				pros::delay(2);
			}
			while (!((right_claw.get_position() < -195) && (right_claw.get_position() > -205)))
			{
				pros::delay(2);
			}
		}

		// left on dpad
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