////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SplitIndexRange.cpp (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "loopnests/SplitIndexRange.h"

#include <utilities/include/Exception.h>

#include <ostream>
#include <queue>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        SplitIndexRange::SplitIndexRange(const IndexRange& indexRange)
        {
            auto index = indexRange.GetIndex();
            auto range = indexRange.GetRange();

            // Add the dimension index as the root of the index tree
            _indices.push_back(index);
            _ranges.push_back(range);
            _parentOffset.push_back(-1); // 'null' sentinel
            _leftChildOffset.push_back(-1); // 'null' sentinel
            _indexOffset[index] = 0;
        }

        const Index& SplitIndexRange::GetDimensionIndex() const
        {
            return _indices[0];
        }

        int SplitIndexRange::NumSplits() const
        {
            auto result = static_cast<int>(_indices.size() + 1) / 2;
            return result;
        }

        int SplitIndexRange::GetBegin() const
        {
            return _ranges[0].Begin();
        }

        int SplitIndexRange::GetSize() const
        {
            return _ranges[0].Size();
        }

        int SplitIndexRange::GetIncrement() const
        {
            return _ranges[0].Increment();
        }

        bool SplitIndexRange::Contains(const Index& index) const
        {
            if (IsDimension(index))
            {
                return true;
            }
            return _indexOffset.count(index) != 0;
        }

        bool SplitIndexRange::IsLoopIndex(const Index& index) const
        {
            auto node = GetNode(index);
            return IsLeaf(node);
        }

        bool SplitIndexRange::IsComputedIndex(const Index& index) const
        {
            auto node = GetNode(index);
            return IsInteriorNode(node);
        }

        bool SplitIndexRange::IsDimension(const Index& index) const
        {
            return index == GetDimensionIndex();
        }

        bool SplitIndexRange::IsChildOf(const Index& child, const Index& parent) const
        {
            auto childOffset = GetNode(child);
            auto parentOffset = GetNode(parent);
            return _parentOffset.at(childOffset) != -1 && _parentOffset.at(childOffset) == parentOffset;
        }

        bool SplitIndexRange::IsParentOf(const Index& parent, const Index& child) const
        {
            return IsChildOf(parent, child);
        }

        // index1 depends on index2? e.g., is index2 in the list of dependent indices?
        bool SplitIndexRange::DependsOn(const Index& index1, const Index& index2) const
        {
            // TODO: assert index1 and index2 are both in this dimension

            // The top-level dimension index depends on everything
            if (IsDimension(index1))
            {
                return true;
            }

            // nothing else depends on the top-level dimension index
            if (IsDimension(index2))
            {
                return false;
            }

            auto node1 = GetNode(index1);
            auto node2 = GetNode(index2);
            if (node1 == node2)
            {
                return false;
            }

            while (node2 > 0)
            {
                auto parentNode = GetParent(node2);
                if (parentNode == node1)
                {
                    return true;
                }
                node2 = parentNode;
            }
            return false;
        }

        int SplitIndexRange::GetSplitSize(int level) const
        {
            if (level > NumSplits())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "GetSplitSize() --- index out of range");
            }
            if (level == NumSplits())
            {
                return 1;
            }
            auto node = GetNthLeaf(level);
            return _ranges[node].Size();
        }

        Range SplitIndexRange::GetDimensionRange() const
        {
            return _ranges[0];
        }

        Range SplitIndexRange::GetIndexRange(const Index& index) const
        {
            auto node = GetNode(index);
            return _ranges[node];
        }

        Index SplitIndexRange::GetSplitIndex(int level) const
        {
            auto node = GetNthLeaf(level);
            return _indices[node];
        }

        SplitIndex SplitIndexRange::Split(int size)
        {
            if (size > _ranges.back().Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Split size larger than smallest existing split");
            }

            auto lastLeaf = GetSmallestLeaf(0); // arbitrarily choose the last leaf --- it will be the right child of the right child of the right child... of the root (the `0` argument)
            return SplitNode(lastLeaf, size);
        }

        SplitIndex SplitIndexRange::Split(Index index, int size)
        {
            if (IsDimension(index))
            {
                index = _indices[0];
            }
            auto node = GetNode(index);
            return SplitNode(node, size);
        }

        int SplitIndexRange::GetNode(const Index& index) const
        {
            return _indexOffset.at(index);
        }

        SplitIndex SplitIndexRange::SplitNode(int node, int size)
        {
            auto prefix = GetDimensionIndex().GetName() + "_";
            auto startIndex = static_cast<int>(_indices.size());
            Index outer = { prefix + std::to_string(startIndex) };
            Index inner = { prefix + std::to_string(startIndex + 1) };

            // `SplitNode(n, size)` splits a leaf of `n` --- the bottom-rightmost leaf. If `n` is a leaf already, split it.
            auto parentOffset = GetSmallestLeaf(node);
            auto parent = _indices[parentOffset];
            auto parentSize = _ranges[parentOffset].Size();
            auto parentIncrement = _ranges[parentOffset].Increment();
            auto leftChildOffset = static_cast<int>(_indices.size());

            // Add outer index to data structure
            auto offset = static_cast<int>(_indices.size());
            _indices.push_back(outer);
            _ranges.push_back({ 0, parentSize, size });
            _parentOffset.push_back(parentOffset);
            _leftChildOffset.push_back(-1);
            _indexOffset[outer] = offset;

            // Add inner index to datastructure
            offset = static_cast<int>(_indices.size());
            _indices.push_back(inner);
            auto thisSize = std::min(parentSize, size); // In case split is larger than original range
            _ranges.push_back({ 0, thisSize, parentIncrement });
            _parentOffset.push_back(parentOffset);
            _leftChildOffset.push_back(-1);
            _leftChildOffset[parentOffset] = leftChildOffset;
            _indexOffset[inner] = offset;

            return SplitIndex{ outer, inner };
        }

        //
        // Binary-tree implementation below:
        //
        int SplitIndexRange::GetParent(int node) const
        {
            return _parentOffset.at(node);
        }

        int SplitIndexRange::GetLeftChild(int node) const
        {
            return _leftChildOffset.at(node);
        }

        int SplitIndexRange::GetRightChild(int node) const
        {
            auto leftChild = _leftChildOffset.at(node);
            return leftChild == -1 ? leftChild : leftChild + 1;
        }

        int SplitIndexRange::GetNthLeaf(int n) const
        {
            for (int i = 0; i < static_cast<int>(_leftChildOffset.size()); ++i)
            {
                auto node = _leftChildOffset[i];
                if (node == -1) // leaves have no children
                {
                    if (n == 0)
                    {
                        return i;
                    }
                    --n;
                }
            }

            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Couldn't find node");
        }

        // returns the "smallest" leaf descendent of index. If index is itself a leaf, return it, else return GetSmallestLeaf(index.rightChild)
        int SplitIndexRange::GetSmallestLeaf(int node) const
        {
            if (IsLeaf(node))
            {
                return node;
            }
            return GetSmallestLeaf(GetRightChild(node));
        }

        bool SplitIndexRange::IsLeaf(int node) const
        {
            auto isLeaf = _leftChildOffset.at(node) == -1;
            if (isLeaf && (_parentOffset.at(node) == -1 && node != 0))
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
            }

            return isLeaf;
        }

        bool SplitIndexRange::IsInteriorNode(int node) const
        {
            return !IsLeaf(node);
        }

        const std::vector<Index>& SplitIndexRange::GetIndices() const
        {
            return _indices;
        }

        std::vector<Index> SplitIndexRange::GetLoopIndices() const
        {
            std::vector<Index> result;
            for (int n = 0; n < static_cast<int>(_indices.size()); ++n)
            {
                if (IsLeaf(n))
                {
                    result.push_back(_indices[n]);
                }
            }
            return result;
        }

        std::vector<Index> SplitIndexRange::GetComputedIndices() const
        {
            std::vector<Index> result;
            for (int n = 0; n < static_cast<int>(_indices.size()); ++n)
            {
                if (IsInteriorNode(n))
                {
                    result.push_back(_indices[n]);
                }
            }
            return result;
        }

        std::vector<Index> SplitIndexRange::GetDependentIndices(const Index& index, bool includeSelf) const
        {
            std::vector<Index> result;
            if (includeSelf)
            {
                result.push_back(index);
            }

            std::queue<int> nodesToVisit;
            auto node = GetNode(index);
            nodesToVisit.push(node);
            // get all children, children of children, etc.
            while (!nodesToVisit.empty())
            {
                auto n = nodesToVisit.front();
                nodesToVisit.pop();
                result.push_back(_indices[n]);
                auto leftChild = GetLeftChild(n);
                if (leftChild != -1)
                {
                    nodesToVisit.push(leftChild);
                    nodesToVisit.push(leftChild + 1);
                }
            }
            return result;
        }

        std::vector<Index> SplitIndexRange::GetDependentLoopIndices(const Index& index, bool includeSelf) const
        {

            std::queue<int> nodesToVisit;
            auto node = IsDimension(index) ? 0 : GetNode(index);
            if (includeSelf && IsLeaf(node))
            {
                // If we're a leaf, no need to check any further
                return { index };
            }

            std::vector<Index> result;
            nodesToVisit.push(node);
            // get all children, children of children, etc.
            while (!nodesToVisit.empty())
            {
                auto n = nodesToVisit.front();
                nodesToVisit.pop();

                for (auto child : { GetLeftChild(n), GetRightChild(n) })
                {
                    if (child != -1)
                    {
                        if (IsLeaf(child))
                        {
                            result.push_back(_indices[child]);
                        }
                        else
                        {
                            nodesToVisit.push(child);
                        }
                    }
                }
            }
            return result;
        }

        bool SplitIndexRange::HasParentIndex(const Index& index) const
        {
            auto node = GetNode(index);
            return node != -1 && node != 0;
        }

        Index SplitIndexRange::GetParentIndex(const Index& index) const
        {
            auto node = GetNode(index);
            if (node == -1 || node == 0)
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "GetParentIndex() --- dimension index has no parent");

            return _indices[GetParent(node)];
        }

        bool SplitIndexRange::IsOuterSplitIndex(const Index& index) const
        {
            auto node = GetNode(index);
            if (node == -1 || node == 0)
            {
                return false;
            }

            auto parentNode = GetParent(node);
            return node == GetLeftChild(parentNode);
        }

        bool SplitIndexRange::IsInnerSplitIndex(const Index& index) const
        {
            auto node = GetNode(index);
            if (node == -1 || node == 0)
            {
                return false;
            }

            auto parentNode = GetParent(node);
            return node == GetRightChild(parentNode);
        }

        Index SplitIndexRange::GetOuterSplitIndex(const Index& parent) const
        {
            auto parentNode = GetNode(parent);
            if (IsLeaf(parentNode))
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "GetOuterSplitIndex() --- called on a non-split index");

            return _indices[GetLeftChild(parentNode)];
        }

        Index SplitIndexRange::GetInnerSplitIndex(const Index& parent) const
        {
            auto parentNode = GetNode(parent);
            if (IsLeaf(parentNode))
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "GetInnerSplitIndex() --- called on a non-split index");

            return _indices[GetRightChild(parentNode)];
        }

        std::vector<Index> SplitIndexRange::GetAllParentIndices(const Index& index) const
        {
            std::vector<Index> result{ GetDimensionIndex() };
            auto node = GetNode(index);
            if (node == -1)
                return result;

            for (;;)
            {
                node = GetParent(node);
                if (node == -1)
                {
                    break;
                }
                result.push_back(_indices[node]);
            }
            return result;
        }

        std::vector<Index> SplitIndexRange::GetChildIndices(const Index& index) const
        {
            if (IsDimension(index))
            {
                return { _indices[0] };
            }

            auto node = GetNode(index);
            if (node == -1)
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
            }

            return { _indices[GetLeftChild(node)], _indices[GetRightChild(node)] };
        }

        void SplitIndexRange::Print(std::ostream& os) const
        {
            os << "Dimension " << GetDimensionIndex() << " range: " << GetDimensionRange() << std::endl;

            os << "  Loop variables:\t";
            for (auto i : GetLoopIndices())
            {
                auto r = GetIndexRange(i);
                os << i << ": " << r << ";\t";
            }
            os << std::endl;

            os << "  Comp variables:\t";
            for (auto i : GetComputedIndices())
            {
                auto r = GetIndexRange(i);
                os << i << ": " << r << " (";
                std::string sep = "";
                for (auto dep : GetChildIndices(i))
                {
                    os << sep << dep;
                    sep = ", ";
                }
                os << ");\t";
            }
            os << std::endl;
        }
    } // namespace loopnests
} // namespace value
} // namespace ell
