#include "object3D.h"
#include "object3Dcube.h"
#include "object3Dcylinder.h"
#include "object3Dsphere.h"

#include <algorithm>

enum RelationType
{
	Stranger = 0,		// not even close
	Ambiguous = 1,		// collide
	FallInLove = 2,		// center of one is inside another
	Breaking = 3,		// overlap right now but getting far away
};

enum CollisionFreeStuckType
{
	BothFree = 0,
	Stuck1st = 1,
	Stuck2nd = 2,
	BothStuck = 3
};

enum CubeFace
{
	XPos = 0,
	XNeg = 1,
	YPos = 2,
	YNeg = 3,
	ZPos = 4,
	ZNeg = 5,
};

typedef struct __collisionInfo {

	RelationType relation;
	glm::vec3 v1After;
	glm::vec3 v2After;

	//glm::vec3 omegaDelta1;
	//glm::vec3 omegaDelta2;

	glm::vec3 angularImpulse1;
	glm::vec3 angularImpulse2;

	CubeFace collideFace;	// cube

} CollisionInfo;

CollisionInfo CollideSph2Sph(Object3Dsphere * obj1, Object3Dsphere * obj2, bool autoDeal);

CollisionInfo CollideSph2Cube(Object3Dsphere * sphere, Object3Dcube * cube, bool autoDeal);

//CollisionInfo CollideSph2Cube(Object3Dsphere ** spheres, Object3Dcube ** cubes, bool autoDeal);

void CollideSph2Cube(std::vector<Object3Dsphere*> &spheres, std::vector<Object3Dcube*> &cubes, bool autoDeal);

void DealCollision(Object3D * obj1, Object3D * obj2, CollisionInfo cInfo);

void printVec3(std::string name, glm::vec3 v);
void printVec3(glm::vec3 v);