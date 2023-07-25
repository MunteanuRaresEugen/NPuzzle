#pragma once

#include "types.h"
#include <array>
#include <optional>
#include <map>
#include <numeric>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include<set>
#include<unordered_map>

template<size_t N = 3>
class State
{
public: // Types
    using Position2D = std::pair<size_t, size_t>;
    using ElementType = uint8_t;
	using Data = std::array<ElementType,N*N>; // TODO checked

private: // members
    Data m_data;

public:
    static const size_t Dimension = N;    

    State() = delete; // TODO checked
    State(Data data) : m_data{ std::move(data) } {};

    const Data& GetData() const
    {
        return m_data;
    }

    static const State GoalState()
    {
        // TODO: Refactor with STL --- checked        
        Data goalData;         
        /*for (auto idx = 0u; idx < goalData.size(); ++idx)
        {
            goalData[idx] = idx+1;
        }*/
        std::iota(goalData.begin(), goalData.end(), 1);
        goalData.back() = 0;        
        return State(goalData);
    }

    bool IsGoalState() const
    {
        return m_data == GoalState().m_data;
    }

    bool IsValid() const
    {
        // TODO refactor with STL(the 3 fors)
		// no duplicates
		// pieces 1 - 8 
		// empty space 0 present

   //     Data sortedData;
   //     
   //    /* for (auto idx = 0u; idx < m_data.size(); ++idx)
   //     {
   //         sortedData[idx] = m_data[idx];
   //     }*/

   //     std::copy(std::begin(m_data), std::end(m_data), std::begin(sortedData));

   //     /*for (auto idxI = 0u; idxI < sortedData.size() - 1 ; ++idxI)
   //     {
			//for (auto idxJ = 0u; idxJ < sortedData.size() - idxI - 1 ; ++idxJ)
			//{
			//	if (sortedData[idxJ] >  sortedData[idxJ + 1])
			//	{
			//		auto temp = sortedData[idxJ];
			//		sortedData[idxJ] = sortedData[idxJ + 1];
			//		sortedData[idxJ + 1] = temp;
			//	}
			//}
   //     } */

   //     //std::sort(sortedData.begin(), sortedData.end());

   //     Data validSortedData;
   //     
   //    /* for (auto idx = 0u; idx < validSortedData.size(); ++idx)
   //     {
   //         validSortedData[idx] = idx;
   //     }*/

   //     Data validSortedData(validSortedData.size());
   //     std::iota(validSortedData.begin(), validSortedData.end(), 0);

   //     return sortedData == validSortedData;

        std::set<ElementType> sortedData(m_data.begin(), m_data.end());

        if (sortedData.size() != m_data.size())
            return false;

        if (*sortedData.begin() != 0 || *sortedData.rbegin() != m_data.size() - 1)
            return false;

        return true; //copy sort iota si comparare alta varianta

        //return std::is_permutation(m_data.begin(), m_begin.end(), GoalState().GetData().begin()); one line

    }

   size_t CountInversions(typename Data::const_iterator begin, typename Data::const_iterator end) const
    {
        size_t acc{ 0u };
        for (auto it = begin; it != end; ++it)
        {
            auto&& current = *it;
            if (current != 0)
                acc += std::count_if(it, end, [current](auto next) { return next != 0 && next < current; });
        }

        return acc;
    };

    bool IsSolvable() const
    {
		// TODO too big lambda
       /* auto countInversions = [](auto begin, auto end)
        {
            size_t acc{ 0u };
            for (auto it = begin; it != end; ++it)
            {
                auto&& current = *it;
                if (current != 0)
                    acc += std::count_if(it, end, [current](auto next) { return next != 0 && next < current; });
            }

            return acc;
        };*/

        const auto inversionsCount = CountInversions(m_data.begin(), m_data.end());
        const auto isInversionCountEven = inversionsCount % 2 == 0;
        const bool isNOdd = N % 2 == 1;
        const bool isBlankRowEven = GetBlankPosition2D().first % 2 == 0;

        return (isNOdd) ? isInversionCountEven :
            (isBlankRowEven) ? !isInversionCountEven :
            isInversionCountEven;
    }

    std::vector<std::pair<State, MoveDirection>> GetChildren() const
    {
        //TODO: Refactor this method + See Move method and refactor the Move method
        /*auto child1 = MoveLeft();
        auto child2 = MoveRight();
        auto child3 = MoveUp();
        auto child4 = MoveDown();

        std::vector<std::pair<State, MoveDirection>> children;
        if (child1) children.emplace_back(*child1, MoveDirection::LEFT);
        if (child2) children.emplace_back(*child2, MoveDirection::RIGHT);
        if (child3) children.emplace_back(*child3, MoveDirection::UP);
        if (child4) children.emplace_back(*child4, MoveDirection::DOWN);*/
        
        std::vector<std::pair<State, MoveDirection>> children;

        Moves possibleMoves = { MoveDirection::LEFT , MoveDirection::RIGHT , MoveDirection::UP , MoveDirection::DOWN };

        for (auto dir: possibleMoves)
        {
            auto child = Move(dir);
            if (child.has_value())
                children.emplace_back(*child, dir);
        }

        return children;
    }

private: // methods

    size_t GetBlankPosition() const
    {
        // TODO refactor using STL algo
        /*for (auto idx = 0u; idx < m_data.size(); ++idx)
        {
            if (m_data[idx] == 0)
                return idx;
        }*/
        const auto& it = std::find(m_data.begin(), m_data.end(), 0);

        if (it != m_data.end())
            //return it - m_data.begin();
            return std::distance(m_data.begin(),it);

        throw std::runtime_error("Unexpected");
    }

    Position2D GetBlankPosition2D() const
    {
        auto&& absolute = GetBlankPosition();
        return { absolute / N, absolute % N }; // structure binding instead of std::make_pair<>
    }

    // TODO: Perform the move if possible and return the state. Returns std::nullopt otherwise.
    std::optional<State> Move(MoveDirection direction) const
    {
        /*switch (direction)
        {
        case MoveDirection::LEFT:   return MoveLeft();
        case MoveDirection::UP:     return MoveUp();
        case MoveDirection::RIGHT:  return MoveRight();
        case MoveDirection::DOWN:   return MoveDown();
        default:                    throw std::runtime_error("Not implemented.");
        }*/

        static std::unordered_map< MoveDirection, std::function<std::optional<State>(const State&)> > mappingEnumToFct =
        {
            {MoveDirection::LEFT,   std::mem_fn(&State::MoveLeft)},
            {MoveDirection::RIGHT,  std::mem_fn(&State::MoveRight)},
            {MoveDirection::UP,     std::mem_fn(&State::MoveUp)},
            {MoveDirection::DOWN,   std::mem_fn(&State::MoveDown)}
        };

        return mappingEnumToFct.at(direction)(*this);
    
    }

    static State SwapTiles(const State& state, size_t firstPos, size_t secondPos)
    {
        auto childData = state.GetData();
        std::swap(childData[firstPos], childData[secondPos]);
        return { std::move(childData) };
    }

    std::optional<State> MoveRight() const
    {
        if (GetBlankPosition2D().second == 0) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition - 1);
    }

    std::optional<State> MoveLeft() const
    {
        if (GetBlankPosition2D().second == N - 1) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition + 1);
    }

    std::optional<State> MoveDown() const
    {
        if (GetBlankPosition2D().first == 0) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition - N);
    }

    std::optional<State> MoveUp() const
    {
        if (GetBlankPosition2D().first == N - 1) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition + N);
    }

};

std::ostream& operator<< (std::ostream& os, MoveDirection dir)
{
    static const std::map<MoveDirection, const char*> namesMap
    {
        { MoveDirection::LEFT, "l" },
        { MoveDirection::RIGHT, "r" },
        { MoveDirection::UP, "u" },
        { MoveDirection::DOWN, "d" },
    };

    os << namesMap.at(dir);
    return os;
}

template <size_t N>
std::ostream& operator<< (std::ostream& os, const State<N>& state)
{
    os << std::endl;
    for (size_t index = 0; index < state.GetData().size(); ++index)
    {
        os << static_cast<unsigned int>(state.GetData()[index]) << ' ';

        if (index % State<>::Dimension == State<>::Dimension - 1) os << std::endl;
    }
    os << std::endl;

    return os;
}

using State3X3 = State<3>;
using State4X4 = State<4>;
