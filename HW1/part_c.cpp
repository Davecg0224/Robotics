
#include "Aria.h"
ArRobot robot;

// set the callbacks
class Callback
{
public:
int g;
void forward();
void backward();
void turnleft();
void turnright();
void stop();
int shift();
};

void Callback::forward()
{	
	// speeding up depends on shifted gear
	if (g != 0)
	{
		robot.setVel(200 * g);
	}
	// if not shifting, stay in initial velocity
	else 
	{
		robot.setVel(50);
	}	
}

void Callback::backward(){robot.setVel(-200);}
void Callback::turnleft(){robot.setRotVel(30);}
void Callback::turnright(){robot.setRotVel(-30);}

int Callback::shift()
{
	g++;
	g = g % 6;	// There are 5 gears in this vehicle to shift.
	printf("%s %d\n", "<Now at gear>", g);
	return g;
}

void Callback::stop()
{
	robot.setRotVel(0);
	robot.setVel(0);
}

Callback cb;
ArFunctorC<Callback> up(cb, &Callback::forward);
ArFunctorC<Callback> down(cb, &Callback::backward);
ArFunctorC<Callback> left(cb, &Callback::turnleft);
ArFunctorC<Callback> right(cb, &Callback::turnright);
ArFunctorC<Callback> freeze(cb, &Callback::stop);
ArRetFunctorC<int, Callback> gear(cb, &Callback::shift);

int main(int argc, char **argv)
{
	Aria::init();
	
	ArSimpleConnector connector(&argc,argv);

	if (!connector.connectRobot(&robot)){
		printf("Could not connect to robot... exiting\n");
		Aria::shutdown();
		Aria::exit(1);
	}

	robot.comInt(ArCommands::ENABLE, 1);

	robot.runAsync(false);

	// Used to perform actions when keyboard keys are pressed
	ArKeyHandler keyHandler;
	Aria::setKeyHandler(&keyHandler);

	// ArRobot contains an exit action for the Escape key. It also 
	// stores a pointer to the keyhandler so that other parts of the program can
	// use the same keyhandler.
	robot.attachKeyHandler(&keyHandler);
	printf("You may press escape to exit\n");

	// Start of controling
	// 1. Lock the robot
	robot.lock();

	// 2. Write your control code here, 
	keyHandler.addKeyHandler('w', &up);
  	keyHandler.addKeyHandler('s', &down);
  	keyHandler.addKeyHandler('a', &left);  
  	keyHandler.addKeyHandler('d', &right);
  	keyHandler.addKeyHandler( ArKeyHandler::SPACE, &freeze);
  	keyHandler.addKeyHandler( ArKeyHandler::TAB, &gear);
	// 3. Unlock the robot
	robot.unlock();

	// 4. Sleep a while and let the robot move
	while(true){
		// turn the velocity to 0 if no button be pressed
		if (keyHandler.getKey() == -1) {cb.stop();}
		printf("%s %f %s %f %s %f %s %f %s %f\n",
			"X:", robot.getX(), "Y:", robot.getY(), "Theta:", robot.getTh(), 
			"v:", robot.getVel(), "w:", robot.getRotVel());
		ArUtil::sleep(300);
	}

	// End of controling

	Aria::shutdown();

	Aria::exit(0);
}
