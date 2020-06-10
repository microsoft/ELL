////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     KernelPredicate.h (value)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Index.h"
#include "LoopIndexInfo.h"

#include "../ValueType.h"

#include <iosfwd>
#include <memory>
#include <optional>
#include <string>
#include <variant>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        class KernelPredicate;
        class LoopVisitSchedule;

        enum class Fragment
        {
            first,
            last,
            endBoundary,
            all
        };

        enum class Placement
        {
            before,
            after
        };

        class EmptyPredicate
        {
        public:
            const EmptyPredicate& Simplify() const { return *this; }
            const EmptyPredicate& Simplify(const LoopIndexSymbolTable& indices, const LoopVisitSchedule& schedule) const { return *this; }

        private:
            friend inline bool operator==(const EmptyPredicate& i1, const EmptyPredicate& i2) { return true; }
            friend inline bool operator!=(const EmptyPredicate& i1, const EmptyPredicate& i2) { return false; }
            friend inline bool operator<(const EmptyPredicate& i1, const EmptyPredicate& i2) { return false; }
        };

        class ConstantPredicate
        {
        public:
            explicit ConstantPredicate(bool value);

            bool GetValue() const;

            const ConstantPredicate& Simplify() const { return *this; }
            const ConstantPredicate& Simplify(const LoopIndexSymbolTable& indices, const LoopVisitSchedule& schedule) const { return *this; }

        private:
            bool _value;

            friend inline bool operator==(const ConstantPredicate& i1, const ConstantPredicate& i2) { return i1.GetValue() == i2.GetValue(); }
            friend inline bool operator!=(const ConstantPredicate& i1, const ConstantPredicate& i2) { return i1.GetValue() != i2.GetValue(); }
            friend inline bool operator<(const ConstantPredicate& i1, const ConstantPredicate& i2) { return i1.GetValue() < i2.GetValue(); }
        };

        class FragmentTypePredicate
        {
        public:
            FragmentTypePredicate(const Index& index, Fragment condition);

            const Index& GetIndex() const;
            Fragment GetCondition() const;

            KernelPredicate Simplify() const;
            KernelPredicate Simplify(const LoopIndexSymbolTable& indices, const LoopVisitSchedule& schedule) const;

        private:
            Index _index;
            Fragment _condition;

            friend inline bool operator==(const FragmentTypePredicate& i1, const FragmentTypePredicate& i2) { return (i1.GetIndex() == i2.GetIndex()) && (i1.GetCondition() == i2.GetCondition()); }
            friend inline bool operator!=(const FragmentTypePredicate& i1, const FragmentTypePredicate& i2) { return (i1.GetIndex() != i2.GetIndex()) || (i1.GetCondition() != i2.GetCondition()); }
            friend inline bool operator<(const FragmentTypePredicate& i1, const FragmentTypePredicate& i2) { return (i1.GetIndex() != i2.GetIndex()) ? (i1.GetIndex() < i2.GetIndex()) : (i1.GetCondition() < i2.GetCondition()); }
        };

        class PlacementPredicate
        {
        public:
            // Where to schedule kernel in its loop (before or after any inner loops)
            explicit PlacementPredicate(Placement where);

            // Where to schedule kernel in relation to an index
            PlacementPredicate(const Index& index, Placement where);

            bool HasIndex() const;
            Index GetIndex() const;
            Placement GetPlacement() const;

            const PlacementPredicate& Simplify() const;
            const PlacementPredicate& Simplify(const LoopIndexSymbolTable& indices, const LoopVisitSchedule& schedule) const;

        private:
            std::optional<Index> _index;
            Placement _placement;

            friend inline bool operator==(const PlacementPredicate& i1, const PlacementPredicate& i2) { return (i1.GetIndex() == i2.GetIndex()) && (i1.GetPlacement() == i2.GetPlacement()); }
            friend inline bool operator!=(const PlacementPredicate& i1, const PlacementPredicate& i2) { return (i1.GetIndex() != i2.GetIndex()) || (i1.GetPlacement() != i2.GetPlacement()); }
            friend inline bool operator<(const PlacementPredicate& i1, const PlacementPredicate& i2) { return (i1.GetIndex() != i2.GetIndex()) ? (i1.GetIndex() < i2.GetIndex()) : (i1.GetPlacement() < i2.GetPlacement()); }
        };

        class IndexDefinedPredicate
        {
        public:
            explicit IndexDefinedPredicate(const Index& index);

            const Index& GetIndex() const;

            const IndexDefinedPredicate& Simplify() const;
            const IndexDefinedPredicate& Simplify(const LoopIndexSymbolTable& indices, const LoopVisitSchedule& schedule) const;

        private:
            Index _index;

            friend inline bool operator==(const IndexDefinedPredicate& i1, const IndexDefinedPredicate& i2) { return i1.GetIndex() == i2.GetIndex(); }
            friend inline bool operator!=(const IndexDefinedPredicate& i1, const IndexDefinedPredicate& i2) { return i1.GetIndex() != i2.GetIndex(); }
            friend inline bool operator<(const IndexDefinedPredicate& i1, const IndexDefinedPredicate& i2) { return i1.GetIndex() < i2.GetIndex(); }
        };

        class KernelPredicateConjunction
        {
        public:
            KernelPredicateConjunction(const KernelPredicate& lhs, const KernelPredicate& rhs);
            KernelPredicateConjunction(const KernelPredicateConjunction& other);
            KernelPredicateConjunction(KernelPredicateConjunction&& other) = default;
            KernelPredicateConjunction& operator=(const KernelPredicateConjunction& other);
            KernelPredicateConjunction& operator=(KernelPredicateConjunction&& other) = default;

            const std::vector<std::unique_ptr<KernelPredicate>>& GetTerms() const;

            KernelPredicate Simplify() const;
            KernelPredicate Simplify(const LoopIndexSymbolTable& indices, const LoopVisitSchedule& schedule) const;

        private:
            friend class KernelPredicate;
            KernelPredicateConjunction(const std::vector<std::unique_ptr<KernelPredicate>>& terms);

            std::vector<std::unique_ptr<KernelPredicate>> _terms;
        };

        class KernelPredicateDisjunction
        {
        public:
            KernelPredicateDisjunction(const KernelPredicate& lhs, const KernelPredicate& rhs);
            KernelPredicateDisjunction(const KernelPredicateDisjunction& other);
            KernelPredicateDisjunction(KernelPredicateDisjunction&& other) = default;
            KernelPredicateDisjunction& operator=(const KernelPredicateDisjunction& other);
            KernelPredicateDisjunction& operator=(KernelPredicateDisjunction&& other) = default;

            const std::vector<std::unique_ptr<KernelPredicate>>& GetTerms() const;

            KernelPredicate Simplify() const;
            KernelPredicate Simplify(const LoopIndexSymbolTable& indices, const LoopVisitSchedule& schedule) const;

        private:
            friend class KernelPredicate;
            KernelPredicateDisjunction(const std::vector<std::unique_ptr<KernelPredicate>>& terms);

            std::vector<std::unique_ptr<KernelPredicate>> _terms;
        };

        class KernelPredicate
        {
        public:
            KernelPredicate() = default;

            KernelPredicate(const EmptyPredicate& predicate);
            KernelPredicate(const ConstantPredicate& predicate);
            KernelPredicate(const FragmentTypePredicate& predicate);
            KernelPredicate(const PlacementPredicate& predicate);
            KernelPredicate(const IndexDefinedPredicate& predicate);
            KernelPredicate(const KernelPredicateConjunction& predicate);
            KernelPredicate(const KernelPredicateDisjunction& predicate);

            KernelPredicate(const KernelPredicate&) = default;
            KernelPredicate(KernelPredicate&&) = default;
            KernelPredicate& operator=(const KernelPredicate&) = default;
            KernelPredicate& operator=(KernelPredicate&&) = default;

            KernelPredicate Simplify() const;
            KernelPredicate Simplify(const LoopIndexSymbolTable& indices, const LoopVisitSchedule& schedule) const;

            template <typename FunctionType>
            void Visit(FunctionType&& f) const; // FunctionType of the form void(const KernelPredicate&)

            bool IsConstant() const;
            bool GetConstantValue() const;

            bool IsAlwaysTrue() const;
            bool IsAlwaysFalse() const;
            bool IsEmpty() const;

            template <typename T>
            bool Is() const;

            template <typename T>
            const T* As() const;

            friend std::ostream& operator<<(std::ostream& os, const KernelPredicate& predicate);

        private:
            std::variant<EmptyPredicate,
                         ConstantPredicate,
                         FragmentTypePredicate,
                         PlacementPredicate,
                         IndexDefinedPredicate,
                         KernelPredicateConjunction,
                         KernelPredicateDisjunction>
                _expr;
        };

        KernelPredicate First(const Index& index);
        KernelPredicate Last(const Index& index);
        KernelPredicate EndBoundary(const Index& index);
        KernelPredicate All(const Index& index);
        KernelPredicate IsDefined(const Index& index);
        KernelPredicate Before(const Index& index);
        KernelPredicate After(const Index& index);

        KernelPredicate operator&&(const KernelPredicate& lhs, const KernelPredicate& rhs);
        KernelPredicate operator||(const KernelPredicate& lhs, const KernelPredicate& rhs);

        KernelPredicate operator==(const Index& index, int value);
        KernelPredicate operator==(int value, const Index& index);
        // KernelPredicate operator==(const Index& index1, const Index& index2);
        KernelPredicate operator!=(const Index& index, int value);
        KernelPredicate operator!=(int value, const Index& index);
        // KernelPredicate operator!=(const Index& index1, const Index& index2);
        KernelPredicate operator<(const Index& index, int value);
        KernelPredicate operator<(int value, const Index& index);
        // KernelPredicate operator<(const Index& index1, const Index& index2);
        KernelPredicate operator>(const Index& index, int value);
        KernelPredicate operator>(int value, const Index& index);
        // KernelPredicate operator>(const Index& index1, const Index& index2);
        KernelPredicate operator<=(const Index& index, int value);
        KernelPredicate operator<=(int value, const Index& index);
        // KernelPredicate operator<=(const Index& index1, const Index& index2);
        KernelPredicate operator>=(const Index& index, int value);
        KernelPredicate operator>=(int value, const Index& index);
        // KernelPredicate operator>=(const Index& index1, const Index& index2);

        std::string ToString(Fragment condition);

        std::ostream& operator<<(std::ostream& os, const EmptyPredicate& predicate);
        std::ostream& operator<<(std::ostream& os, const ConstantPredicate& predicate);
        std::ostream& operator<<(std::ostream& os, const FragmentTypePredicate& predicate);
        std::ostream& operator<<(std::ostream& os, const PlacementPredicate& predicate);
        std::ostream& operator<<(std::ostream& os, const KernelPredicateConjunction& predicate);
        std::ostream& operator<<(std::ostream& os, const KernelPredicateDisjunction& predicate);
        // std::ostream& operator<<(std::ostream& os, const KernelPredicate& predicate);

    } // namespace loopnests
} // namespace value
} // namespace ell

#pragma region implementation
namespace ell
{
namespace value
{
    namespace loopnests
    {
        template <typename T>
        bool KernelPredicate::Is() const
        {
            return std::holds_alternative<T>(_expr);
        }

        template <typename T>
        const T* KernelPredicate::As() const
        {
            return std::get_if<T>(&_expr);
        }

        template <typename FunctionType>
        void KernelPredicate::Visit(FunctionType&& f) const
        {
            f(*this);
            if (auto conj = As<KernelPredicateConjunction>(); conj != nullptr)
            {
                for (const auto& t : conj->GetTerms())
                {
                    t->Visit(f);
                }
            }
            else if (auto disj = As<KernelPredicateDisjunction>(); disj != nullptr)
            {
                for (const auto& t : disj->GetTerms())
                {
                    t->Visit(f);
                }
            }
        }
    } // namespace loopnests
} // namespace value
} // namespace ell
#pragma endregion implementation
