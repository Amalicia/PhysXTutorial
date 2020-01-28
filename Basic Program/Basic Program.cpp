#include <iostream> //cout, cerr
#include <iomanip> //stream formatting
#include <windows.h> //delay function
#include <PxPhysicsAPI.h> //PhysX

using namespace std;
using namespace physx;

//PhysX objects
PxPhysics* physics = 0;
PxFoundation* foundation = 0;
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
debugger::comm::PvdConnection* pvd = 0;
#else
PxPvd*  pvd = 0;
#endif

//simulation objects
PxScene* scene;
PxRigidDynamic* box;
PxRigidStatic* box18;
PxRigidStatic* plane;

///Initialise PhysX objects
bool PxInit()
{
	//default error and allocator callbacks
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	//Init PhysX
	//foundation
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
#else
	foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
#endif

	if(!foundation)
		return false;

	//connect to an external visual debugger (if exists)
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
	pvd = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), "localhost", 5425, 100,
		PxVisualDebuggerExt::getAllConnectionFlags());
#else
	pvd = PxCreatePvd(*foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
	pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif

	//physics
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());
#else
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
#endif

	if(!physics)
		return false;

	//create a default scene
	PxSceneDesc sceneDesc(physics->getTolerancesScale());

	if(!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if(!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	scene = physics->createScene(sceneDesc);

	if (!scene)
		return false;

	return true;
}

/// Release all allocated resources
void PxRelease()
{
	if (scene)
		scene->release();
	if (physics)
		physics->release();
	if (pvd)
		pvd->release();
	if (foundation)
		foundation->release();
}

///Initialise the scene
void InitScene()
{
	//default gravity
	scene->setGravity(PxVec3(0.f, -9.81f, 0.f));

	//materials
	PxMaterial* default_material = physics->createMaterial(0.f, .07f, .5f);   //static friction, dynamic friction, restitution

	//create a static plane (XZ)
	plane = PxCreatePlane(*physics, PxPlane(PxVec3(0.f, 1.f, 0.f), 0.f), *default_material);
	scene->addActor(*plane);

	//create a dynamic actor and place it 10 m above the ground
	box = physics->createRigidDynamic(PxTransform(PxVec3(0.f, 101.f, 0.f)));
	//create a box shape of 1m x 1m x 1m size (values are provided in halves)
	box->createShape(PxBoxGeometry(.5f, .5f, .5f), *default_material);
	//update the mass of the box
	PxRigidBodyExt::updateMassAndInertia(*box, 1.f); //density of 1kg/m^3

	scene->addActor(*box);

	box = physics->createRigidDynamic(PxTransform(PxVec3(5.f, 101.f, 0.f)));
	box->createShape(PxBoxGeometry(.5f, .5f, .5f), *default_material);
	PxRigidBodyExt::updateMassAndInertia(*box, 1.f); //density of 1kg/m^3
	scene->addActor(*box);

	box = physics->createRigidDynamic(PxTransform(PxVec3(10.f, 101.f, 0.f)));
	box->createShape(PxBoxGeometry(.5f, .5f, .5f), *default_material);
	PxRigidBodyExt::updateMassAndInertia(*box, 1.f); //density of 1kg/m^3
	scene->addActor(*box);


	 //create a dynamic actor and place it 10 m above the ground
	box18 = physics->createRigidStatic(PxTransform(PxVec3(0.f, 1.f, 0.f)));
	//create a box shape of 1m x 1m x 1m size (values are provided in halves)
	box18->createShape(PxBoxGeometry(1.f, 1.f, 1.f), *default_material);
	//update the mass of the box
	//PxRigidBodyExt::updateMassAndInertia(*box18, 1.f); //density of 1kg/m^3
	//box18->setRigidBodyFlag(PxRigidBodyFlag::, true);

	box = physics->createRigidDynamic(PxTransform(PxVec3(5.f, 1.f, 0.f)));
	box->createShape(PxBoxGeometry(1.f, 1.f, 1.f), *default_material);
	PxRigidBodyExt::updateMassAndInertia(*box, 1.f);

	scene->addActor(*box);

	box = physics->createRigidDynamic(PxTransform(PxVec3(10.f, 1.f, 0.f)));
	box->createShape(PxBoxGeometry(1.f, 1.f, 1.f), *default_material);
	PxRigidBodyExt::updateMassAndInertia(*box, 1.f);
	box->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	scene->addActor(*box);

	scene->addActor(*box18);
}

/// Perform a single simulation step
void Update(PxReal delta_time)
{
	scene->simulate(delta_time);
	scene->fetchResults(true);
}

/// The main function
int main()
{
	//initialise PhysX	
	if (!PxInit())
	{
		cerr << "Could not initialise PhysX." << endl;
		return 0;
	}

	//initialise the scene
	InitScene();

	//set the simulation step to 1/60th of a second
	PxReal delta_time = 1.f/60.f;

	//simulate until the 'Esc' is pressed

	float count = 0;

	cout << box->getMass() << endl;


	//box->addForce(PxVec3(100.f, 0, 0));

	bool checkBol = false;

	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		//'visualise' position and velocity of the box
		PxVec3 position = box->getGlobalPose().p;
		PxVec3 velocity = box->getLinearVelocity();
		cout << setiosflags(ios::fixed) << setprecision(2) << "x=" << position.x << 
			", y=" << position.y << ", z=" << position.z << ",  ";
		cout << setiosflags(ios::fixed) << setprecision(2) << "vx=" << velocity.x << 
			", vy=" << velocity.y << ", vz=" << velocity.z << endl;
		cout << "counter: " << count << endl;

		//if (velocity.y < -0.01f)
		{
			count ++;
		}

		if (box->isSleeping())
		{
			cout << "\n\n\n\n\n SLeepyboiii";
		}




		if (position.x >= 10)
		{
			velocity.x = 0;
			box->setLinearVelocity(velocity);
		}
		
		//perform a single simulation step
		Update(delta_time);
		
		//introduce 100ms delay for easier visual analysis of the results
		Sleep(1);
	}

	//Release all resources
	PxRelease();

	return 0;
}
