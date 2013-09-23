#include <iostream>
#include <windows.h> //delay function
#include "VisualDebugger.h"
#include "MyPhysicsEngine.h"

using namespace std;
using namespace physx;
using namespace PhysicsEngine;

int main()
{
	MyScene scene;

	//initialise PhysX and the scene	
	try 
	{ 
		PxInit();
		scene.Init();
	}
	catch (Exception exc) 
	{ 
		cerr << exc.what() << endl;
		return 0; 
	}

	VisualDebugger::SetActiveScene(scene);
	
	VisualDebugger::Init("PhysX Tutorial");

	VisualDebugger::Start();

	return 0;
}