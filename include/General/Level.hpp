/* 
 * File:   Level.hpp
 * Author: kolzi
 *
 * Created on 24 kwiecień 2013, 22:13
 */

#ifndef LEVEL_HPP
#define	LEVEL_HPP

#include <unordered_map>
#include <list>
#include <SFML/Graphics/RenderWindow.hpp>

#include "TechnicalShit/PairIntStringHash.hpp"
#include "SimpleIDGenerator.hpp"

class Component;
class System;
class ArchetypesManager;

class Level
{
public:
	Level(sf::RenderWindow& app, ArchetypesManager& archetypesManager);
	Level(const Level& orig);
	virtual ~Level();
	typedef std::pair<int, std::string> CompKey;
	typedef std::unordered_map <CompKey, Component*, PairIntStringHash> CompMap;
	typedef std::unordered_map <std::string, System*> SysMap;
	void read(std::istream& str);
	void update(sf::Time deltaTime);
private:
	sf::RenderWindow& app;
	ArchetypesManager& archetypesManager;
	IDGenerator* idGenerator;
	std::list <System*> systems;
	std::unordered_map <std::string, System*> systemsMap;
	CompMap components;
	
	

};

#endif	/* LEVEL_HPP */
