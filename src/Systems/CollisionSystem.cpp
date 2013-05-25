/* 
 * File:   CollisionSystem.cpp
 * Author: marzena
 * 
 * Created on 20 luty 2013, 00:06
 */

#include "Systems/CollisionSystem.hpp"
#include "Components/BoundingBoxComponent.hpp"
#include "Components/StandsOnComponent.hpp"
#include "Components/StandableComponent.hpp"

#include <iostream>
#include <algorithm>
#include <assert.h>
#include <boost/cast.hpp>

CollisionSystem::CollisionSystem(Level::CompMap& components)
:System(components)
{
}

CollisionSystem::CollisionSystem(const CollisionSystem& orig)
:System(orig)
{
}

CollisionSystem::~CollisionSystem()
{
}

void CollisionSystem::addEntity(int EID)
{
	assert( components.find(Level::CompKey(EID, "Position")) != components.end() &&
			components.find(Level::CompKey(EID, "BoundingBox")) != components.end() );
    entities.push_back(EID);
}

void CollisionSystem::update(sf::Time deltaTime)
{
	for(auto it=entities.begin();it!=entities.end();it++)
	{
		if(components.find(Level::CompKey(*it, "StandsOn")) != components.end())
			boost::polymorphic_downcast<StandsOnComponent*>(components.at(Level::CompKey(*it, "StandsOn")))->standing=false;
	}
	for(auto it=entities.begin();it!=entities.end();it++)
	{
		PositionComponent& posCi=*boost::polymorphic_downcast<PositionComponent*>(components.at(Level::CompKey(*it, "Position")));
		PhysicsComponent empty(*it, 0, 0, 0);
		PhysicsComponent& physCi=
		(components.find(Level::CompKey(*it, "Physics")) != components.end() ?
			*boost::polymorphic_downcast<PhysicsComponent*>(components.at(Level::CompKey(*it, "Physics"))) : empty);

		BoundingBoxComponent& bbCi=*boost::polymorphic_downcast<BoundingBoxComponent*>(components.at(Level::CompKey(*it, "BoundingBox")));
		
		bool iCanStand = components.find(Level::CompKey(*it, "StandsOn")) != components.end();
		bool iStandable = components.find(Level::CompKey(*it, "Standable")) != components.end();
		
		auto jt=it;
		jt++;
		for(jt;jt!=entities.end();jt++)
		{
			PositionComponent& posCj=*boost::polymorphic_downcast<PositionComponent*>(components.at(Level::CompKey(*jt, "Position")));
			PhysicsComponent& physCj=
			(components.find(Level::CompKey(*jt, "Physics")) != components.end() ?
				*boost::polymorphic_downcast<PhysicsComponent*>(components.at(Level::CompKey(*jt, "Physics"))) : empty);
			
			BoundingBoxComponent& bbCj=*boost::polymorphic_downcast<BoundingBoxComponent*>(components.at(Level::CompKey(*jt, "BoundingBox")));
			
			bool jCanStand = components.find(Level::CompKey(*jt, "StandsOn")) != components.end();
			bool jStandable = components.find(Level::CompKey(*jt, "Standable")) != components.end();
			
			
			sf::Rect<double> bbi=bbCi.boundingBox;
			sf::Rect<double> bbj=bbCj.boundingBox;
			
            bbi.left+=posCi.x;
            bbi.top +=posCi.y;
            bbj.left+=posCj.x;
            bbj.top +=posCj.y;
			
			if(bbi.intersects(bbj))
			{
				double tx=999999;
				double ty=999999;
				double relVx=physCi.vx-physCj.vx;
				if(relVx>0)
					tx=(bbi.left+bbi.width-bbj.left)/relVx;
				if(relVx<0)
					tx=(bbi.left-bbj.left-bbj.width)/relVx;
				double relVy=physCi.vy - physCj.vy;
				if(relVy>0)
					ty=(bbi.top+bbi.height-bbj.top)/relVy;
				if(relVy<0)
					ty=(bbi.top-bbj.top-bbj.height)/relVy;
				double t=std::min(tx,ty);
				
				const double eps=0.00001;
				
				if(t==tx && t<=deltaTime.asSeconds()+eps)
				{
					posCi.x-=(t*physCi.vx);
					posCj.x-=(t*physCj.vx);
					physCi.vx=0;
					physCi.ax=0;
					physCj.ax=0;
					physCj.vx=0;
				}
				else if(t==ty && t<=deltaTime.asSeconds()+eps)
				{
					bool iOnj=posCi.y-t*physCi.vy < posCj.y-t*physCj.vy && iCanStand && jStandable;
					bool jOni=posCi.y-t*physCi.vy > posCj.y-t*physCj.vy && jCanStand && iStandable;
					//i stands on j or j stands on i
					if( iOnj || jOni )
					{
						posCi.y-=(t*physCi.vy);
						posCj.y-=(t*physCj.vy);
						physCi.ay=0;
						physCi.vy=0;
						physCj.vy=0;
						physCj.ay=0;
						if(iOnj)
						{
							StandsOnComponent& standsOn=*boost::polymorphic_downcast<StandsOnComponent*>(components.at(Level::CompKey(*it, "StandsOn")));
							StandableComponent& standable=*boost::polymorphic_downcast<StandableComponent*>(components.at(Level::CompKey(*jt, "Standable")));
							standsOn.jumpingTimeLeft=standsOn.baseJumpingTime*standable.jumpingTimeMultiplier;
							standsOn.standing=true;
							standsOn.standsOn=*jt;
						}
						else
						{
							StandsOnComponent& standsOn=*boost::polymorphic_downcast<StandsOnComponent*>(components.at(Level::CompKey(*jt, "StandsOn")));
							StandableComponent& standable=*boost::polymorphic_downcast<StandableComponent*>(components.at(Level::CompKey(*it, "Standable")));
							standsOn.jumpingTimeLeft=standsOn.baseJumpingTime*standable.jumpingTimeMultiplier;
							standsOn.standing=true;
							standsOn.standsOn=*it;
						}
					}
				}
			}
		}
	}
}