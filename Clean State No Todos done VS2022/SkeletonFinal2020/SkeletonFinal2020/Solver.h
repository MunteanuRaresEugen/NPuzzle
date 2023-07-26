#pragma once

#include "State.h"
#include "io.h"

#include <iostream>
#include <set>
#include <queue>
#include<unordered_set>
#include<math.h>

template <class State_t>
struct Heuristics
{
	static size_t GetManhattanDistance(const State_t& state)
	{
		size_t totaldistance = 0;
		const auto& curentData = state.GetData();
		for (size_t i = 0; i < curentData.size(); i++)
		{
			if (curentData[i] != 0)
			{
				size_t currentdistance = std::abs((int)state.GetPosition2D(i).second - (int)state.GetPosition2D(curentData[i] - 1).second) + std::abs((int)state.GetPosition2D(i).first - (int)state.GetPosition2D(curentData[i] - 1).first);
				totaldistance += currentdistance;

			}

		}

		return totaldistance;
	}
};

class Solver
{
public:
	template <class State_t>
	static Moves SolveBFS(const State_t& initialState)
	{
		std::cout << "Solving..." << std::endl << initialState;

		Validate(initialState);
		if (initialState.IsGoalState()) return {}; // no moves required. Initial state is goal state.

		using Node = std::pair<State_t, Moves>;

		//TODO: define OPEN SET correctly
		//std::queue<Node> openSet;
		/*auto mDistance = [](const Node& node) -> int
		{
			int totaldistance = 0;
			const auto& currentstate = node.first;
			const auto& curentData = currentstate.GetData();
			for (size_t i = 0; i < currentstate.GetData().size(); i++)
			{	
				if (curentData[i] != 0)
				{
					int currentdistance = std::abs((int)currentstate.GetPosition2D(i).second - (int)currentstate.GetPosition2D(curentData[i] - 1).second) + std::abs((int)currentstate.GetPosition2D(i).first - (int)currentstate.GetPosition2D(curentData[i] - 1).first);
					totaldistance += currentdistance;
					
				}
				
			}
			totaldistance += node.second.size();

			return totaldistance;
		};*/
		
		auto nodeCompare = [&](const Node& first, const Node& second) -> bool
		{
			auto firstHeuristic = Heuristics<State_t>::GetManhattanDistance(first.first) + first.second.size();
			auto secondHeuristic = Heuristics<State_t>::GetManhattanDistance(second.first) + second.second.size();
			return firstHeuristic > secondHeuristic;
		};
		std::priority_queue<Node,std::vector<Node>,decltype(nodeCompare)> openSet(nodeCompare);
		openSet.push({ initialState, {} });

		//TODO: define CLOSED SET correctly
		//std::set<State_t> closedSet;

		auto stateCompare = [](const State_t& first, const State_t& second) -> bool
		{
			return first.GetData() < second.GetData();
		};

		

 

		//auto stateHash = [](const State_t& state) -> size_t
		//{
	//		const auto &reference = state.GetData();
	//		size_t seed = reference.size();
	//		for (auto& i : reference) {
	//			seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	//		}
	//		return seed;
		//};

		/*auto equalTo = [](const State_t& first, const State_t& second) -> bool
		{
			return first.GetData() == second.GetData();
		};*/

		//std::set<State_t, decltype(stateCompare)> closedSet(stateCompare);
		std::unordered_set<State_t> closedSet;


		// TODO: Create a comparator so std::set can work with the State instances.
		// It doesn't really make sense to compare states otherwise...


		while (!openSet.empty())
		{
			// TODO: Do it nicer , who is first? second? structure binding
			auto currentNode = openSet.top();
			auto&& [currentState, currentMoves] = currentNode;
			openSet.pop();

			// some logging
			static size_t maxDepth = 0;
			if (currentMoves.size() > maxDepth)
			{
				maxDepth = currentMoves.size();
				std::cout << "Max Depth: " << maxDepth << std::endl;
			}
			// end logging

			if (currentState.IsGoalState())
			{
				std::cout << "Visited: " << closedSet.size() << std::endl;
				return currentMoves; // leaving cycle statement
			}

			closedSet.insert(currentState);

			for (auto&& childMovePair : currentState.GetChildren())
			{
				//TODO: Do it nicer - who is first? second? 
				auto&& [childState, move] = childMovePair;

				if (!closedSet.contains(childState)) // TODO: use C++20
				{
					Moves childMoves = currentMoves;
					childMoves.push_back(move);
					openSet.push({ std::move(childState), std::move(childMoves) });
				}
			}
		}

		throw std::runtime_error("Couldn't solve");
	}

private:
	//musthaveTODO implement Validate correctly
	template <class State_t>
	static void Validate(const State_t& state)
	{
		if (!state.IsValid())
		{
			throw std::runtime_error("state not valid");
		}
		if (!state.IsSolvable())
		{
			throw std::runtime_error("state not solvable");
		}
	}
};
