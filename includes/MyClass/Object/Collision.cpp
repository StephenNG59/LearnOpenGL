#include "stdafx.h"
#include "Collision.h"

extern float deltaTime;

void printVec3(std::string name, glm::vec3 v)
{
	std::cout << name << ": ";
	std::cout << v.x << " # " << v.y << " # " << v.z << std::endl;
}
void printVec3(glm::vec3 v)
{
	std::cout << "vec3: " << v.x << " # " << v.y << " # " << v.z << std::endl;
}

CollisionFreeStuckType checkFStype(float m1, float m2)
{
	if (m1 == 0 && m2 == 0)
	{
		return CollisionFreeStuckType::BothStuck;
	}
	else if (m1 == 0)
	{
		return CollisionFreeStuckType::Stuck1st;
	}
	else if (m2 == 0)
	{
		return CollisionFreeStuckType::Stuck2nd;
	}
	else
	{
		return CollisionFreeStuckType::BothFree;
	}
}

CollisionInfo CollideSph2Sph(Object3Dsphere * sph1, Object3Dsphere * sph2, bool autoDeal)
{

	CollisionInfo cInfo;
	CollisionFreeStuckType fsType;

	// ---------------
	// get information

	float m1 = sph1->GetMass(), m2 = sph2->GetMass();
	fsType = checkFStype(m1, m2);

	float r1_abs = sph1->GetRadius(), r2_abs = sph2->GetRadius();

	glm::vec3 p1 = sph1->GetPosition(), p2 = sph2->GetPosition();
	glm::vec3 v1 = sph1->GetVelocity(), v2 = sph2->GetVelocity();

	//glm::vec3 w1 = sph1->GetOmega(), w2 = sph2->GetOmega();							// omega
	glm::vec3 w1LC = sph1->GetOmega(), w2LC = sph2->GetOmega();
	// #NOTE remember to translate LC coord to WC coord!!!
	glm::vec3 w1 = glm::vec3(sph1->GetModelMatrix() * glm::vec4(w1LC, 0.0f)), w2 = glm::vec3(sph2->GetModelMatrix() * glm::vec4(w2LC, 0.0f));


	// --------------
	// detect collide

	glm::vec3 vec1to2 = p2 - p1;
	glm::vec3 xStar = glm::normalize(vec1to2);
												
	float dis = vecMod(vec1to2);														// #NOTE - length()ÊÇÖ¸ÏòÁ¿Î¬¶È!!!!(vec3.length() == 3)

	if (dis >= r1_abs + r2_abs)		
	{
		// not collide
		cInfo.relation = RelationType::Stranger;
		return cInfo;
	}
	
	float v1_xstar = glm::dot(v1, xStar), v2_xstar = glm::dot(v2, xStar);			// v1-->v2 as positive x direction
	

	// #NOTE conditions like a ball stay on the desk
	if (dis < r1_abs + r2_abs/* && abs(v1_xstar - v2_xstar) < 0.01f*/)
	{
		glm::vec3 delta_p = (r1_abs + r2_abs - dis) * xStar;
		if (fsType == CollisionFreeStuckType::BothFree)
		{
			sph1->ChangePosition(-delta_p / 2.0f);
			sph2->ChangePosition(delta_p / 2.0f);
		}
		else if (fsType == CollisionFreeStuckType::Stuck1st)
		{
			sph2->ChangePosition(delta_p);
		}
		else if (fsType == CollisionFreeStuckType::Stuck2nd)
		{
			sph1->ChangePosition(-delta_p);
		}
	}


	if (v1_xstar < v2_xstar)							
	{
		// not collide but overlap
		cInfo.relation = RelationType::Breaking;
		return cInfo;
	}

	cInfo.relation = RelationType::Ambiguous;


	// -----------------------------------------------------------
	// calculate velocity change caused by center-center collision

	glm::vec3 v1_subxstar = v1 - v1_xstar * xStar, v2_subxstar = v2 - v2_xstar * xStar;		// the remaining velocity would not change in the collision

	float e = sph1->GetERestitution() * sph2->GetERestitution();					// coefficient of restitution

	// velocity on the direction of center line    
	// #NOTE still need to consider the angular velocity!!!
	float v1_xstar_prime, v2_xstar_prime, I_xstar_abs;
	float vxDelta1, vxDelta2;	
	glm::vec3 I21_xstar = glm::vec3(0), I12_xstar = glm::vec3(0);
	
	if (fsType == CollisionFreeStuckType::BothFree)
	{
		v1_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar - m2 * e * (v1_xstar - v2_xstar)) / (m1 + m2);
		v2_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar + m1 * e * (v1_xstar - v2_xstar)) / (m1 + m2);
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I12_xstar = -I21_xstar;
		// vxDelta1 = v1_xstar_prime - v1_xstar;
		I_xstar_abs = vecMod(I21_xstar);
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		v1_xstar_prime = v1_xstar;		
		v2_xstar_prime = (1 + e) * v1_xstar - e * v2_xstar;
		I12_xstar = m2 * (v2_xstar_prime - v2_xstar) * xStar;
		I_xstar_abs = vecMod(I12_xstar);
		// vxDelta2 = v2_xstar_prime - v2_xstar;
		// I_xstar_abs = abs(vxDelta2 * m2);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		v1_xstar_prime = (1 + e) * v2_xstar - e * v1_xstar;
		v2_xstar_prime = v2_xstar;
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I_xstar_abs = vecMod(I21_xstar);
		// vxDelta1 = v1_xstar_prime - v1_xstar;
		// I_xstar_abs = abs(vxDelta1 * m1);
	}
	else if (fsType == CollisionFreeStuckType::BothStuck)
	{
		v1_xstar_prime = v1_xstar;
		v2_xstar_prime = v2_xstar;
		return cInfo;
	}


	// ----------------------
	// calculate omega change

	float f1 = sph1->GetFriction(), f2 = sph2->GetFriction();
	glm::vec3 r1 = xStar * r1_abs, r2 = -xStar * r2_abs;

	// calculate direction of relative velocity on the collide point
	glm::vec3 v1touch = (w1 == glm::vec3(0.0f)) ? (v1) : (v1 + glm::cross(w1, r1));
	glm::vec3 v2touch = (w2 == glm::vec3(0.0f)) ? (v2) : (v2 + glm::cross(w2, r2));
	glm::vec3 v12touch = v1touch - v2touch, v21touch = -v12touch;
	glm::vec3 v12touch_yzstar = v12touch - glm::dot(v12touch, xStar) * xStar, v21touch_yzstar = -v12touch_yzstar;

	// glm::vec3 vTouch12Norm = glm::normalize(v1touch - v2touch), vTouch21Norm = -vTouch12Norm;

	// momentum change caused by yzstar part of the impulse

	glm::vec3 delta_L1(0), delta_L2(0);
	glm::vec3 v1_after = v1_xstar_prime * xStar + v1_subxstar;
	glm::vec3 v2_after = v2_xstar_prime * xStar + v2_subxstar;


	if (v12touch_yzstar == glm::vec3(0) || I_xstar_abs == 0)
	{
		cInfo.v1After = v1_after;
		cInfo.v2After = v2_after;
		cInfo.angularImpulse1 = cInfo.angularImpulse2 = glm::vec3(0);

		if (autoDeal)
		{
			DealCollision(sph1, sph2, cInfo);
		}

		return cInfo;
	}

	// --(1)--
	float I1 = sph1->GetInertia(), I2 = sph2->GetInertia();
	float m_equivalent;

	if (fsType == CollisionFreeStuckType::BothFree)
	{
		m_equivalent = 1 / (((r1_abs * r1_abs) / (I1) + 1 / m1) + ((r2_abs * r2_abs) / (I2 * I2) + 1 / m2));
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		m_equivalent = 1 / ((r2_abs * r2_abs) / (I2) + 1 / m2);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		m_equivalent = 1 / ((r1_abs * r1_abs) / (I1) + 1 / m1);
	}

	float I_yzstar_abs = std::min(0.5f * (f1 + f2) * I_xstar_abs, vecMod(v12touch_yzstar) * m_equivalent);

	// --(2)--
	glm::vec3 v12_yzstar_norm = glm::normalize(v12touch_yzstar), v21_yzstar_norm = -v12_yzstar_norm;
	// --(3)--
	delta_L1 = glm::cross(r1, I_yzstar_abs * v21_yzstar_norm);
	delta_L2 = glm::cross(r2, I_yzstar_abs * v12_yzstar_norm + I12_xstar);
	cInfo.angularImpulse1 = delta_L1;
	cInfo.angularImpulse2 = delta_L2;


	glm::vec3 delta_v1(0), delta_v2(0);
	if (fsType == CollisionFreeStuckType::BothFree)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
	}

	v1_after += delta_v1;
	v2_after += delta_v2;

	cInfo.v1After = v1_after;
	cInfo.v2After = v2_after;


	// --------------
	// deal collision

	if (autoDeal)
	{
		DealCollision(sph1, sph2, cInfo);
	}

	return cInfo;
}

CollisionInfo CollideSph2Cube(Object3Dsphere * sphere, Object3Dcube * cube, bool autoDeal)
{

	CollisionInfo cInfo;
	CollisionFreeStuckType fsType;

	float m1 = sphere->GetMass(), m2 = cube->GetMass();
	fsType = checkFStype(m1, m2);


	// ----------------------------------------------------------------
	// get current width, height, depth direction of the (rotated) cube

	glm::vec3 cubeX = glm::vec3(cube->GetRotationMatrix() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));		// width direction
	glm::vec3 cubeY = glm::vec3(cube->GetRotationMatrix() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));		// height direction
	glm::vec3 cubeZ = glm::vec3(cube->GetRotationMatrix() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));		// depth direction
	
	// normalize them to be the x, y, z unit vector

	glm::vec3 cubeXNorm = glm::normalize(cubeX);
	glm::vec3 cubeYNorm = glm::normalize(cubeY);
	glm::vec3 cubeZNorm = glm::normalize(cubeZ);


	// -------------
	// get cube size

	float width = cube->GetWidth(), height = cube->GetHeight(), depth = cube->GetDepth();
	float halfW = width / 2.0f, halfH = height / 2.0f, halfD = depth / 2.0f;
	glm::vec3 cubeHalfExtents(halfW, halfH, halfD);

	glm::vec3 vecCube2SphWC = sphere->GetPosition() - cube->GetPosition();					// world coordinate
	float cube2SphXLC = glm::dot(vecCube2SphWC, cubeXNorm);
	float cube2SphYLC = glm::dot(vecCube2SphWC, cubeYNorm);
	float cube2SphZLC = glm::dot(vecCube2SphWC, cubeZNorm);
	if (abs(cube2SphXLC) < halfW && abs(cube2SphYLC) < halfH && abs(cube2SphZLC) < halfD)
	{
		// the center of the circle is inside the cube
		cInfo.relation = RelationType::FallInLove;
		return cInfo;
	}


	glm::vec3 vecCube2SphLC = glm::vec3(cube2SphXLC, cube2SphYLC, cube2SphZLC);				// local coordinate
	glm::vec3 closestLC = glm::clamp(vecCube2SphLC, -cubeHalfExtents, cubeHalfExtents);		// closest point on cube in local coord
	glm::vec3 closestWC = glm::vec3(cube->GetModelMatrix() * glm::vec4(closestLC, 1.0f));	// closest point on cube in world coord
	glm::vec3 cube2ClosestWC = closestWC - cube->GetPosition();								// cube's center to closest point


	// ---------------
	// get sphere data

	glm::vec3 pSph = sphere->GetPosition();
	float rSph = sphere->GetRadius();


	// --------------
	// detect collide

	glm::vec3 closest2Sph = pSph - closestWC; 
	float dis = vecMod(closest2Sph);
	if (dis > rSph)
	{
		// not collide
		sphere->IsTouchingDesk = false;
		cInfo.relation = RelationType::Stranger;
		return cInfo;
	}
	// if (dis < rSph && is moving apart relatively) { relation = Breaking; } 
	cInfo.relation = RelationType::Ambiguous;


	// determine which face is colliding
	
	CubeFace collideFace;
	glm::vec3 xStar;																		// #NOTE xStar: sphere center to the collide point vector in WC
	if (closestLC.x == halfW) {
		collideFace = CubeFace::XPos;
		xStar = -cubeXNorm;
	}
	else if (closestLC.x == -halfW) {
		collideFace = CubeFace::XNeg;
		xStar = cubeXNorm;
	}
	else if (closestLC.y == halfH) {
		collideFace = CubeFace::YPos;
		xStar = -cubeYNorm;
	}
	else if (closestLC.y == -halfH) {
		collideFace = CubeFace::YNeg;
		xStar = cubeYNorm;
	}
	else if (closestLC.z == halfD) {
		collideFace = CubeFace::ZPos;
		xStar = -cubeZNorm;
	}
	else if (closestLC.z == -halfD) {
		collideFace = CubeFace::ZNeg;
		xStar = cubeZNorm;
	}


	// -----------------
	// start calculation

	// -- step1: velocity and impulse --

	float e = sphere->GetERestitution() * cube->GetERestitution();
	glm::vec3 v1 = sphere->GetVelocity(), v2 = cube->GetVelocity();

	float v1_xstar = glm::dot(v1, xStar), v2_xstar = glm::dot(v2, xStar);

	glm::vec3 force_friction(0);
	// #NOTE conditions like a ball stay on the desk
	if (/*abs(v1_xstar - v2_xstar) < 0.1f &&*/ dis < rSph && abs(v1_xstar - v2_xstar) < 0.1f)
	{
		glm::vec3 delta_p = (rSph - dis) * xStar;
		if (fsType == CollisionFreeStuckType::BothFree)
		{
			sphere->ChangePosition(-delta_p / 2.0f);
			cube->ChangePosition(delta_p / 2.0f);
		}
		else if (fsType == CollisionFreeStuckType::Stuck1st)
		{
			cube->ChangePosition(delta_p);
		}
		else if (fsType == CollisionFreeStuckType::Stuck2nd)
		{
			sphere->ChangePosition(-delta_p);
		}
	}


	if (v1_xstar < v2_xstar)
	{
		// #NOTE - if we do not detect this, before the first collision finishes, the second one will be performed
		//		 - which will make the two objects look like stuck with each other
		cInfo.relation = RelationType::Breaking;
		return cInfo;
	}
	glm::vec3 v1_subxstar = v1 - v1_xstar * xStar, v2_subxstar = v2 - v2_xstar * xStar;



	float v1_xstar_prime, v2_xstar_prime, I_xstar_abs = 0;
	glm::vec3 I21_xstar = glm::vec3(0), I12_xstar = glm::vec3(0);
	if (fsType == CollisionFreeStuckType::BothFree)
	{
		v1_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar - e * m2 * (v1_xstar - v2_xstar)) / (m1 + m2);
		v2_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar + e * m1 * (v1_xstar - v2_xstar)) / (m1 + m2);
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I12_xstar = -I21_xstar;
		I_xstar_abs = vecMod(I21_xstar);
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		v1_xstar_prime = v1_xstar;
		v2_xstar_prime = (1 + e) * v1_xstar - e * v2_xstar;
		I12_xstar = m2 * (v2_xstar_prime - v2_xstar) * xStar;
		I_xstar_abs = vecMod(I12_xstar);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		v1_xstar_prime = (1 + e) * v2_xstar - e * v1_xstar;
		v2_xstar_prime = v2_xstar;
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I_xstar_abs = vecMod(I21_xstar);
	}
	else if (fsType == CollisionFreeStuckType::BothStuck)
	{
		v1_xstar_prime = v1_xstar;
		v2_xstar_prime = v2_xstar;
		return cInfo;
	}


	// -- step2: angular velocity --

	glm::vec3 w1LC = sphere->GetOmega(), w2LC = cube->GetOmega();
	// #NOTE remember to translate LC coord to WC coord!!!
	glm::vec3 w1 = glm::vec3(sphere->GetModelMatrix() * glm::vec4(w1LC, 0.0f)), w2 = glm::vec3(cube->GetModelMatrix() * glm::vec4(w2LC, 0.0f));
	float f1 = sphere->GetFriction(), f2 = cube->GetFriction();
	glm::vec3 r1 = sphere->GetRadius() * xStar, r2 = cube2ClosestWC;					// #NOTE ÕâÀïÒ»¿ªÊ¼ÓÃglm::vec3 r1 = r1 * xStar; »áÓÐbug

	glm::vec3 v1touch = (w1 == glm::vec3(0.0f)) ? (v1) : (v1 + glm::cross(w1, r1));
	glm::vec3 v2touch = (w2 == glm::vec3(0.0f)) ? (v2) : (v2 + glm::cross(w2, r2));
	glm::vec3 v12touch = v1touch - v2touch, v21touch = -v12touch;


	glm::vec3 v12touch_yzstar = v12touch - glm::dot(v12touch, xStar) * xStar, v21touch_yzstar = -v12touch_yzstar;

	// momentum change caused by yzstar part of the impulse

	glm::vec3 v1_after = v1_xstar_prime * xStar + v1_subxstar;
	glm::vec3 v2_after = v2_xstar_prime * xStar + v2_subxstar;
	

	if (v12touch_yzstar == glm::vec3(0)/* || I_xstar_abs == 0*/)			// #INNORMAL when I_xstar_abs == 0, maybe a static friction
	{
		glm::vec3 angularV = glm::cross(w1, r1);

		cInfo.v1After = v1_after;
		cInfo.v2After = v2_after;
		cInfo.angularImpulse1 = cInfo.angularImpulse2 = glm::vec3(0);

		if (autoDeal)
		{
			DealCollision(sphere, cube, cInfo);
		}

		return cInfo;
	}


	// --(1)--
	float I1 = sphere->GetInertia(), I2 = cube->GetInertia();
	float r2_abs = vecMod(cube2ClosestWC);
	float m_equivalent;
	bool is_static_friction = (dis <= rSph && abs(v1_xstar - v2_xstar) < 0.1f);
	float force_N_abs = 0;

	if (fsType == CollisionFreeStuckType::BothFree)
	{
		m_equivalent = 1 / (((rSph * rSph) / (I1) + 1 / m1) + ((r2_abs * r2_abs) / (I2 * I2) + 1 / m2));
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		m_equivalent = 1 / ((r2_abs * r2_abs) / (I2) + 1 / m2);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		m_equivalent = 1 / ((rSph * rSph) / (I1) + 1 / m1);
	}

	float I_yzstar_abs = std::min(0.5f * (f1 + f2) * I_xstar_abs, vecMod(v12touch_yzstar) * m_equivalent);		// #INNORMAL when rotate on the desk, the left one would be very small!!
	// #NOTE static friction
	if (is_static_friction)
	{
		sphere->IsTouchingDesk = true;

		if (fsType == CollisionFreeStuckType::BothFree)
			force_N_abs = 0;
		else if (fsType == CollisionFreeStuckType::Stuck1st)
			force_N_abs = std::max(0.0f, glm::dot(cube->GetForce(), -xStar));
		else if (fsType == CollisionFreeStuckType::Stuck2nd)
			force_N_abs = std::max(0.0f, glm::dot(sphere->GetForce(), xStar));
		
		I_yzstar_abs = std::min(force_N_abs * deltaTime, vecMod(v12touch_yzstar) * m_equivalent);

	}


	// --(2)--
	glm::vec3 v12_yzstar_norm = ((vecMod(v12touch_yzstar) < 0.0001f ) ? glm::vec3(0) : glm::normalize(v12touch_yzstar)), v21_yzstar_norm = -v12_yzstar_norm;
	// --(3)--
	glm::vec3 delta_L1(0), delta_L2(0);

	glm::vec3 delta_L1WC = I_yzstar_abs == 0 ? glm::vec3(0) : glm::cross(r1, I_yzstar_abs * v21_yzstar_norm);
	delta_L1 == glm::vec3(glm::inverse(sphere->GetModelMatrix()) * glm::vec4(delta_L1WC, 0.0f));
	// #INNORMAL when rotate on the desk, it's too small

	glm::vec3 delta_L2WC = glm::cross(r2, I_yzstar_abs * v12_yzstar_norm + I12_xstar);
	delta_L2 == glm::vec3(glm::inverse(cube->GetModelMatrix()) * glm::vec4(delta_L2WC, 0.0f));

	cInfo.angularImpulse1 = delta_L1;
	cInfo.angularImpulse2 = delta_L2;
	// #NOTE - for cube we cannot directly calculate the delta omega since the inertia moment is not static
	//		 - for sphere we can, but we don't



	glm::vec3 delta_v1(0), delta_v2(0);
	if (fsType == CollisionFreeStuckType::BothFree)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
	}


	v1_after += delta_v1;
	v2_after += delta_v2;


	cInfo.v1After = v1_after;
	cInfo.v2After = v2_after;

	if (autoDeal)
	{
		DealCollision(sphere, cube, cInfo);
	}

	return cInfo;

}

void CollideSph2Cube(std::vector<Object3Dsphere*> &spheres, std::vector<Object3Dcube*> &cubes, bool autoDeal)
{
	std::vector<Object3Dsphere*>::iterator sph_it = spheres.begin();
	std::vector<Object3Dcube*>::iterator cube_it = cubes.begin();

	for (/*sph_it = spheres.begin()*/; sph_it < spheres.end(); sph_it++)
	{
		for (cube_it = cubes.begin(); cube_it < cubes.end(); cube_it++)
			CollideSph2Cube(*sph_it, *cube_it, true);
	}

}

void DealCollision(Object3D * obj1, Object3D * obj2, CollisionInfo cInfo)
{
	if (cInfo.relation == RelationType::Ambiguous)		// the most beautiful one
	{
		obj1->SetVelocity(cInfo.v1After);
		obj2->SetVelocity(cInfo.v2After);

		obj1->AddAngularMomentum(cInfo.angularImpulse1);
		obj2->AddAngularMomentum(cInfo.angularImpulse2);
	}
}
