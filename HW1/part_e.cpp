#include "Aria.h"
#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
	Aria::init();
	ArRobot robot;
	ArSonarDevice sonar;	
	ArSimpleConnector connector(&argc,argv);

	if (!connector.connectRobot(&robot)){
		printf("Could not connect to robot... exiting\n");
		Aria::shutdown();
		Aria::exit(1);
	}

	robot.comInt(ArCommands::ENABLE, 1);

	robot.runAsync(false);

	robot.addRangeDevice(&sonar);

	ArActionGotoStraight gotoPoseAction("goto");
	robot.addAction(&gotoPoseAction, 50);
	
	// Transfer the odomteric pose to the true pose
	ArPose truepose(5090, 3580, 3093.97);
	ArPose odompose(robot.getX(), robot.getY(), robot.getTh());
	robot.moveTo(truepose);

	int X, Y;
	double th;
	while (Aria::getRunning()) 
	{	
		cout << "Type where u want to go. Ex: 7 5 90 (for X(m), Y(m), Theta(deg))\n";
		cin >> X >> Y >> th;

		// Goto the target pose
		gotoPoseAction.setGoal(ArPose(X*1000, Y*1000));
		while (!gotoPoseAction.haveAchievedGoal()){}
		robot.setHeading(th);
		while (!robot.isHeadingDone()){}
		gotoPoseAction.cancelGoal();
		printf("Congratulation! You reach the goal.\n");
		printf("%s %f %s %f %s %f\n", "X(m):", robot.getX()/1000, 
			"Y(m):", robot.getY()/1000, "Theta(deg):", robot.getTh());

		ArUtil::sleep(100);
      		break;
	}
  
	// Robot disconnected or time elapsed, shut down
	Aria::exit(0);
	return 0;
}
