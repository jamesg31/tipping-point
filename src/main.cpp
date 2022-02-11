/* Preproccesor */
#include "main.h"
#include <string>
#define DIGITAL_SENSOR_PORT 'H'

/* Controllers */
pros::Controller master(pros::E_CONTROLLER_MASTER); // Main controller
pros::Controller partner(pros::E_CONTROLLER_PARTNER); // Partner controller (connected to main)

/* Motors */
pros::Motor left_mtr_front(4); // Movement motor (left front). Assigned to port: 4
pros::Motor left_mtr_back(19); // Movement motor (left back). Assigned to port: 19
pros::Motor right_mtr_front(14); // Movement motor (right front). Assigned to port: 14
pros::Motor right_mtr_back(16); // Movement motor (right back). Assigned to port: 16
pros::Motor left_claw(1); // Claw motor. Assigned to port: 1
pros::Motor right_claw(2); // Claw motor. Assigned to port: 2
pros::Motor left_lift(17); // Lift motor. Assigned to port: 17
pros::Motor right_lift(20); // Lift motor. Assigned to port: 20

// Program variables
int speed = 2; // Controls the speed of the robot. In this context, a higher value correlates to a slower robot, since the input is *divided* by this value
// Extra note: the speed variable works, in essence, as a boolean, especially since the speed is shown as either slow or fast depending on the value here.

bool locked = true; // Controls whether or not the brakes are locked (enabled)
bool combined = true; // Controls whether or not the left and right movement motor pairs move in synchronization.
bool piston_state = false;

// function to display to lcd
void display_fn()
{
	master.clear();
	
	// Main loop
	while (true)
	{
		double battery = pros::battery::get_capacity();
		master.set_text(0, 0, std::to_string(battery).substr(0, 2) + "% " + std::to_string(pros::competition::get_status()) + " " + std::to_string(left_claw.get_position()).substr(0, 2));
		pros::delay(50);
		partner.set_text(0, 0, std::to_string(battery).substr(0, 2) + "% " + std::to_string(pros::competition::get_status()) + " " + std::to_string(right_claw.get_position()).substr(0, 2));
		pros::delay(50);
		if (speed == 1)
		{
			// Sets speed to fast on lcd
			master.set_text(1, 0, "Sp(X): F");
			pros::delay(50);
			partner.set_text(1, 0, "Sp(X): F");
		}
		else
		{
			// Sets speed to slow on lcd
			master.set_text(1, 0, "Sp(X): S");
			pros::delay(50);
			partner.set_text(1, 0, "Sp(X): S");
		}
		pros::delay(50);
		if (locked)
		{
			// Shows brake status on lcd
			master.set_text(1, 9, "Br(A): On ");
			pros::delay(50);
			partner.set_text(1, 9, "Br(A): On ");
		}
		else
		{
			// Shows brake status on lcd
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
	// The Task class is a task that will be run at one point. It takes in a function.
	pros::Task display_task(display_fn); // Sets the display_fn() function to be run soon
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
	pros::ADIDigitalOut piston(DIGITAL_SENSOR_PORT);

	if (locked) // Effect of locked boolean. Sets the brake mode to HOLD on movement mtrs
	{
		left_claw.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
		right_claw.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
	}

	// Locks lift motors
	left_lift.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
	right_lift.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);

	while (true)
	{
		// Gets how far the analog stick has been pushed as two integers (-127 to 127): https://pros.cs.purdue.edu/v5/api/cpp/misc.html?highlight=get_analog#get-analog
		int left = master.get_analog(ANALOG_LEFT_Y);
		int right = master.get_analog(ANALOG_RIGHT_Y);

		/*
			IMPORTANT: this piece of code is weird. The PROS developers have overloaded the assignment operator (=) in a similar way to overloading a method (that's c++ for you)
			to make it take the effect of setting the position. 
			
			An equivalent block, if the method to set motor position is called set_pos():
			left_mtr_back.set_pos(left/speed);
			left_mtr_front.set_pos(left/speed);
			right_mtr_back.set_pos(right/speed*-1);
			right_mtr_front.set_pos(right/speed*-1);
			
			Overall, this code block moves the robot according to analog stick input, toggled to either fast (default, where speed = 1), or slow (where speed =2 and the input is cut in half)
		*/
		left_mtr_back = left / speed;
		left_mtr_front = left / speed;
		right_mtr_back = right / speed * -1;
		right_mtr_front = right / speed * -1;

		// master claw
		if (combined)
		{
			if (master.get_digital(DIGITAL_R1)) // Checks whether the Right Top trigger is pressed
			{
				// If so, rotate the claw in favor of the left claw (FIXME)
				right_claw.move_velocity(-50);
				left_claw.move_velocity(50);
			}
			else if (master.get_digital(DIGITAL_R2)) // Checks whether the Right Bottom trigger is pressed
			{
				// If so, rotate the claw in favor of the right claw (FIXME)
				right_claw.move_velocity(50);
				left_claw.move_velocity(-50);
			}
			else
			{
				// Reset the claw (keep it at a fixed position)
				right_claw.move_velocity(0);
				left_claw.move_velocity(0);
			}
		}
		else
		{
			/* The following if statements are similar to the ones above, except written for individual control using the Left and Right bumpers */
			if (master.get_digital(DIGITAL_L1))
			{
				left_claw.move_velocity(50);
			}
			else if (master.get_digital(DIGITAL_L2))
			{
				left_claw.move_velocity(-50);
			}
			else
			{
				left_claw.move_velocity(0);
			}

			if (master.get_digital(DIGITAL_R1))
			{
				right_claw.move_velocity(-50);
			}
			else if (master.get_digital(DIGITAL_R2))
			{
				right_claw.move_velocity(50);
			}
			else
			{
				right_claw.move_velocity(0);
			}
		}

		if (master.get_digital(DIGITAL_UP)) // Check if the up button in the DPAD is pressed
		{
			// If so, move the lift up (FIXME)
			left_lift.move_velocity(-45);
			right_lift.move_velocity(45);
		}
		else if (master.get_digital(DIGITAL_DOWN)) // Check if the down button in the DPAD is pressed
		{
			// If so, move the lift down (FIXME)
			left_lift.move_velocity(45);
			right_lift.move_velocity(-45);
		}
		else
		{
			// Reset the lift (lock it at the current position)
			left_lift.move_velocity(0);
			right_lift.move_velocity(0);
		}

		// Toggle speed to fast or slow if either controllers press the X button
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

		// Toggle the locked (brake) state if either controllers press the A button (and update the brake mode)
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

		// Toggle the combined state (whether the movement motors move in unison) if either controllers press the Y button
		if (master.get_digital_new_press(DIGITAL_Y) || partner.get_digital_new_press(DIGITAL_Y))
		{
			if (combined)
			{
				combined = false;
			}
			else
			{
				combined = true;
			}
		}

		// TODO not sure what this is for
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

		// Toggle whether the piston (pneumatics!!) are flowing if either controller presses the left button on the DPAD
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
