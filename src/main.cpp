#include "main.h"
#include <string>

pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::Motor left_mtr(2);
pros::Motor right_mtr(3);
pros::Motor left_claw(4);
pros::Motor right_claw(5);

int speed = 2;
bool locked = true;

void display_fn()
{
	master.clear();
	while (true)
	{
		double battery = master.get_battery_capacity();
		master.set_text(0, 0, std::to_string(battery).substr(0, 2) + "% " + std::to_string(pros::competition::get_status()));
		pros::delay(50);
		if (speed == 1)
		{
			master.set_text(1, 0, "Sp(X): F");
		}
		else
		{
			master.set_text(1, 0, "Sp(X): S");
		}
		pros::delay(50);
		if (locked)
		{
			master.set_text(1, 9, "Br(A): On ");
		}
		else
		{
			master.set_text(1, 9, "Br(A): Off");
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
void autonomous() {}

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

	while (true)
	{
		int left = master.get_analog(ANALOG_LEFT_Y);
		int right = master.get_analog(ANALOG_RIGHT_Y);

		left_mtr = left / speed;
		right_mtr = right / speed;

		if (master.get_digital(DIGITAL_L1))
		{
			left_claw.move_velocity(35);
		}
		else if (master.get_digital(DIGITAL_L2))
		{
			left_claw.move_velocity(-35);
		}
		else
		{
			left_claw.move_velocity(0);
		}

		if (master.get_digital(DIGITAL_R1))
		{
			right_claw.move_velocity(-35);
		}
		else if (master.get_digital(DIGITAL_R2))
		{
			right_claw.move_velocity(35);
		}
		else
		{
			right_claw.move_velocity(0);
		}

		if (master.get_digital_new_press(DIGITAL_X))
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

		if (master.get_digital_new_press(DIGITAL_A))
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

		pros::delay(10);
	}
}